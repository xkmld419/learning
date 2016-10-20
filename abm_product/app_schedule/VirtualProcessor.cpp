
#include "StdAfx.h"


int VirtualProcessor::g_RunProcNum = 0;
/***********************************************
 *	Function Name	:构造函数
 *	Description	:初始化各变量
 *	Input Param	:
 *	Returns		:
 *	complete	:
 ***********************************************/
VirtualProcessor::VirtualProcessor(int ID)
{
	m_iVirtualId = ID;	
	m_iChLastState = -1;
	m_iCnt = 0;
	m_iCheckFlag = 0;//默认表示不需要检测，只要有启停进程的动作就要检测
	//m_dLastTime.getCurDate();
	time(&m_iLastTime);
	m_iSrcStopCheckFlag = 0; //这个表示数据源头是否做了停标记
	m_iCanStop = 1; //这个表示进程能否停止的标志，有水则不能停
	m_iForceStop = 0; //非强停标识 
	m_iBusyFlag = 0;
	m_iBackUpSrc = 0; //默认不是备份态的源 
}
/***********************************************
 *	Function Name	:析构函数
 *	Description	:释放资源
 *	Input Param	:
 *	complete	:
 *	Returns		:
 ***********************************************/
VirtualProcessor::~VirtualProcessor()
{
}
/***********************************************
 *	Function Name	:
 *	Description	:获取该虚处理器下所有的进程
 *	Input Param	:
 *	complete	:
 *	Returns		:
 ***********************************************/

int VirtualProcessor::getProcInfoFromGuardMain()
{
	m_iCnt = 0;
	bool bHaveOp = false;
	if(m_iVirtualId <= 0)//虚处理器ID有误
		return -1;

	for (int i=0; i<Channel::m_pInfoHead->iProcNum; i++)			//遍历所有进程
	{
		if(m_iVirtualId == (Channel::m_pProcInfoHead+i)->iVpID && (Channel::m_pProcInfoHead+i)->iProcessID != -1)		//查找
		{
			m_ProcInfoList[m_iCnt].iPos = i;
			m_ProcInfoList[m_iCnt].iOpFlag = 0;
			m_ProcInfoList[m_iCnt].iDestState = -1;
			m_iCnt++;
		}
	}
	
//	//如果配置的虚拟进程数小于最大进程数，向wf_process表里面插入一条记录
//	if((m_iMaxProc > m_iCnt || m_iMinProc > m_iCnt) && m_iIfInsert)
//	{
//	 int iPos = m_ProcInfoList[m_iCnt-1].iPos;
//	 int iOpTimes = 0;
//	 
//	 if(m_iMaxProc > m_iCnt)
//	 {
//	 	iOpTimes = m_iMaxProc - m_iCnt;
//	 }
//	 else
//	 {
//	 	 iOpTimes = m_iMinProc - m_iCnt;
//	 }
//	 
//	 if((Channel::m_pProcInfoHead+iPos)->iProcessID < 0)
//	 {
//	 	  Log::log(0,"虚处理器%d发现错误进程ID%d",m_iVirtualId,(Channel::m_pProcInfoHead+iPos)->iProcessID);
//	 	  return 0;
//	 }
//	 try
//   {
//   	DEFINE_QUERY(qry);
//  
//    char cSql[1024] = {0};
//    
//    char sParam[100] = {0};
//    /*
//    for(int i = 1;i<= iOpTimes;i++)
//    {
//    	memset(cSql,0,1024);
//    	memset(sParam,0,100);
//    	
//     //sprintf(sParam,"'-m %d -p %d'",(Channel::m_pProcInfoHead+iPos)->iAppID,(Channel::m_pProcInfoHead+iPos)->iProcessID+i);
// 		 sprintf(sParam,"''");
//		 sprintf(cSql,"insert into wf_process(process_id,caption,hint,app_id,parameter, \
//		 left,top,width,height,billflow_id,auto_run,exec_path,db_user_id,host_id,state, \
//		 sys_username,vp_id,org_id,PRIORITY_TYPE,RUN_MESSAGE,STOP_MESSAGE,ROLLBACK_MESSAGE,DICT_FILE) \
//		 select %d,caption,hint,app_id,%s, \
//		 left,top,width,height,billflow_id,auto_run,exec_path,db_user_id,host_id,state, \
//		 sys_username,vp_id,org_id,PRIORITY_TYPE,RUN_MESSAGE,STOP_MESSAGE,ROLLBACK_MESSAGE,DICT_FILE \
//		 from wf_process where process_id = %d",
//		 (Channel::m_pProcInfoHead+iPos)->iProcessID+i,sParam,(Channel::m_pProcInfoHead+iPos)->iProcessID);
//				
//		 qry.setSQL(cSql);   
//	  
//		 qry.execute();
//		 qry.commit();
//		 qry.close();
//	   
//	   
//		 if(((Channel::m_pProcInfoHead+iPos)->iMqID) > 0)
//		 {
//		   memset(cSql,0,sizeof(cSql));
//		   sprintf(cSql,"insert into wf_process_mq(process_id,mq_id) values(%d,%d)",(Channel::m_pProcInfoHead+iPos)->iProcessID+i,(Channel::m_pProcInfoHead+iPos)->iMqID);
//		   qry.setSQL(cSql);
//		   qry.execute();
//		   qry.commit();
//		   qry.close();
//
//		   memset(cSql,0,sizeof(cSql));
//		   sprintf(cSql,"insert into wf_distribute_rule (process_id,out_process_id,CONDITION_ID,SEND_MODE) \
//						select %d,out_process_id,CONDITION_ID,SEND_MODE from wf_distribute_rule \
//						where process_id = %d ",
//						(Channel::m_pProcInfoHead+iPos)->iProcessID+i,(Channel::m_pProcInfoHead+iPos)->iProcessID);
//		   qry.setSQL(cSql);
//		   qry.execute();
//		   qry.commit();
//		   qry.close();
//		 }
//    }
//    */
//    for(int i = 1;i<= iOpTimes;i++)
//    {
//    	memset(cSql,0,1024);
//    	memset(sParam,0,100);
//    	
//     //sprintf(sParam,"'-m %d -p %d'",(Channel::m_pProcInfoHead+iPos)->iAppID,(Channel::m_pProcInfoHead+iPos)->iProcessID+i);
// 		 sprintf(sParam,"''");
// 		 
// 		 sprintf(cSql,"delete from wf_process where process_id=%d",(Channel::m_pProcInfoHead+iPos)->iProcessID+i);
// 		 qry.setSQL(cSql);
//	  
//		 qry.execute();
//		 qry.commit();
//		 qry.close();
// 		 
// 		 memset(cSql,0,sizeof(cSql));
//		 sprintf(cSql,"insert into wf_process(process_id,caption,hint,app_id,parameter, \
//		 left,top,width,height,billflow_id,auto_run,exec_path,db_user_id,host_id,state, \
//		 sys_username,vp_id,org_id,PRIORITY_TYPE,RUN_MESSAGE,STOP_MESSAGE,ROLLBACK_MESSAGE,DICT_FILE) \
//		 select %d,caption,hint,app_id,parameter, \
//		 left,top,width,height,billflow_id,auto_run,exec_path,db_user_id,host_id,state, \
//		 sys_username,vp_id,org_id,PRIORITY_TYPE,RUN_MESSAGE,STOP_MESSAGE,ROLLBACK_MESSAGE,DICT_FILE \
//		 from wf_process where process_id = %d",
//		 (Channel::m_pProcInfoHead+iPos)->iProcessID+i,(Channel::m_pProcInfoHead+iPos)->iProcessID);
//		
//		 
//		 qry.setSQL(cSql);
//	  
//		 qry.execute();
//		 qry.commit();
//		 qry.close();
//	   
//	   
//		 if(((Channel::m_pProcInfoHead+iPos)->iMqID) > 0)
//		 {
//		   memset(cSql,0,sizeof(cSql));
//	 		 sprintf(cSql,"delete from wf_process_mq where process_id=%d",(Channel::m_pProcInfoHead+iPos)->iProcessID+i);
//	 		 qry.setSQL(cSql);		  
//			 qry.execute();
//			 qry.commit();
//			 qry.close();
//		   
//		   memset(cSql,0,sizeof(cSql));
//		   sprintf(cSql,"insert into wf_process_mq(process_id,mq_id) values(%d,%d)",(Channel::m_pProcInfoHead+iPos)->iProcessID+i,(Channel::m_pProcInfoHead+iPos)->iMqID);
//		   qry.setSQL(cSql);
//		   qry.execute();
//		   qry.commit();
//		   qry.close();
//		   
//		   sprintf(cSql,"delete from wf_distribute_rule where process_id=%d",(Channel::m_pProcInfoHead+iPos)->iProcessID+i);
//		   qry.setSQL(cSql);
//		   qry.execute();
//		   qry.commit();
//		   qry.close();
//
//		   memset(cSql,0,sizeof(cSql));
//		   sprintf(cSql,"insert into wf_distribute_rule (process_id,out_process_id,CONDITION_ID,SEND_MODE) \
//						select %d,out_process_id,CONDITION_ID,SEND_MODE from wf_distribute_rule \
//						where process_id = %d ",
//						(Channel::m_pProcInfoHead+iPos)->iProcessID+i,(Channel::m_pProcInfoHead+iPos)->iProcessID);
//		   qry.setSQL(cSql);
//		   qry.execute();
//		   qry.commit();
//		   qry.close();
//		 }
//    }
//    //qry.commit();
//    //qry.close();
//    
//	Channel::m_pInfoHead->iRefreshFlag = 1;
//	int iTimes = 5;
//	while (iTimes--)
//	{ 
//	  if (Channel::m_pInfoHead->iRefreshFlag == 2)
//	  { 
//		 return getProcInfoFromGuardMain();  
//	  }
//		sleep(1);
//	} 
//
//	Log::log(0,"共享内存刷新失败");  
//	}
//	catch (TOCIException &oe) 
//	{    
//		printf(oe.getErrMsg());  
//		printf(oe.getErrSrc());
//		throw oe;
//	}
//	}
//
//	if((m_iCnt > m_iMaxProc) && m_iIfDelete)
//	{
//	int iPos;
//	int iOpTimes = m_iCnt - m_iMaxProc;
//	DEFINE_QUERY(qry);
//	char cSql[1024];
//	for(int i=0;i<iOpTimes;i++)
//	{
//		iPos = m_ProcInfoList[m_iCnt-1-i].iPos;
//		if((Channel::m_pProcInfoHead+iPos)->iProcessID < 0)
//		{
// 			Log::log(0,"虚处理器%d发现错误进程ID%d",m_iVirtualId,(Channel::m_pProcInfoHead+iPos)->iProcessID);
// 			return 0;
//		}
//		if((Channel::m_pProcInfoHead+iPos)->iState == ST_SLEEP)
//		{
//			memset(cSql,0,1024);
//			sprintf(cSql,"delete from wf_process_mq where process_id = %d",
//				(Channel::m_pProcInfoHead+iPos)->iProcessID);
//			qry.setSQL(cSql);
//			qry.execute();
//			qry.commit();
//			qry.close();
//			memset(cSql,0,1024);
//			sprintf(cSql,"delete from wf_process where process_id = %d",
//				(Channel::m_pProcInfoHead+iPos)->iProcessID);
//			qry.setSQL(cSql);
//			qry.execute();
//			qry.commit();
//			qry.close();
//			sprintf(cSql,"delete from wf_distribute_rule where process_id = %d",
//				(Channel::m_pProcInfoHead+iPos)->iProcessID);
//			qry.setSQL(cSql);
//			qry.execute();
//			qry.commit();
//			qry.close();			
//			
//			bHaveOp = true;
//		}	
//	}
//  	
//  	int iTimes = 5;
//  	Channel::m_pInfoHead->iRefreshFlag = 1;
//    while (bHaveOp && iTimes--)
//    { 
//      if (Channel::m_pInfoHead->iRefreshFlag == 2)
//      { 
//          break;
//      }
//       sleep(1);
//    }
//    
//    if(iTimes <= 0)
//    {
//    	Log::log(0,"删除进程时，共享内存刷新失败");  
//    }
//    //不管刷新成功没有，都重新取一边
//    if(bHaveOp)
//    {
//      m_iCnt = 0;
//      for (int i=0; i<Channel::m_pInfoHead->iProcNum; i++)			//遍历所有进程
//	    {
//		     if(m_iVirtualId == (Channel::m_pProcInfoHead+i)->iVpID)		//查找
//		     {
//			     m_ProcInfoList[m_iCnt].iPos = i;
//			     m_ProcInfoList[m_iCnt].iOpFlag = 0;
//			     m_ProcInfoList[m_iCnt].iDestState = -1;
//			     m_iCnt++;
//		     }
//	    }	
//	  }  
//  }

	return m_iCnt;
}
/***********************************************
 *	Function Name	:
 *	Description	:获取该虚处理器应做的启停操作
 *	Input Param	:
 *	complete	:
 *	Returns		:
 ***********************************************/
int VirtualProcessor::getVirtualActionEx()
{
	static time_t tNow;
	time(&tNow);
	
	int iState = Channel::calcWater(m_iVirtualId);				//计算本通道水位
	//int iNextState = Channel::calcNextWater(m_iVirtualId);	//计算后置通道水位
	
	int iRunProcCnt = getBusyProcNumEx();
	if(iRunProcCnt == 0 && iState!=CHANNEL_EMPTY){  //如果没有进程在运行，但有水就该起
		return START;
	}
  else if( (iState == CHANNEL_EMPTY) && (tNow-m_iLastTime > m_iDurTimeOut) )	
		return STOP;
	//低水，需要停
	else if( (iState == CHANNEL_LOW) && (tNow-m_iLastTime > m_iDurTimeOut) )
		return STOP;
	//高水，需要起
	else if( (iState == CHANNEL_HIGH || iState == CHANNEL_FULL) && (tNow-m_iLastTime > m_iDurTimeOut) )
		return START;
	else{
		return NOACTION_NOMAL;
	}
	
	//取消这些逻辑判断，改成有水就起，没水就停
	//采样时间内，通道水位状态发生改变，重记开始时间
	/*
	if(iState != m_iChLastState)
	{
		time(&m_iLastTime);
		m_iChLastState = iState; //重置状态
		return NOACTION_STATE;															//通道水位初变，不做动作
	}
	
	//在采样时间内水位没有发生改变
	if( tNow-m_iLastTime < m_iDurTimeOut)					//但未达到时间阀值
		return NOACTION_TIME;																//通道水位状态持续时间不够，不做动作
		
	//水位持续时间已达阀值
	if(  ( (iState==CHANNEL_HIGH) || (iState==CHANNEL_FULL) )  && ( (iNextState!=CHANNEL_HIGH) && (iNextState!=CHANNEL_FULL) ) )		//通道的状态 == 高水 && 后置通道状态 != 高水
	{
		//m_dLastTime = curTime; 															//可能导致又启动了一个进程，应重记开始时间
		return START;																				//返回需启动进程动作
	}
	else if( (iState==CHANNEL_LOW) || (iState==CHANNEL_EMPTY) )				//通道状态 == 低水
	{
		//m_dLastTime = curTime; 															//可能导致又停止了一个进程，重记开始时间
		return STOP;																				//返回需停止进程动作
	}
	else if( iState==CHANNEL_NOMAL )															//通道状态 == 正常 则无动作
		return NOACTION_NOMAL;
	else
		return NOACTION_NEXT;																//否则，则是由于后通道高水位致使不能启动新进程
	*/
}


//返回ST_INIT ST_WAIT_BOOT ST_RUNNING的进程个数，同时对ST_INIT ST_WAIT_BOOT的进程打上待检测标记
int VirtualProcessor::getBusyProcNumEx()
{
	int ret = 0;
	int iPos;
	TProcessInfo *pProcInfo = Channel::m_pProcInfoHead;//指向进程首地址
	for(int i=0; i<m_iCnt; i++)
	{
		iPos = m_ProcInfoList[i].iPos;
		if( ((pProcInfo+iPos)->iState == ST_INIT) || ((pProcInfo+iPos)->iState == ST_WAIT_BOOT) )
		{
			ret++;
			m_ProcInfoList[i].iOpFlag = 1; //这种预启动的进程，先做为启动，然后再来检测是否成功
		}
		else if( (pProcInfo+iPos)->iState == ST_RUNNING || (pProcInfo+iPos)->iState == ST_DESTROY) //modify on 9/18
		{
			ret++;
		}
	}
	return ret; 
}

// 获取一个运行的进程，要求没有待检测的标记
int VirtualProcessor::getOneBusyProcEx(){
	int iPos;
	for(int i=m_iCnt-1; i>=0; i--){ //倒着查询，便于后起的先停
		iPos = m_ProcInfoList[i].iPos;
		if( m_ProcInfoList[i].iOpFlag==0 && ((Channel::m_pProcInfoHead+iPos)->iState==ST_RUNNING || (Channel::m_pProcInfoHead+iPos)->iState==ST_INIT || (Channel::m_pProcInfoHead+iPos)->iState==ST_WAIT_BOOT) ){
			return i;//返回进程的位置
		}
	}
	return -1;
}
// 获取一个空闲的进程，要求没有待检测的标记，对于ABORT的进程再看开关
int VirtualProcessor::getOneIdleProcEx(){
	int iPos;
	for(int i=0; i<m_iCnt; i++){ //正着查询，便于进程号小的先起
		iPos = m_ProcInfoList[i].iPos;
		if( m_ProcInfoList[i].iOpFlag==0 && ((Channel::m_pProcInfoHead+iPos)->iState==ST_SLEEP || (Channel::m_pProcInfoHead+iPos)->iState==ST_KILLED )){
			return i;//返回进程的位置
		}
		else if(m_ProcInfoList[i].iOpFlag==0 && (Channel::m_pProcInfoHead+iPos)->iState==ST_ABORT && m_iAbort>0){
			return i;
		}
	}
	return -1;
}
//停止一个正在运行的进程，只是改变其状态和标记，不做成功失败的判断
int VirtualProcessor::stopOneProcEx(){
	int	iIndex = getOneBusyProcEx();
	if(iIndex < 0)
		return -1;
	int iPos = m_ProcInfoList[iIndex].iPos;
	(Channel::m_pProcInfoHead+iPos)->iState = ST_WAIT_DOWN; //修改其状态，使GuardMain获知
	m_ProcInfoList[iIndex].iDestState = ST_SLEEP; //指定其目标状态
	m_ProcInfoList[iIndex].iOpFlag = 1; //打上标志 表示需要检测
	m_iCheckFlag = 1;//有进程状态改变，则虚处理器需要检测
	g_RunProcNum--;
	Log::log(0,"虚处理器%d尝试停止进程%d",m_iVirtualId,(Channel::m_pProcInfoHead+iPos)->iProcessID);
	return 0;
}
//启动一个已经终止的进程，只是改变其状态和标记，不做成功失败的判断
int VirtualProcessor::startOneProcEx(){
	if(g_RunProcNum >= m_iSysMaxProcCnt){
		   
		  Channel::m_pLog->log(MBC_MAX_SYSTEM_PROCESS,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,"达到系统最大进程限制");
		return -1;
	}
	int	iIndex = getOneIdleProcEx();
	if(iIndex < 0){
		Log::log(0,"***告警***：虚处理器[%d]已无进程可用，请检测",m_iVirtualId);
		////ALARMLOG(0,MBC_schedule+27,"虚处理器[%d]已无进程可用，请检测",m_iVirtualId);
		return -1;
	}
	int iPos = m_ProcInfoList[iIndex].iPos;
	(Channel::m_pProcInfoHead+iPos)->iState = ST_WAIT_BOOT; //修改其状态，使GuardMain获知
	m_ProcInfoList[iIndex].iDestState = ST_RUNNING; //指定其目标状态
	m_ProcInfoList[iIndex].iOpFlag = 1; //打上标志 下次检测
	m_iCheckFlag = 1;
	g_RunProcNum++;
	Log::log(0,"虚处理器%d尝试启动进程%d",m_iVirtualId,(Channel::m_pProcInfoHead+iPos)->iProcessID);
	return 0;
}
//检测是否到达目标态，对m_iCheckFlag值进程操作，返回是那个进程的原因停不下来
int VirtualProcessor::checkProcState(){
	int iPos;

	for(int i=0; i<m_iCnt; i++)
	{
		m_iCheckFlag = 0;              //先假设不需要检测了
		iPos = m_ProcInfoList[i].iPos; //标出进程位置
		if( (m_ProcInfoList[i].iOpFlag==1) && ((Channel::m_pProcInfoHead+iPos)->iState != m_ProcInfoList[i].iDestState) )
			{			
				if( ( (Channel::m_pProcInfoHead+iPos)->iState==ST_ABORT || 
					  ( Channel::m_pProcInfoHead+iPos)->iState==ST_KILLED || 
						( Channel::m_pProcInfoHead+iPos)->iState==ST_SLEEP )  )
			 { //特殊处理 modify on 20100930 先让其通过检测 下次报警 否则变迁老是失败
				 m_ProcInfoList[i].iOpFlag = 0;	
				 m_ProcInfoList[i].iCheckCnt = 0;
				 //Channel::setSystemRunState("fault");
				 //ALARMLOG(0,MBC_schedule+14,"进程%d状态异常",(Channel::m_pProcInfoHead+iPos)->iProcessID);
				 continue;
			 }
			 m_iCheckFlag = 1;
			 return (Channel::m_pProcInfoHead+iPos)->iProcessID;// 此进程没有到达目标态
		}
		else if(m_ProcInfoList[i].iOpFlag==1 )
		{//已达目标态，但需要要多次确认
			m_ProcInfoList[i].iCheckCnt++;
		}

		if(m_ProcInfoList[i].iCheckCnt > 1)
		{//既然已确定到达目标态，可消除待检测的标志，计数器清0
			m_ProcInfoList[i].iOpFlag = 0;	
			m_ProcInfoList[i].iCheckCnt = 0;
			/*
			if( ST_RUNNING == (Channel::m_pProcInfoHead+iPos)->iState ) //如果进程是在运行，则增加一个正在运行的进程个数
				g_RunProcNum++;
			else
				g_RunProcNum--;
				*/
		}
		
		m_iCheckFlag = m_iCheckFlag | m_ProcInfoList[i].iOpFlag; //只要有一个进程要检测，虚处理器就需要检测
	}
	return 0;
}
int VirtualProcessor::isBusy(){
	return m_iBusyFlag;
}
/***********************************************
 *	Function Name	:
 *	Description	:调整不合理配置，响应高低水
 *	Input Param	:档位
 *	Returns		:
 *	complete	:
 ***********************************************/
int VirtualProcessor::refreshEx()
{
	static char sTemp[100] = {0};
	memset(sTemp,0,100);
	int iTempVirtualId = m_iVirtualId/1000;
	sprintf(sTemp,"vp%d.min_process",iTempVirtualId);
	m_iMinProc = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iMinProc<0){
		Log::log(0,"***告警***：虚处理器[%d]非法的最小进程数参数%d，使用默认值0",m_iVirtualId,m_iMinProc);
		////ALARMLOG(0,MBC_schedule+16,"虚处理器[%d]非法的最小进程数参数%d，使用默认值0",m_iVirtualId,m_iMinProc);
		m_iMinProc = 1;  
	}
	memset(sTemp,0,100);
	sprintf(sTemp,"vp%d.max_process",iTempVirtualId);
	m_iMaxProc = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iMaxProc < 0){
		Log::log(0,"***告警***：虚处理器[%d]非法的最大进程数参数%d，使用默认值0",m_iVirtualId,m_iMaxProc);
		////ALARMLOG(0,MBC_schedule+17,"虚处理器[%d]非法的最大进程数参数%d，使用默认值0",m_iVirtualId,m_iMaxProc);
		m_iMaxProc = 1;
	}

  m_iIfDelete = 1;
  m_iIfDelete = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "change_delete", 1);
  	
  m_iIfInsert = 1;
  m_iIfInsert = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "change_insert", 1);
	memset(sTemp,0,100);
	sprintf(sTemp,"vp%d.shake_time",iTempVirtualId);
	m_iDurTimeOut = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iDurTimeOut<0){
		Log::log(0,"***告警***：虚处理器[%d]非法的防抖动时间参数为%ds，使用默认值60s",m_iVirtualId,m_iDurTimeOut);
		////ALARMLOG(0,MBC_schedule+18,"虚处理器[%d]非法的防抖动时间参数为%ds，使用默认值60s",m_iVirtualId,m_iDurTimeOut);
		m_iDurTimeOut = 60;
	}
	memset(sTemp,0,100);
	sprintf(sTemp,"vp%d.hwm_switch",iTempVirtualId);
	m_iAutoSchedule = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iAutoSchedule<0){
		////ALARMLOG(0,MBC_schedule+19,"虚处理器[%d]非法的自动调度参数为%ds，默认不参与调度",m_iVirtualId,m_iAutoSchedule);
		m_iAutoSchedule = 0;
	}
	m_iAbort = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "force_start_abort", -1);
	if(m_iAbort < 0){
		Log::log(0,"***告警***：系统强制启停ABORT进程的开关%d不正确，使用默认值0",m_iAbort);
		////ALARMLOG(0,MBC_schedule+19,"系统强制启停ABORT进程的开关%d不正确，使用默认值0",m_iAbort);
		m_iAbort = 0;
	}
	m_iShift = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "global_shift", -1);
	if(m_iShift < 0){
		Log::log(0,"***告警***：系统档位%d不正确，使用默认值10",m_iShift);
		////ALARMLOG(0,MBC_schedule+20,"系统档位%d不正确，使用默认值10",m_iShift);
		m_iShift = 10;
	}
	
	m_iBusyTimeOut = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "schedule_busy_time", -1);
	if(m_iBusyTimeOut < 0){
		Log::log(0,"***告警***：超负荷时间阀值不正确，使用默认值10分钟",m_iShift);
		////ALARMLOG(0,MBC_schedule+21,"超负荷时间阀值不正确，使用默认值10分钟",m_iBusyTimeOut);
		m_iBusyTimeOut = 600;
	}
	
	m_iSysMaxProcCnt = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "max_process_num", -1);
	if(m_iSysMaxProcCnt < 0){
		Log::log(0,"***告警***：系统业务进程最大数不正确，使用默认值200个",m_iShift);
		////ALARMLOG(0,MBC_schedule+22,"系统业务进程最大数不正确，使用默认值400个",m_iSysMaxProcCnt);
		m_iSysMaxProcCnt = 400;
	}
	
	if(m_iMaxProc < m_iMinProc){
		Log::log(0,"***告警***：虚处理器[%d]中最大进程数小于最小进程数，强制改变最小进程数为最大进程数!",m_iVirtualId);
		////ALARMLOG(0,MBC_schedule+23,"虚处理器[%d]中最大进程数小于最小进程数，强制改变最小进程数为最大进程数!",m_iVirtualId);
		m_iMinProc = m_iMaxProc;
		sprintf(sTemp,"SCHEDULE.vp%d.min_process=%d",iTempVirtualId,m_iMinProc);
		Channel::m_pCmdCom->changeSectionKey(sTemp);
	}
	
	memset(sTemp,0,100);
	static char sMidSysState[100] = {0};//有可能是系统中间态
	static char sSysState[100] = {0};
	memset(sMidSysState,0,sizeof(sMidSysState));
	memset(sSysState,0,sizeof(sSysState));
	Channel::m_pCmdCom->readIniString("SYSTEM","mid_state",sMidSysState,NULL);	
	Channel::trim(sMidSysState);
	char *p = strrchr(sMidSysState,'|');//倒着查
	if(p != NULL){
		strncpy(sSysState,p+1,99);
	}
	else{
		strncpy(sSysState,sMidSysState,99);
	}
	p = strchr(sMidSysState,'|');//正着查
	if(p != NULL){
		memset(sTemp,0,sizeof(sTemp));
		sTemp[0] = sMidSysState[0];
		m_iForceStop = atoi(sTemp);//读取强制杀进程的标记
	}
	else{
		m_iForceStop = 0;
	}
	sprintf(sTemp,"vp%d.%s_state",iTempVirtualId,sSysState);
	m_iDestState = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, -1);
	if(m_iDestState < 0){
		//Log::log(0,"***告警***：虚处理器[%d]没有配置该系统状态下的目标态，不做改变",m_iVirtualId);
		////ALARMLOG(0,MBC_schedule+24,"虚处理器[%d]没有配置该系统状态下的目标态，不做改变",m_iVirtualId);
		m_iDestState = m_iVirtualState;
	}

	m_iMaxShiftProc = m_iMaxProc*m_iShift/10;		//档位下进程最大数
	if(m_iMaxShiftProc==0 && m_iShift!=0)				//至少保持1个
		m_iMaxShiftProc = 1;
	if(m_iMaxShiftProc < m_iMinProc)					//改变最小进程数
		m_iMinProc = m_iMaxShiftProc;
	else if(m_iMaxShiftProc > m_iMaxProc)			//封顶值到达最大进程数
		m_iMaxShiftProc = m_iMaxProc;
}
/***********************************************
 *	Function Name	:
 *	Description	:判断是否是数据源头
 *	Input Param	:
 *	Returns		:
 *	complete	:
 ***********************************************/
int VirtualProcessor::isDataSrc()
{
	if(m_iCnt <= 0)
		return 0;
	int iPos = m_ProcInfoList[0].iPos;
	TProcessInfo* pProcInfo=Channel::m_pProcInfoHead+iPos;
	//消息收发程序
	if( pProcInfo->iAppID == 1 )
	{
		return 1;
	} 
	return 0;
}
int VirtualProcessor::isBackUpSrc()
{
	if(m_iCnt <= 0)
		return 0;
	int iPos = m_ProcInfoList[0].iPos;
	TProcessInfo* pProcInfo=Channel::m_pProcInfoHead+iPos;
	// 格式化，采集，文件级校验，记录级校验,入库,送高额,汇总文件产生,结算错单回收,loadparaminfo,tdtrans,tygroup
	if( pProcInfo->iAppID==2 || pProcInfo->iAppID==18 || 
		pProcInfo->iAppID==20 || pProcInfo->iAppID==34 || 
		pProcInfo->iAppID==91 || pProcInfo->iAppID==114 || 
		pProcInfo->iAppID==777777 || pProcInfo->iAppID==30 || 
		pProcInfo->iAppID==300)
	{
		return 1;
	} 
	return 0;
}
int VirtualProcessor::checkVirtualState(){
	
 

	refreshEx();																//重新读取参数配置
	
	if(m_iAutoSchedule <= 0)										//如果不参与自动调度，则跳过
			return 0;
			
	getProcInfoFromGuardMain();									//重新获取进程信息
	
	if(m_iMaxShiftProc > m_iCnt){								//超过了实际可用的进程数,共用配置，不必报警
		m_iMaxShiftProc = m_iCnt;
	}
	
	int iNum = 0;
	int iRet = -1;
	iNum = getBusyProcNumEx()-m_iMaxShiftProc;		//档位的变迁导致运行个数大于配置，需要停进程

	for(int i=0; i<iNum; i++)
	{
		stopOneProcEx();
	}

	//启最小的进程数
	//Log::log(0,"虚处理器[%d]开始检测最小化配置...",m_iVirtualId);
	if(startMinProcEx()<0)
		return -1;

	//再做高低水的变迁
	//Log::log(0,"虚处理器[%d]开始检测相关通道水位...",m_iVirtualId);
	int action = getVirtualActionEx();
	//static Date curTime;
	static time_t sNow;
	int iBlockFlag = 0;
	//Channel::setSystemRunState("normal");//先认为系统状态正常
	m_iBusyFlag = 0;
	switch(action)
	{
		case START:
			if(getBusyProcNumEx() < m_iMaxShiftProc)
			{
				//INFOPOINT(1,"虚处理器[%d]尝试启动一个进程",m_iVirtualId);
				Log::log(0,"虚处理器[%d]尝试启动一个进程",m_iVirtualId);
		 Channel::m_pLog->log(MBC_ACTION_SCHEDULE_HIGH,LOG_LEVEL_INFO,LOG_TYPE_PROC,"触发高水位启动进程");
				iRet = startOneProcEx();
				if(iRet >= 0){
					time(&m_iLastTime);
				}
			}
			else
			{
				
				time(&sNow);
				if(sNow-m_iLastTime > m_iBusyTimeOut){
					//Channel::setSystemRunState("busy"); 
					iBlockFlag = 1; //认为积压
					m_iBusyFlag = 1;//超负荷
					Log::log(0,"虚处理器[%d]超负荷",m_iVirtualId);
					//ALARMLOG(0,MBC_schedule+25,"虚处理器[%d]超负荷",m_iVirtualId);
				}
				
			}
			break;
		case STOP:
			if(getBusyProcNumEx() > m_iMinProc)				
			{
				//INFOPOINT(1,"虚处理器[%d]尝试停止一个进程",m_iVirtualId);
				Log::log(0,"虚处理器[%d]尝试停止一个进程",m_iVirtualId);
				
		  	Channel::m_pLog->log(MBC_ACTION_SCHEDULE_LOW,LOG_LEVEL_INFO,LOG_TYPE_PROC,"触发低水位停止进程");
				iRet = stopOneProcEx();
				if(iRet >= 0){
					time(&m_iLastTime);
				}
			}
			else
			{
				//INFOPOINT(2,"虚处理器[%d]已达最小化配置,不做动作",m_iVirtualId);
			}
			break;
		/*
		case NOACTION_STATE:
			//INFOPOINT(2,"虚处理器[%d]中通道的水位初变，不做动作",m_iVirtualId);
			break;
		case NOACTION_TIME:
			//INFOPOINT(2,"虚处理器[%d]中通道水位状态持续时间未达到阀值，不做动作",m_iVirtualId);
			break;
		case NOACTION_NEXT:
			//INFOPOINT(2,"虚处理器[%d]中通道和后置通道同时高水，不做动作",m_iVirtualId);
			break;
		case NOACTION_NOMAL:
			//INFOPOINT(2,"虚处理器[%d]中通道的水位正常，不做动作",m_iVirtualId);
			break;
		*/
		default:
			break;	
	}
	/* 并发基本用不起来，就不显示超负荷了
	if(m_iCnt > 0){
		int iPos = m_ProcInfoList[0].iPos;
		(Channel::m_pProcInfoHead+iPos)->iBlockFlag = iBlockFlag; //修改积压标识，用第一个进程代表虚处理器吧
	}
	*/
	return 0;
}

/***********************************************
 *	Function Name	:startMinProc
 *	Description	:启用最小化配置修改
 *	Input Param	:
 *	Returns		:
 *	complete	:
 ***********************************************/
int VirtualProcessor::startMinProcEx()
{
	int iRunCnt = getBusyProcNumEx();					//不一定真实的在运行，包括所有预起的进程
	int iNeedProc = m_iMinProc - iRunCnt;			//还需再起的进程个数
	int iRet = 0;
	for(int i=0; i<iNeedProc; i++)
	{
		if(startOneProcEx()<0)
			iRet = -1;
	}
	return iRet;
}


//第一次检测，如果是要起就直接起，要停得先做标记
int VirtualProcessor::firstCheckProc()
{
	
	int state;
	int iRetState = getVirtualState(state);//先得到虚处理器目前的状态
	if(iRetState < -1){
		return -2;
	}
	if(  m_iDestState==MODULE_RUNING  && \
		(state==MODULE_IDLE || state==MODULE_RUNING || state==MODULE_FULL) )//当前状态符合目标态
	{
		return 0;
	}
	if( m_iDestState==MODULE_STOP && \
		(state==MODULE_ABORT || state==MODULE_STOP) )//当前状态符合目标态
	{
		return 0;
	}
	if( m_iDestState == MODULE_MAINTAINANCE)//保持不变的方式
	{
		return 0;
	}
	int iRet = 0;
	switch(m_iDestState)
	{
		case MODULE_RUNING:	//目标态是起的话，可直接起
			iRet = startMinProcEx();
			break;
		case MODULE_STOP:
			if(m_iForceStop>0)								//如果是强制停，则直接修改停状态
			{
				forceStopProc();
			}
			else if(isDataSrc() > 0)
			{									//如果是数据源头，则直接停
				stopAllSrcProc();
				m_iSrcStopCheckFlag = 1;        //源头待检测
			}
			else if(state == MODULE_IDLE) 		//如果无任务，则置可停标记
				m_iCanStop = 0; 								//可以停
			else
				m_iCanStop = 1; 								//不可以停
			break;
		default:
			break;
	}
	return iRet;
}
//停数据源头的所有进程
int VirtualProcessor::stopAllSrcProc(){
	if(isDataSrc() > 0)
	{
		m_iSrcStopCheckFlag = 1;
	}
	else
	{
		m_iSrcStopCheckFlag = 0; //如果不是数据源，直接返回
		return 0;
	}
	int iPos;
	int iPid;
	for(int iIndex=0; iIndex<m_iCnt; iIndex++)
	{
		 
	  iPos = m_ProcInfoList[iIndex].iPos;
		iPid = (Channel::m_pProcInfoHead+iPos)->iSysPID;
		/*
		if ( (Channel::m_pProcInfoHead+iPos)->iState == ST_INIT || \
			(Channel::m_pProcInfoHead+iPos)->iState == ST_RUNNING || \
			(Channel::m_pProcInfoHead+iPos)->iState == ST_WAIT_BOOT )
		{
			(Channel::m_pProcInfoHead+iPos)->iState = ST_WAIT_DOWN; //修改其状态，使GuardMain获知
			m_ProcInfoList[iIndex].iDestState = ST_SLEEP; //指定其目标状态
			m_ProcInfoList[iIndex].iOpFlag = 1; //打上标志 下次检测
			m_iCheckFlag = 1;//整个虚处理器需要检测
			g_RunProcNum--;
		}
		*/
		
			if(iPid > 0 && kill(iPid,0)==0 )
		    kill(iPid,SIGUSR1);
	}
	return 0;
}
int VirtualProcessor::stopLeftProc(){
	if( m_iDestState!=MODULE_STOP )
		return 1;
	if(m_iForceStop>0){
		forceStopProc();
		return 0;
	}
	int iPos;
	for(int iIndex=0; iIndex<m_iCnt; iIndex++)
	{
		int iPos = m_ProcInfoList[iIndex].iPos;
		if ( (Channel::m_pProcInfoHead+iPos)->iState == ST_INIT || \
			(Channel::m_pProcInfoHead+iPos)->iState == ST_RUNNING || \
			(Channel::m_pProcInfoHead+iPos)->iState == ST_WAIT_BOOT )
		{
			(Channel::m_pProcInfoHead+iPos)->iState = ST_WAIT_DOWN; //修改其状态，使GuardMain获知
			m_ProcInfoList[iIndex].iDestState = ST_SLEEP; //指定其目标状态
			m_ProcInfoList[iIndex].iOpFlag = 1; //打上标志 下次检测
			m_iCheckFlag = 1;
			g_RunProcNum--; 
		}
	}
	return 0;
}
int VirtualProcessor::getAllProcDetailNum(int& boot,int& init,int& run,int& kill,int& destroy,int& abort,int& sleep)
{
	boot = 0;
	init = 0;
	run = 0;
	kill = 0;
	destroy = 0;
	sleep = 0;
	abort = 0;
	int iPos;
	TProcessInfo* pProcInfo = NULL;
	for(int i=0; i<m_iCnt; i++)
	{
		int iPos = m_ProcInfoList[i].iPos;
		pProcInfo = Channel::m_pProcInfoHead+iPos;
		switch(pProcInfo->iState)
		{
			case ST_INIT:
				init++;
				break;
			case ST_WAIT_BOOT:
				boot++;
				break;
			case ST_RUNNING:
				run++;
				break;
			case ST_SLEEP:
				sleep++;
				break;
			case ST_KILLED:
				kill++;
				break;
			case ST_ABORT:
				abort++;
				break;
			case ST_DESTROY:
				destroy++;
				break;
			default:
				break;
		}
	}
	return init+boot+run+sleep+kill+destroy+abort;
}
int VirtualProcessor::getVirtualState(int& state)
{
	int boot = 0;
	int init = 0;
	int run = 0;
	int kill = 0;
	int destroy = 0;
	int sleep = 0;
	int abort = 0;
	
	int iProcNum = getAllProcDetailNum(boot,init,run,kill,destroy,abort,sleep);
	
	int iBusyNum = boot + init + destroy + run;
	int iIdleNum = kill + sleep  + abort;
	
	int iState = Channel::calcWater(m_iVirtualId);
	if(iState < 0)
	{
		return -1;
	}
	if( (abort+kill) == m_iCnt) //如果所有的进程都有问题，可返回
	{
		m_iVirtualState = MODULE_ABORT;
		state = m_iVirtualState;
		return iState;
	}
	if(sleep == m_iCnt)
	{
		m_iVirtualState = MODULE_STOP;
		state = m_iVirtualState;
		return iState;
	}
	if( (iState==CHANNEL_EMPTY) && (iBusyNum>0) )//通道无任务，且有进程在运行
	{
		m_iVirtualState = MODULE_IDLE;
		state = m_iVirtualState;
		return iState;
	}
	if((run==m_iMaxShiftProc) && iBusyNum>0) //达到档位下最大个数，繁忙
	{
		m_iVirtualState = MODULE_FULL;
		state = m_iVirtualState;
		return iState;
	}
	if(iBusyNum > 0 && abort==0 && kill==0)//
	{
		m_iVirtualState = MODULE_RUNING;
		state = m_iVirtualState;
		return iState;
	}
	if(iBusyNum>0 && (abort>0 || kill>0))
	{
		m_iVirtualState = MODULE_PROC_RUN_ABORT;
		state = m_iVirtualState;
		return iState;
	}
	if(iBusyNum==0 && (abort>0 || kill>0) )
	{
		m_iVirtualState = MODULE_PROC_STOP_ABORT;
		state = m_iVirtualState;
		return iState;
	}
}
int VirtualProcessor::checkTask()
{
	if(m_iForceStop>0 || m_iDestState==MODULE_IDLE || m_iDestState==MODULE_RUNING || m_iDestState==MODULE_FULL || m_iDestState==MODULE_MAINTAINANCE)
	{
		m_iCanStop  = 0;				//如果是起的，或者保持不变，或者说是强杀，可以打上“可以停”的标记
		return 0;
	}
	int iState;
	getVirtualState(iState); //获取虚处理器的状态
	if(iState==MODULE_IDLE)
	{
		m_iCanStop  = 0;						//虚处理器空闲，可以停
	}
	else if(iState==MODULE_ABORT || iState==MODULE_STOP || iState==MODULE_PROC_STOP_ABORT )
	{
		//m_iCanStop  = 0;						//虽然有任务，但是虚处理器已经挂了，应该启动虚处理器，使其把任务打通，否则上游可能会堵住
		if(isDataSrc() > 0)
		{ //对于数据源头，有任务也没关系，可以直接停
			m_iCanStop = 0; //这种情况可以算停了
			return 0;
		}
		int iTaskNum = Channel::calcWater(m_iVirtualId); //计算一下虚处理器的水位
		if(iTaskNum != 0){ //有任务需要处理
			m_iCanStop = 1; //不可以停
			//Log::log(0,"虚处理器[%d]强制启动进程处理残留数据...",m_iVirtualId); 
			if(startOneProcEx()<0) //启动虚处理器把任务打通,如果启动失败，没办法了,只好认为可以停了，如果导致前面进程堵住，只好人工处理了
				m_iCanStop = 0; 
		  checkProcState(); //这个是发现的BUG，就是为了使进程操作标识能够做清理
		}
		else{
			m_iCanStop = 0; //没有任务需要处理，可以停了
		}
	}
	else
	{
		m_iCanStop = 1;							//不能停，要等任务处理完成
	}
	return 0;
}
int VirtualProcessor::checkBackUpTask()
{
	if(isBackUpSrc() > 0){
		m_iCanStop = 0;
		return 0;
	}
	if(isDataSrc() > 0){
		m_iCanStop = 0;
		return 0;
	}
	int iState;
	getVirtualState(iState); //获取虚处理器的状态
	if(iState==MODULE_IDLE){
		m_iCanStop  = 0;						//虚处理器空闲，可以停
	}
	else if(iState==MODULE_ABORT || iState==MODULE_STOP || iState==MODULE_PROC_STOP_ABORT ){
		//m_iCanStop  = 0;						//虽然有任务，但是虚处理器已经挂了，应该启动虚处理器，使其把任务打通，否则上游可能会堵住
		if(isDataSrc() > 0){ //对于数据源头，有任务也没关系，可以直接停
			m_iCanStop = 0; //这种情况可以算停了
			return 0;
		}
		int iTaskNum = Channel::calcWater(m_iVirtualId); //计算一下虚处理器的水位
		if(iTaskNum != 0){ //有任务需要处理
			m_iCanStop = 1; //不可以停
			Log::log(0,"虚处理器[%d]强制启动进程处理残留数据...",m_iVirtualId); 
			if(startOneProcEx()<0) //启动虚处理器把任务打通,如果启动失败，没办法了,只好认为可以停了，如果导致前面进程堵住，只好人工处理了
				m_iCanStop = 0; 
			checkProcState(); //这个是发现的BUG，就是为了使进程操作标识能够做清理
		}
		else{
			m_iCanStop = 0; //没有任务需要处理，可以停了
		}
	}
	else{
		m_iCanStop = 1;							//不能停，要等任务处理完成
		Log::log(0,"需处理器%d还有水",m_iVirtualId);
	}
	return 0;
}
// 强制停止虚处理器的所有进程
int VirtualProcessor::forceStopProc()
{
	int iPos;
	TProcessInfo* pProcInfo = NULL;
	for(int iIndex=0; iIndex<m_iCnt; iIndex++)
	{
		int iPos = m_ProcInfoList[iIndex].iPos;
		pProcInfo = Channel::m_pProcInfoHead+iPos;
			/*
		if ( kill(pProcInfo->iSysPID,0)==0)
		{
			kill(pProcInfo->iSysPID,9);
		}
		*/
		pProcInfo->iState = ST_WAIT_DOWN;
		g_RunProcNum--; //运行个数减1
	}
	return 0;
}
//查看是否有异常的进程
int VirtualProcessor::isErrExist(){
	int iPos;
	TProcessInfo* pProcInfo = NULL;
	for(int iIndex=0; iIndex<m_iCnt; iIndex++)
	{
		int iPos = m_ProcInfoList[iIndex].iPos;
		pProcInfo = Channel::m_pProcInfoHead+iPos;
		if( ST_KILLED==pProcInfo->iState || ST_ABORT==pProcInfo->iState){ 
			return pProcInfo->iProcessID;
		}
	}
	return 0;
}
