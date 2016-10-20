#ifndef __DCC_UN_PACK_H
#define __DCC_UN_PACK_H 

#include <pthread.h> 
#include <sstream>
#include "Process.h"
#include "Log.h"
#include "interrupt.h"
#include "StdEvent.h"
#include "Date.h"

#include "OcpMsgParser.h"
#include "dcc_ParamsMgr.h"
#include "dcc_FileMgr.h"
#include "DccLog.h"
#include "dcc_ConstDef.h"

class DccUnPack : public Process
{
public:
	DccUnPack();
	~DccUnPack();

	int run();    
private:
	char *m_pRecvBuffer;			// 接收缓冲区 sizeof(StdEvent) * (m_iMsgEventNum)
	char *m_pSendBuffer;			// 发送缓冲区 sizeof(StdEvent) * (m_iMsgEventNum)
	int  m_iMsgEventNum;            // 接收消息存放的最大事件数+1，读WF_MQ_ATTR表中MSG_EVENT_NUM的最大值

	char (*m_ContextIds)[256];                       // 要处理的业务Service-Context-Id组
	int  m_iContextNum;                              // 要处理的业务数，即m_ContextIds的行数
    char m_sOriginHost[256];                         // 发起端设备标识
	char m_sNeHost[256];				             // 要处理的业务对应的NE_HOST
	char m_sActiveContext[256];                      // 激活CCR对应的Service-Context-Id
	char m_sServiceContextId[256];                   // 解包后返回的Service-Context-Id
	int  m_iMsgType;                                 // 解包后返回的包类型

	int  m_isActiveCCAOK;                            // 激活CCA是否返回，初始已返回，1返回，0未返回
	Date m_dDate;  
	int  m_nTime;                                    // 激活超时时长

    int m_iTaskId;
    char* m_shmServiceType;
	//createActiveCCR的参数 形式：Session-Id|hh|ee|Destination-Realm|Auth-Application-Id|Service-Context-Id|CC-Request-Type|CC-Request-Number
	char m_ActiveCCRArgv[512];  

	OcpMsgParser *m_pOcpMsgParser;  
	dccParamsMgr *m_pDccParamMgr;
	FileToDccMgr *m_pFileToDccMgr;

private:
    void printManual();
    bool prepare();
	int init();
	// 处理解包后的消息
	int procMsg();      

	// 创建激活CCR
	int createActiveCCR();
void* getShmAddr(key_t , int );
	// 将m_sParseBuffer，前面添加StdEvent后到m_sSendBuffer
	// packType为CCR或CCA，赋值到StdEvent中的billingNbr
	// iTransFileTypeID为该消息的业务类型，与TransFileTypeID对应，赋值到StdEvent中的m_iBillingTypeID
	void prefixStdEvent(const char *sPackType = NULL, int iTransFileTypeID = 0);

    // 将最新状态插入到网元描述表stat_ne_desc
    void updateNeState(int iBsnState, int iNeState);
	void getShmServiceType(const char *, char *);

	char* m_pcopyBuffer;
};



#endif


