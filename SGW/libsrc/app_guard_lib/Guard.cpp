/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Guard.h"
#include "Server.h"
#include "CommonMacro.h"
#include "Environment.h"

extern char **environ;

extern int         g_iShmID;
extern TSHMCMA     *g_pShmTable;
extern TRouteTable *g_pRouteTable;
extern int   g_iLocalHostID;

extern long g_lWorkFlowKey ;
extern long g_lLockWorkFlowKey ;
extern long g_lLockApplicationKey ;
extern long g_lLockClientKey ;

int GetAppIndex(int _iProcessID);
int GetSavBufIndex(int _iProcessID);

union Semnum {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int InitLockGrp( const int iKey, const short iLockNum, const ushort iInitValue ) {

    int iLockGrpID;
    short i;
    Semnum arg;

    // generate the lock using semaphore
    if ( (iLockGrpID = semget(iKey, iLockNum, PERM|IPC_EXCL|IPC_CREAT)) == -1 ) {
        if ( (iLockGrpID = semget(iKey, iLockNum, 0)) == -1 ) {
            perror( "get lock-id error" );
            return -1;
        }
    }

    // set the init-value of the semaphore
    arg.array = (ushort *)malloc( sizeof(ushort)*iLockNum );
    for (i = 0 ; i < iLockNum ; i++ )
        arg.array[i] = iInitValue;
    if (semctl(iLockGrpID, 0, SETALL, arg) == -1 ) {
        semctl( iLockGrpID, 0, IPC_RMID, 0 );
        return -1;
    }
    free(arg.array);

    return iLockGrpID;
}


void sig_chld(int signo) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
        ;
}

void catch_sigchld (int singal)
{
    int status;

    wait(&status);
}

void GetWorkFlowKey ()
{
    if (!g_lWorkFlowKey)
        g_lWorkFlowKey = IpcKeyMgr::getIpcKey (-1,"SHM_WorkFlowKey");
    if (!g_lLockWorkFlowKey)
        g_lLockWorkFlowKey = IpcKeyMgr::getIpcKey (-1,"SEM_LockWorkflow");
    if (!g_lLockApplicationKey)
        g_lLockApplicationKey = IpcKeyMgr::getIpcKey (-1,"SEM_LockApplication");
    if (!g_lLockClientKey)
        g_lLockClientKey = IpcKeyMgr::getIpcKey (-1,"SEM_LockCLient");
		Environment::getDBConn()->commit();
}

// 通讯初始化,初始化信号量
int CommunicationInit(bool bInit) 
{    
    char sAction[100];
    
    bool bCreateShmFlag = false;
    
    sprintf (sAction,"--连接--");
    
    signal(SIGCHLD,SIG_IGN);
    
    GetWorkFlowKey ();
        
    if (g_lWorkFlowKey<0 || g_lLockWorkFlowKey<0 || g_lLockApplicationKey<0 ||g_lLockClientKey<0)
    {
        char sTmp[1000];
        sprintf (sTmp,"获取MC_流程调度共享内存地址出错:\n SHM_WorkFlowKey:%ld \n"
            " SEM_LockWorkflow:%ld \n SEM_LockApplication:%ld \n SEM_LockCLient:%ld \n",
            g_lWorkFlowKey, g_lLockWorkFlowKey, g_lLockApplicationKey, g_lLockClientKey
        );
        TWfLog::log (0,sTmp);
        cout << sTmp <<endl;
        
        return -1;
    }
    
    TWfLog::log(0, "初始化MC_流程调度共享内存...");
    
    if (GetShmAddress(&g_pShmTable,g_lWorkFlowKey)<0 ) {
        
        g_iShmID = shmget(g_lWorkFlowKey,sizeof(TSHMCMA),PERM|IPC_CREAT|IPC_EXCL);
        
        if (g_iShmID <0){
            if ((g_iShmID = shmget(g_lWorkFlowKey,0,0))<0){
                TWfLog::log (0,"MC_流程调度共享内存(%ld)创建失败",g_lWorkFlowKey);
                return -1;
            }
        }
        
        g_pShmTable = (TSHMCMA *)shmat(g_iShmID,(void *)0,0);
        if ((int *)g_pShmTable == (int *)(-1)){
            TWfLog::log (0,"MC_流程调度共享内存(%ld)连接失败",g_lWorkFlowKey);
            return -1;
        }
        memset(g_pShmTable,0,sizeof(TSHMCMA));

        sprintf (sAction,"<<创建>>");
        
        bCreateShmFlag = true;
        
        // 信号量初始化
        TWfLog::log(0, "初始化信号量...");
        
        g_pShmTable->m_iMonitorLock = InitLockGrp(g_lLockWorkFlowKey,1,1);
        if (g_pShmTable->m_iMonitorLock < 0) {
            TWfLog::log(WF_FILE," 初始化信号量失败 %ld ",g_lLockWorkFlowKey);
            return -1;
        }
        g_pShmTable->m_iAppLock = InitLockGrp(g_lLockApplicationKey,1,1);
        if (g_pShmTable->m_iAppLock < 0) {
            TWfLog::log(WF_FILE," 初始化信号量失败 %ld ",g_lLockApplicationKey);
            return -1;        
        }
        g_pShmTable->m_iClientLock = InitLockGrp(g_lLockClientKey,1,1);
        if (g_pShmTable->m_iClientLock < 0) {
            TWfLog::log(WF_FILE," 初始化信号量失败 %ld ",g_lLockClientKey);
            return -1;        
        }
    }
    
    TWfLog::log (0,"MC_流程调度共享内存(%ld)%s成功!",g_lWorkFlowKey, sAction);
    cout<<"MC_流程调度共享内存("<<g_lWorkFlowKey<<")"<<sAction<<"成功!"<<endl;    
    
    if (!bInit && !bCreateShmFlag)
        return 1;
    
    // Init default value
    for (int iPos = 0; iPos < MAX_APP_NUM; iPos++) {
        g_pShmTable->AppSaveBuff[iPos].init();
        g_pShmTable->AppInfo[iPos].init();
    }
    
    return 1;
}


//////////////////////////// Class implementation  /////////////////////////

void TMsgQueueMgr::load()
{
    TProcessQueue **ppNextQueue, **ppNextProc;
    char sql[2000];
    
    m_poQueueHead = NULL;
    
    ppNextProc = &(m_poQueueHead);
    
    DEFINE_QUERY(qry);
    
    qry.close();
#ifdef __SQLITE__
    sprintf(sql,
        " select a.process_id process_id,a.billflow_id, nvl(b.mq_id,-1) mq_id, "
        "   nvl(b.max_msg_num,5000) max_msg_num, nvl(b.msg_event_num, 1) msg_event_num, nvl(c.out_process_id,-1) out_process_id "
        " from wf_process a LEFT OUTER JOIN "
        "   (select ta.mq_id, tb.max_msg_num, tb.msg_event_num, ta.process_id "
        "    from wf_process_mq ta, wf_mq_attr tb where ta.mq_id = tb.mq_id ) b, "
        "   wf_distribute_rule c "
        " where a.process_id = b.process_id and a.process_id = c.process_id "
        " and a.host_id = %d order by a.process_id ", g_iLocalHostID
    );
#else
    sprintf(sql,
        " select a.process_id process_id,a.billflow_id, nvl(b.mq_id,-1) mq_id, "
        "   nvl(b.max_msg_num,5000) max_msg_num, nvl(b.msg_event_num, 1) msg_event_num, nvl(c.out_process_id,-1) out_process_id "
        " from wf_process a, "
        "   (select ta.mq_id, tb.max_msg_num, tb.msg_event_num, ta.process_id "
        "    from wf_process_mq ta, wf_mq_attr tb where ta.mq_id = tb.mq_id ) b, "
        "   wf_distribute_rule c "
        " where a.process_id = b.process_id(+) and a.process_id = c.process_id(+) "
        " and a.host_id = %d order by a.process_id ", g_iLocalHostID
    );
#endif    
    qry.setSQL(sql);
    qry.open();
    
    while(qry.next()) {
        
        static int s_iLastProcID=0; //最后一条取出的进程标识
        
        int iProcID = qry.field("process_id").asInteger();
        int iOutProcQue = qry.field("out_process_id").asInteger();
        
        if ( iProcID != s_iLastProcID) { //> 如果 iProcID != iLastProcID, New Process
            s_iLastProcID = iProcID;
            
            *ppNextProc = new TProcessQueue;
            
            ppNextQueue = ppNextProc;
            ppNextProc  = &((*ppNextProc)->m_poNextProcess);
                        
            (*ppNextQueue)->m_iProcessID = iProcID;
            (*ppNextQueue)->m_iBillFlowID= qry.field("billflow_id").asInteger();
            (*ppNextQueue)->m_iMqID = qry.field("mq_id").asInteger();
            (*ppNextQueue)->m_iQueueSize = qry.field("max_msg_num").asInteger();
			// add by zhangch 
			(*ppNextQueue)->m_iMsgEventNum = qry.field("msg_event_num").asInteger(); 
			(*ppNextQueue)->m_iMsgEventNum = (*ppNextQueue)->m_iMsgEventNum>20 ? 20 : (*ppNextQueue)->m_iMsgEventNum;

#ifdef WF_DEBUG_
cout<<endl;
cout<<"(*ppNextQueue)->m_iProcessID  :"<<(*ppNextQueue)->m_iProcessID<<endl;
cout<<"(*ppNextQueue)->m_iBillFlowID :"<<(*ppNextQueue)->m_iBillFlowID<<endl;
cout<<"(*ppNextQueue)->m_iMqID       :"<<(*ppNextQueue)->m_iMqID<<endl;
cout<<"(*ppNextQueue)->m_iQueueSize  :"<<(*ppNextQueue)->m_iQueueSize<<endl;
cout<<endl;
#endif 
            ppNextQueue = &((*ppNextQueue)->m_poOutProcQueue);
        }        
        
        if (iOutProcQue>0) {
            *ppNextQueue = new TProcessQueue;
            (*ppNextQueue)->m_iProcessID = iOutProcQue;

#ifdef WF_DEBUG_
cout<<"(*ppNextQueue)->m_iProcessID(N):"<<(*ppNextQueue)->m_iProcessID<<endl<<endl;
#endif    
            ppNextQueue = &((*ppNextQueue)->m_poOutProcQueue);
        }
    }
    qry.close();
    
    return;
}

void TMsgQueueMgr::unload()
{
    if (m_poQueueHead) 
        delete m_poQueueHead;
        
    return;
}

TProcessQueue* TMsgQueueMgr::searchProcQueue(int _iProcID)
{
    for (TProcessQueue *pt=m_poQueueHead; pt; pt=pt->m_poNextProcess)
        if (pt->m_iProcessID == _iProcID)
            return pt;

    return NULL;
}

//创建进程所拥有的队列( 进程未配置队列,返回false)
bool TMsgQueueMgr::createSelfQueue(int _iProcID)
{
    bool bRet = false;
    
    TProcessQueue* pProcQue = searchProcQueue(_iProcID);
    
    if (pProcQue && pProcQue->m_iMqID>0) {
        MessageQueue oQueue(pProcQue->m_iMqID);
        if (oQueue.exist()) return true; //队列存在

		// modifed by zhangch
        // oQueue.open(false,true,sizeof(StdEvent),pProcQue->m_iQueueSize);
        oQueue.open(false,true,sizeof(StdEvent)*pProcQue->m_iMsgEventNum,pProcQue->m_iQueueSize);
        oQueue.close();
        bRet = true; //created.
    }

    return bRet;
}

//## 删除进程队列
bool TMsgQueueMgr::deleteSelfQueue(int _iProcID)
{
    TProcessQueue* pProcQue = searchProcQueue(_iProcID);
    
    if (pProcQue && pProcQue->m_iMqID>0) {
        MessageQueue oQueue(pProcQue->m_iMqID);
        if (!oQueue.exist()) 
            return true;
        oQueue.open();
        oQueue.remove();
    }
    
    return true;
}

//## 创建进程关联的所有队列
bool TMsgQueueMgr::createAssociateQueue(int _iProcID)
{
    createSelfQueue(_iProcID);
    
    TProcessQueue* pProcQue = searchProcQueue(_iProcID);
    
    for (; pProcQue; pProcQue=pProcQue->m_poOutProcQueue)
        createSelfQueue(pProcQue->m_iProcessID);
    
    return true;
}

//## 删除进程关联的所有队列
bool TMsgQueueMgr::deleteAssociateQueue(int _iProcID)
{
    deleteSelfQueue(_iProcID);
    
    TProcessQueue* pProcQue = searchProcQueue(_iProcID);
    
    for (;pProcQue; pProcQue=pProcQue->m_poOutProcQueue)
        deleteSelfQueue(_iProcID);
    
    return true;
}

//## 创建流程所有消息队列
bool TMsgQueueMgr::createBillFlowQueue(int _iBillFlowID)
{
    TProcessQueue* pProcQue = m_poQueueHead;
    
    for (; pProcQue; pProcQue=pProcQue->m_poNextProcess) {
        if (pProcQue->m_iBillFlowID == _iBillFlowID)
            createSelfQueue(pProcQue->m_iProcessID);
    }
    
    return true;
}

//## 删除流程所有消息队列
bool TMsgQueueMgr::deleteBillFlowQueue(int _iBillFlowID)
{
    TProcessQueue* pProcQue = m_poQueueHead;
    
    for (; pProcQue; pProcQue=pProcQue->m_poNextProcess) {
        if (pProcQue->m_iBillFlowID == _iBillFlowID)
            deleteSelfQueue(pProcQue->m_iProcessID);
    }
        
    return true;
}

//##获取队列消息数目
int TMsgQueueMgr::getMsgNumber(int _iProcessID)
{
    int iRet = -1;
    
    TProcessQueue* pProcQue = searchProcQueue(_iProcessID);
    
    if (pProcQue && pProcQue->m_iMqID>0) {
        MessageQueue oQueue(pProcQue->m_iMqID);
        if (!oQueue.exist()) return -1;
        oQueue.open();
        iRet = oQueue.getMessageNumber();
    }
    
    return iRet;
}

//##获取消息队列使用百分比
int TMsgQueueMgr::getPercent(int _iProcessID)
{
    int iRet = -1;
    
    TProcessQueue* pProcQue = searchProcQueue(_iProcessID);
    
    if (pProcQue && pProcQue->m_iMqID>0) {
        MessageQueue oQueue(pProcQue->m_iMqID);
        if (!oQueue.exist()) return -1;
        oQueue.open();
        iRet = oQueue.getOccurPercent();
    }
    
    return iRet;
}

void TMsgQueueMgr::printQueue(char sBuff[]) {
    TProcessQueue *pt;
    sBuff[0]=0;
    for (pt = m_poQueueHead; pt; pt = pt->m_poNextProcess) {
        MessageQueue oQueue(pt->m_iMqID);
        if (!oQueue.exist()) continue;
        oQueue.open();
        sprintf(sBuff,"%s ProcessID: %-9d  MsqID: %-9d  MsgNumber: %-9d  UsedPercent: %-9d \n",
            sBuff,pt->m_iProcessID, oQueue.getSysID(), oQueue.getMessageNumber(),
            oQueue.getOccurPercent() );
        
    }
}


//###################################################################

void TRouteTable::load()
{
    TServerHostInfo **ppHostInfo;
    TServerAppInfo  **ppAppInfo;
    
    DEFINE_QUERY(qry);

    m_poMsgQueue = new TMsgQueueMgr;
    
    m_poHostInfo = NULL;
    m_poAppInfo = NULL;
    ppHostInfo = &m_poHostInfo;
    ppAppInfo  = &m_poAppInfo;
    
    qry.close();
    qry.setSQL("select host_id,nvl(host_ip,'0.0.0.0') host_ip,nvl(host_name,'local') "
        " host_name from wf_hostinfo order by host_id ");
    qry.open();
    
    while( qry.next() ) {
                
        *ppHostInfo = new TServerHostInfo;
        
        (*ppHostInfo)->m_iHostID = qry.field("host_id").asInteger();
        strncpy((*ppHostInfo)->m_sHostAddr, qry.field("host_ip").asString(),15 );
        (*ppHostInfo)->m_sHostAddr[15] = 0;
        AllTrim( (*ppHostInfo)->m_sHostAddr );
        strncpy((*ppHostInfo)->m_sHostName, qry.field("host_name").asString(),HOSTNAME_MAX_LEN-1);
        (*ppHostInfo)->m_sHostName[HOSTNAME_MAX_LEN-1] = 0;
        AllTrim( (*ppHostInfo)->m_sHostName );

        #ifdef WF_DEBUG___
        cout<<"(*ppHostInfo)->m_iHostID   : "<<(*ppHostInfo)->m_iHostID<<endl;
        cout<<"(*ppHostInfo)->m_sHostAddr : "<<(*ppHostInfo)->m_sHostAddr<<endl;
        cout<<"(*ppHostInfo)->m_sHostName : "<<(*ppHostInfo)->m_sHostName<<endl;
        cout<<endl;
        #endif        
        
        ppHostInfo = &( (*ppHostInfo)->m_poNext );
    }
    
    qry.close();
    char sql[1000];
    memset(sql,0,1000);
    sprintf(sql, " select b.app_id,a.process_id,a.billflow_id,nvl(a.auto_run,0) auto_run, "
                 "   a.exec_path, b.exec_name||' '||a.parameter exec_command, "
                 "   nvl(b.app_type,0) app_type, nvl(a.state,'A') state, "
                 "   nvl(b.sql_statement,'0') sql, a.host_id, nvl(c.org_id,0) org_id, "
                 "   sys_username, nvl(auto_boot_if_abort,0) auto_boot_if_abort, "
                 "   nvl(DB_CONNECT_STR,'BILL') db_connect_str "
                 " from wf_process a, wf_application b, wf_billflow c "
                 " where a.app_id = b.app_id and a.billflow_id = c.billflow_id and a.host_id=%d" 
                 " order by a.process_id ",g_iLocalHostID/*add by jx 20110429 for 云南 小谭谭*/ );
    qry.setSQL(sql);
    qry.open();
    
    int iPreProcID = 0;
    char * pSysName = getenv("LOGNAME");
    TServerAppInfo  **ppPreAppInfo=ppAppInfo;
    
    while( qry.next()) {
        
        if(qry.field("process_id").asInteger()  == iPreProcID ){
            ////process_id不能重复，否则启停进程会错乱
            printf("严重错误：process_id(%d)重复!\n", iPreProcID);
            if( qry.field("host_id").asInteger()!=g_iLocalHostID
                || strcmp( pSysName, qry.field("sys_username").asString() )!=0  ){
                continue;
            }
            
            ppAppInfo = ppPreAppInfo;
            if( 0 == (*ppAppInfo) ){
                *ppAppInfo = new TServerAppInfo;
                ppPreAppInfo = ppAppInfo;
            }
            
        }else{
            iPreProcID = qry.field("process_id").asInteger();
        
            *ppAppInfo = new TServerAppInfo;
            
            ppPreAppInfo = ppAppInfo;
        }
        
        (*ppAppInfo)->m_iAppID      = qry.field("app_id").asInteger();
        (*ppAppInfo)->m_iProcessID  = qry.field("process_id").asInteger();
        (*ppAppInfo)->m_iBillFlowID = qry.field("billflow_id").asInteger();
        (*ppAppInfo)->m_iAutoRun    = qry.field("auto_run").asInteger();
        (*ppAppInfo)->m_iAppType    = qry.field("app_type").asInteger();
        (*ppAppInfo)->m_iHostID     = qry.field("host_id").asInteger();
        (*ppAppInfo)->m_iOrgID      = qry.field("org_id").asInteger();
        strcpy ((*ppAppInfo)->m_sSqlStatement, qry.field("sql").asString());
        
        char sTmp[2];
        strncpy( sTmp, qry.field("state").asString(),1 );
        (*ppAppInfo)->m_cState      = sTmp[0];
        
        strcpy( (*ppAppInfo)->m_sExecPath , qry.field("exec_path").asString() );
        strcpy( (*ppAppInfo)->m_sExecCommand , qry.field("exec_command").asString() );
        strcpy ((*ppAppInfo)->m_sSysUserName, qry.field("sys_username").asString());
        
        (*ppAppInfo)->m_bAutoBootWhenAbort =  qry.field("auto_boot_if_abort").asInteger();
				strcpy((*ppAppInfo)->m_sDBConnectStr ,qry.field("db_connect_str").asString());
        
        ppAppInfo = &( (*ppAppInfo)->m_poNext );
    }
    
    qry.close();
}

void TRouteTable::unload()
{
    if (m_poHostInfo) {
        delete m_poHostInfo;
        m_poHostInfo = NULL;
    }
    if (m_poAppInfo) {
        delete m_poAppInfo;
        m_poAppInfo = NULL;
    }
    if (m_poMsgQueue) {
        delete m_poMsgQueue;
        m_poMsgQueue = NULL;
    }
}

//## 根据iProcID获取进程对应TServerAppInfo地址, 找不到 return NULL
TServerAppInfo* TRouteTable::getServerAppInfo(int _iProcID)
{
    TServerAppInfo *pAppInfo;
    
    for (pAppInfo = m_poAppInfo; pAppInfo; pAppInfo = pAppInfo->m_poNext) {
        if (pAppInfo->m_iProcessID == _iProcID) {
            return pAppInfo;
        }
    }
    
    return NULL;
}
    

//## 根据hostname查找HostID, If can't found, return -1.
int TRouteTable::getHostIDByHostName(char *_hostname) 
{
    TServerHostInfo *pHostInfo;
    for (pHostInfo = m_poHostInfo; pHostInfo; pHostInfo = pHostInfo->m_poNext ) {
        if (strcmp(pHostInfo->m_sHostName,_hostname) == 0)
            return pHostInfo->m_iHostID;
    }
    return -1;
}

//## 根据 host_id 查找 hostname
bool TRouteTable::getHostNameByHostID(int _iHostID, char _sHostName[])
{
    TServerHostInfo *pHostInfo;
    
    for (pHostInfo = m_poHostInfo; pHostInfo; pHostInfo = pHostInfo->m_poNext ) {
        if (pHostInfo->m_iHostID == _iHostID) {
            strcpy (_sHostName, pHostInfo->m_sHostName);
            return true;
        }
    }
    _sHostName[0] = 0;
    return false;
}

//## 根据 host_id 查找主机信息
TServerHostInfo * TRouteTable::getHostInfoByHostID(int _iHostID)
{
    TServerHostInfo *pHostInfo;
    
    for (pHostInfo = m_poHostInfo; pHostInfo; pHostInfo = pHostInfo->m_poNext ) {
        if (pHostInfo->m_iHostID == _iHostID)
            return pHostInfo;
    }
    return 0;
}

//## 根据 _iProcID 查找进程部署的主机ID, if can't found, return -1;
int TRouteTable::getHostIDByProcessID(int _iProcID, char *_sUserName)
{
    TServerAppInfo  *pAppInfo; 
    for (pAppInfo = m_poAppInfo; pAppInfo; pAppInfo = pAppInfo->m_poNext ) {
        if (pAppInfo->m_iProcessID == _iProcID) {
            strcpy (_sUserName, pAppInfo->m_sSysUserName);
            return pAppInfo->m_iHostID;
        }
    }
    return -1;
}

//## 启动进程 (0-失败, 1-成功)
int TRouteTable::startApp(int _iProcID,int _iProcessLogID, int *pFailedReason/* = 0x0*/)
{
    TServerAppInfo *pt = getServerAppInfo(_iProcID);	    
    
    if (pFailedReason) *pFailedReason = 0;
    
    if (!pt) {
        TWfLog::log(0,"TRouteTable::startApp, 运行表格中未找到 ProcessID = %d 对应的进程!",_iProcID);
        return false;
    }

    int iIdx,iRes;
    iRes = g_pShmTable->getIdleAppInfo(_iProcID,iIdx);
    if (iRes == 0){
        TWfLog::log(0,"TRouteTable::startApp ProcessID:%d , Can't find idle AppInfo",_iProcID);
        return false;
    }
    if (iRes < 0){
        TWfLog::log(0,"TRouteTable::startApp ProcessID:%d , 检测到进程已经在运行",_iProcID);
        if (pFailedReason) *pFailedReason = iRes;
        return false;
    }

    g_pShmTable->AppInfo[iIdx].init();
    g_pShmTable->AppInfo[iIdx].m_iProcessID = _iProcID;
    g_pShmTable->AppInfo[iIdx].m_iBillFlowID = pt->m_iBillFlowID;
    g_pShmTable->AppInfo[iIdx].m_iAppType= pt->m_iAppType;
    g_pShmTable->AppInfo[iIdx].m_iProcessLogID = _iProcessLogID;
    g_pShmTable->changeState(_iProcID,WF_START);

    switch(pt->m_iAppType) {
        
        case 0: //##  后台程序            
            
            /*g_pRouteTable->*/
            createAssociMsgQueue(_iProcID); //确保当前进程涉及的消息队列都建立起来
            
            pt->setState(WF_START); // 置为'启动'状态, 等待已启动的进程调用 ConnectMonitor();

            g_pShmTable->AppInfo[iIdx].m_iAppPID = startCommand(pt);
            
            if (g_pShmTable->AppInfo[iIdx].m_iAppPID <= 0){ //执行未成功
                TWfLog::log(WF_FILE,"TRouteTable::startApp err after start Command: %s",pt->m_sExecCommand);
                g_pShmTable->AppInfo[iIdx].init();
                return false;
            }
            
            break;
                        
        case 1: // PL/SQL块	    	    		
	    			pt->setState(WF_RUNNING);
	    			if (g_pShmTable)
            		g_pShmTable->changeState(_iProcID,WF_RUNNING);

	    			//Pprintf_ex(0,true,0,&(g_pShmTable->AppInfo[iIdx]),"应用进程启动！(ProcessID:%d)",pt->m_iProcessID);

            g_pShmTable->AppInfo[iIdx].m_iAppPID = startSQLCommand(pt,&(g_pShmTable->AppInfo[iIdx]));
            
            if (g_pShmTable->AppInfo[iIdx].m_iAppPID <= 0){ //执行未成功
                TWfLog::log(WF_FILE,"TRouteTable::start_SqlCommand err  ProcessID:%d",pt->m_iProcessID);		
                g_pShmTable->AppInfo[iIdx].init();
                return false;
            }			            
            break;        
        
        case 2: // 前台应用程序
            TWfLog::log(WF_FILE,"TRouteTable::startApp, 前台程序不能在后台启动");
            return false;
        
        case 3: // 后台Unix shell脚本
            
		
            pt->setState(WF_START);
            pt->setState(WF_RUNNING);
	    			if (g_pShmTable)
            		g_pShmTable->changeState(_iProcID,WF_RUNNING);

	   				//Pprintf_ex(0,true,0,&(g_pShmTable->AppInfo[iIdx]),"应用进程启动！(ProcessID:%d)",pt->m_iProcessID);
		
            g_pShmTable->AppInfo[iIdx].m_iAppPID = startShell(pt,&(g_pShmTable->AppInfo[iIdx]));
			
            //sprintf(sMsgBuilt,"[MESSAGE]:应用进程执行完毕！(ProcessID:%d)",_iProcID);
	    //g_pShmTable->AppInfo[iIdx].pushInfo(sMsgBuilt);
            break;

        default:
            TWfLog::log(WF_FILE,"TRouteTable::startApp, unknown AppType:%d",pt->m_iAppType);
            return false;
    }
        
    return true;
}

//## 执行编译出的可执行代码
int TRouteTable::startCommand(TServerAppInfo *pServApp)
{
    int iPos = 0,iForkRet = 0;

    char sPath[80],sCommand[80],*argv[20];
    char sString[80],sString2[80],sString3[80],sString4[80],*pt;
    char sExecName[MAX_APP_PARAM_LEN];
    strcpy(sPath,pServApp->m_sExecPath);    
    strcpy(sExecName,pServApp->m_sExecCommand);
    
    TWfLog::log(WF_FILE,"StartCommand: %s \n ExecPath: %s",sExecName,sPath);
    pt = strtok(sExecName," \n");
    argv[iPos++] = strdup(pt);    
    strcpy(sCommand,pt);
    while((pt = strtok(NULL," \n")) != NULL){
        argv[iPos] = strdup(pt);
        TWfLog::log(WF_FILE,"argv[%d],%s",iPos,argv[iPos]);
        iPos++;
    }
    argv[iPos] = NULL;
    
    iForkRet = fork();
    switch(iForkRet){
        
        case 0:
            if (strlen(sPath)) {
                chdir(sPath);
            }
            else {
                TWfLog::log(WF_FILE,"Not give ExecPath. sCommand:%s",sCommand);
            }
                
            usleep(10000);
            Environment::getDBConn(true); //数据库要求子进程需要重新连接数据库
            
            sprintf(sString,"PROCESS_ID=%d",pServApp->m_iProcessID);
            putenv(sString);            
            sprintf(sString2,"BILLFLOW_ID=%d",pServApp->m_iBillFlowID);
            putenv(sString2);
            sprintf(sString3,"APP_ID=%d",pServApp->m_iAppID);
            putenv(sString3);
            sprintf(sString4,"USE_WF=1");
            putenv(sString4);
            
            execve(sCommand,argv,environ);
            
            exit(0);
            break;
            
        case -1:
            perror("fork");
            TWfLog::log(WF_FILE,"start_Command: fork err");
            cout<<"start_Command: fork err"<<endl;
            break;
            
        default:
            Environment::getDBConn(true);
            TWfLog::log(0,"child process create ,pid:%d",iForkRet);
            cout<<"\n\n\n\n====>> Child process create ,pid:"<<iForkRet<<endl;
            break;
    }
    
    for (int i=0; i<iPos; i++) {
        if (argv[i]) { cout<<"argv["<<i<<"] = "<<argv[i]<<endl; delete argv[i]; }
        else break;
    }    
    return iForkRet;
}

//## 执行PLSQL
int TRouteTable::startSQLCommand(TServerAppInfo *pServApp,TAppInfo *pAppInfo)
{
    int iForkRet = 0;
    char sSqlCommand[MAX_SQL_LEN],sProcessID[10]={0};
    
    TWfLog::log(WF_FILE,"startSQL_Command: ProcessID-> %d",pServApp->m_iProcessID);
    
    strcpy(sSqlCommand,pServApp->m_sSqlStatement);
    sprintf(sProcessID,"%d",pServApp->m_iProcessID);
    replace(sSqlCommand,"%PROCESS_ID",sProcessID);

    iForkRet = fork();
    
    switch (iForkRet) {
        
        case 0: // 进入子进程
        	{
	    			//TOCIDatabase * pDbConn = new TOCIDatabase();
	    try {				
		
		        		//Environment::connectDB(pDbConn,pServApp->m_sDBConnectStr);
		        		Environment::getDBConn(true); //数据库要求子进程需要重新连接数据库
								//TOCIQuery qry(pDbConn);	
								DEFINE_QUERY(qry);
		
                usleep(10000); //?                
                //TWfLog::log(WF_FILE,sSqlCommand);
                qry.close();
                qry.setSQL(sSqlCommand);
                qry.execute();
                qry.commit();
                qry.close();
		
								//pDbConn->disconnect();
								//delete pDbConn;
								Environment::getDBConn(true);		
			                
								//Pprintf_ex(0,true,0,pAppInfo,"应用进程执行完毕！(ProcessID:%d)",pServApp->m_iProcessID);
								pServApp->setState(WF_NORMAL);
                if (g_pShmTable)
                    g_pShmTable->changeState(pServApp->m_iProcessID,WF_NORMAL);
            }
            catch (TOCIException &e) {		
                TWfLog::log(WF_FILE,"Execute sql err: \n%s \n%s ",e.getErrMsg(),e.getErrSrc());
                try {			
										//pDbConn->disconnect();
										//delete pDbConn;
										Environment::getDBConn(true);				
										//Pprintf_ex(9011,true,0,pAppInfo,"应用进程执行出错!(ProcessID:%d,sqlcode:%d)",pServApp->m_iProcessID,e.getErrCode());
                    pServApp->setState(WF_ABORT);
                    if (g_pShmTable)
                        g_pShmTable->changeState(pServApp->m_iProcessID,WF_ABORT);
                }
                catch (TOCIException &e2) {
										//pDbConn->disconnect();
										//delete pDbConn;		
                    TWfLog::log(WF_FILE,"pServApp->setState  or  g_pShmTable->changeState or Ins wf_alter_log err:"
                        "%s \n%s ",e2.getErrMsg(),e2.getErrSrc());
			
                }
            }
            exit(0);
        	}
        case -1:
            perror("fork");
            TWfLog::log(WF_FILE,"Start_SqlCommand: fork err. ProcessID:%d",pServApp->m_iProcessID);
            break;
            
        default:
            Environment::getDBConn(true);
            TWfLog::log(WF_FILE,"Sql child process create ,pid:%d",iForkRet);
            break;
    }
    
    return iForkRet;
}


//## 执行Unix Shell脚本
int TRouteTable::startShell(TServerAppInfo *pServApp,TAppInfo *pAppInfo)
{
    int iPos = 0,iForkRet = 0;

    char sPath[80],sCommand[80],*argv[20];
    char *pt;
    char sExecName[MAX_APP_PARAM_LEN];
    strcpy(sPath,pServApp->m_sExecPath);    
    strcpy(sExecName,pServApp->m_sExecCommand);
    
    TWfLog::log(WF_FILE,"start_Shell: sExecCommand-> %s\n ExecPath: %s",sExecName,sPath);
    pt = strtok(sExecName," \n");
    argv[iPos++] = strdup(pt);    
    strcpy(sCommand,pt);
    while((pt = strtok(NULL," \n")) != NULL){
        argv[iPos] = strdup(pt);
        TWfLog::log(WF_FILE,"argv[%d],%s",iPos,argv[iPos]);
        iPos++;
    }
    argv[iPos] = NULL;
    
    iForkRet = fork();
    switch(iForkRet){
        
        case 0:
            if (strlen(sPath))
                chdir(sPath);    
            usleep(10000);
            
            Environment::getDBConn(true); //数据库要求子进程需要重新连接数据库	   
            
            // 此处默认脚本都执行成功
            pServApp->setState(WF_NORMAL);
            g_pShmTable->changeState(pServApp->m_iProcessID,WF_NORMAL);

            execvp(sCommand,argv);
			
	    			//Pprintf_ex(0,true,0,pAppInfo,"应用进程执行完毕！(ProcessID:%d)",pServApp->m_iProcessID);
                    
            exit(0);
            break;
            
        case -1:
            perror("fork");
            TWfLog::log(WF_FILE,"start_Command: fork err");
            break;
            
        default:
            Environment::getDBConn(true);
            TWfLog::log(WF_FILE,"child process create ,pid:%d",iForkRet);
            break;
    }
    
    for (int i=0; i<iPos; i++) {
        if (argv[i]) { cout<<"argv["<<i<<"] = "<<argv[i]<<endl; delete argv[i]; }
        else break;
    }    
    return iForkRet;
}


//## 停止进程 (0-失败, 1-成功)
int TRouteTable::stopApp(int _iProcID)
{
	//char sMsgBuilt[MAX_APPINFO__LEN] = {0};
	TAppInfo* pt = g_pShmTable->getAppInfoByProcID(_iProcID);

    if (!pt) {
        TWfLog::log(WF_FILE,"TRouteTable::stopApp, 运行实例AppInfo中未找到 ProcessID = %d 对应的进程!",_iProcID);
        return 0;
    }
    
    TServerAppInfo *ptServApp = getServerAppInfo(_iProcID);
    if (!ptServApp) {
        TWfLog::log(WF_FILE,"TRouteTable::stopApp, 运行表格ServAppInfo中未找到 ProcessID = %d 对应的进程!",_iProcID);
        return 0;
    }
    
    if (ptServApp->m_iAppType <= 3) {
        TWfLog::log(WF_FILE,"Stop process:%d, appPID:%d ",_iProcID,pt->m_iAppPID);
        if (ptServApp->m_iAppType == 0) {//后台编译出的可执行代码程序
            
            if (pt->checkAlive()) {            
                pt->setState ('B'); //杀之前置"中间态" 
                ptServApp->setState('B');
                kill(pt->m_iAppPID,SIGUSR1);
                usleep(100000);
            }
            else {
                pt->setState ('X');
                ptServApp->setState('X');
            }
        }
        else //非编译代码, 可能是sql脚本执行进程、Shell脚本进程        
        {
            kill(pt->m_iAppPID,9);	    
	    			//Pprintf_ex(0,true,0,pt,"应用进程退出,但是产生的数据库进程无法停止!(ProcessID:%d)",pt->m_iProcessID);
	    			pt->setState(WF_STOP);
	    			ptServApp->setState(WF_STOP);
        }
        return 1;
    }
    else if (ptServApp->m_iAppType == 4) {
        TWfLog::log(WF_FILE,"TRouteTable::stopApp, 前台程序不能在后台停止");
        return 0;
    }
    else {
        TWfLog::log(WF_FILE,"TRouteTable::stopApp, unknown AppType:%d",ptServApp->m_iAppType);
        return 0;
    }
}

//## 强行改变状态(0-失败, 1-成功)
int TRouteTable::changeState(int _iProcID,char _cState)
{
    TServerAppInfo *pt_server = getServerAppInfo(_iProcID);
    if (!pt_server) {
        TWfLog::log(WF_FILE,"TRouteTable::changeStatus, 运行表格中未找到 ProcessID = %d 对应的进程!",_iProcID);
        return 0;
    }
    pt_server->setState(_cState);
    
    return 1;
}

void TRouteTable::killAllApp ()
{    
    char sUserName[100];
    int iLocalHostID;
    
    memset(sUserName,0,100);
    gethostname(sUserName,100);
    
    iLocalHostID = getHostIDByHostName(sUserName);

    for (int i=0; i<MAX_APP_NUM; i++) {
        if (g_pShmTable->AppInfo[i].m_cState==WF_START || 
            g_pShmTable->AppInfo[i].m_cState==WF_RUNNING )
        {//该进程已经启动中
            char sSysUserName[100];
            
            int iHostID = getHostIDByProcessID(g_pShmTable->AppInfo[i].m_iProcessID, sSysUserName);
            
            if (iHostID!=iLocalHostID || strcmp(sSysUserName,sUserName))
                continue;
            
            if (g_pShmTable->AppInfo[i].checkAlive()) {
                
                kill (g_pShmTable->AppInfo[i].m_iAppPID,9);
                
                changeState(g_pShmTable->AppInfo[i].m_iProcessID, WF_ABORT);
                
                TWfLog::log (WF_FILE,"Child application (ProcessID:%d, SysPID:%d) killed!",
                    g_pShmTable->AppInfo[i].m_iProcessID, g_pShmTable->AppInfo[i].m_iAppPID);
            }
        }
    }
}

//##################################################################

//检查应用进程的状态
//如果进程状态异常(状态running, 进程实际不存在), 则置Abort状态
//如果进程配置为异常需要重启, 则立即启动该进程
void CheckApplication (int iHostID, char sUserName[])
{
    static int iCnt=0;
    static char sMsg[100];
    
    static time_t last=0;
    static time_t now=0;
    
    time (&now);
    if ((now - last) <= 3)
        return;
    
    last = now;
    
    if (!g_pRouteTable) return;
    
    for (int iPos=0; iPos<MAX_APP_NUM; iPos++)
    {
        //如果不是本机、本用户进程,则不处理
        TServerAppInfo *pt = g_pRouteTable->getServerAppInfo ( \
                    g_pShmTable->AppInfo[iPos].m_iProcessID );
        
        if (!pt) continue;
        
        if (pt->m_iHostID != iHostID  || strcmp (pt->m_sSysUserName, sUserName))
            continue;
        
        if (g_pShmTable->AppInfo[iPos].m_cState=='R' && !g_pShmTable->AppInfo[iPos].checkAlive())
        {
            g_pShmTable->AppInfo[iPos].setState ('X');
           //add 20090722 未修改wf_process中的状态，下次登录前台monitor后看到的状态
           //与后台实际状态不符，增加修改表记录
            pt->setState(WF_ABORT);
            sprintf (sMsg,"[Warning]守护进程检测到进程(%d)异常退出!", g_pShmTable->AppInfo[iPos].m_iProcessID);
            g_pShmTable->AppInfo[iPos].pushInfo(sMsg);
            continue; //检测到异常, 不立即启动
        }
        
        if (g_pShmTable->AppInfo[iPos].m_cState == 'X' && pt->m_bAutoBootWhenAbort
            && !g_pShmTable->AppInfo[iPos].checkAlive()
            /*考虑逻辑上可能性: 应用进程刚DisconnectMonitor(Abort),尚未结束进程的情况下
            立即启动该进程会不成功, 因为有文件锁限制一个应用进程只能运行一个实例*/ 
        )
        {
            int iProLogID = TLogProcessMgr::insertLogProcess(pt->m_iProcessID,"start", 0);
            int iRet = g_pRouteTable->startApp(pt->m_iProcessID, iProLogID);
            if (iRet == 0) {
                TWfLog::log(WF_FILE, "[Warning]自动启动异常结束的进程(%s)<ProcessID:%d>失败!",
                    pt->m_sExecCommand, pt->m_iProcessID);
            }
        }       
    }
}


//响应应用程序请求注册登录/注销
void ResponseAppConnect()
{
    TAppReqInfo *pAppRq = &(g_pShmTable->AppReqInfo);
    
    if (pAppRq->m_iStatus == APP_REG_REGISTER) { //应用程序登录
        
        int iIdx = GetAppIndex(pAppRq->m_iProcessID);
        int iSavIdx = GetSavBufIndex(pAppRq->m_iProcessID);
        
        if (iIdx<0 ) {
            TWfLog::log(WF_FILE,"App Login error. ProcessID:%d ",pAppRq->m_iProcessID);
            pAppRq->m_iStatus = APP_REG_REJECT;            
            return;
        }

        if (iSavIdx < 0) {
            TWfLog::log(WF_FILE,"App Login error. 进程ID:%d 分配 '最后处理事件保留缓冲' 失败!",
                pAppRq->m_iProcessID);
            pAppRq->m_iStatus = APP_REG_REJECT;            
            return;
        }
                
        //置 AppInfo(运行实例) 状态
        //g_pShmTable->AppInfo[iIdx].m_cState = WF_RUNNING;
        g_pShmTable->changeState(pAppRq->m_iProcessID,WF_RUNNING);
        g_pShmTable->AppInfo[iIdx].m_iAppPID = pAppRq->m_iAppPID;
        
        //置 ServAppInfo(运行表格) 状态
        int iRes = g_pRouteTable->changeState(pAppRq->m_iProcessID,WF_RUNNING);
        if (iRes == 0)
            TWfLog::log(WF_FILE,"App Login error. changeState.");
        
        //>> 更新 wf_log_process(app_pid,start_date,state)
        cout<<"\niIdx: "<<iIdx<<endl;
        cout<<"iSavIdx: "<<iSavIdx<<endl;
        cout<<"AppInfo["<<iIdx<<"].m_iProcessLogID : "<< g_pShmTable->AppInfo[iIdx].m_iProcessLogID <<endl;
        cout<<"AppInfo["<<iIdx<<"].m_iAppPID : "<< g_pShmTable->AppInfo[iIdx].m_iAppPID <<endl;
        TLogProcessMgr::updateLogProcess(g_pShmTable->AppInfo[iIdx].m_iProcessLogID,
                WF_RUNNING,g_pShmTable->AppInfo[iIdx].m_iAppPID);
        
        pAppRq->m_iIndex = iIdx;
        pAppRq->m_iSaveBufIdx = iSavIdx;
        TWfLog::log(WF_FILE,"App Login sucess! ");
        pAppRq->m_iStatus = APP_REG_ACCEPT; //注册成功
    }
    else if (pAppRq->m_iStatus == APP_REG_UNREGISTER ) { //应用程序注销
        
        int iIdx;
        for (iIdx = 0; iIdx<MAX_APP_NUM; iIdx++ ) {
            if (g_pShmTable->AppInfo[iIdx].m_iProcessID == pAppRq->m_iProcessID)
                break;
        }
        if (iIdx == MAX_APP_NUM ) { //未找到对应的进程
            TWfLog::log(WF_FILE,"App logout error. ProcessID:%d not found in ShmTable",
                pAppRq->m_iProcessID);
            return;
        }
        
        if (pAppRq->m_iAppPID != g_pShmTable->AppInfo[iIdx].m_iAppPID) {
        //系统进程ID对应出错
            TWfLog::log(WF_FILE,"App Logout error. ProcessID:%d, AppPID:%d; PID in SHM:%d",
                pAppRq->m_iProcessID,pAppRq->m_iAppPID, g_pShmTable->AppInfo[iIdx].m_iAppPID );
            return;
        }
        
        //置 AppInfo(运行实例) 状态
        g_pShmTable->changeState(pAppRq->m_iProcessID,pAppRq->m_cState);
        
        //置 ServAppInfo(运行表格) 状态
        int iRes = g_pRouteTable->changeState(pAppRq->m_iProcessID,pAppRq->m_cState);
        if (iRes == 0)
            TWfLog::log(WF_FILE,"App Logout error. changeState.");
        
        
        TWfLog::log(WF_FILE,"App LogOut sucess! ");
        pAppRq->m_iStatus = APP_REG_ACCEPT;
        
        return;
    }    
    return;
}

// indexID or -1
int GetAppIndex(int _iProcID)
{
    for (int idx = 0; idx<MAX_APP_NUM; idx++) {
        if (g_pShmTable->AppInfo[idx].m_iProcessID == _iProcID)
            return idx;
    }
    
    //考虑进程不从守护进程启动的情况
    if (g_pShmTable && g_pRouteTable) {

        int iIdx,iRes;
        
        TServerAppInfo *pt = g_pRouteTable->getServerAppInfo(_iProcID);
        if (!pt) {
            TWfLog::log(0,"GetAppIndex Err., 运行表格中未找到 ProcessID = %d 对应的进程!",_iProcID);
            return -1;
        }
        
        iRes = g_pShmTable->getIdleAppInfo(_iProcID,iIdx);
        if (iRes == 0){
            TWfLog::log(0,"GetAppIndex Err.  ProcessID:%d , Can't find idle AppInfo",_iProcID);
            return -1;
        }
        if (iRes < 0){
            TWfLog::log(0,"GetAppIndex Err.  ProcessID:%d , 检测到进程已经在运行",_iProcID);
            return -1;
        }
        
        g_pShmTable->AppInfo[iIdx].init();
        g_pShmTable->AppInfo[iIdx].m_iProcessID = _iProcID;
        g_pShmTable->AppInfo[iIdx].m_iBillFlowID = pt->m_iBillFlowID;
        g_pShmTable->AppInfo[iIdx].m_iProcessLogID = 0;
        //g_pShmTable->changeState(_iProcID,WF_START);
        
        return iIdx;
    }
    else 
        return -1;
}

int GetSavBufIndex(int _iProcessID)
{
    int iSavBufIdx = -1;
    
    for (iSavBufIdx = 0; iSavBufIdx<MAX_APP_NUM; iSavBufIdx++) {
        if (g_pShmTable->AppSaveBuff[iSavBufIdx].getProcessID() == _iProcessID)
        {
            //g_pShmTable->AppSaveBuff[iSavBufIdx].setProcessID(_iProcessID);
            return iSavBufIdx;
        }
    }
    
    for (iSavBufIdx = 0; iSavBufIdx<MAX_APP_NUM; iSavBufIdx++) {
        if (g_pShmTable->AppSaveBuff[iSavBufIdx].getProcessID() == -1)
        {
            g_pShmTable->AppSaveBuff[iSavBufIdx].setProcessID(_iProcessID);
            return iSavBufIdx;
        }
    }

    return -1;
}

void CalcTreatRate()
{
    static time_t tNow;
    time (&tNow);
    
    for (int iIdx = 0; iIdx<MAX_APP_NUM; iIdx++ ) {
        
        TAppInfo *pt = &(g_pShmTable->AppInfo[iIdx]);
        
        if (   pt->m_iProcessID > 0                 //有效进程ID
            && pt->m_cState == WF_RUNNING           //进程运行中
            && (tNow - pt->m_tLatestCalcTime) >= 10 //间隔超过10秒后计算
        ){
            float fProcAdded = (float)(pt->m_lProcessCnt - pt->m_lLatestProcCnt);
            int   iTimeAdded = tNow - pt->m_tLatestCalcTime;
            pt->m_fTreatRate = (float)fProcAdded/iTimeAdded;
            pt->m_tLatestCalcTime = tNow;
            pt->m_lLatestProcCnt = pt->m_lProcessCnt;
        }
    }
}

///////////////////////////////////////////////////////////////


