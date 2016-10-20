#pragma once
#ifndef _SHMDataMgr_HEAD_
#define  _SHMDataMgr_HEAD_
#include "SgwDefine.h"
#include "ShmParamDef.h"
#include "CSemaphore.h"
#include "SHMSGWInfoBase.h"

//获取共享内存核心参数，X是局部变量,Y是全局变量
int  GETVALUE(int X,int Y);

class SHMDataMgr:public SHMSGWInfoBase
{
public:
	//////////表context_filter
	//创建共享内存数据区和索引区
	static int createContentFilteringSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadContentFilteringSHM();
	//数据加载
	static int loadContentFiltering();
	//清空共享内存数据
	static void clearContentFilteringSHM() {
		m_pSHMMgrContextData->clear();
		//m_pSHMMgrContextIndex->empty();
	};
	//断开与共享内存的连接
	static int detachContentFiltering() {
		m_pSHMMgrContextData->close();
		//m_pSHMMgrContextIndex->close();
	};
	//查询共享内存数据区信息
	static int queryContentFilteringData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	//static int queryContentFilteringIndex(SHMInfo &_oShmInfo);

	//////////table tab_msg_map
	//创建共享内存数据区和索引区
	static int createMsgMapSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateMsgMapSHM();
	//卸载共享内存
	static void unloadMsgMapSHM();
	//数据加载
	static int loadMsgMap();
	//清空共享内存数据
	static void clearMsgMapSHM() {
		m_pSHMMgrMsgMapInfoData->clear();
		m_pSHMMgrMsgMapInfoIndex->empty();
	};
	//断开与共享内存的连接
	static int detachMsgMap() {
		m_pSHMMgrMsgMapInfoData->close();
		m_pSHMMgrMsgMapInfoIndex->close();
	};
	//查询共享内存数据区信息
	static int queryMsgMapData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryMsgMapIndex(SHMInfo &_oShmInfo);

	/////////service_context_list
	//创建共享内存数据区和索引区
	static int createServiceTxtListSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateServiceTxtListSHM();
	//卸载共享内存
	static void unloadServiceTxtListSHM();
	//数据加载
	static int loadServiceTxtList();
	//清空共享内存数据
	static void clearServiceTxtListSHM() {
		m_pSHMMgrServiceTxtListData->clear();
		m_pSHMMgrServiceTxtListIndex->empty();
		m_pSHMMgrSrvTxtListIDIndex->empty();
	}
	//断开与共享内存的连接
	static int detachServiceTxtList() {
		m_pSHMMgrServiceTxtListData->close();
		m_pSHMMgrServiceTxtListIndex->close();
		m_pSHMMgrSrvTxtListIDIndex->close();
	};
	//查询共享内存数据区信息
	static int queryServiceTxtListData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryServiceTxtListIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySrvTxtListIDIndex(SHMInfo &_oShmInfo);

	//////////////表stream_ctrl_info
	//创建共享内存数据区和索引区
	static int createStreamCtrlSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadStreamCtrlSHM();
	//数据加载
	static int loadStreamCtrl();
	//清空共享内存数据
	static void clearStreamCtrlSHM() {
		m_pSHMMgrStreamCtrlInfoData->clear();
		m_pSHMMgrStreamCtrlInfoIndex->empty();
	};
	//断开与共享内存的连接
	static int detachStreamCtrl() {
		m_pSHMMgrStreamCtrlInfoData->close();
		m_pSHMMgrStreamCtrlInfoIndex->close();
	};
	//查询共享内存数据区信息
	static int queryStreamCtrlData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryStreamCtrlIndex(SHMInfo &_oShmInfo);

	///////////表stream_ctrl_data
	//创建共享内存数据区和索引区
	static int createStreamCtrlDataSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateStreamCtrlDataSHM();
	//卸载共享内存
	static void unloadStreamCtrlDataSHM();
	//加载数据到共享内存
	static int loadStreamCtrlData();
	//清空共享内存数据
	static void clearStreamCtrlDataSHM() {
		m_pSHMMgrStreamCtrlData->clear();
		m_pSHMMgrStreamCtrlDataIndex->empty();
	};
	//断开与共享内存的连接
	static int detachStreamCtrlData() {
		m_pSHMMgrStreamCtrlData->close();
		m_pSHMMgrStreamCtrlDataIndex->close();
	};
	//查询共享内存数据区信息
	static int queryStreamCtrlDataData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryStreamCtrlDataIndex(SHMInfo &_oShmInfo);

	/////////userinfo
	//创建共享内存数据区和索引区
	static int createUserInfoSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateUserInfoSHM();
	//卸载共享内存
	static void unloadUserInfoSHM();
	//数据加载
	static int loadUserInfo();
	//清空共享内存数据
	static void clearUserInfoSHM() {
		m_pSHMMgrUserInfoData->clear();
		m_pSHMMgrUserInfoIndex->empty();
		m_pSHMMgrUserInfoIndex_A->empty();
	}
	//断开与共享内存的连接
	static int detachUserInfo() {
		m_pSHMMgrUserInfoData->close();
		m_pSHMMgrUserInfoIndex->close();
		m_pSHMMgrUserInfoIndex_A->close();
	};
	//查询共享内存数据区信息
	static int queryUserInfoData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryUserInfoIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryUserInfoIndex_A(SHMInfo &_oShmInfo);

	//////////////table sgw_sla_que_rel
	//创建共享内存数据区和索引区
	static int createSgwSlaqueRelSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSgwSlaqueRelSHM();
	//数据加载
	static int loadSgwSlaqueRel();
	//清空共享内存数据
	static void clearSgwSlaqueRelSHM() {
		m_pSHMMgrSgwSlaqueRelData->clear();
		m_pSHMMgrSgwSlaqueRelIndex->empty();
	};
	//断开与共享内存的连接
	static int detachSgwSlaqueRel() {
		m_pSHMMgrSgwSlaqueRelData->close();
		m_pSHMMgrSgwSlaqueRelIndex->close();
	};
	//查询共享内存数据区信息
	static int querySgwSlaqueRelData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySgwSlaqueRelIndex(SHMInfo &_oShmInfo);

	////////////table netInfodata
	static int createNetSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateNetSHM();
	//卸载共享内存
	static void unloadNetSHM();
	//数据加载
	static int loadNet();
	//清空共享内存数据
	static void clearNetSHM() {
		m_pSHMMgrNetInfoData->clear();
		m_pSHMMgrNetInfoIndex->empty();
		m_pSHMMgrNetInfoIndex_S->empty();
	};
	//断开与共享内存的连接
	static int detachNet() {
		m_pSHMMgrNetInfoData->close();
		m_pSHMMgrNetInfoIndex->close();
		m_pSHMMgrNetInfoIndex_S->close();
	};
	//查询共享内存数据区信息
	static int queryNetData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryNetIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryNetIndex_s(SHMInfo &_oShmInfo);

	///////////////sessiondata
	//创建共享内存数据区和索引区
	static int createSessionSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSessionSHM();
	//清空共享内存数据
	static void clearSessionSHM() {
		m_pSHMMgrSessionData->clear();
		m_pSHMMgrSessionIndex->empty();
	};
	//断开与共享内存的连接
	static int detachSession() {
		m_pSHMMgrSessionData->close();
		m_pSHMMgrSessionIndex->close();
	};
	//查询共享内存数据区信息
	static int querySessionData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySessionIndex(SHMInfo &_oShmInfo);

	///////sessionclear
	static int createSessionClearSHM(int _iExtenSzie = 0);
	//卸载共享内存数据
	static void unloadSessionClearSHM();
	//清空共享内存数据
	static void clearSessionClearSHM() {
		m_pSHMMgrSessionClearData->clear();
		m_pSHMMgrSessionClearIndex->empty();
		m_pMgrSessionClearData = (SessionClearData *)(*m_pSHMMgrSessionClearData);
		m_pMgrSessionHeadSign = (SessionHead *)(char *)m_pMgrSessionClearData;
		m_pMgrSessionHeadSign[0].m_iFree = 1;//空闲头
		int iTotal = m_pSHMMgrSessionClearData->getTotal();
		for(int i=1; i<iTotal; i++){
			m_pMgrSessionClearData[i].m_iNext = i+1;
			m_pMgrSessionClearData[i].m_iIndexNext = 0;
		}

		//最后一个特殊处理
		m_pMgrSessionClearData[iTotal].m_iNext = 1;
		m_pMgrSessionClearData[iTotal].m_iIndexNext = 0;
		m_pMgrSessionHeadSign[0].m_iPreDeal =1;
	};
	//查询共享内存数据区信息
	static int querySessionClearData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySessionClearIndex(SHMInfo &_oShmInfo);

	//////////////table shmcongestlevel
	//创建共享内存数据区和索引区
	static int createCongestLeveSHM(int _iExtenSzie = 0);
	//更新共享内存
	//static int updateCongestLeveSHM();
	//卸载共享内存
	static void unloadCongestLeveSHM();
	//清空共享内存数据
	static void clearCongestLeveSHM() {
		m_pSHMMgrCongestLevelData->clear();
	};
	//断开与共享内存的连接
	static int detachCongestLeve() {
		m_pSHMMgrCongestLevelData->close();
	};
	//查询共享内存数据区信息
	static int queryCongestLeveData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryCongestLeveIndex(SHMInfo &_oShmInfo);

	///////////service_context_list_base
	//创建共享内存
	static int createServiceTxtListBaseSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadServiceTxtListBaseSHM();
	//数据加载
	static int loadServiceTxtListBase();
	//清空共享内存数据
	static void clearServiceTxtListBaseSHM() {
		m_pSHMMgrServiceTxtListBaseData->clear();
		m_pSHMMgrServiceTxtListBaseIndex->empty();
	};
	//断开与共享内存的连接
	static int detachServiceTxtListBase() {
		m_pSHMMgrServiceTxtListBaseData->close();
		m_pSHMMgrServiceTxtListBaseIndex->close();
	};
	//查询共享内存数据区信息
	static int queryServiceTxtListBaseData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryServiceTxtListBaseIndex(SHMInfo &_oShmInfo);

	//////////table LoadBalanceData
	//创建共享内存
	static int createloadbalanceSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadloadbalanceSHM();
	//读取核心参数
	static int loadBalance();
	//清空共享内存数据
	static void clearloadbalanceSHM() {
		m_pSHMMgrLoadBalanceData->clear();
		m_pSHMMgrLoadBalanceIndex->empty();
	};
	//断开与共享内存的连接
	static int detachloadbalance() {
		m_pSHMMgrLoadBalanceData->close();
		m_pSHMMgrLoadBalanceIndex->close();
	};
	//查询共享内存数据区信息
	static int queryloadbalanceData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryloadbalanceIndex(SHMInfo &_oShmInfo);

	//////////////table base_method_list
	static int createbasemethodSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadbasemethodSHM();
	//清空共享内存数据
	static void clearbasemethodSHM() {
		m_pSHMMgrBaseMethodData->clear();
		m_pSHMMgrBaseMethodindex->empty();
	};
	//数据加载
	static int loadbasemethod();
	//断开与共享内存的连接
	static int detachbasemethod() {
		m_pSHMMgrBaseMethodData->close();
		m_pSHMMgrBaseMethodindex->close();
	};
	//查询共享内存数据区信息
	static int querybasemethodData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querybasemethodIndex(SHMInfo &_oShmInfo);

	////////others CPacketSession
	static int createpacketsessionSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpacketsessionSHM();
	//清空共享内存数据
	static void clearpacketsessionSHM() {
		m_pSHMMgrPacketSessionData->clear();
		m_pSHMMgrPacketSessionindex->empty();
		m_pSHMMgrPacketSubSessionindex->empty();
	};
	//断开与共享内存的连接
	static int detachpacketsession() {
		m_pSHMMgrPacketSessionData->close();
		m_pSHMMgrPacketSessionindex->close();
		m_pSHMMgrPacketSubSessionindex->close();
	};
	//查询共享内存数据区信息
	static int querypacketsessionData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypacketsessionIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypacketsubsessionIndex(SHMInfo &_oShmInfo);

	////////table service_package_route
	static int createpackagerouteSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpackagerouteSHM();
	//清空共享内存数据
	static void clearpackagerouteSHM() {
		m_pSHMMgrServicePackageRouteData->clear();
		m_pSHMMgrServicePackageRouteindex->empty();
		m_pSHMMgrSrvPackageRouteSeqIndex->empty();
	};
	//断开与共享内存的连接
	static int detachpackageroute() {
		m_pSHMMgrServicePackageRouteData->close();
		m_pSHMMgrServicePackageRouteindex->close();
		m_pSHMMgrSrvPackageRouteSeqIndex->close();
	};
	//数据加载
	static int loadpackageroute();
	//查询共享内存数据区信息
	static int querypackagerouteData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypackagerouteIndex(SHMInfo &_oShmInfo);
	static int querypackagerouteIndex_a(SHMInfo &_oShmInfo);

	////////service_package_node
	static int createservicepackageSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadservicepackageSHM();
	//数据加载
	static int loadservicepackage();
	//清空共享内存数据
	static void clearservicepackageSHM() {
		m_pSHMMgrServicePackageData->clear();
		m_pSHMMgrServicePackageindex->empty();
		m_pSHMMgrSrvPackageNodeSeqIndex->empty();
	};
	//断开与共享内存的连接
	static int detachservicepackage() {
		m_pSHMMgrServicePackageData->close();
		m_pSHMMgrServicePackageindex->close();
		m_pSHMMgrSrvPackageNodeSeqIndex->close();
	};
	//查询共享内存数据区信息
	static int queryservicepackageData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryservicepackageIndex(SHMInfo &_oShmInfo);
	static int queryservicepackageIndex_a(SHMInfo &_oShmInfo);

	////////table service_package_route_ccr
	static int createservicepackageCCRSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadservicepackageCCRSHM();
	//数据加载
	static int loadservicepackageCCR();
	//清空共享内存数据
	static void clearservicepackageCCRSHM() {
		m_pSHMMgrServicePackageRouteCCRData->clear();
		m_pSHMMgrSrvPackageRouteCCRSeqindex->empty();
		m_pSHMMgrSrvPackageRouteSeqindex->empty();
	};
	//断开与共享内存的连接
	static int detachservicepackageCCR() {
		m_pSHMMgrServicePackageRouteCCRData->close();
		m_pSHMMgrSrvPackageRouteCCRSeqindex->close();
		m_pSHMMgrSrvPackageRouteSeqindex->close();
	};
	//查询共享内存数据区信息
	static int queryservicepackageCCRData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryservicepackageCCRIndex(SHMInfo &_oShmInfo);
	static int queryservicepackageCCRIndex_a(SHMInfo &_oShmInfo);

	////////table service_package_judge
	static int createpackagejudgeSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpackagejudgeSHM();
	//数据加载
	static int loadpackagejudge();
	//清空共享内存数据
	static void clearpackagejudgeSHM() {
		m_pSHMMgrServicePackageJudgeData->clear();
		m_pSHMMgrServicePackageJudgeindex->empty();
		m_pSHMMgrSrvPackageJudgeNodeSeqIndex->empty();
	};
	//断开与共享内存的连接
	static int detachpackagejudge() {
		m_pSHMMgrServicePackageJudgeData->close();
		m_pSHMMgrServicePackageJudgeindex->close();
		m_pSHMMgrSrvPackageJudgeNodeSeqIndex->close();
	};
	//查询共享内存数据区信息
	static int querypackagejudgeData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypackagejudgeIndex(SHMInfo &_oShmInfo);
	static int querypackagejudgeIndex_a(SHMInfo &_oShmInfo);

	////////table service_package_cca
	static int createpackageccaSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpackageccaSHM();
	//数据加载
	static int loadpackagecca();
	//清空共享内存数据
	static void clearpackageccaSHM() {
		m_pSHMMgrServicePackageCCAData->clear();
		m_pSHMMgrServicePackageCCAindex->empty();
		m_pSHMMgrSrvPackageCCASeqIndex->empty();
	};
	//断开与共享内存的连接
	static int detachpackagecca() {
		m_pSHMMgrServicePackageCCAData->close();
		m_pSHMMgrServicePackageCCAindex->close();
		m_pSHMMgrSrvPackageCCASeqIndex->close();
	};
	//查询共享内存数据区信息
	static int querypackageccaData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypackageccaIndex(SHMInfo &_oShmInfo);
	static int querypackageccaIndex_a(SHMInfo &_oShmInfo);

	////////table service_package_variable
	static int createpackagevleSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpackagevleSHM();
	//数据加载
	static int loadpackagevle();
	//清空共享内存数据
	static void clearpackagevleSHM() {
		m_pSHMMgrServicePackageVariableData->clear();
		m_pSHMMgrServicePackageVariableindex->empty();
	};
	//断开与共享内存的连接
	static int detachpackagevle() {
		m_pSHMMgrServicePackageVariableData->close();
		m_pSHMMgrServicePackageVariableindex->close();
	};
	//查询共享内存数据区信息
	static int querypackagevleData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypackagevleIndex(SHMInfo &_oShmInfo);

	////////table service_package_func
	static int createpackagefuncSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadpackagefuncSHM();
	//数据加载
	static int loadpackagefunc();
	//清空共享内存数据
	static void clearpackagefuncSHM() {
		m_pSHMMgrServicePackageFuncData->clear();
		m_pSHMMgrServicePackageFuncindex->empty();
	};
	//断开与共享内存的连接
	static int detachpackagefunc() {
		m_pSHMMgrServicePackageFuncData->close();
		m_pSHMMgrServicePackageFuncindex->close();
	};
	//查询共享内存数据区信息
	static int querypackagefuncData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querypackagefuncIndex(SHMInfo &_oShmInfo);

	////////共享内存消息统计
	static int createscongestmsgSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadscongestmsgSHM();
	//清空共享内存数据
	static void clearscongestmsgSHM() {
		m_pSHMMgrSCongestMsgData->clear();
		m_pSHMMgrSCongestMsgindex->empty();
	};
	//断开与共享内存的连接
	static int detachscongestmsg() {
		m_pSHMMgrSCongestMsgData->close();
		m_pSHMMgrSCongestMsgindex->close();
	};
	//查询共享内存数据区信息
	static int queryscongestmsgData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryscongestmsgIndex(SHMInfo &_oShmInfo);

	////////wf_process_mq
	static int createwfprocessmqSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadwfprocessmqSHM();
	//数据加载
	static int loadwfprocessmq();
	//清空共享内存数据
	static void clearwfprocessmqSHM() {
		m_pSHMMgrWfprocessMqData->clear();
		m_pSHMMgrWfprocessMqindex->empty();
	};
	//断开与共享内存的连接
	static int detachwfprocessmq() {
		m_pSHMMgrWfprocessMqData->close();
		m_pSHMMgrWfprocessMqindex->close();
	};
	//查询共享内存数据区信息
	static int querywfprocessmqData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querywfprocessmqIndex(SHMInfo &_oShmInfo);

	////////portinfo
	static int createportinfoSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadportinfoSHM();
	//清空共享内存数据
	static void clearportinfoSHM() {
		m_pSHMMgrPortInfoData->clear();
		m_pSHMMgrPortInfoindex->empty();
	};
	//断开与共享内存的连接
	static int detachportinfo() {
		m_pSHMMgrPortInfoData->close();
		m_pSHMMgrPortInfoindex->close();
	};
	//查询共享内存数据区信息
	static int queryportinfoData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryportinfoIndex(SHMInfo &_oShmInfo);

	////////sgw_org_route
	static int createOrgRouteSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadOrgRouteSHM();
	//数据加载
	static int loadOrgRouteData();
	//清空共享内存数据
	static void clearOrgRouteSHM() {
		m_pSHMMgrOrgRouteData->clear();
		m_pSHMMgrOrgRouteindex->empty();
	};
	//断开与共享内存的连接
	static int detachOrgRouteSHM() {
		m_pSHMMgrOrgRouteData->close();
		m_pSHMMgrOrgRouteindex->close();
	};
	//查询共享内存数据区信息
	static int queryOrgRouteData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryOrgRouteIndex(SHMInfo &_oShmInfo);

	////////sgw_org_head
	static int createOrgHeadSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadOrgHeadSHM();
	//清空共享内存数据
	static void clearOrgHeadSHM() {
		m_pSHMMgrOrgHeadData->clear();
		m_pSHMMgrOrgHeadindex->empty();
	};
	//断开与共享内存的连接
	static int detachOrgHeadSHM() {
		m_pSHMMgrOrgHeadData->close();
		m_pSHMMgrOrgHeadindex->close();
	};
	static int loadOrgHeadData();
	//查询共享内存数据区信息
	static int queryOrgHeadData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryOrgHeadIndex(SHMInfo &_oShmInfo);

	////////sgw_global_map
	static int createGlobalMapSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadGlobalMapSHM();
	//数据加载
	static int loadGlobalMapData();
	//清空共享内存数据
	static void clearGlobalMapSHM() {
		m_pSHMMgrGlobalMapData->clear();
		m_pSHMMgrGlobalMapindex->empty();
	};
	//断开与共享内存的连接
	static int detachGlobalMapSHM() {
		m_pSHMMgrGlobalMapData->close();
		m_pSHMMgrGlobalMapindex->close();
	};
	//查询共享内存数据区信息
	static int queryGlobalMapData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryGlobalMapIndex(SHMInfo &_oShmInfo);

	////////channel_info
	static int createChannelSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadChannelSHM();
	//数据加载
	static int loadChannelData();
	//清空共享内存数据
	static void clearChannelSHM() {
		m_pSHMMgrChannelData->clear();
		m_pSHMMgrChannelindex->empty();
	}
	//断开与共享内存的连接
	static int detachChannelSHM() {
		m_pSHMMgrChannelData->close();
		m_pSHMMgrChannelindex->close();
	};
	//查询共享内存数据区信息
	static int queryChannelData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryChannelIndex(SHMInfo &_oShmInfo);

	////////sgw_user_staff
	static int createUserStaffSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadUserStaffSHM();
	//数据加载
	static int loadUserStaffData();
	//清空共享内存数据
	static void clearUserStaffSHM() {
		m_pSHMMgrUserStaffData->clear();
		m_pSHMMgrUserStaffIDindex->empty();
		m_pSHMMgrStaffIDindex->empty();
		m_pSHMMgrUserStaffChannelIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachUserStaffSHM() {
		m_pSHMMgrUserStaffData->close();
		m_pSHMMgrUserStaffIDindex->close();
		m_pSHMMgrStaffIDindex->close();
		m_pSHMMgrUserStaffChannelIDindex->close();
	};
	//查询共享内存数据区信息
	static int queryUserStaffData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryUserStaffIndex_a(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryUserStaffIndex_b(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryUserStaffIndex_c(SHMInfo &_oShmInfo);

	////////sgw_route_rule
	static int createRouteRuleSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadRouteRuleSHM();
	//数据加载
	static int loadRouteRuleData();
	//清空共享内存数据
	static void clearRouteRuleSHM() {
		m_pSHMMgrOrgRoutRuleData->clear();
		m_pSHMMgrOrgRouteIDindex->empty();
		m_pSHMMgrOrgIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachRouteRuleSHM() {
		m_pSHMMgrOrgRoutRuleData->close();
		m_pSHMMgrOrgRouteIDindex->close();
		m_pSHMMgrOrgIDindex->close();
	};
	//查询共享内存数据区信息
	static int queryRouteRuleData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryRouteRuleIndex_a(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryRouteRuleIndex_b(SHMInfo &_oShmInfo);

	////////sgw_area_org
	static int createAreaOrgSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadAreaOrgSHM();
	//数据加载
	static int loadAreaOrgData();
	//清空共享内存数据
	static void clearAreaOrgSHM() {
		m_pSHMMgrAreaOrgData->clear();
		m_pSHMMgrAreaOrgSeqindex->empty();
	}
	//断开与共享内存的连接
	static int detachAreaOrgSHM() {
		m_pSHMMgrAreaOrgData->close();
		m_pSHMMgrAreaOrgSeqindex->close();
	};
	//查询共享内存数据区信息
	static int queryAreaOrgData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryAreaOrgIndex(SHMInfo &_oShmInfo);


	////////sgw_tuxedo_relation_in
	static int createTuxRelaInSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadTuxRelaInSHM();
	//数据加载
	static int loadTuxRelaInData();
	//清空共享内存数据
	static void clearTuxRelaInSHM() {
		m_pSHMMgrTuxRelaInData->clear();
		m_pSHMMgrTuxInSvrsIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachTuxRelaInSHM() {
		m_pSHMMgrTuxRelaInData->close();
		m_pSHMMgrTuxInSvrsIDindex->close();
	};
	//查询共享内存数据区信息
	static int queryTuxRelaInData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryTuxRelaInIndex(SHMInfo &_oShmInfo);


	////////sgw_tuxedo_relation_out
	static int createTuxRelaOutSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadTuxRelaOutSHM();
	//数据加载
	static int loadTuxRelaOutData();
	//清空共享内存数据
	static void clearTuxRelaOutSHM() {
		m_pSHMMgrTuxRelaOutData->clear();
		m_pSHMMgrTuxOutSvrTxtIDindex->empty();
		m_pSHMMgrTuxOutRelaIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachTuxRelaOutSHM() {
		m_pSHMMgrTuxRelaOutData->close();
		m_pSHMMgrTuxOutSvrTxtIDindex->close();
		m_pSHMMgrTuxOutRelaIDindex->close();
	};
	//查询共享内存数据区信息
	static int queryTuxRelaOutData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryTuxRelaOutIndex(SHMInfo &_oShmInfo);
	static int queryTuxRelaOutRelaIDIndex(SHMInfo &_oShmInfo);

	////////sgw_services_list
	static int createSvrListSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSvrListSHM();
	//数据加载
	static int loadSvrListData();
	//清空共享内存数据
	static void clearSvrListSHM() {
		m_pSHMMgrSrvsListData->clear();
		m_pSHMMgrSrvsListIDindex->empty();
		m_pSHMMgrSrvsListServicesNameindex->empty();
	}
	//断开与共享内存的连接
	static int detachSvrListSHM() {
		m_pSHMMgrSrvsListData->close();
		m_pSHMMgrSrvsListIDindex->close();
		m_pSHMMgrSrvsListServicesNameindex->close();
	};
	//查询共享内存数据区信息
	static int querySvrListData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySvrListIndex(SHMInfo &_oShmInfo);
	static int querySvrListServicesNameIndex(SHMInfo &_oShmInfo);


	////////sgw_services_param_relation
	static int createSvrsParamRelaSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSvrsParamRelaSHM();
	//数据加载
	static int loadSvrsParamRelaData();
	//清空共享内存数据
	static void clearSvrsParamRelaSHM() {
		m_pSHMMgrSrvsParamRelaData->clear();
		m_pSHMMgrSrvsParamRelaindex->empty();
		m_pSHMMgrSrvsParamRelaMappingIDIndex->empty();
	}
	//断开与共享内存的连接
	static int detachSvrsParamRelaSHM() {
		m_pSHMMgrSrvsParamRelaData->close();
		m_pSHMMgrSrvsParamRelaindex->close();
		m_pSHMMgrSrvsParamRelaMappingIDIndex->close();
	};
	//查询共享内存数据区信息
	static int querySvrsParamRelaData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySvrsParamRelaIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySvrsParamRelaMappingIDIndex(SHMInfo &_oShmInfo);

	////////sgw_services_param_list
	static int createSvrsParamListSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSvrsParamListSHM();
	//数据加载
	static int loadSvrsParamListData();
	//清空共享内存数据
	static void clearSvrsParamListSHM() {
		m_pSHMMgrSrvsParamListData->clear();
		m_pSHMMgrSrvsParamListindex->empty();
	}
	//断开与共享内存的连接
	static void detachSvrsParamListSHM() {
		m_pSHMMgrSrvsParamListData->close();
		m_pSHMMgrSrvsParamListindex->close();
	}
	//查询共享内存数据区信息
	static int querySvrsParamListData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySvrsParamListIndex(SHMInfo &_oShmInfo);

	////////sgw_service_register
	static int createSvrsRegisterSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSvrsRegisterSHM();
	//数据加载
	static int loadSvrsRegisterData();
	//清空共享内存数据
	static void clearSvrsRegisterSHM() {
		m_pSHMMgrSrvsRegisterData->clear();
		m_pSHMMgrSrvsRegisterindex->empty();
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->empty();
	}
	//断开与共享内存的连接
	static int detachSvrsRegisterSHM() {
		m_pSHMMgrSrvsRegisterData->close();
		m_pSHMMgrSrvsRegisterindex->close();
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->close();
	};
	//查询共享内存数据区信息
	static int querySvrsRegisterData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySvrsRegisterIndex(SHMInfo &_oShmInfo);
	static int querySvrsRegisterInsIDIndex(SHMInfo &_oShmInfo);


	////////sgw_param_instance_list
	static int createParamInsListSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadParamInsListSHM();
	//数据加载
	static int loadParamInsListData();
	//清空共享内存数据
	static void clearParamInsListSHM() {
		m_pSHMMgrParamInsListData->clear();
		m_pSHMMgrParamInsListindex->empty();
		m_pSHMMgrParamInsListMapIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachParamInsListSHM() {
		m_pSHMMgrParamInsListData->close();
		m_pSHMMgrParamInsListindex->close();
		m_pSHMMgrParamInsListMapIDindex->close();
	};
	//查询共享内存数据区信息
	static int queryParamInsListData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryParamInsListIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int queryParamInsListMapIDIndex(SHMInfo &_oShmInfo);

	////////sgw_enum_area
	static int createSgwEnumAreaSHM(int _iExtenSzie = 0);
	//卸载共享内存
	static void unloadSgwEnumAreaSHM();
	//数据加载
	static int loadSgwEnumAreaData();
	//清空共享内存数据
	static void clearSgwEnumAreaSHM() {
		m_pSHMMgrSgwEnumAreaData->clear();
		m_pSHMMgrSgwEnumAreaindex->empty();
		m_pSHMMgrSgwEnumAreaOrgIDindex->empty();
	}
	//断开与共享内存的连接
	static int detachSgwEnumAreaSHM() {
		m_pSHMMgrSgwEnumAreaData->close();
		m_pSHMMgrSgwEnumAreaindex->close();
		m_pSHMMgrSgwEnumAreaOrgIDindex->close();
	};
	//查询共享内存数据区信息
	static int querySgwEnumAreaData(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySgwEnumAreaIndex(SHMInfo &_oShmInfo);
	//查询共享内存索引区信息
	static int querySgwEnumAreaOrgIDIndex(SHMInfo &_oShmInfo);

public:
	//将输入的key按照一定的规则转换
	static long changekey(const char *_skey,int _iModValue);

	static unsigned long trim(const char *sHead);

};

#endif
