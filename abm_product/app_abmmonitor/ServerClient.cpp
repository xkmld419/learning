/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Server.h"

static const int TIME_OUT = 500;

static TSHMCMA *g_pShmTable = NULL;


extern "C" { //begin extern "C"

int tpsvrinit(int argc, char **argv)
{
    userlog("\n    server_client starting...");
    ABMException oExp;
    MonitorInfo oIPCMonitor;
    long lShmKey = oIPCMonitor.getInfoIpc(oExp,"SHM_WorkFlow");
		Environment::getDBConn()->commit();    	
    if (lShmKey < 0) {
        userlog("\n    server_client start error when tpsvrinit, getshm err SHM_WorkFlow\n"
                "    获取共享内存KEY失败! 可能表m_b_ipc_cfg未定义, 退出!");
        exit(0);
    }
    
    //获取共享缓冲地址
    if (GetShmAddress(&g_pShmTable,lShmKey)<0 ){
        userlog("\n    server_client start error when tpsvrinit, getshm err shmkey:%d\n"
                "    获取共享缓冲失败,可能主监控守护进程未启动! 服务不能加载!",lShmKey);
        g_pShmTable = NULL;
        exit(0);
    }
    userlog("\n    server_client started.");
    
    return 0;
}


//######################################################################

/***********************************************************************
    交易: CLIENT_LOGIN
    功能：远程监控客户端连接主监控时调用
    输入参数：
        iHostID(客户端主机ID, 对应m_wf_hostinfo.HOST_ID) 
          --客户端守护进程启动时建议默认根据hostname取ID
        sSysUserName(Unix 用户名)
    返回值：
        iLogID(登录ID; -1登录失败)
        sMsg(错误信息. 当iLogID == -1时有效)
----------------------------------------------------------------------*/
void CLIENT_LOGIN(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    if (!iRet){
        ParaList.reset();
        ParaList.setLogID(-1); //输入参数不合法,登录失败
        ParaList.addMsg("登录主监控守护时输入参数不合法！");
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iClientLock,0);
    g_pShmTable->ClientReqInfo.m_iHostID = ParaList.getHostID();
    ParaList.getSysUserName(g_pShmTable->ClientReqInfo.m_sSysUserName);
    g_pShmTable->ClientReqInfo.m_iStatus = REQ_CLIENT_LOGIN;
    
    ParaList.reset();
    int iTimeOut = 0;
    while(g_pShmTable->ClientReqInfo.m_iStatus == REQ_CLIENT_LOGIN)
    { //wait...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) {
			//8月13日加
			//g_pShmTable->ClientReqInfo.m_iStatus = WF_WAIT;
            ParaList.setLogID(-1); //time out,failed
            ParaList.addMsg("登录主监控超时！");
            ParaList.getBuiltStr(sQqstBuf);
            UnlockSafe(g_pShmTable->m_iClientLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    //主守护进程处理后的状态可能有: 
    //SERVER_ACCEPT MAX_CLIENT_NUM SERVER_REJECT
    if (g_pShmTable->ClientReqInfo.m_iStatus == SERVER_ACCEPT){
        ParaList.setLogID(g_pShmTable->ClientReqInfo.m_iIndex);
    }
    else if (g_pShmTable->ClientReqInfo.m_iStatus == MAX_CLIENT_NUM)
    {
        ParaList.setLogID(-1);
        ParaList.addMsg("远程守护监控连接达到最大限制！");
    }
    else {
        ParaList.setLogID(-1);
        ParaList.addMsg(g_pShmTable->ClientReqInfo.m_sMsgBuf);
    }
    ParaList.getBuiltStr(sQqstBuf);
    UnlockSafe(g_pShmTable->m_iClientLock,0);
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: CLIENT_LOGOUT
    功能：远程监控客户端连接主监控时调用
    输入参数：
        iLogID(登录ID)
        iHostID(主机ID)
        sSysUserName(Unix 用户名)
    返回值：
        iResult(1成功; 0失败)
----------------------------------------------------------------------*/
void CLIENT_LOGOUT(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    if (!iRet){
        ParaList.reset();
        ParaList.setResult(0); //输入参数不合法,注销登录失败
        ParaList.addMsg("输入参数不合法！");
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iClientLock,0);
    g_pShmTable->ClientReqInfo.m_iHostID = ParaList.getHostID();
    g_pShmTable->ClientReqInfo.m_iIndex = ParaList.getLogID();
    ParaList.getSysUserName(g_pShmTable->ClientReqInfo.m_sSysUserName);
    g_pShmTable->ClientReqInfo.m_iStatus = REQ_CLIENT_LOGOUT;
    
    ParaList.reset();
    int iTimeOut = 0;
    while(g_pShmTable->ClientReqInfo.m_iStatus == REQ_CLIENT_LOGOUT)
    { //wait...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            ParaList.setResult(0); //time out,failed
            ParaList.addMsg("注销退出主监控超时！");
            ParaList.getBuiltStr(sQqstBuf);            
            UnlockSafe(g_pShmTable->m_iClientLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    //主守护进程处理后的状态可能有: 
    //SERVER_ACCEPT SERVER_REJECT
    if (g_pShmTable->ClientReqInfo.m_iStatus == SERVER_ACCEPT){
        ParaList.setResult(1);
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("注销退出主监控失败！");
    }
    ParaList.getBuiltStr(sQqstBuf);
    UnlockSafe(g_pShmTable->m_iClientLock,0);
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: GET_COMMAND
    功能：远程监控客户端获取其需要执行的指令时调用
    输入参数：
        iLogID(登录ID)
        iHostID(主机ID)
    返回值：
        iResult    (0-失败, 1-有操作指令, 2-当前无指令)
        iOperation (1-start, 2-stop, 3-ChangeStatus, 4-Refresh)
        iProcessID (指令操作的进程号)
        iStaffID
        iPlanID
        cState
        sMsg (iResult=0时存放失败信息; )
----------------------------------------------------------------------*/
void GET_COMMAND(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
		
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    DEFINE_QUERY(qry);
    qry.commit();
		qry.close();
		
    int iRet = ParaList.parseBuff(sQqstBuf);
    if (!iRet){
        ParaList.reset();
        ParaList.setResult(0);
        ParaList.addMsg("输入参数不合法！");
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock signal
    LockSafe(g_pShmTable->m_iClientLock,0);
    g_pShmTable->ClientReqInfo.m_iHostID = ParaList.getHostID();
    g_pShmTable->ClientReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->ClientReqInfo.m_iStatus = REQ_GET_COMMAND;
    
    ParaList.reset();
    int iTimeOut = 0;
    while(g_pShmTable->ClientReqInfo.m_iStatus == REQ_GET_COMMAND)
    { //wait...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            ParaList.setResult(0); //time out
            ParaList.addMsg("获取指令超时！");            
            ParaList.getBuiltStr(sQqstBuf);            
            UnlockSafe(g_pShmTable->m_iClientLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);               
        }
    }
    
    //主守护进程处理后的状态可能有:
    //REQ_COMMAND_RECV CLIENT_LOG_DEAD REQ_NO_COMMAND
    if (g_pShmTable->ClientReqInfo.m_iStatus == REQ_COMMAND_RECV){
        ParaList.setResult(1);
        ParaList.setOperation(g_pShmTable->ClientReqInfo.m_iOperation);
        ParaList.setProcessID(g_pShmTable->ClientReqInfo.m_iProcessID);
        ParaList.setStaffID(g_pShmTable->ClientReqInfo.m_iStaffID);
        ParaList.setPlanID(g_pShmTable->ClientReqInfo.m_iPlanID);
        ParaList.setState(g_pShmTable->ClientReqInfo.m_cState);
    }
    else if (g_pShmTable->ClientReqInfo.m_iStatus == CLIENT_LOG_DEAD){
        ParaList.setResult(0);
        ParaList.addMsg("Err: CLIENT_LOG_DEAD");
    }
    else if (g_pShmTable->ClientReqInfo.m_iStatus == REQ_NO_COMMAND){
        ParaList.setResult(2);
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("Err: Unknown reason.");
    }
    ParaList.getBuiltStr(sQqstBuf);
    UnlockSafe(g_pShmTable->m_iClientLock,0);
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: ACK_COMMAND
    功能：返回指令执行后的结果信息(在GET_COMMAND后调用)
    输入参数：
        iLogID(登录ID)
        iHostID(主机ID)
        iProcessID(指令操作的进程ID)
        iOperation(指令动作) (1-start, 2-stop, 3-ChangeStatus, 4-Refresh)
        iResult(0-失败, 1-成功) 指令的执行情况
        sMsg(指令执行返回的文本信息)
    返回值：
        iResult(0-失败, 1-成功) 交易的执行情况
        //iMainGuardIndex(当指令为启动指令时,标识远程应用进程在主监控守护主机AppInfo中的下标)
        sMsg (失败原因)
----------------------------------------------------------------------*/
void ACK_COMMAND(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    if (!iRet){
        ParaList.reset();
        ParaList.setResult(0); //输入参数不合法
        ParaList.addMsg("输入参数不合法！");
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iClientLock,0);
    g_pShmTable->ClientReqInfo.m_iHostID = ParaList.getHostID();
    g_pShmTable->ClientReqInfo.m_iIndex = ParaList.getLogID();
    g_pShmTable->ClientReqInfo.m_iProcessID = ParaList.getProcessID();
    g_pShmTable->ClientReqInfo.m_iResult = ParaList.getResult();
    g_pShmTable->ClientReqInfo.m_iOperation = ParaList.getOperation();
    strcpy(g_pShmTable->ClientReqInfo.m_sMsgBuf,ParaList.getMsg());
    
    g_pShmTable->ClientReqInfo.m_iStatus = REQ_ACK_COMMAND;
    
    ParaList.reset();
    int iTimeOut = 0;
    while(g_pShmTable->ClientReqInfo.m_iStatus == REQ_ACK_COMMAND)
    { //wait...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            ParaList.setResult(0); //time out,failed
            ParaList.addMsg("返回指令执行结果超时！");            
            ParaList.getBuiltStr(sQqstBuf);            
            UnlockSafe(g_pShmTable->m_iClientLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    //主守护进程处理后的状态可能有:
    // CLIENT_LOG_DEAD ACK_COMMAND_END
    if (g_pShmTable->ClientReqInfo.m_iStatus == ACK_COMMAND_END)
    {
        ParaList.setResult(1);
        //ParaList.setMainGuardIndex(g_pShmTable->ClientReqInfo.m_iMainGuardIndex);
        //ParaList.addMsg("已获取到iMainGuardIndex");
    }
    else if (g_pShmTable->ClientReqInfo.m_iStatus == CLIENT_LOG_DEAD) {
        ParaList.setResult(0);
        ParaList.addMsg("client log dead.");
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("Failed. Unknown reason.");
    }
    ParaList.getBuiltStr(sQqstBuf);
    
    TWfLog::log(0,"server_client send ACK_COMMAND_END msg:%s",sQqstBuf);
    
    UnlockSafe(g_pShmTable->m_iClientLock,0);
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
}


/***********************************************************************
    交易: SEND_REMOTEMSG
    功能：远程监控发送一个应用程序状态和输出信息到主监控
    输入参数：
        iLogID(登录ID)
        iHostID(主机ID)
        iProecessID
        iProcessCnt
        cState(进程状态)
        iMainGuardIndex
        <sMsg=str_msg>...
    返回值：
        iResult(0-失败, 1-成功)
----------------------------------------------------------------------*/
void SEND_REMOTEMSG(TPSVCINFO *rqst)
{
    TParseBuff ParaList;
    char *sQqstBuf;
    sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
    int iLen = rqst->len;
    memcpy(sQqstBuf, rqst->data, iLen);
    sQqstBuf[iLen] = 0;
    TrimHeadNull(sQqstBuf,iLen);
    
    int iRet = ParaList.parseBuff(sQqstBuf);
    if (!iRet){
        ParaList.reset();
        ParaList.setResult(0); //输入参数不合法
        ParaList.addMsg("输入参数不合法！");
        ParaList.getBuiltStr(sQqstBuf);
        tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
    }
    
    //lock the signal.
    LockSafe(g_pShmTable->m_iClientLock,0);
    g_pShmTable->ClientReqInfo.m_iHostID = ParaList.getHostID();
    g_pShmTable->ClientReqInfo.m_iIndex = ParaList.getLogID();
    //g_pShmTable->ClientReqInfo.m_cState = ParaList.getState();
    //strcpy(g_pShmTable->ClientReqInfo.m_sMsgBuf,ParaList.getBuiltMsg());
    strcpy(g_pShmTable->ClientReqInfo.m_sMsgBuf,sQqstBuf);
    
    g_pShmTable->ClientReqInfo.m_iStatus = REQ_SEND_REMOTEMSG;

    ParaList.reset();
    int iTimeOut = 0;    
    while(g_pShmTable->ClientReqInfo.m_iStatus == REQ_SEND_REMOTEMSG)
    { //wait...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) { 
            ParaList.setResult(0); //time out,failed
            ParaList.addMsg("输出消息超时！");            
            ParaList.getBuiltStr(sQqstBuf);            
            UnlockSafe(g_pShmTable->m_iClientLock,0);
            tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);
        }
    }
    
    //主守护进程处理后的状态可能有:
    // CLIENT_LOG_DEAD REMOTEMSG_SENT
    if (g_pShmTable->ClientReqInfo.m_iStatus == REMOTEMSG_SENT)
    {
        ParaList.setResult(1);
    }
    else if (g_pShmTable->ClientReqInfo.m_iStatus == CLIENT_LOG_DEAD) {
        ParaList.setResult(0);
        ParaList.addMsg("client log dead.");
    }
    else {
        ParaList.setResult(0);
        ParaList.addMsg("Failed. Unknown reason.");
    }
    ParaList.getBuiltStr(sQqstBuf);
    UnlockSafe(g_pShmTable->m_iClientLock,0);
    tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);    
}

//######################################################################
} //end extern "C"

