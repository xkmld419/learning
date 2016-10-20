#ifndef SHMSGWINFOBASE_H_HEADER_INCLUDED_BDBB63C4
#define SHMSGWINFOBASE_H_HEADER_INCLUDED_BDBB63C4
#include "MBC_ABM.h"
#include "SgwDefine.h"
#include "ShmParamDef.h"
#include "CSemaphore.h"
#include "SHMData.h"
#include "SHMStringTreeIndex.h"
#include "SHMIntHashIndex_A.h"
#include "SHMIntHashIndex.h"

#ifndef __SQLLITE__
#include "Environment.h"
#include "TOCIQuery.h"
#define DEFINE_SGW_QUERY(x) DEFINE_QUERY(x)
#define GETVALUE_INT(x,y) x.field(y).asInteger()
#define GETVALUE_LONG(x,y) x.field(y).asLong()
#define GETVALUE_STRING(x,y) x.field(y).asString()
#define GETVALUE_DOUBLE(x,y) x.field(y).asDouble()
#else
#include "TSQLIteQuery.h"
#define DEFINE_SGW_QUERY(x) static SQLITEQuery x
#define GETVALUE_INT(x,y) x.getFieldAsInteger(y)
#define GETVALUE_LONG(x,y) x.getFieldAsLong(y)
#define GETVALUE_STRING(x,y) x.getFieldAsString(y)
#define GETVALUE_DOUBLE(x,y) x.getFieldAsFloat(y)
#endif

#define FREE(X) \
	if (X) { \
	delete X; \
	X = 0; \
	}

class SHMSGWInfoBase
{
public:
	SHMSGWInfoBase(){};

	~SHMSGWInfoBase(){};

	//全量操作
	static int attachALL();
	static int verifyParamAll();
	static void freeAll();
	static void bindData();

	//逐个连接并且绑定共享内存
	static int contentfiltering_init();
	static int streamctrldata_init();
	static int msgmap_init();
	static int servicetxtlist_init();
	static int userinfo_init();
	static int congestleve_init();
	static int session_init();
	static int sessionclear_init();
	static int netinfo_init();
	static int sgwslaquerel_init();
	static int streamctrl_init();
	static int servicetxtlistbase_init();
	static int loadbalancedata_init();
	static int basemethod_init();
	static int packetsession_init();
	static int servicepackage_init();
	static int servicepackageroute_init();
	static int servicepackagerouteCCR_init();
	static int servicepackagejudge_init();
	static int servicepackagecca_init();
	static int servicepackagefunc_init();
	static int servicepackagevariable_init();
	static int scongestmsg_init();
	static int wfprocessmq_init();
	static int portInfo_init();
	static int sgworgroute_init();
	static int sgworghead_init();
	static int channelinfo_init();
	static int userstaff_init();
	static int globalmap_init();
	static int orgrouterule_init();
	static int sgwareaorg_init();
	static int sgwtuxrelain_init();
	static int sgwtuxrelaout_init();
	static int sgwsvrlist_init();
	static int sgwsrvsparamrela_init();
	static int sgwsrvsparamlist_init();
	static int sgwsrvsregister_init();
	static int sgwparaminslist_init();
	static int sgwenumarea_init();

	//逐个释放内存
	static void contentfiltering_free();
	static void streamctrldata_free();
	static void msgmap_free();
	static void servicetxtlist_free();
	static void userinfo_free();
	static void congestleve_free();
	static void session_free();
	static void sessionclear_free();
	static void netinfo_free();
	static void sgwslaquerel_free();
	static void streamctrl_free();
	static void servicetxtlistbase_free();
	static void loadbalancedata_free();
	static void basemethod_free();
	static void packetsession_free();
	static void servicepackage_free();
	static void servicepackageroute_free();
	static void servicepackagerouteCCR_free();
	static void servicepackagejudge_free();
	static void servicepackagecca_free();
	static void servicepackagefunc_free();
	static void servicepackagevariable_free();
	static void scongestmsg_free();
	static void wfprocessmq_free();
	static void portInfo_free();
	static void sgworgroute_free();
	static void sgworghead_free();
	static void channelinfo_free();
	static void userstaff_free();
	static void globalmap_free();
	static void orgrouterule_free();
	static void sgwareaorg_free();
	static void sgwtuxrelain_free();
	static void sgwtuxrelaout_free();
	static void sgwsvrlist_free();
	static void sgwsrvsparamrela_free();
	static void sgwsrvsparamlist_free();
	static void sgwsrvsregister_free();
	static void sgwparaminslist_free();
	static void sgwenumarea_free();

public:
	//该共享内存提供在线业务进程操作接口，无文件名。
	static SHMData<StreamControlData> *m_pSHMMgrStreamCtrlData;
	static SHMIntHashIndex_A  *m_pSHMMgrStreamCtrlDataIndex;
	static StreamControlData *m_pMgrStreamCtrlData;
	static CSemaphore* m_pSHMMgrStreamCtrlDataLock;

	//context_filter表加载到共享内存
	static SHMData<ContextData> *m_pSHMMgrContextData;
	//static SHMStringTreeIndex  *m_pSHMMgrContextIndex;
	static ContextData *m_pMgrContextData;
	static CSemaphore* m_pSHMMgrContextDataLock;

	//TAB_MSG_MAP表加载到共享内存
	static SHMData<SMsgMapInfo> *m_pSHMMgrMsgMapInfoData;
	static SHMIntHashIndex_A  *m_pSHMMgrMsgMapInfoIndex;
	static SMsgMapInfo *m_pMgrMsgMapInfoData;
	static CSemaphore* m_pSHMMgrMsgMapInfoLock;

	//service_context_list表加载到共享内存
	static SHMData<ServiceContextListData> *m_pSHMMgrServiceTxtListData;
	static SHMIntHashIndex_A  *m_pSHMMgrServiceTxtListIndex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvTxtListIDIndex;
	static ServiceContextListData *m_pMgrServiceTxtListData;
	static CSemaphore* m_pSHMMgrServiceTxtListLock;

	//STREAM_CTRL_INFO表加载到共享内存
	static SHMData<StreamControlInfo> *m_pSHMMgrStreamCtrlInfoData;
	static SHMIntHashIndex_A  *m_pSHMMgrStreamCtrlInfoIndex;
	static StreamControlInfo *m_pMgrStreamCtrlInfoData;
	static CSemaphore* m_pSHMMgrStreamCtrlInfoLock;

	//UserInfo表加载到共享内存
	static SHMData<UserInfoData> *m_pSHMMgrUserInfoData;
	static SHMIntHashIndex_A  *m_pSHMMgrUserInfoIndex;
	static SHMIntHashIndex_A  *m_pSHMMgrUserInfoIndex_A;
	static UserInfoData *m_pMgrUserInfoData;
	static CSemaphore* m_pSHMMgrUserInfoDataLock;

	//SGW_SLA_QUE_REL表加载到共享内存
	static SHMData<SgwSlaQueue> *m_pSHMMgrSgwSlaqueRelData;
	static SHMIntHashIndex_A  *m_pSHMMgrSgwSlaqueRelIndex;
	static SgwSlaQueue *m_pMgrSgwSlaqueRelData;
	static CSemaphore* m_pSHMMgrSgwSlaqueRelLock;

	//NetInfoData表加载到共享内存
	static SHMData<NetInfoData> *m_pSHMMgrNetInfoData;
	static SHMIntHashIndex_A  *m_pSHMMgrNetInfoIndex;
	static SHMIntHashIndex_A  *m_pSHMMgrNetInfoIndex_S;
	static NetInfoData *m_pMgrNetInfoData;
	static CSemaphore* m_pSHMMgrNetInfoLock;

	static SHMData<SessionData> *m_pSHMMgrSessionData;
	static SHMIntHashIndex_A  *m_pSHMMgrSessionIndex;
	static SessionData *m_pMgrSessionData;
	static CSemaphore* m_pSHMMgrSessionLock;

	static SHMData<SessionClearData> *m_pSHMMgrSessionClearData;
	static SHMIntHashIndex_A  *m_pSHMMgrSessionClearIndex;
	static SessionClearData *m_pMgrSessionClearData;
	static SessionHead *m_pMgrSessionHeadSign;
	static CSemaphore* m_pSHMMgrSessionClearLock;

	static SHMData<CongestLevel> *m_pSHMMgrCongestLevelData;
	static CongestLevel *m_pMgrCongestLevel;
	static CSemaphore* m_pSHMMgrCongestLevelLock;

	static SHMData<ServiceContextListBaseData> *m_pSHMMgrServiceTxtListBaseData;
	static SHMIntHashIndex_A  *m_pSHMMgrServiceTxtListBaseIndex;
	static ServiceContextListBaseData *m_pMgrServiceTxtListBaseData;
	static CSemaphore* m_pSHMMgrServiceTxtListBaseLock;

	static SHMData<LoadBalanceData> *m_pSHMMgrLoadBalanceData;
	static SHMIntHashIndex_A  *m_pSHMMgrLoadBalanceIndex;
	static LoadBalanceData *m_pMgrLoadBalanceData;
	static CSemaphore* m_pSHMMgrLoadBalanceLock;

	//基础方法表
	static SHMData<CBaseMethod> *m_pSHMMgrBaseMethodData;
	static SHMIntHashIndex_A  *m_pSHMMgrBaseMethodindex;
	static CBaseMethod *m_pMgrBaseMethodData;
	static CSemaphore* m_pSHMMgrBaseMethodLock;

	//能力封装会话管理
	static SHMData<SPacketSession> *m_pSHMMgrPacketSessionData;
	static SHMIntHashIndex_A  *m_pSHMMgrPacketSessionindex;
	static SHMIntHashIndex_A  *m_pSHMMgrPacketSubSessionindex;
	static SPacketSession *m_pMgrPacketSessionData;
	static CSemaphore* m_pSHMMgrPacketSessionLock;

	//能力封装节点表
	static SHMData<ServicePackageNode> *m_pSHMMgrServicePackageData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageNodeSeqIndex;
	static ServicePackageNode *m_pMgrServicePackageData;
	static CSemaphore* m_pSHMMgrServicePackageLock;

	//能力封装路劲表
	static SHMData<ServicePackageRoute> *m_pSHMMgrServicePackageRouteData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageRouteindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageRouteSeqIndex;
	static ServicePackageRoute *m_pMgrServicePackageRouteData;
	static CSemaphore* m_pSHMMgrServicePackageRouteLock;

	//能力封装路劲CCR表
	static SHMData<CServicePackageRouteCCR> *m_pSHMMgrServicePackageRouteCCRData;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageRouteCCRSeqindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageRouteSeqindex;
	static CServicePackageRouteCCR *m_pMgrServicePackageRouteCCRData;
	static CSemaphore* m_pSHMMgrServicePackageRouteCCRLock;

	//能力封装判断条件
	static SHMData<CServicePackageJudge> *m_pSHMMgrServicePackageJudgeData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageJudgeindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageJudgeNodeSeqIndex;
	static CServicePackageJudge *m_pMgrServicePackageJudgeData;
	static CSemaphore* m_pSHMMgrServicePackageJudgeLock;

	//能力封装CCA
	static SHMData< CServicePackageCCA> *m_pSHMMgrServicePackageCCAData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageCCAindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvPackageCCASeqIndex;
	static  CServicePackageCCA *m_pMgrServicePackageCCAData;
	static CSemaphore* m_pSHMMgrServicePackageCCALock;

	//service_package_func
	static SHMData<CServicePackageFunc> *m_pSHMMgrServicePackageFuncData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageFuncindex;
	static  CServicePackageFunc *m_pMgrServicePackageFuncData;
	static CSemaphore* m_pSHMMgrServicePackageFuncLock;

	//service_package_variable
	static SHMData< CServicePackageVariable> *m_pSHMMgrServicePackageVariableData;
	static SHMIntHashIndex_A  *m_pSHMMgrServicePackageVariableindex;
	static   CServicePackageVariable *m_pMgrServicePackageVariableData;
	static CSemaphore* m_pSHMMgrServicePackageVariableLock;

	//service_package_variable
	static SHMData< StatisticsCongestMsg> *m_pSHMMgrSCongestMsgData;
	static SHMIntHashIndex_A  *m_pSHMMgrSCongestMsgindex;
	static   StatisticsCongestMsg *m_pMgrSCongestMsgData;
	static CSemaphore* m_pSHMMgrSCongestMsgLock;

	//wf_process_mq
	static SHMData< ST_WF_PROCESS_MQ> *m_pSHMMgrWfprocessMqData;
	static SHMIntHashIndex  *m_pSHMMgrWfprocessMqindex;
	static ST_WF_PROCESS_MQ *m_pMgrWfprocessMqData;
	static CSemaphore* m_pSHMMgrWfprocessMqLock;

	//PortInfo
	static SHMData< PortInfo> *m_pSHMMgrPortInfoData;
	static SHMIntHashIndex_A  *m_pSHMMgrPortInfoindex;
	static PortInfo *m_pMgrPortInfoData;
	static CSemaphore* m_pSHMMgrPortInfoLock;

	static SHMData<Sgw_Org_Route> *m_pSHMMgrOrgRouteData;
	static SHMIntHashIndex_A  *m_pSHMMgrOrgRouteindex;
	static Sgw_Org_Route *m_pMgrOrgRouteData;
	static CSemaphore* m_pSHMMgrOrgRouteLock;

	static SHMData<Sgw_Org_Head> *m_pSHMMgrOrgHeadData;
	static SHMIntHashIndex_A  *m_pSHMMgrOrgHeadindex;
	static Sgw_Org_Head *m_pMgrOrgHeadData;
	static CSemaphore* m_pSHMMgrOrgHeadLock;

	static SHMData<SgwChannelInfo> *m_pSHMMgrChannelData;
	static SHMIntHashIndex_A  *m_pSHMMgrChannelindex;
	static SgwChannelInfo *m_pMgrChannelData;
	static CSemaphore* m_pSHMMgrChannelLock;

	//static SHMData<SgwStaffInfo> *m_pSHMMgrStaffData;
	//static SHMIntHashIndex_A  *m_pSHMMgrStaffindex;
	//static SgwStaffInfo *m_pMgrStaffData;
	//static CSemaphore* m_pSHMMgrStaffLock;

	//static SHMData<SgwNetChannel> *m_pSHMMgrNetChannelData;
	//static SHMIntHashIndex_A  *m_pSHMMgrNetChannelNetIDindex;
	//static SHMIntHashIndex_A  *m_pSHMMgrNetChannelCodeindex;
	//static SHMIntHashIndex_A  *m_pSHMMgrNetChannelindex;
	//static SgwNetChannel *m_pMgrNetChannelData;
	//static CSemaphore* m_pSHMMgrNetChannelLock;

	//SGW_USER_STAFF
	static SHMData<SgwUserStaff> *m_pSHMMgrUserStaffData;
	static SHMIntHashIndex_A  *m_pSHMMgrUserStaffIDindex;
	static SHMIntHashIndex_A  *m_pSHMMgrStaffIDindex;
	static SHMIntHashIndex_A  *m_pSHMMgrUserStaffChannelIDindex;
	static SgwUserStaff *m_pMgrUserStaffData;
	static CSemaphore* m_pSHMMgrUserStaffLock;

	//SgwGlobalMap
	static SHMData< SgwGlobalMap> *m_pSHMMgrGlobalMapData;
	static SHMIntHashIndex_A  *m_pSHMMgrGlobalMapindex;
	static CSemaphore* m_pSHMMgrGlobalMapLock;
	static SgwGlobalMap *m_pMgrGlobalMapData;

	//sgw_route_rule
	static SHMData<SgwOrgRouteRule> *m_pSHMMgrOrgRoutRuleData;
	static SHMIntHashIndex_A  *m_pSHMMgrOrgRouteIDindex;
	static SHMIntHashIndex_A  *m_pSHMMgrOrgIDindex;
	static SgwOrgRouteRule *m_pMgrOrgRoutRuleData;
	static CSemaphore* m_pSHMMgrOrgRoutRuleLock;

	//sgw_area_org
	static SHMData<SgwAreaOrg> *m_pSHMMgrAreaOrgData;
	static SHMIntHashIndex_A  *m_pSHMMgrAreaOrgSeqindex;
	static SgwAreaOrg *m_pMgrAreaOrgData;
	static CSemaphore* m_pSHMMgrAreaOrgLock;

	//sgw_tuxedo_relation_in
	static SHMData<SgwTuxedoRelationIn> *m_pSHMMgrTuxRelaInData;
	static SHMIntHashIndex_A  *m_pSHMMgrTuxInSvrsIDindex;
	static SgwTuxedoRelationIn *m_pMgrTuxRelaInData;
	static CSemaphore* m_pSHMMgrTuxRelaInLock;

	//sgw_tuxedo_relation_out
	static SHMData<SgwTuxedoRelationOut> *m_pSHMMgrTuxRelaOutData;
	static SHMIntHashIndex_A  *m_pSHMMgrTuxOutSvrTxtIDindex;
	static SHMIntHashIndex_A  *m_pSHMMgrTuxOutRelaIDindex;
	static SgwTuxedoRelationOut *m_pMgrTuxRelaOutData;
	static CSemaphore* m_pSHMMgrTuxRelaOutLock;

	//sgw_services_list
	static SHMData<SgwServicesList> *m_pSHMMgrSrvsListData;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsListIDindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsListServicesNameindex;
	static SgwServicesList *m_pMgrSrvsListData;
	static CSemaphore* m_pSHMMgrSrvsListLock;

	//sgw_services_param_relation
	static SHMData<SgwServicesParamRelation> *m_pSHMMgrSrvsParamRelaData;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsParamRelaindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsParamRelaMappingIDIndex;
	static SgwServicesParamRelation *m_pMgrSrvsParamRelaData;
	static CSemaphore* m_pSHMMgrSrvsParamRelaLock;

	//sgw_services_param_list
	static SHMData<SgwServicesParamList> *m_pSHMMgrSrvsParamListData;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsParamListindex;
	static SgwServicesParamList *m_pMgrSrvsParamListData;
	static CSemaphore* m_pSHMMgrSrvsParamListLock;

	//sgw_services_param_list
	static SHMData<SgwServiceRegister> *m_pSHMMgrSrvsRegisterData;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsRegisterindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSrvsRegisterSvrInsIDIndex;
	static SgwServiceRegister *m_pMgrSrvsRegisterData;
	static CSemaphore* m_pSHMMgrSrvsRegisterLock;

	//sgw_services_param_list
	static SHMData<SgwParamInstanceList> *m_pSHMMgrParamInsListData;
	static SHMIntHashIndex_A  *m_pSHMMgrParamInsListindex;
	static SHMIntHashIndex_A  *m_pSHMMgrParamInsListMapIDindex;
	static SgwParamInstanceList *m_pMgrParamInsListData;
	static CSemaphore* m_pSHMMgrParamInsListLock;

	//sgw_enum_area
	static SHMData<SgwEnumArea> *m_pSHMMgrSgwEnumAreaData;
	static SHMIntHashIndex_A  *m_pSHMMgrSgwEnumAreaindex;
	static SHMIntHashIndex_A  *m_pSHMMgrSgwEnumAreaOrgIDindex;
	static SgwEnumArea *m_pMgrSgwEnumAreaData;
	static CSemaphore* m_pSHMMgrSgwEnumAreaLock;


private:
	static bool m_bAttached;
	static ThreeLogGroup m_oLogObj;
};

#endif /* SHMSGWINFOBASE_H_HEADER_INCLUDED_BDBB63C4 */
