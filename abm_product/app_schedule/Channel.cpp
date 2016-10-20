#include "StdAfx.h"

CommandCom* Channel::m_pCmdCom=0;
THeadInfo*	Channel::m_pInfoHead = 0;
TProcessInfo*	Channel::m_pProcInfoHead = 0;
ThreeLogGroup* Channel::m_pLog = 0;
char Channel::m_sSysRunState[MAX_STATE_LEN];
	
Channel::Channel()
{
}
Channel::~Channel()
{
}

//连接共享内存
int Channel::connectGuardMain()
{
	DetachShm ();
	if(AttachShm (0, (void **)&m_pInfoHead) >= 0)		//连接GuardMain
	{
		m_pProcInfoHead = &m_pInfoHead->ProcInfo;			//得到进程信息首地址
	}
	else 
	{
		//Log::log(0,"运行时告警：连接共享内存失败，GuardMain可能不存在\n");
		//ALARMLOG(0,MBC_schedule+24,"%s","连接共享内存失败，GuardMain可能不存在");
		return -1;
	}
	return 0;
}

//计算通道的状态
int Channel::calcWater(int iVpID)
{
	//找到通道任务数，下游最大任务数，下游VPID
	if(iVpID < 0)
		return 0;
	int iMessageNum = 0;
	int iPercent = 0;
	for (int i=0; i<m_pInfoHead->iProcNum; i++)			//遍历所有进程
	{
		if(iVpID == (m_pProcInfoHead+i)->iVpID)		//查找
		{
			if((m_pProcInfoHead+i)->iMqID > 0)
			{
				 MessageQueue mq( (m_pProcInfoHead+i)->iMqID);
         if(mq.exist())
         {
				  iMessageNum = mq.getMessageNumber();
				 }
				else
				  iMessageNum = 0;
			}
			else
			{
				iMessageNum = 0;
			}
			break;
		}
	}
	
	int iLowLevel = 0;
	int iHighLevel = 0;
	int iQueueSize = 0;
	//从核心参数中读取本通道的配置
	int iState;
	char sTemp[100] = {0};
	memset(sTemp,0,100);
	
	int iTemp = iVpID/1000;
	
	sprintf(sTemp,"vp%d.process_high_level",iTemp);
	iHighLevel = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, 100);//高水值
	
	memset(sTemp,0,100);
	sprintf(sTemp,"vp%d.process_low_level",iTemp);
	iLowLevel = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, 0); //低水值
		
		
	memset(sTemp,0,100);
	sprintf(sTemp,"vp%d.queue_size",iTemp);
	iQueueSize = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", sTemp, 0); //队列长度
	//int iRet = CalcState( iVpID, iTaskNum);
	if(iQueueSize > 0)
		iPercent = iMessageNum*100/iQueueSize;
	
	if(iPercent >= iHighLevel)
		iState = CHANNEL_HIGH;
		
	else if(iPercent <= iLowLevel)
		iState = CHANNEL_LOW;
	else
		iState = CHANNEL_NOMAL;
		
	if(iPercent == 0)
		iState = CHANNEL_EMPTY;
	
	if(iPercent >= 100)
	{
		iPercent = 100;
		iState = CHANNEL_FULL;
	}
	//根据拥塞情况启停指定模块
	int iBlockLevel = 0;
	int iScheduleBlockVP = 0;
	iBlockLevel = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "schedule_block_level", 0); //拥塞级别
	iScheduleBlockVP = Channel::m_pCmdCom->readIniInteger ("SCHEDULE", "schedule_block_vp", 0); //拥塞相关vp
	if( iScheduleBlockVP == iVpID / 1000 )
	{
		if (iBlockLevel > 0)
		{
			iState = CHANNEL_HIGH;
		}
	}
	return iState;
}

//Trim函数
char* Channel::trim(char* sp)
{
	char sDest[1024];
	char *pStr;
	int i = 0;

	if ( sp == NULL )
		return NULL;

	pStr = sp;
	while ( *pStr == ' ' || *pStr == '\t' )  pStr ++;
	strcpy( sDest, pStr );

	i = strlen( sDest ) - 1;
	while((i>=0)&&(sDest[i] == ' ' || sDest[i] == '\t' || sDest[i] == '\r' ) )
	{
		sDest[i] = '\0';
		i -- ;
	}

	strcpy( sp, sDest );

	return ( sp );

}
