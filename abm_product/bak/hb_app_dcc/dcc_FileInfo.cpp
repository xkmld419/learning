/*ver:1*/

#include "dcc_FileInfo.h"
#include "sendDcrFromFile.h"

DccFileFieldInfo::DccFileFieldInfo()
{
	m_sFieldInfo=string("");
	m_sSessionID = string("");
	m_iHopbyHop = 0;
	m_iEndtoEnd = 0;
	m_iAuthApplicationID = 0;
	m_iRequestType = 0;
	m_iRequestType = 0;
	m_iApplicationID = 0;
	m_sDestinationHost = string("");
	m_sDestinationRealm = string("");
	m_iFieldSeq = 0;
}

DccFileFieldInfo::~DccFileFieldInfo()
{
	
}

void DccFileFieldInfo::genDcrHeadValue()
{
	//生成关于dcr包头部及dcr非业务部分所需的信息
	//如hopbyhop，endtoend，sessionid，applicationid等

	
	return;
}

bool DccFileFieldInfo::genSendCCRString(string & sCCRString,char *sServiceContext)
{
	m_oParamMgr.getDccHeadValue(sServiceContext);
	
	return true;
}

DccFileInfo::DccFileInfo()
{
	m_iFileTypeID = 0;
	memset(m_sServiceContext,0,sizeof(m_sServiceContext));
	memset(m_sBreakFileDate,0,sizeof(m_sBreakFileDate));
	memset(m_sBreakFile,0,sizeof(m_sBreakFile));
	memset(m_sFilePatern,0,sizeof(m_sFilePatern));
	m_iBreakLine = 0;
	strcpy(m_sFileTypeState,"ENA");
	m_pStoreDir = NULL;
	m_pBakDir = NULL;
	m_vecFieldList.clear();
	m_iCurFieldSeq = 0;
	m_pCurFile = NULL;
	m_bHaveDoneFile = true;
}

DccFileInfo::~DccFileInfo()
{
	freeFieldInfoVec();
	if (m_pCurFile)
	{
		delete m_pCurFile;
		m_pCurFile = NULL;
	}
}

void DccFileInfo::freeFieldInfoVec()
{
	int i=0;
	for (i=0;i<m_vecFieldList.size();i++)
	{
		DccFileFieldInfo *poTmp=m_vecFieldList[i];
		if (poTmp)
		{
			delete poTmp;
			poTmp = NULL;
		}
	}
}

bool DccFileInfo::checkFileTypeState()
{
	if (strcmp(m_sFileTypeState,"ENA")!=0)
		return false;

	return true;
}

bool DccFileInfo::getFile(File *pFile)
{
	bool bFind=false;
	
	if (!checkFileTypeState())
		return bFind;	

	while(1)
	{
		m_pCurFile=m_pStoreDir->nextFileRecursive();
		if (!m_pCurFile)
			break;

		if (checkPatern(m_pCurFile->getFileName()))
		{
			bFind = true;
			break;
		}
	}
	
	return bFind;
}

bool DccFileInfo::checkPatern(const char * sFileName) throw(string)
{
	  const int SUBSLEN = 10; 
	  const int EBUFLEN = 128; /* 错误消息缓存长度 */ 

	  size_t len; /* 错误消息的长度 */ 
	  regex_t re; /* 编译后的正则表达式 */ 
	  regmatch_t subs[SUBSLEN]; /* 匹配的字符串位置 */ 
	  char errbuf[EBUFLEN]; /* 错误消息 */ 
	  int err, i; 

	  err = regcomp (&re, m_sFilePatern, REG_EXTENDED);
	  if (err) 
	  { 
	    len = regerror (err, &re, errbuf, sizeof (errbuf)); 
	    throw string(errbuf);
	  } 
	  err = regexec (&re, m_pCurFile->getFileName(), (size_t)SUBSLEN, subs, 0); 

	  if (err == REG_NOMATCH) 
	  { 
	    regfree (&re); 
	    return false; 
	  } 
	  else if (err) 
	  { 
	    len = regerror (err, &re, errbuf, sizeof (errbuf)); 
	    throw string(errbuf); 
	  } 

	  regfree(&re);  
	  return true;
}

void DccFileInfo::readRecordToVec()
{
	FILE *fp  =NULL;
	int i=0;

	if ((fp = fopen(m_pCurFile->getFullFileName(),"r")) == NULL)
		return;
	char sFileLine[FILE_FIELD_LENGTH];
	memset(sFileLine,0,sizeof(sFileLine));	
	while (fgets(sFileLine,sizeof(sFileLine),fp))
	{
		if (sFileLine[strlen(sFileLine)-1] == '\n')
			sFileLine[strlen(sFileLine)-1] = 0;
		
		DccFileFieldInfo *pFieldInfo = new DccFileFieldInfo();
		pFieldInfo->m_sFieldInfo=string(sFileLine);
		pFieldInfo->m_iFieldSeq = i;
		if (i==0)
			pFieldInfo->m_iFieldType = FILE_HEAD_TYPE;
		else
			pFieldInfo->m_iFieldType = FILE_BODY_TYPE;
		pFieldInfo->genDcrHeadValue();
		m_vecFieldList.push_back(pFieldInfo);
		memset(sFileLine,0,sizeof(sFileLine));
		i++;
	}

	if (i>1)
	{
		DccFileFieldInfo *pTmpField = m_vecFieldList[m_vecFieldList.size()-1];
		pTmpField->m_iFieldType = FILE_TAIL_TYPE;
	}

	fclose(fp);
		
}

bool DccFileInfo::recordIsEmpty()
{
//后面增加表里是否有头尾的判断,如果有头有尾则>2，仅有头或尾>1，无头无尾>0	
	if (m_vecFieldList.size()<3)   //包括头尾记录数要>2   
		return true;
	else
		return false;
}

DccFileFieldInfo * DccFileInfo::getNextRecord()
{
	DccFileFieldInfo *pFieldInfo = NULL;
	
	m_iCurFieldSeq =  0;
	if (!checkFileTypeState())
		return false;

	if (m_bHaveDoneFile)
	{
		freeFieldInfoVec();
		readRecordToVec();
		if (recordIsEmpty())
		{
			m_bHaveDoneFile = true;						
		}
		else
		{
			m_bHaveDoneFile = false;
			m_iCurFieldSeq++;
			pFieldInfo = m_vecFieldList[m_iCurFieldSeq];
		}
	}
	else
	{
		m_iCurFieldSeq++;
		pFieldInfo = m_vecFieldList[m_iCurFieldSeq];
	}
	
	if (m_iCurFieldSeq == m_vecFieldList.size()-1) 		
	{
		m_bHaveDoneFile = true;
		m_iCurFieldSeq = 0;
	}

	return pFieldInfo;
}

char *DccFileInfo::getServiceContext()
{
	return m_sServiceContext;
}


