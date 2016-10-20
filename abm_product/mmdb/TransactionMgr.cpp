/*VER: 2*/ 
#include <stdio.h>
#include "Environment.h"
#include "CommonMacro.h"
#include "Process.h"
#include "Log.h"
#include "MBC.h"
#include "AcctItemMgr.h"
#include "TransactionMgr.h"
#include "DataIncrementMgr.h"

using namespace std;

//##ModelId=42368E37000C
TransactionMgr::TransactionMgr()
{
}

//##ModelId=42368E41027D
TransactionMgr::~TransactionMgr()
{
}

//事件累积量
int TransactionMgr::selectAllEventAggr(long iServID, int iBillingCycleID, 
vector<AccuData *> & v)
{
    return 0;
}

//##ModelId=4237EA0502C1
long TransactionMgr::selectEventAggr(long iServID,int iBillingCycleID,
int iEventAggrTypeID)
{
    return 0;
}

//用户帐目累积理
//##ModelId=4237EF2901DD
long TransactionMgr::selectAcctItemAggr(long lServID,int iBillingCycleID,
int iAcctItemTypeID)
{
    return AcctItemAccuMgr::get (lServID, iBillingCycleID, iAcctItemTypeID);
}

int TransactionMgr::selectDisctItemAggr(long lServID, int iBillingCycleID, 
vector<AcctItemData *> & v)
{
    return AcctItemAccuMgr::getDisct (lServID, iBillingCycleID, v);
}

int TransactionMgr::selectAllItemAggr(long lServID, int iBillingCycleID, 
vector<AcctItemData *> &v,int iOrgBillingCycleID)
{
    return AcctItemAccuMgr::getAll (lServID, iBillingCycleID, v,iOrgBillingCycleID);
}

int TransactionMgr::selectHisItemAggr(long lServID, 
    vector < AcctItemData * > &v)
{
    return AcctItemAccuMgr::getHis(lServID,v);    
}

//事件累积量
long AggrMgr::getEventAggr(long lServID,int iBillingCycleID,
                                        int iEventAggrTypeID)
{
    return 0;
}

int AggrMgr::getAllEventAggr(long lServID, int iBillingCycleID,
                                    vector<AccuData *> & v)
{
    return 0;
}
	
//帐目累积量
long AggrMgr::getAcctItemAggr(long lServID,int iBillingCycleID,
                                            int iAcctItemTypeID)
{
    return AcctItemAccuMgr::get (lServID, iBillingCycleID,
                                    iAcctItemTypeID);
}

int AggrMgr::getDisctItemAggr(long lServID, int iBillingCycleID,
                                vector<AcctItemData *> & v)
{
    return AcctItemAccuMgr::getDisct (lServID, iBillingCycleID, v);
}

long AggrMgr::getPartyRoleAggr(long lServID, int iBillingCycleID,int iPartyRoleID)
{
    return AcctItemAccuMgr::getAcctItemDataPartyRoleID(
    	lServID, iBillingCycleID, iPartyRoleID);
}

int AggrMgr::getAllItemAggr(long lServID, int iBillingCycleID,
                                vector<AcctItemData *> & v,int iOrgBillingCycleID)
{
    return AcctItemAccuMgr::getAll (lServID, iBillingCycleID, v,iOrgBillingCycleID);
}

long AggrMgr::insertAcctItemAggr(AcctItemData &itemData, bool &bNew, bool isModifyAcct)
{
	bool b=false;
	long lAcctItemID;
	AcctItemAddAggrData * pAggrData = new AcctItemAddAggrData();
	//此处对于pAggrData中没有定义的信息在AcctItemAccuMgr::update中目前除（lOfferInstID）都用不到 
	pAggrData->iBillingCycleID=itemData.m_iBillingCycleID;
	pAggrData->iItemSourceID=itemData.m_iItemSourceID;
	pAggrData->iItemClassID=itemData.m_iItemClassID;
	pAggrData->lAcctItemID=itemData.m_lAcctItemID;
	pAggrData->lServID=itemData.m_lServID;
	pAggrData->lAcctID=itemData.m_lAcctID;
	pAggrData->lOfferInstID=itemData.m_lOfferInstID;
	pAggrData->lCharge=itemData.m_lValue;
	pAggrData->iPayMeasure=itemData.m_iPayMeasure;
	pAggrData->iPayItemTypeID=itemData.m_iPayItemTypeID;
#ifdef ORG_BILLINGCYCLE
    pAggrData->m_iOrgBillingCycleID = itemData.m_iOrgBillingCycleID;
#endif
	sprintf(pAggrData->sStateDate,itemData.m_sStateDate);
	//内存修改
	lAcctItemID = AcctItemAccuMgr::update(pAggrData,bNew,isModifyAcct);
	delete pAggrData;
	pAggrData = NULL;
	return lAcctItemID;
}
