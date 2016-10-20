#ifndef __DCC_PACK_H
#define ___DCC_PACK_H


#include "Process.h"
#include "Log.h"
#include "interrupt.h"
#include "StdEvent.h"
#include "CSemaphore.h"
#include "SHMData.h"

#include "OcpMsgParser.h"
#include "DccLog.h"

/*接收消息到m_sRecvBuffer
根据StdEvent中字段标识消息类型，打包到m_sSendBuffer+sizeof(StdEvent)，
发送m_sSendBuffer*/
class DccPack : public Process
{
public:
	DccPack();
	~DccPack();

	int run();
private:
	int init();

	char *m_pRecvBuffer;   // 接收缓冲 sizeof(StdEvent) * (m_iMsgEventNum)
	char *m_pSendBuffer;   // 发送缓冲 sizeof(StdEvent) * (m_iMsgEventNum)
	int  m_iMsgEventNum;   // 接收消息存放的最大事件数+1，读WF_MQ_ATTR表中MSG_EVENT_NUM的最大值

	OcpMsgParser *m_pOcpMsgParser;  
};

#endif

