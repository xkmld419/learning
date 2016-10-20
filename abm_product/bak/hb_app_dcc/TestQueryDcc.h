#ifndef TEST_QUERY_DCC_H
#define TEST_QUERY_DCC_H

#include <string.h>
#include <fstream>
#include <string>
using namespace std;

#include "Process.h"
#include "Log.h"
#include "StdEvent.h"
#include "Date.h"

#include "TAvpParser.h"
#include "dcc_ParamsMgr.h"
#include "dcc_FileMgr.h"

class TestQueryDcc : public Process
{
public:
    int run();

private:
    int prepare();
    int init();

    int readAvps();
    int createRequest();

private:
    TAVPObject m_recvObj;           // TAVPObject 是一个map<string, string>的前面加了一个TAvpHead
    TAVPObject m_sendObj;
    char *m_pRecvBuffer;			// 接收缓冲区 sizeof(StdEvent) * (m_iMsgEventNum)
    char *m_pSendBuffer;			// 发送缓冲区 sizeof(StdEvent) * (m_iMsgEventNum)
    int  m_iMsgEventNum;            // 接收消息存放的最大事件数+1，读WF_MQ_ATTR表中MSG_EVENT_NUM的最大值
    char m_sOriginHost[256];                         // 发起端设备标识
    char m_sOriginRealm[256];
    char m_sDestinationRealm[256];
    char m_sAuthApplicationId[8]; 
	dccParamsMgr *m_pDccParamMgr;    

    char m_sFileName[256];
};

#endif

