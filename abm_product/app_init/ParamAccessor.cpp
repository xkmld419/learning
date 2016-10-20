/********************************************
*说明：如何区分立即规则和定期规则
*			配置文件中存在effDate字段的，视为定期规则
*			配置文件中不存在effDate字段的，视为立即规则
*			立即规则：	
*							获取参数值，直接取value值
*							修改参数值，直接修改value值
*			定期规则
*							获取参数值：判断当前获取时间是否大于等于effDate。是，则取newValue值；否则取value值
*							修改参数值：如果effDate字段为空，则直接修改newValue值
*													如果effDate字段不为空，则判断当前修改时间是否大于等于effDate。是，则使用newValue替换value,并更新effDate为当前修改时间
*																																												否，则不修改
*
********************************************/

#include "ParamAccessor.h"

ParamAccessor::~ParamAccessor()
{
}

ParamAccessor* ParamAccessor::GetInstance(char *sFileName)
{

	if(0 == ParamAccessor::m_gpParamAccessor)
	{
		ParamAccessor::m_gpParamAccessor = new ParamAccessor(sFileName);
		if(!m_gpParamAccessor->Init())
		{
			delete m_gpParamAccessor;m_gpParamAccessor=0;
			return 0;
		}

	}
	return m_gpParamAccessor;
}

ParamAccessor::ParamAccessor(char *sFileName)
{
	memset(m_sFileName,0,150);
	memset(m_sTmpFileName,0,150);
	memset(m_sBakFileName,0,150);
	
	strcpy(m_sFileName,sFileName);
	sprintf(m_sTmpFileName,"%s.tmp",m_sFileName);
	sprintf(m_sBakFileName,"%s.bak",m_sFileName);
}

bool ParamAccessor::Init()
{
	return true;
}
char *ParamAccessor::TruncStr( char *sp , char *sDst , int len)
{
	memset(sDst , 0 ,  LINEMAXCHARS);
	strncpy(sDst , 	sp , len) ;
	return sDst;
}

char * ParamAccessor::trim( char *sp )
{
	char sDest[LINEMAXCHARS];
	char *pStr;
	int i = 0;

	if ( sp == NULL )
		return NULL;

	pStr = sp;
	while ( *pStr == ' ' || *pStr == '\t' )  pStr ++;
	strcpy( sDest, pStr );

	i = strlen( sDest ) - 1;
	while((i>=0)&&(sDest[i] == ' ' || sDest[i] == '\t' || sDest[i] == '\r' ) )
	{
		sDest[i] = '\0';
		i -- ;
	}

	strcpy( sp, sDest );

	return ( sp );
}

bool ParamAccessor::GetFieldValue(const char* strParamName, const char* field,char *oValue,int iMaxLen)
{
	char sBuffer[LINEMAXCHARS];
	char sKey[LINEMAXCHARS];
	char *p;
		char *pTmp;
	char sTmp1[LINEMAXCHARS];
	bool bFind=false;

	FILE *fd;
	if(!(fd=fopen(m_sFileName,"rb+")))
	{
        //ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_sFileName);
        if(errno == ENOENT)
            //ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_sFileName);
		return false;
	}
	
	if(!field)
		sprintf(sKey,"%s",strParamName);
	else
		sprintf(sKey,"%s.%s",strParamName,field);

	while ( fgets( sBuffer, LINEMAXCHARS, fd ) )
	{
		trim( sBuffer );
		pTmp = (char *) sBuffer;
		if ( ( sBuffer[0] == '#' ) || ( sBuffer[0] == '\n' ) )
			continue;
		char *tmp=strstr(sBuffer,sKey);
		if(tmp==NULL)
		{
			continue;			
		}
		if((p=(char *) strchr(sBuffer,'=')) 
				&&(strcmp( trim(TruncStr( sBuffer , sTmp1 , p - pTmp))  ,  sKey ) == 0)
			)
		{
			//strcpy(oValue,trim(p+1));
			strncpy(oValue,trim(p+1),iMaxLen);
			
			//把换行符('\n')去掉
			oValue[strlen(oValue)-1]='\0';
			
			bFind=true;
			break;
		}
	}
	fclose(fd);
	return bFind;
}

bool ParamAccessor::GetParamValue(char* strParamName, char* oValue,int iMaxLen,int iMode)
{
	if(iMode==0)
	{
		bool flag=GetFieldValue(strParamName,NULL,oValue,iMaxLen);
		return false;
	}
	
	char effDate[15]={0};
	bool flag;//是否获取到参数值
	
	bool effFlag=GetFieldValue(strParamName,"effDate",effDate,sizeof(effDate));
	if(!effFlag)//立即
	{
		flag=GetFieldValue(strParamName,"value",oValue,iMaxLen);
		return flag;
	}
	
	//定期
		if(strcmp(trim(effDate),"")==0)//newValue为空
			flag=GetFieldValue(strParamName,"value",oValue,iMaxLen);
		else
		{
			Date d;
			Date d1(effDate);
			if(d<d1)
				flag=GetFieldValue(strParamName,"value",oValue,iMaxLen);
			else
				flag=GetFieldValue(strParamName,"newValue",oValue,iMaxLen);
		}
	return flag;
}

bool ParamAccessor::ModifyFieldValue(char *strParamName,char * field,char * oModifyValue)
{
	char sBuffer[LINEMAXCHARS]={0};
	char sKey[LINEMAXCHARS]={0};
	char *p;
		char *pTmp;
	char sTmp1[LINEMAXCHARS];
	
	FILE *fd;
	FILE *tmpFile;
	if(!(fd=fopen(m_sFileName,"rb")))
	{
        //ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_sFileName);
        if(errno == ENOENT)
            //ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_sFileName);
		return false;
	}

	if(!(tmpFile=fopen(m_sTmpFileName,"wb+")))
	{
        //ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_sTmpFileName);
        if(errno == ENOENT)
            //ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_sTmpFileName);
		return false;
	}
	
	if(!field || strcmp(field,"")==0)
		sprintf(sKey,"%s",strParamName);
	else
		sprintf(sKey,"%s.%s",strParamName,field);

	while ( fgets( sBuffer, LINEMAXCHARS, fd ) )
	{
		trim( sBuffer );
		pTmp = (char *) sBuffer;
		if ( ( sBuffer[0] == '#' ) || ( sBuffer[0] == '\n' ) )
		{
			fwrite(sBuffer,strlen(sBuffer),1,tmpFile);
			continue;
		}

		if(!strstr(sBuffer,sKey))
		{
			fwrite(sBuffer,strlen(sBuffer),1,tmpFile);
			continue;			
		}

		if((p=(char *) strchr(sBuffer,'=')) 
				&&(strcmp( trim(TruncStr( sBuffer , sTmp1 , p - pTmp))  ,  sKey ) == 0)
			)
		{
			fwrite(sKey,strlen(sKey),1,tmpFile);
			fwrite("=",strlen("="),1,tmpFile);
			fwrite(oModifyValue,strlen(oModifyValue),1,tmpFile);
			fwrite("\n",strlen("\n"),1,tmpFile);
		}
		else
			fwrite(sBuffer,strlen(sBuffer),1,tmpFile);
	}
	
	fflush(tmpFile);
	fclose(fd);
	fclose(tmpFile);
	remove(m_sFileName);
	rename(m_sTmpFileName,m_sFileName);

	return true;
}

bool ParamAccessor::ModifyParamValue(char* strParamName, char* oModifyValue,char *oModifyDate,int iMode)
{
	if(iMode==0)
	{
		char oValue[150]={0};
		if(GetFieldValue(strParamName,NULL,oValue,sizeof(oValue)))
		{
			ModifyFieldValue(strParamName,NULL,oModifyValue);
			return true;
		}
		return false;
	}
	
	char effDate[15]={0};
	char orignValue[LINEMAXCHARS]={0};
	//Date d;//当前修改的系统时间
	bool flag=GetFieldValue(strParamName,"effDate",effDate,sizeof(effDate));
	GetFieldValue(strParamName,"newValue",orignValue,sizeof(orignValue));
	
	if(!flag)//立即
	{
			ModifyFieldValue(strParamName,"value",oModifyValue);
			Log::log(0,"=========修改%s的value值为%s成功=============",strParamName,oModifyValue);
			return true;
	}

	//定期规则
		if(strcmp(trim(effDate),"")==0)//newValue为空
		{
			ModifyFieldValue(strParamName,"newValue",oModifyValue);
			ModifyFieldValue(strParamName,"effDate",oModifyDate);
		}
		else//newValue不为空
		{
			Date d1(effDate);
			Date d(oModifyDate);
			if(d>d1)//如果修改时间大于等于effDate，则使用newValue替换vlaue，修改newValue和effDate
			{								
				ModifyFieldValue(strParamName,"effDate",oModifyDate);
				ModifyFieldValue(strParamName,"newValue",oModifyValue);
				ModifyFieldValue(strParamName,"value",orignValue);
			}
			//如果修改时间小于effDate，则不处理（拒绝修改）
			else
			{
				Log::log(0,"=========修改时间小于effDate,无需修改=============");
				return false;
			}
		}
	Log::log(0,"=========修改%s的value值为%s成功=============",strParamName,oModifyValue);
	return true;
}

void ParamAccessor::BeginTrans()
{
	//先备份好配置文件，以便回滚的时候，回滚到最初文件
	Log::log(0,"=========批量修改之前，先备份配置文件sys.ini到sys.ini.bak，以便后面的回滚！========");
	FILE *fd;
	FILE *tmpFile;
	if(!(fd=fopen(m_sFileName,"rb")))
	{
        //ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_sFileName);
        if(errno == ENOENT)
            //ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_sFileName);
		return ;
	}

	if(!(tmpFile=fopen(m_sBakFileName,"wb+")))
	{
        //ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_sBakFileName);
        if(errno == ENOENT)
            //ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_sBakFileName);
		return ;
	}
	char buffer[150];
	 while(fgets(buffer,150,fd)!=NULL)
	 	fputs(buffer,tmpFile);

	fclose(fd);
	fclose(tmpFile);
	Log::log(0,"=========备份完成！========");	
}
void ParamAccessor::rollback()
{
	remove(m_sFileName);
	rename(m_sBakFileName,m_sFileName);
	Log::log(0,"=========回滚成功！=========");
}
void ParamAccessor::commit()
{
	remove(m_sBakFileName);
	Log::log(0,"=========提交成功！=========");
}


ParamAccessor* ParamAccessor::m_gpParamAccessor;
