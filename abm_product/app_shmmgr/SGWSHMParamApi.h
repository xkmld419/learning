#pragma once
#ifndef _SHMAPI_HEAD_
#define _SHMAPI_HEAD_
#include <vector>
#include "SHMSGWInfoBase.h"
#include "CSemaphore.h"
#include "SgwDefine.h"
#include "ShmParamDef.h"
#include "SHMDataMgr.h"
using namespace std;

//context_filter表加载到共享内存
//提供内容过滤模块的共享内存接口
class SHMContentFiltering:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool contentFilter_P(bool bWait = true){
		m_pSHMMgrContextDataLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool contentFilter_V(){
		m_pSHMMgrContextDataLock->V();
	};

	//返回值 0 成功 非0失败
	static int GetContextFilterInfo(int _iFilterSeq, vector<ContextData> & vsContextInfo);

	//返回值 0 成功 非0失败
	static int AddContextFilterInfo(const ContextData & osContextInfo);

	//返回值 0 成功 非0失败
	static int DelContextFilterInfo(const ContextData & osContextInfo);

	//返回值 0 成功 非0失败
	static int UpdateContextFilterInfo(const ContextData & osContextInfo);

	//返回值 0 成功 非0失败
	static int GetContextFilterInfoAll(vector<ContextData> & vsContextInfo);
};

//TAB_MSG_MAP表加载到共享内存
//提供外界业务进程操作接口
class SHMMsgMapInfo:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool msgmap_P(bool bWait = true){
		m_pSHMMgrMsgMapInfoLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool msgmap_V(){
		m_pSHMMgrMsgMapInfoLock->V();
	};

	//提供输入查询，返回数据	返回值 0 成功 非0失败
	static int GetMsgMapInfo(vector<SMsgMapInfo> & vsMsgMapInfo);
	//增加接口按照主键查询
	static	int GetMsgMapInfo(int _iID ,SMsgMapInfo & vsMsgMapInfo);
	static int AddMsgMapInfo(SMsgMapInfo & vsMsgMapInfo);
	static int DelMsgMapInfo(SMsgMapInfo & vsMsgMapInfo);
	static int UpdateMsgMapInfo(SMsgMapInfo & vsMsgMapInfo);

};

//sgw_global_map表加载到共享内存
//提供外界业务进程操作接口
class SHMGlobalMap:public SHMSGWInfoBase
{
public:
	//提供输入查询，返回数据	返回值 0 成功 非0失败
	static int GetGlobalMapALL(vector<SgwGlobalMap>&vGlobalMap);
	//增加接口
	//按照主键查询
	static int GetGlobalMap(int _iGID, SgwGlobalMap &sGlobalMap);
	static int AddGlobalMap(SgwGlobalMap &sGlobalMap);
	static int DelGlobalMap( SgwGlobalMap &sGlobalMap);
	static int UpdateGlobalMap( SgwGlobalMap &sGlobalMap);

};


//service_context_list表加载到共享内存
//提供外界业务进程操作接口
class SHMServiceTxtList:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicetxtlist_P(bool bWait = true){
		m_pSHMMgrServiceTxtListLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicetxtlist_V(){
		m_pSHMMgrServiceTxtListLock->V();
	};

	//返回值 0 成功 非0失败
	static int GetServiceContextListAll(vector<ServiceContextListData> & vServiceContextListData);

	//按照主键查询
	static	int GetServiceContextList(int _iServiceContextListId,ServiceContextListData &sServiceContextListData);

	static int GetServiceContextList(const char *_strServiceContextId, vector<ServiceContextListData>& vServiceContextListData);

	//通过_iUserSeq 查找m_strServiceContextId，返回值 0 成功 非0失败
	static int GetServiceContextID(int _iUserSeq,vector<ServiceContextListData> &vServiceContextListData);

	//通过_iUserSeq 查找m_strServiceContextId，返回值 0 成功 非0失败
	static int GetServiceContextID(int _iUserSeq,const char *_strServiceContextId,ServiceContextListData &sServiceContextListData);

	//返回值 0 成功 非0失败
	static int DelServiceContextData(ServiceContextListData sServiceContextListData);

	//返回值 0 成功 非0失败
	static int UpdateServiceContextdata(ServiceContextListData sServiceContextListData);

	//返回值 0 成功 非0失败
	static int AddServiceContextData(ServiceContextListData sServiceContextListData);
};


//STREAM_CTRL_INFO表加载到共享内存
//提供业务进程操作接口
//m_sUserName和字段m_sSerConId组成联合索引
class SHMStreamCtrlInfo:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool streamctl_P(bool bWait = true){
		m_pSHMMgrStreamCtrlInfoLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool streamctl_V(){
		m_pSHMMgrStreamCtrlInfoLock->V();
	};

	//根据m_sUserName和m_sSerConId为条件进行查找；返回值 0 成功 非0失败
	static int GetStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo);
	//全量查询
	static int GetStreamCtrlInfoAll(vector<StreamControlInfo> &tStreamCtrlInfo);
	//根据m_sUserName和m_sSerConId为条件进行删除；返回值 0 成功 非0失败
	static int DelStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo);

	//根据m_sUserName和m_sSerConId为条件进行修改；返回值 0 成功 非0失败
	static int UpdateStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo);

	//返回值 0 成功 非0失败
	static int AddStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo);
};

//该共享内存提供在线业务进程操作接口，无表名。
//提供业务进程操作接口
class SHMStreamCtrlData:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool streamctldata_P(bool bWait = true){
		m_pSHMMgrStreamCtrlDataLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool streamctldata_V(){
		m_pSHMMgrStreamCtrlDataLock->V();
	};

	static int GetStreamCtrlDataAll(vector<StreamControlData> &vStreamCtrlData);

	//根据m_sUserName和m_sSerConId为条件进行查找；返回值 0 成功 非0失败
	static int GetStreamCtrlData(StreamControlData &tStreamCtrlData);

	//根据m_sUserName和m_sSerConId为条件进行删除；返回值 0 成功 非0失败
	static int DelStreamCtrlData(StreamControlData &tStreamCtrlData);

	//根据m_sUserName和m_sSerConId为条件进行修改；返回值 0 成功 非0失败
	static int UpdateStreamCtrlData(StreamControlData &tStreamCtrlData);

	//返回值 0 成功 非0失败
	static int AddStreamCtrlData(StreamControlData &tStreamCtrlData);

	//按照主键查询
	static int GetStreamCtrlData(const char *sUserName,const char * sSerConId , long lMonth, StreamControlData &tStreamCtrlData);

};

//UserInfo表加载到共享内存
//提供业务进程操作接口
class SHMUserInfoData:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool userinfo_P(bool bWait = true){
		m_pSHMMgrUserInfoDataLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool userinfo_V(){
		m_pSHMMgrUserInfoDataLock->V();
	};

	//查询接口，成功返回0，失败返回非0
	static int GetUserInfoData(const char *_sUserName,UserInfoData & vsUserInfoData);

	static int GetUserInfoDataAll(vector<UserInfoData> & vsUserInfoData);

	static int AddUserInfoData(UserInfoData & tUserInfoData);

	static int UpdateUserInfoData(UserInfoData & tUserInfoData);

	static int GetUserInfoData(int iUserSeq,UserInfoData & vsUserInfoData);

	//根据USERSEQ来删除
	static int DelUserInfoData(const UserInfoData & vsUserInfoData);

	//根据user_name来删除
	static int DeleteUserInfoData(const UserInfoData & vsUserInfoData);

};

//SGW_SLA_QUE_REL表加载到共享内存
//提供业务进程操作接口
class SHMSgwSlaqueRel:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool sgwslaquerel_P(bool bWait = true){
		m_pSHMMgrSgwSlaqueRelLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool sgwslaquerel_V(){
		m_pSHMMgrSgwSlaqueRelLock->V();
	};

	//查询接口，成功返回0，失败返回非0
	static int GetSlaQueueAllData(vector<SgwSlaQueue> & vSgwSlaQueue);
	//返回值 0 成功 非0失败
	static	int GetSlaQueueData(SgwSlaQueue & sSgwSlaQueue);
	//返回值 0 成功 非0失败
	static	int DelSlaQueueData(SgwSlaQueue sSgwSlaQueue);
	//返回值 0 成功 非0失败
	static	int UpdateSlaQueueData(SgwSlaQueue sSgwSlaQueue);
	//返回值 0 成功 非0失败
	static	int AddSlaQueueData(SgwSlaQueue sSgwSlaQueue);
};

//NetInfoData表加载到共享内存
//提供业务进程操作接口
class SHMNetInfoData:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool netinfo_P(bool bWait = true){
		m_pSHMMgrNetInfoLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool netinfo_V(){
		m_pSHMMgrNetInfoLock->V();
	};

	//提供输入以及返回数据
	static int GetNetInfoAllData(vector<NetInfoData> & vNetInfoData);

	//返回值 0 成功 非0失败
	static int GetNetInfoData(NetInfoData &sNetInfoData);

	static int GetNetInfoData(const char *_strNetinfoName,NetInfoData &oNetInfoData);

	//返回值 0 成功 非0失败
	static int AddNetInfoData(const NetInfoData &sNetInfoData);

	static int UpdateNetInfoData(const NetInfoData &sNetInfoData);

	//增加接口
	//按照主键查询
	static	int DelNetInfoData(const NetInfoData &sNetInfoData);

};

//SessionData表加载到共享内存
//提供业务进程操作接口
class SHMSessionData:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool session_P(bool bWait = true){
		m_pSHMMgrSessionLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool session_V(){
		m_pSHMMgrSessionLock->V();
	};

	//提供输入以及返回数据
	static int GetSessionData(SessionData &tSessionData);

	static int DelSessionData(SessionData &tSessionData,bool _bCopy = false);

	static int UpdateSessionData(SessionData &tSessionData);

	static int AddSessionData(SessionData &tSessionData);

	//按下标查询数据
	static int GetSessionDataALL(vector<SessionData> &vSessionData);
};

//提供业务进程操作接口
class SHMCongestLevel:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool congestlevel_P(bool bWait = true){
		m_pSHMMgrCongestLevelLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool congestlevel_V(){
		m_pSHMMgrCongestLevelLock->V();
	};

	static int GetShmCongestLevel(CongestLevel & sShmCongestLevel);

	static int UpdateShmCongestLevel(CongestLevel sShmCongestLevel);

	static int AddShmCongestLevel(const CongestLevel & sShmCongestLevel);
};

//表service_context_list_base 提供查询和更新共享内存的接口
class SHMServiceTxtListBase:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicetxtlistbase_P(bool bWait = true){
		m_pSHMMgrServiceTxtListBaseLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicetxtlistbase_V(){
		m_pSHMMgrServiceTxtListBaseLock->V();
	};

	//根据主键service_context_id查询需要的字段
	static int getServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData);

	//根据主键service_context_id更新共享内存信息
	static int updateServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData);

	//根据主键service_context_id查询需要的字段
	static int getServiceTxtListBaseAll(vector<ServiceContextListBaseData> &vTxtBaseData);

	//按照主键增加和删除
	static int AddServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData);

	static int  DelServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData);


};

//提供业务访问接口
class SHMLoadBalance:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool loadbalance_P(bool bWait = true){
		m_pSHMMgrLoadBalanceLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool loadbalance_V(){
		m_pSHMMgrLoadBalanceLock->V();
	};

	//根据主键service_context_id查询需要的字段
	static int getLoadbalance(LoadBalanceData &sLodbalance);

	//根据主键service_context_id更新共享内存信息
	static int updateLoadbalance(LoadBalanceData &sLodbalance);

	//根据主键service_context_id查询需要的字段
	static int getLoadbalanceAll(vector<LoadBalanceData> &vLodbalance);
};


//提供base_method_list
class SHMBaseMethod:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool basemethod_P(bool bWait = true){
		m_pSHMMgrBaseMethodLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool basemethod_V(){
		m_pSHMMgrBaseMethodLock->V();
	};

	//返回值 0 成功 非0失败
	static int getBaseMethod(int m_iBaseMethodID, CBaseMethod & sBaseMethod);
	//返回值 0 成功 非0失败
	static int deleteBaseMethod(const CBaseMethod & sBaseMethod);
	//返回值 0 成功 非0失败
	static int updateBaseMethod(const CBaseMethod & sBaseMethod);
	//返回值 0 成功 非0失败
	static int addBaseMethod(const CBaseMethod & sBaseMethod);
	//返回值 0 成功 非0失败
	static int getBaseMethodAll(vector<CBaseMethod> & vBaseMethod);
};

class SHMPacketSession:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool packetsession_P(bool bWait = true){
		m_pSHMMgrPacketSessionLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool packetsession_V(){
		m_pSHMMgrPacketSessionLock->V();
	};

	//返回值 0 成功 非0失败
	//--取主能力下所有会话
	static int getPacketSessionAll(const char * psSessionID, vector<SPacketSession> &sPackageSession);
	//--取单个子能力会话
	static int getPacketSession(const char * psChildSessionID, SPacketSession &sPackageSession);
	//返回值 0 成功 非0失败
	static int deletePacketSession(const SPacketSession &sPackageSession);
	//返回值 0 成功 非0失败
	static int updatePacketSession(const SPacketSession &sPackageSession);
	//返回值 0 成功 非0失败
	static int addPacketSession(const SPacketSession &sPackageSession);

};

class SHMServicePackage:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicepackage_P(bool bWait = true){
		m_pSHMMgrServicePackageLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicepackage_V(){
		m_pSHMMgrServicePackageLock->V();
	};

	static int getServicePackageNode(int iNodeSeq, ServicePackageNode & vServicePackageNode); // ---查询单个节点
	static int getServicePackageNodeALL(const char * psServiceContextId, vector<ServicePackageNode> & vServicePackageNode);//---查询某个能力下所有节点
	//返回值 0 成功 非0失败
	static int deleteServicePackageNode(const ServicePackageNode & sServicePackageNode);
	//返回值 0 成功 非0失败
	static int updateServicePackageNode(const ServicePackageNode & sServicePackageNode);
	//返回值 0 成功 非0失败
	static int addServicePackageNode(const ServicePackageNode & sServicePackageNode);
};

class SHMServicePackageRoute:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool packageroute_P(bool bWait = true){
		m_pSHMMgrServicePackageRouteLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool packageroute_V(){
		m_pSHMMgrServicePackageRouteLock->V();
	};

	static int getServicePackageRoute(int iRouteSeq, ServicePackageRoute & SPackageRoute);  //---查询单个路径
	static int getServicePackageRouteALL(const char * psServicePackageId, vector<ServicePackageRoute> & vSPackageRoute);//---查询某个能力下所有路径
	//返回值 0 成功 非0失败
	static int deleteServicePackageRoute(const ServicePackageRoute & sServicePackageRoute);
	//返回值 0 成功 非0失败
	static int updateServicePackageRoute(const ServicePackageRoute & sServicePackageRoute);
	//返回值 0 成功 非0失败
	static int addServicePackageRoute(const ServicePackageRoute & sServicePackageRoute);

};

class SHMServicePackageRouteCCR:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool packagerouteccr_P(bool bWait = true){
		m_pSHMMgrServicePackageRouteCCRLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool packagerouteccr_V(){
		m_pSHMMgrServicePackageRouteCCRLock->V();
	};

	static int getServicePackageRouteCCR(int iRouteCCRSeq, CServicePackageRouteCCR & sServicePackageRouteCCR);//--查询当个AVP
	static int getServicePackageRouteCCR(int iRouteSeq, vector<CServicePackageRouteCCR> & vServicePackageRouteCCR);//--查询某个能力下的所有AVP
	//返回值 0 成功 非0失败
	static int deleteServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR);
	//返回值 0 成功 非0失败
	static int updateServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR);
	//返回值 0 成功 非0失败
	static int addServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR);


};

class SHMServicePackageJudge:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool packagejudge_P(bool bWait = true){
		m_pSHMMgrServicePackageJudgeLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool packagejudge_V(){
		m_pSHMMgrServicePackageJudgeLock->V();
	};
	//返回值 0 成功 非0失败
	static int getServicePackageJudge(int iJudgeConditionSeq, CServicePackageJudge & sServicePackageJudge);//--查询当个条件
	static int getServicePackageJudgeAll(int iNodeSeq, vector<CServicePackageJudge> & vServicePackageJudge);//--查询某个节点下的所有条件
	//返回值 0 成功 非0失败

	static int deleteServicePackageJudge(const CServicePackageJudge & sServicePackageJudge);
	//返回值 0 成功 非0失败
	static int updateServicePackageJudge(const CServicePackageJudge & sServicePackageJudge);
	//返回值 0 成功 非0失败
	static int addServicePackageJudge(const CServicePackageJudge & sServicePackageJudge);
};

class SHMServicePackageCCA:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicepackageCCA_P(bool bWait = true){
		m_pSHMMgrServicePackageCCALock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicepackageCCA_V(){
		m_pSHMMgrServicePackageCCALock->V();
	};

	static int getServicePackageCCA(int  iServicePackageCCASeq,CServicePackageCCA & sServicePackageCCA);//--查询当个AVP
	static int getServicePackageCCA(const char * psServiceContextId,vector<CServicePackageCCA> & vServicePackageCCA);//--查询某个能力下的所有AVP
	//按照主键查询
	static	int AddServicePackageCCA(CServicePackageCCA & sServicePackageCCA);
	static int DelServicePackageCCA(CServicePackageCCA & sServicePackageCCA);
	static int UpdateServicePackageCCA(CServicePackageCCA & sServicePackageCCA);

};

class SHMServicePackageFunc:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicepackagefunc_P(bool bWait = true){
		m_pSHMMgrServicePackageFuncLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicepackagefunc_V(){
		m_pSHMMgrServicePackageFuncLock->V();
	};

	// 按主键索引查询一个函数调用信息 0成功 非0失败返回码
	static int GetServicePackageFunc(int iCallID,CServicePackageFunc & sServicePackageFunc);
	//增加接口 按照主键查询
	static	int AddServicePackageFunc(CServicePackageFunc & sServicePackageFunc);
	static int DelServicePackageFunc(CServicePackageFunc & sServicePackageFunc);
	static int UpdateServicePackageFunc(CServicePackageFunc & sServicePackageFunc);


};

class SHMServicePackageVle:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool servicepackagevle_P(bool bWait = true){
		m_pSHMMgrServicePackageVariableLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool servicepackagevle_V(){
		m_pSHMMgrServicePackageVariableLock->V();
	};

	// 按主键索引查询一个函数调用信息 0成功 非0失败返回码
	static int GetServicePackageVariable (int iVariableID, CServicePackageVariable & sServicePackageVariable);
	//返回值 0 成功 非0失败
	static int DelServicePackageVariable(CServicePackageVariable & sServicePackageVariable);
	//返回值 0 成功 非0失败
	static int UpdateServicePackageVariable(CServicePackageVariable & sServicePackageVariable);
	static int AddServicePackageVariable(CServicePackageVariable & sServicePackageVariable);
};

class SHMSCongestMsg:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool scongestmsg_P(bool bWait = true){
		m_pSHMMgrSCongestMsgLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool scongestmsg_V(){
		m_pSHMMgrSCongestMsgLock->V();
	};

	//4.提供输入以及返回数据
	static int GetAllStatisticsCongestMsg(vector<StatisticsCongestMsg> &vStatisticsCongestMsg);//取所有信息

	//返回值 0 成功 非0失败
	static int GetStatisticsCongestMsg(const char* m_strServiceContextId,StatisticsCongestMsg &sStatisticsCongestMsg);

	//返回值 0 成功 非0失败
	static int InsertStatisticsCongestMsg(StatisticsCongestMsg sStatisticsCongestMsg);

	////返回值 0 成功 非0失败
	static int UpdateStatisticsCongestInMsg(const char* m_strServiceContextId,int num=1);  //更新 入消息数+1

	//返回值 0 成功 非0失败
	static int UpdateStatisticsCongestOutMsg(const char* m_strServiceContextId,int num=1);  //更新 出消息数+1

	//返回值 0 成功 非0失败
	static int UpdateStatisticsCongestDiscardMsg(const char* m_strServiceContextId,int num=1);  //更新 丢弃消息数+1

};

class SHMWfProcessMq:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool wfprocessmq_P(bool bWait = true){
		m_pSHMMgrWfprocessMqLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool wfprocessmq_V(){
		m_pSHMMgrWfprocessMqLock->V();
	};

	//返回值 0 成功 非0失败
	static int getAllStWfProcessMq(vector<ST_WF_PROCESS_MQ> & vStWfProcessMq);
	//返回值 0 成功 非0失败
	static int getStWfProcessMq(ST_WF_PROCESS_MQ &sStWfProcessMq);
	//返回值 0 成功 非0失败
	static int addStWfProcessMq(const  ST_WF_PROCESS_MQ &sStWfProcessMq);
};

class SHMPortInfo:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool portinfo_P(bool bWait = true){
		m_pSHMMgrPortInfoLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool portinfo_V(){
		m_pSHMMgrPortInfoLock->V();
	};

	//返回值 0 成功 非0失败
	static int getPortInfo(int m_iPortID,PortInfo &oPortInfo);
	//返回值 0 成功 非0失败
	static int updatePortinfo(const PortInfo &oPortInfo);
	//返回值 0 成功 非0失败
	static int addPortinfo(const PortInfo &oPortInfo);

	static int delPortInfo(const PortInfo &oPortInfo);
	static int getPortInfoAll(vector<PortInfo> &vPortInfo);

	static int clearPortInfo();
};

class SHMSessionClear:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool sessionClear_P(bool bWait = true){
		m_pSHMMgrSessionClearLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool sessionClear_V(){
		m_pSHMMgrSessionClearLock->V();
	};

	//返回值 0 成功 非0失败
	static int getSessionClear(SessionClearData &oSessionClear);
	//返回值 0 成功 非0失败
	static void addSessionClear(const SessionClearData &oSessionClear);

	static int delSessionClear(const SessionClearData &oSessionClear);

	static int getSessionClearAll(vector<SessionClearData> &vSessionClear,long _lTime);

	static int setPreDeal(int value) {
		m_pMgrSessionHeadSign[0].m_iPreDeal = value;
	}

	static int getPreDeal() {
		return m_pMgrSessionHeadSign[0].m_iPreDeal;
	}

	static int setFreePos(int value) {
		m_pMgrSessionHeadSign[0].m_iFree = value;
	}

	static int getFreePos() {
		return m_pMgrSessionHeadSign[0].m_iFree;
	}
};

class SHMOrgRoute:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool orgRoute_P(bool bWait = true){
		m_pSHMMgrOrgRouteLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool orgRoute_V(){
		m_pSHMMgrOrgRouteLock->V();
	};

	//通过输入org_id,org_level_id;返回给相应org_id所对应的父节点org_id并且org_level_id为输入的相应级别。
	static int GetRouteOrgID(int src_org_id,int org_level_id,int &descOrgID );

	//支持输入org_area_code,org_level_id,返回org_area_code所对应的相应的级别的org_id;
	static int GetRouteOrgIDUserCode(char *org_area_code,int org_level_id,int &descOrgID );

	//支持按org_id 更新数据（新增、修改）
	static int UpdateRouteOrg(int org_id,int org_level_id,int parent_org_id,char *org_area_code); //如果原来有的数据做修改，如果无数据，即做新增处理

	//支持导出数据。
	//表的字段按org表的字段建立，支持把内存数据导入到数据表中。以便对比数据是否与上载数据一致。
	static int ExpRouteOrg(char *sTable_name);

};

class SHMOrgHead:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool orghead_P(bool bWait = true){
		m_pSHMMgrOrgHeadLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool orghead_V(){
		m_pSHMMgrOrgHeadLock->V();
	};

	//功能要求，输入一相应的号码，返回相应有效数据的org_id;
	static int GetOrgIDbyNbr(char *AccNbr,int &Org_Id);

	//支持按head 更新数据（新增、修改）
	static int UpdateAccNbrHead(int org_id,char *head,char *eff_date,char *exp_date,char *table_name); //如果原来有的数据做修改，如果无数据，即做新增处理

	//增加导出数据
	static int ExpAccNbrHead(char *sTable_name);//表的字段按b_head_region表的字段建立，并增加一个table_name的标示字段，字段类型为varchar2(20)；支持把内存数据导入到数据表中。以便对比数据是否与上载数据一致

};

class SHMChannelInfo:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool ChannelInfo_P(bool bWait = true){
		m_pSHMMgrChannelLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool ChannelInfo_V(){
		m_pSHMMgrChannelLock->V();
	};

	//按渠道编号找渠道信息
	static int GetChannelInfo(char * pszChannelID,SgwChannelInfo & sSgwChannelInfo);
	//按渠道编号更新渠道信息
	static int UpdateChannelInfo(char * pszChannelID,SgwChannelInfo & sSgwChannelInfo);

	static int AddChannelInfo(const SgwChannelInfo & sSgwChannelInfo);

	static int DeleteChannelInfo(char * pszChannelID);

};

//class SHMStaffInfo:public SHMSGWInfoBase
//{
//public:
//	//加锁 成功返回true，失败返回false
//	static bool StaffInfo_P(bool bWait = true){
//		m_pSHMMgrStaffLock->P(bWait);
//	};
//
//	//解锁，成功返回truce，失败返回false
//	static bool StaffInfo_V(){
//		m_pSHMMgrStaffLock->V();
//	};
//
//	//按工号取渠道
//	static int GetStaffInfo(long lStaffID,SgwStaffInfo& sSgwStaffInfo);
//	//按工号更新渠道
//	static int UpdateStaffInfo(long lStaffID,SgwStaffInfo& sSgwStaffInfo);
//
//	static int AddStaffInfo(const SgwStaffInfo& sSgwStaffInfo);
//
//	static int DeleteStaffInfo(long lStaffID);
//
//};

//class SHMNetChannelInfo:public SHMSGWInfoBase
//{
//public:
//	//加锁 成功返回true，失败返回false
//	static bool NetChannel_P(bool bWait = true){
//		m_pSHMMgrNetChannelLock->P(bWait);
//	};
//
//	//解锁，成功返回truce，失败返回false
//	static bool NetChannel_V(){
//		m_pSHMMgrNetChannelLock->V();
//	};
//
//	//按网元编码查询渠道
//	static int GetNetChannelByNet(char * pszNetInfoCode,SgwNetChannel& sSgwNetChannel);
//	//按渠道查询网元编码
//	static int GetNetChannelByChannel(char * pszChannel,SgwNetChannel& sSgwNetChannel);
//	//按主键更新数据
//	static int UpdateNetChannel(int iChannelNetID,SgwNetChannel& sSgwNetChannel);
//
//	static int GetNetChannel(int iChannelNetID,SgwNetChannel& sSgwNetChannel);
//
//	static int AddNetChannel(const SgwNetChannel& sSgwNetChannel);
//	static int DeleteNetChannel(int iChannelNetID);
//};

class SHMUserStaff:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool UserStaff_P(bool bWait = true){
		m_pSHMMgrUserStaffLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool UserStaff_V(){
		m_pSHMMgrUserStaffLock->V();
	};

	//按用户工号
	static int GetUserStaffByUserStarr(int iUserStaffID,SgwUserStaff& sSgwUserStaff);
	//按工号查信息
	static	int GetUserStaffByStarr(int iStaffID,SgwUserStaff& sSgwUserStaff);
	//按渠道查信息
	static	int GetUserStaffByChannel(char * pszChannelID,vector<SgwUserStaff>& vSgwUserStaff);
	//按主键更新信息
	static	int UpdateUserStaff(int iUserStaffID,SgwUserStaff& sSgwUserStaff);

	static int DeleteUserStaff(SgwUserStaff& sSgwUserStaff);

	static int AddUserStaff(SgwUserStaff& sSgwUserStaff);
};

class SHMOrgRouteRule:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool OrgRouteRule_P(bool bWait = true){
		m_pSHMMgrOrgRoutRuleLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool OrgRouteRule_V(){
		m_pSHMMgrOrgRoutRuleLock->V();
	};

	//按OrgID返回路由信息
	static	int GetOrgRouteRuleByOrg(int iOrgID,SgwOrgRouteRule& sOrgRouteRule);
	//按主键更新
	static	int UpdateOrgRouteRule(int iRouteID,SgwOrgRouteRule& sOrgRouteRule);

	static int DeleteOrgRouteRule(const SgwOrgRouteRule& sOrgRouteRule);

	static int AddOrgRouteRule(const SgwOrgRouteRule& sOrgRouteRule);

	static int GetOrgRouteRule(int _iRouteID,SgwOrgRouteRule& sOrgRouteRule);

};

class SHMAreaOrg:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool AreaOrg_P(bool bWait = true){
		m_pSHMMgrAreaOrgLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool AreaOrg_V(){
		m_pSHMMgrAreaOrgLock->V();
	};

	//返回所有数据
	static int GetAllAreaOrg(vector<SgwAreaOrg>&vSgwAreaOrg);
	//按主键更新数据
	static int UpdateAreaOrg(int iAreaSeq,SgwAreaOrg&sSgwAreaOrg);

	static int AddAreaOrg(const SgwAreaOrg&sSgwAreaOrg);
	static int DeleteAreaOrg(const SgwAreaOrg&sSgwAreaOrg);
	static int GetAreaOrg(int iAreaSeq,const SgwAreaOrg &sSgwAreaOrg);
};

class SHMTuxRelaIn:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool TuxRelaIn_P(bool bWait = true){
		m_pSHMMgrTuxRelaInLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool TuxRelaIn_V(){
		m_pSHMMgrTuxRelaInLock->V();
	};

	//按能力查服务
	static int GetServicesContextID(int iServicesID,SgwTuxedoRelationIn&sTuxedoRelation);

	static int UpdateServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation);

	static int DeleteServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation);

	static int AddServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation);
};

class SHMTuxRelaOut:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool TuxRelaOut_P(bool bWait = true){
		m_pSHMMgrTuxRelaOutLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool TuxRelaOut_V(){
		m_pSHMMgrTuxRelaOutLock->V();
	};

	//按服务查能力
	static	int GetServicesID(char * pszServiceContextID,SgwTuxedoRelationOut&sTuxedoRelation);

	static int GetServicesID(int m_iRelationID ,SgwTuxedoRelationOut&sTuxedoRelation);

	static int AddServicesID(const SgwTuxedoRelationOut&sTuxedoRelation);

	static int DeleteServicesID(const SgwTuxedoRelationOut&sTuxedoRelation);

	static int UpdateServicesID(const SgwTuxedoRelationOut&sTuxedoRelation);
};

class SHMSvrList:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool SvrList_P(bool bWait = true){
		m_pSHMMgrSrvsListLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool SvrList_V(){
		m_pSHMMgrSrvsListLock->V();
	};

	//按服务名称（SERVICES_NAME）返回信息
	static	int GetServicesInfo(char * iServicesName,SgwServicesList&sServicesList);

	//按服务编号返回信息
	static	int GetServicesInfo(int iServicesID,SgwServicesList&sServicesList);

	static	int AddServicesInfo(const SgwServicesList&sServicesList);

	static	int UpdateServicesInfo(const SgwServicesList&sServicesList);

	static	int DeleteServicesInfo(const SgwServicesList&sServicesList);

};

class SHMSrvsParamRela:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool SrvsParamRela_P(bool bWait = true){
		m_pSHMMgrSrvsParamRelaLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool SrvsParamRela_V(){
		m_pSHMMgrSrvsParamRelaLock->V();
	};

	//按服务ID返回数据
	static	int GetServicesParam(int iServicesID,vector<SgwServicesParamRelation>& vServicesParam);

	static	int AddServicesParam(const SgwServicesParamRelation &oServicesParam);

	static	int DeleteServicesParam(const SgwServicesParamRelation &oServicesParam);

	static	int UpdateServicesParam(const SgwServicesParamRelation &oServicesParam);

	static int GetServicesParam(int iMappingID,SgwServicesParamRelation &oServicesParam);
};

class SHMSrvsParamList:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool SrvsParamList_P(bool bWait = true){
		m_pSHMMgrSrvsParamListLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool SrvsParamList_V(){
		m_pSHMMgrSrvsParamListLock->V();
	};

	//按参数ID返回信息
	static	int GetServiceParamList(int iParamID,SgwServicesParamList& sServicesparamList);

	static int AddServiceParamList(const SgwServicesParamList& sServicesparamList);

	static int DeleteServiceParamList(const SgwServicesParamList& sServicesparamList);

	static int UpdateServiceParamList(const SgwServicesParamList& sServicesparamList);

};

class SHMSrvsRegister:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool SrvsRegister_P(bool bWait = true){
		m_pSHMMgrSrvsRegisterLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool SrvsRegister_V(){
		m_pSHMMgrSrvsRegisterLock->V();
	};

	//按网元编码与原始服务查询数据
	static	int GetServiceRegisterBySrc(char * pszChannelID,int iSrcID,vector<SgwServiceRegister>& vsServiceRegister);
	//按网元编码与目标服务查询数据
	static	int GetServiceRegisterByMap(char * pszChannelID,int iMapID,vector<SgwServiceRegister>& vsServiceRegister);

	static	int GetServiceRegister(int iServicesInstanceID,SgwServiceRegister& sServiceRegister);

	static	int AddServiceRegister(const SgwServiceRegister& sServiceRegister);

	static	int DeleteServiceRegister(const SgwServiceRegister& sServiceRegister);

	static	int UpdateServiceRegister(const SgwServiceRegister& sServiceRegister);
};

class SHMParamInsList:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool ParamInsList_P(bool bWait = true){
		m_pSHMMgrParamInsListLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool ParamInsList_V(){
		m_pSHMMgrParamInsListLock->V();
	};

	//按实例ID返回所有数据
	static int GetParamInstanceList(int iServicesInstanceID,vector<SgwParamInstanceList>&vInstanceList);

	static int GetParamInstanceList(int iMappingID,SgwParamInstanceList &oInstanceList);

	static int DeleteParamInstanceList(const SgwParamInstanceList &oInstanceList);

	static int AddParamInstanceList(const SgwParamInstanceList &oInstanceList);

	static int UpdateParamInstanceList(const SgwParamInstanceList &oInstanceList);
};

class SHMEnumArea:public SHMSGWInfoBase
{
public:
	//加锁 成功返回true，失败返回false
	static bool SgwEnumArea_P(bool bWait = true){
		m_pSHMMgrSgwEnumAreaLock->P(bWait);
	};

	//解锁，成功返回truce，失败返回false
	static bool SgwEnumArea_V(){
		m_pSHMMgrSgwEnumAreaLock->V();
	};

	//按seq/orgid返回所有数据
	static int GetSgwEnumArea(int iAreaSeq,SgwEnumArea &oSgwEnumArea);

	static int GetSgwEnumAreaByOrgId(int iOrgID,SgwEnumArea &oSgwEnumArea);

	static int DeleteSgwEnumArea(const SgwEnumArea &oSgwEnumArea);

	static int AddSgwEnumArea(const SgwEnumArea &oSgwEnumArea);

	static int UpdateSgwEnumArea(const SgwEnumArea &oSgwEnumArea);
};
#endif
