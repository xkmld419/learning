#include "DccPack.h"

DEFINE_MAIN (DccPack)

#define SHM_DWA_NUM    (IpcKeyMgr::getIpcKey(m_iFlowID, "SHM_DWA_NUM"))
//#define LOCK_DWA_NUM    (IpcKeyMgr::getIpcKey(m_iFlowID, "LOCK_DWA_NUM"))

void *getShmAddr(key_t shmKey, int shmSize)
{
    int shmid;  
    if((shmid=shmget(shmKey, shmSize, SHM_R|SHM_W|IPC_EXCL|IPC_CREAT))==-1)
    {
        if(errno==EEXIST)
            shmid=shmget(shmKey,0,0);
        else
            return NULL;
    }
    return shmat(shmid, NULL, 0);
}
		
void getPack(char *buf, char *sPack, int iPackLen) {
	if (iPackLen < 0)
		return;

	unsigned char buf1[4] = {0};
	buf1[1] = buf[1];
	buf1[2] = buf[2];
	buf1[3] = buf[3];

	int uiLen = buf1[1]*65536+buf1[2]*256+buf1[3];//此为网络序转为字节序修改灵活一些
	char temp[8] = {0};
	for (unsigned int i = 0; i < uiLen; i++)
	{
		memset(temp, 0, 8);
		unsigned char c = (*(buf + i));
		sprintf(temp,"%x%x ", ((c >> 4) & 0xf), (c & 0xf)); 
		strcat(sPack, temp);
	}
}

DccPack::DccPack()
{
}

DccPack::~DccPack()
{
	delete m_pRecvBuffer;   m_pRecvBuffer = NULL;
	delete m_pSendBuffer;   m_pSendBuffer = NULL;
	delete m_pOcpMsgParser; m_pOcpMsgParser = NULL;
}

int DccPack::init()
{
	char sql[1024];
	memset (sql, 0, sizeof(sql));
	sprintf(sql, "SELECT MAX(A.MSG_EVENT_NUM) FROM WF_MQ_ATTR A");

	DEFINE_QUERY (qry);
	qry.setSQL(sql);
	qry.open();
	qry.next();
	m_iMsgEventNum = qry.field(0).asInteger();
	qry.commit();
	qry.close();
	m_pRecvBuffer = new char[sizeof(StdEvent) * (m_iMsgEventNum)];   // 接收缓冲
	m_pSendBuffer = new char[sizeof(StdEvent) * (m_iMsgEventNum)];   // 发送缓冲

	m_pOcpMsgParser = new OcpMsgParser("dictionary.xml");


	char sOriginHost[81] = {0};
	ParamDefineMgr::getParam("DCC", "ORIGIN_HOST", sOriginHost);
	if (strlen(sOriginHost) == 0)
	{
		Log::log(0, "请检查b_param_define中是否配置ORIGIN_HOST");
	}
	char sOriginRealm[81] = {0};
	if (!ParamDefineMgr::getParam("DCC", "ORIGIN_REALM", sOriginRealm))
	{
		Log::log(0, "请检查b_param_define中是否配置ORIGIN_REALM");
		return -1;
	}

	char sLocalIp[64] = {0};
	if (!ParamDefineMgr::getParam("DCC", "LOCAL_IP", sLocalIp))
	{
		Log::log(0, "请检查b_param_define中是否配置LOCAL_IP");
		return -1;
	}
	m_pOcpMsgParser->initLocalInfo(sOriginHost, sOriginRealm, sLocalIp);

	char sLogPath[125] = {0};
	if (!ParamDefineMgr::getParam("DCC", "LOG_PATH", sLogPath))
	{
		Log::log(0, "请检查b_param_define中是否配置LOG_PATH");
		return -1;
	}
	DccLog::setFileName(sLogPath, "DccPack", GetProcessID());   // 设置日志文件名

	return 0;
}

int DccPack::run()
{
	if (init() < 0)
		return -1;

	DenyInterrupt();

	char sPackBuf[61441] = {0};
	string sDelimitLine(100, '*');

    //char sTemp[32] = {0};
    int *pDWANum = (int *)getShmAddr(SHM_DWA_NUM, sizeof(int));
    //CSemaphore *pDWALock = new CSemaphore();
    //sprintf(sTemp, "%d", LOCK_DWA_NUM);
    //pDWALock->getSem (sTemp, 1, 1);    

	Log::log(0, "revertMsg run ......");
	while (!GetInterruptFlag())
	{
		memset(m_pRecvBuffer, 0, sizeof(StdEvent)*(m_iMsgEventNum));
		if (m_poReceiver->receive(m_pRecvBuffer, false) <= 0)
		{
			usleep(5000);
			continue;
		}

		StdEvent *pEvent = (StdEvent*)m_pRecvBuffer;
        	memset(m_pSendBuffer, 0, sizeof(StdEvent)*(m_iMsgEventNum));
		// 根据StdEvent中字段标识包类型不同，打包到m_pSendBuffer+sizeof(StdEvent)
		if (strcmp(pEvent->m_sBillingNBR, "CCR")==0)
		{
			m_pOcpMsgParser->createServCCR(m_pRecvBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1),
						m_pSendBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1));
			strcpy( ((StdEvent *)m_pSendBuffer)->m_sBillingNBR, "CCR");
			Log::log(0, "打包CCR-----------------------------");
		}

		else if (strcmp(pEvent->m_sBillingNBR, "CCA")==0)
		{
			m_pOcpMsgParser->createServCCA(m_pRecvBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1),
						m_pSendBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1));
			strcpy( ((StdEvent *)m_pSendBuffer)->m_sBillingNBR, "CCA");
			Log::log(0, "打包CCA-----------------------------");
		}

		else
			continue;

		string strAvpBuf;
		memset(sPackBuf, 0, 61441);
		string strLogContext = string("");
		strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
		getPack(m_pSendBuffer+sizeof(StdEvent), sPackBuf, 61441);
		strLogContext += strAvpBuf + sPackBuf + '\n' + sDelimitLine;
		DccLog::log(strLogContext.c_str());       // 记录按天日志

        //pDWALock->P();
		if (*pDWANum > 3)
		{
            //pDWALock->V();
			Log::log(0, "链路断开，本条消息被丢弃，消息内容记录在按天日志中");
			DccLog::log("链路断开，本条消息被丢弃，未发送");
			continue;
		}
        //pDWALock->P();
			
		m_poSender->send(m_pSendBuffer);
		AddTicketStat(TICKET_NORMAL);
	}
	DccLog::close();

	return 0;
}

