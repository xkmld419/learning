// Copyright (c) 2011
// All rights reserved.

#include "Process.h"
#include "Log.h"
#include "Exception.h"
#include "ReadIni.h"
#include "LogDB.h"
#include "EventSection.h"
extern EventSection *g_poEventSection;

int Process::m_iProcID;

int Process::m_iAppID;	
int Process::m_iFlowID;
int Process::m_iFromProcID;
	
char Process::m_sTIBS_HOME[80] = {0};
char Process::sLockPath[100] = {0};
	
ThreeLogGroup *Process::m_pLogObj = 0;
CommandCom* Process::m_pCommandCom=0;
CommandCom *Process::m_poCmdCom=NULL;
#ifdef _STAT_FILE_LOG
char Process::m_sLogFilePath[200] = {0};
#endif


void AbortProcess(int isignal)
{
	//TStatLogMgr::insertLogForce();//解决wfstatlog日志丢失问题
	

	BaseProcess::ClearProcessSignal();

	int mtmp_iSysPID=getpid();

	////Pprintf(0,1,0,"应用程序收到信号(%d)异常退出!!", isignal);
	printf("应用程序收到信号(%d)异常退出!!", isignal);
	if (Process::m_iSysPID==mtmp_iSysPID){//在有派生子进程的情况下，只有父进程退出是才调用
		DisConnectMonitor (WF_ABORT);
		DisconnectTerminalMonitor (ST_ABORT);
		cout<<"父进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}
	else{
		cout<<"子进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}


	Log::log (0, "应用程序收到信号(%d)异常退出--", isignal);

///	Process::m_pProRunMgr->ForceKilled(isignal);//  
	Log::log (0, "应用程序收到信号(%d)异常退出--", isignal);
	exit (-1);
}

void StopProcess(int isignal)
{

	Process::ClearProcessSignal();

	int mtmp_iSysPID=getpid();

	////Pprintf(0,1,0,"应用程序退出!(调度进程发起)");
	printf("应用程序退出!(调度进程发起)");
	if (Process::m_iSysPID==mtmp_iSysPID){//在有派生子进程的情况下，只有父进程退出是才调用
		DisConnectMonitor(WF_STOP);
		DisconnectTerminalMonitor ();
		cout<<"父进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}else{
		cout<<"子进程["<<mtmp_iSysPID<<"]退出!"<<endl;
	}
///	Process::m_pProRunMgr->ForceKilled(isignal);
	Log::log (0, "应用程序退出!(调度进程发起)");
	exit (0);
}



void Process::GetLogFileName(string& strFileName)
{
    /* LOG_FILE */
    char sLogFile[512]={0};

    /* 取进程基本信息:PROCESS_ID */
	if ((m_iProcID = GetProcessID()) == -1) {
        BaseProcess::GetLogFileName(strFileName);
		Log::setFileName (const_cast<char*> (strFileName.c_str()));
		THROW(MBC_Process+2);
		exit(1);
	}

    /* LOG_FILE */
	sprintf (sLogFile, "%s/log/process_%d.log", BaseProcess::m_sAPP_HOME, m_iProcID);
    strFileName = sLogFile;
};


Process::Process()
{
	char  sLogFile[512];
	string str;
	int PROCESS_ID;
	
	Process::m_pLogObj = new ThreeLogGroup();

    string strFileName;
    this->GetLogFileName(strFileName);
    Log::setFileName (const_cast<char*>(strFileName.c_str()));
	Log::log (0, "--进程启动--");

	/* SYS PID */
	m_iSysPID = getpid();

    /* 将此进程上锁*/
    char sLockPath[256]={0};
	sprintf(sLockPath, "%s/lockpath/%d", m_sAPP_HOME,m_iProcID);
	
	char sCmd[1000];
	memset(sCmd, 0, sizeof(sCmd));
	sprintf (sCmd,"mkdir -p %s/lockpath",m_sAPP_HOME);
	system(sCmd);

    if (run_onlyone(sLockPath)<0) 
    {
		Log::log (0, "--进程上锁失败--");
		THROW(MBC_Process+3);	
	}


	SetProcessSignal();

	if (!ConnectMonitor(PROCESS_ID) ) 
	{
		cout<<"Process:"<<m_iProcID<<" ConnectMonitor err."<<endl;
	}
	
	/* 连接后台终端monitor */
	ConnectTerminalMonitor (PROCESS_ID);
	
	
	if (!m_poCmdCom)
		m_poCmdCom = new CommandCom();
	
	if(m_poCmdCom)
		m_poCmdCom->InitClient();	
		
}

//##ModelId=4313CB2501E4
Process::~Process()
{
	Log::log (1000, "Process::~Process()");
		
			
	if(m_pLogObj != NULL)
	{		
		delete m_pLogObj;
		m_pLogObj = 0;
	}
		
	
	DisconnectTerminalMonitor ();
	Log::log (1000, "DisConnectTerminalMonitor");			
	
	Log::log (0, "--进程执行结束退出(析构)--");						
}
void Process::getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr)
{
	ReadIni reader;
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	char sFile[254];
	char sTmpVal[32];
	memset(sUser, 0, sizeof(sUser));
	memset(sPwd, 0, sizeof(sPwd));
	memset(sStr, 0, sizeof(sStr));
	memset(sFile, 0, sizeof(sFile));
	
	char sIniTmp[32]={0};
	
	if (m_sTIBS_HOME[strlen(Process::m_sTIBS_HOME)-1] == '/') {
		sprintf (sFile, "%setc/billingconfig", m_sTIBS_HOME);
	} else {
		sprintf (sFile, "%s/etc/billingconfig", m_sTIBS_HOME);
	}	
	strcpy(sIniTmp,sIniHeader);
	//GlobalTools::upperLowerConvert(sIniTmp,_CONVERT_LOWER);

	string strIniTmp = sIniTmp;
        //transform(strIniTmp.begin(),strIniTmp.end(),strIniTmp.begin(),::tolower);

	char sTemp[254];	
	memset(sTemp, 0, sizeof(sTemp));
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.username",strIniTmp.c_str());
	reader.readIniString (sFile, "BILLDB", sTmpVal, sUser, "");
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.password",strIniTmp.c_str());	
	reader.readIniString (sFile, "BILLDB", sTmpVal, sTemp,"");
	decode(sTemp,sPwd);

	string strPwd=sPwd;
        strPwd.erase(strPwd.find_last_not_of(' ')+1);
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.db_server_name",strIniTmp.c_str());
	reader.readIniString (sFile, "BILLDB", sTmpVal, sStr, "");	
	strcpy(sUserName,sUser);
	strcpy(sPasswd,sPwd);
	strcpy(sConnStr,sStr);
	
	return;
}
void Process::redirectDB(char * sIniHeader)
{
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	memset(sUser, 0, sizeof(sUser));
	memset(sPwd, 0, sizeof(sPwd));
	memset(sStr, 0, sizeof(sStr));
		
	getConnectInfo(sIniHeader,sUser,sPwd,sStr);
	
	try {
		Environment::setDBLogin (sUser, sPwd, sStr);
		Environment::getDBConn (true);
	} catch (...) {
	}
	}
//获取当前应用进程ID
//未获取到时,返回 -1 .
int Process::GetProcessID()
{
    char *pt;
    
    if ((pt = getenv ("PROCESS_ID")) == NULL) {
        printf ("[ERROR]: getenv(PROCESS_ID) == NULL, "
                "获取环境变量PROCESS_ID失败" );
        return -1;
    }

    int g_iProcessID = atoi (pt);
    
    printf ("[MESSAGE]: Envionment variable: PROCESS_ID = %d\n",
            g_iProcessID);
    
    return g_iProcessID;
}

int Process::run_onlyone(const char *filename)
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

