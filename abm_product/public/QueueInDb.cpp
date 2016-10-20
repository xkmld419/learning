/***********************************************************************
 * Module:  QueueInDb.cpp
 * Author:  Yuan Peng
 * Modified: 2009年4月20日 10:22:41
 * Version: 1.0
***********************************************************************/

#include "QueueInDb.h"
#include "Environment.h"
#include "MBC.h"
#include "Log.h"
#include "EventToolKits.h"
#include "WfPublic.h"
#include <string.h>
#include <string>

bool QueueInDbMgr::m_bLoad = false;
FileDef *QueueInDbMgr::m_oFileHead = NULL;
//#include "WfPublic.h" 
void replaceStr(char * sql,char *A,char *B)
{
	char tmp_all[4001]={0},tmp1[4001]={0},tmp2[4001]={0};//sql;
	char newStr[401]={0};
	char *p=NULL;

	strcpy(tmp_all,sql);
	while ((p=strstr(tmp_all,A))!=NULL)
	{
		strncpy(tmp1,tmp_all,p-tmp_all);
		strncpy(tmp2,p+strlen(A),strlen(tmp_all)-strlen(A)-strlen(tmp1));
		strcat(tmp1,B);
		strcat(tmp1,tmp2);
		strcpy(tmp_all,tmp1);
	}
	strcpy(sql,tmp_all);
	
}
/*
bool EventAttrFieldMgr::m_bLoadEvent= false;
HashList<EventAttrField*> *EventAttrFieldMgr::m_vEventFieldIndex = NULL;
EventAttrField *EventAttrFieldMgr::m_vEventFieldList = NULL;
bool QueueInDbMgr::m_bLoad = false;
int EventAttrFieldMgr::m_iAttrCnt = 0;

bool EventAttrFieldMgr::load()
{
	int iCnt = 0,i=0;
	
	if (m_bLoadEvent)
		return true;

	DEFINE_QUERY(qry);
	qry.setSQL("SELECT COUNT(1) FROM B_EVENT_ATTR_FIELD_RELA ");
	qry.open();
	qry.next();
	m_iAttrCnt= qry.field(0).asInteger();
	iCnt = m_iAttrCnt+16;
	qry.close();	

	m_vEventFieldList= new EventAttrField[iCnt];
	m_vEventFieldIndex= new HashList<EventAttrField *>((iCnt>>1) | 1);
	
	qry.setSQL("SELECT EVENT_ATTR_ID,FIELD_NAME,DATA_TYPE FROM B_EVENT_ATTR_FIELD_RELA ORDER BY EVENT_ATTR_ID ");
	qry.open();
	while (qry.next())
	{
		int iAttrID = qry.field(0).asInteger();
		char *sFieldName = qry.field(1).asString();
		char *sDataType = qry.field(2).asString();
		m_vEventFieldList[i].setValues(iAttrID,sFieldName,sDataType);
		m_vEventFieldIndex->add(iAttrID,&(m_vEventFieldList[i]));				
		i++;
		if (i ==1)
		{
			sprintf(m_sInsSql,"insert into ")
		}
	}
	qry.close();

	m_bLoadEvent = true;
	
}

bool EventAttrFieldMgr::getEventAttr(int iAttrID, char * sFieldName, char * sDataType)
{
	load();

	EventAttrField *p;
	if (m_vEventFieldIndex.get(iAttrID,&p))
	{
		strcpy(sFieldName,p->m_sFieldName);
		strcpy(sDataType,p->m_sDataType);
		return true;
	}
	else
	{
		sFieldName = NULL;
		sDataType = NULL;
		return false;
	}
}
*/

FileInDBField::FileInDBField(int iErrFileType, int iAttrID, char * sFieldName, char * sDatatype)
{
	m_iErrFileTypeID = iErrFileType;
	m_iEventAttrID = iAttrID;
	strcpy(m_sFieldName,sFieldName);
	strcpy(m_sDataType,sDatatype);
	m_oNext = NULL;
}

bool FileTypeDef::maskStr(char * i_sName, char * i_sMask)
{
    char    sTempFile[300];
    int     i, j, first[300], second[300];

    strncpy( sTempFile, i_sName, 300 );

    int     iNameLen = strlen( sTempFile );

    int     iWildcardLen = strlen( i_sMask );
    char    sTemp[100];

    strncpy( sTemp, i_sMask, 100 );

    for ( i = 0; i <= iWildcardLen; i++ )
    {
        second[i] = 0;
    }

    second[0] = 1;
    j = 1;
    while ( j <= iWildcardLen && sTemp[j - 1] == '*' )
    {
        second[j++] = 1;

    }

    for ( i = 1; i <= iNameLen; i++ )
    {

        for ( j = 0; j <= iWildcardLen; j++ )
        {
            first[j] = second[j];   /* 将后行内容移至前一行，准备计算新的后行 */
        }

        for ( j = 0; j <= iWildcardLen; j++ )
        {
            second[j] = 0;          /* 后行新值置初值 */
        }

        for ( j = 1; j <= iWildcardLen; j++ )
        {   /* 计算新的后行 */
            if (
                (
                    (sTemp[j - 1] == '*')
                &&  (first[j] == 1 || second[j - 1] == 1)
                )
            ||  (
                    (
                        (sTemp[j - 1] == '*')
                    ||  (sTemp[j - 1] == '?')
                    ||  (sTemp[j - 1] == sTempFile[i - 1])
                    ) && (first[j - 1] == 1)
                ) )
            {
                second[j] = 1;
            }
            else
            {
                second[j] = 0;
            }
        }
    }

    /*根据second  的iFixLen 字段来进行判断，如果是1，那么一定可以匹配，如果是0，就是匹配错误。*/
    if ( second[iWildcardLen] == 0 )
    {
        return false;

    }

    else
    {
        return true;
    }
}


bool FileTypeDef::checkName(char * sLongName)
{
	return maskStr(sLongName,m_sFileRule);
}

bool FileTypeDef::unload()
{
	FileInDBField *p =NULL,*q=NULL;

	p=m_oInDBField;
	q=p;
	while (p)
	{
		q = p->m_oNext;
		delete p;
		p = NULL;
		p = q;
	}

	if (q)
	{
		delete q;
		q = NULL;
	}

	m_oInDBField = NULL;
	return true;
}


bool FileTypeDef::load()
{
	char sSql[501]={0};

	unload();	

	DEFINE_QUERY(qry);	
	
	if (m_iErrFileTypeID != -1)
		sprintf(sSql," SELECT B.ERR_FILE_TYPE_ID,B.EVENT_ATTR_ID,B.TABLE_FIELD,B.DATA_TYPE CUSTOMDATA,C.FIELD_NAME,C.DATA_TYPE DEFAULTDATA "
	 			" FROM B_EVENT_ATTR_FIELD_RELA C, B_FILE_INDB_DEF B "
				" WHERE B.EVENT_ATTR_ID = C.EVENT_ATTR_ID(+) "
				"     AND B.ERR_FILE_TYPE_ID = %d "
				"	AND b.IF_INDB > 0 "
				" ORDER BY B.EVENT_ATTR_ID ",m_iErrFileTypeID);
	else
		sprintf(sSql," SELECT -1 ERR_FILE_TYPE,EVENT_ATTR_ID,FIELD_NAME TABLE_FIELD,DATA_TYPE CUSTOMDATA,FIELD_NAME,DATA_TYPE DEFAULTDATA "
				" FROM B_EVENT_ATTR_FIELD_RELA "
				" WHERE IF_INDB > 0 "
				" ORDER BY EVENT_ATTR_ID ");
			
	qry.setSQL(sSql);
	qry.open();
	FileInDBField *p=NULL;	
	while(qry.next())
	{
		int iErrFileType = qry.field(0).asInteger();
		int iAttrID = qry.field(1).asInteger();
		char *sCustField = qry.field(2).asString();
		char *sCustDataType = qry.field(3).asString();
		char *sDefaultField = qry.field(4).asString();
		char *sDefaultDataType= qry.field(5).asString();
		if (strlen(sCustField) == 0)
			sCustField = sDefaultField;
		if (strlen(sCustDataType) == 0)
			sCustDataType = sDefaultDataType;

		if (strlen(sCustField) == 0 || strlen(sCustDataType) == 0)
		{
	
			Log::log(0,"文件类型%d的字段%s未定义或数据类型默认值未定义.",sCustField);
			continue;
		}

		m_iAttrNum++;
		
		FileInDBField *poFieldTmp = new FileInDBField(iErrFileType,iAttrID,sCustField,sCustDataType);
		if (!p)
		{
			m_oInDBField = poFieldTmp;	
			p= m_oInDBField;
		}
		else
		{
			p->m_oNext = poFieldTmp;
			p = poFieldTmp;
		}
	}

	qry.close();

	formatInsSql();

	return true;
	
}

int FileTypeDef::formatInsSql()
{
	FileInDBField *p = m_oInDBField;
	char sSql[4001]={0},sSql2[4001]={0};

	if (!p)
	{
		throw("文件类型%d未配置入库字段.",m_iErrFileTypeID);
	}				
		
	while (p)
	{
		if (strlen(sSql) == 0)
		{
			sprintf(sSql,"%s,",p->m_sFieldName);
			/*if (p->m_sDataType,"D0A")
				strcpy(sSql2,"%s");
			if (p->m_sDataType,"D0B")
				strcpy(sSql2,"%ld");
			if (p->m_sDataType,"D0C")
				strcpy(sSql2,"to_date(%s,'YYYYMMDDHH24MISS')");*/
			sprintf(sSql2,":%s,",p->m_sFieldName);
		}
		else
		{
			sprintf(sSql,"%s%s,",sSql,p->m_sFieldName);		
			/*strcat(sSql2,",");		
			if (p->m_sDataType,"D0A")
				strcat(sSql2,"%s");
			if (p->m_sDataType,"D0B")
				strcat(sSql2,"%ld");
			if (p->m_sDataType,"D0C")
				strcat(sSql2,"to_date(%s,'YYYYMMDDHH24MISS')");*/
			sprintf(sSql2,"%s:%s,",sSql2,p->m_sFieldName);
			
		}
		p = p->m_oNext;
	}

	if (m_iErrFileTypeID!=-1)
	{
		sprintf(m_sInsSql,"INSERT INTO %s (%s INSERT_DATE,ERR_FILE_TYPE_ID,FILE_TYPE_ID)",m_sINDbTable,sSql);
		sprintf(sSql2,"%s SYSDATE,%d,%d)",sSql2,m_iErrFileTypeID,m_iFileTypeID);
		sprintf(m_sInsValue," values (%s",sSql2);	
	}
	else
	{
		sprintf(m_sInsSql,"INSERT INTO %s (%s INSERT_DATE)",m_sINDbTable,sSql);
		sprintf(sSql2,"%s SYSDATE)",sSql2);
		sprintf(m_sInsValue," values (%s",sSql2);	
	}
	
	return 1;
}

FileTypeDef * QueueInDbMgr::getFileTypeByName(char *m_sFileName)
{
	map<int,FileTypeDef *>::iterator iter;
	for (iter = m_oFileTypeList.begin();iter!=m_oFileTypeList.end();iter++)
	{
		FileTypeDef *poTmpFile = (*iter).second;
		if (poTmpFile->m_iErrFileTypeID == -1)
			continue;
		if (poTmpFile->checkName(m_sAbstractFileName))
			return (*iter).second;
	}

	return m_oFileTypeList[-1];
}

FileTypeDef *QueueInDbMgr::getFileTypeByID(int iFileTypeID)
{
	if (m_oFileTypeList[iFileTypeID])
		return m_oFileTypeList[iFileTypeID];
}

int FileDef::commitSingleEvent()
{
	char sSql[8001]={0},sSql2[8001]={0};
	int i = 0;
	if (m_vEventList.size() == 0)
		return 1;
	
	FileInDBField *p =m_oFileType->m_oInDBField;
	if (!p)
	{
		throw("文件类型%d未配置入库字段.",m_oFileType->m_iErrFileTypeID);
	}	

	try
	{
		for(i =0;i<m_vEventList.size();i++)
		{
			EventToolKits pToolKit;	
			DEFINE_QUERY(qry);
			sprintf(sSql,"%s %s",m_oFileType->m_sInsSql,m_oFileType->m_sInsValue);
			qry.setSQL(sSql);
			strcpy(sSql2,sSql);
			FileInDBField *p =m_oFileType->m_oInDBField;
			while(p)
			{
				void *pAttr =0;
				int iDataTypeID = 0;
				string s;
				char sParamName[32]={0},sParamValue[255]={0};

				sprintf(sParamName,":%s",p->m_sFieldName);				
				
				s = pToolKit.get_param(&(m_vEventList[i]), p->m_iEventAttrID, iDataTypeID);
				sprintf(sParamValue,"%s",s.c_str());
				if (iDataTypeID == 1)
				{
					qry.setParameter(p->m_sFieldName,atoi(s.c_str()));
					replaceStr(sSql2,sParamName,sParamValue);					
				}
				else if (iDataTypeID == 2)
				{
					qry.setParameter(p->m_sFieldName,atol(s.c_str()));					
					replaceStr(sSql2,sParamName,sParamValue);					
				}
				else if (iDataTypeID == 3)
				{
					qry.setParameter(p->m_sFieldName,s.c_str());
					sprintf(sParamValue,"\'%s\'",s.c_str());
					replaceStr(sSql2,sParamName,sParamValue);					
				}	

				p = p->m_oNext;
			}
			Log::log(0,"%s\n一条结束\n",sSql2);
			qry.execute();
			qry.commit();
			qry.close();
		}
		m_vEventList.clear();

		return 1;
	}
	
	catch (TOCIException &e)
	{
		Log::log(0,"单条第[%d]条入库发生sql错误，错误代码:%d,错误描述:%s,\n,错误语句:%s",
				i,e.getErrCode(),e.getErrMsg(),sSql2);
		throw("单条入库失败!");
	}

	catch (Exception &e)
	{
		Log::log(0,"单条第[%d]条入库出错，错误信息:%s",i,e.descript());
		throw("单条入库失败!");
	}
	catch (...)
	{
		Log::log(0,"单条第[%d]条入库发生未知错误.",i);
		throw("单条入库失败!");
	}

}

int FileDef::commitEvent()
{
	char sSql[8001]={0};
	if (m_vEventList.size() == 0)
		return 1;

	//commitSingleEvent();
	//return 1;
	
	FileInDBField *p =m_oFileType->m_oInDBField;
	if (!p)
	{
		throw("文件类型%d未配置入库字段.",m_oFileType->m_iErrFileTypeID);
	}	

	try
	{
		EventToolKits pToolKit;	
		DEFINE_QUERY(qry);
		sprintf(sSql,"%s %s",m_oFileType->m_sInsSql,m_oFileType->m_sInsValue);
		qry.setSQL(sSql);

		while (p)
		{
			void *pAttr =0;
			int iDataTypeID = 0;
			int iValueSize =0;
			string s;
			
			s = pToolKit.get_param2(&(m_vEventList[0]), p->m_iEventAttrID, iDataTypeID, &pAttr,iValueSize);
			if (iDataTypeID == 1)
			{
				qry.setParamArray(p->m_sFieldName,(int *)pAttr,sizeof(m_vEventList[0]));
				//Log::log(0,"%s:%d",p->m_sFieldName,*(int *)pAttr);
			}
			else if (iDataTypeID == 2)
			{
				qry.setParamArray(p->m_sFieldName,(long *)pAttr,sizeof(m_vEventList[0]));
				//Log::log(0,"%s:%ld",p->m_sFieldName,*(long *)pAttr);
			}
			else if (iDataTypeID == 3)
			{
				//char sParamValue[]
/*				cout<<"size of s :"<<sizeof(s.c_str())<<endl;
				cout<<"size of pAttr:"<<sizeof(p)<<endl;
				cout<<"size of (char *)pAttr"<<sizeof((char *)p)<<endl;
				cout<<"size of m_sOrgCallingNBR"<<sizeof(m_vEventList[0].m_sOrgCallingNBR)<<endl;
				cout<<"return valuesize :"<<iValueSize<<endl;*/
				qry.setParamArray(p->m_sFieldName,(char **)pAttr,sizeof(m_vEventList[0]),iValueSize);
				//Log::log(0,"%s:%s",p->m_sFieldName,(char *)pAttr);
			}			

			p = p->m_oNext;
			
		}
		qry.execute(m_vEventList.size());
		qry.commit();
		qry.close();

		m_vEventList.clear();

		return 1;
	}
	
	catch (TOCIException &e)
	{
		//FILE *fp;
		char sFileName[65]={0};

		Log::log(0,"入库发生sql错误，错误代码:%d,错误描述:%s,\n,错误语句:%s",
				e.getErrCode(),e.getErrMsg(),e.getErrSrc());
		commitSingleEvent();
/*
		if (strlen(m_sIndbErrPath) > 0)
		{
			char *p=strrchr(m_sIndbErrPath,'/');
			if (!p)
				strcat(m_sIndbErrPath,"/");
			sprintf(sFileName,"%s%s",m_sIndbErrPath,m_sIndbErrFileName);
		}
		else
			strcpy(sFileName,m_sIndbErrFileName);
		fp = fopen(sFileName,"w+");
		if (!fp)
			throw("创建错误记录文件%s失败!",sFileName);

		vector<StdEvent>::iterator iter;
		for (iter = m_vEventList.begin();iter!=m_vEventList.end();iter++)
		{
			
		}
*/
		
	}
	catch (Exception &e)
	{
		Log::log(0,"入库出错，错误信息:%s",e.descript());		
	}
	catch (...)
	{
		Log::log(0,"入库发生未知错误.");		
	}

	return -1;
}

void FileDef::getInstSql(StdEvent & poEventSection, string & sSql)
{
	FileInDBField *p =m_oFileType->m_oInDBField;

	while(p)
	{
		EventToolKits pToolKit;
		int iDataType;
		string s;
		s = pToolKit.get_param2(&poEventSection, p->m_iEventAttrID,iDataType);
		if (iDataType ==1 || iDataType == 2) //int || Long
			sSql = sSql+s.c_str()+",";
		if (iDataType == 3)
			sSql = sSql +"'"+s.c_str()+"',";
		if (iDataType == 0)
			sSql = sSql +"NULL,";
		
		p = p->m_oNext;
	}

	if (m_oFileType->m_iErrFileTypeID !=-1)
	{
		char sTmp[50]={0};
		sprintf(sTmp,"%d,%d)",m_oFileType->m_iErrFileTypeID,m_oFileType->m_iFileTypeID);
		sSql ="values ("+ sSql+" sysdate,"+sTmp;
	}
	else
	{
		sSql = "values ("+sSql +" sysdate )";
	}

	sSql = m_oFileType->m_sInsSql + sSql;
}

int FileDef::commitEvent_2()
{
	int i = 0;

	DEFINE_QUERY(qry);
	try
	{
		for (i=0;i<m_vInstSqlList.size();i++)
		{
			qry.setSQL(m_vInstSqlList[i].c_str());
			qry.execute();
		}

		qry.commit();
	}

	catch (TOCIException &e)
	{
		//FILE *fp;
		char sFileName[65]={0};

		Log::log(0,"入库发生sql错误，错误代码:%d,错误描述:%s,\n,错误语句:%s",
				e.getErrCode(),e.getErrMsg(),m_vInstSqlList[i].c_str());
	}
  return true;
}

int FileDef::saveEvent(StdEvent & poEventSection)
{

	if (m_oFileType->m_iAttrNum <150)
	{
		m_vEventList.push_back(poEventSection);	

		if (m_vEventList.size()==1000)
			commitEvent();	
	}
	else
	{
		/*由于liboci的限制只允许有150个参数，故改为拼sql方式*/
		string sInstSql;
		getInstSql(poEventSection,sInstSql);
		DEFINE_QUERY(qry);
		try
		{			
			qry.setSQL(sInstSql.c_str());
			qry.execute();
			qry.commit();
			qry.close();
		}
		catch (TOCIException &e)
		{
			char sFileName[65]={0};

			Log::log(0,"入库发生sql错误，错误代码:%d,错误描述:%s,\n,错误语句:%s",
					e.getErrCode(),e.getErrMsg(),sInstSql.c_str());
		}
		
		/*m_vInstSqlList.push_back(sInstSql);
		if (m_vInstSqlList.size() == 500)
			commitEvent_2();	*/	
	}
  return true;
}

void QueueInDbMgr::load()
{
	char sSql[501]={0};	
	
	if (m_bLoad)
		return;

	DEFINE_QUERY(qry);
	sprintf(sSql,"SELECT err_file_type_id,file_rule,file_type_id,in_table_name,group_id,nvl(INDB_ERR_PATH,''),INDB_ERR_FILENAME "
			" FROM b_file_define "
			" ORDER BY err_file_type_id desc ");
	qry.setSQL(sSql);
	qry.open();
	while (qry.next())
	{
		char sFileRule[33]={0},sTabName[31]={0};
		int iErrFileType = qry.field(0).asInteger();
		strcpy(sFileRule,qry.field(1).asString());
		int iFileType = qry.field(2).asInteger();
		strcpy(sTabName,qry.field(3).asString());
		int iGrpID = qry.field(4).asInteger();
		char *sIndbErrPath = qry.field(5).asString();
		char *sIndbErrFile = qry.field(6).asString();
		FileTypeDef *oTmpFileType = new FileTypeDef(iErrFileType,sFileRule,iFileType,sTabName,iGrpID,sIndbErrPath,sIndbErrFile);
		oTmpFileType->load();
		m_oFileTypeList[iErrFileType] = oTmpFileType;
	}
	qry.close();

	m_bLoad = true;
	
}

void QueueInDbMgr::unload()
{
	if (m_bLoad == false)
		return;

	map<int,FileTypeDef *>::iterator iter;
	for (iter=m_oFileTypeList.begin();iter!=m_oFileTypeList.end();iter++)
	{
		delete (*iter).second;
	}
	m_oFileTypeList.clear();	

	FileDef *p=NULL,*pre=NULL;
	p=pre=m_oFileHead;
	while (p)
	{
		pre=p;
		p=p->m_oNext;
		delete pre;
		pre =NULL;
	}
}

int QueueInDbMgr::saveEvent(StdEvent & poEventSection,int iFileID)
{
	//这里将属性poEventSection.m_oEventExt.m_iHotBillingTag作为判断是否入库的标记；
	//此属性很少用到，默认值0为入库，如果不入库则置为>0;
	static char sOldName[64]={0};
	
	load();
	
	FileTypeDef *poFileType;	
	
	FileDef *poFile = searchFileDefList(iFileID);
	
	if (!poFile)
	{
		getAbstractFileName(poEventSection.m_oEventExt.m_sFileName);
/*		if (strcmp(sOldName,poEventSection.m_oEventExt.m_sFileName) !=0)
		{
			strcpy(sOldName,poEventSection.m_oEventExt.m_sFileName);
			poFileType= getFileTypeByName(m_sAbstractFileName);
		}*/
		poFileType= getFileTypeByName(m_sAbstractFileName);
		poFile = new FileDef(iFileID,poFileType);
		addFileDefList(poFile);
	}

	poFile->saveEvent(poEventSection);
	
	//poFileType->saveToDB(poEventSection);	

	return 1;
}

int QueueInDbMgr::commitEvent(int iFileID )
{
//	FileTypeDef *poFileType;	
	FileDef *poFile = NULL;

	if (iFileID == -1)   	//提交所有未入库的文件
	{
		for (poFile=m_oFileHead;poFile;poFile=poFile->m_oNext)
		{
			poFile->commitEvent();
		}
		return 1;
	}
	
	poFile = searchFileDefList(iFileID);
	if (poFile)
		poFile->commitEvent();
	//poFileType= getFileTypeByName(m_sAbstractFileName);
	//poFileType->commitDB();
}

FileDef *QueueInDbMgr::searchFileDefList(int iFileID)
{
	FileDef *pt =NULL;

	for (pt=m_oFileHead;pt; pt=pt->m_oNext)
	{
		if (pt->m_iFileID == iFileID)
			break;
	}

	return pt;
}

void QueueInDbMgr::addFileDefList(FileDef * poFileDef)
{
	FileDef *pt =NULL;

	if (!m_oFileHead)
	{
		m_oFileHead = poFileDef;
		return;
	}

	for (pt=m_oFileHead;pt->m_oNext;pt=pt->m_oNext)
	{		
	}

	pt->m_oNext = poFileDef;
}

