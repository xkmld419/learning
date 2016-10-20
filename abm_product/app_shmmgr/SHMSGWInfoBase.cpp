#include <stdio.h>
#include "SHMSGWInfoBase.h"

#define ATTACH_DATA_TWICE(X, Y, Z) \
	if (X) { \
	m_bAttached = true; \
	} else { \
	X = new SHMData<Y> (Z); \
	if (!(X)) { \
	freeAll (); \
	THROW(MBC_UserInfo+1); \
	} \
	if (!(X->exist())) { \
	m_bAttached = false; \
	printf("%s(%d): ATTACH_DATA failed (key:%ld)\n",__FILE__,__LINE__,(long)Z); \
	} \
	}

#define ATTACH_INT_INDEX_TWICE(X, Z) \
	if (X) { \
	m_bAttached = true; \
	} else { \
	X = new SHMIntHashIndex (Z); \
	if (!(X)) { \
	freeAll (); \
	THROW(MBC_UserInfo+1); \
	} \
	if (!(X->exist())) { \
	m_bAttached = false; \
	printf("%s(%d): ATTACH_INT_INDEX failed (key:%ld)\n",__FILE__,__LINE__,(long)Z); \
	} \
	}

#define ATTACH_STRING_INDEX_TWICE(X, Z) \
	if (X) { \
	m_bAttached =true; \
	} else { \
	X = new SHMStringTreeIndex (Z); \
	if (!(X)) { \
	freeAll (); \
	THROW(MBC_UserInfo+1); \
	} \
	if (!(X->exist())) { \
	m_bAttached = false; \
	printf("%s(%d): ATTACH_INT_INDEX failed (key:%ld)\n",__FILE__,__LINE__,(long)Z); \
	} \
	}

#define ATTACH_INT_INDEX_A_TWICE(X, Z) \
	if (X) { \
	m_bAttached =true; \
	}  else { \
	X = new SHMIntHashIndex_A (Z); \
	if (!(X)) { \
	freeAll (); \
	THROW(MBC_UserInfo+1); \
	}  \
	if (!(X->exist())) { \
	m_bAttached = false; \
	printf("%s(%d): ATTACH_INT_INDEX_A failed (key:%ld)\n",__FILE__,__LINE__,(long)Z); \
	} \
	}

//##ModelId=4BDF87DE02AE
SHMData<ContextData > *SHMSGWInfoBase::m_pSHMMgrContextData = NULL;
//SHMStringTreeIndex * SHMSGWInfoBase::m_pSHMMgrContextIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrContextDataLock = NULL;
ContextData* SHMSGWInfoBase::m_pMgrContextData = NULL;

//##ModelId=4BDF87DE02AR
SHMData<SMsgMapInfo> *SHMSGWInfoBase::m_pSHMMgrMsgMapInfoData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrMsgMapInfoIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrMsgMapInfoLock = NULL;
SMsgMapInfo* SHMSGWInfoBase::m_pMgrMsgMapInfoData = NULL;

//##ModelId=4BDF87DE02AR
SHMData<ServiceContextListData > *SHMSGWInfoBase::m_pSHMMgrServiceTxtListData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServiceTxtListIndex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvTxtListIDIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServiceTxtListLock = NULL;
ServiceContextListData* SHMSGWInfoBase::m_pMgrServiceTxtListData = NULL;

//##ModelId=4BDF87DE13ET
SHMData<StreamControlInfo > *SHMSGWInfoBase::m_pSHMMgrStreamCtrlInfoData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrStreamCtrlInfoIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrStreamCtrlInfoLock = NULL;
StreamControlInfo* SHMSGWInfoBase::m_pMgrStreamCtrlInfoData = NULL;

//##ModelId=4BDF87DE13ET
SHMData<StreamControlData > *SHMSGWInfoBase::m_pSHMMgrStreamCtrlData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrStreamCtrlDataIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrStreamCtrlDataLock = NULL;
StreamControlData* SHMSGWInfoBase::m_pMgrStreamCtrlData = NULL;

//##ModelId=4BDF87DE13ET
SHMData<SgwSlaQueue > *SHMSGWInfoBase::m_pSHMMgrSgwSlaqueRelData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSgwSlaqueRelIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSgwSlaqueRelLock = NULL;
SgwSlaQueue* SHMSGWInfoBase::m_pMgrSgwSlaqueRelData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<UserInfoData > *SHMSGWInfoBase::m_pSHMMgrUserInfoData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrUserInfoIndex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrUserInfoIndex_A =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrUserInfoDataLock = NULL;
UserInfoData* SHMSGWInfoBase::m_pMgrUserInfoData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<NetInfoData > *SHMSGWInfoBase::m_pSHMMgrNetInfoData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrNetInfoIndex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrNetInfoIndex_S =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrNetInfoLock = NULL;
NetInfoData* SHMSGWInfoBase::m_pMgrNetInfoData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<SessionData > *SHMSGWInfoBase::m_pSHMMgrSessionData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSessionIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSessionLock = NULL;
SessionData* SHMSGWInfoBase::m_pMgrSessionData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<SessionClearData > *SHMSGWInfoBase::m_pSHMMgrSessionClearData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSessionClearIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSessionClearLock = NULL;
SessionClearData* SHMSGWInfoBase::m_pMgrSessionClearData= NULL;
SessionHead* SHMSGWInfoBase::m_pMgrSessionHeadSign= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CongestLevel > *SHMSGWInfoBase::m_pSHMMgrCongestLevelData = NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrCongestLevelLock = NULL;
CongestLevel* SHMSGWInfoBase::m_pMgrCongestLevel= NULL;

//##ModelId=4BDF87DE13ET
SHMData<ServiceContextListBaseData > *SHMSGWInfoBase::m_pSHMMgrServiceTxtListBaseData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServiceTxtListBaseIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServiceTxtListBaseLock = NULL;
ServiceContextListBaseData* SHMSGWInfoBase::m_pMgrServiceTxtListBaseData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<LoadBalanceData > *SHMSGWInfoBase::m_pSHMMgrLoadBalanceData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrLoadBalanceIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrLoadBalanceLock = NULL;
LoadBalanceData* SHMSGWInfoBase::m_pMgrLoadBalanceData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CBaseMethod > *SHMSGWInfoBase::m_pSHMMgrBaseMethodData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrBaseMethodindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrBaseMethodLock = NULL;
CBaseMethod* SHMSGWInfoBase::m_pMgrBaseMethodData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<SPacketSession > *SHMSGWInfoBase::m_pSHMMgrPacketSessionData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrPacketSessionindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrPacketSubSessionindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrPacketSessionLock = NULL;
SPacketSession* SHMSGWInfoBase::m_pMgrPacketSessionData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<ServicePackageNode > *SHMSGWInfoBase::m_pSHMMgrServicePackageData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageNodeSeqIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageLock = NULL;
ServicePackageNode* SHMSGWInfoBase::m_pMgrServicePackageData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<ServicePackageRoute > *SHMSGWInfoBase::m_pSHMMgrServicePackageRouteData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageRouteindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageRouteSeqIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageRouteLock = NULL;
ServicePackageRoute* SHMSGWInfoBase::m_pMgrServicePackageRouteData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CServicePackageRouteCCR > *SHMSGWInfoBase::m_pSHMMgrServicePackageRouteCCRData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageRouteCCRSeqindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageRouteSeqindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageRouteCCRLock = NULL;
CServicePackageRouteCCR* SHMSGWInfoBase::m_pMgrServicePackageRouteCCRData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CServicePackageJudge > *SHMSGWInfoBase::m_pSHMMgrServicePackageJudgeData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageJudgeindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageJudgeNodeSeqIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageJudgeLock = NULL;
CServicePackageJudge* SHMSGWInfoBase::m_pMgrServicePackageJudgeData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CServicePackageCCA > *SHMSGWInfoBase::m_pSHMMgrServicePackageCCAData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageCCAindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvPackageCCASeqIndex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageCCALock = NULL;
CServicePackageCCA* SHMSGWInfoBase::m_pMgrServicePackageCCAData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CServicePackageFunc > *SHMSGWInfoBase::m_pSHMMgrServicePackageFuncData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageFuncindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageFuncLock = NULL;
CServicePackageFunc* SHMSGWInfoBase::m_pMgrServicePackageFuncData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<CServicePackageVariable > *SHMSGWInfoBase::m_pSHMMgrServicePackageVariableData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrServicePackageVariableindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrServicePackageVariableLock = NULL;
CServicePackageVariable* SHMSGWInfoBase::m_pMgrServicePackageVariableData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<StatisticsCongestMsg > *SHMSGWInfoBase::m_pSHMMgrSCongestMsgData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSCongestMsgindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSCongestMsgLock = NULL;
StatisticsCongestMsg* SHMSGWInfoBase::m_pMgrSCongestMsgData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<ST_WF_PROCESS_MQ > *SHMSGWInfoBase::m_pSHMMgrWfprocessMqData = NULL;
SHMIntHashIndex * SHMSGWInfoBase::m_pSHMMgrWfprocessMqindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrWfprocessMqLock = NULL;
ST_WF_PROCESS_MQ* SHMSGWInfoBase::m_pMgrWfprocessMqData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<PortInfo > *SHMSGWInfoBase::m_pSHMMgrPortInfoData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrPortInfoindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrPortInfoLock = NULL;
PortInfo* SHMSGWInfoBase::m_pMgrPortInfoData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<Sgw_Org_Route > *SHMSGWInfoBase::m_pSHMMgrOrgRouteData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrOrgRouteindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrOrgRouteLock = NULL;
Sgw_Org_Route* SHMSGWInfoBase::m_pMgrOrgRouteData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<Sgw_Org_Head > *SHMSGWInfoBase::m_pSHMMgrOrgHeadData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrOrgHeadindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrOrgHeadLock = NULL;
Sgw_Org_Head* SHMSGWInfoBase::m_pMgrOrgHeadData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<SgwChannelInfo > *SHMSGWInfoBase::m_pSHMMgrChannelData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrChannelindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrChannelLock = NULL;
SgwChannelInfo* SHMSGWInfoBase::m_pMgrChannelData= NULL;

//##ModelId=4BDF87DE13ET
SHMData<SgwGlobalMap > *SHMSGWInfoBase::m_pSHMMgrGlobalMapData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrGlobalMapindex =NULL;
CSemaphore * SHMSGWInfoBase::m_pSHMMgrGlobalMapLock =NULL;
SgwGlobalMap* SHMSGWInfoBase::m_pMgrGlobalMapData= NULL;

SHMData<SgwUserStaff > *SHMSGWInfoBase::m_pSHMMgrUserStaffData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrUserStaffIDindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrStaffIDindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrUserStaffChannelIDindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrUserStaffLock = NULL;
SgwUserStaff* SHMSGWInfoBase::m_pMgrUserStaffData= NULL;

SHMData<SgwOrgRouteRule > *SHMSGWInfoBase::m_pSHMMgrOrgRoutRuleData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrOrgRouteIDindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrOrgIDindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrOrgRoutRuleLock = NULL;
SgwOrgRouteRule* SHMSGWInfoBase::m_pMgrOrgRoutRuleData= NULL;

SHMData<SgwAreaOrg > *SHMSGWInfoBase::m_pSHMMgrAreaOrgData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrAreaOrgSeqindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrAreaOrgLock = NULL;
SgwAreaOrg* SHMSGWInfoBase::m_pMgrAreaOrgData= NULL;

SHMData<SgwTuxedoRelationIn > *SHMSGWInfoBase::m_pSHMMgrTuxRelaInData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrTuxInSvrsIDindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrTuxRelaInLock = NULL;
SgwTuxedoRelationIn* SHMSGWInfoBase::m_pMgrTuxRelaInData= NULL;

SHMData<SgwTuxedoRelationOut > *SHMSGWInfoBase::m_pSHMMgrTuxRelaOutData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrTuxOutSvrTxtIDindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrTuxOutRelaIDindex =NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrTuxRelaOutLock = NULL;
SgwTuxedoRelationOut* SHMSGWInfoBase::m_pMgrTuxRelaOutData= NULL;

SHMData<SgwServicesList > *SHMSGWInfoBase::m_pSHMMgrSrvsListData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsListIDindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsListServicesNameindex =NULL;
SgwServicesList* SHMSGWInfoBase::m_pMgrSrvsListData= NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSrvsListLock = NULL;

SHMData<SgwServicesParamRelation > *SHMSGWInfoBase::m_pSHMMgrSrvsParamRelaData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsParamRelaindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsParamRelaMappingIDIndex =NULL;
SgwServicesParamRelation* SHMSGWInfoBase::m_pMgrSrvsParamRelaData= NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSrvsParamRelaLock = NULL;

SHMData<SgwServicesParamList > *SHMSGWInfoBase::m_pSHMMgrSrvsParamListData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsParamListindex =NULL;
SgwServicesParamList* SHMSGWInfoBase::m_pMgrSrvsParamListData= NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSrvsParamListLock = NULL;

SHMData<SgwServiceRegister > *SHMSGWInfoBase::m_pSHMMgrSrvsRegisterData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsRegisterindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrSrvsRegisterSvrInsIDIndex =NULL;
SgwServiceRegister* SHMSGWInfoBase::m_pMgrSrvsRegisterData= NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSrvsRegisterLock = NULL;

SHMData<SgwParamInstanceList > *SHMSGWInfoBase::m_pSHMMgrParamInsListData = NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrParamInsListindex =NULL;
SHMIntHashIndex_A * SHMSGWInfoBase::m_pSHMMgrParamInsListMapIDindex =NULL;
SgwParamInstanceList* SHMSGWInfoBase::m_pMgrParamInsListData= NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrParamInsListLock = NULL;

SHMData<SgwEnumArea> * SHMSGWInfoBase::m_pSHMMgrSgwEnumAreaData = NULL;
SHMIntHashIndex_A  * SHMSGWInfoBase::m_pSHMMgrSgwEnumAreaindex = NULL;
SHMIntHashIndex_A  * SHMSGWInfoBase::m_pSHMMgrSgwEnumAreaOrgIDindex = NULL;
SgwEnumArea * SHMSGWInfoBase::m_pMgrSgwEnumAreaData = NULL;
CSemaphore* SHMSGWInfoBase::m_pSHMMgrSgwEnumAreaLock = NULL;

bool SHMSGWInfoBase::m_bAttached = true;
ThreeLogGroup SHMSGWInfoBase::m_oLogObj = true;

void SHMSGWInfoBase::bindData()
{
	m_pMgrContextData = (ContextData *)(*m_pSHMMgrContextData);
	m_pMgrMsgMapInfoData = (SMsgMapInfo*)(*m_pSHMMgrMsgMapInfoData);
	m_pMgrServiceTxtListData = (ServiceContextListData *)(*m_pSHMMgrServiceTxtListData);
	m_pMgrStreamCtrlInfoData = (StreamControlInfo *)(*m_pSHMMgrStreamCtrlInfoData);
	m_pMgrStreamCtrlData = (StreamControlData *)(*m_pSHMMgrStreamCtrlData);
	m_pMgrUserInfoData = (UserInfoData *)(*m_pSHMMgrUserInfoData);
	m_pMgrSgwSlaqueRelData = (SgwSlaQueue *)(*m_pSHMMgrSgwSlaqueRelData);
	m_pMgrNetInfoData = (NetInfoData *)(*m_pSHMMgrNetInfoData);
	m_pMgrSessionData = (SessionData *)(*m_pSHMMgrSessionData);
	m_pMgrSessionClearData = (SessionClearData  *)(*m_pSHMMgrSessionClearData);
	m_pMgrSessionHeadSign = (SessionHead  *)(char *)(m_pMgrSessionClearData);
	m_pMgrCongestLevel = (CongestLevel *)(*m_pSHMMgrCongestLevelData);
	m_pMgrServiceTxtListBaseData= (ServiceContextListBaseData *)(*m_pSHMMgrServiceTxtListBaseData);
	m_pMgrLoadBalanceData= (LoadBalanceData *)(*m_pSHMMgrLoadBalanceData);
	m_pMgrBaseMethodData= (CBaseMethod *)(*m_pSHMMgrBaseMethodData);
	m_pMgrPacketSessionData= (SPacketSession *)(*m_pSHMMgrPacketSessionData);
	m_pMgrServicePackageData= (ServicePackageNode *)(*m_pSHMMgrServicePackageData);
	m_pMgrServicePackageRouteData= (ServicePackageRoute *)(*m_pSHMMgrServicePackageRouteData);
	m_pMgrServicePackageRouteCCRData= (CServicePackageRouteCCR *)(*m_pSHMMgrServicePackageRouteCCRData);
	m_pMgrServicePackageJudgeData= (CServicePackageJudge *)(*m_pSHMMgrServicePackageJudgeData);
	m_pMgrServicePackageCCAData= (CServicePackageCCA *)(*m_pSHMMgrServicePackageCCAData);
	m_pMgrServicePackageFuncData= (CServicePackageFunc *)(*m_pSHMMgrServicePackageFuncData);
	m_pMgrServicePackageVariableData= (CServicePackageVariable *)(*m_pSHMMgrServicePackageVariableData);
	m_pMgrSCongestMsgData= (StatisticsCongestMsg *)(*m_pSHMMgrSCongestMsgData);
	m_pMgrWfprocessMqData= (ST_WF_PROCESS_MQ *)(*m_pSHMMgrWfprocessMqData);
	m_pMgrPortInfoData= (PortInfo *)(*m_pSHMMgrPortInfoData);
	m_pMgrOrgHeadData= (Sgw_Org_Head *)(*m_pSHMMgrOrgHeadData);
	m_pMgrOrgRouteData= (Sgw_Org_Route *)(*m_pSHMMgrOrgRouteData);
	m_pMgrChannelData= (SgwChannelInfo *)(*m_pSHMMgrChannelData);
	m_pMgrUserStaffData= (SgwUserStaff *)(*m_pSHMMgrUserStaffData);
	m_pMgrGlobalMapData= (SgwGlobalMap *)(*m_pSHMMgrGlobalMapData);
	m_pMgrOrgRoutRuleData= (SgwOrgRouteRule *)(*m_pSHMMgrOrgRoutRuleData);
	m_pMgrAreaOrgData= (SgwAreaOrg *)(*m_pSHMMgrAreaOrgData);
	m_pMgrTuxRelaInData= (SgwTuxedoRelationIn *)(*m_pSHMMgrTuxRelaInData);
	m_pMgrTuxRelaOutData= (SgwTuxedoRelationOut *)(*m_pSHMMgrTuxRelaOutData);
	m_pMgrSrvsListData= (SgwServicesList *)(*m_pSHMMgrSrvsListData);
	m_pMgrSrvsParamRelaData= (SgwServicesParamRelation *)(*m_pSHMMgrSrvsParamRelaData);
	m_pMgrSrvsParamListData= (SgwServicesParamList *)(*m_pSHMMgrSrvsParamListData);
	m_pMgrSrvsRegisterData= (SgwServiceRegister *)(*m_pSHMMgrSrvsRegisterData);
	m_pMgrParamInsListData= (SgwParamInstanceList *)(*m_pSHMMgrParamInsListData);
}

void SHMSGWInfoBase::contentfiltering_free()
{
	FREE(m_pSHMMgrContextData);
	FREE(m_pSHMMgrContextDataLock);
}

void SHMSGWInfoBase::streamctrldata_free()
{
	FREE(m_pSHMMgrStreamCtrlData);
	FREE(m_pSHMMgrStreamCtrlDataIndex);
	FREE(m_pSHMMgrStreamCtrlDataLock);
}

void SHMSGWInfoBase::msgmap_free()
{
	FREE(m_pSHMMgrMsgMapInfoData);
	FREE(m_pSHMMgrMsgMapInfoIndex);
	FREE(m_pSHMMgrMsgMapInfoLock);
}

void SHMSGWInfoBase::servicetxtlist_free()
{
	FREE(m_pSHMMgrServiceTxtListData);
	FREE(m_pSHMMgrServiceTxtListIndex);
	FREE(m_pSHMMgrSrvTxtListIDIndex);
	FREE(m_pSHMMgrServiceTxtListLock);
}

void SHMSGWInfoBase::userinfo_free()
{
	FREE(m_pSHMMgrUserInfoData);
	FREE(m_pSHMMgrUserInfoIndex);
	FREE(m_pSHMMgrUserInfoIndex_A);
	FREE(m_pSHMMgrUserInfoDataLock);
}

void SHMSGWInfoBase::congestleve_free()
{
	FREE(m_pSHMMgrCongestLevelData);
	FREE(m_pSHMMgrCongestLevelLock);
}

void SHMSGWInfoBase::session_free()
{
	FREE(m_pSHMMgrSessionData);
	FREE(m_pSHMMgrSessionIndex);
	FREE(m_pSHMMgrSessionLock);
}

void SHMSGWInfoBase::sessionclear_free()
{
	FREE(m_pSHMMgrSessionClearData);
	FREE(m_pSHMMgrSessionClearIndex);
	FREE(m_pSHMMgrSessionClearLock);
}

void SHMSGWInfoBase::netinfo_free()
{
	FREE(m_pSHMMgrNetInfoData);
	FREE(m_pSHMMgrNetInfoIndex);
	FREE(m_pSHMMgrNetInfoIndex_S);
	FREE(m_pSHMMgrNetInfoLock);

}

void SHMSGWInfoBase::sgwslaquerel_free()
{
	FREE(m_pSHMMgrSgwSlaqueRelData);
	FREE(m_pSHMMgrSgwSlaqueRelIndex);
	FREE(m_pSHMMgrSgwSlaqueRelLock);
}

void SHMSGWInfoBase::streamctrl_free()
{
	FREE(m_pSHMMgrStreamCtrlInfoData);
	FREE(m_pSHMMgrStreamCtrlInfoIndex);
	FREE(m_pSHMMgrStreamCtrlInfoLock);
}

void SHMSGWInfoBase::servicetxtlistbase_free()
{
	FREE(m_pSHMMgrServiceTxtListBaseData);
	FREE(m_pSHMMgrServiceTxtListBaseIndex);
	FREE(m_pSHMMgrServiceTxtListBaseLock);
}

void SHMSGWInfoBase::loadbalancedata_free()
{
	FREE(m_pSHMMgrLoadBalanceData);
	FREE(m_pSHMMgrLoadBalanceIndex);
	FREE(m_pSHMMgrLoadBalanceLock);
}

void SHMSGWInfoBase::basemethod_free()
{
	FREE(m_pSHMMgrBaseMethodData);
	FREE(m_pSHMMgrBaseMethodindex);
	FREE(m_pSHMMgrBaseMethodLock);
}

void SHMSGWInfoBase::packetsession_free()
{
	FREE(m_pSHMMgrPacketSessionData);
	FREE(m_pSHMMgrPacketSessionindex);
	FREE(m_pSHMMgrPacketSubSessionindex);
	FREE(m_pSHMMgrPacketSessionLock);
}

void SHMSGWInfoBase::servicepackage_free()
{
	FREE(m_pSHMMgrServicePackageData);
	FREE(m_pSHMMgrServicePackageindex);
	FREE(m_pSHMMgrSrvPackageNodeSeqIndex);
	FREE(m_pSHMMgrServicePackageLock);
}

void SHMSGWInfoBase::servicepackageroute_free()
{
	FREE(m_pSHMMgrServicePackageRouteData);
	FREE(m_pSHMMgrServicePackageRouteindex);
	FREE(m_pSHMMgrSrvPackageRouteSeqIndex);
	FREE(m_pSHMMgrServicePackageRouteLock);
}

void SHMSGWInfoBase::servicepackagerouteCCR_free()
{
	FREE(m_pSHMMgrServicePackageRouteCCRData);
	FREE(m_pSHMMgrSrvPackageRouteCCRSeqindex);
	FREE(m_pSHMMgrSrvPackageRouteSeqindex);
	FREE(m_pSHMMgrServicePackageRouteCCRLock);
}

void SHMSGWInfoBase::servicepackagejudge_free()
{
	FREE(m_pSHMMgrServicePackageJudgeData);
	FREE(m_pSHMMgrServicePackageJudgeindex);
	FREE(m_pSHMMgrSrvPackageJudgeNodeSeqIndex);
	FREE(m_pSHMMgrServicePackageJudgeLock);
}

void SHMSGWInfoBase::servicepackagecca_free()
{
	FREE(m_pSHMMgrServicePackageCCAData);
	FREE(m_pSHMMgrServicePackageCCAindex);
	FREE(m_pSHMMgrSrvPackageCCASeqIndex);
	FREE(m_pSHMMgrServicePackageCCALock);
}

void SHMSGWInfoBase::servicepackagefunc_free()
{
	FREE(m_pSHMMgrServicePackageFuncData);
	FREE(m_pSHMMgrServicePackageFuncindex);
	FREE(m_pSHMMgrServicePackageFuncLock);
}

void SHMSGWInfoBase::servicepackagevariable_free()
{
	FREE(m_pSHMMgrServicePackageVariableData);
	FREE(m_pSHMMgrServicePackageVariableindex);
	FREE(m_pSHMMgrServicePackageVariableLock);
}

void SHMSGWInfoBase::scongestmsg_free()
{
	FREE(m_pSHMMgrSCongestMsgLock);
	FREE(m_pSHMMgrSCongestMsgData);
	FREE(m_pSHMMgrSCongestMsgindex);
}

void SHMSGWInfoBase::wfprocessmq_free()
{
	FREE(m_pSHMMgrWfprocessMqData);
	FREE(m_pSHMMgrWfprocessMqindex);
	FREE(m_pSHMMgrWfprocessMqLock);
}

void SHMSGWInfoBase::portInfo_free()
{
	FREE(m_pSHMMgrPortInfoData);
	FREE(m_pSHMMgrPortInfoindex);
	FREE(m_pSHMMgrPortInfoLock);
}

void SHMSGWInfoBase::sgworgroute_free()
{
	FREE(m_pSHMMgrOrgRouteData);
	FREE(m_pSHMMgrOrgRouteindex);
	FREE(m_pSHMMgrOrgRouteLock);
}

void SHMSGWInfoBase::sgworghead_free()
{
	FREE(m_pSHMMgrOrgHeadData);
	FREE(m_pSHMMgrOrgHeadindex);
	FREE(m_pSHMMgrOrgHeadLock);
}

void SHMSGWInfoBase::channelinfo_free()
{
	FREE(m_pSHMMgrChannelData);
	FREE(m_pSHMMgrChannelindex);
	FREE(m_pSHMMgrChannelLock);
}

void SHMSGWInfoBase::globalmap_free()
{
	FREE(m_pSHMMgrGlobalMapData);
	FREE(m_pSHMMgrGlobalMapindex);
	FREE(m_pSHMMgrGlobalMapLock);
}

void SHMSGWInfoBase::userstaff_free()
{
	FREE(m_pSHMMgrUserStaffData);
	FREE(m_pSHMMgrUserStaffIDindex);
	FREE(m_pSHMMgrUserStaffLock);
	FREE(m_pSHMMgrStaffIDindex);
	FREE(m_pSHMMgrUserStaffChannelIDindex);
}

void SHMSGWInfoBase::orgrouterule_free()
{
	FREE(m_pSHMMgrOrgRoutRuleData);
	FREE(m_pSHMMgrOrgRouteIDindex);
	FREE(m_pSHMMgrOrgIDindex);
	FREE(m_pSHMMgrOrgRoutRuleLock);
}

void SHMSGWInfoBase::sgwareaorg_free()
{
	FREE(m_pSHMMgrAreaOrgData);
	FREE(m_pSHMMgrAreaOrgSeqindex);
	FREE(m_pSHMMgrAreaOrgLock);
}

void SHMSGWInfoBase::sgwtuxrelain_free()
{
	FREE(m_pSHMMgrTuxRelaInData);
	FREE(m_pSHMMgrTuxInSvrsIDindex);
	FREE(m_pSHMMgrTuxRelaInLock);
}

void SHMSGWInfoBase::sgwtuxrelaout_free()
{
	FREE(m_pSHMMgrTuxRelaOutData);
	FREE(m_pSHMMgrTuxOutSvrTxtIDindex);
	FREE(m_pSHMMgrTuxOutRelaIDindex);
	FREE(m_pSHMMgrTuxRelaOutLock);
}

void SHMSGWInfoBase::sgwsvrlist_free()
{
	FREE(m_pSHMMgrSrvsListData);
	FREE(m_pSHMMgrSrvsListIDindex);
	FREE(m_pSHMMgrSrvsListServicesNameindex);
	FREE(m_pSHMMgrSrvsListLock);
}

void SHMSGWInfoBase::sgwsrvsparamrela_free()
{
	FREE(m_pSHMMgrSrvsParamRelaData);
	FREE(m_pSHMMgrSrvsParamRelaindex);
	FREE(m_pSHMMgrSrvsParamRelaMappingIDIndex);
	FREE(m_pSHMMgrSrvsParamRelaLock);
}
void SHMSGWInfoBase::sgwsrvsparamlist_free()
{
	FREE(m_pSHMMgrSrvsParamListData);
	FREE(m_pSHMMgrSrvsParamListindex);
	FREE(m_pSHMMgrSrvsParamListLock);
}
void SHMSGWInfoBase::sgwsrvsregister_free()
{
	FREE(m_pSHMMgrSrvsRegisterData);
	FREE(m_pSHMMgrSrvsRegisterindex);
	FREE(m_pSHMMgrSrvsRegisterSvrInsIDIndex);
	FREE(m_pSHMMgrSrvsRegisterLock);
}
void SHMSGWInfoBase::sgwparaminslist_free()
{
	FREE(m_pSHMMgrParamInsListData);
	FREE(m_pSHMMgrParamInsListindex);
	FREE(m_pSHMMgrParamInsListMapIDindex);
	FREE(m_pSHMMgrParamInsListLock);
}
void SHMSGWInfoBase::sgwenumarea_free()
{
	FREE(m_pSHMMgrSgwEnumAreaData);
	FREE(m_pSHMMgrSgwEnumAreaindex);
	FREE(m_pSHMMgrSgwEnumAreaOrgIDindex);
	FREE(m_pSHMMgrSgwEnumAreaLock);
}
///////////////
int SHMSGWInfoBase::sgwenumarea_init()
{
	try {
		if(!m_pSHMMgrSgwEnumAreaLock){
			char sTemp[32] = {0};
			m_pSHMMgrSgwEnumAreaLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PARAMINSLIST_KEY_);
			m_pSHMMgrSgwEnumAreaLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSgwEnumAreaData, SgwEnumArea  , _SHM_SGWENUMAREA_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSgwEnumAreaindex ,_SHM_SGWENUMAREA_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSgwEnumAreaOrgIDindex ,_SHM_SGWENUMAREA_INDEX_KEY_A_);

		if (m_pSHMMgrSgwEnumAreaData->exist()) {
			m_pMgrSgwEnumAreaData = (SgwEnumArea *)(*m_pSHMMgrSgwEnumAreaData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_enum_area共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}
int SHMSGWInfoBase::sgwparaminslist_init()
{
	try {
		if(!m_pSHMMgrParamInsListLock){
			char sTemp[32] = {0};
			m_pSHMMgrParamInsListLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PARAMINSLIST_KEY_);
			m_pSHMMgrParamInsListLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrParamInsListData, SgwParamInstanceList  , _SHM_PARAMINSLIST_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrParamInsListindex ,_SHM_PARAMINSLIST_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrParamInsListMapIDindex ,_SHM_PARAMINSLIST_INDEX_KEY_A_);

		if (m_pSHMMgrParamInsListData->exist()) {
			m_pMgrParamInsListData = (SgwParamInstanceList *)(*m_pSHMMgrParamInsListData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_param_instance_list共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}
int SHMSGWInfoBase::sgwsrvsregister_init()
{
	try {
		if(!m_pSHMMgrSrvsRegisterLock){
			char sTemp[32] = {0};
			m_pSHMMgrSrvsRegisterLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SVRSREGISTER_KEY_);
			m_pSHMMgrSrvsRegisterLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSrvsRegisterData, SgwServiceRegister  , _SHM_SVRSREGISTER_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsRegisterindex ,_SHM_SVRSREGISTER_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsRegisterSvrInsIDIndex ,_SHM_SVRSREGISTER_INDEX_KEY_A_);

		if (m_pSHMMgrSrvsRegisterData->exist()) {
			m_pMgrSrvsRegisterData = (SgwServiceRegister *)(*m_pSHMMgrSrvsRegisterData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_service_register共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwsrvsparamlist_init()
{
	try {
		if(!m_pSHMMgrSrvsParamListLock){
			char sTemp[32] = {0};
			m_pSHMMgrSrvsParamListLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SVRSPARAMLIST_KEY_);
			m_pSHMMgrSrvsParamListLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSrvsParamListData, SgwServicesParamList  , _SHM_SVRSPARAMLIST_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsParamListindex ,_SHM_SVRSPARAMLIST_INDEX_KEY_);

		if (m_pSHMMgrSrvsParamListData->exist()) {
			m_pMgrSrvsParamListData = (SgwServicesParamList *)(*m_pSHMMgrSrvsParamListData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_services_param_list共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwsrvsparamrela_init()
{
	try {
		if(!m_pSHMMgrSrvsParamRelaLock){
			char sTemp[32] = {0};
			m_pSHMMgrSrvsParamRelaLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SVRSPARAMRELA_KEY_);
			m_pSHMMgrSrvsParamRelaLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSrvsParamRelaData, SgwServicesParamRelation  , _SHM_SVRSPARAMRELA_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsParamRelaindex ,_SHM_SVRSPARAMRELA_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsParamRelaMappingIDIndex ,_SHM_SVRSPARAMRELA_INDEX_KEY_A_);

		if (m_pSHMMgrSrvsParamRelaData->exist()) {
			m_pMgrSrvsParamRelaData = (SgwServicesParamRelation *)(*m_pSHMMgrSrvsParamRelaData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_services_param_relation共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwsvrlist_init()
{
	try {
		if(!m_pSHMMgrSrvsListLock){
			char sTemp[32] = {0};
			m_pSHMMgrSrvsListLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SVRLIST_KEY_);
			m_pSHMMgrSrvsListLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSrvsListData, SgwServicesList  , _SHM_SVRLIST_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsListIDindex ,_SHM_SVRLIST_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvsListServicesNameindex ,_SHM_SVRLIST_INDEX_KEY_A_);

		if (m_pSHMMgrSrvsListData->exist()) {
			m_pMgrSrvsListData = (SgwServicesList *)(*m_pSHMMgrSrvsListData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_services_list共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwtuxrelaout_init()
{
	try {
		if(!m_pSHMMgrTuxRelaOutLock){
			char sTemp[32] = {0};
			m_pSHMMgrTuxRelaOutLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_TUXRELAOUT_KEY_);
			m_pSHMMgrTuxRelaOutLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrTuxRelaOutData, SgwTuxedoRelationOut  , _SHM_TUXRELAOUT_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrTuxOutSvrTxtIDindex ,_SHM_TUXRELAOUT_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrTuxOutRelaIDindex ,_SHM_TUXRELAOUT_INDEX_KEY_A_);

		if (m_pSHMMgrTuxRelaOutData->exist()) {
			m_pMgrTuxRelaOutData = (SgwTuxedoRelationOut *)(*m_pSHMMgrTuxRelaOutData);

			if (!m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
				m_pSHMMgrTuxRelaOutData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrTuxOutRelaIDindex->exist()) {
				m_pSHMMgrTuxRelaOutData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		} else {
			if (m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
				m_pSHMMgrMsgMapInfoData->close();
			}

			if (m_pSHMMgrTuxOutRelaIDindex->exist()) {
				m_pSHMMgrMsgMapInfoData->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_tuxedo_relation_out共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwtuxrelain_init()
{
	try {
		if(!m_pSHMMgrTuxRelaInLock){
			char sTemp[32] = {0};
			m_pSHMMgrTuxRelaInLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_TUXRELAIN_KEY_);
			m_pSHMMgrTuxRelaInLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrTuxRelaInData, SgwTuxedoRelationIn  , _SHM_TUXRELAIN_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrTuxInSvrsIDindex ,_SHM_TUXRELAIN_INDEX_KEY_);

		if (m_pSHMMgrTuxRelaInData->exist()) {
			m_pMgrTuxRelaInData = (SgwTuxedoRelationIn *)(*m_pSHMMgrTuxRelaInData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_tuxedo_relation共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::sgwareaorg_init()
{
	try {
		if(!m_pSHMMgrAreaOrgLock){
			char sTemp[32] = {0};
			m_pSHMMgrAreaOrgLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_AREAORG_KEY_);
			m_pSHMMgrAreaOrgLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrAreaOrgData, SgwAreaOrg , _SHM_AREAORG_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrAreaOrgSeqindex ,_SHM_AREAORG_INDEX_KEY_);

		if (m_pSHMMgrAreaOrgData->exist()) {
			m_pMgrAreaOrgData = (SgwAreaOrg *)(*m_pSHMMgrAreaOrgData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_area_org共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::orgrouterule_init()
{
	try {
		if(!m_pSHMMgrOrgRoutRuleLock){
			char sTemp[32] = {0};
			m_pSHMMgrOrgRoutRuleLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_ORGROUTERULE_KEY_);
			m_pSHMMgrOrgRoutRuleLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrOrgRoutRuleData, SgwOrgRouteRule , _SHM_ORGROUTERULE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrOrgRouteIDindex ,_SHM_ORGROUTERULE_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrOrgIDindex ,_SHM_ORGROUTERULE_INDEX_KEY_A_);

		if (m_pSHMMgrOrgRoutRuleData->exist()) {
			m_pMgrOrgRoutRuleData = (SgwOrgRouteRule *)(*m_pSHMMgrOrgRoutRuleData);

			if (!m_pSHMMgrOrgRouteIDindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrOrgIDindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_route_rule共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

//初始化连接贡献内存包含读取核心参数（核心参数如果配置错误或者缺少配置，则记录错误日志，跑出异常）
int SHMSGWInfoBase::contentfiltering_init()
{
	try {
		if(!m_pSHMMgrContextDataLock){
			char sTemp[32] = {0};
			m_pSHMMgrContextDataLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_CONTEXTDATA_KEY_);
			m_pSHMMgrContextDataLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrContextData, ContextData , _SHM_CONTEXTDATA_DATA_KEY_);
		//ATTACH_STRING_INDEX_TWICE(m_pSHMMgrContextIndex ,_SHM_CONTEXTDATA_INDEX_KEY_);

		if (m_pSHMMgrContextData->exist()) {
			m_pMgrContextData = (ContextData *)(*m_pSHMMgrContextData);
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接context_filter共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::msgmap_init()
{
	try {
		if(!m_pSHMMgrMsgMapInfoLock){
			char sTemp[32] = {0};
			m_pSHMMgrMsgMapInfoLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_MSGMAPINFO_KEY_);
			m_pSHMMgrMsgMapInfoLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrMsgMapInfoData, SMsgMapInfo , _SHM_MSGMAPINFO_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrMsgMapInfoIndex ,_SHM_MSGMAPINFO_INDEX_KEY_);

		if (m_pSHMMgrMsgMapInfoData->exist()) {
			m_pMgrMsgMapInfoData = (SMsgMapInfo*)(*m_pSHMMgrMsgMapInfoData);
			if (!m_pSHMMgrMsgMapInfoIndex->exist()) {
				m_pSHMMgrMsgMapInfoData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		} else {
			if (m_pSHMMgrMsgMapInfoIndex->exist()) {
				m_pSHMMgrMsgMapInfoData->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接tab_msg_map共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::servicetxtlist_init()
{
	try {
		if(!m_pSHMMgrServiceTxtListLock){
			char sTemp[32] = {0};
			m_pSHMMgrServiceTxtListLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SERVICETXTLIST_KEY_);
			m_pSHMMgrServiceTxtListLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServiceTxtListData, ServiceContextListData , _SHM_SERVICETXTLIST_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServiceTxtListIndex ,_SHM_SERVICETXTLIST_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvTxtListIDIndex ,_SHM_SERVICETXTLIST_INDEX_KEY_A_);

		if (m_pSHMMgrServiceTxtListData->exist()) {
			m_pMgrServiceTxtListData = (ServiceContextListData *)(*m_pSHMMgrServiceTxtListData);
			if (!m_pSHMMgrServiceTxtListIndex->exist()) {
				m_pSHMMgrServiceTxtListData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrSrvTxtListIDIndex->exist()) {
				m_pSHMMgrServiceTxtListData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrServiceTxtListIndex->exist()) {
				m_pSHMMgrServiceTxtListData->close();
			}
			if (!m_pSHMMgrSrvTxtListIDIndex->exist()) {
				m_pSHMMgrServiceTxtListData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_context_list共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::streamctrldata_init()
{
	try {

		if(!m_pSHMMgrStreamCtrlDataLock){
			char sTemp[32] = {0};
			m_pSHMMgrStreamCtrlDataLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_STREAMCTRLDATA_KEY_);
			m_pSHMMgrStreamCtrlDataLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrStreamCtrlData, StreamControlData , _SHM_STREAMCTRLDATA_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrStreamCtrlDataIndex ,_SHM_STREAMCTRLDATA_INDEX_KEY_);


		if (m_pSHMMgrStreamCtrlData->exist()) {
			m_pMgrStreamCtrlData = (StreamControlData *)(*m_pSHMMgrStreamCtrlData);
			if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
				m_pSHMMgrStreamCtrlData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrStreamCtrlDataIndex->exist()) {
				m_pSHMMgrStreamCtrlDataIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接stream_ctrl_data共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

//初始化参数
int SHMSGWInfoBase::userinfo_init()
{
	try {

		if(!m_pSHMMgrUserInfoDataLock){
			char sTemp[32] = {0};
			m_pSHMMgrUserInfoDataLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_USERINFODATA_KEY_);
			m_pSHMMgrUserInfoDataLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrUserInfoData, UserInfoData , _SHM_USERINFODATA_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrUserInfoIndex ,_SHM_USERINFODATA_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrUserInfoIndex_A ,_SHM_USERINFODATA_INDEX_KEY_A_);

		if (m_pSHMMgrUserInfoData->exist()) {
			m_pMgrUserInfoData = (UserInfoData *)(*m_pSHMMgrUserInfoData);
			if (!m_pSHMMgrUserInfoIndex->exist()) {
				m_pSHMMgrUserInfoData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrUserInfoIndex_A->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrUserInfoIndex->exist()) {
				m_pSHMMgrUserInfoIndex->close();
			}
			if (m_pSHMMgrUserInfoIndex_A->exist()) {
				m_pSHMMgrUserInfoIndex_A->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接userinfo共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::congestleve_init()
{
	try {

		if(!m_pSHMMgrCongestLevelLock){
			char sTemp[32] = {0};
			m_pSHMMgrCongestLevelLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_CONGESTLEVEL_KEY_);
			m_pSHMMgrCongestLevelLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrCongestLevelData, CongestLevel , _SHM_CONGESTLEVEL_DATA_KEY_);

		if (m_pSHMMgrCongestLevelData->exist()) {
			m_pMgrCongestLevel = (CongestLevel *)(*m_pSHMMgrCongestLevelData);
		} else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接shmcongestlevel共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::session_init()
{
	try {

		if(!m_pSHMMgrSessionLock){
			char sTemp[32] = {0};
			m_pSHMMgrSessionLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SESSION_KEY_);
			m_pSHMMgrSessionLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSessionData, SessionData , _SHM_SESSION_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSessionIndex ,_SHM_SESSION_INDEX_KEY_);

		if (m_pSHMMgrSessionData->exist()) {
			m_pMgrSessionData = (SessionData *)(*m_pSHMMgrSessionData);
			if (!m_pSHMMgrSessionIndex->exist()) {
				m_pSHMMgrSessionData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrSessionIndex->exist()) {
				m_pSHMMgrSessionIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;

	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sessiondata共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::sessionclear_init()
{
	try {

		if(!m_pSHMMgrSessionClearLock){
			char sTemp[32] = {0};
			m_pSHMMgrSessionClearLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SESSIONCLEAR_KEY_);
			m_pSHMMgrSessionClearLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSessionClearData, SessionClearData , _SHM_SESSIONCLEAR_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSessionClearIndex ,_SHM_SESSIONCLEAR_INDEX_KEY_);

		if (m_pSHMMgrSessionClearData->exist()) {
			m_pMgrSessionClearData = (SessionClearData *)(*m_pSHMMgrSessionClearData);
			m_pMgrSessionHeadSign = (SessionHead *)(char *)m_pMgrSessionClearData;
			if (!m_pSHMMgrSessionClearIndex->exist()) {
				m_pSHMMgrSessionClearData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrSessionClearIndex->exist()) {
				m_pSHMMgrSessionClearIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接session_clear共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

//初始化参数
int SHMSGWInfoBase::netinfo_init()
{
	try {

		if(!m_pSHMMgrNetInfoLock){
			char sTemp[32] = {0};
			m_pSHMMgrNetInfoLock = new CSemaphore ();

			sprintf (sTemp, "%d", _PV_NETINFO_KEY_);
			m_pSHMMgrNetInfoLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrNetInfoData, NetInfoData , _SHM_NETINFO_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrNetInfoIndex ,_SHM_NETINFO_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrNetInfoIndex_S ,_SHM_NETINFO_INDEX_KEY_S_);

		if (m_pSHMMgrNetInfoData->exist()) {
			m_pMgrNetInfoData = (NetInfoData *)(*m_pSHMMgrNetInfoData);

			if (!m_pSHMMgrNetInfoIndex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrNetInfoIndex_S->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接netInfodata共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::sgwslaquerel_init()
{
	try {
		if(!m_pSHMMgrSgwSlaqueRelLock){
			char sTemp[32] = {0};
			m_pSHMMgrSgwSlaqueRelLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SGWSLAQUEUE_KEY_);
			m_pSHMMgrSgwSlaqueRelLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSgwSlaqueRelData, SgwSlaQueue , _SHM_SGWSLAQUEUE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSgwSlaqueRelIndex ,_SHM_SGWSLAQUEUE_INDEX_KEY_);

		if (m_pSHMMgrSgwSlaqueRelData->exist()) {
			m_pMgrSgwSlaqueRelData = (SgwSlaQueue *)(*m_pSHMMgrSgwSlaqueRelData);
			if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
				m_pSHMMgrSgwSlaqueRelData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrSgwSlaqueRelIndex->exist()) {
				m_pSHMMgrSgwSlaqueRelIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;

	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_sla_que_rel共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::streamctrl_init()
{
	try {

		if(!m_pSHMMgrStreamCtrlInfoLock){
			char sTemp[32] = {0};
			m_pSHMMgrStreamCtrlInfoLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_STREAMCTRLINFO_KEY_);
			m_pSHMMgrStreamCtrlInfoLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrStreamCtrlInfoData, StreamControlInfo , _SHM_STREAMCTRLINFO_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrStreamCtrlInfoIndex ,_SHM_STREAMCTRLINFO_INDEX_KEY_);

		if (m_pSHMMgrStreamCtrlInfoData->exist()) {
			m_pMgrStreamCtrlInfoData = (StreamControlInfo *)(*m_pSHMMgrStreamCtrlInfoData);
			if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
				m_pSHMMgrStreamCtrlInfoData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrStreamCtrlInfoIndex->exist()) {
				m_pSHMMgrStreamCtrlInfoIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接stream_ctrl_info共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::servicetxtlistbase_init()
{
	try {

		if(!m_pSHMMgrServiceTxtListBaseLock){
			char sTemp[32] = {0};
			m_pSHMMgrServiceTxtListBaseLock = new CSemaphore ();

			sprintf (sTemp, "%d", _PV_SERVICETXTLISTBASE_KEY_);
			m_pSHMMgrServiceTxtListBaseLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServiceTxtListBaseData, ServiceContextListBaseData , _SHM_SERVICETXTLISTBASE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServiceTxtListBaseIndex ,_SHM_SERVICETXTLISTBASE_INDEX_KEY_);

		if (m_pSHMMgrServiceTxtListBaseData->exist()) {
			m_pMgrServiceTxtListBaseData = (ServiceContextListBaseData *)(*m_pSHMMgrServiceTxtListBaseData);
			if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
				m_pSHMMgrServiceTxtListBaseData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrServiceTxtListBaseIndex->exist()) {
				m_pSHMMgrServiceTxtListBaseIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_context_list_base共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}


//初始化参数
int SHMSGWInfoBase::loadbalancedata_init()
{
	try {
		if(!m_pSHMMgrLoadBalanceLock){
			char sTemp[32] = {0};
			m_pSHMMgrLoadBalanceLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_LOADBALANCE_KEY_);
			m_pSHMMgrLoadBalanceLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrLoadBalanceData, LoadBalanceData , _SHM_LOADBALANCE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrLoadBalanceIndex ,_SHM_LOADBALANCE_INDEX_KEY_);

		if (m_pSHMMgrLoadBalanceData->exist()) {
			m_pMgrLoadBalanceData = (LoadBalanceData *)(*m_pSHMMgrLoadBalanceData);
			if (!m_pSHMMgrLoadBalanceIndex->exist()) {
				m_pSHMMgrLoadBalanceData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrLoadBalanceIndex->exist()) {
				m_pSHMMgrLoadBalanceIndex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接loadbalancedata共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::basemethod_init()
{
	try {

		if(!m_pSHMMgrBaseMethodLock){
			char sTemp[32] = {0};
			m_pSHMMgrBaseMethodLock = new CSemaphore ();

			sprintf (sTemp, "%d", _PV_BASEMETHOD_KEY_);
			m_pSHMMgrBaseMethodLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrBaseMethodData, CBaseMethod , _SHM_BASEMETHOD_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrBaseMethodindex ,_SHM_BASEMETHOD_INDEX_KEY_);

		if (m_pSHMMgrBaseMethodData->exist()) {
			m_pMgrBaseMethodData = (CBaseMethod *)(*m_pSHMMgrBaseMethodData);
			if (!m_pSHMMgrBaseMethodindex->exist()) {
				m_pSHMMgrBaseMethodData->close();
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			if (m_pSHMMgrBaseMethodindex->exist()) {
				m_pSHMMgrBaseMethodindex->close();
			}
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接base_method共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackage_init()
{
	try {
		if(!m_pSHMMgrServicePackageLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SERVICEPACKAGE_KEY_);
			m_pSHMMgrServicePackageLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageData, ServicePackageNode , _SHM_SERVICEPACKAGE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageindex ,_SHM_SERVICEPACKAGE_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageNodeSeqIndex ,_SHM_SERVICEPACKAGE_INDEX_KEY_A_);

		if (m_pSHMMgrServicePackageData->exist()) {
			m_pMgrServicePackageData = (ServicePackageNode *)(*m_pSHMMgrServicePackageData);

			if (!m_pSHMMgrServicePackageindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_node共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackageroute_init()
{
	try {
		if(!m_pSHMMgrServicePackageRouteLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageRouteLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PACKAGEROUTE_KEY_);
			m_pSHMMgrServicePackageRouteLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageRouteData, ServicePackageRoute , _SHM_PACKAGEROUTE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageRouteindex ,_SHM_PACKAGEROUTE_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageRouteSeqIndex ,_SHM_PACKAGEROUTE_INDEX_KEY_A_);

		if (m_pSHMMgrServicePackageRouteData->exist()) {
			m_pMgrServicePackageRouteData = (ServicePackageRoute *)(*m_pSHMMgrServicePackageRouteData);

			if (!m_pSHMMgrServicePackageRouteindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_route共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

//初始化参数
int SHMSGWInfoBase::packetsession_init()
{
	try {

		if(!m_pSHMMgrPacketSessionLock){
			char sTemp[32] = {0};
			m_pSHMMgrPacketSessionLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PACKETSESSION_KEY_);
			m_pSHMMgrPacketSessionLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrPacketSessionData, SPacketSession , _SHM_PACKETSESSION_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrPacketSessionindex ,_SHM_PACKETSESSION_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrPacketSubSessionindex ,_SHM_PACKETSUBSESSION_INDEX_KEY_);

		if (m_pSHMMgrPacketSessionData->exist()) {
			m_pMgrPacketSessionData = (SPacketSession *)(*m_pSHMMgrPacketSessionData);

			if (!m_pSHMMgrPacketSessionindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrPacketSubSessionindex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}

		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接s_packet_session共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackagerouteCCR_init()
{
	try {

		if(!m_pSHMMgrServicePackageRouteCCRLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageRouteCCRLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PACKAGEROUTECCR_KEY_);
			m_pSHMMgrServicePackageRouteCCRLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageRouteCCRData, CServicePackageRouteCCR , _SHM_PACKAGEROUTECCR_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageRouteCCRSeqindex ,_SHM_PACKAGEROUTECCR_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageRouteSeqindex ,_SHM_PACKAGEROUTECCR_INDEX_KEY_A_);

		if (m_pSHMMgrServicePackageRouteCCRData->exist()) {
			m_pMgrServicePackageRouteCCRData = (CServicePackageRouteCCR *)(*m_pSHMMgrServicePackageRouteCCRData);

			if (!m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_route_ccr共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackagejudge_init()
{
	try {

		if(!m_pSHMMgrServicePackageJudgeLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageJudgeLock = new CSemaphore ();

			sprintf (sTemp, "%d", _PV_SERVICEPACKAGEJUDGE_KEY_);
			m_pSHMMgrServicePackageJudgeLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageJudgeData, CServicePackageJudge , _SHM_SERVICEPACKAGEJUDGE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageJudgeindex ,_SHM_SERVICEPACKAGEJUDGE_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageJudgeNodeSeqIndex ,_SHM_SERVICEPACKAGEJUDGE_INDEX_KEY_A_);

		if (m_pSHMMgrServicePackageJudgeData->exist()) {
			m_pMgrServicePackageJudgeData = (CServicePackageJudge *)(*m_pSHMMgrServicePackageJudgeData);

			if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_judge共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackagecca_init()
{
	try {
		if(!m_pSHMMgrServicePackageCCALock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageCCALock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SERVICEPACKAGECCA_KEY_);
			m_pSHMMgrServicePackageCCALock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageCCAData, CServicePackageCCA , _SHM_SERVICEPACKAGECCA_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageCCAindex ,_SHM_SERVICEPACKAGECCA_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSrvPackageCCASeqIndex ,_SHM_SERVICEPACKAGECCA_INDEX_KEY_A_);

		if (m_pSHMMgrServicePackageCCAData->exist()) {
			m_pMgrServicePackageCCAData = (CServicePackageCCA *)(*m_pSHMMgrServicePackageCCAData);

			if (!m_pSHMMgrServicePackageCCAindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			} else {
				if (!m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
					return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
				}
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_cca共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::servicepackagefunc_init()
{
	try {
		if(!m_pSHMMgrServicePackageFuncLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageFuncLock = new CSemaphore ();

			sprintf (sTemp, "%d", _PV_SERVICEPACKAGEFUNC_KEY_);
			m_pSHMMgrServicePackageFuncLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageFuncData, CServicePackageFunc , _SHM_SERVICEPACKAGEFUNC_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageFuncindex ,_SHM_SERVICEPACKAGEFUNC_INDEX_KEY_);

		if (m_pSHMMgrServicePackageFuncData->exist()) {
			m_pMgrServicePackageFuncData = (CServicePackageFunc *)(*m_pSHMMgrServicePackageFuncData);

			if (!m_pSHMMgrServicePackageFuncindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_func共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::servicepackagevariable_init()
{
	try {
		if(!m_pSHMMgrServicePackageVariableLock){
			char sTemp[32] = {0};
			m_pSHMMgrServicePackageVariableLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SERVICEPACKAGEVLE_KEY_);
			m_pSHMMgrServicePackageVariableLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrServicePackageVariableData, CServicePackageVariable , _SHM_SERVICEPACKAGEVLE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrServicePackageVariableindex ,_SHM_SERVICEPACKAGEVLE_INDEX_KEY_);

		if (m_pSHMMgrServicePackageVariableData->exist()) {
			m_pMgrServicePackageVariableData = (CServicePackageVariable *)(*m_pSHMMgrServicePackageVariableData);

			if (!m_pSHMMgrServicePackageVariableindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接service_package_variable共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::scongestmsg_init()
{
	try {
		if(!m_pSHMMgrSCongestMsgLock){
			char sTemp[32] = {0};
			m_pSHMMgrSCongestMsgLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_SCONGESTMSG_KEY_);
			m_pSHMMgrSCongestMsgLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrSCongestMsgData, StatisticsCongestMsg , _SHM_SCONGESTMSG_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrSCongestMsgindex ,_SHM_SCONGESTMSG_INDEX_KEY_);

		if (m_pSHMMgrSCongestMsgData->exist()) {
			m_pMgrSCongestMsgData = (StatisticsCongestMsg *)(*m_pSHMMgrSCongestMsgData);

			if (!m_pSHMMgrSCongestMsgindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接statisticscongestmsg共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::wfprocessmq_init()
{
	try {
		if(!m_pSHMMgrWfprocessMqLock){
			char sTemp[32] = {0};
			m_pSHMMgrWfprocessMqLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_WFPROCESSMQ_KEY_);
			m_pSHMMgrWfprocessMqLock->getSem (sTemp, 1, 1);
		}

		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrWfprocessMqData, ST_WF_PROCESS_MQ , _SHM_WFPROCESSMQ_DATA_KEY_);
		ATTACH_INT_INDEX_TWICE(m_pSHMMgrWfprocessMqindex ,_SHM_WFPROCESSMQ_INDEX_KEY_);

		if (m_pSHMMgrWfprocessMqData->exist()) {
			m_pMgrWfprocessMqData = (ST_WF_PROCESS_MQ *)(*m_pSHMMgrWfprocessMqData);

			if (!m_pSHMMgrWfprocessMqindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接wf_process_mq共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}
}

int SHMSGWInfoBase::portInfo_init()
{
	try {
		if(!m_pSHMMgrPortInfoLock){
			char sTemp[32] = {0};
			m_pSHMMgrPortInfoLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_PORTINFO_KEY_);
			m_pSHMMgrPortInfoLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrPortInfoData, PortInfo , _SHM_PORTINFO_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrPortInfoindex ,_SHM_PORTINFO_INDEX_KEY_);

		if (m_pSHMMgrPortInfoData->exist()) {
			m_pMgrPortInfoData = (PortInfo *)(*m_pSHMMgrPortInfoData);

			if (!m_pSHMMgrPortInfoindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接portinfo共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::sgworgroute_init()
{
	try {
		if(!m_pSHMMgrOrgRouteLock){
			char sTemp[32] = {0};
			m_pSHMMgrOrgRouteLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_ORGROUTE_KEY_);
			m_pSHMMgrOrgRouteLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrOrgRouteData, Sgw_Org_Route , _SHM_ORGROUTE_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrOrgRouteindex ,_SHM_ORGROUTE_INDEX_KEY_);

		if (m_pSHMMgrOrgRouteData->exist()) {
			m_pMgrOrgRouteData = (Sgw_Org_Route *)(*m_pSHMMgrOrgRouteData);

			if (!m_pSHMMgrOrgRouteindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_org_route共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::globalmap_init()
{
	try {
		if(!m_pSHMMgrGlobalMapLock){
			char sTemp[32] = {0};
			m_pSHMMgrGlobalMapLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_GLOBALMAP_KEY_);
			m_pSHMMgrGlobalMapLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrGlobalMapData, SgwGlobalMap , _SHM_GLOBALMAP_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrGlobalMapindex ,_SHM_GLOBALMAP_INDEX_KEY_);

		if (m_pSHMMgrGlobalMapData->exist()) {
			m_pMgrGlobalMapData = (SgwGlobalMap *)(*m_pSHMMgrGlobalMapData);

			if (!m_pSHMMgrGlobalMapindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_global_map共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::sgworghead_init()
{
	try {
		if(!m_pSHMMgrOrgHeadLock){
			char sTemp[32] = {0};
			m_pSHMMgrOrgHeadLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_ORGHEAD_KEY_);
			m_pSHMMgrOrgHeadLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrOrgHeadData, Sgw_Org_Head , _SHM_ORGHEAD_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrOrgHeadindex ,_SHM_ORGHEAD_INDEX_KEY_);

		if (m_pSHMMgrOrgHeadData->exist()) {
			m_pMgrOrgHeadData = (Sgw_Org_Head *)(*m_pSHMMgrOrgHeadData);

			if (!m_pSHMMgrOrgHeadindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接sgw_org_head共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::channelinfo_init()
{
	try {
		if(!m_pSHMMgrChannelLock){
			char sTemp[32] = {0};
			m_pSHMMgrChannelLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_CHANNEL_KEY_);
			m_pSHMMgrChannelLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrChannelData, SgwChannelInfo , _SHM_CHANNEL_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrChannelindex ,_SHM_CHANNEL_INDEX_KEY_);

		if (m_pSHMMgrChannelData->exist()) {
			m_pMgrChannelData = (SgwChannelInfo *)(*m_pSHMMgrChannelData);

			if (!m_pSHMMgrChannelindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接channelInfo共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

int SHMSGWInfoBase::userstaff_init()
{
	try {
		if(!m_pSHMMgrUserStaffLock){
			char sTemp[32] = {0};
			m_pSHMMgrUserStaffLock = new CSemaphore ();
			sprintf (sTemp, "%d", _PV_USERSTAFF_KEY_);
			m_pSHMMgrUserStaffLock->getSem (sTemp, 1, 1);
		}
		//连接共享内存
		ATTACH_DATA_TWICE(m_pSHMMgrUserStaffData, SgwUserStaff , _SHM_USERSTAFF_DATA_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrUserStaffIDindex ,_SHM_USERSTAFF_INDEX_KEY_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrStaffIDindex ,_SHM_USERSTAFF_INDEX_KEY_A_);
		ATTACH_INT_INDEX_A_TWICE(m_pSHMMgrUserStaffChannelIDindex ,_SHM_USERSTAFF_INDEX_KEY_B_);

		if (m_pSHMMgrUserStaffData->exist()) {
			m_pMgrUserStaffData = (SgwUserStaff *)(*m_pSHMMgrUserStaffData);

			if (!m_pSHMMgrUserStaffIDindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrStaffIDindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
			if (!m_pSHMMgrUserStaffChannelIDindex->exist()) {
				return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
			}
		}  else {
			return MBC_SHM_ERRNO_DATA_NOT_EXIST;
		}
		return 0;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		m_oLogObj.sgwlog(MBC_MEM_LINK_FAIL,-1,-1,-1,2,2,-1,NULL,"连接user_staff共享内存失败(%s)",e.descript());
		exit(MBC_MEM_LINK_FAIL);
	}

}

//##ModelId=4247790103D4
void SHMSGWInfoBase::freeAll()
{
	contentfiltering_free();
	streamctrldata_free();
	msgmap_free();
	servicetxtlist_free();
	userinfo_free();
	congestleve_free();
	session_free();
	sessionclear_free();
	netinfo_free();
	sgwslaquerel_free();
	streamctrl_free();
	servicetxtlistbase_free();
	loadbalancedata_free();
	basemethod_free();
	packetsession_free();
	servicepackage_free();
	servicepackageroute_free();
	servicepackagerouteCCR_free();
	servicepackagejudge_free();
	servicepackagecca_free();
	servicepackagefunc_free();
	servicepackagevariable_free();
	scongestmsg_free();
	wfprocessmq_free();
	portInfo_free();
	sgworgroute_free();
	sgworghead_free();
	channelinfo_free();
	userstaff_free();
	globalmap_free();
	orgrouterule_free();
	sgwareaorg_free();
	sgwtuxrelain_free();
	sgwtuxrelaout_free();
	sgwsvrlist_free();
	sgwsrvsparamrela_free();
	sgwsrvsparamlist_free();
	sgwsrvsregister_free();
	sgwparaminslist_free();
	sgwenumarea_free();
}

int SHMSGWInfoBase::verifyParamAll()
{
	return 0;
}

int SHMSGWInfoBase::attachALL()
{
	contentfiltering_init();
	streamctrldata_init();
	msgmap_init();
	servicetxtlist_init();
	userinfo_init();
	congestleve_init();
	session_init();
	sessionclear_init();
	netinfo_init();
	sgwslaquerel_init();
	streamctrl_init();
	servicetxtlistbase_init();
	loadbalancedata_init();
	basemethod_init();
	packetsession_init();
	servicepackage_init();
	servicepackageroute_init();
	servicepackagerouteCCR_init();
	servicepackagejudge_init();
	servicepackagecca_init();
	servicepackagefunc_init();
	servicepackagevariable_init();
	scongestmsg_init();
	wfprocessmq_init();
	portInfo_init();
	sgworgroute_init();
	userstaff_init();
	globalmap_init();
	sgworghead_init();
	sgworgroute_init();
	channelinfo_init();
	orgrouterule_init();
	sgwareaorg_init();
	sgwtuxrelain_init();
	sgwtuxrelaout_init();
	sgwsvrlist_init();
	sgwsrvsparamrela_init();
	sgwsrvsparamlist_init();
	sgwsrvsregister_init();
	sgwparaminslist_init();
	sgwenumarea_init();

	if (m_bAttached) {
		bindData ();
	} else {
		return  MBC_MEM_LINK_FAIL;
	}
	return 0;
}
