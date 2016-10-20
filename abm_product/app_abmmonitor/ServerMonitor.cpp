/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Server.h"

static const int TIME_OUT = 500;

static TSHMCMA *g_pShmTable = NULL;


extern "C" {

int tpsvrinit(int argc, char **argv)
{
    userlog("\n    server_monitor starting...");
    ABMException oExp;
    MonitorInfo oIPCMonitor;
    long lShmKey = oIPCMonitor.getInfoIpc(oExp,"SHM_WorkFlow");
    if (lShmKey < 0) {
        userlog("\n    server_monitor start error when tpsvrinit, getshm err shmkey\n"
                "    获取共享内存KEY失败! 可能表m_b_ipc_cfg未定义, 退出!");
        exit(0);
    }
    
    //获取共享缓冲地址
    if (GetShmAddress(&g_pShmTable,lShmKey)<0 ){
        userlog("\n    server_monitor start error when tpsvrinit, getshm err shmkey:%d\n"
                "    连接共享内存失败! 服务不能加载! 退出!",lShmKey);
        g_pShmTable = NULL;
        exit(0);
    }
    userlog("\n    server_monitor started.");
    Environment::getDBConn()->commit();
    return 0;
}

//######################################################################

/***********************************************************************
    交易: MONITOR_LOGIN
    功能：前台监控Monitor计费流程登录主监控守护时调用
    输入参数：
        //##iBillFlowID(计费流程ID) (此参数已取消)
        sOrgID(控制的OrgID列表, 输入格式 <sOrgID=1|23|4|15|...|n>)
        sHostAddr(前台监控主机IP地址);
        iStaffID(前台监控登录员工号);
        iLogMode(登录模式：1-旁观登录,只能查看; 2-强行监控,可以执行指令)
    返回值：
        iResult(0-注册失败; 1-注册成功; 2-参数不合法)
        iLogID(登录ID)
        iAuthID(身份认证ID)
----------------------------------------------------------------------*/
void MONITOR_LOGIN(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getStaffID()<0 ||    ParaList.getLogMode()<0 ){
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iBillFlowID = ParaList.getBillFlowID();
    g_pShmTable->MntReqInfo.m_iStaffID = ParaList.getStaffID();
    g_pShmTable->MntReqInfo.m_iLogMode = ParaList.getLogMode();
    ParaList.getHostAddr(g_pShmTable->MntReqInfo.m_sHostAddr);
    ParaList.getOrgIDStr(g_pShmTable->MntReqInfo.m_sOrgIDStr);
    
    g_pShmTable->MntReqInfo.m_iStatus = MNT_REQ_REGISTER;

    int iTimeOut = 0;
   	while( g_pShmTable->MntReqInfo.m_iStatus == MNT_REQ_REGISTER)
    {//等待主守护进程处理...
				Environment::getDBConn()->commit();
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }

    ParaList.reset();
    
    //主守护进程处理后的状态可能有: REG_REJECT_MAXNUM,REG_ACCEPT
    if (g_pShmTable->MntReqInfo.m_iStatus == REG_ACCEPT) {
        ParaList.setResult(1);
        ParaList.setLogID(g_pShmTable->MntReqInfo.m_iIndex);
        ParaList.setAuthID(g_pShmTable->MntReqInfo.m_iAuthID);
    }
    else {
        ParaList.setResult(0);
    }
    
    ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);

    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: MONITOR_LOGOUT
    功能：前台监控Monitor的计费流程退出监控时调用
    输入参数：
        iLogID(登录ID)
        iAuthID(身份认证ID)
    返回值：
        iResult(0-失败; 1-成功; 2-参数不合法; 3-该连接不存在)
----------------------------------------------------------------------*/
void MONITOR_LOGOUT(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);

    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0){
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);

    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    g_pShmTable->MntReqInfo.m_iStatus = MNT_REQ_UNREGISTER;
    
    int iTimeOut = 0;
    while (g_pShmTable->MntReqInfo.m_iStatus == MNT_REQ_UNREGISTER)
    {//等待主守护进程处理...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }

    ParaList.reset();
    
    //主守护进程处理后的状态可能有: MNT_LOG_DEAD,REG_UNREGISTERED
    if (g_pShmTable->MntReqInfo.m_iStatus == REG_UNREGISTERED ) {
        ParaList.setResult(1); //success.
    }
    else {
        ParaList.setResult(3); //connection dead.
    }

    ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);

    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    
}


/***********************************************************************
    交易: RECV_APPINFO
    功能：前台监控Monitor接收当前计费流程对应的应用进程相关信息
        (每次只输出一个process的信息,包括状态信息/进程输出信息/处理量)
    输入参数：
        iLogID(登录ID)
        iAuthID(身份认证ID)
    返回值：
      iResult(0-失败; 1-成功; 2-参数不合法; 3-该连接不存在)
      iProcessID(进程ID)
      cState(进程状态)
      iProcessCnt(进程处理数据量)  
        ps:iProcessCnt在后台是无符号长整型的值,当达到最大值后,会自动归0
      sMsg[多项](进程输出信息)表达格式为:<sMsg=s1><sMsg=s2>...<sMsg=sN>
          ps:(没有信息时为sMsg空)

Mod 2005/06/13
RECV_APPINFO 返回的格式：
格式这样：(第一节 只有iResult,不同节的iProcessID可能相同)
<iResult=1>$<iProcessID=11><...>$<iProcessID=12><...>$<iProcessID=11><...>
----------------------------------------------------------------------*/
void RECV_APPINFO(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0)
    {
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    g_pShmTable->MntReqInfo.m_iStatus = MNT_REQ_APPINFO;
    
    int iTimeOut = 0;
    while( g_pShmTable->MntReqInfo.m_iStatus == MNT_REQ_APPINFO) 
    {//等待主守护进程处理...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }

    //主守护进程处理后的状态可能有: MNT_LOG_DEAD,REQ_APPINFO_FILLED
    if (g_pShmTable->MntReqInfo.m_iStatus == REQ_APPINFO_FILLED) {
        //ParaList.parseBuff(g_pShmTable->MntReqInfo.m_sMsgBuf);
        //ParaList.setResult(1); //success.    
        sprintf(sQqstBuf,"<iResult=1>%s",g_pShmTable->MntReqInfo.m_sMsgBuf);
    }
    else {
        ParaList.setResult(3); //connection dead.
        ParaList.getBuiltStr(sQqstBuf);
    }

    //ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);
    
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


void RECV_PASSINFO(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0)
    {
        ParaList.reset();
        ParaList.setResult(2); //JdHk2NJ}2;:O7(
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
	LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();        g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    g_pShmTable->MntReqInfo.m_iMqID = ParaList.getMqId();        g_pShmTable->MntReqInfo.m_iFlag = ParaList.getFlag();	 g_pShmTable->MntReqInfo.m_iProcessID = ParaList.getProcessID();        g_pShmTable->MntReqInfo.m_iStatus = MNT_REQ_PASSINFO;
    int iTimeOut =0;
     while( g_pShmTable->MntReqInfo.m_iStatus == MNT_REQ_PASSINFO) 
    {
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    if (g_pShmTable->MntReqInfo.m_iStatus == REQ_PASSINFO_FILLED) {
        sprintf(sQqstBuf,"<iResult=1>$%s",g_pShmTable->MntReqInfo.m_sMsgBuf);
    }
    else
    {if (g_pShmTable->MntReqInfo.m_iStatus == NO_PASSINFO)
       {
       ParaList.setResult(3);
       ParaList.addMsg("NO PASSINFO");
       ParaList.getBuiltStr(sQqstBuf);
       
    }
    else {
        ParaList.setResult(3); //connection dead.
        ParaList.getBuiltStr(sQqstBuf);
    }
   } 
    //ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);
    
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: SEND_COMMAND
    功能: 前台监控Monitor发送执行指令(启动/停止后台应用程序)时调用
    输入参数:
        iLogID(登录ID)
        iAuthID(身份认证ID)
        iProcessID(对应m_wf_process.PROCESS_ID)
        iOperation(操作 1-启动; 2-停止)
    返回值:
        iResult(0-失败; 1-成功; 2-参数不合法; 3-该连接不存在, 4-进程已经存在)
        sMsg( iRuesult = 0 时的失败信息)
----------------------------------------------------------------------*/
void SEND_COMMAND(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0){
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    g_pShmTable->MntReqInfo.m_iProcessID = ParaList.getProcessID();
    g_pShmTable->MntReqInfo.m_iOperation = ParaList.getOperation();
    strcpy( g_pShmTable->MntReqInfo.m_sMsgBuf,sQqstBuf);
    
    g_pShmTable->MntReqInfo.m_iStatus = REQ_SEND_COMMAND; //请求执行指令
    
    int iTimeOut = 0;
    while (g_pShmTable->MntReqInfo.m_iStatus == REQ_SEND_COMMAND ||
        g_pShmTable->MntReqInfo.m_iStatus == REQ_COMMAND_SENDING)
    {//waiting...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            if (g_pShmTable->MntReqInfo.m_iStatus == REQ_SEND_COMMAND)
                ParaList.addMsg("主监控未响应");
            else
                ParaList.addMsg("指令发送给远程监控异常");
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    ParaList.reset();
    
    //主守护进程处理后的状态可能有: 
    //MNT_LOG_DEAD,NO_PRIVILEGE,REG_APP_MAXNUM,
    //REQ_COMMAND_SENT,COMMAND_EXEC_ERR,REMOTECMA_NOT_START
    if (g_pShmTable->MntReqInfo.m_iStatus == MNT_LOG_DEAD) 
        ParaList.setResult(3); //connection dead.
    else if (g_pShmTable->MntReqInfo.m_iStatus == REQ_COMMAND_SENT)
        ParaList.setResult(1); //success.
    else if (g_pShmTable->MntReqInfo.m_iStatus == NO_PRIVILEGE) {
        ParaList.setResult(0);
        ParaList.addMsg("对不起，你无执行该指令的权限");
    }
    else if (g_pShmTable->MntReqInfo.m_iStatus == REG_APP_MAXNUM) {
        ParaList.setResult(0);
        ParaList.addMsg("对不起，后台执行的应用进程达到系统定义的最大限制");
    }
    else if (g_pShmTable->MntReqInfo.m_iStatus == REMOTECMA_NOT_START) {
        char sTemp[1024];
        ParaList.setResult(0);
        sprintf (sTemp," 远程守护进程GuardRemote尚未启动!\n"
            "\nAction: 请在相应的主机用户[%s]下执行 'rmnt -b'\n",
            g_pShmTable->MntReqInfo.m_sMsgBuf
        );
        ParaList.addMsg(sTemp);        
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("指令执行失败");
        ParaList.addMsg(g_pShmTable->MntReqInfo.m_sMsgBuf);
    }
    
    ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);

    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: CHANGE_STATUS
    功能: 前台监控Monitor发送强行改变进程节点状态指令时调用
    输入参数:
        iLogID(登录ID)
        iAuthID(身份认证ID)
        iProcessID(对应m_wf_process.PROCESS_ID)
        cState(更改的目标状态) (A-未启动 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令)
    返回值:
        iResult(0-失败; 1-成功; 2-参数不合法; 3-该连接不存在)
----------------------------------------------------------------------*/
void CHANGE_STATUS(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    char cState = ParaList.getState();
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0
        || ( cState!='A' && cState!='R' && cState!='E' )
    )
    {
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    g_pShmTable->MntReqInfo.m_iProcessID = ParaList.getProcessID();
    g_pShmTable->MntReqInfo.m_cState = ParaList.getState();
    //strcpy( g_pShmTable->MntReqInfo.m_sMsgBuf, sQqstBuf);
    sprintf (g_pShmTable->MntReqInfo.m_sMsgBuf,"<iOperation=3>%s",sQqstBuf);
    
    g_pShmTable->MntReqInfo.m_iStatus = REQ_CHANGESTATUS; //请求执行指令
    
    int iTimeOut = 0;
    while (g_pShmTable->MntReqInfo.m_iStatus == REQ_CHANGESTATUS ||
        g_pShmTable->MntReqInfo.m_iStatus == REQ_STATUS_CHANGEING)
    {//waiting...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    ParaList.reset();
    
    //主守护进程处理后的状态可能有: 
    //MNT_LOG_DEAD, NO_PRIVILEGE, REMOTECMA_NOT_START
    //REQ_CHANGESTATUS_OVER, REQ_CHANGESTATUS_ERR
    if (g_pShmTable->MntReqInfo.m_iStatus == MNT_LOG_DEAD) 
        ParaList.setResult(3); //connection dead.
    else if (g_pShmTable->MntReqInfo.m_iStatus == REQ_CHANGESTATUS_OVER)
        ParaList.setResult(1); //success.
    else if (g_pShmTable->MntReqInfo.m_iStatus == NO_PRIVILEGE) {
        ParaList.setResult(0);
        ParaList.addMsg("对不起，你无执行该指令的权限");
    }
    else if (g_pShmTable->MntReqInfo.m_iStatus == REMOTECMA_NOT_START) {
        ParaList.setResult(0);
        ParaList.addMsg("远程守护进程未启动");
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("状态改变指令执行失败");
    }
    
    ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);

    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);

}


/***********************************************************************
    交易: REFRESH_RUNPLAN
    功能: 前台监控Monitor发送执行指令(更新运行计划表格)时调用
    输入参数:
        iLogID(登录ID)
        iAuthID(身份认证ID)
    返回值:
        iResult(0-失败; 1-成功; 2-参数不合法; 3-该连接不存在)
----------------------------------------------------------------------*/
void REFRESH_RUNPLAN(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);

    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    
    if (!iRet || ParaList.getLogID()<0 || ParaList.getAuthID()<0){
        ParaList.reset();
        ParaList.setResult(2); //输入参数不合法
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iMonitorLock,0);
    
    g_pShmTable->MntReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->MntReqInfo.m_iAuthID = ParaList.getAuthID();
    //strcpy( g_pShmTable->MntReqInfo.m_sMsgBuf,sQqstBuf);
    
    g_pShmTable->MntReqInfo.m_iStatus = REQ_REFRESH_RUNPLAN; //请求执行指令
    
    int iTimeOut = 0;
    while (g_pShmTable->MntReqInfo.m_iStatus == REQ_REFRESH_RUNPLAN ||
        g_pShmTable->MntReqInfo.m_iStatus == REFRESHING_RUNPLAN)
    {//waiting...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            g_pShmTable->MntReqInfo.m_iStatus = WF_WAIT;
            ParaList.reset();
            ParaList.setResult(0); //time out,failed
            ParaList.getBuiltStr(sQqstBuf);
            
            UnlockSafe(g_pShmTable->m_iMonitorLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    ParaList.reset();
    
    //主守护进程处理后的状态可能有: 
    //MNT_LOG_DEAD, NO_PRIVILEGE, RUNPLAN_REFRESHED, REFRESHED_ERR
    if (g_pShmTable->MntReqInfo.m_iStatus == MNT_LOG_DEAD) 
        ParaList.setResult(3); //connection dead.
    else if (g_pShmTable->MntReqInfo.m_iStatus == RUNPLAN_REFRESHED)
        ParaList.setResult(1); //success.
    else if (g_pShmTable->MntReqInfo.m_iStatus == NO_PRIVILEGE) {
        ParaList.setResult(0);
        ParaList.addMsg("对不起，你无执行该指令的权限");
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("刷新运行计划指令执行失败");
    }
    
    ParaList.getBuiltStr(sQqstBuf);

    //unlock signal.
    UnlockSafe(g_pShmTable->m_iMonitorLock,0);

    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


//######################################################################

} //--End extern "C"
