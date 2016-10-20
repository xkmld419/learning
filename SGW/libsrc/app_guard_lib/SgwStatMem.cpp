#include "SgwStatMem.h"

#include "mntapi.h"
#include "IpcKey.h"
#include "Date.h"
//
//#include "CommonMacro.h"

struct S_StatAllInfo *StatAllInfoMgr::pStatAllInfo = 0;
TProcessInfo *StatAllInfoMgr::pProcInfoHead = 0;
THeadInfo *StatAllInfoMgr::pInfoHead = 0;
long StatAllInfoMgr::m_lKey = 0;
long StatAllInfoMgr::m_lLockKey = 0;
CSemaphore *StatAllInfoMgr::pSem = 0;
CommandCom *StatAllInfoMgr::m_pCmdCom = 0;

StatAllInfoMgr::StatAllInfoMgr()
{

}

StatAllInfoMgr::~StatAllInfoMgr()
{
	if(StatAllInfoMgr::pSem)
		delete pSem;
	if (m_pCmdCom)
	{
		m_pCmdCom->Logout();
		delete m_pCmdCom;
	}
}

bool StatAllInfoMgr::ConnectGuardMain()
{
	DetachShm ();
	if (AttachShm (0, (void **)&pInfoHead) >= 0) 
	{
		pProcInfoHead = &pInfoHead->ProcInfo;
	}
	else 
	{
		return false;
	}
	return true;
}

bool StatAllInfoMgr::GetStatMemKey()
{
	m_lKey = IpcKeyMgr::getIpcKey(-1,"SHM_StatMemKey");
	if (m_lKey >=0 )
		return true;

	//Log::log (0,"获取共享内存键出错(SHM_StatMemKey)");

	return false;
}

bool StatAllInfoMgr::GetStatLock()
{
	if(pSem)
		return true;
	m_lLockKey = IpcKeyMgr::getIpcKey(-1,"SHM_StatMemLockKey");
	char sKey[32] = {0};
	if ( m_lLockKey>0)
	{
		sprintf(sKey,"%ld",m_lLockKey);
		pSem = new CSemaphore();
		if(!pSem->getSem(sKey))
		{
			delete pSem;
			pSem = 0;
			return false;			
		}
		return true;
	}

	//Log::log (0,"获取共享内存键出错(SHM_StatMemLockKey)");
	return false;
}

bool StatAllInfoMgr::AttachStatShm()
{
	long lSize = sizeof(S_StatAllInfo);
	if (!m_lKey) {
		if (!GetStatMemKey ())
			return false;
	}

	if (pStatAllInfo)
		DetachStatShm ();

	int shm_id = shmget (m_lKey, lSize, 0660);
	if (shm_id < 0) {
		return false;
	}

	pStatAllInfo = (S_StatAllInfo*)shmat (shm_id, 0, 0);
	if (pStatAllInfo == NULL) {
		return false;
	}

	//*pRet = (void *)pStatAllInfo;
	ConnectGuardMain();
	if (!m_pCmdCom)
	{
		m_pCmdCom = new CommandCom();
		m_pCmdCom->InitClient();
	}	
	return true;
}

bool StatAllInfoMgr::DetachStatShm()
{
	shmdt ((char *)pStatAllInfo);
	pStatAllInfo = NULL;
	return true;
}

bool StatAllInfoMgr::CreateStatShm()
{
	if(AttachStatShm())
		return true;
	long lSize = sizeof(S_StatAllInfo);
	if (!m_lKey) {
		if (!GetStatMemKey ()){
			//ALARMLOG28(0,MBC_CLASS_Memory,119,"%s","创建共享内存失败。创建进程管理共享内存失败。");
			return false;
		}
	}
	if(!pSem)
		if(!GetStatLock())
			return false;

	int shm_id = shmget (m_lKey, lSize, 0660|IPC_EXCL|IPC_CREAT);
	if (shm_id < 0) {
		//ALARMLOG28(0,MBC_CLASS_Memory,119,"%s","创建共享内存失败。创建进程管理共享内存失败。");
		return false;
	}

	pStatAllInfo = (S_StatAllInfo*)shmat (shm_id, 0, 0);
	if (pStatAllInfo == NULL) {
		//ALARMLOG28(0,MBC_CLASS_Memory,119,"%s","创建共享内存失败。创建进程管理共享内存失败。");
		return false;
	}
	return true;

}

bool StatAllInfoMgr::DestroyStatShm()
{
	if (!m_lKey) {
		if (!GetStatMemKey ())
			return false;
	}

	int shm_id = shmget (m_lKey, 0, 0660);
	if (shm_id < 0) {
		return false;
	}

	shmctl (shm_id, IPC_RMID, 0);
	return true;
}

bool StatAllInfoMgr::ClearStatShm()
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	memset(pStatAllInfo,0,sizeof(S_StatAllInfo));
	pSem->V();
	return true;
}


void StatAllInfoMgr::GetDayAndHour(char* sDatetime,int& iDay,int& iHour)
{
	char sTime[16] = {0};
	Date dTime;
	if (sDatetime == NULL)
	{
		dTime.getTimeString(sTime);
	}
	else
	{
		strcpy(sTime,sDatetime);
	}
	iDay = -1;
	iHour = -1;
	char tmp[8] = {0};
	strncpy(tmp,sTime +6,4);
	iDay = atoi(tmp);
	iHour = iDay%100;
	iDay = iDay/100;
}

bool StatAllInfoMgr::setBalancePer(int iPer)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	pStatAllInfo->iBalancePer = iPer;
	pSem->V();
}

//网元处理情况接口
bool StatAllInfoMgr::addNetInfoCount( long lNetInfoID,char *sNetInfoName, char *sServiceName,int iReqNumAdd/*增量*/,bool bReqSucc, int iReqDataAdd/*增量*/,int iCallServiceAdd,bool bCallSucc,char* sDatetime )
{
	int iDay = 0;
	int iHour = 0;
	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_NetInfoCount_Buff *pNetInfo = 0;
	pNetInfo = pStatAllInfo->aNetInfoCount;
	pNetInfo += ON_TODAY - 1; //按天
	int i = 0;
	if(pNetInfo->iDay!=iDay)//换天
	{
		memset(pNetInfo->aNetInfoCount_,0,sizeof(S_NetInfoCount)*MAX_STAT_NETINFO_COUNT);
		pNetInfo->iDay = iDay;
		pNetInfo->iTotal = 1;
		pNetInfo->aNetInfoCount_[i].lNetInfoID = lNetInfoID;
		pNetInfo->aNetInfoCount_[i].lReqData = iReqDataAdd;
		pNetInfo->aNetInfoCount_[i].lCallServiceNum = iCallServiceAdd;
		pNetInfo->aNetInfoCount_[i].lReqNum = iReqNumAdd;
		strcpy(pNetInfo->aNetInfoCount_[i].sNetInfoName,sNetInfoName);
		strcpy(pNetInfo->aNetInfoCount_[i].sServiceName,sServiceName);
		if(bCallSucc)
		{
			pNetInfo->aNetInfoCount_[i].lCallServiceSucc = iCallServiceAdd;
		}
		if (bReqSucc)
		{
			pNetInfo->aNetInfoCount_[i].lReqSuccNum = iReqNumAdd;
		}

	}
	else
	{
		i = -1;
		for (int n=0;n<pNetInfo->iTotal;n++)
		{
			if(strcmp(pNetInfo->aNetInfoCount_[n].sServiceName, sServiceName) == 0&&
				pNetInfo->aNetInfoCount_[n].lNetInfoID == lNetInfoID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pNetInfo->iTotal++;
		}
		pNetInfo->aNetInfoCount_[i].lNetInfoID = lNetInfoID;
		strcpy(pNetInfo->aNetInfoCount_[i].sNetInfoName,sNetInfoName);
		strcpy(pNetInfo->aNetInfoCount_[i].sServiceName,sServiceName);
		pNetInfo->aNetInfoCount_[i].lReqData += iReqDataAdd;
		pNetInfo->aNetInfoCount_[i].lCallServiceNum += iCallServiceAdd;
		pNetInfo->aNetInfoCount_[i].lReqNum += iReqNumAdd;
		if(bCallSucc)
		{
			pNetInfo->aNetInfoCount_[i].lCallServiceSucc += iCallServiceAdd;
		}
		if (bReqSucc)
		{
			pNetInfo->aNetInfoCount_[i].lReqSuccNum += iReqNumAdd;
		}

	}

	pNetInfo = pStatAllInfo->aNetInfoCount;
	pNetInfo += ON_HOUR - 1; //按小时
	i = 0;
	if(pNetInfo->iHour!=iHour)//换小时
	{
		memset(pNetInfo->aNetInfoCount_,0,sizeof(S_NetInfoCount)*MAX_STAT_NETINFO_COUNT);
		pNetInfo->iHour = iHour;
		pNetInfo->iTotal = 1;
		pNetInfo->aNetInfoCount_[i].lNetInfoID = lNetInfoID;
		pNetInfo->aNetInfoCount_[i].lReqData = iReqDataAdd;
		pNetInfo->aNetInfoCount_[i].lCallServiceNum = iCallServiceAdd;
		pNetInfo->aNetInfoCount_[i].lReqNum = iReqNumAdd;
		strcpy(pNetInfo->aNetInfoCount_[i].sNetInfoName,sNetInfoName);
		strcpy(pNetInfo->aNetInfoCount_[i].sServiceName,sServiceName);
		if(bCallSucc)
		{
			pNetInfo->aNetInfoCount_[i].lCallServiceSucc = iCallServiceAdd;
		}
		if (bReqSucc)
		{
			pNetInfo->aNetInfoCount_[i].lReqSuccNum = iReqNumAdd;
		}

	}
	else
	{
		i = -1;
		for (int n=0;n<pNetInfo->iTotal;n++)
		{
			if(strcmp(pNetInfo->aNetInfoCount_[n].sServiceName, sServiceName) == 0&&
				pNetInfo->aNetInfoCount_[n].lNetInfoID == lNetInfoID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pNetInfo->iTotal++;
		}
		pNetInfo->aNetInfoCount_[i].lNetInfoID = lNetInfoID;
		strcpy(pNetInfo->aNetInfoCount_[i].sNetInfoName,sNetInfoName);
		strcpy(pNetInfo->aNetInfoCount_[i].sServiceName,sServiceName);
		pNetInfo->aNetInfoCount_[i].lReqData += iReqDataAdd;
		pNetInfo->aNetInfoCount_[i].lCallServiceNum += iCallServiceAdd;
		pNetInfo->aNetInfoCount_[i].lReqNum += iReqNumAdd;
		if(bCallSucc)
		{
			pNetInfo->aNetInfoCount_[i].lCallServiceSucc += iCallServiceAdd;
		}
		if (bReqSucc)
		{
			pNetInfo->aNetInfoCount_[i].lReqSuccNum += iReqNumAdd;
		}
	}
	pNetInfo = pStatAllInfo->aNetInfoCount;
	pNetInfo += ON_ONLINE - 1; //启动以来
	i = -1;
	for (int n=0;n<pNetInfo->iTotal;n++)
	{
		if(strcmp(pNetInfo->aNetInfoCount_[n].sServiceName, sServiceName) == 0&&
			pNetInfo->aNetInfoCount_[n].lNetInfoID == lNetInfoID)
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pNetInfo->iTotal++;
	}
	pNetInfo->aNetInfoCount_[i].lNetInfoID = lNetInfoID;
	strcpy(pNetInfo->aNetInfoCount_[i].sNetInfoName,sNetInfoName);
	strcpy(pNetInfo->aNetInfoCount_[i].sServiceName,sServiceName);
	pNetInfo->aNetInfoCount_[i].lReqData += iReqDataAdd;
	pNetInfo->aNetInfoCount_[i].lCallServiceNum += iCallServiceAdd;
	pNetInfo->aNetInfoCount_[i].lReqNum += iReqNumAdd;
	if(bCallSucc)
	{
		pNetInfo->aNetInfoCount_[i].lCallServiceSucc += iCallServiceAdd;
	}
	if (bReqSucc)
	{
		pNetInfo->aNetInfoCount_[i].lReqSuccNum += iReqNumAdd;
	}

	pSem->V();
	return true;
}

bool StatAllInfoMgr::addServiceCount( char *sServiceName,int iPLevel,int iNum,bool bSucc,char* sDatetime )
{
	int iDay = 0;
	int iHour = 0;
	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_ServiceCount_Buff *pService = 0;
	pService = pStatAllInfo->aServiceCount;
	pService += ON_TODAY -1;//天
	int i = 0;
	if(pService->iDay != iDay)
	{
		memset(pService->aServiceCount_,0,sizeof(S_ServiceCount)*MAX_STAT_SERVICE_COUNT);
		pService->iDay = iDay;
		pService->iTotal = 1;
		strcpy(pService->aServiceCount_[i].sServiceName,sServiceName);
		pService->aServiceCount_[i].iPLevel = iPLevel;
		pService->aServiceCount_[i].lTotal = iNum;
		if( bSucc )
			pService->aServiceCount_[i].lSuccNum = iNum;
	}
	else
	{
		i = -1;
		for(int n=0;n<pService->iTotal;n++)
		{
			if(strcmp(pService->aServiceCount_[n].sServiceName, sServiceName) == 0&&
				pService->aServiceCount_[n].iPLevel == iPLevel)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pService->iTotal++;
		}
		strcpy(pService->aServiceCount_[i].sServiceName,sServiceName);
		pService->aServiceCount_[i].iPLevel = iPLevel;
		pService->aServiceCount_[i].lTotal += iNum;
		if( bSucc )
			pService->aServiceCount_[i].lSuccNum += iNum;
	}

	pService = pStatAllInfo->aServiceCount;
	pService += ON_HOUR -1;//时
	i = 0;
	if(pService->iHour != iHour)
	{
		memset(pService->aServiceCount_,0,sizeof(S_ServiceCount)*MAX_STAT_SERVICE_COUNT);
		pService->iHour = iHour;
		pService->iTotal = 1;
		strcpy(pService->aServiceCount_[i].sServiceName,sServiceName);
		pService->aServiceCount_[i].iPLevel = iPLevel;
		pService->aServiceCount_[i].lTotal = iNum;
		if( bSucc )
			pService->aServiceCount_[i].lSuccNum = iNum;
	}
	else
	{
		i = -1;
		for(int n=0;n<pService->iTotal;n++)
		{
			if(strcmp(pService->aServiceCount_[n].sServiceName, sServiceName) == 0&&
				pService->aServiceCount_[n].iPLevel == iPLevel)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pService->iTotal++;
		}
		strcpy(pService->aServiceCount_[i].sServiceName,sServiceName);
		pService->aServiceCount_[i].iPLevel = iPLevel;
		pService->aServiceCount_[i].lTotal += iNum;
		if( bSucc )
			pService->aServiceCount_[i].lSuccNum += iNum;
	}

	pService = pStatAllInfo->aServiceCount;
	pService += ON_ONLINE -1;//时

	i = -1;
	for(int n=0;n<pService->iTotal;n++)
	{
		if(strcmp(pService->aServiceCount_[n].sServiceName, sServiceName) == 0&&
			pService->aServiceCount_[n].iPLevel == iPLevel)
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pService->iTotal++;
	}
	strcpy(pService->aServiceCount_[i].sServiceName,sServiceName);
	pService->aServiceCount_[i].iPLevel = iPLevel;
	pService->aServiceCount_[i].lTotal += iNum;
	if( bSucc )
		pService->aServiceCount_[i].lSuccNum += iNum;

	pSem->V();
	return true;
}

//外部网元情况接口
bool StatAllInfoMgr::addExtNetInfo( long lExtNetInfoID,char* sNetInfoName,long lUserID, char* sUserName, char* sServiceName,int iNum,bool bSucc,char* sDatetime )
{
	int iDay = 0;
	int iHour = 0;
	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_ExtNetInfo_Buff *pExtNetInfo = 0;
	pExtNetInfo = pStatAllInfo->aExtNetInfo;
	pExtNetInfo += ON_TODAY - 1; //按天
	int i = 0;
	if( pExtNetInfo->iDay != iDay )//换天
	{
		memset(pExtNetInfo->aExtNetInfo_,0,sizeof(S_ExtNetInfo)*MAX_STAT_NETINFO_COUNT);
		pExtNetInfo->iDay = iDay;
		pExtNetInfo->iTotal = 1;
		pExtNetInfo->aExtNetInfo_[i].lExtNetInfoID = lExtNetInfoID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sNetInfoName,sNetInfoName);
		pExtNetInfo->aExtNetInfo_[i].lUserID = lUserID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sUserName,sUserName);
		strcpy(pExtNetInfo->aExtNetInfo_[i].sServiceName,sServiceName);
		pExtNetInfo->aExtNetInfo_[i].lToatl = iNum;
		if (bSucc)
		{
			pExtNetInfo->aExtNetInfo_[i].lSuccNum = iNum;
		}
	}
	else
	{
		i = -1;
		for (int n=0;n<pExtNetInfo->iTotal;n++)
		{
			if(strcmp(pExtNetInfo->aExtNetInfo_[n].sServiceName,sServiceName) == 0&&
				pExtNetInfo->aExtNetInfo_[n].lExtNetInfoID == lExtNetInfoID&&
				pExtNetInfo->aExtNetInfo_[n].lUserID  == lUserID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pExtNetInfo->iTotal++;
		}
		pExtNetInfo->aExtNetInfo_[i].lExtNetInfoID = lExtNetInfoID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sNetInfoName,sNetInfoName);
		pExtNetInfo->aExtNetInfo_[i].lUserID = lUserID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sUserName,sUserName);
		strcpy(pExtNetInfo->aExtNetInfo_[i].sServiceName,sServiceName);
		pExtNetInfo->aExtNetInfo_[i].lToatl += iNum;
		if (bSucc)
		{
			pExtNetInfo->aExtNetInfo_[i].lSuccNum += iNum;
		}
	}
	pExtNetInfo = pStatAllInfo->aExtNetInfo;
	pExtNetInfo += ON_HOUR - 1; //按小时
	i = 0;
	if( pExtNetInfo->iHour != iHour )//按小时
	{
		memset(pExtNetInfo->aExtNetInfo_,0,sizeof(S_ExtNetInfo)*MAX_STAT_NETINFO_COUNT);
		pExtNetInfo->iHour = iHour;
		pExtNetInfo->iTotal = 1;
		pExtNetInfo->aExtNetInfo_[i].lExtNetInfoID = lExtNetInfoID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sNetInfoName,sNetInfoName);
		pExtNetInfo->aExtNetInfo_[i].lUserID = lUserID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sUserName,sUserName);
		strcpy(pExtNetInfo->aExtNetInfo_[i].sServiceName,sServiceName);
		pExtNetInfo->aExtNetInfo_[i].lToatl = iNum;
		if (bSucc)
		{
			pExtNetInfo->aExtNetInfo_[i].lSuccNum = iNum;
		}
	}
	else
	{
		i = -1;
		for (int n=0;n<pExtNetInfo->iTotal;n++)
		{
			if(strcmp(pExtNetInfo->aExtNetInfo_[n].sServiceName,sServiceName) == 0&&
				pExtNetInfo->aExtNetInfo_[n].lExtNetInfoID == lExtNetInfoID&&
				pExtNetInfo->aExtNetInfo_[n].lUserID  == lUserID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pExtNetInfo->iTotal++;
		}
		pExtNetInfo->aExtNetInfo_[i].lExtNetInfoID = lExtNetInfoID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sNetInfoName,sNetInfoName);
		pExtNetInfo->aExtNetInfo_[i].lUserID = lUserID;
		strcpy(pExtNetInfo->aExtNetInfo_[i].sUserName,sUserName);
		strcpy(pExtNetInfo->aExtNetInfo_[i].sServiceName,sServiceName);
		pExtNetInfo->aExtNetInfo_[i].lToatl += iNum;
		if (bSucc)
		{
			pExtNetInfo->aExtNetInfo_[i].lSuccNum += iNum;
		}
	}

	pExtNetInfo = pStatAllInfo->aExtNetInfo;
	pExtNetInfo += ON_ONLINE - 1; //启动
	i = -1;
	for (int n=0;n<pExtNetInfo->iTotal;n++)
	{
		if(strcmp(pExtNetInfo->aExtNetInfo_[n].sServiceName,sServiceName) == 0&&
			pExtNetInfo->aExtNetInfo_[n].lExtNetInfoID == lExtNetInfoID&&
			pExtNetInfo->aExtNetInfo_[n].lUserID  == lUserID)
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pExtNetInfo->iTotal++;
	}
	pExtNetInfo->aExtNetInfo_[i].lExtNetInfoID = lExtNetInfoID;
	strcpy(pExtNetInfo->aExtNetInfo_[i].sNetInfoName,sNetInfoName);
	pExtNetInfo->aExtNetInfo_[i].lUserID = lUserID;
	strcpy(pExtNetInfo->aExtNetInfo_[i].sUserName,sUserName);
	strcpy(pExtNetInfo->aExtNetInfo_[i].sServiceName,sServiceName);
	pExtNetInfo->aExtNetInfo_[i].lToatl += iNum;
	if (bSucc)
	{
		pExtNetInfo->aExtNetInfo_[i].lSuccNum += iNum;
	}

	pSem->V();
	return true;
}


//统计信息
bool StatAllInfoMgr::addAllInfo(int iReqNum,int iTimeOutNum,int iSuccNum,int iFaultNum,
					   int iAuthNum,int iCodeFaultNum,int iTranNum,int iMarkCheckNum,int iSRFaultNum,
					   int iOtherNum,char* sDatetime)
{
	int iDay = 0;
	int iHour = 0;
	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_AllCount_Buff *pAllCountInfo = 0;
	pAllCountInfo = pStatAllInfo->aAllCount;
	pAllCountInfo += ON_TODAY - 1; //按天	
	if(pAllCountInfo->iDay != iDay)
	{
		memset(&pAllCountInfo->aAllCount_,0,sizeof(S_AllCount));
		pAllCountInfo->iDay = iDay;
	}
	pAllCountInfo->aAllCount_.lAuthNum += iAuthNum;
	pAllCountInfo->aAllCount_.lCodeFaultNum += iCodeFaultNum;
	pAllCountInfo->aAllCount_.lFaultNum += iFaultNum;
	pAllCountInfo->aAllCount_.lMarkCheckNum += iMarkCheckNum;
	pAllCountInfo->aAllCount_.lReqNum += iReqNum;
	pAllCountInfo->aAllCount_.lSRFaultNum += iSRFaultNum;
	pAllCountInfo->aAllCount_.lSuccNum += iSuccNum;
	pAllCountInfo->aAllCount_.lOtherNum += iOtherNum;
	
	pAllCountInfo = pStatAllInfo->aAllCount;
	pAllCountInfo += ON_HOUR - 1; //按小时	
	if(pAllCountInfo->iHour != iHour)
	{
		memset(&pAllCountInfo->aAllCount_,0,sizeof(S_AllCount));
		pAllCountInfo->iHour = iHour;
	}
	pAllCountInfo->aAllCount_.lAuthNum += iAuthNum;
	pAllCountInfo->aAllCount_.lCodeFaultNum += iCodeFaultNum;
	pAllCountInfo->aAllCount_.lFaultNum += iFaultNum;
	pAllCountInfo->aAllCount_.lMarkCheckNum += iMarkCheckNum;
	pAllCountInfo->aAllCount_.lReqNum += iReqNum;
	pAllCountInfo->aAllCount_.lSRFaultNum += iSRFaultNum;
	pAllCountInfo->aAllCount_.lSuccNum += iSuccNum;
	pAllCountInfo->aAllCount_.lOtherNum += iOtherNum;

	pAllCountInfo = pStatAllInfo->aAllCount;
	pAllCountInfo += ON_ONLINE - 1; //启动
	pAllCountInfo->aAllCount_.lAuthNum += iAuthNum;
	pAllCountInfo->aAllCount_.lCodeFaultNum += iCodeFaultNum;
	pAllCountInfo->aAllCount_.lFaultNum += iFaultNum;
	pAllCountInfo->aAllCount_.lMarkCheckNum += iMarkCheckNum;
	pAllCountInfo->aAllCount_.lReqNum += iReqNum;
	pAllCountInfo->aAllCount_.lSRFaultNum += iSRFaultNum;
	pAllCountInfo->aAllCount_.lSuccNum += iSuccNum;
	pAllCountInfo->aAllCount_.lOtherNum += iOtherNum;

	pSem->V();
	return true;
}

//话单统计
bool StatAllInfoMgr::addTicket(int iProcessID,int iType/*0:Normal;1:Error*/,int iNum,char* sDatetime)
{
	int iDay = 0;
	int iHour = 0;

	if(iProcessID<=0)
		return false;


	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	if (!pProcInfoHead)
	{
		return false;
	}
	pSem->P();
	for(int i=0;i<pInfoHead->iProcNum;i++)
	{
		if(pProcInfoHead[i].iProcessID == iProcessID)
		{
			pProcInfoHead[i].iAllTickets += iNum;
			if(iType == 0)
				pProcInfoHead[i].iNormalTickets += iNum;
			else
				pProcInfoHead[i].iErrTickets += iNum;
			break;
		}
	}
	
	struct S_ProcessInfo_Buff *pStatProcInfo = 0;
	pStatProcInfo = pStatAllInfo->aProcessInfo;
	pStatProcInfo += ON_TODAY - 1; //按天	
	int i = 0;
	if(pStatProcInfo->iDay != iDay)
	{
		memset(pStatProcInfo->aProcessInfo_,0,sizeof(S_ProcessInfo)*MAX_STAT_PROCESS_INFO);
		pStatProcInfo->iDay = iDay;
		pStatProcInfo->iTotal = 1;
		pStatProcInfo->aProcessInfo_[i].iProcessID = iProcessID;
		pStatProcInfo->aProcessInfo_[i].iAllTickets += iNum;
		if(iType == 0)
			pStatProcInfo->aProcessInfo_[i].iNormalTickets += iNum;
		else
			pStatProcInfo->aProcessInfo_[i].iErrTickets += iNum;
	}
	else
	{
		i = -1;
		for (int n=0;n<pStatProcInfo->iTotal;n++)
		{
			if (pStatProcInfo->aProcessInfo_[n].iProcessID == iProcessID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pStatProcInfo->iTotal++;
		}
		pStatProcInfo->aProcessInfo_[i].iProcessID = iProcessID;
		pStatProcInfo->aProcessInfo_[i].iAllTickets += iNum;
		if(iType == 0)
			pStatProcInfo->aProcessInfo_[i].iNormalTickets += iNum;
		else
			pStatProcInfo->aProcessInfo_[i].iErrTickets += iNum;
	}

	pStatProcInfo = pStatAllInfo->aProcessInfo;
	pStatProcInfo += ON_HOUR - 1; //按小时
	i = 0;
	if(pStatProcInfo->iHour != iHour)
	{
		memset(pStatProcInfo->aProcessInfo_,0,sizeof(S_ProcessInfo)*MAX_STAT_PROCESS_INFO);
		pStatProcInfo->iHour = iHour;
		pStatProcInfo->iTotal = 1;
		pStatProcInfo->aProcessInfo_[i].iProcessID = iProcessID;
		pStatProcInfo->aProcessInfo_[i].iAllTickets += iNum;
		if(iType == 0)
			pStatProcInfo->aProcessInfo_[i].iNormalTickets += iNum;
		else
			pStatProcInfo->aProcessInfo_[i].iErrTickets += iNum;
	}
	else
	{
		i = -1;
		for (int n=0;n<pStatProcInfo->iTotal;n++)
		{
			if (pStatProcInfo->aProcessInfo_[n].iProcessID == iProcessID)
			{
				i = n;
				break;
			}
		}
		if (i == -1)
		{
			i = pStatProcInfo->iTotal++;
		}
		pStatProcInfo->aProcessInfo_[i].iProcessID = iProcessID;
		pStatProcInfo->aProcessInfo_[i].iAllTickets += iNum;
		if(iType == 0)
			pStatProcInfo->aProcessInfo_[i].iNormalTickets += iNum;
		else
			pStatProcInfo->aProcessInfo_[i].iErrTickets += iNum;
	}
	pStatProcInfo = pStatAllInfo->aProcessInfo;
	pStatProcInfo += ON_ONLINE - 1; //按启动以来	
	i = -1;
	for (int n=0;n<pStatProcInfo->iTotal;n++)
	{
		if (pStatProcInfo->aProcessInfo_[n].iProcessID == iProcessID)
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pStatProcInfo->iTotal++;
	}
	pStatProcInfo->aProcessInfo_[i].iProcessID = iProcessID;
	pStatProcInfo->aProcessInfo_[i].iAllTickets += iNum;
	if(iType == 0)
		pStatProcInfo->aProcessInfo_[i].iNormalTickets += iNum;
	else
		pStatProcInfo->aProcessInfo_[i].iErrTickets += iNum;
	
	pSem->V();
	return true;
}

//拥塞级别
bool StatAllInfoMgr::setBlockLevel(int iBlockLevel,int iRateCfg,char* sBlockTime)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	if (!pProcInfoHead)
	{
		return false;
	}
	pSem->P();
	pStatAllInfo->iBlockLevel = iBlockLevel;
	pStatAllInfo->iRateCfg = iRateCfg;
	strcpy(pStatAllInfo->sBlockTime,sBlockTime);
	pSem->V();
	//为了高低水写核心参数
	char sCmd[128] = {0};
	sprintf(sCmd,"business.schedule_block_level=%d",iBlockLevel);
	m_pCmdCom->changeSectionKey(sCmd);
	return true;

}

//
bool StatAllInfoMgr::setBlockRate(int iRate)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	if (!pProcInfoHead)
	{
		return false;
	}

	pSem->P();
	pStatAllInfo->iRate = iRate;
	pSem->V();
	return true;
}

//拥塞统计
bool StatAllInfoMgr::addBlockInfo( char* sServiceName,int iPLevel, int lInNum, int lOutNum,int lDiscardNum,char* sDatetime )
{
	int iDay = 0;
	int iHour = 0;
	GetDayAndHour(sDatetime,iDay,iHour);
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_BlockInfo_Buff *pBlockInfo = 0;
	pBlockInfo = &(pStatAllInfo->aBlockInfo);
	
	int i = -1;
	for (int n=0;n<pBlockInfo->iTotal;n++)
	{
		if(strcmp(pBlockInfo->aBlockInfo_[n].sServiceName,sServiceName) == 0 && 
			pBlockInfo->aBlockInfo_[n].iPLevel == iPLevel)
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pBlockInfo->iTotal++;
	}
	strcpy(pBlockInfo->aBlockInfo_[i].sServiceName,sServiceName);
	pBlockInfo->aBlockInfo_[i].iPLevel = iPLevel;
	pBlockInfo->aBlockInfo_[i].lInNum += lInNum;
	pBlockInfo->aBlockInfo_[i].lOutNum += lOutNum;
	pBlockInfo->aBlockInfo_[i].lDiscardNum += lDiscardNum;

	pSem->V();
	return true;
}

//信息点
/*

//信息点
//信息点1
//index [m_szServiceContextId,m_szOriginHost,m_szProtocolType,m_iResultCode,
//	m_iSuccessFlag,m_iReqDir,m_szProtocolType,m_iMsgType]

struct S_InfoPoint
{
int	m_iMsgType;			// 消息类型
int	m_iSuccessFlag;			// 是否成功
int	m_iReqDir;			// 方向
int	m_iReqCount;			// 消息数 累加
int	m_iReqBytes;			// 消息大小 Byte 累加
int	m_iResultCode;			// 返回码
char m_szKey[100];			// 能力||消息类型（1|0）
char m_szServiceContextId[100];		// 能力名称
char m_szOriginHost[100];		// 请求 OriginHost
char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
};
struct S_InfoPoint_Buff
{
int iTotal;
struct S_InfoPoint aInfoPoint_[MAX_STAT_INFOPOINT_INFO];
};

//信息点2 top 10
//index【m_szService_context_id,m_szMajor_avp_value,m_szProtocolType】
struct S_InfoPointTop
{
int m_iCount;				// 消息数 累加
char m_szService_context_id[100];	// 能力名称
char m_szMajor_avp_value[100];		// 关键字段值
char m_szKey[100];			// 能力||消息类型（1|0）
char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
};

struct S_InfoPointTop_Buff
{
int iTotal;
struct S_InfoPointTop aInfoPointTop_[MAX_STAT_INFOPOINT_INFO];
};

*/
bool StatAllInfoMgr::addInfoPoint(struct S_InfoPoint *pInfoPoint)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_InfoPoint_Buff *pInfoPointBuff = 0;
	pInfoPointBuff = &(pStatAllInfo->aInfoPoint);

	if (!pInfoPoint)
	{
		pSem->V();
		return false;
	}
//比较
	//index [m_szServiceContextId,m_szOriginHost,m_szProtocolType,m_iResultCode,
	//	m_iSuccessFlag,m_iReqDir,m_iMsgType]
	int i = -1;
	for (int n=0;n<pInfoPointBuff->iTotal;n++)
	{
		if(strcmp(pInfoPointBuff->aInfoPoint_[n].m_szServiceContextId,pInfoPoint->m_szServiceContextId) == 0 && 
			strcmp(pInfoPointBuff->aInfoPoint_[n].m_szOriginHost,pInfoPoint->m_szOriginHost) == 0 && 
			strcmp(pInfoPointBuff->aInfoPoint_[n].m_szProtocolType,pInfoPoint->m_szProtocolType) == 0 && 
			pInfoPointBuff->aInfoPoint_[n].m_iResultCode == pInfoPoint->m_iResultCode &&
			pInfoPointBuff->aInfoPoint_[n].m_iSuccessFlag == pInfoPoint->m_iSuccessFlag &&
			pInfoPointBuff->aInfoPoint_[n].m_iReqDir == pInfoPoint->m_iReqDir &&
			pInfoPointBuff->aInfoPoint_[n].m_iMsgType == pInfoPoint->m_iMsgType 
		  )
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pInfoPointBuff->iTotal++;
	}

	if (i >= MAX_STAT_INFOPOINT_INFO)
	{
		return false;
	}

/*
	int	m_iReqCount;			// 消息数 累加
	int	m_iReqBytes;			// 消息大小 Byte 累加
*/
	pInfoPointBuff->aInfoPoint_[i].m_iMsgType		= pInfoPoint->m_iMsgType ;			// 消息类型
	pInfoPointBuff->aInfoPoint_[i].m_iSuccessFlag	= pInfoPoint->m_iSuccessFlag;		// 是否成功
	pInfoPointBuff->aInfoPoint_[i].m_iReqDir		= pInfoPoint->m_iReqDir;			// 方向
	pInfoPointBuff->aInfoPoint_[i].m_lReqCount		+= pInfoPoint->m_lReqCount;			// 消息数 累加
	pInfoPointBuff->aInfoPoint_[i].m_lReqBytes		+= pInfoPoint->m_lReqBytes;			// 消息大小 Byte 累加
	pInfoPointBuff->aInfoPoint_[i].m_iResultCode	= pInfoPoint->m_iResultCode;		// 返回码
	
	//char m_szKey[100];			// 能力||消息类型（1|0）
	strcpy(pInfoPointBuff->aInfoPoint_[i].m_szKey,pInfoPoint->m_szKey);
	//char m_szServiceContextId[100];		// 能力名称
	strcpy(pInfoPointBuff->aInfoPoint_[i].m_szServiceContextId,pInfoPoint->m_szServiceContextId);
	//char m_szOriginHost[100];		// 请求 OriginHost
	strcpy(pInfoPointBuff->aInfoPoint_[i].m_szOriginHost,pInfoPoint->m_szOriginHost);
	//char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
	strcpy(pInfoPointBuff->aInfoPoint_[i].m_szProtocolType,pInfoPoint->m_szProtocolType);

	pSem->V();
	return true;
}

int StatAllInfoMgr::getInfoPoint(struct S_InfoPoint *pInfoPoint,int iBuffLen)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	if (!pInfoPoint)
	{
		pSem->V();
		return -1;
	}

	int iLen = 0;
	struct S_InfoPoint_Buff *pInfoPointBuff = 0;
	pInfoPointBuff = &(pStatAllInfo->aInfoPoint);

	if(0 == pInfoPoint || iBuffLen == 0)
		return 0;
	if (pInfoPointBuff->iTotal > iBuffLen)
	{
		iLen = iBuffLen;
	}
	else
		iLen = pInfoPointBuff->iTotal;
	memcpy((void *)pInfoPoint,(void *)pInfoPointBuff->aInfoPoint_,sizeof(S_InfoPoint)*iLen);
	
	delInfoPoint();
	pSem->V();
	return iLen;
}

bool StatAllInfoMgr::delInfoPoint()
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	/*
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	*/
	struct S_InfoPoint_Buff *pInfoPointBuff = 0;
	pInfoPointBuff = &(pStatAllInfo->aInfoPoint);
	memset(pInfoPointBuff,0,sizeof(S_InfoPoint_Buff));
	//pSem->V();
	return true;
}

//
bool StatAllInfoMgr::addInfoPointTop(struct S_InfoPointTop *pInfoPointTop)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	struct S_InfoPointTop_Buff *pInfoPointTopBuff = 0;
	pInfoPointTopBuff = &(pStatAllInfo->aInfoPointTop);

	if (!pInfoPointTop)
	{
		pSem->V();
		return false;
	}

	//比较
	//index【m_szService_context_id,m_szMajor_avp_value,m_szProtocolType】
	int i = -1;
	for (int n=0;n<pInfoPointTopBuff->iTotal;n++)
	{
		if(strcmp(pInfoPointTopBuff->aInfoPointTop_[n].m_szService_context_id,pInfoPointTop->m_szService_context_id) == 0 && 
			strcmp(pInfoPointTopBuff->aInfoPointTop_[n].m_szMajor_avp_value,pInfoPointTop->m_szMajor_avp_value) == 0 && 
			strcmp(pInfoPointTopBuff->aInfoPointTop_[n].m_szProtocolType,pInfoPointTop->m_szProtocolType) == 0 )
		{
			i = n;
			break;
		}
	}
	if (i == -1)
	{
		i = pInfoPointTopBuff->iTotal++;
	}

	if (i >= MAX_STAT_INFOPOINT_INFO)
	{
		return false;
	}

	/*
	int m_iCount;				// 消息数 累加
	char m_szService_context_id[100];	// 能力名称
	char m_szMajor_avp_value[100];		// 关键字段值
	char m_szKey[100];			// 能力||消息类型（1|0）
	char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
	*/
	pInfoPointTopBuff->aInfoPointTop_[i].m_lCount	+= pInfoPointTop->m_lCount ;

	strcpy(pInfoPointTopBuff->aInfoPointTop_[i].m_szService_context_id,pInfoPointTop->m_szService_context_id);
	strcpy(pInfoPointTopBuff->aInfoPointTop_[i].m_szMajor_avp_value,pInfoPointTop->m_szMajor_avp_value);
	strcpy(pInfoPointTopBuff->aInfoPointTop_[i].m_szKey,pInfoPointTop->m_szKey);
	strcpy(pInfoPointTopBuff->aInfoPointTop_[i].m_szProtocolType,pInfoPointTop->m_szProtocolType);

	pSem->V();
	return true;
}

int StatAllInfoMgr::getInfoPointTop(struct S_InfoPointTop *pInfoPointTop,int iBuffLen)
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	int iLen = 0;
	struct S_InfoPointTop_Buff *pInfoPointTopBuff = 0;
	pInfoPointTopBuff = &(pStatAllInfo->aInfoPointTop);

	if(0 == pInfoPointTop || iBuffLen == 0)
		return 0;

	if (pInfoPointTopBuff->iTotal > iBuffLen)
	{
		iLen = iBuffLen;
	}
	else
		iLen = pInfoPointTopBuff->iTotal;
	memcpy((void *)pInfoPointTop,(void *)pInfoPointTopBuff->aInfoPointTop_,sizeof(S_InfoPointTop)*iLen);
	
	delInfoPointTop();
	pSem->V();
	return iLen;
}

bool StatAllInfoMgr::delInfoPointTop()
{
	if(!pStatAllInfo)
	{
		if(!AttachStatShm())
			return false;
	}
	/*
	if (!pSem)
	{
		if(!GetStatLock())
			return false;
	}
	pSem->P();
	*/
	struct S_InfoPointTop_Buff *pInfoPointTopBuff = 0;
	pInfoPointTopBuff = &(pStatAllInfo->aInfoPointTop);
	memset(pInfoPointTopBuff,0,sizeof(S_InfoPointTop_Buff));
	//pSem->V();
	return true;
}