#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include "ABMException.h"
#include "LogV2.h"
#include "HSSLogV3.h"
#include "Application.h"
#include "mntapi.h"

#ifdef ATTACH_GUARD_MAIN
#include "SrvSHMMgr.h"
__HSS::TAppInfo *g_poAppInfo = NULL;
__HSS::TSHMCMA *g_poGuard = NULL;
__HSS::THeadInfo *g_poHeadInfo = NULL;
__HSS::TProcessInfo *g_poProcInfo = NULL;        
#endif


#define HSS_HOME "HSSDIR"

int g_iModuID = -1;
int g_iProcID = -1;
int g_iNodeID = -1;
char g_sAppName[32] = {0}; 
volatile sig_atomic_t g_sigReceived = 0;
volatile sig_atomic_t g_iAlarmRcv = 0;

bool ReceiveStop(void);
void SigHandler(int sig);

void SigHandler(int sig)
{
	g_sigReceived = sig;
}//modify by nwp 060310

bool RcvExcepSig(void) {
        if (g_sigReceived) {
                return g_sigReceived != SIGTERM;
        }
        return false;
}

bool ReceiveStop(void)
{
	return (g_sigReceived==0)? false: true;
}

static void catchAlarmSig(int sig)
{
    g_iAlarmRcv = 0;   		
    return;
}

#ifdef ATTACH_GUARD_MAIN

static int connectMonitor(ABMException &oExp)
{
    if (SRVSHM(openGuardSHM(oExp)) != 0)
        return -1;    
    g_poGuard = SRVSHM(getGuardData());
    LockSafe(g_poGuard->m_iAppLock, 0);
    g_poGuard->AppReqInfo.m_iProcessID = g_iProcID;
    g_poGuard->AppReqInfo.m_iAppPID = getpid();
    g_poGuard->AppReqInfo.m_iStatus = APP_REG_REGISTER;  
    for (int i=0; ; ++i) {     
        if (i >= 1000) {
            ADD_EXCEPT0(oExp, "应用程序GUARD_MAIN注册失败 ！");
            return -1;    
        }
        if (g_poGuard->AppReqInfo.m_iStatus == APP_REG_ACCEPT)
            break;
        else if (g_poGuard->AppReqInfo.m_iStatus == APP_REG_REGISTER)    
            usleep(10000);
        else {    
            ADD_EXCEPT0(oExp, "应用程序GUARD_MAIN注册失败 ！");
            return -1;
        }      
    }
    UnlockSafe(g_poGuard->m_iAppLock,0);    
}

static void disconnMonitor(char cState='E')
{
    LockSafe(g_poGuard->m_iAppLock,0);
    g_poGuard->AppReqInfo.m_iProcessID = g_iProcID;
    
    if (g_poAppInfo)
        g_poGuard->AppReqInfo.m_iAppPID = g_poAppInfo->m_iAppPID;
        
    g_poGuard->AppReqInfo.m_cState = cState;
    g_poGuard->AppReqInfo.m_iStatus = APP_REG_UNREGISTER;
    
    for (int i=0; ;i++) {
        if ((g_poGuard->AppReqInfo.m_iStatus!=APP_REG_UNREGISTER) || (i>1000))
            break;
        else
            usleep(10000);        
    }
    UnlockSafe(g_poGuard->m_iAppLock,0);
    //shmdt((char*)g_poGuard);    
}

static int connectTerminal(ABMException &oExp)
{
    int i;
    if (SRVSHM(openTerminalSHM(oExp)) != 0)
        return -1;
    g_poHeadInfo = SRVSHM(getTerminalData());
    g_poProcInfo = &g_poHeadInfo->ProcInfo;
    for (i=0; i<g_poHeadInfo->iProcNum; i++) {
        if (g_poProcInfo->iProcessID == g_iProcID)
            break;
        g_poProcInfo++;
    }

    if (i >= g_poHeadInfo->iProcNum) {
        g_poProcInfo = NULL;
        return -1;
    }

    time ((time_t *)&g_poProcInfo->lLoginTime);
    
    g_poProcInfo->tSaveTime = g_poProcInfo->lLoginTime;
    g_poProcInfo->lSaveTickets = 0;
    
    g_poProcInfo->iAllTickets = 0;
    g_poProcInfo->iNormalTickets = 0;
    g_poProcInfo->iErrTickets = 0;
    g_poProcInfo->iBlackTickets = 0;
    g_poProcInfo->iOtherTickets = 0;

    g_poProcInfo->iSysPID = getpid ();

    g_poProcInfo->iState = ST_RUNNING;
    return 0;          
}

static void disconnTerminal(int iState=3)
{
    if (g_poProcInfo) {
        time ((time_t *)&g_poProcInfo->lLoginTime);
        g_poProcInfo->iState = iState;
    }    
}
/*
#else

#define connectMonitor(x) 0
#define disconnMonitor(x) 
#define connectTerminal(x) 0
#define disconnTerminal(x)*/
#endif

int SigHandlerSet(int sig, void (*handler)(int))
{
    struct sigaction new_action;
    struct sigaction old_action;
    
    memset(&new_action, 0x00, sizeof(new_action));
    memset(&old_action, 0x00, sizeof(old_action));

    new_action.sa_handler = handler;
    sigemptyset(&new_action.sa_mask);
    sigfillset (&new_action.sa_mask);

    if (sigaction(sig, &new_action, &old_action) != 0) 
    	return -1;	
    
    return 0;
}

int HandleSigactions()
{
    //阻塞SIGHUP, SIGINT, SIGQUIT
    sigset_t mask_action;
    sigemptyset(&mask_action);
    sigaddset(&mask_action, SIGHUP);
    sigaddset(&mask_action, SIGINT);
    sigaddset(&mask_action, SIGQUIT);
    /*SOCK_STREAM socket, 向一个对端主动关闭 或者本端关闭的fd
     发送数据时, 会产生SIGPIPE信号*/
    sigaddset(&mask_action, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &mask_action, 0) != 0) {
    	perror("sigprocmask");
    	return -1;	
    }
    //安全退出信号
    if (SigHandlerSet(SIGTERM, SigHandler) != 0) 
    	return -1;	
    //增加超时控制的信号注册
    if (SigHandlerSet(SIGALRM, catchAlarmSig) != 0)
        return -1;
    return 0;	

}


Application::Application()
{
	memset(m_sEnvAppDly, 0x00, sizeof(m_sEnvAppDly));
	memset(m_sHostName, 0x00, sizeof(m_sHostName));	
}

int Application::appInit(ABMException &oExp)
{
    //初始化运行环境
    char *p = getenv(HSS_HOME);
    if (p == NULL) {
        ADD_EXCEPT1(oExp, "环境变量%s 没有设置，程序初始化失败!", HSS_HOME);
        ADD_LOG(V3_FATAL, V3_SYS_MGR, MBC_ENV_NOT_SET, "环境变量%s 没有设置，程序初始化失败!", HSS_HOME);
        return -1;
    }
    strncpy(m_sEnvAppDly, p, sizeof(m_sEnvAppDly)-2);
    if (m_sEnvAppDly[strlen(m_sEnvAppDly)-1] != '/') 
        m_sEnvAppDly[strlen(m_sEnvAppDly)] = '/';
	char *pserver = getenv("HSSSERVER");
	if (!pserver || strcmp(pserver, "hsstest")) {
		ADD_EXCEPT0(oExp, "envirement HSSSERVER lost or not equ hsstest, programe exit!");
		ADD_LOG(V3_FATAL, V3_SYS_MGR, MBC_ENV_NOT_SET, "env \"HSSSERVER\" not equ \"hsstest\", modify .profile try again!");
		return -1;
	}
    
	//日志类初始化
    char sLogFile[256];
	snprintf(sLogFile, sizeof(sLogFile), "%slog/%s_%d_%d.log", \
        						m_sEnvAppDly, g_sAppName, g_iProcID, getpid());
	LogV2::setFileName(sLogFile);
    
	//运行上锁
	char  sLockFile[256];
	snprintf(sLockFile, sizeof(sLockFile), "%sdata/lock/%s_%d.lock", \
                                 m_sEnvAppDly, g_sAppName, g_iProcID);	
    if (tryLock(sLockFile, oExp) != 0) {
        ADD_EXCEPT0(oExp, "进程上锁失败,进程退出， 检查-p参数!");
        ADD_LOG(V3_FATAL, V3_SCHEDUL, MBC_THREAD_EXIST_ERROR, "%s -p %d -m %d 进程上锁失败,进程退出， 检查-p参数!", \
            g_sAppName, g_iProcID, g_iModuID);
        return -1;   
    }
	
	//license鉴权
	int opt;
	if (gethostname(m_sHostName, sizeof(m_sHostName)) != 0) {
	}
	
	//信号量注册
    if (HandleSigactions() != 0) {
        ADD_EXCEPT0(oExp,"应用程序信号量注册失败!");
        ADD_LOG(V3_FATAL, V3_SYS_MGR, MBC_SEMP_OPERA_FAIL, "应用程序信号量注册失败!");
        return -1;	
    }
    
/*
    if (!ConnectMonitor(g_iProcID) || ConnectTerminalMonitor(g_iProcID)) {
        ADD_EXCEPT0(oExp, "应用程序链接GUARD_MAIN注册失败!");
        ADD_LOG(V3_FATAL, V3_SCHEDUL, MBC_MEM_LINK_FAIL, "应用程序链接GUARD_MAIN注册失败!");
        return -1;    
    }
*/
    
    return 0;
}


Application::~Application ( ) 
{

}

int Application::tryLock(const char *pfname, ABMException &oExp)
{
	int  fd;
	char buf[16];
	memset(buf,'\0',sizeof(buf));
	if ((fd=open(pfname, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0) {
		ADD_EXCEPT1(oExp, "应用程序申请琐时打开文件失败! errno=%d", errno);
		return -1;
	}
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLK, &lock) < 0) {
		ADD_EXCEPT1(oExp, "应用程序文件上锁失败! errno=%d", errno);
		return -1;
	}
	
	if (ftruncate(fd, 0) < 0) {
		ADD_EXCEPT1(oExp,"应用程序上锁时ftruncate函数调用失败! errno=%d", errno);
		return -1;
	}
	snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());	
	if (write(fd, buf, strlen(buf)) != (int)strlen(buf)) {
		ADD_EXCEPT1(oExp, "上锁时write系统调用失败! errno=%d", errno);
		return -1;
	}
	return 0;
		
}

