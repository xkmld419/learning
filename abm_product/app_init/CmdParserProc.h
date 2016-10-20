#ifndef CMDPARSERPROC_H_HEADER_INCLUDED_B41FA7EC
#define CMDPARSERPROC_H_HEADER_INCLUDED_B41FA7EC
#include "InDependProcess.h"
#include "Log.h"
#include "CmdReceiver.h"
#include "CmdSender.h"
#include "HssTask.h"
#include "CommandCom.h"
#include <map>
using namespace std;
#define	MAX_SEM_KEY_LEN			32			//信号量名字最大长度


#define	DECLARE_MSG_MAP() \
protected:	\
map<long ,CHbTaskBase*> m_mapTask;	\
void InitMsgTask(void); \



class CmdSender;
//##ModelId=4BE01B670147
class CmdParserProc 
{
  public:
    //##ModelId=4BE01B670155
    CmdParserProc();

    //##ModelId=4BE01B670156
    virtual ~CmdParserProc();
    //##ModelId=4BF5F5CD03DD
    void start();
    //##ModelId=4BF6379201DC
    bool Init(  CmdReceiver* pRecv,CmdSender* pSenderRet);

	bool execMsgCmd(CmdMsg* pMsg,long&lMsgID,int& iRetSize,variant& vtRet);
  private:
    //进程退出时响应
    //##ModelId=4BF600E00305
    virtual int OnExit();
    //##ModelId=4BF6365F0091
    CmdSender* m_pMsgSenderInit;

	//
	CmdSender* m_pMsgSendRet;

    //##ModelId=4BF6368602CE
    CmdReceiver* m_pMsgRecverInit;

	//0号服务进程的接收消息队列
	unsigned int m_uiServiceMqID;

	CSemaphore* m_pSemaphore;

	map<long,int> m_mapProcess2MqID;

	void InitProcessIDMapMq(void);
	static	int	m_iAlarmLevelProc ;
	static	int	m_iAlarmDbLevelProc ;

	DECLARE_MSG_MAP()
};



#endif /* CMDPARSERPROC_H_HEADER_INCLUDED_B41FA7EC */
