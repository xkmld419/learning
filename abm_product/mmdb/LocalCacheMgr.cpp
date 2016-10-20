#include "LocalCacheMgr.h"
#include "Environment.h"
#include <stdlib.h>
#include "Log.h"
#include "OfferAccuMgr.h"
#include "CommonMacro.h"

CSemaphore * LocalCacheMgr::m_poCacheIndexLock = 0;
CSemaphore * LocalCacheMgr::m_poAddressDataLock = 0;
CSemaphore * LocalCacheMgr::m_poEventWaitLock = 0;
CSemaphore * LocalCacheMgr::m_poEventCacheDataLock = 0;
CSemaphore * LocalCacheMgr::m_poEventPriceDataLock = 0;
CSemaphore * LocalCacheMgr::m_poEventAggrDataLock = 0;
CSemaphore * LocalCacheMgr::m_poAcctItemDataLock = 0;
CSemaphore * LocalCacheMgr::m_poDisctAggrDataLock = 0;
CSemaphore * LocalCacheMgr::m_poDisctDetailDataLock = 0;
CSemaphore * LocalCacheMgr::m_poMiddleIndexLock = 0;
CSemaphore * LocalCacheMgr::m_poMiddleInfoDataLock[PROCESS_NUM_MAX] = {0};
CSemaphore * LocalCacheMgr::m_poMiddleCacheDataLock = 0;

CSemaphore * LocalCacheMgr::m_poGetDataLock[CACHE_GET_LOCK] = {0};

void LocalCacheMgr::initEnv()
{
	if (!G_POPRODOFFFERDISCT)
		G_POPRODOFFFERDISCT = new ProdOfferDisctAggr();

	create();
	bindData();

    if (!m_bAttached) {
        THROW (MBC_AcctItemAccu+1);
    }

    char sTemp[32];
	
	if (!m_poCacheIndexLock) {
		m_poCacheIndexLock = new CSemaphore ();
		sprintf (sTemp, "%d", CACHE_INDEX_LOCK);
		m_poCacheIndexLock->getSem (sTemp, 1, 1);
	}

	if (!m_poAddressDataLock) {
		m_poAddressDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", ADDRESS_CACHE_DATA_LOCK);
		m_poAddressDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poEventCacheDataLock) {
		m_poEventCacheDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", EVENT_CACHE_DATA_LOCK);
		m_poEventCacheDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poEventWaitLock) {
		m_poEventWaitLock = new CSemaphore ();
		sprintf (sTemp, "%d", EVENT_WAIT_INDEX_LOCK);
		m_poEventWaitLock->getSem (sTemp, 1, 1);
	}

	if (!m_poEventPriceDataLock) {
		m_poEventPriceDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", EVENT_PRICE_CACHE_DATA_LOCK);
		m_poEventPriceDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poEventAggrDataLock) {
		m_poEventAggrDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", EVENT_AGGR_CACHE_DATA_LOCK);
		m_poEventAggrDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poAcctItemDataLock) {
		m_poAcctItemDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", ACCT_ITEM_CACHE_DATA_LOCK);
		m_poAcctItemDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poDisctAggrDataLock) {
		m_poDisctAggrDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", DISCT_AGGR_CACHE_DATA_LOCK);
		m_poDisctAggrDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poDisctDetailDataLock) {
		m_poDisctDetailDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", DISCT_DETAIL_CACHE_DATA_LOCK);
		m_poDisctDetailDataLock->getSem (sTemp, 1, 1);
	}

	if (!m_poMiddleIndexLock) {
		m_poMiddleIndexLock = new CSemaphore ();
		sprintf (sTemp, "%d", MIDDLE_CACHE_INDEX_LOCK);
		m_poMiddleIndexLock->getSem (sTemp, 1, 1);
	}

	if (!m_poMiddleCacheDataLock) {
		m_poMiddleCacheDataLock = new CSemaphore ();
		sprintf (sTemp, "%d", MIDDLE_CACHE_DATA_LOCK);
		m_poMiddleCacheDataLock->getSem (sTemp, 1, 1);
	}
	int i;
	int iKey = CACHE_GET_DATA_LOCK;
	for (i=0; i<CACHE_GET_LOCK; i++) {
		iKey = iKey + i;
		if (m_poGetDataLock[i]) continue;
		sprintf (sTemp, "%d", iKey);
		m_poGetDataLock[i] = new CSemaphore ();
		m_poGetDataLock[i]->getSem (sTemp, 1, 1);
    }

	iKey = MIDDLE_INFO_DATA_LOCK;
	for (i=0; i<PROCESS_NUM_MAX; i++) {
		iKey = iKey +i;
		if (m_poMiddleInfoDataLock[i]) continue;
		sprintf (sTemp, "%d", iKey);
		m_poMiddleInfoDataLock[i] = new CSemaphore ();
		m_poMiddleInfoDataLock[i]->getSem (sTemp, 1, 1);
    }

	m_poAddAddress = 0;
	m_iAddressTotal = m_poAddressCacheData->getTotal();
	m_iOffset = 1;
	m_viAddressFail.resize(512);
	m_viAddressWait.resize(51200);//考虑效率放大些
	initEventSource();
}

LocalCacheMgr::LocalCacheMgr()
{
	initEnv();
}

LocalCacheMgr::LocalCacheMgr(AcctItemAccuMgr *poAcctItemMgr,OfferAccuMgr *poAccuMgr,
		DataIncrementMgr* poInDB,AccuLockMgr * poLockMgr,EventSender * poSender,
		int iProcID) : LocalCacheInfo()
{
	if(!poAcctItemMgr){
		THROW (MBC_CacheMgr+3);
	}
	m_poAcctItemMgr = poAcctItemMgr;

	if(!poAccuMgr){
		THROW (MBC_CacheMgr+4);
	}
	m_poAccuMgr = poAccuMgr;

	if (!poInDB){
		THROW (MBC_CacheMgr+5);
	}
	m_poInDB = poInDB;

	if (!poLockMgr){
		THROW (MBC_CacheMgr+6);
	}
	m_poLockMgr = poLockMgr;

	if(!poSender){
		THROW (MBC_CacheMgr+7);
	}
	m_poSender = poSender;

	if (iProcID<=0){
		THROW (MBC_CacheMgr+8);
	}
	m_iProcID = iProcID;

	initEnv();	
}

bool LocalCacheMgr::addEvent(StdEvent * pData)
{
	bool bRet = true;
	unsigned int i = 0;
	init();
	m_poAddressDataLock->P();
	i = m_poAddressCacheData->malloc();
	m_poAddressDataLock->V();
	if (0 == i) return false;
	m_iAddressOffset = i;
	m_poAddAddress = &m_poAddressCache[i];
	m_poEventCacheDataLock->P();
	i = m_poEventCacheData->malloc();
	m_poEventCacheDataLock->V();
	if (0 == i) return false;
	memcpy(&m_poEventCache[i],pData,sizeof(StdEvent));
	m_poAddAddress->m_iSend = TYPE_SEND_NO;
	m_poAddAddress->m_iStdEventOffset = i;
	m_poAddAddress->m_iProcessID = pData->m_iProcessID;
	m_poAddAddress->m_lEventID = pData->m_lEventID;
	return bRet;
}

bool LocalCacheMgr::addEvent(StdEvent * pData,long lEventID)
{
	bool bRet = true;
	unsigned int i = 0,k=0;
	m_poEventWaitLock->P();
	i = m_poEventWaitData->malloc();
	if (0 == i){
		m_poEventWaitLock->V();
		return false;
	}
	StdEventCacheData * pEventWait = m_poEventWait+i;
	memcpy(&(pEventWait->m_oData),pData,sizeof(StdEvent));
	pEventWait->m_iNextOffset = 0;
	if (m_poEventWaitIndex->get(lEventID,&k))
	{
		unsigned int  * piTemp;
		while(k){
			piTemp = &(m_poEventWait[k].m_iNextOffset);
			k = m_poEventWait[k].m_iNextOffset;
		}
		*piTemp = i;
	}else{
		m_poEventWaitIndex->add(lEventID,i);
	}
	m_poEventWaitLock->V();
	return bRet;
}

bool LocalCacheMgr::addEventPricing(EventPriceData *pData)
{
	bool bRet = true;
	unsigned int i = 0;
	m_poEventPriceDataLock->P();
	i = m_poEventPriceCacheData->malloc();
	m_poEventPriceDataLock->V();
	if (0 == i ) return false;
	if (0 == m_poAddAddress->m_iEventPriceOffset){
		m_poAddAddress->m_iEventPriceOffset = i;
	}
	EventPriceCacheData * pTemp = m_poEventPriceCache+i;
	memcpy(pTemp,pData,sizeof(EventPriceData));
	pTemp->m_iNextOffset = 0;
	if (m_poEventPriceLast){
		m_poEventPriceLast->m_iNextOffset = i;
	}
	m_poEventPriceLast = pTemp;
	return bRet;
}

bool LocalCacheMgr::addAccuAggr(AccuCacheData *pData)
{
	bool bRet = true;
	unsigned int i = 0;
	m_poEventAggrDataLock->P();
	i = m_poAccuCacheData->malloc();
	m_poEventAggrDataLock->V();
	if (0 == i ) return false;
	if (0 == m_poAddAddress->m_iEventAccuOffset){
		m_poAddAddress->m_iEventAccuOffset = i;
	}
	AccuCacheData * pTemp = m_poAccuCache+i;
	memcpy(pTemp,pData,sizeof(AccuCacheData));
	pTemp->m_iNextOffset = 0;
	if (m_poAccuLast){
		m_poAccuLast->m_iNextOffset = i;
	}
	m_poAccuLast = pTemp;
	return bRet;
}

bool LocalCacheMgr::addAcctItemAggr(AcctItemAddAggrData *pData)
{
	bool bRet = true;
	unsigned int i = 0;
	m_poAcctItemDataLock->P();
	i = m_poAcctItemCacheData->malloc();
	m_poAcctItemDataLock->V();
	if (0 == i ) return false;
	if (0 == m_poAddAddress->m_iAcctItemOffset){
		m_poAddAddress->m_iAcctItemOffset = i;
	}
	AcctItemCacheData * pTemp = m_poAcctItemCache+i;
	pData->lAcctItemID = 0;
	memcpy(pTemp,pData,sizeof(AcctItemAddAggrData));
	pTemp->m_iNextOffset = 0;
	if (m_poAcctItemLast){
		m_poAcctItemLast->m_iNextOffset = i;
	}
	m_poAcctItemLast = pTemp;
	return bRet;
}

bool LocalCacheMgr::addAcctDisctAggr(DisctCacheData *pData)
{
	bool bRet = true;
	unsigned int i = 0;
	m_poDisctAggrDataLock->P();
	i = m_poDisctCacheData->malloc();
	m_poDisctAggrDataLock->V();
	if (0 == i ) return false;
	if (0 == m_poAddAddress->m_iDisctAggrOffset){
		m_poAddAddress->m_iDisctAggrOffset = i;
	}
	DisctCacheData * pTemp = m_poDisctCache+i;
	memcpy(pTemp,pData,sizeof(DisctCacheData));
	pTemp->m_iNextOffset = 0;
	if (m_poDisctLast){
		m_poDisctLast->m_iNextOffset = i;
	}
	m_poDisctLast = pTemp;
	m_poDisctDetailLast = 0;
	return bRet;
}

bool LocalCacheMgr::addAcctDisctDetail(DisctDetailCacheData *pData)
{
	bool bRet = true;
	unsigned int i = 0;
	m_poDisctDetailDataLock->P();
	i = m_poDisctDetailCacheData->malloc();
	m_poDisctDetailDataLock->V();
	if (0 == i ) return false;
	if (0 == m_poDisctLast->m_iDetailOffset){
		m_poDisctLast->m_iDetailOffset = i;
	}
	DisctDetailCacheData * pTemp = m_poDisctDetailCache+i;
	memcpy(pTemp,pData,sizeof(DisctDetailCacheData));
	pTemp->m_iNextOffset = 0;
	if (m_poDisctDetailLast){
		m_poDisctDetailLast->m_iNextOffset = i;
	}
	m_poDisctDetailLast = pTemp;
	return bRet;
}

bool LocalCacheMgr::addLockKey(long *pKeyBuf,int iNum)
{
	bool bRet = true;
	unsigned int i = 0,iTemp = 0;
	if (0 == iNum) return true;
	LockKeyData * pTemp;
	for (int j =0;j<iNum;j++)
	{
		i = m_poKeyCacheData->malloc();//不用加锁
		if (0 == i )
		{
			//回退
			i = m_poAddAddress->m_iLockKeyOffset;
			while(i){
				iTemp = i;
				m_poKeyCacheData->revoke(i);
				i = m_poKeyCache[iTemp].m_iNextOffset;
			}
			return false;
		}
		if (0 == m_poAddAddress->m_iLockKeyOffset){
			m_poAddAddress->m_iLockKeyOffset = i;
		}
		pTemp = m_poKeyCache + i;
		pTemp->m_lKey = pKeyBuf[j];
		pTemp->m_iNextOffset = 0;
		if (iTemp >0){
			m_poKeyCache[iTemp].m_iNextOffset = i;
		}
		iTemp = i;
	}
	return bRet;
}

bool LocalCacheMgr::getSyncAccu(vector<AccuCacheData *> &vAccu,vector<DisctResult> &vDisct,long &lEventID)
{
	bool bRet = false;
	if (m_iOffset > m_iAddressTotal){
		m_iOffset = 1;
	}
	unsigned int i = 0,j = 0,m = 0;
	DisctResult disctData;
	bool bContinue = false;
	AddressCacheData * pData = NULL;
	while(m_iOffset<=m_iAddressTotal){
		bContinue = false;
		pData = m_poAddressCache+m_iOffset;
		if (TYPE_SEND_NO == pData->m_iSend && TYPE_CACHE_RDY == pData->m_iType)
		{
			m = m_iOffset%CACHE_GET_LOCK;
			m_poGetDataLock[m]->P();
			if (TYPE_SEND_NO == pData->m_iSend && TYPE_CACHE_RDY == pData->m_iType){
				pData->m_iSend = TYPE_SEND_YES;
			}else{
				bContinue = true;
			}
			m_poGetDataLock[m]->V();
			if(bContinue){
				m_iOffset ++;
				continue;
			}
			lEventID = pData->m_lEventID;
			if (pData->m_iEventAccuOffset)
			{
				i = pData->m_iEventAccuOffset;
				while(i){
					vAccu.push_back(&(m_poAccuCache[i]));
					i = m_poAccuCache[i].m_iNextOffset;
				}
			}
			if (pData->m_iDisctAggrOffset)
			{
				i = pData->m_iDisctAggrOffset;
				while(i){
					memset(&disctData,0,sizeof(DisctResult));
					disctData.m_vDetail.clear();
					disctData.m_pDisct = &(m_poDisctCache[i]);
					j = m_poDisctCache[i].m_iDetailOffset;
					while(j){
						disctData.m_vDetail.push_back(&(m_poDisctDetailCache[j]));
						j = m_poDisctDetailCache[j].m_iNextOffset;
					}
					vDisct.push_back(disctData);
					i = m_poDisctCache[i].m_iNextOffset;
				}
			}
			return true;
		}
		m_iOffset ++;
	}
	return bRet;
}

//考虑到超时数据有限，从整体性能上考虑这里就差点，不再增加额外的索引关联来使用
bool LocalCacheMgr::getSyncAccuByEventID(long lEventID,vector<AccuCacheData *> &vAccu,vector<DisctResult> &vDisct)
{
	bool bRet = false;
	unsigned int iOffset = 1;
	unsigned int i = 0,j = 0;
	DisctResult disctData;
	AddressCacheData * pData = NULL;
	while(iOffset<=m_iAddressTotal){
		pData = m_poAddressCache+iOffset;
		if (lEventID != pData->m_lEventID) {
			iOffset++;
			continue;
		}		
		pData->m_iSend = TYPE_SEND_YES;
		bRet = true;
		if (pData->m_iEventAccuOffset)
		{
			i = pData->m_iEventAccuOffset;
			while(i){
				vAccu.push_back(&(m_poAccuCache[i]));
				i = m_poAccuCache[i].m_iNextOffset;
			}
		}
		if (pData->m_iDisctAggrOffset)
		{
			i = pData->m_iDisctAggrOffset;
			while(i){
				memset(&disctData,0,sizeof(DisctResult));
				disctData.m_vDetail.clear();
				disctData.m_pDisct = &(m_poDisctCache[i]);
				j = m_poDisctCache[i].m_iDetailOffset;
				while(j){
					disctData.m_vDetail.push_back(&(m_poDisctDetailCache[j]));
					j = m_poDisctDetailCache[j].m_iNextOffset;
				}
				vDisct.push_back(disctData);
				i = m_poDisctCache[i].m_iNextOffset;
			}
		}
		if (bRet) break;
		iOffset++;
	}
	if(!bRet){
		Log::log(0,"严重错误：根据传入的EVENT_ID=%ld 在本地缓存区中不存在记录，请检查原因。",lEventID);
	}
	return bRet;
}

bool LocalCacheMgr::syncResult(long lEventID,int iType)
{
	bool bRet = true;
	unsigned int iAddress = 0,iInfo = 0,iData = 0,m = 0;
	if (!m_poCacheIndex->get(lEventID,&iAddress))
	{
		Log::log(0,"严重错误：ABM返回的EVENT_ID=%ld 在本地缓存区索引中不存在，请检查原因。",lEventID);
		return false;
	}

	m_poAddressCache[iAddress].m_iType = iType;
	int iProcessID = m_poAddressCache[iAddress].m_iProcessID;
	if (!m_poMiddleIndex->get(iProcessID,&iInfo)){
		m_poMiddleIndexLock->P();
		if (m_poMiddleIndex->get(iProcessID,&iInfo)){
			m_poMiddleIndexLock->V();
			goto _FLAG1;
		}
		iInfo = m_poMiddleInfoData->malloc();
		if (0 == iInfo){
			Log::log(0,"进程数太多，没有可以使用的中间缓冲信息区，请调整");
			m_poMiddleIndexLock->V();
			THROW (MBC_CacheMgr+1);
		}
		m_poMiddleInfo[iInfo].m_iProcessID = iProcessID;
		iData = m_poMiddleCacheData->malloc();
		if (0 == iData){
			Log::log(0,"中间缓冲区的数据区太小，请调整");
			m_poMiddleIndexLock->V();
			THROW (MBC_CacheMgr+2);
		}
		m_poMiddleInfo[iInfo].m_iDataBegOffset = iData;
		m_poMiddleInfo[iInfo].m_iDataEndOffset = iData;
		m_poMiddleCache[iData].m_iAddressOffset = iAddress;
		m_poMiddleCache[iData].m_iNextOffset = 0;
		m_poMiddleIndex->add(iProcessID,iInfo);
		m_poMiddleIndexLock->V();
		return bRet;
	}
_FLAG1:
	m_poMiddleCacheDataLock->P();
	iData = m_poMiddleCacheData->malloc();
	m_poMiddleCacheDataLock->V();
	if (0 == iData){
		Log::log(0,"中间缓冲区的数据区太小，请调整");
		THROW (MBC_CacheMgr+2);
	}
	m_poMiddleCache[iData].m_iAddressOffset = iAddress;
	m_poMiddleCache[iData].m_iNextOffset = 0;
	m = iProcessID % PROCESS_NUM_MAX;
	m_poMiddleInfoDataLock[m]->P();
	if (m_poMiddleInfo[iInfo].m_iDataEndOffset >0 && m_poMiddleInfo[iInfo].m_iDataBegOffset != 0){
		m_poMiddleCache[m_poMiddleInfo[iInfo].m_iDataEndOffset].m_iNextOffset = iData;
		m_poMiddleInfo[iInfo].m_iDataEndOffset = iData;
	}else{
		m_poMiddleInfo[iInfo].m_iDataBegOffset = iData;
		m_poMiddleInfo[iInfo].m_iDataEndOffset = iData;
	}
	m_poMiddleInfoDataLock[m]->V();

	return bRet;
}

bool LocalCacheMgr::syncSuccess(long lEventID)
{
	return syncResult(lEventID,TYPE_CACHE_SUCCESS);
}

void LocalCacheMgr::syncSuccess(int iProcID)
{
	bool bRet = false;
	unsigned int iOffset = 1;

	AddressCacheData * pData = NULL;
	while(iOffset<=m_iAddressTotal)
	{
		pData = m_poAddressCache+iOffset;
        if (pData->m_iStdEventOffset ==0) 
        {
            iOffset++;
            continue;
        }
		if(iProcID == pData->m_iProcessID){
			if (TYPE_CACHE_SUCCESS != pData->m_iType){
				syncResult(pData->m_lEventID,TYPE_CACHE_SUCCESS);
			}
		}
        iOffset++;
	}
	return;
}

void LocalCacheMgr::setFailToSuccess(int iProcID)
{
	bool bRet = false;
	unsigned int iOffset = 1;

	AddressCacheData * pData = NULL;
	while(iOffset<=m_iAddressTotal)
	{
		pData = m_poAddressCache+iOffset;
		if (pData->m_iStdEventOffset ==0){
			iOffset++;
			continue;
		}
		if(iProcID == pData->m_iProcessID){
			if (TYPE_CACHE_FAIL == pData->m_iType){
				pData->m_iType = TYPE_CACHE_SUCCESS;
			}
		}
		iOffset++;
	}
	return;
}

bool LocalCacheMgr::syncSuccess(long lEventID,vector<AuditEventAccuData *> &vpAccuData,
		map<AuditDisctAccuData *,vector<AuditDisctAccuDetailData *> > &mpDisctData)
{
	unsigned int iAddress = 0;
	if (!m_poCacheIndex->get(lEventID,&iAddress))
	{
		Log::log(0,"严重错误：ABM返回的EVENT_ID=%ld 在本地缓存区索引中不存在，请检查原因。",lEventID);
		return false;
	}

	bool bFind = false;
	unsigned int iIndex = 0;
	int iSize = vpAccuData.size();
	for (int i=0;i<iSize;i++)
	{
		bFind = false;
		iIndex = m_poAddressCache[iAddress].m_iEventAccuOffset;
		while(iIndex){
			if (m_poAccuCache[iIndex].m_lAccuInstID == vpAccuData[i]->m_lAccuInstID){
				bFind = true;
				if(m_poAccuCache[iIndex].m_cOperation == 'i'){
					m_poAccuCache[iIndex].m_lAccuInstID = vpAccuData[i]->m_lABMKeyID;					
				}
				m_poAccuCache[iIndex].m_lVersionID = vpAccuData[i]->m_lVersionID;
			}
			iIndex = m_poAccuCache[iIndex].m_iNextOffset;
		}
		if(!bFind){
			Log::log(0,"严重错误：ABM返回的EVENT_ID=%ld 的事件累积量主健EVENT_AGGR_ID=%ld,ABM_KEY_ID=%ld 在本地缓存区中不存在，请检查原因。",
				lEventID,vpAccuData[i]->m_lAccuInstID,vpAccuData[i]->m_lABMKeyID);
		}
	}
	
	unsigned int iIndexDetail = 0;
	bool bFindDetail = false;
	map<AuditDisctAccuData *,vector<AuditDisctAccuDetailData *> >::iterator iter;
	vector<AuditDisctAccuDetailData *>::iterator iterDetail;
	for(iter = mpDisctData.begin(); iter != mpDisctData.end(); iter++)
	{
		bFind = false;
		iIndex = m_poAddressCache[iAddress].m_iDisctAggrOffset;
		while(iIndex){
			if (m_poDisctCache[iIndex].m_lItemDisctSeq == iter->first->m_lItemDisctSeq){
				bFind = true;
				if(m_poDisctCache[iIndex].m_cOperation == 'i'){
					m_poDisctCache[iIndex].m_lItemDisctSeq = iter->first->m_lABMKeyID;
				}
				m_poDisctCache[iIndex].m_lVersionID = iter->first->m_lVersionID;				
				for(iterDetail = (iter->second).begin();iterDetail != (iter->second).end();iterDetail++)
				{
					bFindDetail = false;
					iIndexDetail = m_poDisctCache[iIndex].m_iDetailOffset;
					while(iIndexDetail){
						if(m_poDisctDetailCache[iIndexDetail].m_lItemDetailSeq == ((AuditDisctAccuDetailData *)*iterDetail)->m_lItemDetailSeq){
							bFindDetail = true;
							if(m_poDisctDetailCache[iIndexDetail].m_cOperation == 'i'){
								m_poDisctDetailCache[iIndexDetail].m_lItemDetailSeq = ((AuditDisctAccuDetailData *)*iterDetail)->m_lABMKeyID;
								m_poDisctDetailCache[iIndexDetail].m_lItemDisctSeq = m_poDisctCache[iIndex].m_lItemDisctSeq;
							}
							m_poDisctDetailCache[iIndexDetail].m_lVersionID = ((AuditDisctAccuDetailData *)*iterDetail)->m_lVersionID;
						}
						iIndexDetail = m_poDisctDetailCache[iIndexDetail].m_iNextOffset;
					}
					if(!bFindDetail){
						Log::log(0,"严重错误：ABM返回的EVENT_ID=%ld 的优惠明细主健ITEM_DETAIL_SEQ=%ld,ABM_KEY_ID=%ld 在本地缓存区中不存在，请检查原因。",
                                                        lEventID,((AuditDisctAccuDetailData *)*iterDetail)->m_lItemDetailSeq,((AuditDisctAccuDetailData *)*iterDetail)->m_lABMKeyID);
					}
				}
			}
			iIndex = m_poDisctCache[iIndex].m_iNextOffset;
		}
		if(!bFind){
			Log::log(0,"严重错误：ABM返回的EVENT_ID=%ld 的优惠累积量主健ITEM_DISCT_SEQ=%ld,ABM_KEY_ID=%ld 在本地缓存区中不存在，请检查原因。",
			lEventID,iter->first->m_lItemDisctSeq,iter->first->m_lABMKeyID);
		}
	}

	return syncResult(lEventID,TYPE_CACHE_SUCCESS);
}

bool LocalCacheMgr::syncFail(long lEventID)
{
	return syncResult(lEventID,TYPE_CACHE_FAIL);
}

void LocalCacheMgr::init()
{
	m_iAddressOffset = 0;
	m_poEventPriceLast = 0;
	m_poAccuLast = 0;
	m_poAcctItemLast = 0;
	m_poDisctLast = 0;
	m_poDisctDetailLast = 0;
	m_poAddAddress = 0;
}

void LocalCacheMgr::initTempP()
{
	m_poEventPriceLast = 0;
	m_poAccuLast = 0;
	m_poAcctItemLast = 0;
	m_poDisctLast = 0;
	m_poDisctDetailLast = 0;
}

void LocalCacheMgr::initEventSource()
{
	m_viAddressFail.clear();
	m_viAddressWait.clear();
}

bool LocalCacheMgr::commit()
{
	m_poCacheIndexLock->P();
	m_poCacheIndex->add(m_poAddAddress->m_lEventID,m_iAddressOffset);
	m_poCacheIndexLock->V();
	Date d;//这里有点浪费
	m_poAddAddress->m_lBeginDate =  atol(d.toString());
	m_poAddAddress->m_iType = TYPE_CACHE_RDY;
	return true;
}

bool LocalCacheMgr::rollback()
{
	unlockKey(m_poAddAddress->m_iLockKeyOffset);
	return clearCache(m_poAddAddress->m_lEventID);
}

bool LocalCacheMgr::clearCache(long lEventID,bool bunlock)
{
	unsigned int i = 0,iTemp = 0,iAddressOffset = 0;
	if (!m_poCacheIndex->get(lEventID,&iAddressOffset)) return true;
	AddressCacheData * pAddress = m_poAddressCache+iAddressOffset;
	pAddress->m_iType = TYPE_CACHE_INIT;
	if (pAddress->m_iStdEventOffset){
		m_poEventCacheData->revoke(pAddress->m_iStdEventOffset);
	}
	if (pAddress->m_iEventPriceOffset){
		i = pAddress->m_iEventPriceOffset;
		while(i){
			iTemp = m_poEventPriceCache[i].m_iNextOffset;
			m_poEventPriceCacheData->revoke(i);
			i = iTemp;
		}
	}
	if (pAddress->m_iEventAccuOffset){
		i = pAddress->m_iEventAccuOffset;
		while(i){
			iTemp = m_poAccuCache[i].m_iNextOffset;
			m_poAccuCacheData->revoke(i);
			i = iTemp;
		}
	}
	if (pAddress->m_iDisctAggrOffset)
	{
		i = pAddress->m_iDisctAggrOffset;
		unsigned int iDetail = 0,iDetailTemp = 0;
		while(i)
		{
			iTemp = m_poDisctCache[i].m_iNextOffset;
			iDetail = m_poDisctCache[i].m_iDetailOffset;
			while(iDetail){
				iDetailTemp = m_poDisctDetailCache[iDetail].m_iNextOffset;
				m_poDisctDetailCacheData->revoke(iDetail);
				iDetail = iDetailTemp;
			}
			m_poDisctCacheData->revoke(i);
			i = iTemp;
		}
	}
	if (pAddress->m_iAcctItemOffset){
		i = pAddress->m_iAcctItemOffset;
		while(i){
			iTemp = m_poAcctItemCache[i].m_iNextOffset;
			m_poAcctItemCacheData->revoke(i);
			i = iTemp;
		}
	}	
	if (pAddress->m_iLockKeyOffset){		
		i = pAddress->m_iLockKeyOffset;
		if(bunlock){
			unlockKey(i);
		}
		while(i){
			iTemp = m_poKeyCache[i].m_iNextOffset;
			m_poKeyCacheData->revoke(i);
			i = iTemp;
		}
	}
	m_poAddressCacheData->revoke(iAddressOffset);
	m_poCacheIndex->revokeIdx(lEventID,i);
	return true;
}

void LocalCacheMgr::insertData(unsigned int iOffset)
{
	static BillingCycleMgr oMgr;
	static BillingCycle * pBillingCycle = NULL;

	#ifdef HAVE_ABM
	unsigned int i = 0,j = 0;
	long lPK = 0;
	bool bNew;

	i = m_poAddressCache[iOffset].m_iStdEventOffset;

	if (NULL == pBillingCycle || 
			pBillingCycle->getBillingCycleID() != m_poEventCache[i].m_iBillingCycleID){
				pBillingCycle = oMgr.getBillingCycle(m_poEventCache[i].m_iBillingCycleID);				
	}

	if (!G_PUSERINFO->getServ (G_SERV, m_poEventCache[i].m_iServID, 
		pBillingCycle->getEndDate())){
			G_PSERV = NULL;
	}

	int iNum = 0;
	bool bNewEvent = true;//事件是否未发送过
	memset(m_oItemData,0,sizeof(m_oItemData));
	int iEvent = m_poAddressCache[iOffset].m_iStdEventOffset;
	i = m_poAddressCache[iOffset].m_iAcctItemOffset;
	while(i){
		if (G_PSERV != NULL){//帐户修正
			m_poAcctItemCache[i].m_oData.lAcctID = G_PSERV->getAcctID(m_poAcctItemCache[i].m_oData.iPayItemTypeID,
				pBillingCycle->getEndDate());
		}
		lPK = m_poAcctItemMgr->update(&(m_poAcctItemCache[i].m_oData), bNew);
		m_poAcctItemCache[i].m_oData.lAcctItemID = lPK;
        if (bNew) {
            m_poAcctItemCache[i].m_oData.cOperation = 'i';
        } else {
            m_poAcctItemCache[i].m_oData.cOperation = 'u';
        }
		if (iNum<MAX_ITEM_RECORD){
			memcpy(&m_oItemData[iNum],&m_poAcctItemCache[i].m_oData,sizeof(AcctItemAddAggrData));
			iNum ++;
		}else{			
			m_poSender->send(m_oItemData,&m_poEventCache[iEvent],bNewEvent);
			memset(m_oItemData,0,sizeof(m_oItemData));
			iNum = 0;
			memcpy(&m_oItemData[iNum],&m_poAcctItemCache[i].m_oData,sizeof(AcctItemAddAggrData));
			iNum ++;
			bNewEvent = false;
		}
		i = m_poAcctItemCache[i].m_iNextOffset;
	}

	if(iNum>0){
		m_poSender->send(m_oItemData,&m_poEventCache[iEvent],bNewEvent);
	}
	

	i = m_poAddressCache[iOffset].m_iEventAccuOffset;
	while(i){
		if (m_poAccuCache[i].m_cOperation != 'r'){
			m_poAccuMgr->updateAccByCache(&(m_poAccuCache[i]));
		}
		i = m_poAcctItemCache[i].m_iNextOffset;
	}

	i = m_poAddressCache[iOffset].m_iDisctAggrOffset;
	unsigned int iAggrOffset;
	while(i){
		if (m_poDisctCache[i].m_cOperation == 'u' || m_poDisctCache[i].m_cOperation == 'i'){
			iAggrOffset = G_POPRODOFFFERDISCT->updateAggrByCache(&(m_poDisctCache[i]));
			j = m_poDisctCache[i].m_iDetailOffset;
			if (0 == iAggrOffset) {
				Log::log(0,"ERROR：商品实例ID[%ld]的共享区为空",m_poDisctCache[i].m_lOfferInstID);
				i = m_poDisctCache[i].m_iNextOffset;
				continue;
			}
			while(j){
				G_POPRODOFFFERDISCT->updateDetailByCache(&(m_poDisctDetailCache[j]),iAggrOffset);
				j = m_poDisctDetailCache[j].m_iNextOffset;
			}
		}
		i = m_poDisctCache[i].m_iNextOffset;
	}

__BEGIN:
	i = m_poAddressCache[iOffset].m_iStdEventOffset;
	if(!m_poInDB->addEvent(&m_poEventCache[i])){
		Log::log (1, "add event fail");
		goto __FAILURE;
	}

	i = m_poAddressCache[iOffset].m_iEventPriceOffset;
	while(i){
		if(!m_poInDB->addEventPrice(&(m_poEventPriceCache[i].m_oData))){
			Log::log (1, "add event price fail");
			goto __FAILURE;
		}
		i = m_poEventPriceCache[i].m_iNextOffset;
	}
	
	i = m_poAddressCache[iOffset].m_iAcctItemOffset;
	while(i){
		if (!m_poInDB->addAcctItemAddAggr (&(m_poAcctItemCache[i].m_oData))) {
			Log::log (1, "add acct item fail");
			goto __FAILURE;
		}
		i = m_poAcctItemCache[i].m_iNextOffset;
	}

	i = m_poAddressCache[iOffset].m_iEventAccuOffset;
	static EventAddAggrData oAccuData;
	while(i){
		oAccuData.cOperation = m_poAccuCache[i].m_cOperation;
		oAccuData.iFileID = m_poAccuCache[i].m_iFileID;
		oAccuData.iEventSourceType = m_poAccuCache[i].m_iEventSourceType;
		oAccuData.iBillingCycleID = m_poAccuCache[i].m_iBillingCycleID;
		oAccuData.iOrgID = m_poAccuCache[i].m_iOrgID;
		oAccuData.lEventID = m_poAccuCache[i].m_lEventID;
		oAccuData.lAccInstID = m_poAccuCache[i].m_lAccuInstID;
		oAccuData.lOfferInstID = m_poAccuCache[i].m_lOfferInstID;
		oAccuData.lCycleInstID = m_poAccuCache[i].m_lCycleInstID;
		oAccuData.lInstMemberID = m_poAccuCache[i].m_lMemberID;
		oAccuData.lServID = m_poAccuCache[i].m_lServID;
		oAccuData.lValue = m_poAccuCache[i].m_lAddValue;
		oAccuData.iAccTypeID = m_poAccuCache[i].m_iAccuTypeID;
		strcpy(oAccuData.sSplitBy , m_poAccuCache[i].m_sSplitby);
		strcpy(oAccuData.sStateDate , m_poAccuCache[i].m_sStateDate);
        if (!m_poInDB->addEventAddAggr (&oAccuData)){
			Log::log (1, "add event add aggr fail");
			goto __FAILURE;
		}
		i = m_poAccuCache[i].m_iNextOffset;
	}
	
	i = m_poAddressCache[iOffset].m_iDisctAggrOffset;
	static AcctDisctAggrData oDisctData;
	static AcctDisctDetailData oDetailData;
	while(i){
		oDisctData.cOperation = m_poDisctCache[i].m_cOperation;
		oDisctData.iFileID = m_poDisctCache[i].m_iFileID;
		oDisctData.iEventSourceType = m_poDisctCache[i].m_iEventSourceType;
		oDisctData.iBillingCycleID = m_poDisctCache[i].m_iBillingCycleID;
		oDisctData.iOrgID = m_poDisctCache[i].m_iOrgID;
		oDisctData.lItemDisctSeq = m_poDisctCache[i].m_lItemDisctSeq;
		oDisctData.lProdOfferID = m_poDisctCache[i].m_lOfferInstID;
		oDisctData.iProdOfferCombine = m_poDisctCache[i].m_iCombineID;
		oDisctData.iPricingMeasure = m_poDisctCache[i].m_iMeasureID;
		oDisctData.iItemGroupID = m_poDisctCache[i].m_iItemGroupID;
		oDisctData.iInType = m_poDisctCache[i].m_iInType;
		oDisctData.lInValue = m_poDisctCache[i].m_lInValue;
		oDisctData.lValue = m_poDisctCache[i].m_lAddValue;
		oDisctData.lValue = m_poDisctCache[i].m_lAddValue;
		oDisctData.lDisctValue = m_poDisctCache[i].m_lAddDisctValue;
		if (!m_poInDB->addAcctDisctAggr (&oDisctData)) {
			Log::log (1, "add acct disct aggr  fail");
			goto __FAILURE;
		}
		j = m_poDisctCache[i].m_iDetailOffset;
		while(j){
			oDetailData.cOperation = m_poDisctDetailCache[j].m_cOperation;
			oDetailData.iFileID = m_poDisctDetailCache[j].m_iFileID;
			oDetailData.iEventSourceType = m_poDisctDetailCache[j].m_iEventSourceType;
			oDetailData.iBillingCycleID = m_poDisctDetailCache[j].m_iBillingCycleID;
			oDetailData.iOrgID = m_poDisctDetailCache[j].m_iOrgID;
			oDetailData.lEventID = m_poDisctDetailCache[j].m_lEventID;
			oDetailData.lServID = m_poDisctDetailCache[j].m_lServID;
			oDetailData.iAcctItemTypeID = m_poDisctDetailCache[j].m_iAcctItemTypeID;
			oDetailData.lInCharge = m_poDisctDetailCache[j].m_lAddValue;
			oDetailData.lOutCharge = m_poDisctDetailCache[j].m_lAddDisctValue;
			oDetailData.lItemDisctSeq = m_poDisctDetailCache[j].m_lItemDisctSeq;
			if (!m_poInDB->addAcctDisctDetail (&oDetailData)) {
				Log::log (1, "add acct disct detail fail");
				goto __FAILURE;
			}
			j = m_poDisctDetailCache[j].m_iNextOffset;
		}
		i = m_poDisctCache[i].m_iNextOffset;
	}

__OK:
	m_poInDB->commit ();

	return;

__FAILURE:
    m_poInDB->rollback ();

    m_poInDB->forceCommit ();
    while (!m_poInDB->ifCanWrite ()) {
        usleep (1000);
    }

    goto __BEGIN;
	#endif
	return;
}

void LocalCacheMgr::unlockKey(unsigned int iOffset)
{
	unsigned int i = iOffset;
	static long lKeyBuf[MAX_OFFER_INSTANCE_NUMBER];
	int iNum = 0;
	while(i>0){
		lKeyBuf[iNum++]=m_poKeyCache[i].m_lKey;
		i = m_poKeyCache[i].m_iNextOffset;
	}
	if(iNum >0){
		m_poLockMgr->unLockAccu(lKeyBuf,iNum);
	}
	return;
}

void LocalCacheMgr::relaWaitProc(long lEventID,bool bRelaOther)
{
	unsigned int i,iData,iInfo;
	if(!m_poEventWaitIndex->get(lEventID,&i)) return;
	m_poEventWaitLock->P();
	m_poEventWaitIndex->revokeIdx(lEventID,iInfo);
	m_poEventWaitLock->V();
	while(i)
	{
		if (m_iProcID == m_poEventWait[i].m_oData.m_iProcessID){
			m_viAddressWait.push_back(i);
		}else
		{
			if (!m_poMiddleIndex->get(m_poEventWait[i].m_oData.m_iProcessID,&iInfo))
			{
				m_poMiddleIndexLock->P();
				if (m_poMiddleIndex->get(m_poEventWait[i].m_oData.m_iProcessID,&iInfo)){
					m_poMiddleIndexLock->V();
					goto _FLAG1;
				}
				iInfo = m_poMiddleInfoData->malloc();
				if (0 == iInfo){
					Log::log(0,"进程数太多，没有可以使用的中间缓冲信息区，请调整");
					m_poMiddleIndexLock->V();
					THROW (MBC_CacheMgr+1);
				}
				m_poMiddleInfo[iInfo].m_iProcessID = m_poEventWait[i].m_oData.m_iProcessID;
				unsigned int iData = m_poMiddleCacheData->malloc();
				if (0 == iData){
					Log::log(0,"中间缓冲区的数据区太小，请调整");
					m_poMiddleIndexLock->V();
					THROW (MBC_CacheMgr+2);
				}
				m_poMiddleInfo[iInfo].m_iDataBegOffset = 0;
				m_poMiddleInfo[iInfo].m_iDataEndOffset = 0;
				m_poMiddleInfo[iInfo].m_iWaitOffset = iData;
				m_poMiddleCache[iData].m_iAddressOffset = 0;
				m_poMiddleCache[iData].m_iNextOffset = 0;
				m_poMiddleCache[iData].m_iWaitEventOffset = i;
				m_poMiddleIndex->add(m_poMiddleInfo[iInfo].m_iProcessID,iInfo);
				m_poMiddleIndexLock->V();
				i = m_poEventWait[i].m_iNextOffset;	
				continue;
			}
			_FLAG1:
			m_poMiddleCacheDataLock->P();
			iData = m_poMiddleCacheData->malloc();
			m_poMiddleCacheDataLock->V();
			if (0 == iData){
				Log::log(0,"中间缓冲区的数据区太小，请调整");
				THROW (MBC_CacheMgr+2);
			}
			m_poMiddleCache[iData].m_iAddressOffset = 0;
			m_poMiddleCache[iData].m_iNextOffset = 0;
			m_poMiddleCache[iData].m_iWaitEventOffset = i;
			m_poMiddleIndexLock->P();
			if (0 == m_poMiddleInfo[iInfo].m_iWaitOffset){
				m_poMiddleInfo[iInfo].m_iWaitOffset = iData;
			}else{
				m_poMiddleCache[m_poMiddleInfo[iInfo].m_iWaitOffset].m_iNextOffset = iData;
			}
			m_poMiddleIndexLock->V();
		}
		i = m_poEventWait[i].m_iNextOffset;	
	}

	if (bRelaOther) return;
	unsigned int iTemp = 0;
	m_poMiddleIndex->get(m_iProcID,&iInfo);
	i = m_poMiddleInfo[iInfo].m_iWaitOffset;
	if (i>0)
	{
		m_poMiddleIndexLock->P();
		m_poMiddleInfo[iInfo].m_iWaitOffset = 0;
		m_poMiddleIndexLock->V();
		while(i){
			iTemp = i;
			m_viAddressWait.push_back(m_poMiddleCache[i].m_iWaitEventOffset);
			i = m_poMiddleCache[i].m_iNextOffset;
			m_poMiddleCacheData->revoke(iTemp);
		}
	}
	return;
}

bool LocalCacheMgr::cacheProc()
{
	bool bRet = false;
	unsigned int i = 0,iHead = 0,iAddress = 0,iTemp = 0;
	if (!m_poMiddleIndex->get(m_iProcID,&i)) return bRet;

	if (m_poMiddleInfo[i].m_iDataEndOffset>0)
	{
		int m = m_iProcID % PROCESS_NUM_MAX;
		m_poMiddleInfoDataLock[m]->P();
		if (m_poMiddleInfo[i].m_iDataBegOffset >0){
			iHead = m_poMiddleInfo[i].m_iDataBegOffset;
			m_poMiddleInfo[i].m_iDataBegOffset = 0;
			m_poMiddleInfo[i].m_iDataEndOffset = 0;
		}
		m_poMiddleInfoDataLock[m]->V();
	}
	while(iHead>0)
	{
		bRet = true;
		iTemp = iHead;
		iAddress = m_poMiddleCache[iHead].m_iAddressOffset;
		switch (m_poAddressCache[iAddress].m_iType)
		{
		case TYPE_CACHE_SUCCESS:
			insertData(iAddress);
			unlockKey(m_poAddressCache[iAddress].m_iLockKeyOffset);
			relaWaitProc(m_poAddressCache[iAddress].m_lEventID);
			clearCache(m_poAddressCache[iAddress].m_lEventID);
			break;
		case TYPE_CACHE_FAIL:
			m_viAddressFail.push_back(m_poAddressCache[iAddress].m_iStdEventOffset);
			break;
		default:
			Log::log(0,"EVNET_ID=%ld,的本地缓存数据状态不正确（状态:%d），请检查。",
					m_poAddressCache[iAddress].m_lEventID,m_poAddressCache[iAddress].m_iType);
		}
		iHead = m_poMiddleCache[iHead].m_iNextOffset;
		m_poMiddleCacheData->revoke(iTemp);
	}
	return bRet;
}

void LocalCacheMgr::convertWaitEvent(int iLocation,StdEvent ** pEvent)
{
	unsigned int iValue = m_viAddressWait[iLocation];
	memcpy(*pEvent,&(m_poEventWait[iValue].m_oData),sizeof(StdEvent));
	m_poEventWaitData->revoke(iValue);
	return;
}

void LocalCacheMgr::convertFailEvent(int iLocation,StdEvent ** pEvent)
{
	unsigned int iValue = m_viAddressFail[iLocation];
	memcpy(*pEvent,&(m_poEventCache[iValue]),sizeof(StdEvent));
	clearCache((*pEvent)->m_lEventID,true);
	return;
}

bool LocalCacheMgr::overdue(const Date & time, long &lEventID){
    static unsigned int iTotal = m_poAddressCacheData->getTotal();
    static unsigned int cursor = 1;
    static char temp[20] = {0};

    if (cursor > iTotal){
        cursor = 1;
    }

    while (cursor < iTotal){
        if (lEventID = m_poAddressCache[cursor].m_lEventID) {
            sprintf(temp, "%ld", m_poAddressCache[cursor].m_lBeginDate);
            Date curTime(temp);
            if ( curTime <= time ) {
                cursor++;
                return true;
            }
        }
        cursor++;
    }

    return false;
}
void LocalCacheMgr::create()
{
	if (!m_poCacheIndex->exist()) {
		m_poCacheIndex->create(CHCHE_NUM_MAX);
	}
	if (!m_poAddressCacheData->exist()) {
		m_poAddressCacheData->create(CHCHE_NUM_MAX);
	}
	if (!m_poEventWaitIndex->exist()) {
		m_poEventWaitIndex->create(CHCHE_NUM_MAX);
	}
	if (!m_poEventWaitData->exist()) {
		m_poEventWaitData->create(CHCHE_NUM_MAX*EVENT_WAIT_FACTOR);
	}
	if (!m_poEventCacheData->exist()) {
		m_poEventCacheData->create(CHCHE_NUM_MAX);
	}
	if (!m_poEventPriceCacheData->exist()) {
		m_poEventPriceCacheData->create(CHCHE_NUM_MAX*EVENT_PRICE_FACTOR);
	}
	if (!m_poAccuCacheData->exist()) {
		m_poAccuCacheData->create(CHCHE_NUM_MAX*EVENT_AGGR_FACTOR);
	}
	if (!m_poAcctItemCacheData->exist()) {
		m_poAcctItemCacheData->create(CHCHE_NUM_MAX*ACCT_ITEM_FACTOR);
	}
	if (!m_poDisctCacheData->exist()) {
		m_poDisctCacheData->create(CHCHE_NUM_MAX*DISCT_AGGR_FACTOR);
	}
	if (!m_poDisctDetailCacheData->exist()) {
		m_poDisctDetailCacheData->create(CHCHE_NUM_MAX*DISCT_AGGR_FACTOR*DISCT_DETAIL_FACTOR);
	}
	if (!m_poMiddleIndex->exist()) {
		m_poMiddleIndex->create(PROCESS_NUM_MAX);
	}
	if (!m_poMiddleInfoData->exist()) {
		m_poMiddleInfoData->create(PROCESS_NUM_MAX);
	}
	if (!m_poMiddleCacheData->exist()) {
		m_poMiddleCacheData->create(CHCHE_NUM_MAX+CHCHE_NUM_MAX*EVENT_WAIT_FACTOR);
	}
	if (!m_poKeyCacheData->exist()) {
		m_poKeyCacheData->create(CHCHE_NUM_MAX*LOCK_KEY_FACTOR);
	}

	m_bAttached = true;
	return;
}

void LocalCacheMgr::remove()
{
	if (m_poCacheIndex->exist ()) {
		m_poCacheIndex->remove ();
	}
	if (m_poAddressCacheData->exist ()) {
		m_poAddressCacheData->remove ();
	}
	if (m_poEventWaitIndex->exist ()) {
		m_poEventWaitIndex->remove ();
	}
	if (m_poEventWaitData->exist ()) {
		m_poEventWaitData->remove ();
	}
	if (m_poEventCacheData->exist ()) {
		m_poEventCacheData->remove ();
	}
	if (m_poEventPriceCacheData->exist ()) {
		m_poEventPriceCacheData->remove ();
	}
	if (m_poAccuCacheData->exist ()) {
		m_poAccuCacheData->remove ();
	}
	if (m_poAcctItemCacheData->exist ()) {
		m_poAcctItemCacheData->remove ();
	}
	if (m_poDisctCacheData->exist ()) {
		m_poDisctCacheData->remove ();
	}
	if (m_poDisctDetailCacheData->exist ()) {
		m_poDisctDetailCacheData->remove ();
	}
	if (m_poMiddleIndex->exist ()) {
		m_poMiddleIndex->remove ();
	}
	if (m_poMiddleInfoData->exist ()) {
		m_poMiddleInfoData->remove ();
	}
	if (m_poMiddleCacheData->exist ()) {
		m_poMiddleCacheData->remove ();
	}
	if (m_poKeyCacheData->exist ()) {
		m_poKeyCacheData->remove ();
	}
	return;
}
