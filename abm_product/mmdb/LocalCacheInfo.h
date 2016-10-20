/***************************************************************************
*	Copyright (c) 2010,亚信联创科技股份有限公司电信计费研发部
*	All rights reserved.
*	
*	文件名: LocalCacheInfo.h
*	功能:	批价结果本地缓存数据结构定义及操作方法封装
*	作者：	wangs
*	日期：	2010-08-13
*	版本：	1.0
*	更改记录：
*   2010-08-13 增加 AccuCacheData、DisctCacheData、DisctDetailCacheData定义
*   2010-08-14 增加每个类的说明，调整类字段顺序，HB专用字段放到类底部
*   2010-10-5  增加除与ABM交互的累积量外的结构定义，数据操作方法定义
****************************************************************************/

#ifndef _LOCAL_CACHE_INFO_H_
#define _LOCAL_CACHE_INFO_H_

#include "SHMData.h"
#include "SHMIntHashIndex.h"
#include "SHMData_A.h"
#include "SHMIntHashIndex_A.h"
#include "StdEvent.h"
#include "DataIncrementMgr.h"
#include "IpcKey.h"
#include "AcctItemAccu.h"

using namespace std;

const bool FORCE_YES							= true;
const bool FORCE_NO							= false;
const unsigned int TYPE_CACHE_INIT			= 0;	//初始态
const unsigned int TYPE_CACHE_RDY				= 1;	//准备就绪态
const unsigned int TYPE_CACHE_SUCCESS			= 2;	//处理成功态
const unsigned int TYPE_CACHE_FAIL			= 3;	//处理失败态

const unsigned int TYPE_SEND_NO				= 0;	//未发送状态
const unsigned int TYPE_SEND_YES				= 1;	//已发送状态

#define CACHE_INDEX				(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_Index"))
#define ADDRESS_CACHE_DATA		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_AddressData"))
#define EVENT_WAIT_INDEX		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_EventWaitIndex"))
#define EVENT_WAIT_DATA			(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_EventWaitData"))
#define EVENT_CACHE_DATA		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_EventData"))
#define EVENT_PRICE_CACHE_DATA	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_EventPriceData"))
#define EVENT_AGGR_CACHE_DATA	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_EventAggrData"))
#define ACCT_ITEM_CACHE_DATA	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_AcctItemData"))
#define DISCT_AGGR_CACHE_DATA	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_DisctAggrData"))
#define DISCT_DETAIL_CACHE_DATA	(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_DisctDetailData"))
#define MIDDLE_CACHE_INDEX		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_MiddleIndex"))
#define MIDDLE_INFO_DATA		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_MiddleInfoData"))
#define MIDDLE_CACHE_DATA		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(), "SHM_CACHE_MiddleData"))
#define LOCK_KEY_CACHE_DATA		(IpcKeyMgr::getIpcKey(FORCE_NO,getFlowID(),"SHM_CACHE_LockKeyData"))//由FORCE_YES改为FORCE_NO 在配置上体现

/***************************************************************************
*	类名：AccuCacheData
*	功能：事件累积量缓存数据结构定义
*   说明：
*	1、计费发起更新确认，ABM接受，给结果标志说明成功
*	2、计费发起更新确认，ABM确认数据不一致，把有差异的返回（m_lAccuInstID，最终值）
*		场景1：两边都有但是值不一致（更新ABM给的值后重批）
*		场景2：HB新增，ABM已有，HB按ABM的值新增（新增ABM给的值后重批）
*		场景3：HB新增，ABM没有数据，肯定成功，不会失败
***************************************************************************/
struct AccuCacheData
{
	long m_lAccuInstID;			//主健，由HB生成，ABM做比对时如果失败时返回此主键及最终值
	long m_lOfferInstID;		//商品实例ID
	long m_lMemberID;			//成员明细ID
	long m_lServID;				//用户ID
	int  m_iAccuTypeID;			//累积量类型ID
	int  m_iBillingCycleID;		//默认打开，不再用宏控制
	long m_lCycleTypeID;		//周期类型ID
	long m_lBeginDate;			//开始时间，格式：20100813153848
	long m_lEndDate;			//结束时间，格式：20100813153848
	char m_sSplitby[32];		//拆分依据
	long m_lCycleInstID;		//周期实例ID
	long m_lValue;				//原值
	long m_lAddValue;			//增量值
	long m_lABMKeyID;			//ABM主键
    long m_lVersionID;			//版本ID

	//HB用
	int  m_cOperation;			//标识记录操作方法('i':新增记录,'u':在老记录上累积,'d': 删除,'r'：参考记录，'o' 老记录)
	int  m_iFileID;				//file_id
	int  m_iEventSourceType;	//事件源类型
	int  m_iOrgID;				//ORG_ID
	long m_lEventID;			//事件ID
	char m_sStateDate[16];			//状态时间

	unsigned int m_iNextOffset;  //指向下一条的索引
};

/***************************************************************************
*	类名：DisctCacheData
*	功能：优惠累积量汇总缓存数据结构定义
*   说明：
*	1、计费发起更新确认，ABM接受，给结果标志说明成功
*	2、计费发起更新确认，ABM确认数据不一致，把有差异的返回（m_lItemDisctSeq，最终值）
*		场景1：两边都有但是值不一致（更新ABM给的值后重批）
*		场景2：HB新增，ABM已有，HB按ABM的值新增（新增ABM给的值后重批）
*		场景3：HB新增，ABM没有数据，肯定成功，不会失败
***************************************************************************/
struct DisctCacheData
{
	long m_lItemDisctSeq;		//主健，由HB生成，ABM做比对时如果失败时返回此主键及最终值
	long m_lOfferInstID;		//商品实例ID
	int  m_iBillingCycleID;		//帐期ID
	int  m_iOrgBillingCycleID;	//原帐期ID
	int  m_iCombineID;			//定价组合ID
	int  m_iMeasureID;			//定价度量ID
	int  m_iItemGroupID;		//计费帐目组ID
	int  m_iInType;				//输入对象类型
	long m_lInValue;			//输入对象值
	long m_lValue;				//原输入值
	long m_lDisctValue;			//原优惠值
	long m_lAddValue;			//输入值增量
	long m_lAddDisctValue;		//优惠值增量
	long m_lABMKeyID;			//ABM主键
    long m_lVersionID;			//版本ID

	//HB用
	int  m_cOperation;			//标识记录操作方法('i':新增记录,'u':在老记录上累积,'d': 删除,'r'：参考记录，'o' 老记录)
	int  m_iFileID;				//file_id
	int  m_iEventSourceType;	//事件源类型
	int  m_iOrgID;				//ORG_ID

	unsigned int m_iNextOffset;  //指向下一条的索引
	unsigned int m_iDetailOffset;//指向明细的索引
};

/***************************************************************************
*	类名：DisctDetailCacheData
*	功能：优惠累积量明细缓存数据结构定义
***************************************************************************/
struct DisctDetailCacheData
{
	long m_lItemDetailSeq;		//主健
	long m_lServID;				//用户ID
	int  m_iBillingCycleID;		//帐期ID
	int  m_iAcctItemTypeID;		//帐目类型ID
	long m_lValue;				//原输入值
	long m_lDisctValue;			//原优惠值
	long m_lAddValue;			//输入值增量
	long m_lAddDisctValue;		//优惠值增量
	long m_lABMKeyID;			//ABM主键
    long m_lVersionID;			//版本ID

	//HB用
	int  m_cOperation;			//标识记录操作方法('i':新增记录,'u':在老记录上累积,'d': 删除,'r'：参考记录，'o' 老记录)
	long m_lItemDisctSeq;		//外健
	long m_lEventID;			//事件ID
	int  m_iFileID;				//file_id
	int  m_iEventSourceType;	//事件源类型
	int  m_iOrgID;				//ORG_ID

	unsigned int m_iNextOffset;//指向下一条的索引
};


/***************************************************************************
*	类名：DisctCache
*	功能：优惠累积量缓存结构封装
***************************************************************************/
struct DisctAccu
{
	DisctCacheData  m_oDisct;
	vector<DisctDetailCacheData > m_voDetail;
};

/***************************************************************************
*	类名：EventPriceCacheData
*	功能：清单批价轨迹缓存数据结构定义
***************************************************************************/
struct EventPriceCacheData
{
	EventPriceData m_oData;
	unsigned int m_iNextOffset;//指向下一条的索引
};

/***************************************************************************
*	类名：AcctItemCacheData
*	功能：总帐缓存数据结构定义
***************************************************************************/
struct AcctItemCacheData
{
	AcctItemAddAggrData m_oData;
	unsigned int m_iNextOffset;//指向下一条的索引
};

//EndFileData 这个数据先在控制区中入库，后面根据业务再来考虑是否需要缓存及相关处理逻辑

/***************************************************************************
*	类名：StdEventCacheData
*	功能：事件缓存数据结构定义
***************************************************************************/
struct StdEventCacheData
{
	StdEvent m_oData;
	unsigned int m_iNextOffset;//指向下一条的索引
};

/***************************************************************************
*	类名：AddressCacheData
*	功能：缓存区存放数据的地址的索引结构定义
***************************************************************************/
struct AddressCacheData
{
	long		   m_lEventID;//事件ID
	unsigned int m_iStdEventOffset;//指向事件的索引
	unsigned int m_iEventPriceOffset;//指向事件轨迹的索引
	unsigned int m_iEventAccuOffset;//指向事件累积量的索引
	unsigned int m_iDisctAggrOffset;//指向优惠累积量的索引
	unsigned int m_iAcctItemOffset;//指向总帐的索引
	unsigned int m_iLockKeyOffset;//指向锁关健字的索引
	long		   m_lBeginDate;//生成时间，格式：20100813153848
	unsigned int m_iSend;//是否已送ABM（TYPE_SEND_NO/TYPE_SEND_YES)
	unsigned int m_iType;//处理结果类型
	unsigned int m_iProcessID;//进程ID
};

/***************************************************************************
*	类名：MiddleInfoData
*	功能：中间信息区存放数据的结构定义
***************************************************************************/
struct MiddleInfoData
{
	unsigned int m_iProcessID;//进程ID
	unsigned int m_iDataBegOffset;//指向数据区的开始索引
	unsigned int m_iDataEndOffset;//指向数据区的结束索引
	unsigned int m_iWaitOffset;
};

/***************************************************************************
*	类名：MiddleCacheData
*	功能：中间数据区存放数据的结构定义
***************************************************************************/
struct MiddleCacheData
{
	unsigned int m_iWaitEventOffset;
	unsigned int m_iAddressOffset;//指向地址区的索引
	unsigned int m_iNextOffset;//指向下一条的索引
	unsigned int m_iOtherOffset;
	unsigned long m_iOffset1;
	unsigned long m_iOtherOffset1;
};

/***************************************************************************
*	类名：LockKeyData
*	功能：加锁关健字结构定义
***************************************************************************/
struct LockKeyData
{
	long m_lKey;//Key值
	unsigned int m_iNextOffset;//指向下一条的索引
	unsigned int m_iOtherOffset;
	unsigned long m_iOffset1;
	unsigned long m_iOtherOffset1;
};

/***************************************************************************
*	类名：LocalCacheInfo
*	功能：实现缓存区数据的操作封装
***************************************************************************/
class LocalCacheInfo
{
public:
	LocalCacheInfo();
	operator bool();

protected:
	void bindData();

	//缓存区索引区
	static SHMIntHashIndex_A *m_poCacheIndex;

	//首地址数据区
	static SHMData_A<AddressCacheData> *m_poAddressCacheData;

	//首地址数据区指针
	static AddressCacheData *m_poAddressCache;

	//等待事件索引区
	static SHMIntHashIndex_A *m_poEventWaitIndex;

	//等待事件数据区
	static SHMData_A<StdEventCacheData> *m_poEventWaitData;

	//等待事件数据区指针
	static StdEventCacheData *m_poEventWait;

	//缓存事件数据区
	static SHMData_A<StdEvent> *m_poEventCacheData;

	//缓存事件数据区指针
	static StdEvent *m_poEventCache;

	//事件轨迹数据区
	static SHMData_A<EventPriceCacheData> *m_poEventPriceCacheData;

	//事件轨迹数据区指针
	static EventPriceCacheData *m_poEventPriceCache;

	//事件累积量数据区
	static SHMData_A<AccuCacheData> *m_poAccuCacheData;

	//事件累积量数据区指针
	static AccuCacheData *m_poAccuCache;

	//总帐数据区
	static SHMData_A<AcctItemCacheData> *m_poAcctItemCacheData;

	//总帐数据区指针
	static AcctItemCacheData *m_poAcctItemCache;

	//优惠汇总数据区
	static SHMData_A<DisctCacheData> *m_poDisctCacheData;

	//优惠汇总数据区指针
	static DisctCacheData *m_poDisctCache;

	//优惠明细数据区
	static SHMData_A<DisctDetailCacheData> *m_poDisctDetailCacheData;

	//优惠明细数据区指针
	static DisctDetailCacheData *m_poDisctDetailCache;

	//中间索引区
	static SHMIntHashIndex *m_poMiddleIndex;

	//中间信息数据区
	static SHMData<MiddleInfoData> *m_poMiddleInfoData;

	//中间信息数据区指针
	static MiddleInfoData *m_poMiddleInfo;

	//中间数据区
	static SHMData_A<MiddleCacheData> *m_poMiddleCacheData;

	//中间数据区指针
	static MiddleCacheData *m_poMiddleCache;

	//锁关健字数据区
	static SHMData_A<LockKeyData> *m_poKeyCacheData;

	//锁关健字数据区指针
	static LockKeyData *m_poKeyCache;

	//是否成功连接共享内存
	static bool m_bAttached;

private:
	void freeAll();
};

#endif /* _LOCAL_CACHE_INFO_H_ */
