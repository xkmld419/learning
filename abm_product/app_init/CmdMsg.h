#ifndef CMDMSG_H_HEADER_INCLUDED_B4723CF5
#define CMDMSG_H_HEADER_INCLUDED_B4723CF5

//##ModelId=4B8DC58201D1
#include <string>
#include "Variant.h"
#include "MsgDefineServer.h"
#include "MsgDefineClient.h"
#include <string>
#include <vector>
#include "Date.h"
#include "IniFileMgr.h"
#define	INITSERVER_LOGFILE		"initserver_"

using namespace std;

class CmdMsgHistory;
class CmdSender;
class CmdReceiver;
class CmdMsgFactory;
class CommandCom;
class CmdMsg
{
  public:
    //##ModelId=4B8DCB7203E7
    virtual unsigned int GetSize();

	//##ModelId=4B8E232902E3
	unsigned int GetVariantSize(variant& vaValue);

protected:
	//##ModelId=4B8DCB730002
	void* DetachData(int &iSize);

	//前面空出sizeof(int)长度
	void* DetachSendData(int& iSize);

	//##ModelId=4B8DCB730005
	unsigned int AttachData(void* pData);
	


public:
	virtual int Execute(void);

	public:
	void SetSessionID(long lSessionID); 
	//static long GetSessionID(void){ return m_pMsgValue->lSessionID;};
public:	

	long GetMsgDest(void)
	{
		if(m_pMsgValue){
				return m_pMsgValue->lDest;
		}
		return -1;
	};

	long GetMsgSessionID(void)
	{
		if(m_pMsgValue){
			return m_pMsgValue->lSessionID;
		}
		return -1;
	}
	string GetMsgSrc(void)
	{
		if(m_pMsgValue)
		{
			return m_pMsgValue->vaSrc_Url;
		}
		return string("");
	}

	long GetMsgCmd(void)
	{
		if(m_pMsgValue)
		{
			return m_pMsgValue->lCmd;
		}
		return -1;
	}
	bool GetMsgContent(variant& Var)
	{
		if(m_pMsgValue)
		{
			Var = m_pMsgValue->vaContent;
		}
		return false;
	}
	~CmdMsg(){ if(0!= m_pMsgValue) delete m_pMsgValue; m_pMsgValue=0;if(0!=m_pData)free(m_pData); m_pData=0; };
private:
	void AttachVariant(variant::vt_type iVt,void* pVariant,int iPtrSize=0);

	void DetachVariant(void** pVariant);

protected:
	friend class CmdMsgFactory;
	friend class CmdSender;
	friend class CmdReceiver;
	friend class CommandCom;
	CmdMsg();


	//##ModelId=4B8E230B0033
    typedef struct _HB_MSG_
	{
        //##ModelId=4B8E230E0012
		long	lDest;		//源
        //##ModelId=4B8E230D03b88
		string	vaSrc_Url;			//目的 
        //##ModelId=4B8E230D03D3
		long	lCmd;		//命令
        //##ModelId=4B8E230E0004
		long	lSessionID;	//暂且当成用户ID来用
		variant	vaContent;		//内容
		long iVarPtrSize;				//当variant是个指针时需要知道大小
	}Msg;
    //##ModelId=4B8E230E02B6
	Msg* m_pMsgValue;
	char *m_pMsgData;
private:
	unsigned int m_iSize;
	void* m_pData;
	static long lSessionID;
};

class CmdMsgFactory
{
public:
	CmdMsgFactory()
	{
		m_pIniFileMgr = new CIniFileMgr();

	};
	static void ChangeMsgCmd(CmdMsg* pMsg,long lCmd)
	{
		pMsg->m_pMsgValue->lCmd=lCmd;
	};
	void SetMsgLogPath(string strFilePath) 
	{
		m_strMsgLogFilePath = strFilePath;
	};

	enum MSG_OP_TYPE
	{ OP_UNKONW=0,OP_SND,OP_RECV,OP_DEL};
	//重新设置日志文件名 initserver_yyyymmdd.dat
	void RefreshLogFileName(void);
	//bool SaveMsg(CmdMsg* pMsg);

	//存储消息日志
	bool SaveMsg(CmdMsg* pMsg,MSG_OP_TYPE State=OP_RECV);
	bool SaveMsg(CmdMsg* pMsg,stIniCompKey& oIniCompKey);	
	int GetHistoryMsgasString(vector<string>&arrayCmdMsg,const char* pBeginDate,const char* pEndDate=0);

	int GetHistoryMsg(vector<CmdMsgHistory*>& arrayCmdMsg,const char* pBeginDate,const char* pEndDate=0);
private:
	string m_strMsgLogFilePath;

	string	m_strMsgLogFileNameW;
	string  m_strMsgLogFileNameR;


	void GetMsgLogName(string& strFileName,Date& d);

	int ReadMsgLog(string& strFileName,vector<CmdMsgHistory*>& arrayCmdMsg);
	private:
	CIniFileMgr* m_pIniFileMgr;

};

class CmdMsgHistory
{
        public:
                CmdMsg* m_pMsg;
                char m_sDate[16];
                CmdMsgFactory::MSG_OP_TYPE m_oType;
		stIniCompKey* m_pIniCompKey;
        public:
        ~CmdMsgHistory(){
                if(0!=m_pMsg)
                        delete m_pMsg;
                m_pMsg=0;
		if(0!= m_pIniCompKey)
			delete m_pIniCompKey;
        };
        CmdMsgHistory()
        {
                m_pMsg=0;
                m_oType = CmdMsgFactory::OP_UNKONW;
                memset(m_sDate,0,sizeof(m_sDate));
		m_pIniCompKey =0;
        };
        void setMsgState(CmdMsgFactory::MSG_OP_TYPE oType)
        {
                m_oType=oType;
        };


};

#endif /* CMDMSG_H_HEADER_INCLUDED_B4723CF5 */


