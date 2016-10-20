#include "sgwInit.h"
#include "Log.h"
#include "unistd.h"
#include "CommandCom.h"
#include "Date.h"
#include "MBC_ABM.h"

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <functional>

#include "GlobalTools.h"
extern int g_argc;
extern char ** g_argv;
#define MAXFILE 65535
int hbInit::m_gServerState=0;
struct sigaction hbInit::m_gsigaction;
const char* SECTION_SYSTEM="SYSTEM";
const char* PARAM_STATE="state";
DEFINE_MAIN(hbInit)
using namespace StandardDevelopMent;
bool CheckProcessIsStop(pid_t pid)
{
	if(pid==0)
		return true;
	if((kill(pid, 0) == -1 && errno == ESRCH))
		return true;
	return false;
};

bool CheckProcessIsStop(const char* cstrAppName)
{
	if(0== cstrAppName)
		return true;//不存在表示已经停止
	char sCheckShell[256]={0};
	sprintf(sCheckShell," ps -ef|grep %s |grep -iv 'grep'| "
		" awk '{print $1,$2}' |grep  `whoami` |awk '{print $2}' ",cstrAppName);
	FILE* fd = popen(sCheckShell,"r");
	if(fd==0)
		return true;
	if(feof(fd)==EOF)
		return true;
	char sPid[32]={0};
	if(fscanf(fd,"%s",sPid)==EOF)
		return true;
	pclose(fd);
	return CheckProcessIsStop(atoi(sPid)); 
};

int StartApp(const char* pAppName,char* argv[32],const char* pBinDir)
{
	char tmp[1024] = {0};
	sprintf(tmp,"%s/%s",pBinDir,pAppName);
	if(-1 == access(tmp,X_OK|F_OK|R_OK|W_OK)){
		//ALARMLOG28(0,MBC_CLASS_Fatal,5,"进程派生失败:%s",pAppName);
		Log::log(0,"进程派生失败:%s",pAppName);
		return -1;
	}
	//我不知道这样能否避免在多线程启动的程序产生死锁
	//如果有人发现有问题 ... 
	//死锁的描述: 多线程中会不可避免的使用到锁，比如string
	//当线程产生锁，这时又调用了fork那按照unix的写时复制原则新的进程这个锁状态
	//也是锁住的，且没有人对他关闭
	pid_t pid;
	int i;
	pid=fork();
	if(pid<0){
		printf("error in fork\n");
		exit(1);
	}else if(pid>0){
		return pid;
	}
	sleep(1);
	setsid();
	
	if(pBinDir==0)
		chdir("/");
	else
		chdir(pBinDir);
/*		
	umask(0);
	for(i=0;i<MAXFILE;i++)
		close(i);
*/
	execve(pAppName,argv,environ);
	return 0;
}



bool hbInit::StopServer(void)
{
	if(!pCommandCom->InitClient())
		return false;
	return true;
}

bool hbInit::GetInputYesOrNo(void)
{
	cout<<"y/n: ";
	string strGet;
	cin >>strGet;
	transform(strGet.begin(),strGet.end(),strGet.begin(),::tolower);
	if(!strGet.compare("yes")||!strGet.compare("y"))
		return true;
	return false;
}
bool hbInit::CheckAll(bool bConfirm)
{
	bool bGet=true;
	if(m_bDisplay){
	cout<<"检查核心参数配置文件"<<endl;
	}	
	if(bConfirm)
		bGet =GetInputYesOrNo();
	if(bGet){
		if(!CheckCfgFile())
			return false;
	}
	return true;
}

bool hbInit::GetNowSystemState(string& strState)
{
	char sSystemState[64]={0};
	if(!pCommandCom->readIniString(SECTION_SYSTEM,PARAM_STATE,sSystemState,0))
		return false;
	strState=sSystemState;
	return true;
}

bool hbInit::CheckCfgFile(void)
{
	string strBillingCfgFile ;
	StandardTools::GetStdConfigFile(strBillingCfgFile);
	string strBillingCfgFileStd = strBillingCfgFile+".std";
	if(-1== access(strBillingCfgFile.c_str(),F_OK|R_OK|W_OK)) {
		char sLogMsg[256] = {0};
    sprintf(sLogMsg,"错误码为101:文件不存在或者文件不可读写, 请检查文件 :%s",strBillingCfgFile.c_str());
		Log::log(0,"%s",sLogMsg);			
		m_bAccess = true;
		return false;
	}
	if(-1==access(strBillingCfgFileStd.c_str(),F_OK|R_OK|W_OK))
		return false;
	return true;
}
 
bool hbInit::CheckNowSysteState(void)
{
	if(m_bDisplay)
	cout<<"检查当前系统状态"<<endl;
	string strTmp;	
	
	char sHome[254] = {0};
	char sTemp[256] = {0};
	char * p = NULL;
	if ((p=getenv ("ABM_PRO_DIR")) == NULL)
	  	sprintf (sHome, "/home/bill/ABM_HOME");
	else
	  	sprintf (sHome, "%s", p);
  char sFile[254];
	if (sHome[strlen(sHome)-1] == '/') {
	  sprintf (sFile, "%setc/abmconfig", sHome);
	} else {
	  sprintf (sFile, "%s/etc/abmconfig", sHome);
	}		

	
	
	if(m_bChecked){
		if(GetInputYesOrNo()){
			GetNowSystemState(strTmp);
			if(strTmp == "")
			{
					ReadIni mIni;
					mIni.readIniString (sFile, "system", "state", sTemp,"offline");
					strTmp = sTemp;
			}
			if(strTmp.compare("online")==0||strTmp.compare("maintenance")==0){
				if(m_bDisplay)
				cout<<"系统当前状态不符"<<endl;
				return false;
			}
		}else
			return true;
	}else{
		GetNowSystemState(strTmp);
		if(strTmp == "")
		{
				ReadIni mIni;
				mIni.readIniString (sFile, "system", "state", sTemp,"offline");
				strTmp = sTemp;
		}		
		if(strTmp.compare("online")==0||strTmp.compare("maintenance")==0){
				if(m_bDisplay)
				cout<<"系统当前状态不符"<<endl;
				return false;	
		}
		return true;
	}
}

bool hbInit::UnLoadAllMen()
{
	int iOK = -1;
	//iOK = SHMCmdSet::unloadSHMALL();
	if(iOK == 0) {
		return true;
	}
		return false;
}


bool hbInit::CreateLogMem()
{
	/*   开始创建并初始化三个日志组  */
	if(CommandCom::m_poLogg ){
		delete CommandCom::m_poLogg; 
		CommandCom::m_poLogg=0;
	}
	m_poLoggMgr = new ThreeLogGroupMgr();
	if(!m_poLoggMgr)
		{
		  Log::log(0,"%s","三个日志组创建失败");
		  return false;
		}
	if(!m_poLoggMgr->init())
		{
		  Log::log(0,"%s","三个日志组创建失败");
		  return false;
		}			
	 if(!m_poLoggMgr->create())
		{
		  Log::log(0,"%s","三个日志组创建失败");
		  return false;
		}	
	delete m_poLoggMgr; 
	m_poLoggMgr=NULL;	
		if(!CommandCom::m_poLogg )
		{
			CommandCom::m_poLogg = new ThreeLogGroup(true);
				if(!CommandCom::m_poLogg )
				{
		     Log::log(0,"%s","三个日志组初始化失败");
		     return false;
		   }
		}			
		CommandCom::m_poLogg->InitLogGroup();
		CommandCom::m_poLogg->m_iLogProid=0;
		CommandCom::m_poLogg->m_iLogAppid=0;
		/*   三个日志组  创建完毕  */		
		
		/*if(CommandCom::m_poLogg){
			delete CommandCom::m_poLogg; 
		  CommandCom::m_poLogg=0;
		}
		
		CommandCom::m_poLogg = new ThreeLogGroup();
			if(!CommandCom::m_poLogg )
				{
		     Log::log(0,"%s","三个日志组初始化失败");
		     return false;
		   }*/
		return true;
}



bool hbInit::LoadData()
{
	char sHome[254] = {0};
	char sBatch[256] = {0};
	char * p = NULL;
	if ((p=getenv ("ABM_PRO_DIR")) == NULL)
	  	sprintf (sHome, "/home/bill/ABM_HOME");
	else
	  	sprintf (sHome, "%s", p);
  char sFile[254];
	if (sHome[strlen(sHome)-1] == '/') {
	  sprintf (sFile, "%setc/abmconfig", sHome);
	} else {
	  sprintf (sFile, "%s/etc/abmconfig", sHome);
	}		
	ReadIni mIni;
	mIni.readIniString (sFile, "CHECKPOINT", "log_checkpoint_batch", sBatch,NULL);	
  CheckPointMgr *pCheckPointMgr = new CheckPointMgr();
  if(pCheckPointMgr == NULL) {	  
		return false;
	}
	pCheckPointMgr->touchInfoLoad(sBatch);
	if(pCheckPointMgr!=NULL) {
		delete pCheckPointMgr;
		pCheckPointMgr = NULL;
	}
	return true;
}

int hbInit::run()
{	
	char sLogMsg[2048] = {0};			
	
	int iBootService = 0;
	
	if(!Init()) {		
		return 0;
	}
	if(!CheckCfgFile())
			return false;	
	if(!PrepareParam())
		return 0;
	string strState;
	int fd1;
/*	if(!m_bDisplay){
		dup2(1,fd1);
		close(1);
	}*/
	if(CheckProcessIsStop("initserver")||CheckNowSysteState()){		
		Log::log(0,"开始创建日志内存");
	  bool bCreateOK = CreateLogMem();
	  if(!bCreateOK) {
		  Log::log(0,"创建日志内存失败");
		  return 0;
	  }
	  Log::log(0,"开始创建统计内存");
		/**
		bCreateOK = StatAllInfoMgr::CreateStatShm();
	  if(!bCreateOK) {
		  Log::log(0,"创建统计内存失败");
		  return 0;
	  }
	  StatAllInfoMgr::ClearStatShm();
	  **/
		
		/*业务进程的内存创建*/
	
	int cCreateOK = 0;
	printf("是否需要卸载所有的业务内存 1:是, 其他：否\n");
	scanf("%d",&cCreateOK);
	if(cCreateOK == 1) {
		Log::log(0,"开始卸载所有业务内存");
		UnLoadAllMen();
		Log::log(0,"业务内存卸载结束");
	}
	printf("是否需要创建所有的业务内存 1:是, 其他：否\n");
	scanf("%d",&cCreateOK);
	if(cCreateOK == 1) {
		Log::log(0,"开始创建所有业务内存");
		Log::log(0,"业务内存创建结束");
	}
	
	int cLoadOK = 0;
	printf("是否需要重新加载业务数据 1:是 其他：否\n");
	scanf("%d",&cLoadOK);
	if(cLoadOK==1) {		
		Log::log(0,"开始重新加载业务数据:");			
		bool bRet = LoadData();
		if(!bRet) {
			Log::log(0,"加载业务数据失败");
			return 0;
		}
		Log::log(0,"业务数据加载结束!");
	}
	
	if(CheckProcessIsStop("initserver")){
		if(CheckAll(m_bChecked)){
			if(!StartService())
				return -1;
			else
			{
				iBootService = 1;
			}
		}
	}
	}else {
		CheckCfgFile();
	}
	if( iBootService == 1 )
	{
		Log::log(0,"等待1号进程启动!");
		for(int i = 0;i<10;i++)
		{
			//for(int n = 0;n<i;n++)
			cout<<'.';
			sleep(1);	
		}
		cout<<'\r';
	}
	
	if(m_bAccess) {
		return -1;
	}
	
  if(	pCommandCom == NULL) {
  	pCommandCom = new CommandCom();
  }		
	if(!pCommandCom->InitClient())
		return false;
		
  if(	m_pLogg == NULL) {
  	m_pLogg = new ThreeLogGroup();
  }
  
  
	if(m_eMode == MODE_START)//-v 或不加参数操作
	{
		bool bTmp1 = CheckNowSysteState();
		bool bTmp = false;
		if(bTmp1)
			bTmp = ShiftServerMode(STATE_ONLINE);
		if(m_bOK) {
			Date d;
			if(bTmp){
			   sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit –v命令显示abminit的处理过程操作成功。",m_strUserName.c_str(),d.toString());
			   pCommandCom->reLoad();
			   m_pLogg->log(MBC_DISPLAY_SYSTEM,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,sLogMsg);
		  }else {
		  	sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit –v命令显示abminit的处理过程操作失败。",m_strUserName.c_str(),d.toString());
		  	m_pLogg->log(MBC_DISPLAY_SYSTEM,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,sLogMsg);
		  }
		  Log::log(0,"%s",sLogMsg);
		  
		}
		
		if(m_bShiftOK) {
			Date d;
			if(bTmp1) {
			  if(bTmp){
			    sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit 命令切换系统状态（offline模式变为online）操作成功。",m_strUserName.c_str(),d.toString());
					pCommandCom->reLoad();
					m_pLogg->log(MBC_DISPLAY_SYSTEM,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,sLogMsg);
		    }else {
		  	  sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit 命令切换系统状态（offline模式变为online）操作失败。",m_strUserName.c_str(),d.toString());
		  	  m_pLogg->log(MBC_DISPLAY_SYSTEM,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,sLogMsg);
		    }
		    Log::log(0,"%s",sLogMsg);
		    
		}else {
			sprintf(sLogMsg,"当前状态不为offline模式，请检查模式");
			Log::log(0,"%s",sLogMsg);
			m_pLogg->log(MBC_DISPLAY_SYSTEM,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,sLogMsg);
		}
	}
	}if(m_eMode== MODE_RELOAD)  //-r操作 
	{
		pCommandCom->reLoad();
	}if(m_eMode== MODE_MAIN)  //-s操作 
	{
		bool bTmp1 = false;
		if(CheckNowSysteState())
			bTmp1 = ShiftServerMode(STATE_MAIN);		
		Date d;
		if(bTmp1) {
		  sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit –s命令切换系统状态（offline模式变为maintenance）操作成功。",m_strUserName.c_str(),d.toString());
			pCommandCom->reLoad();
			m_pLogg->log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,sLogMsg);		
		}else {
			sprintf(sLogMsg,"用户:%s,在时间点:%s 进行了abminit –s命令切换系统状态（offline模式变为maintenance）操作失败。",m_strUserName.c_str(),d.toString());
			m_pLogg->log(MBC_SWITCH_SYSTEM,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,sLogMsg);		
		}
		Log::log(0,"%s",sLogMsg);
		m_pLogg->log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,sLogMsg);		
	}
	GetNowSystemState(strState);
	/*if(!m_bDisplay){
		dup2(fd1,1);
		cout.clear();
	}*/
	if(strState.empty()){
		if(m_bDisplay)
		cout<<"当前系统状态无法识别"<<endl;
	}
	else
	{
		if(m_bDisplay)
		cout<<"当前系统状态:"<<strState<<endl;
	}
	return 0;
}

void hbInit::OnSignal(int signum,siginfo_t *info,void *myact)
{
	if((info!=0)&&(info->si_value.sival_int==1))
	{
		//重启成功
		m_gServerState=1;
	}
	if((info!=0)&&(info->si_value.sival_int==2))
	{
		//重启失败
		m_gServerState=2;
	}
	if((info!=0)&&(info->si_value.sival_int==7))
	{
		m_gServerState=7;
		cout<<'\r';
		cout<<"启动1号进程................................."<<endl;
	}
	if(m_gServerState==1)
	{
		cout<<'\r';
		cout<<"InitServer Start......................................OK"<<endl;
	}
	//不成功我也不管啦
	sigaction(signum,&m_gsigaction,NULL) ;
}

hbInit::hbInit(void)
{
	m_eMode=MODE_UNKNOW;
	m_gServerState =0;
	m_bChecked=false;
	m_bDisplay =false;
	m_bOK = false;
	m_bShiftOK = false;
	m_bAccess = false;
	m_pLogg = NULL;
	pCommandCom = NULL;
	struct passwd pwd;
          struct passwd *result;
         char logBuffer[1024]={0};
           char pwdBuffer[1024]={0};

           if (getlogin_r (logBuffer, 1024) == 0)
             if (getpwnam_r (logBuffer, &pwd, pwdBuffer, 1024, &result) == 0){
			m_strUserName=pwd.pw_name;
	}
}

hbInit::~hbInit(void)
{
	/*if(m_poLoggMgr!=NULL) {
		delete m_poLoggMgr;
		m_poLoggMgr = NULL;
	}*/
}

bool hbInit::Init()
{
	//初始
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	sigemptyset(&m_gsigaction.sa_mask);
	act.sa_flags=SA_SIGINFO;
	act.sa_sigaction=OnSignal;
	if(sigaction(SIGUSR1,&act,&m_gsigaction) < 0)
	{
		return false;
	}

	m_strTIBSHOME_Path= getenv( ENV_HOME );
	if(m_strTIBSHOME_Path.empty())
	{
		//没有配置环境变量
		m_strTIBSHOME_Path =DEFAULT_ENV_PATH;
	}
	return true;
}

bool hbInit::ShiftServerMode(_SERVICE_STATE_ ServiceState)
{
	if(!pCommandCom->InitClient())
		return false;
	if(CheckProcessIsStop("initserver")){
		if(CheckAll(m_bChecked)){
			if(!StartService())
				return false;
		}
	}
	char sSystemState[64]={0};
	if(!pCommandCom->readIniString(SECTION_SYSTEM,PARAM_STATE,sSystemState,0))
		return false;
 
	string strTmp = "SYSTEM.state=";
	string sTmp = "SYSTEM.billing_online_time=";
	string sStateDate = "SYSTEM.state_date=";
	if(ServiceState==STATE_MAIN){
		strTmp.append("maintenance");
	}else if(ServiceState==STATE_ONLINE){
		strTmp.append("online");						
	}else
		return false;

	pCommandCom->changeSectionKey(strTmp);	
	
	Date d;	
	sStateDate.append(d.toString());
  pCommandCom->changeSectionKey(sStateDate);	
	
	if(ServiceState==STATE_ONLINE){
		strTmp="SYSTEM.mid_state=online";
		pCommandCom->changeSectionKey(strTmp);

		sTmp.append(d.toString());
		pCommandCom->changeSectionKey(sTmp);
		Log::log(0,"系统切换成online的时间为%s",d.toString());	
	}
	else if(ServiceState==STATE_MAIN){
		strTmp="SYSTEM.mid_state=maintenance";
		pCommandCom->changeSectionKey(strTmp);

		//sTmp.append(d.toString());
		//changeSectionKey(sTmp);
		Log::log(0,"系统切换成maintenance的时间为%s",d.toString());	
	}
	return true;
}


bool hbInit::StartService(void)
{
	string strBinDir = m_strTIBSHOME_Path;
	strBinDir.append("/bin");
	char sPid[32]={0};
	sprintf(sPid,"%d",getpid());
	char* argvInitServer[32]={"initserver",0,"-service",0};
	argvInitServer[1]=sPid;
	if(0>StartApp("initserver",argvInitServer,strBinDir.c_str()))
		return false;
	return TestServerActiveSignal(240);
}

bool hbInit::TestServerActiveSignal(int iTimes)
{
	iTimes=iTimes;
	char* c[3]={"/ ","--","\\ "};
	int j =0;	
	while(j++<iTimes)
	{
		int i=j%3;
		if(m_bDisplay){
		cout<<"等待服务启动.........................................."<<c[i]<<flush;
		}
		sleep(1);
		if(m_bDisplay){	
		cout<<'\r';
		}
		if(m_gServerState==1)
		{
			if(m_bDisplay){
			cout<<"服务启动成功"<<endl;
			}
			Log::log(0,"服务启动成功:%d",0);
			CommandCom *pCmdCom =new CommandCom();
			if(pCmdCom!=0)
			{
				pCmdCom->InitClient();
				return true;
			}		
			return true;
		}
		if(m_gServerState==2)
		{
			//SetProcessState(2);
			Log::log(0,"服务启动失败:%d",0);
			return false;
		}
	}
	cout<<endl;	
	Log::log(0,"服务启动超时,我也不知道启动没能启动:%d",0);
	return false;
}

bool hbInit::PrepareParam(void)
{
	m_bOK = false;
	m_bShiftOK = false;
	m_bShiftOK = true;
	if(g_argc<2)
	{
		m_eMode=MODE_START;
		m_bShiftOK = true;
		m_bDisplay=true;
		//m_bChecked=true;
		return true;
	}
	int ch;
	
	while ((ch = getopt(g_argc,g_argv, "vysicr?h")) != -1)
	{
		m_eMode=MODE_START;
		switch(ch)
		{
		case 'c':
			{
				m_eMode=MODE_DOWN;
			}break;
		case 'y':
			{
				m_bDisplay=true;
				m_bChecked=true;
			}break;
		case 'i':
			{
				m_eMode=MODE_START;
			}break;
		case 'v':
			{
				m_eMode=MODE_START;
				m_bShiftOK = false;
				m_bDisplay=true;
				m_bOK =true;
			}break;
		case 'r':
			{
				m_eMode=MODE_RELOAD;
			}break;
		case 's':
			{
				m_eMode=MODE_MAIN;
			}break;
		case '?':
		case 'h':
			{
				cout <<" ******************************************************************" << endl;
				cout <<"  使用方式:" << endl;
				cout <<" abminit [-s|-h] [-r] [-v]"<<endl;
				cout <<" 参数描述:"<<endl;
				cout <<" 没有参数将系统启动成online模式" << endl;
				cout <<" -s 将系统启动成maintenance模式" << endl;
				//cout <<" -i 将系统启动成  online 模式"<<endl;
				//cout <<" -v 显示处理过程 "<<endl;
				cout <<" -h 键入此命令可以获得使用帮助" << endl;
				cout <<" -r 重置0号进程，更新参数内存 "<<endl;
				//cout <<" -y 每一步处理都需要确认" << endl;
				cout <<" ******************************************************************" << endl;
				return 0;
			}break;
		}
	}
	if(MODE_UNKNOW == m_eMode)
	{
		cout<<"无法识别的参数 : ";
		for(int i=1;i<g_argc;i++){
			cout<<g_argv[i];
		}
		cout<<endl;
		return false;
	}
	return true;
}


