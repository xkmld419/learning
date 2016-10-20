#include <signal.h>
#include <iostream>
using namespace std;
#include "BaseProcess.h"
#include "Log.h"
#include "Exception.h"
#include "MBC.h"
#include "Env.h"
SignalAction BaseProcess::m_pAbortFunc=0;
SignalAction BaseProcess::m_pStopFunc=0;
char    BaseProcess::m_sAPP_HOME[200]={0};
char    BaseProcess::m_sAPP_NAME[64]={0};
int	BaseProcess::m_iSysPID=0;

BaseProcess *g_pProc = 0;
int g_argc;
char ** g_argv;


void BaseProcess::AbortProcess(int isignal)
{
	BaseProcess::ClearProcessSignal();

	int mtmp_iSysPID=getpid();

	cerr<<"应用程序收到信号("<<isignal<<")异常退出!!";
	if (BaseProcess::m_iSysPID==mtmp_iSysPID){//在有派生子进程的情况下，只有父进程退出是才调用
		cerr<<"父进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}
	else{
		cerr<<"子进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}
	Log::log (0, "应用程序收到信号(%d)异常退出--", isignal);
	exit (-1);
}

void BaseProcess::StopProcess(int isignal)
{
	BaseProcess::ClearProcessSignal();

	int mtmp_iSysPID=getpid();

	cout<<"应用程序退出!(调度进程发起)"<<endl;
	if (BaseProcess::m_iSysPID==mtmp_iSysPID){
		cout<<"父进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}else{
		cout<<"子进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}
	Log::log (0, "应用程序退出!(调度进程发起)");
	exit (0);
}


void BaseProcess::NormalExitProcess(void)
{
    BaseProcess::ClearProcessSignal();
    int mtmp_iSysPID=getpid();

    Log::log (0, "应用程序正常退出");
    exit(1);
}

void BaseProcess::GetLogFileName(string& strFileName)
{
    /* LOG_FILE */
    char sLogFile[512]={0};
    Date d;

    sprintf (sLogFile, "%s/log/PROC_%s_%s.log", m_sAPP_HOME,g_argv[0],d.toString("MMDD"));
    strFileName=sLogFile;
};


void BaseProcess::GetAppPathName(string& strPath)
{
    char *p =NULL;
    if((p=getenv(ENV_HOME))==0){
        strPath="/opt";
    }else
        strPath=p;
};
BaseProcess::BaseProcess(RUN_FLAG sFlag)
{
    string strFileName;
    string strEnvHome;
    GetAppPathName(strEnvHome);
    int iLen=(sizeof(m_sAPP_HOME)-1)>strEnvHome.length()?strEnvHome.length():(sizeof(m_sAPP_HOME)-1);
    memset(m_sAPP_HOME,0,sizeof(m_sAPP_HOME));
    strncpy(m_sAPP_HOME,strEnvHome.c_str(),iLen);
    
    char sLockPath[256]={0};
    if(sFlag==SYSTEM){
        //一个系统只能运行一个实例
        sprintf(sLockPath, "%s/lockpath/%s", m_sAPP_HOME,g_argv[0]);
    }else if(sFlag ==USER){
        //一个用户只能运行一个实例
        sprintf(sLockPath, "%s/lockpath/%s_%d", m_sAPP_HOME,g_argv[0],getuid());
    }
    
    if(sLockPath[0]!=0){
        if(this->run_onlyone(sLockPath)!=0){
		cerr<<sLockPath<<endl;
             THROW(MBC_Process+10);
        }
    }
   
    this->GetLogFileName(strFileName);
    Log::setFileName (const_cast<char*>(strFileName.c_str()));
	Log::log (0, "--进程启动--");

	/* SYS PID */
	m_iSysPID = getpid();

    RegistStopFunc(StopProcess);
    RegistAbortFunc(AbortProcess);
    SetProcessSignal();
}


BaseProcess::~BaseProcess(void)
{
}

int BaseProcess::log(int log_level, char *format,  ...) 
{
    va_list ap;
	Log::log (log_level, format, ap);

	return 1;
}

int BaseProcess::warn()
{
	return 0;
}

void BaseProcess::SetProcessSignal()
{
    if((0==m_pAbortFunc)||(0==m_pStopFunc))
        return ;
	// 设置信号处理
	if (signal(SIGQUIT,AbortProcess) == SIG_ERR) {
		THROW(MBC_Process+4);
	}
	if (signal(SIGTERM,AbortProcess) == SIG_ERR) {
		THROW(MBC_Process+5);
	}
	if (signal(SIGINT,AbortProcess) == SIG_ERR)  {
		THROW(MBC_Process+6);
	}
	if (signal(SIGSEGV,AbortProcess) == SIG_ERR)  {
		THROW(MBC_Process+8);
	}

	if (signal(SIGCHLD,SIG_IGN) == SIG_ERR)  {
		THROW(MBC_Process+8);
	}

	//SIGUSR1 信号为前台调度界面发送停止进程控制命令时,由后台守护进程产生
	if (signal(SIGUSR1,StopProcess) == SIG_ERR)  {
		THROW(MBC_Process+7);
	}
}

void BaseProcess::ClearProcessSignal()
{
	// 设置信号处理
	if (signal(SIGQUIT,SIG_DFL) == SIG_ERR) {
		cerr<<"Can't catch SIGQUIT";
	}
	if (signal(SIGTERM,SIG_DFL) == SIG_ERR) {
		cerr<<"Can't catch SIGTERM";
	}
	if (signal(SIGINT,SIG_DFL) == SIG_ERR)  {
		cerr<<"Can't catch SIGINT";
	}
	if (signal(SIGSEGV,SIG_DFL) == SIG_ERR)  {
		cerr<<"Can't catch SIGSEGV";
	}

	if (signal(SIGCHLD,SIG_DFL) == SIG_ERR)  {
		cerr<<"Can't catch SIGCHLD";
	}

	//SIGUSR1 信号为前台调度界面发送停止进程控制命令时,由后台守护进程产生
	if (signal(SIGUSR1,SIG_DFL) == SIG_ERR)  {
		cerr<<"Can't catch SIGUSR1";
	}
}

int BaseProcess::run_onlyone(const char *filename)
{
    int  fd, val;
    char buf[10];
    memset(buf, 0, sizeof(buf));

    if ((fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
        return -1;

    // try and set a write lock on the entire file
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) < 0)
    {
        if (errno == EACCES || errno == EAGAIN){
		 THROW(MBC_Process+10);
		 }
        else
            return -1;
    }

    // truncate to zero length, now that we have the lock
    if (ftruncate(fd, 0) < 0)
        return -1;

    // and write our process ID
    sprintf(buf, "%d\n", getpid());
    if (write(fd, buf, strlen(buf)) != (int)strlen(buf))
        return -1;

    // set close-on-exec flag for descriptor
    if ( (val = fcntl(fd, F_GETFD, 0)) < 0)
        return -1;
    val |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, val) < 0)
        return -1;

    // leave file open until we terminate: lock will be held
   
    return 0;
}
