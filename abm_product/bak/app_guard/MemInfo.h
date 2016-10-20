/*VER: 1*/ 
#ifndef __meminfo_h__
#define __meminfo_h__


#include "OfferAccuMgr.h"
#include "AcctItemAccu.h"
#include "ProdOfferAggr.h"
#include "UserInfo.h"
#include "BillingCycleMgr.h"

//优惠记录明细数据
#define PROD_OFFER_DETAILBUF_KEY		IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTBUF")
//优惠数据数据区
#define PROD_OFFER_AGGRBUF_KEY 			IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERTOTALBUF")
//优惠数据索引区
#define PROD_OFFER_DISCT_INDEX_KEY 	IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTINDEX")


class MemInfo : public UserInfoBase,public OfferAccuBase,public AcctItemBase,public ProdOfferDisctAggr
{
public:
	MemInfo(){};
	~MemInfo(){};
public:
	int GetMemInfo(char* strIpcKey,long &lUsed,long &lTotal,int &iConnNum);
	void FreeMem(char* strIpcKey);
	int GetStrIdxInfo(char *strIpcKey,long &lListUsed,long &lListTotal);
};

int MemInfo::GetMemInfo(char* strIpcKey,long &lUsed,long &lTotal,int &iConnNum)
{
	if(strcmp(strIpcKey,"SHM_CUST_INFO_DATA")==0&&m_poCustData->exist())
	{
		lTotal = m_poCustData->getTotal();
		lUsed  = m_poCustData->getCount();
		iConnNum = m_poCustData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_CUST_INFO_INDEX")==0&&m_poCustIndex->exist())
	{
		lTotal = m_poCustIndex->getTotal();
		lUsed  = m_poCustIndex->getCount();
		iConnNum = m_poCustIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_CUSTATTR_INFO_DATA")==0&&m_poCustAttrData->exist())
	{
		lTotal = m_poCustAttrData->getTotal();
		lUsed  = m_poCustAttrData->getCount();
		iConnNum = m_poCustAttrData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODUCT_INFO_DATA")==0&&m_poProductData->exist())
	{
		lTotal = m_poProductData->getTotal();
		lUsed  = m_poProductData->getCount();
		iConnNum = m_poProductData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODUCT_INFO_INDEX")==0&&m_poProductIndex->exist())
	{	
		lTotal = m_poProductIndex->getTotal();
		lUsed  = m_poProductIndex->getCount();	
		iConnNum = m_poProductIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERV_INFO_DATA")==0&&m_poServData->exist())
	{
		lTotal = m_poServData->getTotal();
		lUsed  = m_poServData->getCount();	
		iConnNum = m_poServData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERV_INFO_INDEX")==0&&UserInfoBase::m_poServIndex->exist())
	{
		lTotal = UserInfoBase::m_poServIndex->getTotal();
		lUsed  = UserInfoBase::m_poServIndex->getCount();	
		iConnNum = UserInfoBase::m_poServIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVLOCATION_INFO_DATA")==0&&m_poServLocationData->exist())
	{
		lTotal = m_poServLocationData->getTotal();
		lUsed  = m_poServLocationData->getCount();	
		iConnNum = m_poServLocationData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVSTATE_INFO_DATA")==0&&m_poServStateData->exist())
	{
		lTotal = m_poServStateData->getTotal();
		lUsed  = m_poServStateData->getCount();	
		iConnNum = m_poServStateData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVATTR_INFO_DATA")==0&&m_poServAttrData->exist())
	{
		lTotal = m_poServAttrData->getTotal();
		lUsed  = m_poServAttrData->getCount();	
		iConnNum = m_poServAttrData->connnum();
		return 1;
	}

	else if(strcmp(strIpcKey,"SHM_SERVIDENT_INFO_DATA")==0&&m_poServIdentData->exist())
	{
		lTotal = m_poServIdentData->getTotal();
		lUsed  = m_poServIdentData->getCount();	
		iConnNum = m_poServIdentData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVIDENT_INFO_INDEX")==0&&m_poServIdentIndex->exist())
	{
		lTotal = m_poServIdentIndex->getTotal();
		lUsed  = m_poServIdentIndex->getCount();	
		iConnNum = m_poServIdentIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCT_INFO_DATA")==0&&m_poServProductData->exist())
	{
		lTotal = m_poServProductData->getTotal();
		lUsed  = m_poServProductData->getCount();	
		iConnNum = m_poServProductData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCT_INFO_INDEX")==0&&m_poServProductIndex->exist())
	{
		lTotal = m_poServProductIndex->getTotal();
		lUsed  = m_poServProductIndex->getCount();	
		iConnNum = m_poServProductIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCTATTR_INFO_DATA")==0&&m_poServProductAttrData->exist())
	{
		lTotal = m_poServProductAttrData->getTotal();
		lUsed  = m_poServProductAttrData->getCount();	
		iConnNum = m_poServProductAttrData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVACCT_INFO_DATA")==0&&m_poServAcctData->exist())
	{
		lTotal = m_poServAcctData->getTotal();
		lUsed  = m_poServAcctData->getCount();	
		iConnNum = m_poServAcctData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_ACCT_INFO_DATA")==0&&m_poAcctData->exist())
	{
		lTotal = m_poAcctData->getTotal();
		lUsed  = m_poAcctData->getCount();	
		iConnNum = m_poAcctData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_ACCT_INFO_INDEX")==0&&m_poAcctIndex->exist())
	{
		lTotal = m_poAcctIndex->getTotal();
		lUsed  = m_poAcctIndex->getCount();	
		iConnNum = m_poAcctIndex->connnum();
		return 1;
	}

	//add by jinx 070412
	else if(strcmp(strIpcKey,"SHM_AccuData")==0&&m_poAcctIndex->exist())
	{
		lTotal = m_poAccuData->getTotal();
		lUsed  = m_poAccuData->getCount();	
		iConnNum = m_poAccuData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_O")==0&&m_poAccuIndex->exist())
	{
		lTotal = m_poAccuIndex->getTotal();
		lUsed  = m_poAccuIndex->getCount();	
		iConnNum = m_poAccuIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_S")==0&&m_poAccuServIndex->exist())
	{
		lTotal = m_poAccuServIndex->getTotal();
		lUsed  = m_poAccuServIndex->getCount();	
		iConnNum = m_poAccuServIndex->connnum();
		return 1;
	}	
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_I")==0&&m_poAccuInstIndex->exist())
	{
		lTotal = m_poAccuInstIndex->getTotal();
		lUsed  = m_poAccuInstIndex->getCount();	
		iConnNum = m_poAccuInstIndex->connnum();
		return 1;
	}			
	else if(strcmp(strIpcKey,"SHM_AccuCycleData")==0&&m_poCycleInstData->exist())
	{
		lTotal = m_poCycleInstData->getTotal();
		lUsed  = m_poCycleInstData->getCount();	
		iConnNum = m_poCycleInstData->connnum();
		return 1;
	}	
	else if(strcmp(strIpcKey,"SHM_AccuCycleData_Index")==0&&m_poCycleInstIndex->exist())
	{
		lTotal = m_poCycleInstIndex->getTotal();
		lUsed  = m_poCycleInstIndex->getCount();	
		iConnNum = m_poCycleInstIndex->connnum();
		return 1;
	}

	else if(strcmp(strIpcKey,"SHM_AcctItemData")==0&&m_poAcctItemData->exist())
	{
		lTotal = m_poAcctItemData->getTotal();
		lUsed  = m_poAcctItemData->getCount();	
		iConnNum = m_poAcctItemData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_AcctItemIndex_S")==0&&AcctItemBase::m_poServIndex->exist())
	{
		lTotal = AcctItemBase::m_poServIndex->getTotal();
		lUsed  = AcctItemBase::m_poServIndex->getCount();	
		iConnNum = AcctItemBase::m_poServIndex->connnum();
		return 1;
	}
	
	//define in ProdOfferAggr.h(meminfo.h)
	else if(strcmp(strIpcKey,"SHM_PRODOFFERDISCTBUF")==0&&m_poProdOfferDetailData->exist())
	{
		lTotal = m_poProdOfferDetailData->getTotal();
		lUsed  = m_poProdOfferDetailData->getCount();	
		iConnNum = m_poProdOfferDetailData->connnum();
		return 1;
	}

	else if(strcmp(strIpcKey,"SHM_PRODOFFERTOTALBUF")==0&&m_poProdOfferAggrData->exist())
	{
		lTotal = m_poProdOfferAggrData->getTotal();
		lUsed  = m_poProdOfferAggrData->getCount();	
		iConnNum = m_poProdOfferAggrData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERDISCTINDEX")==0&&m_poProdOfferAggrIndex->exist())
	{
		lTotal = m_poProdOfferAggrIndex->getTotal();
		lUsed  = m_poProdOfferAggrIndex->getCount();	
		iConnNum = m_poProdOfferAggrIndex->connnum();
		return 1;
	}
	
	//new
	else if(strcmp(strIpcKey,"SHM_SERVLOCATION_INFO_INDEX")==0&&m_poServLocationIndex->exist())
	{
		lTotal = m_poServLocationIndex->getTotal();
		lUsed  = m_poServLocationIndex->getCount();	
		iConnNum = m_poServLocationIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVTYPE_INFO_DATA")==0&&m_poServTypeData->exist())
	{
		lTotal = m_poServTypeData->getTotal();
		lUsed  = m_poServTypeData->getCount();	
		iConnNum = m_poServTypeData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_SERVACCT_ACCT_INDEX")==0&&m_poServAcctIndex->exist())
	{
		lTotal = m_poServAcctIndex->getTotal();
		lUsed  = m_poServAcctIndex->getCount();	
		iConnNum = m_poServAcctIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINS_DATA")==0&&m_poProdOfferInsData->exist())
	{
		lTotal = m_poProdOfferInsData->getTotal();
		lUsed  = m_poProdOfferInsData->getCount();	
		iConnNum = m_poProdOfferInsData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINS_INDEX")==0&&m_poProdOfferInsIndex->exist())
	{
		lTotal = m_poProdOfferInsIndex->getTotal();
		lUsed  = m_poProdOfferInsIndex->getCount();	
		iConnNum = m_poProdOfferInsIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_OFFERDETAILINS_DATA")==0&&m_poOfferDetailInsData->exist())
	{
		lTotal = m_poOfferDetailInsData->getTotal();
		lUsed  = m_poOfferDetailInsData->getCount();	
		iConnNum = m_poOfferDetailInsData->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_OFFERDETAILINS_INSIDOFFER_INDEX")==0&&m_poOfferDetailInsOfferInsIndex->exist())
	{
		lTotal = m_poOfferDetailInsOfferInsIndex->getTotal();
		lUsed  = m_poOfferDetailInsOfferInsIndex->getCount();	
		iConnNum = m_poOfferDetailInsOfferInsIndex->connnum();
		return 1;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINSATTR_DATA")==0&&m_poProdOfferInsAttrData->exist())
	{
		lTotal = m_poProdOfferInsAttrData->getTotal();
		lUsed  = m_poProdOfferInsAttrData->getCount();	
		iConnNum = m_poProdOfferInsAttrData->connnum();
		return 1;
	}
	
	else
	{
		lUsed = 0;
		lTotal = 0;
		iConnNum = 0;
		return 0;
	}
	return 0;
}

int MemInfo::GetStrIdxInfo(char *strIpcKey,long &lListUsed,long &lListTotal)
{
	if(strcmp(strIpcKey,"SHM_SERVIDENT_INFO_INDEX")==0&&m_poServIdentIndex->exist())
	{
		lListTotal = m_poServIdentIndex->getlTotal();
		lListUsed  = m_poServIdentIndex->getlCount();	
		return 1;
	}
	else
	{
		lListUsed = 0;
		lListTotal = 0;
		return 0;
	}
	return 0;
}

void MemInfo::FreeMem(char* strIpcKey)
{
	if(strcmp(strIpcKey,"SHM_CUST_INFO_DATA")==0)
	{
		if(m_poCustData->exist())
			m_poCustData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_CUST_INFO_INDEX")==0)
	{
		if(m_poCustIndex->exist())
			m_poCustIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_CUSTATTR_INFO_DATA")==0)
	{
		if(m_poCustAttrData->exist())
			m_poCustAttrData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODUCT_INFO_DATA")==0)
	{
		if(m_poProductData->exist())
			m_poProductData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODUCT_INFO_INDEX")==0)
	{	
		if(m_poProductIndex->exist())
			m_poProductIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERV_INFO_DATA")==0)
	{
		if(m_poServData->exist())
			m_poServData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERV_INFO_INDEX")==0)
	{
		if(UserInfoBase::m_poServIndex->exist())
			UserInfoBase::m_poServIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVLOCATION_INFO_DATA")==0)
	{
		if(m_poServLocationData->exist())
			m_poServLocationData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVSTATE_INFO_DATA")==0)
	{
		if(m_poServStateData->exist())
			m_poServStateData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVATTR_INFO_DATA")==0)
	{
		if(m_poServAttrData->exist())
			m_poServAttrData->remove();
		return;
	}

	else if(strcmp(strIpcKey,"SHM_SERVIDENT_INFO_DATA")==0)
	{
		if(m_poServIdentData->exist())
			m_poServIdentData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVIDENT_INFO_INDEX")==0)
	{
		if(m_poServIdentIndex->exist())
			m_poServIdentIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCT_INFO_DATA")==0)
	{
		if(m_poServProductData->exist())
			m_poServProductData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCT_INFO_INDEX")==0)
	{
		if(m_poServProductIndex->exist())
			m_poServProductIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVPRODUCTATTR_INFO_DATA")==0)
	{
		if(m_poServProductAttrData->exist())
			m_poServProductAttrData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVACCT_INFO_DATA")==0)
	{
		if(m_poServAcctData->exist())
			m_poServAcctData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_ACCT_INFO_DATA")==0)
	{
		if(m_poAcctData->exist())
			m_poAcctData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_ACCT_INFO_INDEX")==0)
	{
		if(m_poAcctIndex->exist())
			m_poAcctIndex->remove();
		return;
	}
	//
	else if(strcmp(strIpcKey,"SHM_AccuData")==0&&m_poAcctIndex->exist())
	{
		m_poAccuData->remove();
		return;		
	}
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_O")==0&&m_poAcctIndex->exist())
	{
		m_poAccuIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_S")==0&&m_poAccuServIndex->exist())
	{
		m_poAccuServIndex->remove();
		return;
	}	
	else if(strcmp(strIpcKey,"SHM_AccuData_Index_I")==0&&m_poAccuInstIndex->exist())
	{
		m_poAccuInstIndex->remove();
		return;
	}			
	else if(strcmp(strIpcKey,"SHM_AccuCycleData")==0&&m_poCycleInstData->exist())
	{
		m_poCycleInstData->remove();
		return;
	}	
	else if(strcmp(strIpcKey,"SHM_AccuCycleData_Index")==0&&m_poCycleInstIndex->exist())
	{
		m_poCycleInstIndex->remove();
		return;
	}

	else if(strcmp(strIpcKey,"SHM_AcctItemData")==0&&m_poAcctItemData->exist())
	{
		m_poAcctItemData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_AcctItemIndex_S")==0&&AcctItemBase::m_poServIndex->exist())
	{
		AcctItemBase::m_poServIndex->remove();
		return;
	}
	
	//define in ProdOfferAggr.h(meminfo.h)
	else if(strcmp(strIpcKey,"SHM_PRODOFFERDISCTBUF")==0&&m_poProdOfferDetailData->exist())
	{
		 m_poProdOfferDetailData->remove();
		return;
	}

	else if(strcmp(strIpcKey,"SHM_PRODOFFERTOTALBUF")==0&&m_poProdOfferAggrData->exist())
	{
		 m_poProdOfferAggrData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERDISCTINDEX")==0&&m_poProdOfferAggrIndex->exist())
	{
		 m_poProdOfferAggrIndex->remove();
		return;
	}
	
	//new
	else if(strcmp(strIpcKey,"SHM_SERVLOCATION_INFO_INDEX")==0&&m_poServLocationIndex->exist())
	{
		m_poServLocationIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVTYPE_INFO_DATA")==0&&m_poServTypeData->exist())
	{
		m_poServTypeData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_SERVACCT_ACCT_INDEX")==0&&m_poServAcctIndex->exist())
	{
		m_poServAcctIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINS_DATA")==0&&m_poProdOfferInsData->exist())
	{
		m_poProdOfferInsData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINS_INDEX")==0&&m_poProdOfferInsIndex->exist())
	{
		m_poProdOfferInsIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_OFFERDETAILINS_DATA")==0&&m_poOfferDetailInsData->exist())
	{
		m_poOfferDetailInsData->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_OFFERDETAILINS_INSIDOFFER_INDEX")==0&&m_poOfferDetailInsOfferInsIndex->exist())
	{
		m_poOfferDetailInsOfferInsIndex->remove();
		return;
	}
	else if(strcmp(strIpcKey,"SHM_PRODOFFERINSATTR_DATA")==0&&m_poProdOfferInsAttrData->exist())
	{
		m_poProdOfferInsAttrData->remove();
		return;
	}
}


#endif

