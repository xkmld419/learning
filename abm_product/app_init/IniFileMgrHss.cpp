#include "SysParamDefine.h"
#include "IniFileMgrHss.h"
#include "WriteIni.h"
#include "CSemaphore.h"
#include "hash_list.h"
#include "GlobalTools.h"
#ifdef	DEF_LINUX
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "BaseRuleMgr.h"
int hashex (const char *key)
{
         int     hval;
        const char      *ptr;
        char    c;
        int     i;

        hval = 0;
        for (ptr = key, i = 1; c=*ptr++; i++)
                hval += c*i; /* ascii char times its 1-based index */

        return (hval%HASH_NUM);
}

void CIniFileMgr::ShiftRight(char sParamValue[MAX_STACK_LEN][MAX_KEY_VALUE_LEN],int iPos)
{
	if(iPos>=MAX_STACK_LEN)
		memset(sParamValue,0,sizeof(sParamValue));
	else
	{
		memcpy(sParamValue[iPos],sParamValue[0],sizeof(char)*MAX_KEY_VALUE_LEN*(MAX_STACK_LEN-iPos));
		memset(sParamValue,0,sizeof(char)*MAX_KEY_VALUE_LEN*iPos);
	}	
}

CIniFileMgr::CIniFileMgr()
{
	m_pIniKey=0;
	m_iTotalNum=0;
	m_iUseNum=1;
	m_iMemLockKey = DEFAULT_LOCK_KEY;
	m_poIniSection=0;
	memset(m_sTmp,0,sizeof(m_sTmp));
	m_prwLock = 0;
	m_pRuleMgr=0;// new BaseRuleMgr();	
}

bool CIniFileMgr::ClearShm()
{
	if(0!=m_poIniSection)
	{
		m_poIniSection->remove();
		delete m_poIniSection;
		m_poIniSection=0;
	}
	return true;
}
void CIniFileMgr::Clear()
{

	if(0!= m_poIniSection)
	{
		return;
		//delete m_poIniSection;
		m_poIniSection=0;
		return ;
	}else
	{
		if(m_pIniKey)
			delete [] m_pIniKey;
		m_pIniKey=0;
	}
}
CIniFileMgr::~CIniFileMgr(void)
{
	Clear();
}
bool CIniFileMgr::GetIniKey(const char* sSectionName,
							const char* sParamName,
							stIniCompKey& oIniCompKey)
{
	CIniSection oSection;
	if(GetIniKey(sSectionName,sParamName,oSection)){
		strcpy(oIniCompKey.sSectionName,oSection.sSectionName);
		strcpy(oIniCompKey.sKeyName,oSection.sKeyName);
		strcpy(oIniCompKey.sEffDate,oSection.sEffDate);
		memcpy(oIniCompKey.sKeyValue,oSection.sKeyValue,sizeof(oIniCompKey.sKeyValue));
		return true;
	}else
		return false;
}

bool CIniFileMgr::GetIniKey(const char* sSectionName,
							const char* sParamName,
							CIniSection& oIniCompKey)
{
	if((0== sSectionName)||(0==sParamName))
		return false;
	if(0!=m_pIniKey)
	{	
		READLOCK(DEFAULT_LOCK_KEY)
		for(int i=1;i<m_iUseNum;i++)
		{
			if((strcasecmp(m_pIniKey[i].sSectionName,sSectionName)==0)&&
				(strcasecmp(m_pIniKey[i].sKeyName,sParamName)==0))
			{
				strcpy(oIniCompKey.sKeyName,m_pIniKey[i].sKeyName);
				memcpy(oIniCompKey.sKeyValue,m_pIniKey[i].sKeyValue,sizeof(oIniCompKey.sKeyValue));
				strcpy(oIniCompKey.sSectionName,m_pIniKey[i].sSectionName);
				strcpy(oIniCompKey.sEffDate,m_pIniKey[i].sEffDate);
				return true;
			}
		}
	}
	return false;
}
bool CIniFileMgr::SaveToFile(const char *cpsFileName)
{
	if(0==cpsFileName)
		return false;
	WRITELOCK(DEFAULT_LOCK_KEY)
	CIniFileMgr* pFileMgr = new CIniFileMgr();
	pFileMgr->LoadIniFromFile(cpsFileName);

	vector<stIniCompKey> vtIniCompKey;
	if(pFileMgr->Compare(this,vtIniCompKey,false))
	{
		for(vector<stIniCompKey>::iterator it = vtIniCompKey.begin();
			it!= vtIniCompKey.end();it++)
		{
			SaveKey(cpsFileName,*it);
		}
	}
	delete pFileMgr;pFileMgr=0;
	return true;
}
bool CIniFileMgr::ReplaceKey(stIniCompKey &oIniKey)
{
        //加共享内存锁
        if(m_pIniKey==0)
                return false;
		WRITELOCK(DEFAULT_LOCK_KEY)
		for(int i=1;i<=m_iUseNum;i++)
        {
            if((strcmp(oIniKey.sSectionName,m_pIniKey[i].sSectionName)==0)
                        &&(strcmp(oIniKey.sKeyName,m_pIniKey[i].sKeyName)==0))
              {
					memcpy(m_pIniKey[i].sKeyValue,oIniKey.sKeyValue,sizeof(oIniKey.sKeyValue));
					return true;
                }
         }
        Log::log(0,"没有找到需要修改的节点: SectionName:%s,KeyName:%s",oIniKey.sSectionName,
                oIniKey.sKeyName);
		return false;
}
bool CIniFileMgr::getAllSection(SysParamHis **ppSysParamHis,int& iCnt)
{
		return getSection(0,ppSysParamHis,iCnt);
}

bool CIniFileMgr::getSection(const char* pSection,
							 SysParamHis** ppSysParamHis,
							 int &iCnt)
{
	static SysParamHis *pParamHis = new SysParamHis[m_iUseNum];

		iCnt=0;
		for(int i=1;i<=this->m_iUseNum;i++){
			if((pSection==0)||(pSection[0]==0)||
				(strcmp(m_pIniKey[i].sSectionName,pSection)==0)){
					strcpy(pParamHis[iCnt].sSectionName,
							m_pIniKey[i].sSectionName);
					strcpy(pParamHis[iCnt].sParamName,
							m_pIniKey[i].sKeyName);
					if(m_pIniKey[i].sEffDate[0]!=0){
						strcpy(pParamHis[iCnt].sParamValue,
						m_pIniKey[i].sKeyValue[1]);
						strcpy(pParamHis[iCnt].sParamOrValue,
							m_pIniKey[i].sKeyValue[0]);
						strcpy(pParamHis[iCnt].sDate,
							m_pIniKey[i].sEffDate);
					}else{
						strcpy(pParamHis[iCnt].sParamValue,
							m_pIniKey[i].sKeyValue[0]);
						strcpy(pParamHis[iCnt].sParamOrValue,
							m_pIniKey[i].sKeyValue[2]);
					}
					iCnt++;
				}
			}
		*ppSysParamHis = new SysParamHis[iCnt];
		memcpy(*ppSysParamHis,pParamHis,sizeof(SysParamHis)*iCnt);
		return true;
}

bool CIniFileMgr::getSectionParam(const char* pSection,
								  const char* pParamName,
								  SysParamHis& oParamHis)
{
	for(int i=1;i<=this->m_iUseNum;i++){
		if((strcmp(m_pIniKey[i].sSectionName,pSection)==0)&&
			(strcmp(m_pIniKey[i].sKeyName,pParamName)==0)){
			strcpy(oParamHis.sSectionName,
				m_pIniKey[i].sSectionName);
			strcpy(oParamHis.sParamName,
				m_pIniKey[i].sKeyName);
		if(m_pIniKey[i].sEffDate[0]!=0){
			strcpy(oParamHis.sParamValue,
				m_pIniKey[i].sKeyValue[1]);
			strcpy(oParamHis.sParamOrValue,
				m_pIniKey[i].sKeyValue[0]);
			strcpy(oParamHis.sDate,
				m_pIniKey[i].sEffDate);
			}else{
			strcpy(oParamHis.sParamValue,
				m_pIniKey[i].sKeyValue[0]);
			strcpy(oParamHis.sParamOrValue,
				m_pIniKey[i].sKeyValue[2]);
			}
			break;
		}
	}
	return true;
}
bool CIniFileMgr::AddKey(CIniSection& oSection,OP_FLAG flag)
{
	stIniCompKey oIniKey;
	strcpy(oIniKey.sSectionName,oSection.sSectionName);
	strcpy(oIniKey.sKeyName,oSection.sKeyName);
	memcpy(&(oIniKey.sKeyValue[0][0]),&(oSection.sKeyValue[0][0]),sizeof(oIniKey.sKeyValue));
	strcpy(oIniKey.sEffDate,oSection.sEffDate);
	oIniKey.flag =flag;
	return AddKey(oIniKey);
}
bool CIniFileMgr::AddKey(stIniCompKey &oIniKey)
{
	//加共享内存锁
	if(m_pIniKey==0)
		return false;
	WRITELOCK(DEFAULT_LOCK_KEY)	
	if(oIniKey.flag ==OP_NEW)
	{
		//需要加上循环判断是否该键值是否存在
		if(m_iUseNum>=m_iTotalNum)
		{
			if(m_poIniSection!=0){
				//在共享内存中不能扩展
				Log::log(0,"共享内存已经满了");
				return false;
			}else{
				m_iTotalNum= ExtendSectionMem(&m_pIniKey,m_iTotalNum);
			}
		}
		strcpy(m_pIniKey[++m_iUseNum].sKeyName,oIniKey.sKeyName);
		strcpy(m_pIniKey[m_iUseNum].sSectionName,oIniKey.sSectionName);
		ShiftRight(m_pIniKey[m_iUseNum].sKeyValue);
		strcpy(m_pIniKey[m_iUseNum].sKeyValue[0],oIniKey.sKeyValue[0]);
		if(0!= m_poIniSection)
			m_poIniSection->malloc();
		return true;
	}
	if(oIniKey.flag == OP_FIX)
	{
		for(int i=1;i<=m_iUseNum;i++)
		{
			if((strcmp(oIniKey.sSectionName,m_pIniKey[i].sSectionName)==0)
				&&(strcmp(oIniKey.sKeyName,m_pIniKey[i].sKeyName)==0))
			{
				if(oIniKey.sKeyValue[0][0]==0)
				{
					strcpy(oIniKey.sKeyValue[0],m_pIniKey[i].sKeyValue[0]);	
				}	
				ShiftRight(m_pIniKey[i].sKeyValue,2);
				strcpy(m_pIniKey[i].sKeyValue[0],oIniKey.sKeyValue[0]);
				for(int j=1;j<MAX_STACK_LEN-1;j++)
					strcpy(m_pIniKey[i].sKeyValue[j],oIniKey.sKeyValue[j]);
				if(oIniKey.sEffDate[0]!=0)
					strcpy(m_pIniKey[i].sEffDate,oIniKey.sEffDate);
				strcpy(oIniKey.sKeyValue[2],m_pIniKey[i].sKeyValue[2]);	
				return true;
			}
		}
		Log::log(0,"没有找到需要修改的节点: SectionName:%s,KeyName:%s",oIniKey.sSectionName,
			oIniKey.sKeyName);
		return false;
	}
	if(oIniKey.flag ==OP_DEL)
	{
		//删除操作没有同步到共享内存
		for(int i=1;i<=m_iUseNum;i++)
		{
			if((strcmp(oIniKey.sSectionName,m_pIniKey[i].sSectionName)==0)
				&&(strcmp(oIniKey.sKeyName,m_pIniKey[i].sKeyName)==0))
			{
				if(m_iUseNum==i)
				{
					memset((void*)&m_pIniKey[i],0,sizeof(CIniSection));
				}else
				{
					memcpy((void*)&m_pIniKey[i],(void*)&m_pIniKey[i+1],
							sizeof(CIniSection)*(m_iUseNum-i));
				}
				m_iUseNum--;
				return true;
			}
		}
		Log::log(0,"没有找到需要修改的节点: SectionName:%s,KeyName:%s",oIniKey.sSectionName,
			oIniKey.sKeyName);
		return false;
	}
	Log::log(0,"无法识别");
	return false;
}
void CIniFileMgr::ChangeSHMDataCount(unsigned int iCnt)
{
	if(m_poIniSection)
	{
		m_poIniSection->setCount(iCnt+1);
	}
}
void CIniFileMgr::FlashIt(void)
{
	//刷新下共享内存
	if(m_poIniSection)
	{
		m_iUseNum = m_poIniSection->getCount();
	}
}
bool CIniFileMgr::SaveKey(const char* cpsFileName,stIniCompKey & oIniKey)
{
	if(0== cpsFileName)
		return false;

	CSemaphore m_Lock;
	int iKey = hashex(cpsFileName);
	char sKey[16]={0};
	sprintf(sKey,"%d",iKey);
	string strCpComdArgv =cpsFileName;
	string strCpComdArgv1=strCpComdArgv;
	int iPid=0;

	if (-1 ==access(strCpComdArgv1.c_str(), R_OK| W_OK| F_OK ) )
	{
		Log::log(0,"文件不存在或者文件不可读写, 请检查文件 :%s",strCpComdArgv1.c_str());
		return false;
	}
	WriteIni oWriteIni;
	if(oIniKey.flag== OP_DEL)
	{
		oWriteIni.DelIniByIdent(strCpComdArgv1.c_str(),oIniKey.sSectionName,oIniKey.sKeyName);
	}else
	{
		//判断是否需要展开
		//只要eff_date字段有值就表示需要展开
		//param_name 后面加.old_value和new_value,eff_date
		if(strlen(oIniKey.sEffDate)>0)
		{
			string strParamTmp;
			strParamTmp = oIniKey.sKeyName;
			strParamTmp.append(".");
			strParamTmp =strParamTmp+OLD_VALUE;
			oWriteIni.WriteIniString(strCpComdArgv1.c_str(),oIniKey.sSectionName,strParamTmp.c_str(),oIniKey.sKeyValue[0]);
			strParamTmp = oIniKey.sKeyName;
			strParamTmp.append(".");
			strParamTmp = strParamTmp+NEW_VALUE;
			oWriteIni.WriteIniString(strCpComdArgv1.c_str(),oIniKey.sSectionName,strParamTmp.c_str(),oIniKey.sKeyValue[1]);
			strParamTmp = oIniKey.sKeyName;
			strParamTmp.append(".");
			strParamTmp = strParamTmp+EFF_DATE_NAME;
			oWriteIni.WriteIniString(strCpComdArgv1.c_str(),oIniKey.sSectionName,strParamTmp.c_str(),oIniKey.sEffDate);
		}else
			oWriteIni.WriteIniString(strCpComdArgv1.c_str(),oIniKey.sSectionName,oIniKey.sKeyName,oIniKey.sKeyValue[0]);
	}

	return true;

}

bool CIniFileMgr::Init()
{
	return true;
}


bool CIniFileMgr::AnalysisLine(string& strLine,string& strSection,string& strKey,string& strValue)
{
	strLine.erase(0,strLine.find_first_not_of(' '));
	strLine.erase(strLine.find_last_not_of(' ')+1);
	if(strLine.empty())
		return true;

	strKey.clear();
	strValue.clear();
	int iState=0,iStatePrev=0;//-1,错误状态,0,初始太,1, [  2,] ,3#,4=,5其他
	int iPosBegin=-1,iPosEnd=-1;
	for(unsigned int i=0;i<strLine.length();i++)
	{
		switch(strLine[i])
		{
		case ' ':
			continue;
		case '[':
			{
				if(iState==5)
					break;
				iPosBegin=i;
				iState=1;
			}break;
		case ']':
			{
				if(iState==5)
					break;
				if(iState!= 1){
					iState=-1;
					return false;
				}
				else
					iState=2;
				iPosEnd =i;
			}break;
		case '#':
			{
				if(iState==5&&(strKey.empty()))
					return false;
				if(iState==1){
					iState=-1;
					return false;
				}
				else
					iState=3;
				
				iPosEnd=i;
			}break;
		case '=':
			{
				if(iState==1)
					break;
				else
				{
					if(iState==3)
						break;
					iState=4;
				}
				iPosEnd=i;
			}break;
		default:
			{
				if(iState==5)
					break;
				else
				{
					if(iState==2)
					{
						iState=-1;
						return false;
					}
					if((iState==1)||(iState==3))
						break;
					iPosBegin=i;
					iState=5;
				}
			}
			break;
		}
		if(3==iState)
			break;
		if(-1==iState)
		{
			Log::log(0,"无法解析行:%s",strLine.c_str());
			return false;
		}
		if(2==iState)
		{
			strSection.assign(strLine,iPosBegin+1,iPosEnd-iPosBegin-1);
			m_strSection= strSection;
		}else
		{
			if(4==iState)
			{
				strKey.assign(strLine,iPosBegin,iPosEnd-iPosBegin);
				iPosBegin = iPosEnd+1;
			}else
			{
				if((3==iState)&&(!strKey.empty()))
				{
					strValue.assign(strLine,iPosBegin,iPosEnd-iPosBegin);
				}
			}
		}
	}
	if(((3==iState)&&(!strKey.empty()))||((5==iState)&&(!strKey.empty())))
	{
		if(iPosEnd>=iPosBegin)
			strValue.assign(strLine,iPosBegin,iPosEnd-iPosBegin);
		else
			strValue.assign(strLine,iPosBegin,strLine.length());
	}else
	{
		if((-1==iState)||(iPosEnd==-1))
			return false;
	}
	return true;
}

bool CIniFileMgr::GetIniKeyByString(std::string &strLine, 
									stIniCompKey &oIniCompKey)
{
	CIniSection oSection;
	if(GetIniKeyByString(strLine,oSection)){
		strcpy(oIniCompKey.sSectionName,oSection.sSectionName);
		strcpy(oIniCompKey.sKeyName,oSection.sKeyName);
		strcpy(oIniCompKey.sEffDate,oSection.sEffDate);
		memcpy(oIniCompKey.sKeyValue,oSection.sKeyValue,sizeof(oIniCompKey.sKeyValue));
		return true;
	}else
		return false;
}
bool CIniFileMgr::GetIniKeyByString(std::string &strLine, 
									CIniSection&oIniCompKey)
{
	string strSection,strParam,strValue,strContent;
	strContent = strLine;
	string stra,strb;
	vector<string> arrayStringValue;
	vector<string> arrayStringParam;
	vector<string> arrayParamPrefix;
	vector<string> arrayParamSuffix;
	vector<string> arraySection;
	int i=0;
	while(splitParamInfo(strContent,strSection,strParam,strValue))
	{
		i++;
		arraySection.push_back(strSection);
		arrayStringParam.push_back(strParam);
		arrayStringValue.push_back(strValue);
		int iPosBegin = strParam.find_last_of('.');
		if(iPosBegin>0){
			stra.clear();strb.clear();
			stra.append(strParam,0,iPosBegin);
			strb.append(strParam,iPosBegin+1,strParam.length());
			arrayParamPrefix.push_back(stra);
			arrayParamSuffix.push_back(strb);
		}
		strSection.clear();strParam.clear();strValue.clear();
	}
	if(i==0)
		return false;
	vector<string>::iterator it = arraySection.begin();
	int iLen = (MAX_SECTION_LEN-1)>it->length()?it->length():MAX_SECTION_LEN-1;	
	strncpy(oIniCompKey.sSectionName,it->c_str(),
		(MAX_SECTION_LEN-1)>it->length()?it->length():MAX_SECTION_LEN-1);
	oIniCompKey.sSectionName[iLen]=0;
	it = arrayStringParam.begin();
	strncpy(oIniCompKey.sKeyName,it->c_str(),
		(MAX_KEY_NAME_LEN-1)>it->length()?it->length():MAX_KEY_NAME_LEN-1);
		iLen = (MAX_KEY_NAME_LEN-1)>it->length()?it->length():MAX_KEY_NAME_LEN-1;	
	oIniCompKey.sKeyName[iLen]=0;
	it=arrayStringValue.begin();
	strncpy(oIniCompKey.sKeyValue[0],it->c_str(),
			(MAX_KEY_VALUE_LEN-1)>it->length()?it->length():MAX_KEY_VALUE_LEN-1);
	iLen = (MAX_KEY_VALUE_LEN-1)>it->length()?it->length():MAX_KEY_VALUE_LEN-1;	
	oIniCompKey.sKeyValue[0][iLen]=0;	
	if((arrayParamPrefix.size()!=arrayParamSuffix.size())
		||(arrayParamPrefix.size()!=arrayStringValue.size())
		||arrayParamPrefix.size()<2)
	{
		return true;
	}
	oIniCompKey.sKeyValue[0][0]=0;
	i =0;
	for(vector<string>::iterator it =arrayParamSuffix.begin();
		it!= arrayParamSuffix.end();it++,i++)
	{
		strValue=arrayStringValue[i];	
		if(strcasecmp(it->c_str(),NEW_VALUE)==0)
			strncpy(oIniCompKey.sKeyValue[1],strValue.c_str(),
						MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1);
		else if (strcasecmp(it->c_str(),OLD_VALUE)==0)
			strncpy(oIniCompKey.sKeyValue[0],strValue.c_str(),
						MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1);
		else if (strcasecmp(it->c_str(),EFF_DATE_NAME)==0)
			strncpy(oIniCompKey.sEffDate,strValue.c_str(),
						MAX_DATE_LEN>strValue.length()?strValue.length():MAX_DATE_LEN-1);
	}

	char* stringFind = strrchr(oIniCompKey.sKeyName,'.');
                if(0==stringFind)
                        return true;
                int iPos = (int)(stringFind -oIniCompKey.sKeyName );	
		oIniCompKey.sKeyName[iPos]=0;
	return true;
}

bool CIniFileMgr::Compare(CIniFileMgr* pIniFileMgr,vector<stIniCompKey>& vtCompKey,bool bLock)
{
	CRWLock* pLock=0;
	if(bLock)
		pLock = new CRWLock(DEFAULT_LOCK_KEY);
	int iFlags=0; //1,新增，2，修改,3,删除
	if(0 != pIniFileMgr)
	{
		unsigned int i=0,j=0;
		for( j=1;j<=pIniFileMgr->m_iUseNum;j++)
		{
			iFlags=1;
			for( i=1; i<=m_iUseNum;i++)
			{
				if(0==strcmp(m_pIniKey[i].sSectionName,pIniFileMgr->m_pIniKey[j].sSectionName))
				{
					iFlags=1;
					if(0==strcmp(m_pIniKey[i].sKeyName,pIniFileMgr->m_pIniKey[j].sKeyName))
					{
						iFlags=2;
						if(0==strcmp(m_pIniKey[i].sKeyValue[0],pIniFileMgr->m_pIniKey[j].sKeyValue[0]))
						{
							if(0==strcmp(m_pIniKey[i].sKeyValue[1],pIniFileMgr->m_pIniKey[j].sKeyValue[1]))
							{
								if(0== strcmp(m_pIniKey[i].sEffDate,pIniFileMgr->m_pIniKey[j].sEffDate))
								{
									iFlags=0;	break;
								}break;
							}break;
						}break;
					}
				}
			}
			if((iFlags==1)||(iFlags==2))	
			{
				stIniCompKey oIniCompKey;
				strcpy(oIniCompKey.sSectionName ,pIniFileMgr->m_pIniKey[j].sSectionName);
				strcpy(oIniCompKey.sKeyName,pIniFileMgr->m_pIniKey[j].sKeyName);
				memcpy(oIniCompKey.sKeyValue,pIniFileMgr->m_pIniKey[j].sKeyValue,sizeof(oIniCompKey.sKeyValue));
				strcpy(oIniCompKey.sEffDate,pIniFileMgr->m_pIniKey[j].sEffDate);
				if(iFlags==1)
				{
					//新增
					oIniCompKey.flag = OP_NEW;
				}
				if(iFlags==2)
				{
					//修改
					oIniCompKey.flag = OP_FIX;
				}
				vtCompKey.push_back(oIniCompKey);
			}
		}
		for( i=1; i<=m_iUseNum;i++)
		{
			iFlags=3;
			for( j=1;j<=pIniFileMgr->m_iUseNum;j++)
			{
				if((0==strcmp(m_pIniKey[i].sSectionName,pIniFileMgr->m_pIniKey[j].sSectionName))
					&&(0==strcmp(m_pIniKey[i].sKeyName,pIniFileMgr->m_pIniKey[j].sKeyName)))
				{
					iFlags=0;
					break;
				}
			}
			if(iFlags==3)
			{
				stIniCompKey oIniCompKey;
				strcpy(oIniCompKey.sSectionName ,m_pIniKey[i].sSectionName);
				strcpy(oIniCompKey.sKeyName,m_pIniKey[i].sKeyName);
				strcpy(oIniCompKey.sKeyValue[0],m_pIniKey[i].sKeyValue[0]);
				oIniCompKey.flag = OP_DEL;
				vtCompKey.push_back(oIniCompKey);
			}
		}
	if(bLock)
			delete pLock;
		return true;
	}else{
		if(bLock)
			delete pLock;
		return false;
	}
}


unsigned int CIniFileMgr::LoadFromStream(istream& inputStream)
{
	if (!inputStream.good())
	{
		//读取流失败
		Log::log(0,"读取数据流失败,%d",errno);
		return 0;
	}

	string strLine;
	string strSection,strKey,strValue;
	int i=0;
	int iCpyLen=0;
	CIniSection oIniSection;
	memset(&oIniSection,0,sizeof(CIniSection));
	do{
		getline(inputStream,strLine, '\n');
		strLine.erase(0,strLine.find_first_not_of(' '));
		strLine.erase(strLine.find_last_not_of(' ')+1);
		if(!strLine.empty())
		{
			if(!AnalysisLine(strLine,strSection,strKey,strValue))
			{
				//无法识别该行
				Log::log(0,"无法识别该行:%s",strLine.c_str());
			}else
			{
				if(!strKey.empty())
				{
					i++;
					strSection.erase(0,strSection.find_first_not_of(' '));
					strSection.erase(strSection.find_last_not_of(' ')+1);
					iCpyLen=MAX_SECTION_LEN>strSection.length()?strSection.length():MAX_SECTION_LEN-1;
					strncpy(oIniSection.sSectionName,strSection.c_str(),iCpyLen);
					oIniSection.sSectionName[iCpyLen]=0;

					strKey.erase(0,strKey.find_first_not_of(' '));
					strKey.erase(strKey.find_last_not_of(' ')+1);
					iCpyLen=MAX_KEY_VALUE_LEN>strKey.length()?strKey.length():MAX_KEY_VALUE_LEN-1;
					strncpy(oIniSection.sKeyName,strKey.c_str(),iCpyLen);
					oIniSection.sKeyName[iCpyLen]=0;

					strValue.erase(0,strValue.find_first_not_of(' '));
					strValue.erase(strValue.find_last_not_of(' ')+1);
					iCpyLen=MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1;
					strncpy(oIniSection.sKeyValue[0],strValue.c_str(),iCpyLen);
					oIniSection.sKeyValue[0][iCpyLen]=0;

					InsertSection(&oIniSection);
				}
			}
		}
	}while(!inputStream.eof());
	return i;

}

bool CIniFileMgr::MergeSection(CIniSection *pSectionDest, 
							   CIniSection *pSectionSource1)
{
	if((0==pSectionDest)||(0==pSectionSource1))
		return false;
	int iPos=0;
	if( (0== strcasecmp(pSectionDest->sSectionName,
		pSectionSource1->sSectionName)))
	{
		//Section相同
		char* stringFind = strrchr(pSectionDest->sKeyName,'.');
		if(0==stringFind)
			return false;
		iPos = (int)(stringFind -pSectionDest->sKeyName );
		if(iPos==0)
			return false;
		if((strlen(pSectionSource1->sKeyName)>iPos)&&
			(strncmp(pSectionDest->sKeyName,
			pSectionSource1->sKeyName,iPos)==0))
		{
			if(strcasecmp(&pSectionDest->sKeyName[iPos+1],NEW_VALUE)==0)
			{
				if(strcasecmp(&pSectionSource1->sKeyName[iPos+1],EFF_DATE_NAME)==0){
					if(!GlobalTools::CheckDateString(pSectionSource1->sKeyValue[0]))
						return false;
					strcpy(pSectionDest->sEffDate,pSectionSource1->sKeyValue[0]);
					pSectionDest->sKeyName[iPos]=0;
					return true;
				}
			}
		}
	}
	return false;
}
//这个狂恶心,需要合并三个段，例如配置成
//[SECTION]
//A.new_value=a
//A.old_value=b
//A.eff_date=20100101010101
//对于这种需要存储在一个 CIniSection  里面,ini文件不是用来做这个却非要用来做这个
bool CIniFileMgr::MergeSection(CIniSection* pSectionDest,CIniSection* pSectionSource1,CIniSection* pSectionSource2)
{
	if((0== pSectionDest)||(0==pSectionSource1)||(0==pSectionSource2))
		return false;
	int iPos=0;
	if( (0== strcasecmp(pSectionDest->sSectionName,pSectionSource1->sSectionName))&&
		(0==strcasecmp(pSectionSource1->sSectionName,pSectionSource2->sSectionName)))
	{
		//Section相同
		char* stringFind = strrchr(pSectionDest->sKeyName,'.');
		if(0==stringFind)
			return false;
		iPos = (int)(stringFind -pSectionDest->sKeyName );
		if(iPos==0)
			return false;
		if((strlen(pSectionSource1->sKeyName)>iPos)&&
			(strlen(pSectionSource2->sKeyName)>iPos)&&
			(strncmp(pSectionSource1->sKeyName,pSectionSource2->sKeyName,iPos)==0)&&
				(strncmp(pSectionDest->sKeyName,pSectionSource1->sKeyName,iPos)==0))
		{
			if(strcasecmp(&pSectionDest->sKeyName[iPos+1],NEW_VALUE)==0)
			{
				if(strcasecmp(&pSectionSource1->sKeyName[iPos+1],OLD_VALUE)==0)
				{
					//MD不检查最后一个了
					if(!GlobalTools::CheckDateString(pSectionSource2->sKeyValue[0]))
						return false;
					strcpy(pSectionDest->sEffDate,pSectionSource2->sKeyValue[0]);
					strcpy(pSectionDest->sKeyValue[1],pSectionDest->sKeyValue[0]);
					strcpy(pSectionDest->sKeyValue[0],pSectionSource1->sKeyValue[0]);					
				}else
				{
					if(!GlobalTools::CheckDateString(pSectionSource1->sKeyValue[0]))
						return false;
					strcpy(pSectionDest->sEffDate,pSectionSource2->sKeyValue[0]);
					strcpy(pSectionDest->sKeyValue[1],pSectionDest->sKeyValue[0]);
					strcpy(pSectionDest->sKeyValue[0],pSectionSource2->sKeyValue[0]);
				}
			}else
			{
				if(strcasecmp(&pSectionDest->sKeyName[iPos+1],OLD_VALUE)==0)
				{
					if(strcasecmp(&pSectionSource1->sKeyName[iPos+1],NEW_VALUE)==0)
					{
						//MD不检查最后一个了
						if(!GlobalTools::CheckDateString(pSectionSource2->sKeyValue[0]))
							return false;
						strcpy(pSectionDest->sEffDate,pSectionSource2->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[1],pSectionSource1->sKeyValue[0]);
					}else
					{
						if(!GlobalTools::CheckDateString(pSectionSource1->sKeyValue[0]))
							return false;
						strcpy(pSectionDest->sEffDate,pSectionSource1->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[1],pSectionSource2->sKeyValue[0]);
					}
				}else
				{
					if(!GlobalTools::CheckDateString(pSectionDest->sKeyValue[0]))
						return false;
					if(strcasecmp(&pSectionSource1->sKeyName[iPos+1],NEW_VALUE)==0)
					{
						if(!(strcasecmp(&pSectionSource2->sKeyName[iPos+1],OLD_VALUE)==0))
							return false;
						strcpy(pSectionDest->sEffDate,pSectionDest->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[0],pSectionSource2->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[1],pSectionSource1->sKeyValue[0]);
					}else
					{
						if(!(strcasecmp(&pSectionSource1->sKeyName[iPos+1],OLD_VALUE)==0))
							return false;
						if(!(strcasecmp(&pSectionSource2->sKeyName[iPos+1],NEW_VALUE)==0))
							return false;
						strcpy(pSectionDest->sEffDate,pSectionDest->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[1],pSectionSource2->sKeyValue[0]);
						strcpy(pSectionDest->sKeyValue[2],pSectionSource1->sKeyValue[0]);
					}
				}
			}
		}else
			return false;
	}else
		return false;
	pSectionDest->sKeyName[iPos]=0;
	return true;
}

bool CIniFileMgr::InsertSection(CIniSection *pIniSection)
{
	if(0!= pIniSection)
	{
		if(m_iUseNum>=m_iTotalNum)
			m_iTotalNum = ExtendSectionMem(&m_pIniKey,m_iTotalNum);
		int i=1;
		for(; i<=m_iUseNum;i++)
		{
			if((0==strcasecmp(pIniSection->sSectionName,m_pIniKey[i].sSectionName))&&
				(0==strcasecmp(pIniSection->sKeyName,m_pIniKey[i].sKeyName)))
			{
				Log::log(0,"相同的Section 和Key已经存在,SectionName:%s,KeyName:%s",pIniSection->sSectionName,pIniSection->sKeyName);
				return false;
			}
		}
		if((i>2)&&(MergeSection(&m_pIniKey[m_iUseNum-1],&m_pIniKey[m_iUseNum],pIniSection)))
		{
			m_iUseNum--;
			return true;
		}
		m_pIniKey[++m_iUseNum] = *pIniSection;
		return true;
	}else
	{
		Log::log(0,"插入Section 节点失败");
		return false;
	}
}
bool CIniFileMgr::FlashMemory(SysParamHis **ppSysParamHis,int& iCnt)
{
	if(m_pRuleMgr==0)
		m_pRuleMgr =new BaseRuleMgr();
	if(m_strFileName.empty())
		return false;
	vector<stIniCompKey> vctKey;

	CIniFileMgr *pFileMgrTmp = new CIniFileMgr();
	if(!pFileMgrTmp->LoadIniFromFile(m_strFileName.c_str()))
		return false;
	if(!Compare(pFileMgrTmp,vctKey))
		return false;
	
	vector<SysParamHis> vecParamHis;

	vector<stIniCompKey>::iterator it = vctKey.begin();
	for( ;it!=vctKey.end();it++){
		if(0==m_pRuleMgr->checkParamNeed(it->sSectionName,it->sKeyName)){
		SysParamHis ParamHis;
		strcpy(ParamHis.sSectionName,it->sSectionName);
		strcpy(ParamHis.sParamName,it->sKeyName);
		strcpy(ParamHis.sParamValue,it->sKeyValue[0]);
		vecParamHis.push_back(ParamHis);
		}else{
			if(m_pRuleMgr->check(it->sSectionName,
				it->sKeyName,it->sKeyValue[0],it->sEffDate,0,false)>0){
					SysParamHis ParamHis;
					strcpy(ParamHis.sSectionName,it->sSectionName);
					strcpy(ParamHis.sParamName,it->sKeyName);
					strcpy(ParamHis.sParamValue,it->sKeyValue[0]);
					vecParamHis.push_back(ParamHis);
			}else
			{
				if(this->AddKey((*it))<0){
					SysParamHis ParamHis;
					strcpy(ParamHis.sSectionName,it->sSectionName);
					strcpy(ParamHis.sParamName,it->sKeyName);
					strcpy(ParamHis.sParamValue,it->sKeyValue[0]);
					vecParamHis.push_back(ParamHis);
				}
			}
		}
	}
	iCnt= vecParamHis.size();
	ChangeSHMDataCount(this->m_iUseNum);
	if(iCnt>0){
		*ppSysParamHis= new SysParamHis[iCnt];
		for(vector<SysParamHis>::iterator it = vecParamHis.begin();
			it!= vecParamHis.end();it++){
				*ppSysParamHis[0]=(*it);
		}
		return false;
	}else
		return true;
}
bool CIniFileMgr::changeSectionKey(string strContent)
{
	if(strContent.empty())
		return false;
	string strChangeSection;
	string::size_type nPosBegin=0;
	string::size_type nPosEnd=0;
	nPosEnd = strContent.find(',');
	vector<CIniSection> vecKey;
	string strTmp;	
	while(nPosEnd!=string::npos){
		//分段读取每一个操作
		strTmp.assign(strContent,nPosBegin,nPosEnd-nPosBegin);
		nPosBegin = nPosEnd+1;
		nPosEnd = strContent.find(',',nPosBegin);
		CIniSection stKey;
		if(!GetIniKeyByString(strTmp,stKey))
			return false;
		vecKey.push_back(stKey);
	}
	if(nPosBegin!= strContent.length())
	{
		CIniSection stKey;
		strTmp.assign(strContent,nPosBegin,strContent.length());
		if(!GetIniKeyByString(strTmp,stKey))
			return false;
		vecKey.push_back(stKey);
	}

	//分析下是否需要合并
	vector<CIniSection>::iterator it = vecKey.begin();
	vector<CIniSection>::iterator itPre= vecKey.begin();	
	vector<CIniSection> vecKeyTmp;

	int iPos1,iPos2;
	CIniSection Key1,Key2;
	for(it++ ;it!=vecKey.end();){//只要从第二个开始判断这样写没错
		Key1=(*itPre);
		Key2=(*it);	
		if(MergeSection(&Key1,&Key2)){
			vecKeyTmp.push_back(Key1);
			itPre=it;//如果合并了先指向相同的节点
			it++;
			itPre=it;
			if(it==vecKey.end())
				break;
			it++;
			if(it==vecKey.end())
				break;
		}else{
			it++;
			itPre++;
		}
		vecKeyTmp.push_back(Key1);
	}
	if(itPre!=vecKey.end()){
		itPre = vecKey.end();
		--itPre;
		Key1=(*itPre);
		vecKeyTmp.push_back(Key1);
	}
	it=vecKeyTmp.begin();
	for(;it!=vecKeyTmp.end();it++)
	{
		if(!AddKey((*it),OP_FIX))
			return false;
	}
	return true;
}

bool CIniFileMgr::CopyToShm(const char*cpsKey,long lShmSize)
{
	if((0==cpsKey)||(cpsKey[0]==0))
	{
		Log::log(0,"核心参数共享内存没有设置");
		return false;
	}
	m_strMemKey=cpsKey;
	long lKey =atol(cpsKey);	
	WRITELOCK(DEFAULT_LOCK_KEY)
	if(0!= m_poIniSection)
		delete m_poIniSection;
	m_poIniSection = new SHMData<CIniSection>(lKey);	
	if(!m_poIniSection->exist())
	{
		if(0>=lShmSize)
			m_poIniSection->create(10240);
		else
		{
			lShmSize = (1024*lShmSize)/sizeof(CIniSection);
			m_poIniSection->create(lShmSize);
		}
	}


	CIniSection* pSection= (CIniSection *)(*m_poIniSection);
	if(m_poIniSection->getTotal()<=m_iTotalNum)
	{
		Log::log(0,"共享内存太小,无法导入数据");
		Log::log(0,"核心参数共享内存过小");
		return false;
	}

	int i=1;//m_iNum;
	unsigned int iOffset=0;
	m_poIniSection->reset();
	while(i<=m_iTotalNum)
	{
		iOffset = m_poIniSection->malloc();
		memcpy((void*)&pSection[iOffset],(void*)&m_pIniKey[i++],sizeof(CIniSection));
	}
	delete[] m_pIniKey; m_pIniKey=0;
	m_pIniKey=(CIniSection*)m_poIniSection;
	m_iTotalNum = m_poIniSection->getTotal();
	return true;
}
bool CIniFileMgr::GetDataFromShm(const char* cpsKey)
{
	if((0!=m_pIniKey)&&(m_poIniSection==0))
	{
		delete[] m_pIniKey;
		m_pIniKey=0;
	}
	if((0==cpsKey)||(cpsKey[0]==0))
		return false;
	if(!AttachIniShm(cpsKey))
		return false;
	m_pIniKey = (CIniSection*)(*m_poIniSection);
	m_iUseNum = m_poIniSection->getCount();
	m_iTotalNum = m_poIniSection->getTotal();
	return true;
}
bool CIniFileMgr::AttachIniShm(const char *cpsKey)
{
	if(0!= m_poIniSection)
		delete m_poIniSection;
	m_poIniSection = new SHMData<CIniSection>(atol(cpsKey));
	
	if(!m_poIniSection)
		return false;
	else
	{
		//检查是否attach成功
		if(!m_poIniSection->exist())
		{
			delete m_poIniSection;
			m_poIniSection=0;
			return false;
		}
	}
	return true;
}

bool CIniFileMgr::ReLoadIniFromFile(void)
{
	CIniFileMgr* pIniFileMgr = new CIniFileMgr();
	pIniFileMgr->LoadIniFromFile(m_strFileName.c_str());
	vector<stIniCompKey> vtKey;
	if(Compare(this,vtKey))
	{
		for(vector<stIniCompKey> ::iterator it = vtKey.begin();
			it!=vtKey.end();it++)
		{
			AddKey(*it);
		}
	}else
	{
		delete pIniFileMgr; pIniFileMgr=0;
		return false;
	}
	delete pIniFileMgr;pIniFileMgr=0;
	return true;
}

int	CIniFileMgr::ExtendSectionMem(CIniSection** ppSection,int iNum)
{
	if(0== ppSection)
		return 0;
	if((!*ppSection)||(iNum==0)){
		*ppSection = new CIniSection[MAX_INI_KEY_NUM];
	}else{
		try{
			CIniSection* pTemp = new CIniSection[MAX_INI_KEY_NUM+iNum];
			memcpy(pTemp,*ppSection,sizeof(CIniSection)*iNum);
			if(*ppSection){
				delete[] *ppSection;
			}
			*ppSection = pTemp;
		
		}catch(bad_alloc& e)
		{
			badalloc(e);
			return 0;
		}
	}
	return MAX_INI_KEY_NUM+iNum;
}

bool CIniFileMgr::LoadIniFromFile(const char *cpsFileName)
{
	//安全检查 
	if(0==cpsFileName)
		return false;

	if (-1 ==access( cpsFileName, R_OK| W_OK| F_OK ) )
	{
		//MBC_File+1
		Log::log(0,"文件不存在或者文件不可读写, 请检查文件 :%s",cpsFileName);
		return false;
	}

	m_iTotalNum= ExtendSectionMem(&m_pIniKey,0);

	m_strFileName = (char *)cpsFileName;
	ifstream oInputFile;
	oInputFile.open(cpsFileName);
	if(!oInputFile.is_open())
	{
        ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",cpsFileName);
        if(errno == ENOENT)
            ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",cpsFileName);
		Log::log(0,"文件打开失败,检查文件是否在用 :%s",cpsFileName);
		return false;
	}

	if( LoadFromStream(oInputFile)<=0)
	{
		Log::log(0,"装载配置文件失败 :%s",cpsFileName);
		oInputFile.close();
		m_iUseNum=1;
		return false;
	}
	oInputFile.close();
	return true;

}

