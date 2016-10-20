#include "StdAfx.h"
//#include "../app_infomonitor/InfoMonitor.h"
Schedule::Schedule()
{
	memset(m_sRunState,0,sizeof(m_sRunState));
	memset(m_sSysState,0,sizeof(m_sSysState));
	memset(m_sMidSysState,0,sizeof(m_sMidSysState));
	m_iMemPercent = 0;
	m_iCpuPercent = 0;
	if(Channel::m_pLog == NULL)
	{
	  try
	  {
		  Channel::m_pLog = new ThreeLogGroup();
	  }
	  catch (bad_alloc& e)
	  {
		  badalloc(e);
		  exit(-1);
	  }
	  
	   Channel::m_pLog->m_iLogProid = -1;
     Channel::m_pLog->m_iLogAppid = -1;	
	}	
}

Schedule::~Schedule()
{
	if(0!= Channel::m_pCmdCom)
	{
		Channel::m_pCmdCom->Logout();
		delete Channel::m_pCmdCom;
	}
	Channel::m_pCmdCom=0;
	if(Channel::m_pLog)
	{
		delete Channel::m_pLog;
		Channel::m_pLog=NULL;	
	}
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		if(m_vVirtual[i] != NULL)
			delete m_vVirtual[i];
	}
}


/***********************************************
 *	Function Name	:isTimeOut
 *	Description	:使进程休眠
 *	Input Param	:
 *	Returns		:
 *	complete	:
 ***********************************************/

int Schedule::isTimeOut()
{
	sleep(m_iWaitTime);
	return 1;
}
//解析中间态
char* Schedule::parserState()
{
	Channel::trim(m_sMidSysState);
	char *p = strrchr(m_sMidSysState,'|');
	memset(m_sSysState,0,sizeof(m_sSysState));
	if(p != NULL){
		strncpy(m_sSysState,p+1,14);
	}
	else
	{
		strncpy(m_sSysState,m_sMidSysState,14);
	}
	return p;
}
/***********************************************
 *	Function Name	:refresh
 *	Description	:刷新配置
 *	Input Param	:
 *	Returns		:
 *	complete	:
 ***********************************************/
int Schedule::refresh()
{
	if(0==Channel::m_pCmdCom)
		return 0;
	if(Channel::m_pInfoHead->iRefreshFlag == 2)//检测到mnt -r
	{
		Channel::m_pInfoHead->iRefreshFlag = 0;
		Log::log(0,"%s","检测到配置更新，重新加载虚处理器");
		//INFOPOINT(1,"%s","检测到配置更新，重新加载虚处理器");
		getCurVirProcessor();
	}
	static char sTemp[100] = {0};
	memset(sTemp,0,sizeof(sTemp));
	
	m_vtSysParamHis.clear();
	//Channel::m_pCmdCom->FlushParam(&m_vtSysParamHis);
	getSysConfigEx();
	//Log::log(0,"系统:档位[%d],轮转时间[%ds],状态[%s],自动调度[%d]",m_iShift,m_iWaitTime,m_sSysState,m_iAutoCtlFlag);
	//INFOPOINT(2,"系统:档位[%d],轮转时间[%ds],状态[%s],自动调度[%d]",m_iShift,m_iWaitTime,m_sSysState,m_iAutoCtlFlag);
	return 0;
}

//修改系统状态
int Schedule::modifySysState(char * state)
{
	char sCmd[100] = {0};

	sprintf(sCmd,"SYSTEM.state=%s\0",state);

	//variant var;
	//var = sCmd;

	//CmdMsg* pMsg = Channel::m_pCmdCom->CreateNewMsg( MSG_STATE_CHANGE_RETURN,"SYSTEM",var);
	
	//if(!Channel::m_pCmdCom->send(pMsg))
	if(!Channel::m_pCmdCom->changeSectionKey(sCmd))
	{
		Log::log(0,"修改核心参数消息发送失败\n");
	  return -1;//消息发送失败
	 }
	sprintf(sCmd,"SYSTEM.mid_state=%s\0",state);
	if(!Channel::m_pCmdCom->changeSectionKey(sCmd))
	{
		Log::log(0,"修改核心参数消息发送失败\n");
	  return -1;//消息发送失败
	 }
	 
	 time_t tt = time(NULL);
	 
	 struct tm *p = localtime(&tt);
	 
	 char chargedate[15] = {0};
	
	strftime(chargedate,15,"%Y%m%d%H%M%S",p);
	
	sprintf(sCmd,"SYSTEM.state_date=%s\0",chargedate);
	
	if(!Channel::m_pCmdCom->changeSectionKey(sCmd))
	{
		Log::log(0,"修改核心参数消息发送失败\n");
	  return -1;//消息发送失败
	}	 
	return 0;
}
//修改CHECKPOINT状态
int Schedule::modifyCheckPointState(int iState){
	char sCmd[100] = {0};

	sprintf(sCmd,"SYSTEM.checkpoint_state=%d\0",iState);

	//variant var;
	//var = sCmd;

	//CmdMsg* pMsg = Channel::m_pCmdCom->CreateNewMsg( MSG_PARAM_CHANGE,"SYSTEM",var);
	//if(!Channel::m_pCmdCom->send(pMsg))
	if(!Channel::m_pCmdCom->changeSectionKey(sCmd))
	{
		Log::log(0,"修改核心参数消息发送失败\n");
	  return -1;//消息发送失败
	 }
	return 0;
}

//系统运行态检测
int Schedule::checkSysRunStateEx()
{
	int iTotal = m_vVirtual.size();
	int iProcessID = 0;
	int bWaringFlag = false;
	for(int i=0; i<iTotal; i++){
		m_vVirtual[i]->refreshEx();
		m_vVirtual[i]->getProcInfoFromGuardMain();
		iProcessID = m_vVirtual[i]->isErrExist();
		if(iProcessID > 0)
		{
			Channel::m_pLog->log(MBC_THREAD_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_SYSTEM,-1,NULL,"进程 %d 异常,请检查",iProcessID);
			modifySysRunState("fault");			
			bWaringFlag = true;
		}
		if(m_vVirtual[i]->isBusy() > 0){
				
			Channel::m_pLog->log(MBC_SYS_BUSY,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"系统超负荷,系统长时间处于高水位");
			modifySysRunState("busy");
			bWaringFlag = true;
		}
	}
	
	
	if( GetCpuInfo() && m_iCpuPercent >= m_iCpuHighValue ){
			
			Channel::m_pLog->log(MBC_CPU_OVERLOAD,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"系统超负荷,CPU使用率[%d%%]",m_iCpuPercent);
			modifySysRunState("busy");
			bWaringFlag = true;
	} 
	if( GetMemInfo() && m_iMemPercent >= m_iMemHighValue ){			
			 Channel::m_pLog->log(MBC_MEM_SPACE_SMALL,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"系统超负荷,内存使用率[%d%%]",m_iMemPercent);
			 modifySysRunState("busy");
			bWaringFlag = true;
	} 
	
	if(!bWaringFlag)
	{
	  modifySysRunState("normal");
	}
	
	return 0;
}
//修改系统运行状态
int Schedule::modifySysRunState(char* state)
{
	char sCmd[100] = {0};
	char sTempState[100] = {0};
	memset(sTempState,0,sizeof(sTempState));

	if(!strcmp(state,m_sRunState)){
		return 0;
	}
	sprintf(sCmd,"SYSTEM.run_state=%s\0",state);

	//variant var;
	//var = sCmd;
	//CmdMsg* pMsg = Channel::m_pCmdCom->CreateNewMsg( MSG_PARAM_CHANGE,"SYSTEM",var);
	//MSG_STATE_CHANGE_RETURN
	//CmdMsg* pMsg = Channel::m_pCmdCom->CreateNewMsg( MSG_STATE_CHANGE_RETURN,"SYSTEM",var);
	if(!Channel::m_pCmdCom->changeSectionKey(sCmd)){
		//Log::log(0,"修改核心参数消息发送失败\n");
		//ALARMLOG(0,MBC_schedule+25,"%s","修改核心参数消息发送失败");
	  return -1;//消息发送失败
	 }
	return 0;
}
//向0号进程发起注册
int Schedule::registerToInitEx()
{
	try{
		Channel::m_pCmdCom = new CommandCom();
	}catch (bad_alloc& e)
	{
		badalloc(e);
		//ALARMLOG(0,MBC_initserver+1,"%s","分配通信实例失败");
		exit(-1);
	}

	//注册客户端
	if(!Channel::m_pCmdCom->InitClient())
	{
		delete Channel::m_pCmdCom; Channel::m_pCmdCom=0;
		//ALARMLOG(0,MBC_initserver+1,"%s","注册客户端失败");
		exit(-1);
	}
  
  /*
	//读取默认配置
	Channel::m_pCmdCom->GetInitParam("SYSTEM");
	Channel::m_pCmdCom->GetInitParam("SCHEDULE");
	Channel::m_pCmdCom->GetInitParam("log");
	Channel::m_pCmdCom->GetInitParam("monitor");
	//预约下面的更改
	Channel::m_pCmdCom->SubscribeCmd("SYSTEM",MSG_PARAM_CHANGE);
	Channel::m_pCmdCom->SubscribeCmd("SYSTEM",MSG_STATE_CHANGE);
	Channel::m_pCmdCom->SubscribeCmd("SCHEDULE",MSG_PARAM_CHANGE);
	Channel::m_pCmdCom->SubscribeCmd("log",MSG_PARAM_CHANGE);
	Channel::m_pCmdCom->SubscribeCmd("monitor",MSG_PARAM_CHANGE);
	//Channel::m_pCmdCom->SubscribeCmd("SCHEDULE",MSG_PARAM_CHANGE);
	*/

	return 0;

}

//加载虚处理器配置
int Schedule::getCurVirProcessor()
{
	if(0==Channel::m_pCmdCom)
		return 0;
	char *  pUserName = getenv("LOGNAME");
  if (!strlen(pUserName) ) {
        Log::log (0, "%s","获取不到LOGNAME");
        THROW (MBC_IpcKey + 1);
  }
	//先对表 找到所有VPID
	//DEFINE_QUERY(qry);
	char sSQL[1024]={0};
	int iProcess;
	int iOutProcess;
	vector<int> vVirProc;
	vector<int>::iterator it;
	/*
	sprintf(sSQL,"select distinct a.vp_id from wf_process a where a.vp_id>0 and a.sys_username='%s' order by vp_id",pUserName);
	qry.setSQL(sSQL);
	qry.open();
	while(qry.next())
	{
		vVirProc.push_back(qry.field (0).asInteger());
	}
	qry.close();
	*/
	
	TProcessInfo *pProcInfo = Channel::m_pProcInfoHead;//指向进程首地址
	int iCnt = Channel::m_pInfoHead->iProcNum;
	for(int i=0; i<iCnt; i++)
	{
		if( (Channel::m_pProcInfoHead+i)->iVpID >0 )
		{
			it = find(vVirProc.begin(),vVirProc.end(),(Channel::m_pProcInfoHead+i)->iVpID);
			if(it == vVirProc.end())
				vVirProc.push_back((Channel::m_pProcInfoHead+i)->iVpID);
		}
	}
	
	char sTemp[100]={0};
	int iRet = -1;
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		if(m_vVirtual[i] != NULL)
		{
			delete m_vVirtual[i];
		}
	}
	m_vVirtual.clear();
	//分析当前状态下实际管理的虚处理器
	int iTempID;
	for(int i=0; i<vVirProc.size(); i++)
	{
		iTempID= vVirProc[i]/1000;
		memset(sTemp,0,100);
		sprintf(sTemp,"vp%d.%s_state",iTempID,m_sSysState);
		iRet = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
		if(iRet != -1)
		{
			VirtualProcessor * vp = new VirtualProcessor(vVirProc[i]);
			m_vVirtual.push_back(vp);
		}
	}

	return 0;

}
//读取核心参数
int Schedule::getSysConfigEx()
{
	if(0==Channel::m_pCmdCom)
		return 0;
	char sTemp[100]={0};
	//读系统档位
	m_iShift = Channel::m_pCmdCom->readIniInteger ( "SCHEDULE", "global_shift", 0);	
	if(m_iShift<=0 || m_iShift>=11)
	{
		//ALARMLOG(0,MBC_schedule+3,"非法的档位值%d，使用默认配置10档",m_iShift);
		m_iShift = 10;
	}
	//采样时间
	m_iWaitTime= Channel::m_pCmdCom->readIniInteger ( "SCHEDULE", "water_check_interval", 0);	
	//m_iWaitTime= Channel::m_pCmdCom->readIniInteger ( "other", "check_process_time", 0);	
	if(m_iWaitTime <= 0)
	{
		//ALARMLOG(0,MBC_schedule+4,"非法的采样时间%d，使用默认配置10秒",m_iWaitTime);
		m_iWaitTime =2;
	}
	Channel::m_pCmdCom->readIniString("SYSTEM","run_state",m_sRunState,NULL);		
	//系统状态
	Channel::m_pCmdCom->readIniString("SYSTEM","mid_state",m_sMidSysState,NULL);	
	parserState();
		


	//高低水开关
	sprintf(sTemp,"%s_auto_schedule",m_sSysState);
	m_iAutoCtlFlag=Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iAutoCtlFlag<0)
	{
		//ALARMLOG(0,MBC_schedule+5,"非法的自动控制开关值%d，使用默认参数0",m_iAutoCtlFlag);
		m_iAutoCtlFlag = 0;
	}
	//进程启动超时阀值
	m_iStartTime = Channel::m_pCmdCom->readIniInteger("SCHEDULE","process_start_time_out",-1);
	if(m_iStartTime < 0){
		//ALARMLOG(0,MBC_schedule+6,"进程启动时间超时阀值%d不正确，使用默认值10s",m_iStartTime);
		m_iStartTime = 10;
	}
	//CPU过载值
	//m_iCpuHighValue = Channel::m_pCmdCom->readIniInteger("monitor","cpu_warning_value",-1);
	m_iCpuHighValue = Channel::m_pCmdCom->readIniInteger("SYSTEM","cpu_used",-1);
	if(m_iCpuHighValue < 0){
		//ALARMLOG(0,MBC_schedule+7,"CPU过载告警阀值%d不正确，使用默认值90",m_iCpuHighValue);
		m_iCpuHighValue = 90;
	}
	//MEM过载值
	m_iMemHighValue = Channel::m_pCmdCom->readIniInteger("SYSTEM","mem_used",-1);
	if(m_iMemHighValue < 0){
		//ALARMLOG(0,MBC_schedule+8,"内存超限告警阀值%d不正确，使用默认值90",m_iMemHighValue);
		m_iMemHighValue = 90;
	}
	//系统变迁超时阀值
	m_iStateTimeOut = Channel::m_pCmdCom->readIniInteger("SCHEDULE","system_state_time_out",-1);
	if(m_iStateTimeOut < 0){
		//ALARMLOG(0,MBC_schedule+9,"系统变迁超时阀值%d不正确，使用默认值1800秒",m_iStateTimeOut);
		m_iStateTimeOut = 1800;
	}
	//checkpoint状态
	m_iCheckPointState = Channel::m_pCmdCom->readIniInteger("SYSTEM","checkpoint_state",-1);
	if(m_iCheckPointState < 0){
		//ALARMLOG(0,MBC_schedule+15,"checkpoint值%d不正确，使用默认值0",m_iCheckPointState);
		m_iCheckPointState = 0;
	}
	//告警日志开关
	//Log::m_iInfoLevel = Channel::m_pCmdCom->readIniInteger("log","log_level",-1);
	//Log::m_iAlarmLevel = Channel::m_pCmdCom->readIniInteger("log","log_alarm_level",-1);
	//Log::m_iAlarmDbLevel = Channel::m_pCmdCom->readIniInteger("log","log_alarmdb_level",-1);
	
  char cScheduleVp[200];
  char cCmd[100];
  memset(cScheduleVp,0,200);
  memset(cCmd,0,100);
  Channel::m_pCmdCom->readIniString("SCHEDULE","module_list",cScheduleVp,NULL);
  
  int iDiffWarn = Channel::m_pCmdCom->readIniInteger("SCHEDULE","diff_warning_level",50);
  int iSrcMax = 0;
  int iMax = 0;
  int iVpID = 0;
  if(cScheduleVp[0])
  {
  	char *pt = strtok(cScheduleVp," ,\n");
  	
  	iVpID = atoi(pt);
  	sprintf(cCmd,"vp%d.process_high_level",iVpID);
  	iSrcMax = Channel::m_pCmdCom->readIniInteger("SCHEDULE",cCmd,-1);
  	
  	
  	while((pt = strtok(NULL," ,\n")) != NULL)
  	{
  		 iVpID = atoi(pt);
  		 memset(cCmd,0,100);
  	   sprintf(cCmd,"vp%d.process_high_level",iVpID);
  	   iMax = Channel::m_pCmdCom->readIniInteger("SCHEDULE",cCmd,-1);
  	   
  	   if( (iSrcMax - iMax) > iDiffWarn)
  	   {
  	   	 /*Channel::m_pLog->log(MBC_PARAM_ERROR,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,
  	   	 "虚拟进程%d 最大高水位远小于上游虚拟进程,建议修改最大高水位为%d",iVpID*1000,iSrcMax);
  	   	 Log::log(0,"虚拟进程%d 最大高水位远小于上游虚拟进程,建议修改最大高水位为%d",iVpID*1000,iSrcMax); */
  	   	 break;
  	   }
  	   iSrcMax = iMax;  	   
  	}
  	
  }
  
}

/*
高低水调度，CheckPoint响应，状态变迁响应
*/
int Schedule::runEx()
{
	//首先制定日志文件名
	char sLogFile[256] ;
	memset(sLogFile, 0, sizeof(sLogFile));
	char* p= getenv( "ABM_PRO_DIR" );
	snprintf (sLogFile, sizeof(sLogFile), "%s/log/%s.log", p, "schedule");
	Log::setFileName (sLogFile);
	//Log::setAlarmName (sLogFile);
	
	Log::log(0,"流程调度开始启动");
	//INFOPOINT(1,"%s","流程调度开始启动");
	
	//向0号进程发起注册
	Log::log(0,"向initserver发起注册");
	//INFOPOINT(1,"%s","向initserver发起注册");
	registerToInitEx();
	
	//尝试连接守护进程
	Log::log(0,"连接GuradMain守护进程");
	//INFOPOINT(1,"%s","连接GuradMain守护进程");
	if(Channel::connectGuardMain() < 0){
		return -1;
	}
	Log::log(0,"开始运转");
	//读取核心参数流程调度的配置
	getSysConfigEx();
	//获取当前状态下系统需要管理的虚处理器
	getCurVirProcessor();
	
	char sLastState[50] = {0};//保存系统的当前状态
	int iRet = -1;
	int iType;
	long lRet;
	//strncpy(sLastState,m_sSysState,49);//第一次起来不做状态变迁，等待变迁指令发起
	//接管中断信号
	DenyInterrupt();
	while (!GetInterruptFlag())
	{
		
		//刷新核心参数
		refresh(); 
		
		//先置默认系统状态，中间可能会被修改
		//Channel::setSystemRunState(m_sRunState);
		
		//检测状态变迁
		if(strcmp(sLastState,m_sSysState) !=0 )	//检查到系统状态发生变化
		{
			Log::log(0,"系统状态%s尝试变迁为%s...",sLastState,m_sSysState);
			//INFOPOINT(1,"系统状态%s尝试变迁为%s...",sLastState,m_sSysState);
			/*
			//当系统状态要由maintenance变迁成online时，判断infomonitor是否存在如果不存在则需要启动
			if(strcmp(sLastState,"maintenance") == 0)
				InfoMonitor::runProcess("infomonitor",NULL);
			else if(strcmp(m_sSysState,"maintenance") == 0)//当系统第一次启动时需要启动infomonitor改写核心进程状态、时间
				InfoMonitor::runProcess("infomonitor",NULL);
			*/
			//状态变迁
			int iRet = stateTransitionEx();  
			if(iRet < 0)
			{ //变迁失败会不停的做变迁
				memset(sLastState,0,sizeof(sLastState));
				//modifySysState(m_sSysState);	//变迁失败也变目标态
				//变迁失败，虚拟的修改系统运行状态
				//Channel::setSystemRunState("fault");
				//ALARMLOG(0,MBC_schedule+1,"系统状态%s变迁%s失败！",sLastState,m_sSysState);
				//strcpy(sLastState,m_sSysState); 
			}
			else
			{
				memset(sLastState,0,sizeof(sLastState));
				//变迁成功，修改系统SYSTE.state=m_sSysState
				modifySysState(m_sSysState);		
				Log::log(0,"系统状态变迁为%s成功\n",m_sSysState); 
				//INFOPOINT(1,"系统状态%s变迁%s成功",sLastState,m_sSysState);
				strncpy(sLastState,m_sSysState,49);
			}
                   continue; 
		}
		
		//高低水自动调度
		iRet = doDynamicScheduleEx();		
		if(iRet == 0){
			//INFOPOINT(2,"%s","高低水自动调度关闭");
		}
		else if(iRet == -1){
			//高低水调度失败，修改运行时状态
			//Channel::setSystemRunState("fault");
		}
		else if(iRet == 1){
			Log::log(0,"需要配合checkpoint，暂时关闭自动调度");
			//INFOPOINT(1,"%s","需要配合checkpoint，暂时关闭自动调度");
		}
		else{
			//INFOPOINT(2,"%s","高低水自动调度完成");
		}
		
		/*
		//做checkpoint的检查
		iRet = checkPoint(); 
		if(iRet < 0){
			//修改运行时状态
			//Channel::setSystemRunState("fault");
			//ALARMLOG(0,MBC_schedule+15,"%s","checkpoint准备失败");
		}
		else if (iRet > 0){
			//修改核心参数为2,表示做好了checkpoint的准备
			modifyCheckPointState(2); 
			Log::log(0,"checkpoint准备完成");
			//INFOPOINT(1,"%s","checkpoint准备完成");
		}
		else{
			;
		}*/
		
		//系统运行态状态检测，虚拟修改系统运行状态
		//checkSysRunState(); 
		//在所有动作完成之后，修改系统运行时状态
		//Channel::modifySystemRunState();
		
		//备份态响应
		//doBackUp();
		
		checkSysRunStateEx();
		//采样间隔沉睡中
		isTimeOut();
		//INFOPOINT(2,"%s","开始下一轮调度");
	}

	return 0;
}
//高低水自动调度
int Schedule::doDynamicScheduleEx()
{
	//不启用自动调度
	if(!m_iAutoCtlFlag)							
		return 0;
	//需要做checkpoint 暂时"关闭"高低水
	if(m_iCheckPointState !=0 ){   
		return 1;
	}

	int iRet = -1;
	int  Flag = 0;								//继续检测的标志
	
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		iRet = m_vVirtual[i]->checkVirtualState();
		if(iRet<0){
			//Channel::setSystemRunState("fault");
		}
	}
	//先留出1秒的时间，等待进程状态的变化
	sleep(1);
	int iSleepCnt = 1;	//超时计数器
	int iWrongProc;			//有问题的进程号
	while(1)
	{
		Flag = 0;					//先假设不要检测了
		for(int j=0; j<m_vVirtual.size(); j++)
		{
			Flag = Flag | m_vVirtual[j]->m_iCheckFlag;//如果有虚处理器需要检测，Flag就为1
			
			if(m_vVirtual[j]->m_iCheckFlag == 0)//状态已符合要求，不需要变迁
			{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
				continue;
			}
			//继续检测标志位
			iRet = m_vVirtual[j]->checkProcState();
			if(iRet > 0){
				iWrongProc = iRet;
				//INFOPOINT(3,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
			}
			else{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
			}
		}
		//状态3次确认，超时判断
		if(1==Flag && iSleepCnt>m_iStartTime*3){
			//ALARMLOG(0,MBC_schedule+11,"进程[%d]状态改变超时",iWrongProc);
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			//INFOPOINT(1,"%s","所有虚处理器进程状态检测成功");
			break;
		}
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	return 2;
}
//状态变迁响应
int Schedule::stateTransitionEx(){
	Log::log(0,"首先处理数据源头的虚处理器...");
	//INFOPOINT(1,"%s","首先处理数据源头的虚处理器...");
	int iSleepCnt = 1; //超时计数器
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		m_vVirtual[i]->refreshEx();									//刷新参数
		m_vVirtual[i]->getProcInfoFromGuardMain();	//刷新配置
		m_vVirtual[i]->firstCheckProc();						//停数据源头 或者是直接起进程
		usleep(10000);															//每发一批启停指令，休息一会，给GuardMain留出时间fork业务进程
	}
	int iWrongProc;	//记录有问题的进程号
	int Flag = 0;
	int iRet;
	Log::log(0,"下游水位检测...");
	//再做下游水位检测，如果有一个不能停，则都不许停，如果要起的就起起来
	
	//这个模块只对非数据源的进程进行检测，如果前面数据源一直停不下来，造成一直有消息，那就会一直停不下来
	while(1) 
	{
		Flag = 0;
		for(int i=0; i<m_vVirtual.size(); i++)
		{
			if(m_vVirtual[i]->m_iSrcStopCheckFlag == 1 ) //数据源头是直接停得，不做水位的检查
				continue;
				//检查虚处理器能不能停，操作m_iCanStop变量
			m_vVirtual[i]->checkTask();								//下游进程水位检测
			Flag = Flag | m_vVirtual[i]->m_iCanStop;
		}
		if(1==Flag && iSleepCnt>m_iStateTimeOut){ 	//进程水位始终下不来
			//ALARMLOG(0,MBC_schedule+12,"%s","进程停止超时，始终有任务待处理");
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			break;
		} 
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	
	//如果是停，首先确保数据源头停止，这个模块只对数据源模块操作
	while(1)
	{
		Flag = 0; //先假设不要检测了
		for(int j=0; j<m_vVirtual.size(); j++)
		{
			if(m_vVirtual[j]->m_iSrcStopCheckFlag != 1 ) //如果不是待停止的数据源头，先不做检测
				continue;
				
			Flag = Flag | m_vVirtual[j]->m_iCheckFlag;//如果有虚处理器需要检测，Flag就为1
			
			if(m_vVirtual[j]->m_iCheckFlag == 0)//状态已符合要求，不需要变迁
			{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
				continue;
			}
			iRet = m_vVirtual[j]->checkProcState();
			if(iRet > 0)
			{
				iWrongProc = iRet;
				//INFOPOINT(3,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
				if(iSleepCnt%10 == 0)
					Log::log(0,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
			}
			else
			{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
			}
		}
		//状态变迁超时
		if(1==Flag && iSleepCnt>m_iStateTimeOut){											//超时
			//ALARMLOG(0,MBC_schedule+11,"进程[%d]状态改变超时",iWrongProc);
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			sleep(2);
			break;
		}
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	Log::log(0,"开始处理下游虚处理器...");
	//INFOPOINT(1,"%s","开始处理下游虚处理器...");

	Log::log(0,"所有下游虚处理器通过检测，开始状态变迁");
	//INFOPOINT(1,"%s","所有下游虚处理器通过检测，开始状态变迁");
	int iNeedStop = 0;
	for(int i=0; i<m_vVirtual.size(); i++) 			//把剩下的进程该停的停掉
	{
		iNeedStop = m_vVirtual[i]->stopLeftProc();						//停进程
		if(iNeedStop <= 0) //需要停得才休息一下，给GuardMain一点歇息
		 usleep(10000);														//先留出一定的时间，等待进程状态的变化
	}
	//检测进程状态变化
	while(1)
	{
		Flag = 0; //先假设不要检测了
		for(int j=0; j<m_vVirtual.size(); j++)
		{
			Flag = Flag | m_vVirtual[j]->m_iCheckFlag;//如果有虚处理器需要检测，Flag就为1
			
			if(m_vVirtual[j]->m_iCheckFlag == 0)//状态已符合要求，不需要变迁
			{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
				continue;
			}
				
			iRet = m_vVirtual[j]->checkProcState();
			if(iRet > 0){
				iWrongProc = iRet;
				//INFOPOINT(3,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
				if(iSleepCnt%10 == 0)
					Log::log(0,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
			}
			else{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId);
			}
		}
		if(1==Flag && iSleepCnt>m_iStateTimeOut){
			//ALARMLOG(0,MBC_schedule+11,"%s","进程[%d]状态改变超时",iWrongProc);
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			//INFOPOINT(3,"%s","所有虚处理器变迁成功");
			break;
		}
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	return 0;
}
//checkpoint状态响应,先停数据源，再检测下游水位，不停下游进程
int Schedule::checkPoint(){
	if(m_iCheckPointState != 1)  //不需要做checkpoint
		return 0;
	Log::log(0,"响应checkpoint,首先停止数据源头的虚处理器...");
	//INFOPOINT(1,"%s","响应checkpoint,首先停止数据源头的虚处理器...");
	int iSleepCnt = 1; //超时计数器
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		m_vVirtual[i]->refreshEx();									//刷新参数
		m_vVirtual[i]->getProcInfoFromGuardMain();	//刷新配置
		m_vVirtual[i]->stopAllSrcProc();						//直接停数据源头
		usleep(100000);
	}
	int iWrongProc;	//有问题的进程号
	int Flag = 0;
	int iRet;
	while(1) //确保数据源头停止
	{
		Flag = 0; //先假设不要检测了
		for(int j=0; j<m_vVirtual.size(); j++)
		{
			if(m_vVirtual[j]->m_iSrcStopCheckFlag != 1 ) //如果不是待停止的数据源头，先不做检测
				continue;
				
			Flag = Flag | m_vVirtual[j]->m_iCheckFlag;//如果有虚处理器需要检测，Flag就为1
			
			if(m_vVirtual[j]->m_iCheckFlag == 0)//状态已符合要求，不需要变迁
			{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
				continue;
			}
			iRet = m_vVirtual[j]->checkProcState();
			if(iRet > 0){
				iWrongProc = iRet;
				//INFOPOINT(3,"第%d次检测:虚处理器[%d]中进程[%d]未达目标态",iSleepCnt,m_vVirtual[j]->m_iVirtualId,iRet);
			}
			else{
				//INFOPOINT(3,"虚处理器[%d]中所有进程已达目标态",m_vVirtual[j]->m_iVirtualId);
			}
		}
		if(1==Flag && iSleepCnt>m_iStateTimeOut){											//暂时先这么写，时间待确定
			//ALARMLOG(0,MBC_schedule+11,"进程[%d]状态改变超时",iWrongProc);
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			break;
		}
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	Log::log(0,"响应checkpoint,开始检测下游虚处理器水位...");
	//INFOPOINT(1,"%s","响应checkpoint,开始检测下游虚处理器水位...");
	while(1) //再做下游水位检测
	{
		Flag = 0;
		for(int i=0; i<m_vVirtual.size(); i++)
		{
			m_vVirtual[i]->checkTask();								//下游进程水位检测
			Flag = Flag | m_vVirtual[i]->m_iCanStop;
		}
		if(1==Flag && iSleepCnt>m_iStateTimeOut){ 	//进程水位始终下不来
			//ALARMLOG(0,MBC_schedule+12,"%s","进程停止超时，始终有任务待处理");
			//Channel::setSystemRunState("fault");
			return -1;
		}
		if(0==Flag){
			break;
		}
		else{
			sleep(1);
			iSleepCnt++;
		}
	}
	return 1;
}

int Schedule::doBackUp(){
	DEFINE_QUERY(qry);
	char sSQL[1024] = {0};
	char sState[20] = {0};
	sprintf(sSQL,"%s","select state from b_backup_state_process where process_id=-1");
	try{
		qry.close();
		qry.setSQL(sSQL);
		qry.open();
		if(qry.next()){
			strncpy(sState,qry.field("state").asString(),19);
		}
		else{
			qry.close();
			return 0;
		}
		qry.close();
	}
	catch(TOCIException& e){
		Log::log(0,"%s","读取备份态状态失败");
		qry.close();
		return  -1;
	}
	if( !strcmp(sState,"END") || !strcmp(sState,"RUN") ){
		return 0;
	}
	if( !strcmp(sState,"OUT") ){
		moveBackData();//把数据挪回来
		memset(sSQL,0,sizeof(sSQL));
		sprintf(sSQL,"%s","update b_backup_state_process set state='END',out_date=sysdate where process_id=-1 and state='OUT' ");
	  try{
	  	qry.close();
			qry.setSQL(sSQL);
			qry.execute();
			qry.commit();
	  }
	  catch(TOCIException& e) 
	  {
	  	Log::log(0,"%s","更新schedule备份态失败");
	    qry.close();
	    return -1;
	  }
	  qry.close();
	  Log::log(0,"%s","schedule备份态结束");
	  return 0;
	}
	if(strcmp(sState,"RDY") != 0)
		return 0;
	int Flag = 0;
	int iSleepCnt = 0;
	for(int i=0; i<m_vVirtual.size(); i++)
	{
		m_vVirtual[i]->refreshEx();									//刷新参数
		m_vVirtual[i]->getProcInfoFromGuardMain();	//刷新配置
	}
	while(1) //再做下游水位检测
	{
		Flag = 0;
		for(int i=0; i<m_vVirtual.size(); i++)
		{
			m_vVirtual[i]->checkBackUpTask();								//下游进程水位检测
			Flag = Flag | m_vVirtual[i]->m_iCanStop;
		}
		if(1==Flag && iSleepCnt>900){ 	//进程水位始终下不来
			//ALARMLOG(0,MBC_schedule+12,"%s","备份态等待水位为0超时");
			return -1;
		}
		if(0==Flag){
			memset(sSQL,0,sizeof(sSQL));
			sprintf(sSQL,"%s","update b_backup_state_process set state='RUN',in_date=sysdate where process_id=-1 and state='RDY' ");
		  try{
		  	qry.close();
				qry.setSQL(sSQL);
				qry.execute();
				qry.commit();
		  }
		  catch(TOCIException& e) 
		  {
		  	Log::log(0,"%s","更新schedule备份态失败");
		    qry.close();
		    return -1;
		  }
			Log::log(0,"schedule备份态准备成功");
			break;
		}
		else{
			sleep(1);

			iSleepCnt++;
		}
	}
}
int Schedule::moveBackData(){
	DEFINE_QUERY(qry);
	char sSQL[1024] = {0};
	sprintf(sSQL,"%s","begin insert into b_package_state_merge "
                    "select * from b_package_state_merge_backup; "
                    "delete from b_package_state_merge_backup; "
                    "end;" );
  try{
		qry.setSQL(sSQL);
		qry.execute();
		qry.commit();
  }
  catch(TOCIException& e) 
  {
  	Log::log(0,"%s","执行数据转移时出现错误");
    qry.close();
    return -1;
  }
  qry.close();
	return 0;
}

bool Schedule::GetMemInfo()
{
	m_iMemPercent = 0;
	int MemTotal;
	int MemFree;
	char ScanBuff[256]={'\0'};
	char sTmpBuff[128]={'\0'};
	char sTmp[128]={'\0'};
	char sTotalMem[128]={'\0'};
	char sFreeMem[128]={'\0'};
	char *p=NULL;
	FILE * fp=NULL;

	#ifdef DEF_LINUX
	strcpy(ScanBuff," free -m|sed -n '2p'|awk '{print $2}' ");
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareMemPercent Error");
	}
	if (fgets(sTmpBuff,sizeof(sTmpBuff),fp))
	{
		pclose(fp);
		strcpy(sTotalMem,sTmpBuff);
		
	}
	else
	{
		pclose(fp);
		return false;
	}
	#else
	
	#ifdef DFF_AIX
	strcpy(ScanBuff," vmstat |grep \"System\"|grep -v \"grep\"|awk '{print $4}' ");//命令结果为mem=41984MB
	#endif
	#ifdef DEF_HP
	strcpy(ScanBuff," machinfo|grep \"Memory\"|awk '{print $2}' ");//Memory = 16353 MB (15.969727 GB)
	#endif
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareMemPercent Error");
	}
	if (fgets(sTmpBuff,sizeof(sTmpBuff),fp))
	{
		pclose(fp);
		#ifdef DEF_AIX
		//strcpy(sTmp,&sTmpBuff[4]);//去掉前四位mem=
		p=strstr(sTmpBuff,"=");
		strcpy(sTmp,p+1);
		p=NULL;
		p=index(sTmp,'M');
		strncpy(sTotalMem,sTmp,p-sTmp);//去掉后两位MB
		strcat(sTotalMem,'\0');
		#endif	
		
		#ifdef DEF_HP
		 strcpy(sTotalMem,sTmpBuff);
		#endif	
	}
	else
	{
		pclose(fp);
		return false;
	}
	#endif

	memset(ScanBuff,0x00,sizeof(ScanBuff));

	#ifdef DFF_AIX
	//hpunix vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total += $4} END{print total/4/1024}'
	strcpy(ScanBuff," vmstat  |sed -n '7p'|awk '{print $4/256}' ");//获取FREE内存，已经转化为MB
	#endif
	#ifdef DEF_HP
	strcpy(ScanBuff," vmstat  |sed -n '3p'|awk '{print $5/256}' ");//获取FREE内存，已经转化为MB
	#endif
	#ifdef DEF_LINUX
	strcpy(ScanBuff," vmstat  |sed -n '3p'|awk '{print $4/1024}' ");//获取FREE内存，已经转化为MB	
	#endif
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		return false;
	}
	if (fgets(sFreeMem,sizeof(sFreeMem),fp))
	{
		pclose(fp);	
	}
	else
	{
		pclose(fp);
		return false;
	}
	// //## 返回使用百分比，精确到整数
	MemTotal = atoi(sTotalMem);
	if(MemTotal == 0)
	{
		 return false;
	}
	MemFree = atoi(sFreeMem);
	
	m_iMemPercent=((MemTotal-MemFree)*100/MemTotal);
	return true;
}


/******************************************************************************************
*输入：	无
*输出：	无
*返回：	无
*调用：	成员函数
*描述：	准备CPU比例
*******************************************************************************************/
bool Schedule::GetCpuInfo()
{
	m_iCpuPercent = 0;
	FILE *fp=NULL;
	char ScanBuff[256]={'\0'};
	char sCpuPercent[256]={'\0'};
 
	memset(sCpuPercent,0x00,sizeof(sCpuPercent));

	/*
	//ps aux|sed -n '2,$ p' |awk 'BEGIN{total = 0} {total += $3} END{print total}'
	//hpunix vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total +=$14} END{print total/4}'
	#ifdef DEF_AIX
	strcpy(ScanBuff," vmstat  |sed -n '7p'|awk '{print $16}' ");
	#endif
	#ifdef DEF_HP
	strcpy(ScanBuff," vmstat  |sed -n '3p'|awk '{print $18}' ");
	#endif
	#ifdef DEF_LINUX
	//strcpy(ScanBuff," vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total +=$15} END{print total/4}' ");
  strcpy(ScanBuff," vmstat |sed -n '3p'|awk '{print $15}' ");	
	#endif
	*/

#ifdef DEF_AIX
	strcpy(ScanBuff," vmstat 1 2 |sed -n '8p'|awk '{print $16}' ");
#endif
#ifdef DEF_HP
	strcpy(ScanBuff," vmstat 1 2 |sed -n '4p'|awk '{print $18}' ");
#endif
#ifdef DEF_LINUX
	strcpy(ScanBuff," vmstat 1 2 |sed -n '4p'|awk '{print $15}' "); 
#endif

	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareCpuPercent Error");
	}
	if (fgets(sCpuPercent,sizeof(sCpuPercent),fp))
	{
		pclose(fp);
		m_iCpuPercent=100-atoi(sCpuPercent);
		return true;

	}
	else
	{
		pclose(fp);
		return false;
	}
}
