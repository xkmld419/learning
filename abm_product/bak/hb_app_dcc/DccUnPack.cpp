#include "DccUnPack.h"

DEFINE_MAIN (DccUnPack)

#define SHM_SERVICETYPE    (IpcKeyMgr::getIpcKey(Process::m_iFlowID, "SHM_SERVICETYPE"))
/*
void* getShmAddr(key_t shmKey, int shmSize)
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
*/


void getPack(char *buf, char *sPack, int iPackLen)
{
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

DccUnPack::DccUnPack()
{
}

DccUnPack::~DccUnPack()
{
    //m_mapTransContext.clear();
    delete []m_ContextIds;
    m_ContextIds = NULL;
    delete m_pRecvBuffer;
    m_pRecvBuffer = NULL;
    delete m_pSendBuffer;
    m_pSendBuffer = NULL;
    delete m_pOcpMsgParser;
    m_pOcpMsgParser = NULL;
    delete m_pFileToDccMgr;
    m_pFileToDccMgr = NULL;
}

void DccUnPack::printManual()
{
    Log::log (0,
              "********\n"
              "dccUnPack 调用方法：\n"
              "-o taskID   任务号\n");
}


bool DccUnPack::prepare()
{
    m_iTaskId = -1;

    for (int i=1; i<g_argc; ++i)
    {
        if (g_argv[i][0] != '-')
        {
            printManual();
            return false;
        }
        switch (g_argv[i][1] | 0x20)
        {
        case 'o':
            i++;
            m_iTaskId = atoi(g_argv[i]);
            break;
        default:
            printManual();
            return false;
        }
    }

    if (m_iTaskId == -1)
    {
        Log::log(0, "请在运行参数指定执行任务ID");
        printManual();
        return false;
    }
    return true;
}

int DccUnPack::init()
{
    char sql[1024];
    DEFINE_QUERY (qry);

    memset (sql, 0, sizeof(sql));
    sprintf(sql, "SELECT SUM(1) FROM TRANS_FILE_SERVICE_CONTEXT WHERE TRANS_FILE_TYPE_ID<>0 AND DCC_TASK_ID=%d", m_iTaskId);
    qry.setSQL(sql);
    qry.open();
    qry.next();
    m_iContextNum = qry.field(0).asInteger();
    qry.commit();
    qry.close();

    m_ContextIds = new char[m_iContextNum][256];

    int i = 0;
    memset (sql, 0, sizeof(sql));
    sprintf(sql, "SELECT A.TRANS_FILE_TYPE_ID, A.SERVICE_CONTEXT FROM TRANS_FILE_SERVICE_CONTEXT A WHERE A.TRANS_FILE_TYPE_ID<>0 AND A.DCC_TASK_ID=%d", m_iTaskId);
    qry.setSQL(sql);
    qry.open();
    while (qry.next())
    {
        // m_mapTransContext[qry.field(1).asString()] = qry.field(0).asInteger();
        strcpy(m_ContextIds[i++], qry.field(1).asString());
    }
    qry.commit();
    qry.close();

    m_pFileToDccMgr = FileToDccMgr::getInstance();
    m_pFileToDccMgr->init();

    ParamDefineMgr::getParam("DCC", "ORIGIN_HOST", m_sOriginHost);
    if (strlen(m_sOriginHost) == 0)
    {
        Log::log(0, "请检查b_param_define中是否配置ORIGIN_HOST");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "SELECT  A.SERVICE_CONTEXT,B.DCC_NE_HOST FROM TRANS_FILE_SERVICE_CONTEXT a,TRANS_DCC_TASK B "
            " WHERE A.DCC_TASK_ID=B.DCC_TASK_ID AND A.TRANS_FILE_TYPE_ID=0 AND B.DCC_TASK_ID=%d", m_iTaskId);
    qry.setSQL(sql);
    qry.open();
    qry.next();
    strcpy(m_sActiveContext, qry.field(0).asString());
    strcpy(m_sNeHost,qry.field(1).asString());
    qry.commit();
    qry.close();

    memset (sql, 0, sizeof(sql));
    sprintf(sql, "SELECT MAX(A.MSG_EVENT_NUM) FROM WF_MQ_ATTR A");
    qry.setSQL(sql);
    qry.open();
    qry.next();
    m_iMsgEventNum = qry.field(0).asInteger();
    qry.commit();
    qry.close();

    m_pRecvBuffer = new char[sizeof(StdEvent) * m_iMsgEventNum];			// 接收缓冲区
    m_pSendBuffer = new char[sizeof(StdEvent) * m_iMsgEventNum];			// 发送缓冲区
    m_pcopyBuffer = new char[sizeof(StdEvent) * m_iMsgEventNum];	   //复制一份

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

    m_pDccParamMgr = new dccParamsMgr;
    m_pDccParamMgr->getDccHeadValue(m_sNeHost);

    m_nTime = 10;          // active CCA 超时时长缺省为10
    char buffer[256];
    if (ParamDefineMgr::getParam("DCC", "ACTIVE_TIMEOUT", buffer))
        m_nTime = atoi(buffer);

    m_isActiveCCAOK = 1;

    char sLogPath[125] = {0};
    if (!ParamDefineMgr::getParam("DCC", "LOG_PATH", sLogPath))
    {
        Log::log(0, "请检查b_param_define中是否配置LOG_PATH");
        return -1;
    }
    DccLog::setFileName(sLogPath, "DccUnPack", GetProcessID());   // 设置日志文件名


    m_shmServiceType =(char*)m_pOcpMsgParser->getshm();
    return 0;
}

int DccUnPack::run()
{
    if (prepare() < 0)
        return -1;

    if (init() < 0)
        return -1;

    DenyInterrupt();

    Date dDate;
    Log::log(0, "dccProc run ......");
    while (!GetInterruptFlag())
    {
        if (m_isActiveCCAOK == 0)    // 激活CCA未返回
        {
            dDate.getCurDate();     // 获取当前时间
            if (dDate.diffSec(m_dDate) > m_nTime)      // 判断是否超时
            {
                /* 发送激活CCR*/
                createActiveCCR();
                prefixStdEvent("ACTIVECCR");
                m_poSender->send(m_pSendBuffer);
                Log::log(0,"超时未收到激活CCA包，重新发送激活CCR包");
                m_dDate.getCurDate();    // 记录重发时间
            }
        }

        memset(m_pRecvBuffer, 0, sizeof(StdEvent)*m_iMsgEventNum);
        if (m_poReceiver->receive(m_pRecvBuffer, false) <= 0)
        {
            usleep(5000);
            continue;
        }
         //复制一份保存
         memset(m_pcopyBuffer,0x00,sizeof(StdEvent)*m_iMsgEventNum);
	memcpy(m_pcopyBuffer,m_pRecvBuffer,sizeof(StdEvent)*m_iMsgEventNum);

        // 检查激活CCR是否连续发送失败，若失败下次不判断超时
        StdEvent *pEvent = (StdEvent *)m_pRecvBuffer;
        if (strcmp(pEvent->m_sBillingNBR, "REBOOT") == 0)
        {
            Log::log(0,"激活CCR连续发送失败，不再判断超时");
            m_isActiveCCAOK = 1;
            continue;
        }

        memset(m_sServiceContextId, 0, sizeof(m_sServiceContextId));
        memset(m_pSendBuffer, 0, sizeof(StdEvent) * m_iMsgEventNum);

        // 解包 m_pSendBuffer+sizeof(StdEvent)中存放解包结果或对应A包的打包结果，返回原消息类型，Service-Context-Id
        m_iMsgType = m_pOcpMsgParser->parserPack(m_pRecvBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1),
                     m_pSendBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1), m_sServiceContextId);

        if (procMsg() < 0)
        {
            Log::log(0, "消息无法解析");
            // add 20100601
            // 记录网元状态“Online故障”到stat_ne_desc
            updateNeState(ONLINE, ONLINE_FAULT);
        }

    }
    DccLog::close();

    return 0;
}

int DccUnPack::procMsg()
{
    int transFileTypeID = 0;

    // 若Service-Context-Id!=0，设置对应的业务类型到transFileTypeID
    // 第一次返回的CCA包为激活请求回复，包含Service-Context-Id
    if (strlen(m_sServiceContextId)!=0)
    {
        transFileTypeID = m_pFileToDccMgr->getFileTypeByServiceContext(m_sServiceContextId);
    }

    string strAvpBuf;
    char sPackBuf[61441] = {0};
    string sDelimitLine(100, '*');
    string strLogContext = string("");      // 存放按天日志内容

std::string tmp;
    DEFINE_QUERY (qry);
    switch (m_iMsgType)
    {
    case CEA:
        // 判断CER是否失败
        if (atoi(m_pSendBuffer+sizeof(StdEvent)) != 2001)     // CER发送失败
        {
            Log::log(0, "CER发送失败，发送信息到dccTran");
            memset(m_pSendBuffer, 0, sizeof(StdEvent) * m_iMsgEventNum);
            prefixStdEvent("CERFAILED");
            m_poSender->send(m_pSendBuffer);
            break;
        }
        Log::log(0,"收到CEA，CER发送成功");
        strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
        getPack(m_pRecvBuffer+sizeof(StdEvent), sPackBuf, 61441);
        strLogContext += strAvpBuf + sPackBuf + '\n' + sDelimitLine;
        DccLog::log(strLogContext.c_str());       // 记录按天日志

        // 调用打包接口生成激活CCR到m_pSendBuffer+sizeof(StdEvent)
        createActiveCCR();
        strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
        memset(sPackBuf, 0, sizeof(sPackBuf));
        strLogContext = string("");
        getPack(m_pSendBuffer+sizeof(StdEvent), sPackBuf, 61441);
        strLogContext += strAvpBuf + sPackBuf + '\n' + sDelimitLine;
        DccLog::log(strLogContext.c_str());       // 记录按天日志

        prefixStdEvent("ACTIVECCR");
        m_poSender->send(m_pSendBuffer);
        Log::log(0,"已发送激活CCR包");
        m_isActiveCCAOK = 0;  // 激活CCA未返回
        m_dDate.getCurDate();  // 记录发送时间
        AddTicketStat(TICKET_NORMAL);

        break;
    case CCR1:
        // 解析出来的结果前加StdEvent到m_pSendBuffer
        strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
        getPack(m_pRecvBuffer+sizeof(StdEvent), sPackBuf, 61441);
        strLogContext += strAvpBuf + sPackBuf + '\n' + sDelimitLine;
        DccLog::log(strLogContext.c_str());       // 记录按天日志
        prefixStdEvent("CCR", transFileTypeID);

        //modified by zhufengsheng 2011.03.17
        //如果是新业务发源码
        //如果是静态数据类的业务
        //发解包之后的
        //新的业务会找不到^^^符号
        //新业务后面拼凑不起话单
        tmp = m_pSendBuffer + sizeof(StdEvent);
        if(tmp.find("^^^") == std::string::npos)
        {
            //发源码给dccUnPack继续解包
            strcpy(((StdEvent*)(m_pcopyBuffer))->m_sBillingNBR, "RAW");
            Log::log(0,"收到新业务CCR,发送源码给解包程序继续解包");
            m_poSender->send(m_pcopyBuffer);
        }
        else
        {
            //发解码之后的
            //strcpy(((StdEvent*)(m_pSendBuffer))->m_sCalledAreaCode,"OLD");
            Log::log(0,"解析CCR包成功静态数据类");
            m_poSender->send(m_pSendBuffer);
        }

        AddTicketStat(TICKET_NORMAL);


        //这里入日志表2010-12-07 by 朱逢胜

        try
        {
            std::string sContext = m_pSendBuffer+sizeof(StdEvent);
            if(sContext.find("^^^")!=std::string::npos)
            {
                sContext =sContext.substr(sContext.find("^^^")+3);
            }
            std::string sSessionId;
            m_pOcpMsgParser->getAvpValue("Session-Id",sSessionId);
            long iHopByhop =  m_pOcpMsgParser->getHopByHop();

            char szSql[2048]= {0};
            sprintf(szSql,"insert into dcc_trans_log(sessionid,hopbyhop,recivemsgtype,in_sys_time,indb_time,RECEIVE_CONTEXT)  \
                                    values('%s',%ld,'CCR',to_timestamp('%s','yyyymmddhh24missff'),systimestamp,'%s')",
                    sSessionId.c_str(),iHopByhop,((StdEvent*)(m_pRecvBuffer))->m_sReservedField[0],sContext.c_str());
            qry.close();
            qry.setSQL(szSql);
            qry.execute();
            qry.commit();
            qry.close();
        }
        catch ( TOCIException &e )
        {
            std::ostringstream os_log;
            os_log.str( "" );
            os_log << "***********************sql error*********************** \n" << std::endl << e.getErrCode()
            << std::endl <<e.GetErrMsg() << std::endl << e.getErrSrc() << std::endl;
            Log::log( 0,os_log.str().c_str());
        }
        //end of add by 朱逢胜2010-12-07
        break;

    case CCA:
        strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
        getPack(m_pRecvBuffer+sizeof(StdEvent), sPackBuf, 61441);
        strLogContext += strAvpBuf + sPackBuf + '\n' + sDelimitLine;
        DccLog::log(strLogContext.c_str());       // 记录按天日志
        AddTicketStat(TICKET_NORMAL);

        if (strlen(m_sServiceContextId)!=0)   // 激活CCA，需判断是否失败
        {
            char *p=strchr(m_pSendBuffer+sizeof(StdEvent),'|');
            int iResultCode=0;
            int iServiceResultCode=-1;
            if (p)
            {
                iResultCode=atoi(m_pSendBuffer+sizeof(StdEvent));
                iServiceResultCode = atoi(p+1);
            }
            Log::log(0,"收到激活CCA的result_code:%d,serivce_result_code=%d,ServiceContextID = %s,",iResultCode,iServiceResultCode,m_sServiceContextId);
            if (!p || iResultCode!=2001 ||  iServiceResultCode!=0)    // 激活失败，重新发送激活CCR
            {
                createActiveCCR();
                prefixStdEvent("ACTIVECCR");
                m_poSender->send(m_pSendBuffer);
                Log::log(0,"激活失败，重新发送激活CCR包");
                m_dDate.getCurDate();    // 记录重发时间
            }
            else
            {
                Log::log(0,"激活成功");
                m_isActiveCCAOK = 1;   // 激活成功
                // add 20100601
                // 记录网元状态“Online_正常”到stat_ne_desc
                updateNeState(ONLINE, ONLINE_NORMAL);
            }
        }
        else
        {
            // 解析出来的结果前加StdEvent到m_pSendBuffer
            prefixStdEvent("CCA", transFileTypeID);
//------------------------------------------------------------------------------
            std::string sessionid = m_pOcpMsgParser->getSessionid();
            char serviceType[SHM_ITEM_LEN+1]= {0};
            getShmServiceType(sessionid.c_str(),serviceType);
            //找不到共享内存中的serviceType标示是老业务
            if(strcmp(serviceType,"")==0)
            {
                //发送解包后的字符串
                //strcpy(((StdEvent*)(m_pSendBuffer))->m_sBillingNBR,"RAW");
                m_poSender->send(m_pSendBuffer);
                Log::log(0,"解析CCA包成功静态数据类业务");
            }
            else
            {
                //发送源码
                strcpy(((StdEvent*)(m_pcopyBuffer))->m_sBillingNBR,"RAW");
                m_poSender->send(m_pcopyBuffer);
	       Log::log(0,"收到新业务CCA,发送源码给解包程序继续解包");
            }
//------------------------------------------------------------------------------


            //这里更新dcc_trans_log modified by 朱逢胜2010.12.07
            std::string sContext = m_pSendBuffer+sizeof(StdEvent);

            try
            {
                char szSql[2048]= {0};
                sprintf(szSql,"update dcc_trans_log t set t.recivemsgtype = 'CCA',t.in_sys_time = to_timestamp('%s','yyyymmddhh24missff') ,  \
                                         RECEIVE_CONTEXT = '%s' where t.sessionid = '%s' and t.hopbyhop = %ld",
                        ((StdEvent*)(m_pRecvBuffer))->m_sReservedField[0],
                        sContext.c_str(),
                        m_pOcpMsgParser->getSessionid().c_str(),
                        m_pOcpMsgParser->getHopByHop());
                qry.close();
                qry.setSQL(szSql);
                qry.execute();
                qry.commit();
                qry.close();
            }
            catch ( TOCIException &e )
            {
                std::ostringstream os_log;
                os_log.str( "" );
                os_log << "***********************sql error*********************** \n" << std::endl
                << e.getErrCode()<< std::endl <<e.GetErrMsg() << std::endl << e.getErrSrc() << std::endl;
                Log::log( 0,os_log.str().c_str());
            }
        }
        break;

    case DWR:
        // 解析后生成的A包前加StdEvent到m_pSendBuffer
        prefixStdEvent("DWA");
        m_poSender->send(m_pSendBuffer);
        Log::log(0,"收到DWR包，返回DWA包");
        break;
    case DPR:
        // 解析后生成的A包前加StdEvent到m_pSendBuffer
        prefixStdEvent("DPA");
        m_poSender->send(m_pSendBuffer);
        Log::log(0,"收到DPR包，返回DPA包");
        break;

    default:
        // 丢弃
        strAvpBuf=m_pOcpMsgParser->getAllAvp(true);
        getPack(m_pRecvBuffer+sizeof(StdEvent), sPackBuf, 61441);
        DccLog::log("%s%s\n%s", strAvpBuf.c_str(), sPackBuf, sDelimitLine.c_str());       // 记录按天日志
        break;
    }

    return 0;
}

int DccUnPack::createActiveCCR()
{
    if (m_pDccParamMgr == NULL)
        m_pDccParamMgr = new dccParamsMgr;

    m_pDccParamMgr->getDccHeadValue(m_sNeHost);

    /*给m_ActiveCCRArgv赋值*/
    char buff[256] = {0};
    memset(m_ActiveCCRArgv, 0, sizeof(m_ActiveCCRArgv));
    sprintf(m_ActiveCCRArgv, "%s|%d|%d", m_pDccParamMgr->m_sSessionID, m_pDccParamMgr->m_iHopByHop, m_pDccParamMgr->m_iEndToEnd);

//---------------------------------------------------------------------------
/*
    char *p = strrchr(m_pDccParamMgr->m_sSessionID, ';');
    unsigned int hh = atol(p+1);
    memset(m_shmServiceType+(hh%SHM_ITEM_NUM)*SHM_ITEM_LEN, 0, SHM_ITEM_LEN);
    strcpy(m_shmServiceType+(hh%SHM_ITEM_NUM)*SHM_ITEM_LEN, "Active.Net-Ctrl");
    */
//---------------------------------------------------------------------------


    if (!ParamDefineMgr::getParam("DCC", "DESTINATION_REALM", buff))
    {
        Log::log(0, "请检查b_param_define中是否配置DESTINATION_REALM");
        return -1;
    }
    strcat(m_ActiveCCRArgv, "|");
    strcat(m_ActiveCCRArgv, buff);
    memset(buff, 0, sizeof(buff));
    if (!ParamDefineMgr::getParam("DCC", "AUTH_APPLICATION_ID", buff))
    {
        Log::log(0, "请检查b_param_define中是否配置AUTH_APPLICATION_ID");
        return -1;
    }
    strcat(m_ActiveCCRArgv, "|");
    strcat(m_ActiveCCRArgv, buff);
    strcat(m_ActiveCCRArgv, "|");
    strcat(m_ActiveCCRArgv, m_sActiveContext);
    strcat(m_ActiveCCRArgv, "|4|0");

    return m_pOcpMsgParser->createActiveCCR(m_ActiveCCRArgv, m_ContextIds, m_iContextNum,
                                            m_pSendBuffer+sizeof(StdEvent), sizeof(StdEvent)*(m_iMsgEventNum-1));
}

void DccUnPack::prefixStdEvent(const char *sPackType, int iTransFileTypeID)
{
    StdEvent *pEvent = (StdEvent*)m_pSendBuffer;

    if (sPackType != NULL)
        strcpy(pEvent->m_sBillingNBR, sPackType);
    pEvent->m_iBillingTypeID = iTransFileTypeID;

}


void DccUnPack::updateNeState(int iBsnState, int iNeState)
{
    char sql[1024];
    DEFINE_QUERY (qry);

    if (iNeState == ONLINE_NORMAL)
    {
        memset (sql, 0, sizeof(sql));
        sprintf(sql, "INSERT INTO STAT_NE_DESC (ORIGIN_HOST, TASK_ID, BSN_STATE, NE_STATE, SYSUPTIME, STATE_TIME) "
                "VALUES ('%s', '%d', %d, %d, SYSDATE, SYSDATE)", m_sOriginHost, m_iTaskId, iBsnState, iNeState);
    }
    else
    {
        char sSysUpTime[15] = {0};
        memset (sql, 0, sizeof(sql));
        sprintf(sql, "SELECT TO_CHAR(SYSUPTIME, 'YYYYMMDDHH24MISS') FROM STAT_NE_DESC WHERE BSN_STATE = '0' "
                "AND STATE_TIME IN (SELECT MAX(STATE_TIME) FROM STAT_NE_DESC)");
        qry.setSQL(sql);
        qry.open();
        if (qry.next())
        {
            strcpy(sSysUpTime, qry.field(0).asString());
        }
        qry.commit();
        qry.close();

        memset (sql, 0, sizeof(sql));
        if (strlen(sSysUpTime) == 0)
        {
            sprintf(sql, "INSERT INTO STAT_NE_DESC (ORIGIN_HOST, TASK_ID, BSN_STATE, NE_STATE, SYSUPTIME, STATE_TIME) "
                    "VALUES ('%s', '%d', %d, %d, NULL, SYSDATE)", m_sOriginHost, m_iTaskId, iBsnState, iNeState);
        }
        else
        {
            sprintf(sql, "INSERT INTO STAT_NE_DESC (ORIGIN_HOST, TASK_ID, BSN_STATE, NE_STATE, SYSUPTIME, STATE_TIME) "
                    "VALUES ('%s', '%d', %d, %d, to_date('%s', 'YYYYMMDDHH24MISS'), SYSDATE)",
                    m_sOriginHost, m_iTaskId, iBsnState, iNeState, sSysUpTime);
        }
    }
    qry.setSQL(sql);
    qry.execute();
    qry.commit();
    qry.close();
}


void DccUnPack::getShmServiceType(const char *sessionId, char *serviceType)
{
    char *p = strrchr((char*)sessionId, ';');
    unsigned int hh = atol(p+1);
    //m_shmServiceType = (char *)getShmAddr(SHM_SERVICETYPE, SHM_ITEM_NUM * SHM_ITEM_LEN);
    strcpy(serviceType, m_shmServiceType+(hh%SHM_ITEM_NUM)*SHM_ITEM_LEN);
    //modified bu zhufengsheng 2011.03.17
}

void* DccUnPack::getShmAddr(key_t shmKey, int shmSize)
{
    int shmid;
    if((shmid=shmget(shmKey, shmSize, SHM_R|SHM_W|IPC_EXCL|IPC_CREAT))==-1)
    {
        if(errno==EEXIST)
            shmid=shmget(shmKey,0,0);
        else
            return NULL;
    }
         void* p = NULL;
		 p = shmat(shmid, NULL, 0);
	return  p;
    
}
