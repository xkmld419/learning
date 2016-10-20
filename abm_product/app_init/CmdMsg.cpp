# include "CmdMsg.h"
//##ModelId=4B8DCB7203E7
//#include "DebugTools.h"
#include <iostream>
#include <assert.h>
#include "CommandCom.h"
#include <sys/types.h>
#include <pwd.h>
#define	DEFAULT_ENV_PATH		"/opt/opthb"
#include "Log.h"
#include "ReadIni.h"
using namespace std;

#define	LOG_SECTION					"LOG"
#define	LOG_PATH							"log_path"

long CmdMsg::lSessionID=0;
unsigned int CmdMsg::GetSize(void)
{
	if(!m_pMsgValue)
		return 0;
	return sizeof(m_pMsgValue->lSessionID)+sizeof(m_pMsgValue->lDest)+(m_pMsgValue->vaSrc_Url.size()/sizeof(long)+1)*sizeof(long)
		+sizeof(m_pMsgValue->lCmd)+sizeof(m_pMsgValue->iVarPtrSize)+GetVariantSize(m_pMsgValue->vaContent)+sizeof(long)+
		sizeof(long)*6;
}

void *CmdMsg::DetachSendData(int &iSize)
{
	iSize = GetSize()+sizeof(long);
	if(!m_pMsgValue)
		return 0;

	char* pDataTmp =(char*) malloc(sizeof(char)*iSize);
	void* pData= (&pDataTmp[0])+sizeof(long);
	memset(pDataTmp,0,sizeof(char)*iSize);

	*(long*)pData=sizeof(m_pMsgValue->lSessionID);
	pData=((char*)pData)+sizeof(long);
	*(long*)pData=m_pMsgValue->lSessionID;
	pData=((char*)pData)+sizeof(m_pMsgValue->lSessionID);

	*(long*)pData = sizeof(m_pMsgValue->lDest);
	pData = ((char*)pData)+sizeof(long);
	*(long*)pData = m_pMsgValue->lDest;
	pData = ((char*)pData)+sizeof(m_pMsgValue->lDest);

	*(long*)pData = m_pMsgValue->vaSrc_Url.size();
	pData = ((char*)pData)+sizeof(long);
	memcpy(pData,m_pMsgValue->vaSrc_Url.c_str(),m_pMsgValue->vaSrc_Url.size());
	pData = ((char*)pData)+(m_pMsgValue->vaSrc_Url.size()/sizeof(long)+1)*sizeof(long);

	*(long*)((char*)pData) = sizeof(long);
	pData = ((char*)pData)+sizeof(long);
	*(long*)pData = m_pMsgValue->lCmd;
	pData = ((char*)pData)+sizeof(m_pMsgValue->lCmd);

	*(long*)pData = sizeof(long);
	pData=((char*)pData)+sizeof(long);
	*(long*)pData = m_pMsgValue->iVarPtrSize;
	pData = ((char*)pData)+sizeof(m_pMsgValue->iVarPtrSize);

	*(long*)pData = GetVariantSize(m_pMsgValue->vaContent)+sizeof(long);
	pData = ((char*)pData)+sizeof(long);
	DetachVariant(&pData);
	//delete m_pMsgValue; m_pMsgValue=0;
	assert((char*)pData+GetVariantSize(m_pMsgValue->vaContent)+sizeof(long)-pDataTmp==iSize);	
	return pDataTmp;
}

void CmdMsg::SetSessionID(long lSessionID)
{
	if(m_pMsgValue)
		m_pMsgValue->lSessionID=lSessionID;
}

void CmdMsg::DetachVariant(void** ppVariant)
{
	string strTmp ;
	void*pVariant = *ppVariant;
	*(long*)pVariant =m_pMsgValue->vaContent.get_type();
	pVariant = (char*)(pVariant)+sizeof(long);
	switch(m_pMsgValue->vaContent.get_type())
	{
	case variant::vt_integer:
		(*(int*)pVariant)= m_pMsgValue->vaContent.asInt();
		break;
	case variant::vt_long:
		(*(long*)pVariant)= m_pMsgValue->vaContent.asLong();
		break;
	case variant::vt_float:
		(*(float*)pVariant)= m_pMsgValue->vaContent.asFloat();
		break;
	case variant::vt_boolean:
		(*(bool*)pVariant)= m_pMsgValue->vaContent.asBool();
		break;
	case variant::vt_string:
		//strcpy((char*)pVariant,m_pMsgValue->vaContent.asStringEx().c_str());
		strTmp = m_pMsgValue->vaContent.asStringEx();
		memcpy(pVariant,strTmp.c_str(),
				m_pMsgValue->vaContent.asStringEx().size());
		break;
	case variant::vt_ptr:
		{
			if(m_pMsgValue->iVarPtrSize>0)
				memcpy(pVariant,m_pMsgValue->vaContent.asPtr(),m_pMsgValue->iVarPtrSize);
		}
		break;
	case variant::vt_empty:
		{
		}break;
	default:
		abort();
	}
}

void CmdMsg::AttachVariant(variant::vt_type iVt,void* pVariant,int iPtrSize)
{
	if(!m_pMsgValue)
		return;
	m_pMsgValue->vaContent.change_type( iVt);
	switch(iVt)
	{
	case variant::vt_integer:
		m_pMsgValue->vaContent = *((int*)pVariant);
		break;
	case variant::vt_long:
		m_pMsgValue->vaContent=*((long*)pVariant);
		break;
	case variant::vt_float:
		m_pMsgValue->vaContent=*((float*)pVariant);
		break;
	case variant::vt_boolean:
		m_pMsgValue->vaContent = *((bool*)pVariant);
		break;
	case variant::vt_string:
		m_pMsgValue->vaContent=(char*)pVariant;
		break;
	case variant::vt_date:
		{
			m_pMsgValue->vaContent=Date((char*)pVariant);
			break;
		}
	case variant::vt_ptr:
		{
			if(iPtrSize==0)
				abort();	
			m_pMsgValue->vaContent=(void*)pVariant;
		}break;
	case variant::vt_empty:
		{
		}break;
	default:
		abort();
	}
}

//##ModelId=4B8DCB730005
unsigned int CmdMsg::AttachData(void* pData)
{
	//可以优化不需要每次都new
	if(m_pMsgValue)
		delete m_pMsgValue;
	m_pMsgValue =0;
	m_pMsgValue = new Msg;
	long iLen = 0;
	m_pData = pData;

	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->lSessionID=(long)(*(long*)pData);
	pData=((char*)pData)+sizeof(m_pMsgValue->lSessionID);

	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->lDest = (long)(*(long*)pData);
	pData = ((char*)pData)+sizeof(m_pMsgValue->lDest);

	iLen = (long)(*(long*)pData);
	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->vaSrc_Url.assign((char*)pData,iLen);

	pData = ((char*)pData)+(iLen/sizeof(long)+1)*sizeof(long);
	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->lCmd=(long)(*(long*)pData);
	pData = ((char*)pData)+sizeof(m_pMsgValue->lCmd);

	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->iVarPtrSize = (long)(*(long*)pData);
	pData = ((char*)pData)+sizeof(m_pMsgValue->iVarPtrSize);


	pData = ((char*)pData)+sizeof(long);
	m_pMsgValue->vaContent.change_type((variant::vt_type)(long)(*(long*)pData));
	pData = ((char*)pData)+sizeof(long);
	AttachVariant((variant::vt_type)m_pMsgValue->vaContent.get_type() ,pData,m_pMsgValue->iVarPtrSize);

	return GetSize();
}

unsigned int CmdMsg::GetVariantSize(variant &vaValue)
{
	switch(vaValue.get_type())
	{
	case variant::vt_integer:
		return sizeof(long);//+sizeof(int);
	case variant::vt_long:
		return sizeof(long);//+sizeof(int);
	case variant::vt_boolean:
		return sizeof(long);//+sizeof(int);
	case variant::vt_string:
		return ((vaValue.string_val_.size()/sizeof(long))+1)*sizeof(long)+sizeof(long);//+sizeof(int);
	case variant::vt_ptr:
		{
			//指针型数据需要外部大小
			return m_pMsgValue->iVarPtrSize;//+sizeof(int); 
		}
	default:
		return 0;//sizeof(int)*2;
	}
}
int CmdMsg::Execute(void)
{
	if(!m_pMsgValue)
		return 0;
	cout<<"lDest: "<<m_pMsgValue->lDest<<endl;
	cout<<"lCmd: "<<m_pMsgValue->lCmd<<endl;
	cout<<"Url:"<<m_pMsgValue->vaSrc_Url<<endl;
	cout<<"vaContent: "<<m_pMsgValue->vaContent.asStringEx()<<endl;
	return 1;
}
//##ModelId=4B8E232902E3
CmdMsg::CmdMsg()
{
	m_iSize = 0;
	m_pMsgValue = 0;
	m_pMsgData=0;
	m_pData=0;
}
void CmdMsgFactory::GetMsgLogName(string& strFileName,Date& d)
{
	int iPos = strFileName.find_last_of('_');
	string strDate;
	if(iPos==-1)
	{
		//文件名不带 '_' 或者文件名为空
		strFileName=INITSERVER_LOGFILE;
	}else
	{
		strDate = strFileName.substr(iPos,10);
	}
	strFileName=INITSERVER_LOGFILE;

	char sDateTmp[11]={0};
	strncpy(sDateTmp,d.toString(),10);
	if(strDate.length()==10)
	{
		if(strDate.compare(sDateTmp)==0)
			return ;
	}	
	strDate = sDateTmp;
	strFileName=strFileName+strDate+".dat";
	return ;
}
void CmdMsgFactory::RefreshLogFileName(void)
{
	Date d1;
	GetMsgLogName(m_strMsgLogFileNameW,d1);
}

int CmdMsgFactory::GetHistoryMsg(vector<CmdMsgHistory*>& arrayCmdMsg,const char* pBeginDate,const char* pEndDate)
{
	Date d1,d2;
	if(0==pBeginDate)
	{
		return 0;
	}else
	{
		d1.parse(pBeginDate);
	}
	if(0!=pEndDate)
	{
		d2.parse(pEndDate);
	}

	
	while(d1<d2)
	{
		GetMsgLogName(m_strMsgLogFileNameR,d1);
		ReadMsgLog(m_strMsgLogFileNameR,arrayCmdMsg);
		/*if(!arrayCmdMsg.empty())
			cout<<"********LogFile:"<<m_strMsgLogFileNameR<<endl;
		vector<CmdMsgHistory*>::iterator it = arrayCmdMsg.begin();
		for(;it!= arrayCmdMsg.end();it++)
		{
			cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
			CmdMsgHistory* pMsg =*it;
			Trace::TraceMsgLog(pMsg->m_pMsg);
			switch(pMsg->m_oType)
			{
				case CmdMsgFactory::OP_SND:
				{
					cout<<"Init Send"<<endl;
				}break;
				case CmdMsgFactory::OP_RECV:
				{
					cout<<"Init Recv"<<endl;
				}break;
				case CmdMsgFactory::OP_DEL:
				{
					cout<<"Init Del"<<endl;
				}break;
				defualt:
				{
					cout<<"What Fuck"<<endl;
				}
			}
			cout<<"Date:"<<pMsg->m_sDate<<endl;
			delete pMsg;
		}
		arrayCmdMsg.clear();*/
		d1.addHour(1);
	}
	return arrayCmdMsg.size();
}


int CmdMsgFactory::GetHistoryMsgasString(vector<string>&arrayCmdMsg,const char* pBeginDate,const char* pEndDate)
{
	if(m_strMsgLogFilePath.empty())
	{
		string strTIBSHOME_Path= getenv( "HBCONFIGDIR" );
		if(strTIBSHOME_Path.empty())
		{
			//没有配置环境变量
			Log::log(0,"环境变量没有设置:%s","HBCONFIGDIR");	
		}
		string strTIBSHOME_CfgFile =strTIBSHOME_Path+"/hssconfig";
		ReadIni oReadIni;
		char sKeyValue[1024]={0};
		oReadIni.readIniString(strTIBSHOME_CfgFile.c_str(),LOG_SECTION,LOG_PATH,sKeyValue,0);
		this->SetMsgLogPath(sKeyValue);
	}
	vector<CmdMsgHistory*>arrayMsgHistory;
	char sBegin[16]="20100925000000";
	if(0==pBeginDate)
	{
		pBeginDate = sBegin;
	}
	if(GetHistoryMsg(arrayMsgHistory,pBeginDate,pEndDate)<=0)
	{
		return 0;
	}
	CmdMsgHistory* pMsgHistory=0;
	passwd *pPass =0; 
	stIniCompKey oIniCompKey;
	for(vector<CmdMsgHistory*>::iterator it =arrayMsgHistory.begin();
		it!=arrayMsgHistory.end();it++)
	{
		pMsgHistory=(*it);
		if(pMsgHistory->m_oType!= OP_RECV)
			continue;
		string strInitHistory;
		strInitHistory.append("[STAFFID:");
		int lUID= pMsgHistory->m_pMsg->GetMsgSessionID();
		pPass=getpwuid(lUID);
		if((0==lUID)||(0==pPass)||(pPass->pw_name==0)){
			strInitHistory.append("Unknow]");
		}else{
			strInitHistory.append(pPass->pw_name);
			strInitHistory.append("]");
		}
		strInitHistory.append("[PARAM:");

		variant vt;
		pMsgHistory->m_pMsg->GetMsgContent(vt);
		if(!vt.isString())
			continue;
		string strVariant = vt.asStringEx();
		//if(!m_pIniFileMgr->GetIniKeyByString(strVariant,oIniCompKey))
			//continue;
		stIniCompKey* pIniCompKey = pMsgHistory->m_pIniCompKey;	
		strInitHistory.append(pIniCompKey->sSectionName);
		strInitHistory.append(".");
		strInitHistory.append(pIniCompKey->sKeyName);
		strInitHistory.append("]  ");
		long lCmd = pMsgHistory->m_pMsg->GetMsgCmd();
		if(lCmd==MSG_PARAM_CHANGE)
		{
			char sTemp[256]={0};
			sprintf(sTemp,"操作时间:%s ,参数原值为:%s,修改参数值为:%s",pMsgHistory->m_sDate,pIniCompKey->sKeyValue[2],pIniCompKey->sKeyValue[0]);
			strInitHistory.append(sTemp);
		}else
		{
			if(lCmd==MSG_PARAM_S_N)
			{
				strInitHistory.append("读取参数");
			}else
				continue;
		}
		arrayCmdMsg.push_back(strInitHistory);
		delete pMsgHistory; pMsgHistory=0;
	}
	arrayMsgHistory.clear();
	return arrayCmdMsg.size();
}

int CmdMsgFactory::ReadMsgLog(string& strFileName,vector<CmdMsgHistory*>& arrayCmdMsg)
{
	if(strFileName.empty())
		return 0;
	
	FILE *fLog= 0;
	string strFullFileName =m_strMsgLogFilePath+'/'+m_strMsgLogFileNameR;
	if (-1 ==access(strFullFileName.c_str(), R_OK| W_OK| F_OK ) )
	{
		//MBC_File+1
		return 0;
	}	
	fLog = fopen(strFullFileName.c_str(),"rb");
	if(fLog==0)
	{
        ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",strFullFileName.c_str());
        if(errno == ENOENT)
            ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",strFullFileName.c_str());
		Log::log(0,"文件不存在或者文件不可读写或者没内容, 请检查文件 :%s:%d",strFileName.c_str(),errno);
		return 0;
	}
	char sBufTmp[16]={0};
	char sDate[16]={0};
	char* pBuf=0;
	int iRLen =0;
	while(!feof(fLog))
	{
		iRLen = fread(sBufTmp,sizeof(char),sizeof(char)*14,fLog);
		if((14!=iRLen)||(feof(fLog)))
			break;
		strncpy(sDate,sBufTmp,14);
		sDate[14]=0;
		int iSize=0;
		int iState=0;
		iRLen=fread(&iState,sizeof(char),sizeof(int)/sizeof(char),fLog);
		if((sizeof(int)/sizeof(char)!=iRLen)||feof(fLog))
			break;
		iRLen=fread(&iSize,sizeof(char),sizeof(int)/sizeof(char),fLog);
		if((sizeof(int)/sizeof(char)!=iRLen)||feof(fLog))
			break;
		int iBufLen =iSize;//atoi(sBufTmp);
		pBuf =(char*)malloc(sizeof(char)*iBufLen);
		iRLen=fread(pBuf,sizeof(char),iBufLen,fLog);
		if((iRLen!= iBufLen)||feof(fLog))
			break;
		CmdMsg* pMsg = CommandCom::CreateNullMsg();
		pMsg->AttachData(pBuf+sizeof(long));
		stIniCompKey* pIniComKey = (stIniCompKey*)malloc(sizeof(stIniCompKey));
		iRLen =fread((char*)pIniComKey,sizeof(char),sizeof(stIniCompKey),fLog);
		if(iRLen != sizeof(stIniCompKey)||feof(fLog))
			break;
		CmdMsgHistory* pMsgHistory = new CmdMsgHistory();
		pMsgHistory->m_pMsg=pMsg;
		pMsgHistory->setMsgState((CmdMsgFactory::MSG_OP_TYPE)iState);
		pMsgHistory->m_pIniCompKey = pIniComKey;
		strcpy(pMsgHistory->m_sDate,sDate);
		arrayCmdMsg.push_back(pMsgHistory);
		iRLen = fread(sBufTmp,sizeof(char),sizeof(char)*14,fLog);
		if(14!=iRLen)
			break;
	}
	fclose(fLog);
	if(pBuf)
		delete pBuf;
	return arrayCmdMsg.size();
}
bool CmdMsgFactory::SaveMsg(CmdMsg* pMsg,stIniCompKey & oIniKey)
{
	bool bRet = (0==pMsg);
        bRet = m_strMsgLogFilePath.empty();
        if((0==pMsg)||(m_strMsgLogFilePath.empty()))
                return true;
        int iSize=0;
        void* pBuf =0;
        pBuf = pMsg->DetachSendData(iSize);
        if((0== pBuf)||(0>=iSize))
                return false;

        string strFullFileName ;
	if(m_strMsgLogFilePath[m_strMsgLogFilePath.length()-1]=='/')
		strFullFileName=m_strMsgLogFilePath+m_strMsgLogFileNameW;
	else
		strFullFileName=m_strMsgLogFilePath+'/'+m_strMsgLogFileNameW;
        FILE *fLog= 0;
        fLog = fopen(strFullFileName.c_str(),"a+");
        if(!fLog)
        {
		if(pBuf)
			free(pBuf);
		pBuf=0;
            ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",strFullFileName.c_str());
            if(errno == ENOENT)
                ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",strFullFileName.c_str());
                Log::log(0,"文件打开失败:%s",strFullFileName.c_str());
                fLog = fopen(m_strMsgLogFileNameW.c_str(),"a+");
                if(!fLog)
                {
                ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_strMsgLogFileNameW.c_str());
                if(errno == ENOENT)
                    ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_strMsgLogFileNameW.c_str());
                        Log::log(0,"日志文件打开失败:%s, 无法存储消息日志",m_strMsgLogFileNameW.c_str());
                        return true;
                }
        }
        Date d1;
        char sStrDate[16]={0};
        strcpy(sStrDate,d1.toString());
        int iFlag = oIniKey.flag;
        if(fwrite(sStrDate,sizeof(char),14,fLog)!=14)
        {
                goto _log_exit;
        }
        if(fwrite(&iFlag,sizeof(int),1,fLog)!=1)
                goto _log_exit;
        if(fwrite(&iSize,sizeof(int),1,fLog)!=1)
        {
                goto _log_exit;
        }
        if(fwrite(pBuf,sizeof(char),iSize,fLog)!=iSize)
        {
                goto _log_exit;
        }
	iSize = sizeof(stIniCompKey)/sizeof(char);
	if(fwrite(&oIniKey,sizeof(char),iSize,fLog)!=iSize)
	{
		goto _log_exit;
	}
        if(fwrite(sStrDate,sizeof(char),14,fLog)!=14)
        {
                goto _log_exit;
        }

_log_exit:
	if(pBuf)
		free(pBuf);
	pBuf=0;
        fclose(fLog);
	string strInitHistory;
	strInitHistory.append("[STAFFID:");
	int lUID= pMsg->GetMsgSessionID();
	char sTemp[256]={0};
        passwd *pPass =getpwuid(lUID);
        if((0==lUID)||(0==pPass)||(pPass->pw_name==0)){
               strInitHistory.append("Unknow]");
        }else{
               strInitHistory.append(pPass->pw_name);
               strInitHistory.append("]");
        }
	strInitHistory.append("[PARAM:");
	variant vt;
        pMsg->GetMsgContent(vt);
        if(!vt.isString())
               return true ;
	string strVariant = vt.asStringEx();
       	
	strInitHistory.append(oIniKey.sSectionName);
                strInitHistory.append(".");
                strInitHistory.append(oIniKey.sKeyName);
                strInitHistory.append("]  "); 
	long lCmd = pMsg->GetMsgCmd();	
							if(lCmd==MSG_PARAM_CHANGE)
               {
               	sprintf(sTemp,"参数原值为:%s,修改参数值为:%s",
               	oIniKey.sKeyValue[2],oIniKey.sKeyValue[0]);
               	strInitHistory.append(sTemp);
						}else
                {
                        if(lCmd==MSG_PARAM_S_N)
                        {
                                strInitHistory.append("读取参数");
                        }else
                                return false;
                }
	return true;	
}
bool CmdMsgFactory::SaveMsg(CmdMsg* pMsg,MSG_OP_TYPE State)
{
	return true;
	bool bRet = (0==pMsg);
	bRet = m_strMsgLogFilePath.empty();
	if((0==pMsg)||(m_strMsgLogFilePath.empty()))
		return true;
	int iSize=0;
	void* pBuf =0;
	pBuf = pMsg->DetachSendData(iSize);
	if((0== pBuf)||(0>=iSize))
		return false;

	string strFullFileName =m_strMsgLogFilePath+'/'+m_strMsgLogFileNameW;
	FILE *fLog= 0;
	fLog = fopen(strFullFileName.c_str(),"a+");
	if(!fLog)
	{
        ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",strFullFileName.c_str());
        if(errno == ENOENT)
            ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_strMsgLogFileNameW.c_str());
		Log::log(0,"日志文件打开失败:%s",strFullFileName.c_str());
		fLog = fopen(m_strMsgLogFileNameW.c_str(),"a+");
		if(!fLog)
		{
            ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",m_strMsgLogFileNameW.c_str());
            if(errno == ENOENT)
                ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",m_strMsgLogFileNameW.c_str());
			Log::log(0,"日志文件打开失败:%s, 无法存储消息日志",m_strMsgLogFileNameW.c_str());
			return true;
		}
	}
	Date d1;
	char sStrDate[16]={0};
	strcpy(sStrDate,d1.toString());
	int iFlag = State;
	if(fwrite(sStrDate,sizeof(char),14,fLog)!=14)
	{
		goto _log_exit;
	}
	if(fwrite(&iFlag,sizeof(int),1,fLog)!=1)
		goto _log_exit;
	if(fwrite(&iSize,sizeof(int),1,fLog)!=1)
	{
		goto _log_exit;
	}
	if(fwrite(pBuf,sizeof(char),iSize,fLog)!=iSize)
	{
		goto _log_exit;
	}
	if(fwrite(sStrDate,sizeof(char),14,fLog)!=14)
	{
		goto _log_exit;
	}

_log_exit:
	fclose(fLog);
	return true;
}

