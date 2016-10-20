/*VER: 1*/ 
#include "GuardRemote.h"
#include "Server.h"
#include "mntapi.h"
#include <userlog.h>

extern TSHMCMA    *g_pShmTable;     //通信代理核心表

int          g_iLocalHostID = 0;
int          g_iShmID = 0;               //共享内存标识

TRouteTable  *g_pRouteTable = NULL;   //运行表格

static int   g_iRmtLogID = -1;
static long  g_lRmtWorkFlowKey = 0;
static char  g_sLogName[32];

int InitRmtShm (int iCrtFlg = 1);

void StopGuard(int isignal)
{
    static int iKillSig = 0;
    
    if (iKillSig) {
        LogOutMainGuardSever();
        exit(0);
    }
    
    //接受到信号
    if (isignal>0) {
        iKillSig = isignal;
    }
    
    return;
}

void ShutdownGuard (int iClearShm=0)
{
    char sCmd[100];
    
    int iRet = InitRmtShm(0) ;
    if (iRet <= 0) 
        return;
    
    sprintf (sCmd,"ipcrm -M 0x%x",g_lRmtWorkFlowKey);
    
    TRouteTable *pRT = new TRouteTable();
        
    int iSysPID = g_pShmTable->m_iClientLock;
    
    if (iSysPID <= 0) {
        cout<<"远程监控守护进程GuardRemote 不存在!"<<endl;
        if (iClearShm) {
            pRT->killAllApp();
            cout<<endl<<sCmd<<endl;
            system(sCmd);
        }
        return;
    }
        
    kill (iSysPID,SIGUSR1);
    
    usleep(100000);
    
    while (kill(iSysPID,0) == 0) { //守护进程仍然存在
        static int iTimes =0;
        if (iTimes>3) {
            cout << "\n远程监控守护进程GuardRemote(PID:"<<iSysPID<<")停止失败!"<<endl;
            return;
        }
        iTimes ++;
        sleep(1);
    }
    
    g_pShmTable->m_iClientLock = 0;
    
    TWfLog::log (0,"远程监控守护进程GuardRemote(PID:%d)已停止!",iSysPID);
    cout<<"\n远程监控守护进程GuardRemote(PID:"<<iSysPID<<")已停止!"<<endl;
    
    if (iClearShm) {
        pRT->killAllApp();
        cout<<endl<<sCmd<<endl;
        system(sCmd);
    }
    
    return;
}

bool TpInit(const char *sFunc, int iSleep=10)
{    
    int ret = tpinit((TPINIT *)NULL);
    if ( ret == -1)
    {
        userlog("%s: tpinit error %d %s",sFunc,tperrno,tpstrerror(tperrno));
        TWfLog::log(WF_FILE,"%s: tpinit error %d %s",sFunc,tperrno,tpstrerror(tperrno));
        sleep(iSleep);
        return false;
    }
    return true;
}


bool TpAlloc(char **ppBufptr, const char *sFuncName)
{
    *ppBufptr = tpalloc("STRING",NULL,1*1024);
    if ( *ppBufptr == NULL)
    {
        userlog("%s: tpalloc error %d %s",sFuncName,tperrno,tpstrerror(tperrno));
        TWfLog::log(WF_FILE,"%s: tpalloc error %d %s",sFuncName,tperrno,tpstrerror(tperrno));
        tpterm();
        return false;
    }
    return true;
}


int main(int argc, char ** argv)
{
    char opt;
        
    while ( (opt=getopt(argc,argv,"bdk"))!=EOF){
        switch (opt){
        case 'b': //远程监控守护进程启动 (共享内存 若不存在,则创建; 若存在,直接连接)
            break;
        case 'd': //远程监控守护进程退出 
            ShutdownGuard ();
            exit(1);
        case 'k':
            ShutdownGuard (1);
            exit(1);
        default:
            cout <<"usage: " <<argv[0]<<" [-b]|[-d]|[-k] \n";
            exit(1);
        }
    }
        
    signal(SIGCHLD,SIG_IGN);
    signal(SIGUSR1,StopGuard);

    try {
        
        char sFileName[200];
        char *p = getenv ("TIBS_HOME");
        if (p) {
            strcpy (sFileName, p);
            strcat (sFileName,"/bin/tuxenv.ini");
        }
        else {
            TWfLog::log(WF_FILE,"GuardRemote: getenv $TIBS_HOME error");
            exit(0);
        }
        
        //连接共享内存
        int iRet = InitRmtShm (1);
        if (iRet<0 ) {
            TWfLog::log(WF_FILE,"GuardRemote: get shm err.");
            exit(0);
        }
        
        CheckGuardRmtExist();
        
        g_pShmTable->m_iClientLock = getpid(); //借用, 存放当前远程守护进程的PID
        
        //初始化运行表格
        g_pRouteTable = new TRouteTable;
                
        //get LocalHostID.     

        char *sHostName = NULL;
        if ((sHostName = getenv ("ABM_HOST_NAME")) == NULL)
        {
        	TWfLog::log(WF_FILE,"GuardRemote: Can't get hostname.");
            return 0;
        }
        
  
        g_iLocalHostID = g_pRouteTable->getHostIDByHostName(sHostName);
 
        
        strcpy (g_sLogName,getenv("LOGNAME"));
        
        int iret = tuxreadenv (sFileName,"TUXCOMM");
        if (iret<0) {
            userlog ("Error on tuxreadenv TUXCOMM");
            TWfLog::log (WF_FILE,"Error on tuxreadenv TUXCOMM");
            exit(0);
        }       
        
        //Try log GuardMain
        if (!LogMainGuardServer(g_iLocalHostID,g_sLogName) ) {
            cout<<"连接主监控失败! 远程监控GuardRemote 退出"<<endl;
            return 0;
        }
        
        while(1) { //主循环

            usleep(200000);
            
            //处理本地应用程序登录
            ResponseAppConnect();
            
            //接收,处理主守护服务端指令
            ProcessServerCommand();
            
            //发送本地进程相关信息(输出信息+状态信息)
            SendLocalAppInfo();
                        
            StopGuard(0);
        }        
    }
    catch (TOCIException &e) {
        cout << e.getErrMsg () << endl << e.getErrSrc()<<endl;
        TWfLog::log(WF_FILE, "%s \n %s\n", e.getErrMsg(),e.getErrSrc() );
        LogOutMainGuardSever();
    }
    catch (Exception &e) {
        cout << e.descript() << endl;
        TWfLog::log(WF_FILE, e.descript() );
        LogOutMainGuardSever();
    }
    catch (...) {
        cout << "err." <<endl;
        TWfLog::log(WF_FILE,"远程守护进程(GuardRemote)异常退出，未知错误！");
        LogOutMainGuardSever();
    }
}


/////////////////////////////////////////////////////////////////////

int InitRmtShm (int iCrtFlg)
{
    char sAction[100];
    
    g_pShmTable = 0;
        ABMException oExp;
    MonitorInfo oIPCMonitor;
    
    sprintf (sAction,"--连接--");
        
    g_lRmtWorkFlowKey = oIPCMonitor.getInfoIpc(oExp,"SHM_WorkFlow");
        
    if (g_lRmtWorkFlowKey < 0)
    {
        char sTmp[1000];
        sprintf (sTmp,"获取远程流程调度共享内存地址出错:\n SHM_WorkFlow:%ld \n",
            g_lRmtWorkFlowKey);
        TWfLog::log (WF_FILE,sTmp);
        cout << sTmp <<endl;
        
        return -1;
    }
    Environment::getDBConn()->commit();
    TWfLog::log(0, "初始化远程流程调度共享内存...");
    
    if (GetShmAddress(&g_pShmTable,g_lRmtWorkFlowKey)<0 && iCrtFlg) {
        
        g_iShmID = shmget(g_lRmtWorkFlowKey,sizeof(TSHMCMA),PERM|IPC_CREAT|IPC_EXCL);
        
        if (g_iShmID <0){
            if ((g_iShmID = shmget(g_lRmtWorkFlowKey,0,0))<0){
                TWfLog::log (WF_FILE,"远程流程调度共享内存(%ld)创建失败",g_lRmtWorkFlowKey);
                return -1;
            }
        }
        
        g_pShmTable = (TSHMCMA *)shmat(g_iShmID,(void *)0,0);
        if ((int *)g_pShmTable == (int *)(-1)){
            TWfLog::log (WF_FILE,"远程流程调度共享内存(%ld)连接失败",g_lRmtWorkFlowKey);
            return -1;
        }
        
        memset(g_pShmTable,0,sizeof(TSHMCMA));

        sprintf (sAction,"<<创建>>");
    }        
    
    if (g_pShmTable > 0) 
        strcat (sAction,"成功!");
    else
        strcat (sAction,"失败!");
    
    TWfLog::log (0,"远程流程调度共享内存(%ld)%s",g_lRmtWorkFlowKey, sAction);
    cout<<"远程流程调度共享内存("<<g_lRmtWorkFlowKey<<")"<<sAction<<endl;    
    
    if (g_pShmTable <=0) {
        return -1;
    }

    return 1;
}


bool LogMainGuardServer(int _iHostID, char *sLogName)
{
    int ret;
    long len = 0,flags = 0;
    char *bufptr;
    
    if (!TpInit("Log MainGuardServer",0))
        return false;
    
    if (!TpAlloc(&bufptr,"Log MainGuardServer"))
        return false;
        
    sprintf( bufptr,"<iHostID=%d><sSysUserName=%s>",g_iLocalHostID,sLogName);
    len = strlen(bufptr);
    if (tpcall( "CLIENT_LOGIN",bufptr,len,&bufptr,&len,flags) == -1)
    {
        tpfree(bufptr);
        TWfLog::log(WF_FILE, "Log MainGuardServer: CLIENT_LOGIN service requst fail");
        tpterm();
        return false;
    }
    
    TrimHeadNull(bufptr,len);
    
    TParseBuff ParaList;
    bool bRet = ParaList.parseBuff(bufptr);
    if (!bRet) {
        TWfLog::log(WF_FILE, "Log MainGuardServer err when parseBuff(%s)",bufptr);
        return false;
    }
    g_iRmtLogID = ParaList.getLogID();
    if (g_iRmtLogID == -1) {
        TWfLog::log(WF_FILE, "Log MainGuardServer err when getLogID(). ");
        TWfLog::log(WF_FILE, ParaList.getMsg());
        return false;
    }
    
    tpfree(bufptr);
    ret = tpterm();
    if ( ret == -1)
    {
        cout<<"Log MainGuardServer: Error in exiting application\n";
        userlog("Log MainGuardServer: tpterm error %d %s",tperrno,tpstrerror(tperrno));
        TWfLog::log(WF_FILE,"Log MainGuardServer: tpterm error %d %s",tperrno,tpstrerror(tperrno));
        return true; //虽然断开服务连接失败，但是远程监控登录主监控成功
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////
bool GetCommand(char *_sCmd)
{
    int ret;
    long len = 0,flags = 0;
    char *bufptr;
    
    if (!TpInit("Get_Command"))
        return false;
    
    if (!TpAlloc(&bufptr,"Get_Command"))
        return false;
        
    sprintf( bufptr,"<iHostID=%d><iLogID=%d>",g_iLocalHostID,g_iRmtLogID);
    len = strlen(bufptr);
    if (tpcall( "GET_COMMAND",bufptr,len,&bufptr,&len,flags) == -1)
    {
        tpfree(bufptr);
        TWfLog::log(WF_FILE, "Get_Command: GET_COMMAND service requst fail");
        tpterm();
        return false;
    }
    
    TrimHeadNull(bufptr,len);
    strcpy(_sCmd, bufptr);    
    tpfree(bufptr);
    tpterm();    
    
    return true;
}

/////////////////////////////////////////////////////////////////////

bool AckCommand(int _iOperation, char _cState, int _iProcID,int _iResultID,
    int &_iMainGuardIndex, char _sMsg[])
{
    int ret;
    long len = 0,flags = 0;
    char *bufptr;
    
    if (!TpInit("Ack_Command"))
        return false;
    
    if (!TpAlloc(&bufptr,"Ack_Command"))
        return false;
        
    sprintf( bufptr,"<iHostID=%d><sSysUserName=%s><iLogID=%d><iProcessID=%d>"
        "<iResult=%d><iOperation=%d>",
        g_iLocalHostID,g_sLogName,g_iRmtLogID,_iProcID,_iResultID,_iOperation);
    
    if (_iOperation == 3) { //change status
        char sTemp[100];
        sprintf(sTemp,"<cState=%c>",_cState);
        strcat(bufptr,sTemp);
    }
    
    if (strlen(_sMsg) )
    sprintf( bufptr,"%s<sMsg=%s>",bufptr,_sMsg);
    len = strlen(bufptr);
    
    //cout<<"Ack_Command sent bufptr: "<<bufptr<<endl;
    
    if (tpcall( "ACK_COMMAND",bufptr,len,&bufptr,&len,flags) == -1)
    {
        tpfree(bufptr);
        TWfLog::log(WF_FILE, "Ack_Command: ACK_COMMAND service requst fail");
        tpterm();
        return false;
    }

    tpfree(bufptr);
    tpterm();    
    
    //分析交易返回结果
    TrimHeadNull(bufptr,len);
    TParseBuff ParaList;
    bool bRet = ParaList.parseBuff(bufptr);
    if (!bRet) {
        TWfLog::log(WF_FILE, "Ack_Command err when parseBuff(%s)",bufptr);
        return false;
    }
    
    //cout<<"Ack_Command ret bufptr: "<<bufptr<<endl;
    //cout<<"Ack_Command ret bufptr[5]: "<<(bufptr+5)<<endl;
    
    int iResult = ParaList.getResult();
    if (iResult == 0) //交易失败
        return false;
    
    _iMainGuardIndex = ParaList.getMainGuardIndex();
    strcpy(_sMsg, ParaList.getMsg() );
    
    return true;
}

/////////////////////////////////////////////////////////////////////

void ProcessServerCommand()
{
    char sCommand[1024];
    char sMsgBuff[1024];
    sMsgBuff[0] = 0;
    TParseBuff ParaList;
    int iExecResult;
    
    if (!GetCommand(sCommand) ) {
        TWfLog::log(WF_FILE,"Process_ServerCommand: can't Get_Command");
        return;
    }
        
    bool bRet = ParaList.parseBuff(sCommand);
    if (!bRet) {
        TWfLog::log(WF_FILE,"Process_ServerCommand:  parseBuff(sCommand)");
        TWfLog::log(WF_FILE,"sCommand:%s",sCommand);
        usleep(500000); //sleep 0.5 second.
        return;
    }
    
    int iResult = ParaList.getResult();
    
    if (iResult == 2) //no command.
        return ;

    cout<<"Get command: "<<sCommand<<endl;

    
    int iOperation = ParaList.getOperation();
    int iProcessID = ParaList.getProcessID();
    int iStaffID = ParaList.getStaffID();
    int iPlanID =  ParaList.getPlanID();
    char cState = ParaList.getState();
    
    char sSysUserName[32]; sSysUserName[0]=0;
    
    int iBelongHostID = g_pRouteTable->getHostIDByProcessID(iProcessID, sSysUserName);
    
    if (iOperation!=4 && (iBelongHostID != g_iLocalHostID || strcmp(sSysUserName,g_sLogName)))
    {
        TWfLog::log(WF_FILE,"Process_ServerCommand error: \n"
            " iBelongHostID(%d) != g_iLocalHostID(%d)\n"
            " sSysUserName(%s) != LogName(%s) "
            " (非本机部署的应用进程)", iBelongHostID, g_iLocalHostID,sSysUserName,g_sLogName);
        return;
    }
    
    switch(iResult) {
        case 0: //fail
            TWfLog::log(WF_FILE,"Process_ServerCommand: get command err! errMsg:%s",
                ParaList.getMsg() );
            usleep(500000);
            return;
        
        case 2: //no command.
            return;
        
        case 1: //command exist
        
            int iProcessLogID ;
            
            switch(iOperation) {
                case 1: //-start
                    iProcessLogID = TLogProcessMgr::insertLogProcess(iProcessID,sCommand,iStaffID,iPlanID);
                    iExecResult = g_pRouteTable->startApp(iProcessID,iProcessLogID);
                    break;
                case 2: //-stop
                    iExecResult = g_pRouteTable->stopApp(iProcessID);
                    break;
                case 3: //-ChangeStatus
                    iExecResult = g_pRouteTable->changeState(iProcessID,cState);
                    g_pShmTable->changeState(iProcessID,cState);
                    break;
                case 4: //-Refresh
                    g_pRouteTable->reload();
                    iExecResult = 1;
                    break;
                default:
                    TWfLog::log(WF_FILE,"Process_ServerCommand: invalid iOperation:%d",iOperation);
                    iExecResult = 0;
            }
            
            break;
            
        default:
            TWfLog::log(WF_FILE,"Process_ServerCommand: get command err, unknown iResult.");
            usleep(500000);
            return;            
    }
    
    //iExecResult,指令的执行结果(0-失败, 1-成功)
    int iMainGuardIndex;
    if (!AckCommand(iOperation,cState,iProcessID,iExecResult,iMainGuardIndex,sMsgBuff) ) 
    { //交易失败
        TWfLog::log(WF_FILE,"Process_ServerCommand: can't Ack_Command");
        return;
    }
    
    if (iExecResult == 0)// 指令执行未成功,不管Ack结果
        return;
    
    if (iOperation == 1 && iMainGuardIndex>= 0) {//返回的是启动指令执行结果 and 主守护进程分配了下标
        g_pShmTable->setMainGuardIndex(iProcessID,iMainGuardIndex);
    }
    
    return;
}

/////////////////////////////////////////////////////////////////////
// 输出每个本机进程的至多5条消息
void SendLocalAppInfo()
{
    int ret;
    long len = 0,flags = 0;
    char *bufptr;
    char stbuf[2048];
    
    if (!TpInit("Send_LocalAppInfo"))
        return;
        
    if (!TpAlloc(&bufptr,"Send_LocalAppInfo"))
        return;
        
    sprintf (stbuf,"<iHostID=%d><iLogID=%d><sSysUserName=%s>",
        g_iLocalHostID,g_iRmtLogID,g_sLogName);
    
    char sTemp[MAX_APPINFO__LEN*5];
    
    for (int iAppIdx=0; iAppIdx<MAX_APP_NUM; iAppIdx++) { //应用进程循环
        
        if (g_pShmTable->AppInfo[iAppIdx].m_iProcessID < 0) //节点未被启用过
            continue;

        //> 检查是否需要输出该进程信息
        if ( !g_pShmTable->AppInfo[iAppIdx].checkNeedOutput())
            continue;

        TParseBuff ParaList;
        ParaList.reset();
        
        //格式化应用进程信息
        ParaList.setProcessID(g_pShmTable->AppInfo[iAppIdx].m_iProcessID);
        ParaList.setState(g_pShmTable->AppInfo[iAppIdx].getState());
        ParaList.setProcessCnt(g_pShmTable->AppInfo[iAppIdx].m_lProcessCnt);
        //ParaList.setMainGuardIndex(g_pShmTable->AppInfo[iAppIdx].m_iMainGuardIndex);
        
        for(int i=0; i<5; i++) { //每次单个应用进程最多输出5条
            char str[MAX_APPINFO__LEN];
            if ( g_pShmTable->AppInfo[iAppIdx].getInfo(str) )
                ParaList.addMsg(str);
            else break;
        }
        ParaList.getBuiltStr(sTemp);
        
        char sTstr[1024];
        sprintf(bufptr,"%s%s",stbuf,sTemp);
        bufptr[1023] = 0;
        strcpy(sTstr,bufptr);
        len = strlen(bufptr);
        if (tpcall( "SEND_REMOTEMSG",bufptr,len,&bufptr,&len,flags) == -1)
        {
            TWfLog::log(WF_FILE, "Send_LocalAppInfo: SEND_REMOTEMSG(%s) service requst fail",sTstr);
        }
        
        //... 交易是否成功的判断 ...
        TrimHeadNull(bufptr,len);
        ParaList.parseBuff(bufptr);
        if (ParaList.getResult()!=1) {
            TWfLog::log(WF_FILE, "Send_LocalAppInfo: SEND_REMOTEMSG(%s) Error!",sTstr);
        }
        
        //cout<<"Rmt Send RemoteMsg:"<<sTstr<<endl;
    }
    
    tpfree(bufptr);
    tpterm();
}

/////////////////////////////////////////////////////////////////////

bool LogOutMainGuardSever()
{
    int ret;
    long len = 0,flags = 0;
    char *bufptr;
    
    if (!TpInit("LogOut_MainGuardSever"))
        return false;
    
    if (!TpAlloc(&bufptr,"LogOut_MainGuardSever"))
        return false;
        
    sprintf( bufptr,"<iHostID=%d><iLogID=%d><sSysUserName=%s>",
        g_iLocalHostID,g_iRmtLogID,g_sLogName);
    len = strlen(bufptr);
    if (tpcall( "CLIENT_LOGOUT",bufptr,len,&bufptr,&len,flags) == -1)
    {
        tpfree(bufptr);
        TWfLog::log(WF_FILE, "LogOut_MainGuardSever: CLIENT_LOGOUT service requst fail");
        tpterm();
        return false;
    }
    
    TWfLog::log(0, "LogOut_MainGuardSever: CLIENT_LOGOUT service requst Success!");
    
    tpfree(bufptr);
    tpterm();    
    
    return true;
}


void CheckGuardRmtExist()
{    
    if (g_pShmTable) {
        
        int iSysPID = g_pShmTable->m_iClientLock;
        
        if (iSysPID <= 0) return;
        
        if (kill(iSysPID,0) == 0) { //监控守护进程存在
            TWfLog::log (0,"远程监控守护进程GuardRemote(PID:%d)已存在, 不需要再次启动!",iSysPID);
            cout << "远程监控守护进程GuardRemote(PID:"<<iSysPID<<")已存在,不需要再次启动!"<<endl;
            exit (0);
        }
    } 
}


/////////////////////////////////////////////////////////////////////

