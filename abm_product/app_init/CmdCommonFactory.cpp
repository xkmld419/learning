
#include "CmdCommonFactory.h"
#include "CmdMsgQueue.h"
#define	_JUST_DBLINK_
#include "CmdReceiver.h"
#include "CmdSender.h"
#include "Environment.h"
CCmdCommonFactory::CCmdCommonFactory(void)
{
}

CCmdCommonFactory::~CCmdCommonFactory(void)
{
}


CmdSender* CCmdCommonFactory::CreateCmdSender(unsigned int iCmdID)
{
	CmdSender* pSender = new CmdSender();
	if(!pSender->Init(iCmdID,true))
	{
		//创建消息对列失败
		Log::log(0,"创建消息队列发送对象失败");
		delete pSender;pSender=0;
	}
	return pSender;
}

CmdReceiver* CCmdCommonFactory::CreateCmdReceiver(unsigned int iCmdID)
{
	CmdReceiver* pCmdRecv = new CmdReceiver();
	if(!pCmdRecv->Init(iCmdID,false))
	{
		//创建消息队列对象失败
		Log::log(0,"创建消息队列接受对象失败");
		delete pCmdRecv;pCmdRecv=0;
	}
	return pCmdRecv;
}

//CmdSender* CCmdCommonFactory::CreateCmdSenderByProcID(int iProcID)
//{
//	char sSqlcode[256]={0};
//	TOCIQuery qry (Environment::getDBConn());
//	unsigned iMQID=0;
//	sprintf (sSqlcode, "select MQ_ID from wf_process_mq_cmd a where a.process_id =iProcID");	
//	qry.setSQL(sSqlcode);
//	qry.open();
//	if (qry.next()) {
//		iMQID = qry.field(0).asInteger();
//	} else {
//		THROW(MBC_Process+3);
//	}
//	qry.close();
//
//	return CreateCmdSender(iMQID);
//}

CmdReceiver* CCmdCommonFactory::CreateCmdReceiverByProcID(int iProcID)
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

	char sSqlcode[256]={0};
	TOCIQuery qry (Environment::getDBConn());
	unsigned iMQID=0;
	sprintf (sSqlcode, "select MQ_ID from wf_process_mq_cmd a where a.process_id =%d ",iProcID);	
	qry.setSQL(sSqlcode);
	qry.open();
	if (qry.next()) {
		iMQID = qry.field(0).asInteger();
	} else {
		THROW(MBC_Process+3);
	}
	qry.close();

	return CreateCmdReceiver(iMQID);
}
/*
CmdReceiver* CCmdCommonFactory::CreateClientRecvSub(void)
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

	char sSqlcode[256]={0};
        unsigned int iMQID=0;
        TOCIQuery qry (Environment::getDBConn());
        sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =95273 and host_id=%d and sys_username='%s' ",iHostID,pUserName);
        qry.setSQL(sSqlcode);
        qry.open();
        if (qry.next()) {
                iMQID = qry.field(0).asInteger();
        } else {
                THROW(MBC_Process+3);
        }
        qry.close();
        return CreateCmdReceiver(iMQID);
}*/

CmdReceiver* CCmdCommonFactory::CreateClientRecv(void)
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);
	char sSqlcode[256]={0};
	unsigned int iMQID=0;
	TOCIQuery qry (Environment::getDBConn());
	sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =95272 and host_id=%d and SYS_USERNAME='%s' ",iHostID,pUserName);	
	qry.setSQL(sSqlcode);
	qry.open();
	if (qry.next()) {
		iMQID = qry.field(0).asInteger();
	} else {
		THROW(MBC_Process+3);
	}
	qry.close();
	return CreateCmdReceiver(iMQID);
}

void CCmdCommonFactory::Destory(CmdMsgQueue* pMsgQueue)
{
	if(0!= pMsgQueue)
	{
		delete pMsgQueue; pMsgQueue=0;
	}
}

//CmdReceiver* CCmdCommonFactory::CreateInitServerRecv(void)
//{
//	char sSqlcode[256]={0};
//	unsigned int iMQID=0;
//	TOCIQuery qry (Environment::getDBConn());
//	sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =9527");	
//		qry.setSQL(sSqlcode);
//	qry.open();
//	if (qry.next()) {
//		iMQID = qry.field(0).asInteger();
//	} else {
//		THROW(MBC_Process+3);
//	}
//	qry.close();
//	return CreateCmdReceiver(iMQID);
//}

CmdReceiver* CCmdCommonFactory::CreateServerRecv(void)
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);
	char sSqlcode[256]={0};
	unsigned int iMQID=0;
	TOCIQuery qry (Environment::getDBConn());
	sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =9527 and host_id=%d and SYS_USERNAME ='%s' ",iHostID,pUserName);	
	qry.setSQL(sSqlcode);
	qry.open();
	if (qry.next()) {
		iMQID = qry.field(0).asInteger();
	} else {
		THROW(MBC_Process+3);
	}
	qry.close();
	return CreateCmdReceiver(iMQID);
}
/*
CmdSender * CCmdCommonFactory::CreateServerSenderSub(void)
{
	return 0;
	
}
*/
CmdSender * CCmdCommonFactory::CreateServerSender(void)
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);
	char sSqlcode[256]={0};
	unsigned int iMQID=0;
	TOCIQuery qry (Environment::getDBConn());
	sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =95271 and host_id=%d and SYS_USERNAME='%s' ",iHostID,pUserName);	
	qry.setSQL(sSqlcode);
	qry.open();
	if (qry.next()) {
		iMQID = qry.field(0).asInteger();
	} else {
		THROW(MBC_Process+3);
	}
	qry.close();
	return CreateCmdSender(iMQID);
}

CmdSender* CCmdCommonFactory::CreateClientSender()
{
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);
	char sSqlcode[256]={0};
	unsigned int iMQID=0;
	TOCIQuery qry (Environment::getDBConn());
	sprintf (sSqlcode, "select CMD_MQ_ID from WF_APP_CMD_MQ a where a.app_id =9527 and host_id=%d and SYS_USERNAME ='%s' ",iHostID,pUserName);	
	qry.setSQL(sSqlcode);
	qry.open(); if (qry.next()) { iMQID = qry.field(0).asInteger(); } else { THROW(MBC_Process+3);
	}
	qry.close();
	return CreateCmdSender(iMQID);
}


