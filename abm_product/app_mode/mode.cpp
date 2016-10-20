#include "mode.h"
#include "Log.h"
#include "mntapi.h"
#include "ParamDefineMgr.h"
#include <sys/wait.h>
#include "ThreeLogGroup.h"
#include "MBC_ABM.h"
//#include "../app_infomonitor/InfoMonitor.h"

/*
其中LogFlag为日志类别，定义如下：
#define  OPERATIONLOG       1                       //业务进程处理日志
#define  PARAMLOG             2                       //参数管理日志
#define  ATTEMPERLOG        3                       //进程调度日志
#define  HBACCESS             4                       //数据库修改日志
#define  SYSTEMLOG           5                      //系统管理日志
 
例如：LOGG(ATTEMPERLOG,"--进程启动--");
*/

int TMode::m_iTimeOut = 30;
bool TMode::m_bInitFlag = false;

TMode::TMode()
{ 
    m_iTimeOut = 0;
    m_pCommandCom = 0; 
};

TMode::~TMode()
{
};

bool TMode::init(int iTimeOut,CommandCom *pCommandCom)
{
	bool bRet = false;
	if( m_bInitFlag )
		return m_bInitFlag;
	if(iTimeOut)
		m_iTimeOut = iTimeOut;
	if(pCommandCom)
		m_pCommandCom = pCommandCom;
//预约
	m_bInitFlag = bRet;
	return bRet;
};

int TMode::deal(char *sArgv[],int iArgc)
{
	int iRet = 0;
	char sOrgStat[128] = {0};
	char sNowStat[128] = {0};
	char sRetStr[512] = {0};
	int iStateType = 0;
	bool bNeedCheck = false;
	char ch = 0;

	char *pUser = getenv("LOGNAME");
	//LOGG(OPERATIONLOG,"用户[%s],执行命令集 hbmode!",pUser);
	Log::log(0,"用户[%s],执行命令集 abmmode!",pUser);
	//m_oLog.log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"用户[%s],执行命令集 abmmode !",
  //	pUser); 		

	if ( iArgc <= 1 )
	{
		showHelp();
		//
		//ALARMLOG28(0,MBC_CLASS_Fatal,167,"%s","应用进程参数不正确（缺少必选项）");
		Log::log(0,"%s","参数错误：错误码[167]应用进程参数不正确（缺少必选项）");
		//m_oLog.log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"%s",
	  //	"参数错误：错误码[167]应用进程参数不正确（缺少必选项）");
		return -1;//输入参数错
	}
	else if( iArgc > 3 )
	{
		//ALARMLOG28(0,MBC_CLASS_Fatal,169,"%s","应用进程参数不正确（错误的取值）");
		Log::log(0,"%s","参数错误：错误码[169]应用进程参数不正确（错误的取值）");
		//m_oLog.log(MBC_PARAM_THREAD_VALUE_ERROR,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"%s",
	  //	"参数错误：错误码[169]应用进程参数不正确（缺少必选项）");		
		return -1;
	}
	//if( strcmp(sArgv[0],"hbmode") != 0 )
	//{
	//	return -1;//输入参数错
	//}
	

	if( strcmp(sArgv[1],"-k") == 0 )
	{
		char sValue[16]={0};
		
		this->getNowSysState(_NET_STATE_,sNowStat);
		if( strcmp(sNowStat,"innet") == 0 )
		{
			printf("系统当前为innet状态，无法切换到offline，是否先切换为outnet！(Y/N)\n");
			while(1)
			{
				//scanf("%c\n",&ch);
				if(iArgc > 2)
				{
					if( strcmp(sArgv[2],"-y") == 0)
					{
						cout <<"Y"<< endl;
						break;
					}
				}
				cin >> ch;
				if(ch == 'N'||ch == 'n')
				{
					//return 0;
					iRet = -8;
					break;		
				}
				else if(ch != 'Y'&& ch != 'y')
					continue;
				else 
				{
					//cout << endl;
					break;
				}
			//
			}
			if(iRet != -8)
			{
				iStateType = _NET_STATE_;
				strcpy( sOrgStat,"innet" );
				iRet = changeSysState(_NET_STATE_,"innet","outnet");
			}
			//iRet = -8;
		}
		//else
		if(iRet >= 0)
		{
			iStateType = _SYS_STATE_;
			strcpy( sOrgStat,"maintenance" );
			if( (iRet = changeSysState(_SYS_STATE_,"maintenance","offline")) == -1)
			{
				strcpy( sOrgStat,"online" );
				iRet = changeSysState(_SYS_STATE_,"online","offline");
			}
			bNeedCheck = true;
			/*
			ParamDefineMgr::getParam("CHECKPOINT","CHECKPOINT",sValue);
			int iValue = atoi(sValue);
			if(iValue!=0)
			{
		#ifndef __IN_HB_MONITOR__
			cout<<"需要CheckPoint表数据"<<flush;
		#endif
			int iPid=0;
			 if((iPid= fork()==0))
	        	{
	                        execlp("/bin/sh","sh","-c","/opt/opthb/src/app_test/bkeventab.sh",NULL);
				exit(3);
	                }
	                	wait(0);
			}*/
		}
		

	}
	else if( strcmp(sArgv[1],"-ki") == 0 )
	{
		this->getNowSysState(_NET_STATE_,sNowStat);
		if( strcmp(sNowStat,"innet") == 0 )
		{
			iRet = -8;
		}
		else
		{
			iStateType = _SYS_STATE_;

			printf("系统将强制切换(存在待处理数据)到offline状态！(Y/N)\n");
			while(1)
			{
				//scanf("%c\n",&ch);
				if(iArgc > 2)
				{
					if( strcmp(sArgv[2],"-y") == 0)
					{
						cout <<"Y"<< endl;
						break;
					}
				}
				cin >> ch;
				if(ch == 'N'||ch == 'n')
					return 0;
				else if(ch != 'Y'&& ch != 'y')
					continue;
				else 
				{
					//cout << endl;
					break;
				}
			}
			


			strcpy( sOrgStat,"maintenance" );
			if( (iRet = changeSysState(_SYS_STATE_,"maintenance","offline",1)) == -1)
			{
				strcpy( sOrgStat,"online" );
				iRet = changeSysState(_SYS_STATE_,"online","offline",1);
			}
			//bNeedCheck = true;
		}
	}
	else if( strcmp(sArgv[1],"-m") == 0 )
	{
		this->getNowSysState(_NET_STATE_,sNowStat);
		this->getNowSysState(_RUN_STATE_,sOrgStat);

		memset( sOrgStat,0,sizeof(sOrgStat) );
		iStateType = _SYS_STATE_;
		strcpy( sOrgStat,"maintenance" );
		iRet = changeSysState(_SYS_STATE_,"maintenance","online");
		//bNeedCheck = true;

	}
	else if( strcmp(sArgv[1],"-s") == 0 )
	{
		this->getNowSysState(_NET_STATE_,sNowStat);
		if( strcmp(sNowStat,"innet") == 0 )
		{
			iRet = -10;
		}
		else
		{
			iStateType = _SYS_STATE_;
			strcpy( sOrgStat,"online" );
			iRet = changeSysState(_SYS_STATE_,"online","maintenance");
		}
		bNeedCheck = true;
	}
	else if( strcmp(sArgv[1],"-u") == 0 )
	{
		iStateType = _SYS_STATE_;
		strcpy( sOrgStat,"online" );
		iRet = changeSysState(_SYS_STATE_,"online","maintenance",1);
	}
	else if( strcmp(sArgv[1],"-r") == 0 )
	{
		getNowSysState(_SYS_STATE_,sNowStat);
		if( strcmp(sNowStat,"offline") == 0 )
		{
			iRet = -7;
		}
		else if( strcmp(sNowStat,"maintenance") == 0)
		{
			iRet = -11;
		}
		else
		{
			iStateType = _NET_STATE_;
			strcpy( sOrgStat,"outnet" );
			iRet = changeSysState(_NET_STATE_,"outnet","innet");
		}
	}
	else if( strcmp(sArgv[1],"-d") == 0 )
	{
		iStateType = _NET_STATE_;
		strcpy( sOrgStat,"innet" );
		iRet = changeSysState(_NET_STATE_,"innet","outnet");
	}
	else if( strcmp(sArgv[1],"-p") == 0 )
	{
		iRet = 1;
	}
	else if( strcmp(sArgv[1],"-n") == 0 )
	{
		if( getProcessFault() == 0 )
			iRet = -9;
		else
		{
			iStateType = _RUN_STATE_;
			if((iRet = getNowSysState(_SYS_STATE_,sNowStat)) == 1)
			{
				strcpy( sOrgStat,"fault" );
				if(strcmp(sNowStat,"online")==0)
					iRet = changeSysState(_RUN_STATE_,"fault","normal",0,1);
				else
					iRet = -1;
			}
		}
	}
	else if( strcmp(sArgv[1],"-N") == 0 )
	{
		iStateType = _RUN_STATE_;
		if((iRet = getNowSysState(_SYS_STATE_,sNowStat)) == 1)
		{
			strcpy( sOrgStat,"fault" );
			if( strcmp(sNowStat,"offline")==0 )
				iRet = changeSysState(_RUN_STATE_,"fault","normal",0,1);
			else
				iRet = -1;
		}
	}
	else if( strcmp(sArgv[1],"-h") == 0 )
	{
		iRet = 1;
		showHelp();
	}
	else if(strcmp(sArgv[1],"-sh")==0)//显示中间态
	{
		char sNowStat[128] = {0};
		getNowMidState(_SYS_STATE_,sNowStat);
		printf("系统当前中间状态:[%s]\n",sNowStat);
		memset(sNowStat,0,sizeof(sNowStat));
		getNowMidState(_RUN_STATE_,sNowStat);
		printf("系统运行状态:[%s]\n",sNowStat);	
		memset(sNowStat,0,sizeof(sNowStat));
		getNowMidState(_NET_STATE_,sNowStat);
		printf("系统网络中间状态:[%s]\n",sNowStat);		
		return 1;
	}
	else if(strcmp(sArgv[1],"-ss")==0)//重置系统状态
	{
		char sNowStat[128] = {0};
		char sMidStat[128] = {0};
		
		if(strcmp(sArgv[2],"o")==0) //online
		{
			strcpy(sMidStat,"SYSTEM.mid_state=online");
			strcpy(sNowStat,"SYSTEM.state=online");
		}
		else if(strcmp(sArgv[2],"f")==0) //offline
		{
			strcpy(sMidStat,"SYSTEM.mid_state=offline");
			strcpy(sNowStat,"SYSTEM.state=offline");			
		}
		else if(strcmp(sArgv[2],"m")==0) //maintenance
		{
			strcpy(sMidStat,"SYSTEM.mid_state=maintenance");
			strcpy(sNowStat,"SYSTEM.state=maintenance");				
		}
		else
		{
			printf("设置失败！！\n");
			return -1;
		}		
		m_pCommandCom->changeSectionKey(sMidStat);
		m_pCommandCom->changeSectionKey(sNowStat);
		printf("设置成功！！\n");
		return 1;
		
	}	
	else if(strcmp(sArgv[1],"-sn")==0)//重置网络状态
	{
		char sNowStat[128] = {0};
		char sMidStat[128] = {0};
		
		if(strcmp(sArgv[2],"o")==0) //outnet
		{
			strcpy(sMidStat,"SYSTEM.mid_net_state=outnet");
			strcpy(sNowStat,"SYSTEM.net_state=outnet");
		}
		else if(strcmp(sArgv[2],"i")==0) //innet
		{
			strcpy(sMidStat,"SYSTEM.mid_net_state=innet");
			strcpy(sNowStat,"SYSTEM.net_state=innet");
		}
		else if(strcmp(sArgv[2],"m")==0) //maintenance
		{
			strcpy(sMidStat,"SYSTEM.mid_state=maintenance");
			strcpy(sNowStat,"SYSTEM.state=maintenance");				
		}
		else
		{
			printf("设置失败！！\n");
			return -1;
		}
		m_pCommandCom->changeSectionKey(sMidStat);
		m_pCommandCom->changeSectionKey(sNowStat);
		
		printf("设置成功！！\n");
		return 1;
	}
	else
	{
		
		showHelp();
		//ALARMLOG28(0,MBC_CLASS_Fatal,168,"%s","应用进程参数不正确（不支持的选项）");
		return -1;//输入参数错
	}
	memset( sNowStat,0,sizeof(sNowStat) );
	getNowSysState(iStateType,sNowStat);
	
	switch ( iRet )
	{
	case 0:
		sprintf(sRetStr,"获取不到原始状态!\n");
		break;
	case -1:
		sprintf(sRetStr,"原状态[%s]不匹配!\n",sOrgStat);
		break;
	case -2:
		sprintf(sRetStr,"检查是否有别的进程在修改，目标状态有误[%s]!\n",sNowStat);
		break;
	case -3:
		sprintf(sRetStr,"状态变更失败，请查看日志!\n");
		break;
	case -4:
		sprintf(sRetStr,"状态传入参数错!\n");
		break;
	case -5:
		sprintf(sRetStr,"消息发送失败!\n");
		break;
	case -6:
		sprintf(sRetStr,"原状态[%s]和目标状态一致，不需要变更\n",sOrgStat);
		break;
	case -7:
		sprintf(sRetStr,"系统为[offline]状态,无法注册在网[innet]!\n");
		break;
	case -8:
		sprintf(sRetStr,"系统在网[innet],无法切换至[offline]状态!\n");
		break;
	case -9:
		sprintf(sRetStr,"系统运行状态异常[fault]，有进程状态异常，请先修复故障再变更状态\n");
		break;
	case -10:
		sprintf(sRetStr,"系统在网[innet],无法切换至[maintenance]状态!\n");		
		break;
	case -11:
		sprintf(sRetStr,"系统为[maintenance]状态,无法注册在网[innet]!\n");
		break;	
	case 1:
		sprintf(sRetStr,"abmmode命令执行成功!\n");
		break;
	default:
		sprintf(sRetStr,"未知错误!\n");
		break;
	}

	printf(sRetStr);
	//if(iRet < 1)
		//m_oLog.log(MBC_SGW_UNDOWN_FAIL,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"%s",sRetStr);
	//else
  	//m_oLog.log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"%s",sRetStr);
	showSysState();
	//checkpoint
/*
	if( bNeedCheck && iRet == 1 )
	{
		printf("开始CheckPoint...!\n");
		m_pCheckPointMgr = new CheckPointMgr();
		if(iRet = m_pCheckPointMgr->touchInfoDown())
			printf("CheckPoint成功!\n");
		else
			printf("CheckPoint失败!\n");
		//delete m_pCheckPointMgr;
	}
*/	
	//LOGG(OPERATIONLOG,"用户[%s],执行命令集 hbmode 完毕!",pUser);
	if(iArgc > 1)
	{
  	//m_oLog.log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"用户[%s],执行命令集 abmmode %s完毕!",
	  	//pUser,sArgv[1]); 				
		Log::log(0,"用户[%s],执行命令集 abmmode %s完毕!",pUser,sArgv[1]);
	}
	else
	{
  	//m_oLog.log(MBC_SWITCH_SYSTEM,LOG_LEVEL_INFO,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"用户[%s],执行命令集 abmmode 完毕!",
	  //	pUser); 				
		Log::log(0,"用户[%s],执行命令集 abmmode 完毕!",pUser);
	}
	//char ShutDown[]=". ./stop.sh";
	char sStatTmp[128]={0};
	getNowSysState(_SYS_STATE_,sStatTmp);
	/*
	if(strcmp(sStatTmp,"offline")==0
		&&(strcmp(sArgv[1],"-k") == 0||strcmp(sArgv[1],"-ki") == 0)
		)//&&(*getenv("STOPALL")=='1'))
	//system(ShutDown);
	{
		//InfoMonitor::stopBin();
		//InfoMonitor::stopSys();
	}
	*/
	return iRet;

};

int TMode::getNowSysState( int iStatType ,char *sNowStat )
{
	//char sSaction[128] = "SYSTEM";
	char sKey[128] = {0};

	if ( iStatType == _SYS_STATE_ )
		strcpy(sKey,"state");
	else if( iStatType == _RUN_STATE_ )
		strcpy( sKey,"run_state" );
	else if( iStatType == _NET_STATE_ )
	strcpy( sKey,"net_state" );
	else
	return 0;
	//调用接口获取
	m_pCommandCom->readIniString( _HBMODE_SECTION,sKey,sNowStat,"" );

	if( strlen(sNowStat) > 0 )
		return 1;
	else
		return 0;
};


int TMode::getNowMidState( int iStatType ,char *sNowStat )
{
	//char sSaction[128] = "SYSTEM";
	char sKey[128] = {0};

	if ( iStatType == _SYS_STATE_ )
		strcpy(sKey,"mid_state");
	else if( iStatType == _RUN_STATE_ )
		strcpy( sKey,"run_state" );
	else if( iStatType == _NET_STATE_ )
		strcpy( sKey,"mid_net_state" );
	else
	return 0;
	//调用接口获取
	m_pCommandCom->readIniString( _HBMODE_SECTION,sKey,sNowStat,"" );

	if( strlen(sNowStat) > 0 )
		return 1;
	else
		return 0;
};


int TMode::changeSysState( int iStatType, char *sOrgStat,char *sNewStat,int iFource /*= 0 */,int iFlag /*= 0*/ )
{
	char sNowStat[128] = {0};
	char sMidStat[128] = {0};
	char sKey[128] = {0};
	char strUrl[512] = {0};
	char sTmpVar[128]= {0};
	char sRunStat[128] = {0};
	char sNowMidStat[128] = {0};
	//char sSaction[128] = "SYSTEM";
	if(strcmp(sOrgStat,sNewStat)== 0)
		return -6;//原状态和目标状态一致，不需要变更
	getNowSysState(iStatType,sNowStat);

	if( strlen( sNowStat ) == 0 )
		return 0;//获取不到系统当前状态
	if( strcmp( sNowStat,sOrgStat ) != 0 )
		return -1;//原状态和当前状态不匹配


	//调用接口

	if ( iStatType == _SYS_STATE_ )
		strcpy(sKey,"state");
	else if( iStatType == _RUN_STATE_ )
		strcpy( sKey,"run_state" );
	else if( iStatType == _NET_STATE_ )
		strcpy( sKey,"net_state" );
	else
		return -4;//传入参数错
		
		//if(iStatType == _RUN_STATE_ || iStatType == _NET_STATE_ )
		if(iStatType == _RUN_STATE_ )
		{
			sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sNewStat);
			if(m_pCommandCom->changeSectionKey(sTmpVar))
			{
				return 1;
			}
			else
			{
				return -3;
			}
			
		}

	//修改当前状态到sNewstat(先切换成中间态)
	
	if( iFlag  == 0 )
		sprintf( sMidStat,"%d|%s|%s",iFource,sOrgStat,sNewStat );
	else if( iFlag >= 1 )
		sprintf( sMidStat,"%s",sNewStat );
	
	//sprintf( sMidStat,"%s",sNewStat );
	sprintf( sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat );
	
	
	sprintf(strUrl,"%s\0",_HBMODE_SECTION);
	//hbmode 只修改mid_state，读取state,高低水修改state，读取mid_state
	if( iFlag ==0){
		if(iStatType == _SYS_STATE_)
			strcpy(sKey,"mid_state");
		else if(iStatType == _NET_STATE_)
			strcpy(sKey,"mid_net_state");
		sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat);
		m_pCommandCom->changeSectionKey(sTmpVar);
	}else {
		if(iStatType == _SYS_STATE_)
			strcpy(sKey,"mid_state");
		else if(iStatType == _NET_STATE_)
			strcpy(sKey,"mid_net_state");
		sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat);
		m_pCommandCom->changeSectionKey(sTmpVar);
	}
	
	#ifndef __IN_HB_MONITOR__
	printf("发送状态更新指令！\n");
	#endif
	
	#ifndef __IN_HB_MONITOR__
	printf("发送指令成功！\n");
	#endif
	//循环等待最终态sNewstat直至超时
	char* c[3]={"/ ","--","\\ "};
	int i =0;
	while(1)
	{
		int j=i%3;
	#ifndef __IN_HB_MONITOR__
		cout<<"等待状态变更动作完成......................................."<<c[j]<<flush;
	#endif
		sleep(1);
	#ifndef __IN_HB_MONITOR__		
		cout<<'\r';
	#endif
		
		memset(sNowStat,0,128);
		getNowSysState(iStatType,sNowStat);
		if (strcmp( sNowStat,sNewStat ) == 0)
		{
	#ifndef __IN_HB_MONITOR__		
		cout<<endl;
	#endif			
			if(iStatType == _NET_STATE_)
			{
#ifndef __IN_HB_MONITOR__
				if( strcmp( sNowStat,"outnet" ) == 0 )
					printf("去激活成功\n");
				else
					printf("激活成功\n");
#endif		
			}
			return 1;
		}
		//变更失败
		//当中间态变为原状态视为变更失败。
		memset(sNowMidStat,0,128);
		getNowMidState(iStatType,sNowMidStat);
		if(strcmp( sNowStat,sNowMidStat ) == 0)
		{
	#ifndef __IN_HB_MONITOR__		
		cout<<endl;
	#endif			
			if(iStatType == _NET_STATE_)
			{
#ifndef __IN_HB_MONITOR__
				if( strcmp( sNowStat,"outnet" ) == 0 )
					printf("激活失败\n");
				else
				{
		char sNowStat[128] = {0};
		char sMidStat[128] = {0};
		
		strcpy(sMidStat,"SYSTEM.mid_net_state=outnet");
		strcpy(sNowStat,"SYSTEM.net_state=outnet");
		
		m_pCommandCom->changeSectionKey(sMidStat);
		m_pCommandCom->changeSectionKey(sNowStat);
							
					printf("去激活失败\n");
				}
#endif
			}
			return -3;
		}

		i++;
	}
	return -3;//变更失败
};


int TMode::changeSysState( int iStatType, char *sOrgStat,char *sNewStat,bool Ismonitor,int iFource ,int iFlag )
{
		char sNowStat[128] = {0};
	char sMidStat[128] = {0};
	char sKey[128] = {0};
	char strUrl[512] = {0};
	char sTmpVar[128]= {0};
	char sRunStat[128] = {0};
	//char sSaction[128] = "SYSTEM";
	if(strcmp(sOrgStat,sNewStat)== 0)
		return -6;//原状态和目标状态一致，不需要变更
	getNowSysState(iStatType,sNowStat);

	if( strlen( sNowStat ) == 0 )
		return 0;//获取不到系统当前状态
	if( strcmp( sNowStat,sOrgStat ) != 0 )
		return -1;//原状态和当前状态不匹配


	//调用接口

	if ( iStatType == _SYS_STATE_ )
		strcpy(sKey,"state");
	else if( iStatType == _RUN_STATE_ )
		strcpy( sKey,"run_state" );
	else if( iStatType == _NET_STATE_ )
		strcpy( sKey,"net_state" );
	else
		return -4;//传入参数错
		
		//if(iStatType == _RUN_STATE_ || iStatType == _NET_STATE_ )
		if(iStatType == _RUN_STATE_ )
		{
			sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sNewStat);
			if(m_pCommandCom->changeSectionKey(sTmpVar))
			{
				return 1;
			}
			else
			{
				return -3;
			}
			
		}

	//修改当前状态到sNewstat(先切换成中间态)
	
	if( iFlag  == 0 )
		sprintf( sMidStat,"%d|%s|%s",iFource,sOrgStat,sNewStat );
	else if( iFlag >= 1 )
		sprintf( sMidStat,"%s",sNewStat );
	
	//sprintf( sMidStat,"%s",sNewStat );
	sprintf( sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat );
	
	
	sprintf(strUrl,"%s\0",_HBMODE_SECTION);
	//hbmode 只修改mid_state，读取state,高低水修改state，读取mid_state
	if( iFlag ==0){
		if(iStatType == _SYS_STATE_)
			strcpy(sKey,"mid_state");
		else if(iStatType == _NET_STATE_)
			strcpy(sKey,"mid_net_state");
		sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat);
		m_pCommandCom->changeSectionKey(sTmpVar);
	}else {
		if(iStatType == _SYS_STATE_)
			strcpy(sKey,"mid_state");
		else if(iStatType == _NET_STATE_)
			strcpy(sKey,"mid_net_state");
		sprintf(sTmpVar,"%s.%s=%s",_HBMODE_SECTION,sKey,sMidStat);
		m_pCommandCom->changeSectionKey(sTmpVar);
	}
	
	if(!Ismonitor)
	{
	  printf("发送状态更新指令！\n");
  }
	
	//循环等待最终态sNewstat直至超时
	char* c[3]={"/ ","--","\\ "};
	int i =0;
	while(1)
	{
		int j=i%3;
		if(!Ismonitor)
	  {
		  cout<<"等待状态变更动作完成......................................."<<c[j]<<flush;
	  }
		sleep(1);
		if(!Ismonitor)
	  {
		  cout<<'\r';
    }
		
		
		getNowSysState(iStatType,sNowStat);
		if (strcmp( sNowStat,sNewStat ) == 0)
		{ 
			if(!Ismonitor)
			{
			  cout<<endl;
			}
			return 1;
		}
		

		i++;
	}
	return -3;//变更失败
}
int TMode::showSysState()
{
	char sNowStat[128] = {0};
	getNowSysState(_SYS_STATE_,sNowStat);
	printf("系统当前状态:[%s]\n",sNowStat);
	memset(sNowStat,0,sizeof(sNowStat));
	getNowSysState(_RUN_STATE_,sNowStat);
	printf("系统运行状态:[%s]\n",sNowStat);	
	memset(sNowStat,0,sizeof(sNowStat));
	getNowSysState(_NET_STATE_,sNowStat);
	printf("系统网络状态:[%s]\n",sNowStat);

	return 1;
};

int TMode::showHelp()
{
	cout <<" ******************************************************************" << endl;
	cout <<"  使用方式:" << endl;
	cout <<" -k 执行立即shutdown，将系统由maintenance  或online模式变为offline模式" << endl;
	cout <<" -ki 执行立即shutdown，强制系统由maintenance  或online模式变为offline模式" << endl;
	cout <<" -m 将系统从maintenance模式变为online模式" << endl;
	cout <<" -s 执行（非强制）由online模式变成maintenance模式" << endl;
	cout <<" -u 执行（强制）由online模式变成maintenance模式" << endl;
	//cout <<" -r 执行由离网变成在网状态(计费网注册)" << endl;
	//cout <<" -d 执行由在网状态变成离网状态(计费网注销)" << endl;
	cout <<" -p 显示当前ABM的状态" << endl;
	//cout <<" -n 对online故障的系统经过修复后重新切换成正常态。" << endl;
	//cout <<" -N 对offline故障的系统经过修复后重新切换成正常态。" << endl;
	cout <<" -y 对-Ki参数切换默认选择yes" << endl;

	cout <<" -h 键入此命令可以获得使用帮助" << endl;
	cout <<" ******************************************************************" << endl;
	return 1;
};


int TMode::getProcessFault()
{
	THeadInfo *pInfoHead = NULL;
	TProcessInfo *pProcInfoHead = NULL;
	int i =0;

	DetachShm ();
	if (AttachShm (0, (void **)&pInfoHead) >= 0) {
	    pProcInfoHead = &pInfoHead->ProcInfo;
	} else {
	    return -1;
	}
	
	for(i=0; i<pInfoHead->iProcNum; i++)
	{
		if( (pProcInfoHead + i)->iState == ST_ABORT||(pProcInfoHead + i)->iState == ST_KILLED )
			return 0;
	}
	return 1;
	
}
