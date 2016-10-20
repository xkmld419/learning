#ifndef	_CCMD_COMMON_FACTORY_
#define	_CCMD_COMMON_FACTORY_
#include "CmdMsgQueue.h"
//#include "CmdSender.h"
//#include "CmdReceiver.h"
class CmdSender;
class CmdReceiver;
//提供消息通讯 CmdSend和 CmdRecv 的工厂类，
//赋值创建和回收CmdSend和 CmdRecv
class CCmdCommonFactory
{
public:
	CCmdCommonFactory(void);
	~CCmdCommonFactory(void);
	static CmdSender* CreateCmdSender(unsigned int iCmdID);
	static CmdReceiver* CreateCmdReceiver(unsigned int iCmdID);
	//static CmdSender* CreateCmdSenderByProcID(int iProcID);
	static CmdReceiver* CreateCmdReceiverByProcID(int iProcID);
	static void Destory(CmdMsgQueue* pMsgQueue);
	static CmdReceiver* CreateServerRecv(void);
	static CmdSender * CreateServerSender(void);
	static CmdSender* CreateClientSender(void);
	static CmdReceiver* CreateClientRecv(void);
public:

};


#endif

