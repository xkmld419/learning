#include "CmdParserProc.h"
#include "CHssFileSystemTask.h"
#include "CHssParamMgrTask.h"
#include "CHssMemMgrTask.h"
#include "CHssProcessMgrTask.h"
//#include "DebugTools.h"
#include "Environment.h"
#include "CmdParserPoolMgr.h"
int	CmdParserProc::m_iAlarmLevelProc=0;
int	CmdParserProc::m_iAlarmDbLevelProc=0;
extern char ** g_argvex;
#define			CMD_PARSER_PROCE		1


#define		BEGIN_MSG_MAP()	\
void CmdParserProc::InitMsgTask(void) \
{ \
	

#define		END_MSG_MAP() \
} \


#define		ON_MSG_MAP(A,B) \
	static B*  m_p##B =0;\
	if(m_p##B==0) \
	{ \
	m_p##B = new B(); \
	m_p##B->Init(); \
	} \
m_mapTask[A]=m_p##B ; \

#define		ON_MSG_MAP_EX(A,B) \
	if(m_p##B==0) \
	{ \
	m_p##B = new B(); \
	m_p##B->Init(); \
	} \
m_mapTask[A]=m_p##B ; \


//void CmdParserProc::addMsgTask(void)
//{
//	CHbFileSystemTask* pSystemTask = new CHbFileSystemTask();
//	m_mapTask[MSG_SYSTEM_FILE_SYSTEM]=pSystemTask;
//}
//##ModelId=4BE01B670155

BEGIN_MSG_MAP()
	ON_MSG_MAP(MSG_SYSTEM_FILE_SYSTEM,CHbFileSystemTask);
	ON_MSG_MAP(MSG_PARAM_S_N,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_CHANGE,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_ACTIVE_MOUDLE_M,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_ACTIVE_MOUDLE_S,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_SHM_ATTACH,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_S_C,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_S_D,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_S_ALL,CHbParamMgrTask);
		ON_MSG_MAP_EX(MSG_STATE_CHANGE,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_STATE_CHANGE_RETURN,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_S_TIMING,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_SAVE_IT,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_NEW_VALUE_EFFECT,CHbParamMgrTask);
	ON_MSG_MAP_EX(MSG_PARAM_LOG_PATH,CHbParamMgrTask);	
	ON_MSG_MAP(MSG_MEM_CREATE,CHbMemMgrTask);
	ON_MSG_MAP_EX(MSG_MEM_DESTORY,CHbMemMgrTask);
	ON_MSG_MAP(MSG_PROCESS_STOP_SIG,CHbProcessMgrTask);
	ON_MSG_MAP_EX(MSG_PROCESS_START_SIG,CHbProcessMgrTask);
	ON_MSG_MAP_EX(MSG_GAME_OVER,CHbProcessMgrTask);
END_MSG_MAP()


CmdParserProc::CmdParserProc()
{
	m_pMsgSendRet = 0;
	m_pMsgSenderInit=0;
	m_pMsgRecverInit =0;
	m_pSemaphore=0;
}


void CmdParserProc::InitProcessIDMapMq(void)
{
	if(m_pMsgSendRet!=0)
		return ;
	DEFINE_QUERY (qry);
	qry.setSQL("select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =95272 ");
	qry.open();
	int iMQID=0;
	while(qry.next())
	{
		iMQID=qry.field(0).asInteger();
	//	m_pMsgSendRet = new CmdSender();
	}
	qry.close();
	if(iMQID==0)
	{
		Log::log(0,"没有配置任何进程对应的接受消息队列,很诡异,检查 wf_app_cmd_mq是否有配置");
	}
	m_pMsgSendRet = new CmdSender();
	if(!m_pMsgSendRet->Init(iMQID,true))
	{
		delete m_pMsgSendRet;m_pMsgSendRet=0;
		return;
	}
	return ;
}

//##ModelId=4BE01B670156
CmdParserProc::~CmdParserProc()
{
	if(m_pMsgSenderInit)
		delete m_pMsgSenderInit;
	m_pMsgSenderInit=0;

	if(m_pMsgSendRet)
		delete m_pMsgSendRet;
	m_pMsgSendRet=0;

	if(m_pMsgRecverInit)
		delete m_pMsgRecverInit;
	m_pMsgRecverInit=0;
}

bool CmdParserProc::execMsgCmd(CmdMsg* pMsg,long& lMsgID,int& iRetSize,variant& vtRet)
{
	long lKey = pMsg->GetMsgCmd();
	map<long ,CHbTaskBase*>::iterator it = m_mapTask.find(lKey);
	if(it != m_mapTask.end())
	{
		Log::log(0,"找到对应的处理任务:%d",it->first);
		//Trace::TraceMsgLog(pMsg);
		it->second->getTaskResult(pMsg,lMsgID,vtRet,&iRetSize);
		return true;
	}else
	{
		Log::log(0,"找不到到对应的处理任务");
		//Trace::TraceMsgLog(pMsg);
		vtRet="找不到到对应的处理任务";
		return false;
	}
}
//##ModelId=4BF5F5CD03DD
void CmdParserProc::start()
{
	close(1);//关闭输出
	strcpy(g_argvex[0],"CmdParser");
	CmdMsg* pMsg = CommandCom::CreateNullMsg();
	if(0== m_pMsgRecverInit)
		return ;
	bool bRet=false;
	//进程执行
	while(1)
	{
		//Log::log(0,"我还在 ");
		variant vtRet ;
		int iRetSize=0;
		long lMsgID=MSG_OP_RETURN_OK;
		if(m_pMsgRecverInit->RecvCmd(pMsg))
		{
			bRet=execMsgCmd(pMsg,lMsgID,iRetSize,vtRet);
		}else
		{
			usleep(1000);
			continue;
		}
		//long lProcID= pMsg->GetMsgDest();
		long lPid = pMsg->GetMsgDest();	
	//	map<long,int>::iterator it = m_mapProcess2MqID.find(lProcID);
		CmdMsg* pMsgRet = CommandCom::CreateNewMsg(lMsgID,
			SRC_RETURN,vtRet,iRetSize);

	//	if(it!= m_mapProcess2MqID.end())
		{
	//		if(it->second != m_uiServiceMqID)
			{
			//	m_pMsgSendRet->ChangeQueueID(lCmdID);
				m_pMsgSendRet->SendCmd(pMsgRet,lPid);
				delete pMsgRet; pMsgRet=0;
			}
		}
		if((bRet)&&(lMsgID==MSG_OP_RETURN_OK))	
		{
			CmdMsgFactory::ChangeMsgCmd(pMsg,MSG_OP_RETURN_OK|pMsg->GetMsgCmd());
			m_pMsgSenderInit->SendCmd(pMsg,(long)1);

		}
		delete pMsg;pMsg=0;
		        pMsg = CommandCom::CreateNullMsg();
	}
	delete pMsg; pMsg=0;
}

//##ModelId=4BF600E00305
int CmdParserProc::OnExit()
{
	return 0;
}

//##ModelId=4BF6379201DC
bool CmdParserProc::Init(  CmdReceiver* pRecv,CmdSender* pSender)
{
	if(0==pRecv)
		return false;
	m_pMsgRecverInit = pRecv;
	m_pMsgSenderInit = pSender;

	//m_pMsgSendRet = new CmdSender();
	
	InitProcessIDMapMq();
	InitMsgTask();
	return true;
}

