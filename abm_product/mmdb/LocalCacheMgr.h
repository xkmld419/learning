/***************************************************************************
*	Copyright (c) 2010,亚信联创科技股份有限公司电信计费研发部
*	All rights reserved.
*	
*	文件名: LocalCacheMgr.h
*	功能:	本地缓存累积量管理封装
*	作者：	wangs
*	日期：	2010-10-06
*	版本：	1.0
*	更改记录：
****************************************************************************/

#ifndef _LOCAL_CACHE_MGR_H
#define _LOCAL_CACHE_MGR_H

#include "LocalCacheInfo.h"
#include "ParamDefineMgr.h"
#include "CSemaphore.h"
#include "DataIncrementMgr.h"
#include "AccuLock.h"
#include "ProdOfferAggr.h"
#include "EventSender.h"
#include "CommonMacro.h"
#include <vector>
#include <map>
#ifdef HAVE_ABM
#include "abmcmdaccu.h"
#endif
using namespace std;
class OfferAccuMgr;

//最大批价进程数
#define PROCESS_NUM_MAX		64

//最大接口进程并行锁
#define CACHE_GET_LOCK		8

//本地缓存区大小定义
#define CHCHE_NUM_MAX		(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "LOCAL_CACHE_BASE"))
#define EVENT_WAIT_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "EVENT_WAIT_FACTOR"))
#define EVENT_PRICE_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "EVENT_PRICE_FACTOR"))
#define EVENT_AGGR_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "EVENT_AGGR_FACTOR"))
#define DISCT_AGGR_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "DISCT_AGGR_FACTOR"))
#define DISCT_DETAIL_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "DISCT_DETAIL_FACTOR"))
#define ACCT_ITEM_FACTOR	(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "ACCT_ITEM_FACTOR"))
#define LOCK_KEY_FACTOR		(ParamDefineMgr::getLongParam(getFlowID(), "CACHE_NUM", "LOCK_KEY_FACTOR"))

//锁的KEY值定义
#define CACHE_INDEX_LOCK				(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_Index"))
#define ADDRESS_CACHE_DATA_LOCK			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_AddressData"))
#define EVENT_CACHE_DATA_LOCK			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_EventCacheData"))
#define EVENT_WAIT_INDEX_LOCK			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_EventWaitIndex"))
#define EVENT_PRICE_CACHE_DATA_LOCK		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_EventPriceData"))
#define EVENT_AGGR_CACHE_DATA_LOCK		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_EventAggrData"))
#define ACCT_ITEM_CACHE_DATA_LOCK		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_AcctItemData"))
#define DISCT_AGGR_CACHE_DATA_LOCK		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_DisctAggrData"))
#define DISCT_DETAIL_CACHE_DATA_LOCK	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_DisctDetailData"))
#define MIDDLE_CACHE_INDEX_LOCK			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_MiddleIndex"))
#define MIDDLE_INFO_DATA_LOCK			(IpcKeyMgr::getIpcKey(-1, "SEM_CACHE_MiddleInfoData"))//区间值[.,.+PROCESS_NUM_MAX]
#define MIDDLE_CACHE_DATA_LOCK			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SEM_CACHE_MiddleData"))
#define CACHE_GET_DATA_LOCK				(IpcKeyMgr::getIpcKey(-1, "SEM_CACHE_GetData"))//区间值[.,.+CACHE_GET_LOCK]

//查询本地缓存优惠累积量的结构定义
struct DisctResult
{
	DisctCacheData * m_pDisct;
	vector<DisctDetailCacheData *> m_vDetail;
};

/*//关健字对应的首地址 暂时不用，后续删除
struct KeyAddress
{
	long m_lEventID;
	unsigned int m_iOffset;
}*/

//本地缓存管理类
class LocalCacheMgr : public LocalCacheInfo
{
public:
	LocalCacheMgr(AcctItemAccuMgr *poAcctItemMgr,OfferAccuMgr *poAccuMgr,
		DataIncrementMgr* poInDB,AccuLockMgr * poLockMgr,EventSender * poSender,int iProc);

	LocalCacheMgr();

	//增加批价事件
	bool addEvent(StdEvent * pData);

	//增加等待事件
	bool addEvent(StdEvent * pData,long lEventID);

	//增加事件轨迹
	bool addEventPricing(EventPriceData *pData);

	//增加事件累积量
	bool addAccuAggr(AccuCacheData *pData);

	//增加总帐
	bool addAcctItemAggr(AcctItemAddAggrData *pData);

	//增加优惠汇总
	bool addAcctDisctAggr(DisctCacheData *pData);

	//增加优惠明细
	bool addAcctDisctDetail(DisctDetailCacheData *pData);

	//增加锁关健字
	bool addLockKey(long *pKeyBuf,int iNum);

	//本地缓存清理
	bool clearCache(long lEventID,bool bunlock = false);

	//查询本地缓存累积量数据
	bool getSyncAccu(vector<AccuCacheData *> &vAccu,vector<DisctResult> &vDisct,long &lEventID);

	//查询本地缓存累积量数据（通过传入的事件ID）
	bool getSyncAccuByEventID(long lEventID,vector<AccuCacheData *> &vAccu,vector<DisctResult> &vDisct);

	//按事件确认成功
	bool syncSuccess(long lEventID);

	//按进程确认成功
	void syncSuccess(int iProcID);

	//置失败状态为成功状态
	void setFailToSuccess(int iProcID);

	//abm接口确认成功的累积量集合

	//bool syncSuccess(long lEventID,vector<AuditEventAccuData *> &vpAccuData,
	//	map<AuditDisctAccuData *,vector<AuditDisctAccuDetailData *>>  &mpDisctData);

	//abm接口确认失败的累积量集合
	bool syncFail(long lEventID);

	//一次批价结果数据增加完成提交
	bool commit();

	//一次批价结果增加回退
	bool rollback();

	//本地缓存区创建
	void create();

	//本地缓存区删除
	void remove();

	//初始化变量
	void init();

	//临时事件来源初始化
	void initEventSource();

	//初始化临时指针
	void initTempP();

public:
	//缓存区管理
	bool cacheProc();

	//转换等待事件
	void convertWaitEvent(int iLocation,StdEvent ** pEvent);

	//转换失败事件
	void convertFailEvent(int iLocation,StdEvent ** pEvent);

	//关联等待话单处理
	void relaWaitProc(long lEventID,bool bRelaOther = false);

    //超时管理
    bool overdue(const Date & time, long &lEventID);

private:

	//初始化环境
	void initEnv();	

	//结果处理
	bool syncResult(long lEventID,int iType);

	//数据入库
	void insertData(unsigned int iOffset);

	//解锁
	void unlockKey(unsigned int iOffset);

private:
	//信号量定义
	static CSemaphore * m_poCacheIndexLock;
	static CSemaphore * m_poAddressDataLock;
	static CSemaphore * m_poEventCacheDataLock;
	static CSemaphore * m_poEventWaitLock;
	static CSemaphore * m_poEventPriceDataLock;
	static CSemaphore * m_poEventAggrDataLock;
	static CSemaphore * m_poAcctItemDataLock;
	static CSemaphore * m_poDisctAggrDataLock;
	static CSemaphore * m_poDisctDetailDataLock;
	static CSemaphore * m_poMiddleIndexLock;
	static CSemaphore * m_poMiddleInfoDataLock[PROCESS_NUM_MAX];
	static CSemaphore * m_poMiddleCacheDataLock;
	static CSemaphore * m_poGetDataLock[CACHE_GET_LOCK];

	//地址指针定义
	AddressCacheData * m_poAddAddress;
	//地址索引
	unsigned int m_iAddressOffset;
	//临时指针定义
	EventPriceCacheData * m_poEventPriceLast;
	AccuCacheData * m_poAccuLast;
	AcctItemCacheData * m_poAcctItemLast;
	DisctCacheData *m_poDisctLast;
	DisctDetailCacheData *m_poDisctDetailLast;

	//地址大小
	unsigned int m_iAddressTotal;
	//地址偏移
	unsigned int m_iOffset;

	//成员变量指针
	int m_iProcID;
	AcctItemAccuMgr *m_poAcctItemMgr;
	OfferAccuMgr    *m_poAccuMgr;
	AccuLockMgr	*m_poLockMgr;
	DataIncrementMgr *m_poInDB;
	EventSender * m_poSender;

	//发送帐单缓存
	AcctItemTransData m_oItemData[MAX_ITEM_RECORD];

public:
	//失败话单地址
	vector<unsigned int> m_viAddressFail;
	//等待话单地址
	vector<unsigned int> m_viAddressWait;
};

#endif //_LOCAL_CACHE_MGR_H
