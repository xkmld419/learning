/*VER: 3*/ 
#include <time.h>
#include "mntapi.h"
#include "GuardMain.h"
#include "Log.h"
#include "Environment.h"
#include "ParamDefineMgr.h"
#include "Process.h"

#ifdef _STAT_FILE_LOG

#include "Directory.h"

const int STAT_LOG_FILE_DELAY_SEC = 10; //文件创建超过10秒则强制输出

#endif
#ifndef DISCARD_EVENT_TYPE_ID
#define DISCARD_EVENT_TYPE_ID 9999
#endif

#define MONITOR_CLOSE

#define FORMAT_STRING_LENGTH	200

extern char ** g_argv;

TSHMCMA *   g_pShmTable = NULL;
TAppInfo *  g_pAppInfo = NULL;

static THeadInfo *pInfoHead = NULL;
static TProcessInfo *pCurProc = NULL;
static int g_iRegistered;
static int g_iProcessID = 0;
static int g_iBillflowID;


//static int g_bUseWf = 0;
static TAppSaveBuff *g_poAppSaveBuff = NULL;
static int shm_id;
static void *pMemHead = NULL;

//监控共享内存ID(在表b_ipc_cfg中定义)
long g_lBillMntKey = 0; 
long g_lWorkFlowKey = 0;
long g_lLockWorkFlowKey = 0;
long g_lLockApplicationKey = 0;
long g_lLockClientKey = 0;

const int PRICING_APP_ID = 10;
//获取挂死文件上次已发送的话单条数
int GetLastSendTickets(int iFileID)
{
	if(!pCurProc)
		return 0;
	if(pCurProc->iFileID == iFileID)
	{
		return pCurProc->iSendTickets;
	}
	return 0;
}
//增加正在处理的文件已发送的话单数
int AddSendTickets(int iFileID,int iTickets)
{
	if(!pCurProc)
		return 0;
	if(pCurProc->iFileID == iFileID) //原文件累计
	{
		pCurProc->iSendTickets += iTickets;
	}
	else
	{
		pCurProc->iFileID = iFileID;  //新文件重新计数
		pCurProc->iSendTickets = iTickets;
	}
	return 0;
}
//获取上次处理的文件ID
int GetLastFileID()
{
	if(!pCurProc)
		return 0;
	return pCurProc->iFileID;
}
void SetProcHead(THeadInfo* pHeadInfo1)
{
	pInfoHead=pHeadInfo1;
	 pCurProc = &pInfoHead->ProcInfo;
}

int ConnectTerminalMonitor (int iProcID)
{
    int i;
    char *env;

    if (iProcID == -1) {
        env = getenv ("PROCESS_ID");
        if (env != NULL) {
            iProcID = atoi (env);
            printf ("[MESSAGE]: PROCESS_ID = [%d]\n", iProcID);
        } else {
            printf ("[MESSAGE]: No PROCESS_ID specified in environment\n");
            return -1;
        }
    }    

    if (AttachShm (0, (void **)&pInfoHead) < 0) {
        printf ("[MESSAGE]: Not ConnectTerminalMonitor!\n");
        return -1;
    }
    
    MntAPI::m_pProcInfoHead = &pInfoHead->ProcInfo;
    MntAPI::m_poInfoHead = pInfoHead;
    
    pCurProc = &pInfoHead->ProcInfo;
    for (i=0; i<pInfoHead->iProcNum; i++) {

        if (pCurProc->iProcessID == iProcID)
            break;
        pCurProc++;
    }

    if (i >= pInfoHead->iProcNum) {
        pCurProc = NULL;
        return -1;
    }

    time ((time_t *)&pCurProc->lLoginTime);
    
    pCurProc->tSaveTime = pCurProc->lLoginTime;
    pCurProc->lSaveTickets = 0;
    
    pCurProc->iAllTickets = 0;
    pCurProc->iNormalTickets = 0;
    pCurProc->iErrTickets = 0;
    pCurProc->iBlackTickets = 0;
    pCurProc->iOtherTickets = 0;

    pCurProc->iSysPID = getpid ();

    pCurProc->iState = ST_RUNNING;

    return 0;

}

int DisconnectTerminalMonitor (int iState)
{
    Log::log (0, "%s(ProcID:%d) into DisconnectTerminalMonitor()",g_argv[0], g_iProcessID);
    if (pCurProc) {
        time ((time_t *)&pCurProc->lLoginTime);
        pCurProc->iState = iState;
    }

    DetachShm ();
    pCurProc = NULL;
    
    return 0;
}

int AddTicketStat (int iType, int iNum)
{
    if (!pCurProc)
        return 0;

    switch (iType) {
    case TICKET_NORMAL:
        pCurProc->iNormalTickets += iNum;
        break;
    case TICKET_ERROR:
        pCurProc->iErrTickets += iNum;
        break;
    case TICKET_BLACK:
        pCurProc->iBlackTickets += iNum;
        break;
    case TICKET_OTHER:
        pCurProc->iOtherTickets += iNum;
        break;
    default:
        pCurProc->iOtherTickets += iNum;
        break;
    }

    pCurProc->iAllTickets += iNum;
    
    return pCurProc->iAllTickets;
}

int SetProcState (int iState)
{
    pCurProc->iState = iState;
    return 0;
}


/*
 *函数
 *  GetBillFlowID
 *功能
 *  返回当前进程的组标识
 */
int GetBillFlowID()
{
    char * env;
    if (!pCurProc){
            if( (env=(char *)getenv("BILLFLOW_ID") )!=NULL)
                    return  atoi(env);
            else
                    return 0;
    }

    return pCurProc->iBillFlowID;
}


static bool GetBillMntKey ()
{
    g_lBillMntKey = IpcKeyMgr::getIpcKey(-1,"SHM_BillMntKey");
    Environment::getDBConn()->commit();
    if (g_lBillMntKey >=0)
        return true;
    
    TWfLog::log (0,"获取共享内存键出错(SHM_BillMntKey)");
    
    return false;   
}

int CreateShm (long lSize, void **pRet)
{
    if (!g_lBillMntKey) {
        if (!GetBillMntKey ())
            return -1;
    }
    
    shm_id = shmget (g_lBillMntKey, lSize, 0660|IPC_EXCL|IPC_CREAT);
    if (shm_id < 0) {
        return -1;
    }

    pMemHead = shmat (shm_id, 0, 0);
    if (pMemHead == NULL) {
        return -1;
    }

    *pRet = (void *)pMemHead;
    return 0;

}

int AttachShm (long lSize, void **pRet)
{
    if (!g_lBillMntKey) {
        if (!GetBillMntKey ())
            return -1;
    }
    
    //add by yks 2007.07.27
    if (pMemHead)
        DetachShm ();
    
    shm_id = shmget (g_lBillMntKey, lSize, 0660);
    if (shm_id < 0) {
        return -1;
    }

    pMemHead = shmat (shm_id, 0, 0);
    if (pMemHead == NULL) {
        return -1;
    }

    *pRet = (void *)pMemHead;
    return 0;
}

int DetachShm ()
{
    shmdt ((char *)pMemHead);
    pMemHead = NULL;
    return 0;
}

int DestroyShm ()
{
    if (!g_lBillMntKey) {
        if (!GetBillMntKey ())
            return -1;
    }
    
    shm_id = shmget (g_lBillMntKey, 0, 0660);
    if (shm_id < 0) {
        return -1;
    }

    shmctl (shm_id, IPC_RMID, 0);
    return 0;
}

/////////////////////////////////////////////////////////////////////


void TreatSingleQuotes(char src[])
{
    char sTemp[2000] = {0};
    char *ptTmp = &(sTemp[0]);
    char *ptSrc = &(src[0]);
    int iLen=0;
    for ( ; *ptSrc; ptSrc=ptSrc+1) {
        if (*ptSrc == '\'') {
            *ptTmp='\'';
            ptTmp = ptTmp+1;
            iLen++;
            *ptTmp='\'';
            ptTmp = ptTmp+1;
            iLen++;
        }
        else {
            *ptTmp = *ptSrc;
            ptTmp = ptTmp+1;
            iLen++;
        }
        if (iLen>=999) {
           sTemp[999]=0;
           break;
        }
    }
    strcpy(src,sTemp);
}

bool ConnectMonitor(int iProcID)
{
    cout<<endl;
    
    //char *pt;
    char sMsg[100];
    
    int iShmID;
    if (iProcID == -1) {    
        if ((g_iProcessID=GetProcessID()) == -1)
            return false;
    }
    else 
        g_iProcessID = iProcID;        
    
    usleep(100000);
    
    if (!g_lWorkFlowKey) {
        g_lWorkFlowKey = IpcKeyMgr::getIpcKey (-1,"SHM_WorkFlowKey");
        Environment::getDBConn()->commit();
        if (g_lWorkFlowKey<0) {
            TWfLog::log (0,"获取共享内存地址失败(SHM_WorkFlowKey)");
            return false;
        }
    }
    
    if ((iShmID = shmget(g_lWorkFlowKey,0,0))<0){
        perror("shmget");
        return false;
    }
    
    g_pShmTable = (TSHMCMA *)shmat(iShmID,(void *)0,0);
        
    if (-1 == (long)g_pShmTable){
        perror("shmat");
        return false;
    }

    cout<<"[MESSAGE]: Process:"<<g_iProcessID<<" Monitor Connecting ..."<<endl;
    
    LockSafe(g_pShmTable->m_iAppLock,0);
    
    cout<<endl<<"locked..."<<endl;
    
    g_pShmTable->AppReqInfo.m_iProcessID = g_iProcessID;
    g_pShmTable->AppReqInfo.m_iAppPID = getpid();
    g_pShmTable->AppReqInfo.m_iStatus = APP_REG_REGISTER;
    
    sprintf (sMsg, "守护进程拒绝应用进程连接监控的请求");
    while( g_pShmTable->AppReqInfo.m_iStatus == APP_REG_REGISTER)
    {
        static int iCount = 0;
        if (++iCount > 1000) {
            g_pShmTable->AppReqInfo.m_iStatus = APP_REG_REJECT;
            sprintf (sMsg, "守护进程未能相应进程连接监控的请求,可能GuardMain进程不存在!");
            break;
        }
        usleep(10000);
    }
    
    if (g_pShmTable->AppReqInfo.m_iStatus != APP_REG_ACCEPT){
        cout<<"[MESSAGE]: "<<sMsg<<endl;
        UnlockSafe(g_pShmTable->m_iAppLock,0);
        return false;
    }
    
    // get GuardMain msg.
    g_poAppSaveBuff = &( g_pShmTable->AppSaveBuff[g_pShmTable->AppReqInfo.m_iSaveBufIdx]);
    
    g_pAppInfo = &( g_pShmTable->AppInfo[g_pShmTable->AppReqInfo.m_iIndex]);
        
    g_iRegistered = 1;
    
    g_iBillflowID = g_pAppInfo->m_iBillFlowID;
    
    UnlockSafe(g_pShmTable->m_iAppLock,0);

   // Pprintf(0,1,0,"应用进程启动！(ProcessID:%d)",g_iProcessID); 
    
    return true;
}

//## 应用程序退出监控
bool DisConnectMonitor(char _cMode)
{
    
    if (g_iRegistered == 0)
        return -1;
    
    /*
    if (g_bUseWf != 1) {
        cout<<"[MESSAGE]: Do not need Disconnect WorkFlow."<<endl;
        return true;
    }
    */

    Log::log (0, "退出工作流监控!");
    TWfLog::log (0, "退出工作流监控!");
    cout<<endl<<" 退出工作流监控 ..."<<endl;
        
    LockSafe(g_pShmTable->m_iAppLock,0);
    
    g_pShmTable->AppReqInfo.m_iProcessID = g_iProcessID;
    
    if (g_pAppInfo)
        g_pShmTable->AppReqInfo.m_iAppPID = g_pAppInfo->m_iAppPID;
        
    g_pShmTable->AppReqInfo.m_cState = _cMode;
    g_pShmTable->AppReqInfo.m_iStatus = APP_REG_UNREGISTER;
    
    while( g_pShmTable->AppReqInfo.m_iStatus == APP_REG_UNREGISTER ) {
        static int i = 0;
        usleep(10000);
        if (i++>1000)
            break;
    }
    
    g_iRegistered = 0;
    UnlockSafe(g_pShmTable->m_iAppLock,0);
    shmdt( (char*)g_pShmTable );
    
    g_pAppInfo = 0x0;
    
    cout<<"[MESSAGE]: Process:"<<g_iProcessID<<" Monitor Disconnected . "<<endl<<endl;
    return true;
}


/*
输出应用进程日志、告警信息到前台调度界面
    iAlertCode == 0时 表示非告警信息
        (AlertCode 在表BILL.WF_ALERT_DEFINE 中有解释,各模块增加告警代码时,
        需要在CommonMacro.h这个头文件和数据库表中增加相应的记录)
    bInsertDB: 是否需要入告警日志表(wf_alert_log)

*/

void PutPetriRet(int iRet, char *sRetInfo)
{
    if (g_pAppInfo) {
        g_pAppInfo->putRetInfo(iRet, sRetInfo );

    }
}

void ClearPetriRet( )
{
    if (g_pAppInfo) {
        g_pAppInfo->putRetInfo(0, "" );

    }
}



bool Pprintf(int iAlertCode,bool bInsertDB,int iFileID,char const *fmt,...)
{
    va_list argptr;
    char sMsgSrc[1024],sMsgBuilt[1024];

    /*    如果数据库没有成功连接， 强制不入库*******要讨论，by xueqt */
    if (!(DB_CONNECTED)) bInsertDB = false;
    
    va_start(argptr, fmt);
    vsprintf(sMsgSrc,fmt, argptr);
    va_end(argptr);

    if (iAlertCode) {
        sprintf(sMsgBuilt,"[WARNING]: 告警编码:%d |",iAlertCode);
    } else {
        sprintf(sMsgBuilt,"[MESSAGE]: ");
    }

    if (iFileID>0) {
        sprintf(sMsgBuilt,"%s 处理文件ID:%d ",sMsgBuilt,iFileID);
    }

    sprintf(sMsgBuilt,"%s%s",sMsgBuilt,sMsgSrc);
    
    //替换sMsgSrc中的单引号
    TreatSingleQuotes(sMsgSrc);
    
    sMsgBuilt[MAX_APPINFO__LEN-1]=0;
    
    int iProcLogID=0;
        
    if (g_pAppInfo) {
        g_pAppInfo->pushInfo(sMsgBuilt);
        iProcLogID = g_pAppInfo->m_iProcessLogID;
    }
    
    if (bInsertDB) {
        char sql[2048];
        DEFINE_QUERY(qry);
        qry.close();
        qry.setSQL("select seq_alert_log_id.nextval log_id from dual");
        qry.open();         
        qry.next();         
        int iLogID = qry.field("log_id").asInteger();
 
        qry.close();
        sprintf(sql,
          " insert into wf_alert_log(alert_log_id,process_log_id,msg_date,msg_time, \n"
          " process_id,billflow_id,file_id,alert_code,alert_content) values ( \n"
          " %d,%d,to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss'), \n"
          " %d,%d,%d,%d,'%s' )",iLogID,iProcLogID,g_iProcessID,
          g_iBillflowID,iFileID,iAlertCode,sMsgSrc);

        qry.setSQL(sql);
        qry.execute();
        qry.commit();
        qry.close();
    }
    
    return true;      
}

bool Pprintf_ex(int iAlertCode,bool bInsertDB,int iFileID,TAppInfo *pAppInfo,char const *fmt,...)
{
	va_list argptr;
    	char sMsgSrc[1024],sMsgBuilt[1024];
    
	va_start(argptr, fmt);
    	vsprintf(sMsgSrc,fmt, argptr);
    	va_end(argptr);

    	if (iAlertCode) {
        	sprintf(sMsgBuilt,"[WARNING]: 告警编码:%d |",iAlertCode);
    	} else {
        	sprintf(sMsgBuilt,"[MESSAGE]: ");
    	}

    	if (iFileID>0) {
        	sprintf(sMsgBuilt,"%s 处理文件ID:%d ",sMsgBuilt,iFileID);
    	}

    	sprintf(sMsgBuilt,"%s%s",sMsgBuilt,sMsgSrc);
    
    	//替换sMsgSrc中的单引号
    	TreatSingleQuotes(sMsgSrc);

	sMsgBuilt[MAX_APPINFO__LEN-1]=0;
    
    	int iProcLogID=0;

	pAppInfo->pushInfo(sMsgBuilt);
	iProcLogID = pAppInfo->m_iProcessLogID;

	if (bInsertDB) {
        char sql[2048];
        DEFINE_QUERY(qry);
        qry.close();
        qry.setSQL("select seq_alert_log_id.nextval log_id from dual");
        qry.open();         
        qry.next();         
        int iLogID = qry.field("log_id").asInteger();
 
        qry.close();
        sprintf(sql,
          " insert into wf_alert_log(alert_log_id,process_log_id,msg_date,msg_time, \n"
          " process_id,billflow_id,file_id,alert_code,alert_content) values ( \n"
          " %d,%d,to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss'), \n"
          " %d,%d,%d,%d,'%s' )",iLogID,iProcLogID,pAppInfo->m_iProcessID,
          pAppInfo->m_iBillFlowID,iFileID,iAlertCode,sMsgSrc);

        qry.setSQL(sql);
        qry.execute();
        qry.commit();
        qry.close();
    }

    return true;
}

/*
 * 函数
 *　　WriteMsg
 * 功能
 *    应用程序输出信息
 * 参数
 *    信息代码
 *    信息内容
 */
int WriteMsg(int code, const char *fmt, ...)
{

        va_list ap;
        char sBuff[256];

        va_start (ap, fmt);
                vsprintf (sBuff, fmt, ap);
        vprintf(fmt,ap);
        va_end (ap);

        if (!pCurProc)
                return 0;

    time ((time_t *)&(pCurProc->MsgItem[pCurProc->iCurMsgPos].lTime));
    pCurProc->MsgItem[pCurProc->iCurMsgPos].iMsgType = code;
    pCurProc->MsgItem[pCurProc->iCurMsgPos].iNewFlag = 1;
    strncpy (pCurProc->MsgItem[pCurProc->iCurMsgPos].sMsg, sBuff, MAX_MSG_LEN);
    pCurProc->iCurMsgPos = (pCurProc->iCurMsgPos+1) % MAX_MSG_NUM;

        return 0;
}

/*
 * 函数
 *　　WriteErr
 * 功能
 *    应用程序输出错误或告警
 * 参数
 *    信息代码
 *    信息内容
 */
int WriteErr(int code, const char *fmt, ...)
{

        va_list ap;
        char sBuff[256];

        va_start (ap, fmt);
                vsprintf (sBuff, fmt, ap);
        vprintf(fmt,ap);
        va_end (ap);

        if (!pCurProc)
                return 0;

    time ((time_t *)&(pCurProc->ErrItem[pCurProc->iCurErrPos].lTime));
    pCurProc->ErrItem[pCurProc->iCurErrPos].iMsgType = code;
    pCurProc->ErrItem[pCurProc->iCurErrPos].iNewFlag = 1;
    strncpy (pCurProc->ErrItem[pCurProc->iCurErrPos].sMsg, sBuff, MAX_MSG_LEN);
    pCurProc->iCurErrPos = (pCurProc->iCurErrPos+1) % MAX_MSG_NUM;

        return 0;
}


//增加当前应用进程数据处理量(调用一次加1)
bool AddProcessCnt(int _iAddCnt)
{
    if (!g_pAppInfo) 
        return false;
    
    g_pAppInfo->m_lProcessCnt += _iAddCnt;
    return true;
}

//获取当前应用进程ID,对应WF_PROCESS.PROCESS_ID
//未获取到时,返回 -1 .
int GetProcessID()
{
    char *pt;
    
    if (g_iProcessID > 0)
        return g_iProcessID;
    
    if ((pt = getenv ("PROCESS_ID")) == NULL) {
        cout << "[ERROR]: getenv(PROCESS_ID) == NULL, 获取环境变量PROCESS_ID失败" <<endl;
        return -1;
    }
    g_iProcessID = atoi (pt);
    cout <<"[MESSAGE]: Envionment variable: PROCESS_ID = "<<g_iProcessID<<endl;
    
    // 顺便求出 g_iBillflowID (否则当应用进程不连接MC的时候g_iBillflowID为0)
    
    char sql[2048];
    DEFINE_QUERY(qry);    
    qry.close();
    sprintf(sql,"select billflow_id from wf_process where process_id=%d", g_iProcessID);
    qry.setSQL(sql);
    qry.open();
    if (qry.next())
        g_iBillflowID = qry.field("billflow_id").asInteger();
    
    qry.close();
    
    return g_iProcessID;
}

bool SaveLastEvent(StdEvent *pEvt)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    g_poAppSaveBuff->saveEvent(*pEvt);
    return true;
}
bool GetLastEvent(StdEvent *pEvt)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;

    return g_poAppSaveBuff->getEvent(*pEvt);
}
bool SetEventState (int _iState) //## 设置当前事件的处理状态, 初始值0
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    g_poAppSaveBuff->setEventState (_iState);
    
    return true;
}
bool GetEventState (int *piState)
{   //## 事件不存在返回false; 存在返回true, *piState = 实际状态
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);

    if (!g_poAppSaveBuff) return false;
    return g_poAppSaveBuff->getEventState (piState);
}
bool SetSaveBuff (void *pt, int iLen)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    g_poAppSaveBuff->setSaveBuff (pt, iLen);
    return true;
}
bool GetSaveBuff (void *pt, int iLen)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    g_poAppSaveBuff->getSaveBuff (pt, iLen);
    return true;
}
bool SetSaveValue (int iValueID, long lValue)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    return g_poAppSaveBuff->setSaveValue (iValueID, lValue);
}
bool GetSaveValue (int iValueID, long &lValue)
{
    if (!g_poAppSaveBuff && g_pShmTable) 
        g_poAppSaveBuff=g_pShmTable->getAppSaveBuffByProcID(g_iProcessID);
    if (!g_poAppSaveBuff) return false;
    return g_poAppSaveBuff->getSaveValue (iValueID, lValue);
}

// (内部使用)根据进程ID获取其SavBuf地址
TAppSaveBuff *GetSaveBufByProcID (int iProcessID)
{
    int iProcID = iProcessID;
    TAppSaveBuff *pSavBuf = g_poAppSaveBuff; //默认设置当前进程的地址
        
    //## 取得真正进程ID
    if (iProcID == 0) { //## 输入进程ID空    
        if (g_iProcessID <= 0) {
            GetProcessID();
            if (g_iProcessID <=0 )
                return 0x0;
        }
        iProcID = g_iProcessID;
    }
    
    if ((iProcID == g_iProcessID) && pSavBuf)
        return pSavBuf;
    
    if (!g_pShmTable) return false;
    
    pSavBuf = g_pShmTable->getAppSaveBuffByProcID(iProcID);
    
    return pSavBuf;
}

//## 存取指定进程的用户积压标志(如果iProcessID为0, 则取当前进程ID)
bool SetServBlockFlag (int iFlag, int iProcessID)
{
    TAppSaveBuff *pSavBuf = GetSaveBufByProcID (iProcessID);
    
    if (!pSavBuf) return false;
    
    pSavBuf->setServBlockFlag(iFlag);
    
    return true;
}
int  GetServBlockFlag (int iProcessID)
{
    TAppSaveBuff *pSavBuf = GetSaveBufByProcID (iProcessID);
        
    if (!pSavBuf) return -1;
    
    return pSavBuf->getServBlockFlag(); 
}

//## 检查进程iProcessID 是否活着
bool CheckProcAlive (int iProcessID)
{
    TAppInfo *pApp ;
    
    if (iProcessID <= 0 || !g_pShmTable) return false;
    
    pApp = g_pShmTable->getAppInfoByProcID (iProcessID);
    
    if (!pApp) return false;
    
    return pApp->checkAlive ();
}



bool SetSaveValueByProcID (int iValueID, long lValue, int iProcessID )
{
    
    TAppSaveBuff *pSavBuf = GetSaveBufByProcID (iProcessID);
    
    if (!pSavBuf) return false;
    
    pSavBuf->setSaveValue (iValueID, lValue);
    
    return true;
}
bool GetSaveValueByProcID (int iValueID, long &lValue, int iProcessID)
{
    TAppSaveBuff *pSavBuf = GetSaveBufByProcID (iProcessID);
        
    if (!pSavBuf) return false;
    
    return pSavBuf->getSaveValue (iValueID, lValue);
}



/////////////////////////////////////////////////

void TStatLog::reset()
{
    m_sFileName[0] = 0;
    m_iInNormal=0; 
    m_iInBlack =0; 
    m_iInDup=0;
    m_iInPreErr=0; 
    m_iInPricErr=0; 
    m_iInUseless=0;
    m_iOutNormal=0; 
    m_iOutBlack=0; 
    m_iOutDup=0; 
    m_iOutPreErr=0; 
    m_iOutPricErr=0; 
    m_iOutUseless=0;
    m_iFormatErr=0; 
    m_iOutFormatErr=0; 
    m_iRemainA =0; 
    m_iRemainB=0;
    m_poNext = NULL;
    m_iDistributeRemainA=0;
    m_iDistributeRemainB=0;
	m_iTempProcessID = 0;
	
	m_lNormalDuration=0;
	m_lNormalAmount=0;
	m_lNormalCharge=0;
	
	m_lBlackDuration=0;
	m_lBlackAmount=0;
	m_lBlackCharge=0;
	
	m_lDupDuration=0;
	m_lDupAmount=0;
	m_lDupCharge=0;
	
	m_lPreErrDuration=0;
	m_lPreErrAmount=0;
	m_lPreErrCharge=0;
	
	m_lPricErrDuration=0;
	m_lPricErrAmount=0;
	m_lPricErrCharge=0;
	
	m_lUseLessDuration=0;
	m_lUseLessAmount=0;
	m_lUseLessCharge=0;
	
	m_lFormatErrDuration=0;
	m_lFormatErrAmount=0;
	m_lFormatErrCharge=0;	
#ifdef _STAT_FILE_LOG    
	m_iProcLogID = 0;
	m_iProcessID = 0;
	m_iBillflowID = 0;
	memset(m_sMsgDate, 0, sizeof(m_sMsgDate));
	memset(m_sMsgTime, 0, sizeof(m_sMsgTime));
#endif 	
}

void TStatLog::addInNormal(int iCnt)
{
    m_iInNormal+=iCnt;
}
void TStatLog::addInBlack(int iCnt)
{
    m_iInBlack+=iCnt;

}
void TStatLog::addInDup(int iCnt)
{
    m_iInDup+=iCnt;
}
void TStatLog::addInPreErr(int iCnt)
{
    m_iInPreErr+=iCnt;
}
void TStatLog::addInPricErr(int iCnt)
{
    m_iInPricErr+=iCnt;
}
void TStatLog::addInUseless(int iCnt)
{
    m_iInUseless+=iCnt;
}
void TStatLog::addOutNormal(StdEvent *pEvt, int iCnt)
{
    m_iOutNormal+=iCnt;
    
	m_lNormalDuration += pEvt->m_lDuration; 
//	m_lNormalAmount += pEvt->m_lSumAmount;   	
	m_lNormalAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lNormalCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }
    
    if (g_pAppInfo)
        g_pAppInfo->m_lNormalCnt+=iCnt;
}
void TStatLog::addOutBlack(StdEvent *pEvt, int iCnt)
{
    m_iOutBlack+=iCnt;
    
	m_lBlackDuration += pEvt->m_lDuration; 
//	m_lBlackAmount += pEvt->m_lSumAmount;   	
	m_lBlackAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lBlackCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }    
    
    if (g_pAppInfo)
        g_pAppInfo->m_lBlackCnt+=iCnt;
}
void TStatLog::addOutDup(StdEvent *pEvt, int iCnt)
{
    m_iOutDup+=iCnt;
    
	m_lDupDuration += pEvt->m_lDuration; 
//	m_lDupAmount += pEvt->m_lSumAmount;   	
	m_lDupAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lDupCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }     
    
    if (g_pAppInfo)
        g_pAppInfo->m_lOtherCnt+=iCnt;
}
void TStatLog::addOutPreErr(StdEvent *pEvt, int iCnt)
{
    m_iOutPreErr+=iCnt;
    
	m_lPreErrDuration += pEvt->m_lDuration; 
//	m_lPreErrAmount += pEvt->m_lSumAmount;   	
	m_lPreErrAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lPreErrCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }     
    
    if (g_pAppInfo)
        g_pAppInfo->m_lErrorCnt+=iCnt;
}
void TStatLog::addOutPricErr(StdEvent *pEvt, int iCnt)
{
    m_iOutPricErr+=iCnt;
    
	m_lPricErrDuration += pEvt->m_lDuration; 
//	m_lPricErrAmount += pEvt->m_lSumAmount;   	
	m_lPricErrAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lPricErrCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }      
    
    if (g_pAppInfo)
        g_pAppInfo->m_lErrorCnt+=iCnt;
}            
void TStatLog::addOutUseless(StdEvent *pEvt, int iCnt)
{
    m_iOutUseless+=iCnt;
    
	m_lUseLessDuration += pEvt->m_lDuration; 
//	m_lUseLessAmount += pEvt->m_lSumAmount;   	
	m_lUseLessAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lUseLessCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }        
    
    if (g_pAppInfo)
        g_pAppInfo->m_lOtherCnt+=iCnt;
}
void TStatLog::addFormatErr(int iCnt)
{
    m_iFormatErr+=iCnt;
    
//    if (g_pAppInfo)
//        g_pAppInfo->m_lErrorCnt+=iCnt;
}

void TStatLog::addOutFormatErr(StdEvent *pEvt, int iCnt)
{
    this->m_iOutFormatErr+=iCnt;
    
	m_lFormatErrDuration += pEvt->m_lDuration; 
//	m_lFormatErrAmount += pEvt->m_lSumAmount;   	
	m_lFormatErrAmount += pEvt->m_lRecvAmount +  pEvt->m_lSendAmount + pEvt->m_lRecvAmountV6 + pEvt->m_lSendAmountV6;   	
    for(int j=0;j<MAX_CHARGE_NUM;j++)
    {
    	m_lFormatErrCharge += pEvt->m_oChargeInfo[j].m_lCharge;
    }
    
    if (g_pAppInfo)
        g_pAppInfo->m_lErrorCnt+=iCnt;
}


//统计模块输入数据
void TStatLog::addInEvent(int _iEvtTypeID, int iCnt)
{
    int iEvtType = 0;
    iEvtType = _iEvtTypeID;
    
    switch (iEvtType) {
      case CYCLE_FEE_EVENT_TYPE: //周期性费用事件
          addInNormal(iCnt);
          break;
      case CALL_EVENT_TYPE:      //语音事件
          addInNormal(iCnt);
          break;
      case DATA_EVENT_TYPE:      //数据事件
          addInNormal(iCnt);
          break;
      case VALUE_ADDED_EVENT_TYPE: //增值事件
          addInNormal(iCnt);
          break;
      case 0 : //预处理输入时是0
          addInNormal(iCnt);
          break;
      
      case DUP_EVENT_TYPE: //重复
          addInDup(iCnt);
          break;
      case NOOWNER_EVENT_TYPE: //无主
          addInBlack(iCnt);
          break;
      case PP_PARAM_ERR_EVENT_TYPE: //预处理参数错误
          addInPreErr(iCnt);
          break;
      case RATE_PARAM_ERR_EVENT_TYPE: //批价参数错误
          addInPricErr(iCnt);
          break;
      case OTHER_EVENT_TYPE: //无效
          addInUseless(iCnt);
          break;
      case ERR_EVENT_TYPE:   //格式错误(预处理)
          addFormatErr(iCnt);
          break;
      default:
          //addInUseless(iCnt);
          addInNormal(iCnt); //允许在事件类型树第二层节点增加正常事件类型
    }
}
//统计模块输出数据
void TStatLog::addOutEvent(int _iEvtTypeID, StdEvent *pEvt, int iCnt)
{
    int iEvtType = 0;

    iEvtType = _iEvtTypeID;


    static int iCount=0;
    iCount = iCount+iCnt;
    if (iCount > 1000)
    {
        iCount = 0;
        if (g_pAppInfo)
            g_pAppInfo->forceOutput ();
    }
        

    switch (iEvtType) {
      case CYCLE_FEE_EVENT_TYPE: //周期性费用事件
          addOutNormal(pEvt, iCnt);
          break;
      case CALL_EVENT_TYPE:      //语音事件
          addOutNormal(pEvt, iCnt);
          break;
      case DATA_EVENT_TYPE:      //数据事件
          addOutNormal(pEvt, iCnt);
          break;
      case VALUE_ADDED_EVENT_TYPE: //增值事件
          addOutNormal(pEvt, iCnt);
          break;
      case 0: //预处理正常事件
          addOutNormal(pEvt, iCnt);
          break;
      
      case DUP_EVENT_TYPE: //重复
          addOutDup(pEvt, iCnt);
          break;
      case NOOWNER_EVENT_TYPE: //无主
          addOutBlack(pEvt, iCnt);
          break;
      case PP_PARAM_ERR_EVENT_TYPE: //预处理参数错误
          addOutPreErr(pEvt, iCnt);
          break;
      case RATE_PARAM_ERR_EVENT_TYPE: //批价参数错误
          addOutPricErr(pEvt, iCnt);
          break;
      case OTHER_EVENT_TYPE: //无效
          addOutUseless(pEvt, iCnt);
          break;
      case ERR_EVENT_TYPE:   //格式错误(预处理)
          addOutFormatErr(pEvt, iCnt);
          break;
      
      case DISCARD_EVENT_TYPE_ID: //format 丢弃话单
      {
          static bool bInit (false);
          static bool bSendDiscardFlag (true);
          if (!bInit) {
            char sValue[100];
            if (ParamDefineMgr::getParam ("ATTR_FORMAT","SEND_DISCARD_FLAG",sValue))
            {
                if (sValue[0] == '0')
                    bSendDiscardFlag = false;
            }
            bInit = true;
          }
          
          if (bSendDiscardFlag)
            addOutUseless(pEvt, iCnt);
          else
            addRemainB(1);
          
          break;
      }
      
      default:
          //addOutUseless(iCnt);
          addOutNormal(pEvt, iCnt); //允许在事件类型树第二层节点增加正常事件类型
    }    
}

void TStatLog::addDistributeRemianA (int iCnt) 
{
   	m_iDistributeRemainA += iCnt;
}
    
void TStatLog::addDistributeRemianB (int iCnt) 
{
  	m_iDistributeRemainB += iCnt;
} 
void TStatLog::insertLog()
{
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select seq_stat_log_id.nextval log_id from dual");
    qry.open();
    qry.next();
    int iLogID = qry.field("log_id").asInteger();
    
    int ProcLogID=0;
    if (g_pAppInfo)
    {
        ProcLogID = g_pAppInfo->m_iProcessLogID;		
        g_pAppInfo->forceOutput();
    }
        
    qry.close();

    // === mod by yks 2007.09.29 ===
    qry.setSQL ("insert into wf_stat_log(stat_log_id,process_log_id,msg_date,msg_time, "
        " process_id,billflow_id,file_id,in_normal_event,in_black_event,   "
        " in_dup_event,in_pre_err_event,in_useless_event,out_normal_event, "
        " out_black_event,out_dup_enent,out_pre_err_event,out_pric_err_event,out_useless_event, "
        " format_err,in_pric_err_event,out_format_err,remain_a,remain_b, " 
        " distributeRemian_A,distributeRemian_B, "
        " normal_duration, normal_amount, normal_charge, "
        " black_duration, black_amount, black_charge, "
        " dup_duration, dup_amount, dup_charge, "
        " preerr_duration, preerr_amount, preerr_charge, "
        " pricerr_duration, pricerr_amount, pricerr_charge, "
        " useless_duration, useless_amount, useless_charge, "
        " formaterr_duration, formaterr_amount, formaterr_charge "
        "  ) values "
        "(:iLogID,:ProcLogID,to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss'), "
        " :g_iProcessID,:g_iBillflowID,:m_iFileID,      :m_iInNormal,  :m_iInBlack, "
        " :m_iInDup,    :m_iInPreErr,  :m_iInUseless,   :m_iOutNormal, "
        " :m_iOutBlack, :m_iOutDup,    :m_iOutPreErr,   :m_iOutPricErr,:m_iOutUseless, "
        " :m_iFormatErr,:m_iInPricErr, :m_iOutFormatErr,:m_iRemainA,   :m_iRemainB, " 
        " :m_iDistributeRemainA, :m_iDistributeRemainB, "
        " :m_lNormalDuration, :m_lNormalAmount, :m_lNormalCharge, "
        " :m_lBlackDuration, :m_lBlackAmount, :m_lBlackCharge, "
        " :m_lDupDuration, :m_lDupAmount, :m_lDupCharge, "
        " :m_lPreErrDuration, :m_lPreErrAmount, :m_lPreErrCharge, "
        " :m_lPricErrDuration, :m_lPricErrAmount, :m_lPricErrCharge, "
        " :m_lUseLessDuration, :m_lUseLessAmount, :m_lUseLessCharge, "
        " :m_lFormatErrDuration, :m_lFormatErrAmount, :m_lFormatErrCharge) "
    );
    
    qry.setParameter ("iLogID", iLogID);
    qry.setParameter ("ProcLogID", ProcLogID);
    qry.setParameter ("g_iProcessID", g_iProcessID);
    qry.setParameter ("g_iBillflowID", g_iBillflowID);
    qry.setParameter ("m_iFileID", m_iFileID);
    qry.setParameter ("m_iInNormal", m_iInNormal);
    qry.setParameter ("m_iInBlack", m_iInBlack);
    
    qry.setParameter ("m_iInDup", m_iInDup);
    qry.setParameter ("m_iInPreErr", m_iInPreErr);
    qry.setParameter ("m_iInUseless", m_iInUseless);
    qry.setParameter ("m_iOutNormal", m_iOutNormal);
    
    qry.setParameter ("m_iOutBlack", m_iOutBlack);
    qry.setParameter ("m_iOutDup", m_iOutDup);
    qry.setParameter ("m_iOutPreErr", m_iOutPreErr);
    qry.setParameter ("m_iOutPricErr", m_iOutPricErr);
    qry.setParameter ("m_iOutUseless", m_iOutUseless);
    
    qry.setParameter ("m_iFormatErr", m_iFormatErr);
    qry.setParameter ("m_iInPricErr", m_iInPricErr);
    qry.setParameter ("m_iOutFormatErr", m_iOutFormatErr);
    qry.setParameter ("m_iRemainA", m_iRemainA);
    qry.setParameter ("m_iRemainB", m_iRemainB);
    
    qry.setParameter ("m_iDistributeRemainA", m_iDistributeRemainA);
    qry.setParameter ("m_iDistributeRemainB", m_iDistributeRemainB);

    qry.setParameter ("m_lNormalDuration", m_lNormalDuration);
    qry.setParameter ("m_lNormalAmount", m_lNormalAmount);
    qry.setParameter ("m_lNormalCharge", m_lNormalCharge);
    
    qry.setParameter ("m_lBlackDuration", m_lBlackDuration);
    qry.setParameter ("m_lBlackAmount", m_lBlackAmount);
    qry.setParameter ("m_lBlackCharge", m_lBlackCharge);
    
    qry.setParameter ("m_lDupDuration", m_lDupDuration);
    qry.setParameter ("m_lDupAmount", m_lDupAmount);
    qry.setParameter ("m_lDupCharge", m_lDupCharge);
    
    qry.setParameter ("m_lPreErrDuration", m_lPreErrDuration);
    qry.setParameter ("m_lPreErrAmount", m_lPreErrAmount);
    qry.setParameter ("m_lPreErrCharge", m_lPreErrCharge);
    
    qry.setParameter ("m_lPricErrDuration", m_lPricErrDuration);
    qry.setParameter ("m_lPricErrAmount", m_lPricErrAmount);
    qry.setParameter ("m_lPricErrCharge", m_lPricErrCharge);
    
    qry.setParameter ("m_lUseLessDuration", m_lUseLessDuration);
    qry.setParameter ("m_lUseLessAmount", m_lUseLessAmount);
    qry.setParameter ("m_lUseLessCharge", m_lUseLessCharge);
    
    qry.setParameter ("m_lFormatErrDuration", m_lFormatErrDuration);
    qry.setParameter ("m_lFormatErrAmount", m_lFormatErrAmount);
    qry.setParameter ("m_lFormatErrCharge", m_lFormatErrCharge);                

    qry.execute();
    qry.commit();
    qry.close();
}

void TStatLog::insertLogEx()
{
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select seq_stat_log_id.nextval log_id from dual");
    qry.open();
    qry.next();
    int iLogID = qry.field("log_id").asInteger();
    
    int ProcLogID=0;
    if (g_pAppInfo)
    {
        ProcLogID = g_pAppInfo->m_iProcessLogID;		
        g_pAppInfo->forceOutput();
    }
        
    qry.close();

    // === mod by yks 2007.09.29 ===
    qry.setSQL ("insert into wf_stat_log(stat_log_id,process_log_id,msg_date,msg_time, "
        " process_id,FromProcessID,DESCPROCESSID,billflow_id,file_id,in_normal_event,in_black_event,   "
        " in_dup_event,in_pre_err_event,in_useless_event,out_normal_event, "
        " out_black_event,out_dup_enent,out_pre_err_event,out_pric_err_event,out_useless_event, "
        " format_err,in_pric_err_event,out_format_err,remain_a,remain_b , "
        " distributeRemian_A,distributeRemian_B, "
        " normal_duration, normal_amount, normal_charge, "
        " black_duration, black_amount, black_charge, "
        " dup_duration, dup_amount, dup_charge, "
        " preerr_duration, preerr_amount, preerr_charge, "
        " pricerr_duration, pricerr_amount, pricerr_charge, "
        " useless_duration, useless_amount, useless_charge, "
        " formaterr_duration, formaterr_amount, formaterr_charge "
        "  ) values "
        "(:iLogID,:ProcLogID,to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss'), "
        " :iProcessID,:FromProcessID,:iDescProcessID,:g_iBillflowID,:m_iFileID,      :m_iInNormal,  :m_iInBlack, "
        " :m_iInDup,    :m_iInPreErr,  :m_iInUseless,   :m_iOutNormal, "
        " :m_iOutBlack, :m_iOutDup,    :m_iOutPreErr,   :m_iOutPricErr,:m_iOutUseless, "
        " :m_iFormatErr,:m_iInPricErr, :m_iOutFormatErr,:m_iRemainA,:m_iRemainB, "
        " :m_iDistributeRemainA, :m_iDistributeRemainB, "
        " :m_lNormalDuration, :m_lNormalAmount, :m_lNormalCharge, "
        " :m_lBlackDuration, :m_lBlackAmount, :m_lBlackCharge, "
        " :m_lDupDuration, :m_lDupAmount, :m_lDupCharge, "
        " :m_lPreErrDuration, :m_lPreErrAmount, :m_lPreErrCharge, "
        " :m_lPricErrDuration, :m_lPricErrAmount, :m_lPricErrCharge, "
        " :m_lUseLessDuration, :m_lUseLessAmount, :m_lUseLessCharge, "
        " :m_lFormatErrDuration, :m_lFormatErrAmount, :m_lFormatErrCharge) "
    );
    
    qry.setParameter ("iLogID", iLogID);
    qry.setParameter ("ProcLogID", ProcLogID);
    qry.setParameter ("iProcessID", g_iProcessID);
    qry.setParameter ("FromProcessID",m_iFromProcessID);
	qry.setParameter ("iDescProcessID",m_iDescProcessID);
    qry.setParameter ("g_iBillflowID", g_iBillflowID);
    qry.setParameter ("m_iFileID", m_iFileID);
    qry.setParameter ("m_iInNormal", m_iInNormal);
    qry.setParameter ("m_iInBlack", m_iInBlack);
    
    qry.setParameter ("m_iInDup", m_iInDup);
    qry.setParameter ("m_iInPreErr", m_iInPreErr);
    qry.setParameter ("m_iInUseless", m_iInUseless);
    qry.setParameter ("m_iOutNormal", m_iOutNormal);
    
    qry.setParameter ("m_iOutBlack", m_iOutBlack);
    qry.setParameter ("m_iOutDup", m_iOutDup);
    qry.setParameter ("m_iOutPreErr", m_iOutPreErr);
    qry.setParameter ("m_iOutPricErr", m_iOutPricErr);
    qry.setParameter ("m_iOutUseless", m_iOutUseless);
    
    qry.setParameter ("m_iFormatErr", m_iFormatErr);
    qry.setParameter ("m_iInPricErr", m_iInPricErr);
    qry.setParameter ("m_iOutFormatErr", m_iOutFormatErr);
    qry.setParameter ("m_iRemainA", m_iRemainA);
    qry.setParameter ("m_iRemainB", m_iRemainB);
		
	qry.setParameter ("m_iDistributeRemainA", m_iDistributeRemainA);
    qry.setParameter ("m_iDistributeRemainB", m_iDistributeRemainB);
    
    qry.setParameter ("m_lNormalDuration", m_lNormalDuration);
    qry.setParameter ("m_lNormalAmount", m_lNormalAmount);
    qry.setParameter ("m_lNormalCharge", m_lNormalCharge);
    
    qry.setParameter ("m_lBlackDuration", m_lBlackDuration);
    qry.setParameter ("m_lBlackAmount", m_lBlackAmount);
    qry.setParameter ("m_lBlackCharge", m_lBlackCharge);
    
    qry.setParameter ("m_lDupDuration", m_lDupDuration);
    qry.setParameter ("m_lDupAmount", m_lDupAmount);
    qry.setParameter ("m_lDupCharge", m_lDupCharge);
    
    qry.setParameter ("m_lPreErrDuration", m_lPreErrDuration);
    qry.setParameter ("m_lPreErrAmount", m_lPreErrAmount);
    qry.setParameter ("m_lPreErrCharge", m_lPreErrCharge);
    
    qry.setParameter ("m_lPricErrDuration", m_lPricErrDuration);
    qry.setParameter ("m_lPricErrAmount", m_lPricErrAmount);
    qry.setParameter ("m_lPricErrCharge", m_lPricErrCharge);
    
    qry.setParameter ("m_lUseLessDuration", m_lUseLessDuration);
    qry.setParameter ("m_lUseLessAmount", m_lUseLessAmount);
    qry.setParameter ("m_lUseLessCharge", m_lUseLessCharge);
    
    qry.setParameter ("m_lFormatErrDuration", m_lFormatErrDuration);
    qry.setParameter ("m_lFormatErrAmount", m_lFormatErrAmount);
    qry.setParameter ("m_lFormatErrCharge", m_lFormatErrCharge);                
    
    qry.execute();
    qry.commit();
    qry.close();
}

// 计费应用进程处理日志

TStatLog *TStatLogMgr::searchLogByFileID(int iFileID)
{
    TStatLog *pt = NULL;
    
    for (pt=m_poLog; pt; pt=pt->m_poNext) {
        if (pt->m_iFileID == iFileID)
            break;
    }
    return pt;
}

TStatLog *TStatLogMgr::searchLogEX(string strMapKey)
{
    TStatLog *pt = NULL;
    
    m_mapIter = m_mapStatLog.find(strMapKey); 
		if(m_mapIter == m_mapStatLog.end()) 
		{
		    //没找到
		    pt = NULL;
		}
		else 
		{
		    //找到
		    pt = (TStatLog *) m_mapIter->second;
		} 
    
    return pt;
}

void TStatLogMgr::insertLogForce()
{
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	if (!m_bForceFlag)//
		return;
	
	TStatLog *pStatLog = NULL;
  for (m_mapIter = m_mapStatLog.begin(); m_mapIter != m_mapStatLog.end(); m_mapIter++)
	{
		pStatLog = (TStatLog*)m_mapIter->second;
		if (NULL != m_mapIter->second)
		{	
#ifdef _STAT_FILE_LOG
			if (!writeStatLog(pStatLog))
			{
				Log::log(0, "WF_STAT_LOG文件日志写入失败");
			}	
#else			
			pStatLog->insertLogEx();
#endif
			delete m_mapIter->second;
		}
	}
	m_mapStatLog.clear() ;	
#ifdef _STAT_FILE_LOG
	commitStatLog(true);
#endif 	
}

void TStatLogMgr::setForceFlag(bool bFlag)
{
	m_bForceFlag = bFlag;
}


TStatLogMgr::~TStatLogMgr()
{
	TStatLog *pStatLog = NULL;
  for (m_mapIter = m_mapStatLog.begin(); m_mapIter != m_mapStatLog.end(); m_mapIter++)
	{
		pStatLog = (TStatLog*)m_mapIter->second;
		if (NULL != m_mapIter->second)
		{	
#ifdef _STAT_FILE_LOG
			if (!writeStatLog(pStatLog))
			{
				Log::log(0, "WF_STAT_LOG文件日志写入失败");
			}	
#else			
			pStatLog->insertLogEx();
#endif
			delete m_mapIter->second;
		} 
	}
	m_mapStatLog.clear() ;
#ifdef _STAT_FILE_LOG
	commitStatLog(true);
#endif 		
}

void TStatLogMgr::addRemainAEX (int _iFileID, StdEvent *pEvt, int iCnt)
{
		#ifndef __WF_STAT_DETAIL_LOG__
			return;
		#endif
	
    // 只对原始事件做量的统计
    if (pEvt->m_iEventSourceType !=1 ) 
    {  
        return;
    }
    
    int iFromProcessID = pEvt->m_iProcessID;
    
    memset(m_sTMapKey, 0, sizeof(m_sTMapKey));
    sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,0);
    m_strMapKey = m_sTMapKey;
    
    TStatLog *pt = searchLogEX(m_strMapKey);
    
    if (pt) {
        pt->addRemainA(iCnt);
        return;
    }
}

void TStatLogMgr::addRemainBEX (int _iFileID, StdEvent *pEvt, int iCnt)
{

#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
	int iFromProcessID = 0;
	if(NULL == pEvt) 
		iFromProcessID = 0;
	else
		iFromProcessID = 0;//Process::m_iFromProcID;

    memset(m_sTMapKey, 0, sizeof(m_sTMapKey));
    sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,0);
    m_strMapKey = m_sTMapKey;
    
    TStatLog *pt = searchLogEX(m_strMapKey);
    
    if (pt) {
        pt->addRemainB(iCnt);
        return;
    }
}
//统计输入事件(包括从文件读取、从队列获取的数据时调用)
void TStatLogMgr::addInEventEX(int _iFileID, int _iEventTypeID,int iFromProcessID,int iCnt)
{
    if (_iEventTypeID<0) //非业务事件(可能是文件结束信号或强制提交信号等)
        return;
    
    int iEvtType = getParentSecLevelEvtType(_iEventTypeID);
    
    memset(m_sTMapKey, 0, sizeof(m_sTMapKey));
    sprintf(m_sTMapKey, "%d_%d_%d", _iFileID, iFromProcessID, 0);
    m_strMapKey = m_sTMapKey;
    
    TStatLog *pt = searchLogEX(m_strMapKey);
    
    if (pt) {
        pt->addInEvent(iEvtType,iCnt);
        return;
    }
    
    if(m_mapStatLog.size() >= 1024)
    {
    	Log::log(0, "wf_stat_log达到阀值强制输出");
    	insertLogForce();
    }	
    
    //if not found. new one TStatLog instance
    TStatLog *pStaLog = new TStatLog(_iFileID,iFromProcessID);
    if(pStaLog)
    {
    	m_mapStatLog.insert(map<string, TStatLog *> :: value_type(m_strMapKey,pStaLog));
    	pStaLog->addInEvent(iEvtType,iCnt);
    }
} 

void TStatLogMgr::addInEventEX(int _iFileID, StdEvent *pEvt,  int iCnt)
{
	
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    // 只对原始事件做量的统计
    if (pEvt->m_iEventSourceType==1) 
    {	
        addInEventEX(pEvt->m_iFileID,pEvt->m_iEventTypeID,pEvt->m_iProcessID, iCnt);
    }
}

void TStatLogMgr::addOutEventEX(int _iFileID, int _iEventTypeID, StdEvent *pEvt, int iFromProcessID,int iDesProcessID[],int iDesProIDCount,int iCnt)
{
	if (_iEventTypeID<0) //非业务事件(可能是文件结束信号或强制提交信号等)
        return;
    
	int iEvtType = getParentSecLevelEvtType(_iEventTypeID);
	TStatLog *pt = NULL;
	
	for (int i=0;i < iDesProIDCount;i++)
	{
		memset(m_sTMapKey,0,sizeof(m_sTMapKey));
		sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,iDesProcessID[i]);
		m_strMapKey = m_sTMapKey;
		
		pt = searchLogEX(m_strMapKey.c_str());
    
		if (NULL != pt) 
		{
			pt->addOutEvent(iEvtType, pEvt, iCnt);
			if(0 == pt->m_iDescProcessID) 
				pt->m_iDescProcessID = iDesProcessID[i];
			continue;
		}
		else
		{
			memset(m_sTMapKey,0,sizeof(m_sTMapKey));
			sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,0);
			
			m_strMapKey = m_sTMapKey;
			pt = searchLogEX(m_strMapKey.c_str());
			
			if (NULL != pt) 
			{
				if (pt->m_iTempProcessID > 0 && iDesProcessID[i] == pt->m_iTempProcessID)
				{
					pt->addOutEvent(iEvtType, pEvt, iCnt);
				}
				else if (pt->m_iTempProcessID > 0 && iDesProcessID[i] != pt->m_iTempProcessID)
				{
					TStatLog *pStaLog = new TStatLog(_iFileID,iFromProcessID,iDesProcessID[i]);
					if(pStaLog)
					{
						memset(m_sTMapKey,0,sizeof(m_sTMapKey));
						sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,iDesProcessID[i]);
						m_strMapKey = m_sTMapKey;
						m_mapStatLog.insert(map<string, TStatLog *> :: value_type(m_strMapKey,pStaLog));
						pStaLog->addOutEvent(iEvtType, pEvt, iCnt);
					}
				}
				else if (pt->m_iTempProcessID <= 0)
				{
					pt->addOutEvent(iEvtType, pEvt, iCnt);
					pt->m_iTempProcessID = iDesProcessID[i];
				}

				if(0 == pt->m_iDescProcessID) 
					pt->m_iDescProcessID = iDesProcessID[i];
			}
			else
			{
				//if not found. new one TStatLog instance
				memset(m_sTMapKey,0,sizeof(m_sTMapKey));
				sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,iDesProcessID[i]);
				
				m_strMapKey = m_sTMapKey;
				TStatLog *pStaLog = new TStatLog(_iFileID,iFromProcessID,iDesProcessID[i]);
				if(pStaLog)
				{
					m_mapStatLog.insert(map<string, TStatLog *> :: value_type(m_strMapKey,pStaLog));
					pStaLog->addOutEvent(iEvtType, pEvt, iCnt);
					if(0 == pStaLog->m_iDescProcessID) 
						pStaLog->m_iDescProcessID = iDesProcessID[i];
				}
			}
		}
	}
}

void TStatLogMgr::addOutEventEX(int _iFileID, int iFromProcessId, StdEvent *pEvt, int _iDesProcessID[],int _iDesProIDCount, int iCnt)
{
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    // 只对原始事件做量的统计
    if (pEvt->m_iEventSourceType==1) 
	{  
		//输入的时候没有办法获取目标进程，这里把它补上
        addOutEventEX(_iFileID, pEvt->m_iEventTypeID, pEvt, iFromProcessId,_iDesProcessID,_iDesProIDCount, iCnt);
    }
    
    AddProcessCnt (iCnt);
}

//文件处理结束后，发送提交信号：
void TStatLogMgr::insertLogEx(int _iFileID,int _iFromProcessID,int _iProcessIDBuf[],int _iProCount)
{

#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    TStatLog *pStatLog = NULL;

    if(m_mapStatLog.size() <= 0) return;
    
  for(int i=0;i<_iProCount;i++)
	{
		memset(m_sTMapKey,0,sizeof(m_sTMapKey));		
		snprintf(m_sTMapKey,sizeof(m_sTMapKey),"%d_%d_%d",_iFileID,_iFromProcessID,_iProcessIDBuf[i]);
		
		m_strMapKey = m_sTMapKey;
		m_mapIter = m_mapStatLog.find(m_strMapKey);
		if(m_mapIter != m_mapStatLog.end())
		{
			pStatLog = (TStatLog*)m_mapIter->second;
			if (NULL != m_mapIter->second)
			{	
#ifdef _STAT_FILE_LOG
				if (!writeStatLog(pStatLog))
				{
					Log::log(0, "WF_STAT_LOG文件日志写入失败");
				}	
#else				
				pStatLog->insertLogEx();
#endif				
				delete m_mapIter->second;
			}
			m_mapStatLog.erase(m_mapIter);
		}
		else
		{
			//使用 目的 进程ID = 0 来找
			memset(m_sTMapKey,0,sizeof(m_sTMapKey));
			snprintf(m_sTMapKey,sizeof(m_sTMapKey),"%d_%d_%d",_iFileID,_iFromProcessID,0);
			m_strMapKey = m_sTMapKey;
			m_mapIter = m_mapStatLog.find(m_strMapKey);

			if(m_mapIter != m_mapStatLog.end())
			{
				pStatLog = (TStatLog*)m_mapIter->second;
				if (NULL != m_mapIter->second)
				{	
#ifdef _STAT_FILE_LOG
					if (!writeStatLog(pStatLog))
					{
						Log::log(0, "WF_STAT_LOG文件日志写入失败");
					}	
#else				
					pStatLog->insertLogEx();
#endif	
					delete m_mapIter->second;
				}
				m_mapStatLog.erase(m_mapIter);
			}
			else //使用 来源 进程ID 为 0 的来找
			{
				memset(m_sTMapKey,0,sizeof(m_sTMapKey));
				snprintf(m_sTMapKey,sizeof(m_sTMapKey),"%d_%d_%d",_iFileID,0,_iProcessIDBuf[i]);
				m_strMapKey = m_sTMapKey;
				m_mapIter = m_mapStatLog.find(m_strMapKey);
				if(m_mapIter != m_mapStatLog.end())
				{
					pStatLog = (TStatLog*)m_mapIter->second;
					if (NULL != m_mapIter->second)
					{	
#ifdef _STAT_FILE_LOG
						if (!writeStatLog(pStatLog))
						{
							Log::log(0, "WF_STAT_LOG文件日志写入失败");
						}	
#else				
						pStatLog->insertLogEx();
#endif
						delete m_mapIter->second;
					}
					m_mapStatLog.erase(m_mapIter);
				}
				//else
				//{
					//Log::log (0, "FileID = %d ,FromProcessID = %d,DesProcessID = %d 提交到 wf_stat_log 失败",_iFileID,_iFromProcessID,_iProcessIDBuf[i]);
				//}
			}
		}
	}
    
#ifdef _STAT_FILE_LOG
	commitStatLog(true);//3.24改为每个true提交日志文件
#endif    
    //TWfLog::log(WF_FILE,"Can not find the statLog of fileID:%d",_iFileID);
}
#ifdef _STAT_FILE_LOG
bool TStatLogMgr::initLogFilePath()
{	
	snprintf(m_sLogOpenFilePath, sizeof(m_sLogOpenFilePath), "%s/openfile/%d", Process::m_sLogFilePath, Process::m_iProcID);
	snprintf(m_sLogCloseFilePath, sizeof(m_sLogCloseFilePath), "%s/closefile/%d", Process::m_sLogFilePath, Process::m_iProcID);	
	
	///确保目录存在
	DIR * dp;
	if ((  dp=opendir(m_sLogOpenFilePath) ) == NULL )
	{
		//不能打开目录,试图创建该目录
		if( mkdir(m_sLogOpenFilePath, 0770) != 0 )
		{
			char sFilePath[300] = {0};
			snprintf(sFilePath, sizeof(sFilePath), "%s/openfile", Process::m_sLogFilePath);
			
			if ( mkdir(sFilePath, 0770) != 0 || mkdir(m_sLogOpenFilePath, 0770) != 0 )
			{		
	   			Log::log(0,"进程临时STAT_LOG文件目录(%s)创建失败!", m_sLogOpenFilePath);
				return false;
			}	
		}	
	}
	if(dp) 
		closedir(dp);  
		
	//检测是否有未提交的日志文件,防止进程被强杀	
		
	if ((  dp=opendir(m_sLogCloseFilePath) ) == NULL )
	{
		//不能打开目录,试图创建该目录
		if( mkdir(m_sLogCloseFilePath, 0770) != 0 )
		{
			char sFilePath[300] = {0};
			snprintf(sFilePath, sizeof(sFilePath), "%s/closefile", Process::m_sLogFilePath);
			
			if ( mkdir(sFilePath, 0770) != 0 || mkdir(m_sLogCloseFilePath, 0770) != 0)
			{		
	   			Log::log(0,"进程临时STAT_LOG文件目录(%s)创建失败!", m_sLogOpenFilePath);
				return false;
			}	
		}	
	}
	if(dp) 
		closedir(dp);
		
	commitStatLogForce();//检测是否存在未提交的日志
	
	return true;	  		  					
}	//文件目录初始化完毕

bool TStatLogMgr::writeStatLog(TStatLog* pStatLog)
{
	if (m_sLogOpenFilePath[0] == '\0' || m_sLogCloseFilePath[0] == '\0')//初始化写文件文件路径
	{	
		if (!initLogFilePath())
			return false;
	}
	
	if (m_pFile == NULL)
	{
		static Date oLogFileDate;
		oLogFileDate.getCurDate();
		
		strcpy(m_sLogCreateTime, oLogFileDate.toString());
		snprintf(m_sLogFileName, sizeof(m_sLogFileName), "%d_%s_%d.statlog", Process::m_iProcID, m_sLogCreateTime, oLogFileDate.getMilSec());
		
		snprintf(m_sLogFullFileName, sizeof(m_sLogFullFileName), "%s/%s", m_sLogOpenFilePath, m_sLogFileName);	
		m_pFile = fopen(m_sLogFullFileName, "wb+");
		
		if (m_pFile == NULL)
			return false;
	}	
	
	while (pStatLog)
	{
    	pStatLog->m_iProcLogID = 0;
    	if (g_pAppInfo)
    	{
    	    pStatLog->m_iProcLogID = g_pAppInfo->m_iProcessLogID;		
    	    g_pAppInfo->forceOutput();
    	}		
		
		pStatLog->m_iProcessID = g_iProcessID;
		pStatLog->m_iBillflowID = g_iBillflowID;
		
		static Date oMsgDate;
		oMsgDate.getCurDate();
		
		char *sMsgDate = oMsgDate.toString();
		
		strncpy(pStatLog->m_sMsgDate, sMsgDate, 8);
		strncpy(pStatLog->m_sMsgTime, sMsgDate+8, 6);
		
		if ( fwrite(pStatLog, sizeof(TStatLog), 1, m_pFile) != 1)
			return false;
			
		fflush (m_pFile);	//放到这儿来
		++m_iLogRecords;
			
		pStatLog = pStatLog->m_poNext;
	}
	
    //fflush (m_pFile);
    
    return true;
}	

int TStatLogMgr::indbStatLog(TStatLog* pStatLog, int iLogRecords)
{
	DEFINE_QUERY(qry);
	
    qry.setSQL ("insert into wf_stat_log(stat_log_id,process_log_id,msg_date,msg_time, "
        " process_id,FromProcessID,DESCPROCESSID,billflow_id,file_id,in_normal_event,in_black_event,   "
        " in_dup_event,in_pre_err_event,in_useless_event,out_normal_event, "
        " out_black_event,out_dup_enent,out_pre_err_event,out_pric_err_event,out_useless_event, "
        " format_err,in_pric_err_event,out_format_err,remain_a,remain_b , "
        " distributeRemian_A,distributeRemian_B, "
        " normal_duration, normal_amount, normal_charge, "
        " black_duration, black_amount, black_charge, "
        " dup_duration, dup_amount, dup_charge, "
        " preerr_duration, preerr_amount, preerr_charge, "
        " pricerr_duration, pricerr_amount, pricerr_charge, "
        " useless_duration, useless_amount, useless_charge, "
        " formaterr_duration, formaterr_amount, formaterr_charge "
        "  ) values "
        "(seq_stat_log_id.nextval, :ProcLogID, :msg_date, :msg_time, "
        " :iProcessID, :FromProcessID, :iDescProcessID, :g_iBillflowID, :m_iFileID, :m_iInNormal, :m_iInBlack, "
        " :m_iInDup,    :m_iInPreErr,  :m_iInUseless,   :m_iOutNormal, "
        " :m_iOutBlack, :m_iOutDup,    :m_iOutPreErr,   :m_iOutPricErr,:m_iOutUseless, "
        " :m_iFormatErr,:m_iInPricErr, :m_iOutFormatErr,:m_iRemainA,:m_iRemainB, "
        " :m_iDistributeRemainA, :m_iDistributeRemainB, "
        " :m_lNormalDuration, :m_lNormalAmount, :m_lNormalCharge, "
        " :m_lBlackDuration, :m_lBlackAmount, :m_lBlackCharge, "
        " :m_lDupDuration, :m_lDupAmount, :m_lDupCharge, "
        " :m_lPreErrDuration, :m_lPreErrAmount, :m_lPreErrCharge, "
        " :m_lPricErrDuration, :m_lPricErrAmount, :m_lPricErrCharge, "
        " :m_lUseLessDuration, :m_lUseLessAmount, :m_lUseLessCharge, "
        " :m_lFormatErrDuration, :m_lFormatErrAmount, :m_lFormatErrCharge) "
    );	
    
    qry.setParamArray ("ProcLogID", &(pStatLog[0].m_iProcLogID), sizeof(pStatLog[0]));
    qry.setParamArray ("msg_date", (char **)&(pStatLog[0].m_sMsgDate), sizeof(pStatLog[0]), sizeof(pStatLog[0].m_sMsgDate));
    qry.setParamArray ("msg_time", (char **)&(pStatLog[0].m_sMsgTime), sizeof(pStatLog[0]), sizeof(pStatLog[0].m_sMsgTime));    
    qry.setParamArray ("iProcessID", &(pStatLog[0].m_iProcessID), sizeof(pStatLog[0]));    
    qry.setParamArray ("FromProcessID",&(pStatLog[0].m_iFromProcessID), sizeof(pStatLog[0]));
	qry.setParamArray ("iDescProcessID",&(pStatLog[0].m_iDescProcessID), sizeof(pStatLog[0]));
    qry.setParamArray ("g_iBillflowID", &(pStatLog[0].m_iBillflowID), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iFileID", &(pStatLog[0].m_iFileID), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iInNormal", &(pStatLog[0].m_iInNormal), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iInBlack", &(pStatLog[0].m_iInBlack), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_iInDup", &(pStatLog[0].m_iInDup), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iInPreErr", &(pStatLog[0].m_iInPreErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iInUseless", &(pStatLog[0].m_iInUseless), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutNormal", &(pStatLog[0].m_iOutNormal), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_iOutBlack", &(pStatLog[0].m_iOutBlack), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutDup", &(pStatLog[0].m_iOutDup), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutPreErr", &(pStatLog[0].m_iOutPreErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutPricErr", &(pStatLog[0].m_iOutPricErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutUseless", &(pStatLog[0].m_iOutUseless), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_iFormatErr", &(pStatLog[0].m_iFormatErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iInPricErr", &(pStatLog[0].m_iInPricErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iOutFormatErr", &(pStatLog[0].m_iOutFormatErr), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iRemainA", &(pStatLog[0].m_iRemainA), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iRemainB", &(pStatLog[0].m_iRemainB), sizeof(pStatLog[0]));
		
	qry.setParamArray ("m_iDistributeRemainA", &(pStatLog[0].m_iDistributeRemainA), sizeof(pStatLog[0]));
    qry.setParamArray ("m_iDistributeRemainB", &(pStatLog[0].m_iDistributeRemainB), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lNormalDuration", &(pStatLog[0].m_lNormalDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lNormalAmount", &(pStatLog[0].m_lNormalAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lNormalCharge", &(pStatLog[0].m_lNormalCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lBlackDuration", &(pStatLog[0].m_lBlackDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lBlackAmount", &(pStatLog[0].m_lBlackAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lBlackCharge", &(pStatLog[0].m_lBlackCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lDupDuration", &(pStatLog[0].m_lDupDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lDupAmount", &(pStatLog[0].m_lDupAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lDupCharge", &(pStatLog[0].m_lDupCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lPreErrDuration", &(pStatLog[0].m_lPreErrDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lPreErrAmount", &(pStatLog[0].m_lPreErrAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lPreErrCharge", &(pStatLog[0].m_lPreErrCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lPricErrDuration", &(pStatLog[0].m_lPricErrDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lPricErrAmount", &(pStatLog[0].m_lPricErrAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lPricErrCharge", &(pStatLog[0].m_lPricErrCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lUseLessDuration", &(pStatLog[0].m_lUseLessDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lUseLessAmount", &(pStatLog[0].m_lUseLessAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lUseLessCharge", &(pStatLog[0].m_lUseLessCharge), sizeof(pStatLog[0]));
    
    qry.setParamArray ("m_lFormatErrDuration", &(pStatLog[0].m_lFormatErrDuration), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lFormatErrAmount", &(pStatLog[0].m_lFormatErrAmount), sizeof(pStatLog[0]));
    qry.setParamArray ("m_lFormatErrCharge", &(pStatLog[0].m_lFormatErrCharge), sizeof(pStatLog[0]));
    
	qry.execute(iLogRecords);
	qry.commit();
	qry.close();
	
	return iLogRecords;	
}

int TStatLogMgr::commitStatLogForce()
{
	Directory oDirectory(m_sLogOpenFilePath);
	
	int iFileCnt = oDirectory.getFileList();
	if (iFileCnt <= 0)
	{
		//Log::log(0, "未检测到进程有未提交的wfstatlog文件日志");
		return 0;
	}	
	
	Log::log(0, "上次程序异常退出存在未提交的wfstatlog临时文件日志,将强制提交,但日志信息可能已有丢失,请手工稽核");
		
	for (int i = 0; i < iFileCnt; i++)
	{
		File *pFile = oDirectory.getFile(i);
		
		char sLogCommitFileName[400] = {0};
		snprintf(sLogCommitFileName, sizeof(sLogCommitFileName), "%s/%s", m_sLogCloseFilePath, pFile->getFileName());
		
		if (rename(pFile->getFullFileName(), sLogCommitFileName) != 0)
			return -1;		
	}	
	
	return iFileCnt;	
}	

int TStatLogMgr::commitStatLog(bool bCommit)
{
	if (m_pFile == NULL) 
		return 0;
	
	//if (bCommit || m_iLogRecords > 1024 || checkDelay())
	if (bCommit || m_iLogRecords > 1024)//3.24改为按file_id提交 没有必要再checkDelay()了
	{
		fflush(m_pFile);
		fclose(m_pFile);
		m_pFile = NULL;
		m_iLogRecords = 0;
		
		static char sLogCommitFileName[400];
		memset(sLogCommitFileName, 0, sizeof(sLogCommitFileName));
		
		snprintf(sLogCommitFileName, sizeof(sLogCommitFileName), "%s/%s", m_sLogCloseFilePath, m_sLogFileName);
		
		if (rename(m_sLogFullFileName, sLogCommitFileName) != 0)
			return -1;
		
		memset(m_sLogFileName, 0, sizeof(m_sLogFileName));
		memset(m_sLogFullFileName, 0, sizeof(m_sLogFullFileName));
		
		return 1;
	}
	
	return 0;
}

bool TStatLogMgr::checkDelay()
{
	static Date d1, d2;
	
	d1.getCurDate();
	d2.parse(m_sLogCreateTime);
	
	if (d1.diffSec(d2) > STAT_LOG_FILE_DELAY_SEC)
		return true;
		
	return false;	
}	

#endif

//统计输入事件(包括从文件读取、从队列获取的数据时调用)
void TStatLogMgr::addInEvent(int _iFileID, int _iEvtTypeID, int iCnt)
{
    if (_iEvtTypeID<0) //非业务事件(可能是文件结束信号或强制提交信号等)
        return;
    
    int iEvtType = getParentSecLevelEvtType(_iEvtTypeID);
    
    TStatLog *pt = searchLogByFileID(_iFileID);
    
    if (pt) {
        pt->addInEvent(iEvtType,iCnt);
        return;
    }
    
    //if not found. new one TStatLog instance
    TStatLog **ppt;
    for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
        ;
    }

    (*ppt) = new TStatLog(_iFileID);
    
    (*ppt)->addInEvent(iEvtType,iCnt);
} 
void TStatLogMgr::addInEvent(int _iFileID, StdEvent *pEvt,  int iCnt)
{
#ifdef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    // 只对原始事件做量的统计
    if (pEvt->m_iEventSourceType==1) {  
        addInEvent(_iFileID, pEvt->m_iEventTypeID, iCnt);
    }
}

//统计输出事件(包括发送数据到下游队列、数据入库时调用)
void TStatLogMgr::addOutEvent(int _iFileID, int _iEvtTypeID, StdEvent *pEvt, int iCnt)
{
    if (_iEvtTypeID<0) //非业务事件(可能是文件结束信号或强制提交信号等)
        return;
    
    int iEvtType = getParentSecLevelEvtType(_iEvtTypeID);
    
    TStatLog *pt = searchLogByFileID(_iFileID);
    
    if (pt) {
        pt->addOutEvent(iEvtType, pEvt, iCnt);
        return;
    }
    
    //if not found. new one TStatLog instance
    TStatLog **ppt;

    for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
        ;
    }

    (*ppt) = new TStatLog(_iFileID);
    
    (*ppt)->addOutEvent(iEvtType, pEvt, iCnt);
}

void TStatLogMgr::addOutEvent(int _iFileID,  StdEvent *pEvt,  int iCnt)
{
	
#ifdef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    // 只对原始事件做量的统计
    if (pEvt->m_iEventSourceType==1) {  
        addOutEvent(_iFileID, pEvt->m_iEventTypeID, pEvt, iCnt);
    }
    
    AddProcessCnt (iCnt);
}

void TStatLogMgr::addRemainA (int _iFileID, int iCnt)
{
	
#ifdef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    TStatLog *pt = searchLogByFileID(_iFileID);
    if (pt)   pt->addRemainA (iCnt);
    else {
        TStatLog **ppt;
        for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
            ;
        }
        (*ppt) = new TStatLog(_iFileID);
        (*ppt)->addRemainA (iCnt);
    }
}
void TStatLogMgr::addRemainB (int _iFileID, int iCnt)
{

#ifdef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    TStatLog *pt = searchLogByFileID(_iFileID);
    if (pt)   pt->addRemainB (iCnt);
    else {
        TStatLog **ppt;
        for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
            ;
        }
        (*ppt) = new TStatLog(_iFileID);
        (*ppt)->addRemainB (iCnt);
    }
}

void TStatLogMgr::addDistributeRemianA(int _iFileID, int iAppID,int iFromProcessId,StdEvent *pEvt, int _iDesProcessID[],int _iDesProIDCount, int iCnt)
{
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
		
	addDistributeRemianA(_iFileID,iAppID,iFromProcessId,_iDesProcessID,_iDesProIDCount,iCnt);
}	
	
void TStatLogMgr::addDistributeRemianA(int _iFileID, int iAppID, int iFromProcessID,int iDesProcessID[],int iDesProIDCount,int iCnt)
{ 
	for (int i=0;i < iDesProIDCount;i++)
	{
		if(PRICING_APP_ID != iAppID && 0 ==i) continue;//除了批价，其他的模块第一个都不加拆分
			
    	memset(m_sTMapKey, 0, sizeof(m_sTMapKey));
		//if (0 == i)
		//{
			//sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,0);
		//}
		//else
		//{
		sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,iDesProcessID[i]);
		//}
		
		m_strMapKey = m_sTMapKey;
    
		TStatLog *pt = searchLogEX(m_strMapKey.c_str());
    
		if (NULL != pt) 
		{
			if(0 == pt->m_iDescProcessID)
			{
				pt->m_iDescProcessID = iDesProcessID[i];
			}
			pt->addDistributeRemianA(iCnt);
			continue;
		}
		else
		{
			;
		}
	}
}

void TStatLogMgr::addDistributeRemianB(int _iFileID, int iFromProcessId,StdEvent *pEvt,int _iDesProIDCount, int iCnt)
{
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
		
	addDistributeRemianB(_iFileID,iFromProcessId,_iDesProIDCount,iCnt);
}	

void TStatLogMgr::addDistributeRemianB(int _iFileID, int iFromProcessID,int iDesProIDCount,int iCnt)
{ 
	//for (int i=0;i < iDesProIDCount;i++)
	//{
    	memset(m_sTMapKey, 0, sizeof(m_sTMapKey));
		sprintf(m_sTMapKey,"%d_%d_%d",_iFileID,iFromProcessID,0);
		m_strMapKey = m_sTMapKey;
    
		TStatLog *pt = searchLogEX(m_strMapKey.c_str());
    
		if (NULL != pt) 
		{
			pt->addDistributeRemianB(iCnt);
		}
		else
		{
			TStatLog *pStaLog = new TStatLog(_iFileID,iFromProcessID);
	    if(pStaLog != NULL)
	    {
	    	m_mapStatLog.insert(map<string, TStatLog *> :: value_type(m_strMapKey,pStaLog));
	    	pStaLog->addDistributeRemianB(iCnt);
	    }
		}
	//}
}
/*
void TStatLogMgr::addDistributeRemianA(int _iFileID, int iCnt)
{	
    TStatLog *pt = searchLogByFileID(_iFileID);
    if (pt)   pt->addDistributeRemianA (iCnt);
    else {
        TStatLog **ppt;
        for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
            ;
        }
        (*ppt) = new TStatLog(_iFileID);
        (*ppt)->addDistributeRemianA (iCnt);
    }	
}

    
void TStatLogMgr::addDistributeRemianB(int _iFileID, int iCnt)
{
	
#ifndef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    TStatLog *pt = searchLogByFileID(_iFileID);
    if (pt)   pt->addDistributeRemianB (iCnt);
    else {
        TStatLog **ppt;
        for (ppt=&m_poLog; *ppt; ppt= &((*ppt)->m_poNext) ) {
            ;
        }
        (*ppt) = new TStatLog(_iFileID);
        (*ppt)->addDistributeRemianB (iCnt);
    }	
}
*/
//文件处理结束后，发送提交信号：
void TStatLogMgr::insertLog(int _iFileID)
{

#ifdef __WF_STAT_DETAIL_LOG__
	return;
#endif
	
    TStatLog *pHead, *pTail;

    if (!m_poLog) return;
    
    pHead = pTail = m_poLog;
        
    while(pHead) {
        if (pHead->m_iFileID != _iFileID) {
            pTail = pHead;
            pHead = pHead->m_poNext;
            continue;
        }
        if (pHead ==pTail) {
            m_poLog = pHead->m_poNext;
            pHead->insertLog();
            delete pHead;
            return;
        }
        else {
            pTail->m_poNext = pHead->m_poNext;
            pHead->insertLog();
            delete pHead;
            return;
        }
    }
    
    //TWfLog::log(WF_FILE,"Can not find the statLog of fileID:%d",_iFileID);
}

void TStatLogMgr::initSecLevelEventType()
{
    if (!m_pSecLevelEventType)
        m_pSecLevelEventType = new KeyTree<int>;
    
    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL ("select event_type_id from ratable_event_type where sum_event_type= "
        "(select nvl(min(event_type_id),0) from ratable_event_type where sum_event_type is null)");
    qry.open();
    
    while (qry.next()) {
        int iEvtTypeID = qry.field("event_type_id").asInteger();
        m_pSecLevelEventType->add(iEvtTypeID,iEvtTypeID);
    }
    qry.close();
}

// 找不到 返回0, 无此事件类型 返回-1
int TStatLogMgr::getParentSecLevelEvtType(int _iEvtTypeID)
{
    int iRet,iEvtTypeID;
    iEvtTypeID = _iEvtTypeID;
    
    if (!m_pSecLevelEventType)
        initSecLevelEventType();
    
   // if (!m_poEventTypeMgr)
   //     m_poEventTypeMgr = new EventTypeMgr();
    
    while (1) {
        if (iEvtTypeID <= 0) return iEvtTypeID;
        if (m_pSecLevelEventType->get(iEvtTypeID,&iRet))
            return iEvtTypeID;
     //   iEvtTypeID = m_poEventTypeMgr->getParent(iEvtTypeID);
    }
    
    return 0;
}


// 返回进程(iProcID)的消息队列中消息数目, 消息队列不存在返回-1
int  MntAPI::getMsgNumber (int iProcID)
{
    TProcessInfo *pt = getProcessInfo (iProcID);
    if (!pt)  return -1;
    
    if (pt->iMqID <= 0)
        return -1;
    
    MessageQueue mq (pt->iMqID);
    if (mq.exist()) {
        return mq.getMessageNumber();
    }
    else
        return -1;
}


// 停进程 iProcID
// bCheckMQ : 是否等待进程自身消息队列数据处理完成再停进程
// iWaitSecs: 等待 iWaitSecs 秒后,仍然未能停止进程, 则返回. (为0时,一直等待)
// 返回
//   1 -- 成功
//   0 -- 进程当前处于未启动状态 (如 sleep,abort等), 不需要执行停止操作
//  -1 -- 消息队列数据超时未处理完
//  -2 -- 已发送停止进程信号, 但是进程超时未退出
//  -3 -- 进程当前状态异常 (如一直处于 正在启动<init>, 正在停止<downing>状态), 
//        不允许发送停止进程信号
//  -4 -- 进程ID不存在
int  MntAPI::stopProcess (int iProcID, int iWaitSecs, bool bCheckMQ)
{
    int iSleep = 0, iCnt = 0;
    
    TProcessInfo *pt = getProcessInfo (iProcID);
    if (!pt)  return -4;
    
    if (pt->iState==ST_KILLED || pt->iState==ST_ABORT || pt->iState==ST_SLEEP)
        return 0;
    
    while (pt->iState==ST_INIT || pt->iState==ST_DESTROY 
        || pt->iState==ST_WAIT_BOOT  || pt->iState==ST_WAIT_DOWN )
    {
        if (iCnt++ > 5)
            return -3;
        sleep (1);
    }
    
    //## 如果有队列,等到队列空
    if (bCheckMQ) {
        while (getMsgNumber(iProcID) > 0) {
            if (iSleep++ > iWaitSecs && iWaitSecs!=0)
                return -1;
            sleep (1);
        }
    }
    
    //## 停止进程
    iSleep = 0;
    if (pt->iState != ST_RUNNING)
        return -4;
    
    Log::log (0, "尝试停止进程:[%s](%d)", pt->sName, iProcID);
    TWfLog::log (0, " @@@@@@@@ 应用进程[%s](%d)尝试停止进程:[%s](%d)", 
            pCurProc->sName,g_iProcessID,  pt->sName,iProcID);
    
    pt->iState = ST_WAIT_DOWN;
    
    while (pt->iState != ST_KILLED && pt->iState != ST_ABORT && pt->iState != ST_SLEEP)
    {
        if (iSleep++ > iWaitSecs  &&  iWaitSecs!=0)
        {
            Log::log (0, "尝试停止进程失败:[%s](%d)", pt->sName, iProcID);
            TWfLog::log (0, " @@@@@@@@ 应用进程[%s](%d)尝试停止进程失败:[%s](%d)", 
                pCurProc->sName,g_iProcessID,  pt->sName,iProcID);
            return -2;
        }
        sleep (1);
    }
    
    return 1;
}
    
    
// 启动进程 iProcID
// 返回
//   1 -- 成功
//   0 -- 当前进程已经处于运行状态, 不需要再次启动
//  -1 -- 启动失败 (进程ID不存在) 
//  -2 -- 当前进程状态异常(不能发送启动信号)
//  -3 -- 发送启动信号后启动不成功
int  MntAPI::startProcess (int iProcID)
{
    int iSleep = 0;
    
    TProcessInfo *pt = getProcessInfo (iProcID);
    if (!pt)  return -1;
    
    if (pt->iState==ST_RUNNING || pt->iState==ST_WAIT_BOOT ||pt->iState==ST_INIT)
        return 0;
    
    if (pt->iState==ST_DESTROY || pt->iState==ST_WAIT_DOWN)
        return -2;
    
    Log::log (0, "尝试启动进程:[%s](%d)", pt->sName, iProcID);
    TWfLog::log (0, " @@@@@@@@ 应用进程[%s](%d)尝试启动进程:[%s](%d)", 
            pCurProc->sName,g_iProcessID,  pt->sName,iProcID);
    
    pt->iState = ST_WAIT_BOOT;
    
    while (pt->iState != ST_RUNNING)
    {
        if (iSleep++ > 5)
            return -3;
        sleep (1);
    }
    
    return 1;
}

TProcessInfo * MntAPI::getProcessInfo (int iProcID)
{
    TProcessInfo *pt;
    
    for (pt = m_pProcInfoHead;  pt;  pt++)
    {
        if (pt->iProcessID == iProcID)
            return pt;
    }
    
    return 0;
}

/////////////////////////////////////

KeyTree<int>* TStatLogMgr::m_pSecLevelEventType = NULL;

//EventTypeMgr* TStatLogMgr::m_poEventTypeMgr = NULL;
	
map<string,TStatLog *> TStatLogMgr::m_mapStatLog;
map<string, TStatLog*>::iterator TStatLogMgr::m_mapIter ;
string TStatLogMgr::m_strMapKey = "";
char TStatLogMgr::m_sTMapKey[64] = {0};
bool TStatLogMgr::m_bForceFlag=true;
	
TStatLog* TStatLogMgr::m_poLog = NULL;

TProcessInfo *MntAPI::m_pProcInfoHead = NULL;
THeadInfo *MntAPI::m_poInfoHead = NULL;

#ifdef _STAT_FILE_LOG
 
char TStatLogMgr::m_sLogCreateTime[16] = {0};

FILE *TStatLogMgr::m_pFile = NULL;

char TStatLogMgr::m_sLogOpenFilePath[300] = {0};
	
char TStatLogMgr::m_sLogCloseFilePath[300] = {0};
	
char TStatLogMgr::m_sLogFileName[100] = {0};
	
char TStatLogMgr::m_sLogFullFileName[400] = {0};
	
int TStatLogMgr::m_iLogRecords = 0;

#endif    
