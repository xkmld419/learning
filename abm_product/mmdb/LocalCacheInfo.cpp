#include "LocalCacheInfo.h"
#include "UserInfo.h"

//静态变量初始化
bool LocalCacheInfo::m_bAttached = false;

SHMIntHashIndex_A *LocalCacheInfo::m_poCacheIndex = 0;
SHMData_A<AddressCacheData> *LocalCacheInfo::m_poAddressCacheData = 0;
AddressCacheData *LocalCacheInfo::m_poAddressCache = 0;

SHMIntHashIndex_A *LocalCacheInfo::m_poEventWaitIndex = 0;
SHMData_A<StdEventCacheData> *LocalCacheInfo::m_poEventWaitData = 0;
StdEventCacheData *LocalCacheInfo::m_poEventWait = 0;

SHMData_A<StdEvent> *LocalCacheInfo::m_poEventCacheData = 0;
StdEvent *LocalCacheInfo::m_poEventCache = 0;

SHMData_A<EventPriceCacheData> *LocalCacheInfo::m_poEventPriceCacheData = 0;
EventPriceCacheData *LocalCacheInfo::m_poEventPriceCache = 0;

SHMData_A<AccuCacheData> *LocalCacheInfo::m_poAccuCacheData = 0;
AccuCacheData *LocalCacheInfo::m_poAccuCache = 0;

SHMData_A<AcctItemCacheData> *LocalCacheInfo::m_poAcctItemCacheData = 0;
AcctItemCacheData *LocalCacheInfo::m_poAcctItemCache = 0;

SHMData_A<DisctCacheData> *LocalCacheInfo::m_poDisctCacheData = 0;
DisctCacheData *LocalCacheInfo::m_poDisctCache = 0;

SHMData_A<DisctDetailCacheData> *LocalCacheInfo::m_poDisctDetailCacheData = 0;
DisctDetailCacheData *LocalCacheInfo::m_poDisctDetailCache = 0;

SHMIntHashIndex *LocalCacheInfo::m_poMiddleIndex = 0;
SHMData<MiddleInfoData> *LocalCacheInfo::m_poMiddleInfoData = 0;
MiddleInfoData *LocalCacheInfo::m_poMiddleInfo = 0;

SHMData_A<MiddleCacheData> *LocalCacheInfo::m_poMiddleCacheData = 0;
MiddleCacheData *LocalCacheInfo::m_poMiddleCache = 0;

SHMData_A<LockKeyData> *LocalCacheInfo::m_poKeyCacheData = 0;
LockKeyData *LocalCacheInfo::m_poKeyCache = 0;

LocalCacheInfo::LocalCacheInfo()
{
//dprintf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
#undef ATTACH_DATA
#undef ATTACH_INT_INDEX
#undef ATTACH_DATA_A
#undef ATTACH_INT_INDEX_A
#undef ATTACH_STRING_INDEX 

	#define ATTACH_DATA_A(X, Y, Z) \
		X = new SHMData_A<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
		}

	#define ATTACH_INT_INDEX_A(X, Z) \
		X = new SHMIntHashIndex_A (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
		}

	#define ATTACH_DATA(X, Y, Z) \
		X = new SHMData<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
		}

	#define ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
		}

	#define ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
		}
		
	if (m_bAttached)
		return;

	m_bAttached = true;	

	ATTACH_INT_INDEX_A(m_poCacheIndex ,CACHE_INDEX);
	ATTACH_DATA_A(m_poAddressCacheData, AddressCacheData, ADDRESS_CACHE_DATA);

	ATTACH_INT_INDEX_A(m_poEventWaitIndex ,EVENT_WAIT_INDEX);
	ATTACH_DATA_A(m_poEventWaitData, StdEventCacheData, EVENT_WAIT_DATA);
	ATTACH_DATA_A(m_poEventCacheData, StdEvent, EVENT_CACHE_DATA);

	ATTACH_DATA_A(m_poEventPriceCacheData, EventPriceCacheData, EVENT_PRICE_CACHE_DATA);

	ATTACH_DATA_A(m_poAccuCacheData, AccuCacheData, EVENT_AGGR_CACHE_DATA);

	ATTACH_DATA_A(m_poAcctItemCacheData, AcctItemCacheData, ACCT_ITEM_CACHE_DATA);

	ATTACH_DATA_A(m_poDisctCacheData, DisctCacheData, DISCT_AGGR_CACHE_DATA);

	ATTACH_DATA_A(m_poDisctDetailCacheData, DisctDetailCacheData, DISCT_DETAIL_CACHE_DATA);

	ATTACH_INT_INDEX(m_poMiddleIndex ,MIDDLE_CACHE_INDEX);
	ATTACH_DATA(m_poMiddleInfoData, MiddleInfoData, MIDDLE_INFO_DATA);
	ATTACH_DATA_A(m_poMiddleCacheData, MiddleCacheData, MIDDLE_CACHE_DATA);

	ATTACH_DATA_A(m_poKeyCacheData, LockKeyData, LOCK_KEY_CACHE_DATA);

	if (m_bAttached)
		bindData ();
}

void LocalCacheInfo::freeAll()
{
	#define FREE(X) \
		if (X) { \
			delete X; \
			X = 0; \
		}

	FREE(m_poCacheIndex);
	FREE(m_poAddressCacheData);
	FREE(m_poEventWaitIndex);
	FREE(m_poEventWaitData);
	FREE(m_poEventCacheData);
	FREE(m_poEventPriceCacheData);
	FREE(m_poAccuCacheData);
	FREE(m_poAcctItemCacheData);
	FREE(m_poDisctCacheData);
	FREE(m_poDisctDetailCacheData);
	FREE(m_poMiddleIndex);
	FREE(m_poMiddleInfoData);
	FREE(m_poMiddleCacheData);
	FREE(m_poKeyCacheData);
}

void LocalCacheInfo::bindData()
{
	m_poAddressCache = (AddressCacheData *)(*m_poAddressCacheData);
	m_poEventWait = (StdEventCacheData *)(*m_poEventWaitData);
	m_poEventCache = (StdEvent *)(*m_poEventCacheData);
	m_poEventPriceCache = (EventPriceCacheData *)(*m_poEventPriceCacheData);
	m_poAccuCache = (AccuCacheData *)(*m_poAccuCacheData);
	m_poAcctItemCache = (AcctItemCacheData *)(*m_poAcctItemCacheData);
	m_poDisctCache = (DisctCacheData *)(*m_poDisctCacheData);
	m_poDisctDetailCache = (DisctDetailCacheData *)(*m_poDisctDetailCacheData);
	m_poMiddleInfo = (MiddleInfoData *)(*m_poMiddleInfoData);
	m_poMiddleCache = (MiddleCacheData *)(*m_poMiddleCacheData);
	m_poKeyCache = (LockKeyData *)(*m_poKeyCacheData);
}

LocalCacheInfo::operator bool()
{
	return m_bAttached;
}
