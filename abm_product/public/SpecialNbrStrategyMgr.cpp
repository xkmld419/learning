/*VER: 3*/ 
#include "SpecialNbrStrategyMgr.h"
#include "Environment.h"
#include "Log.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

SpecialNbrStrategyMgr::SpecialNbrStrategyMgr()
{
#ifdef PRIVATE_MEMORY
	load();	
#endif
}

SpecialNbrStrategyMgr::~SpecialNbrStrategyMgr()
{
#ifdef PRIVATE_MEMORY
	unLoad();
#endif
}

//取得策略ID
#ifdef PRIVATE_MEMORY
bool SpecialNbrStrategyMgr::getStrategyID (StdEvent * pEvent,int iOfferID,int &iStrategyID)
{
	bool bRet = false;
	int iLen  = 0;
	SpecialNbrStrategy  * pTemp;
	char sAreaCode[MAX_CALLING_AREA_CODE_LEN];
	char sAccNbr[MAX_CALLING_NBR_LEN];

	if(pEvent->m_iCallTypeID == 2){//被叫
		strcpy(sAccNbr,pEvent->m_sCallingNBR);
		strcpy(sAreaCode,pEvent->m_sCalledAreaCode);
	}else {//主叫和呼转
		strcpy(sAccNbr,pEvent->m_sCalledNBR);
		strcpy(sAreaCode,pEvent->m_sCalledAreaCode);
	}
	while(m_pNbrStrategy->getMax(sAccNbr,&pTemp,&iLen))
	{
		iLen =  strlen(pTemp->m_sAccNbr)-1;
		strncpy(sAccNbr,pTemp->m_sAccNbr,iLen);
		sAccNbr[iLen]='\0';
		while(pTemp)
		{
			if((pTemp->m_sAreaCode[0]=='*' ||strcmp(sAreaCode,pTemp->m_sAreaCode)==0)
				&& (pTemp->m_iOfferID == 0 || pTemp->m_iOfferID == iOfferID)
				&& strcmp(pEvent->m_sStartDate,pTemp->m_sEffDate)>=0 
				&& strcmp(pEvent->m_sStartDate,pTemp->m_sExpDate)<0 
				)
			{
					iStrategyID = pTemp->m_iStrategyID;
					bRet = true;
					return bRet;
			}
			else
			{
				pTemp = pTemp->m_poNext;
			}
		}
		if(iLen<m_iLen){
			break;
		}
	}
	return bRet;
}
#else
bool SpecialNbrStrategyMgr::getStrategyID (StdEvent * pEvent,int iOfferID,int &iStrategyID)
{
    bool bRet = false;
	int iLen  = 0;
	SpecialNbrStrategy  * pTemp;
	char sAreaCode[MAX_CALLING_AREA_CODE_LEN];
	char sAccNbr[MAX_CALLING_NBR_LEN];
	SpecialNbrStrategy *pSNbrStrategy= 0;//(SpecialNbrStrategy*)(*G_PPARAMINFO->m_poSpecialNbrStrategyData);
	if(pEvent->m_iCallTypeID == 2){ 
		strcpy(sAccNbr,pEvent->m_sCallingNBR);
		strcpy(sAreaCode,pEvent->m_sCalledAreaCode);
	}else {  
		strcpy(sAccNbr,pEvent->m_sCalledNBR);
		strcpy(sAreaCode,pEvent->m_sCalledAreaCode);
	}
			
			//while(m_pNbrStrategy->getMax(sAccNbr,&pTemp,&iLen))
			unsigned int iTemp = 0; 
			while(G_PPARAMINFO->m_poSpecialNbrStrategyIndex->getMax(sAccNbr,&iTemp))
			{
				pSNbrStrategy = &(G_PPARAMINFO->m_poSpecialNbrStrategyList[iTemp]);
				iLen =  strlen(pSNbrStrategy->m_sAccNbr)-1;
				strncpy(sAccNbr,pSNbrStrategy->m_sAccNbr,iLen);
				sAccNbr[iLen]='\0';
				
				while(iTemp > 0){
					pTemp = &(G_PPARAMINFO->m_poSpecialNbrStrategyList[iTemp]);
					if((pTemp->m_sAreaCode[0]=='*' ||strcmp(sAreaCode,pTemp->m_sAreaCode)==0) && (pTemp->m_iOfferID == 0 || pTemp->m_iOfferID == iOfferID)
							&& strcmp(pEvent->m_sStartDate,pTemp->m_sEffDate)>=0 && strcmp(pEvent->m_sStartDate,pTemp->m_sExpDate)<0)
					{
							iStrategyID = pTemp->m_iStrategyID;
							bRet = true;
							return bRet;
					}else{
							iTemp = pTemp->m_iNext;
					}
			    }
			    if(iLen<m_iLen){
				    break;
			    }
		    }
		    return bRet;
}
#endif

#ifdef PRIVATE_MEMORY
bool SpecialNbrStrategyMgr::checkOfferStrategy(int iOfferID)
{
	int i = 0;
	return m_poOfferIndex->get(iOfferID,&i);
}
#else
bool SpecialNbrStrategyMgr::checkOfferStrategy(int iOfferID)
{
	unsigned int i = 0;
	return G_PPARAMINFO->m_poSpecialNbrOfferIndex->get(iOfferID,&i);
}
#endif

void  SpecialNbrStrategyMgr::load ()
{
	SpecialNbrStrategy * pTemp;
	SpecialNbrStrategy * pTemp2;
	m_pNbrStrategy =  new KeyTree<SpecialNbrStrategy*>;
	if(!m_pNbrStrategy){
		THROW(MBC_SpecailNbrStrategyMgr+1);
	}
	
	TOCIQuery qry(Environment::getDBConn());
	
    qry.setSQL("select nvl(area_code,'*') area_code,acc_nbr,strategy_id,eff_date,"
		" nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date ,"
		" nvl(offer_id,0) offer_id"
		" from b_special_nbr_strategy order by acc_nbr,area_code,offer_id");
    qry.open();
	while(qry.next())
	{
		pTemp = new SpecialNbrStrategy();
		strcpy(pTemp->m_sAreaCode,qry.field(0).asString());
		strcpy(pTemp->m_sAccNbr,qry.field(1).asString());
		pTemp->m_iStrategyID = qry.field(2).asInteger();
		strcpy(pTemp->m_sEffDate,qry.field(3).asString());
		strcpy(pTemp->m_sExpDate,qry.field(4).asString());
		pTemp->m_iOfferID = qry.field(5).asInteger();
		pTemp->m_poNext = 0;
		if(m_pNbrStrategy->get(pTemp->m_sAccNbr,&pTemp2)){
			pTemp->m_poNext = pTemp2;
		}
		m_pNbrStrategy->add(pTemp->m_sAccNbr,pTemp);
	}
	qry.close();
	qry.commit();

	m_iLen = 0;
	qry.setSQL("select min(length(acc_nbr)) from b_special_nbr_strategy");
	qry.open();
	if(qry.next()){
		m_iLen = qry.field(0).asInteger();
	}
	if(m_iLen<1) m_iLen =1;
	qry.close();
	qry.commit();

	char sSqlcode[500];
	int iCount = 0;
	qry.setSQL ("select count(*) from product_offer");
	qry.open (); qry.next ();
	iCount = qry.field(0).asInteger () + 1;
	qry.close ();
	qry.commit();

	m_poOfferIndex = new HashList<int>(iCount);
	if(!m_poOfferIndex){
		THROW(MBC_SpecailNbrStrategyMgr+1);
	}
	sprintf(sSqlcode,"select distinct po.offer_id from pricing_combine a,event_pricing_strategy b ,"
			" product_offer po where a.event_pricing_strategy_id = b.event_pricing_strategy_id "
			" and b.event_type_id = %d and a.pricing_plan_id = po.pricing_plan_id"
			,SPECIAL_STRATEGY_EVENT_TYPE);
	qry.setSQL (sSqlcode);
	qry.open();
	while(qry.next())
	{
		m_poOfferIndex->add(qry.field(0).asInteger(),0);
	}
	qry.close();
	qry.commit();
	return ;
}

void SpecialNbrStrategyMgr::unLoad()
{
	if(m_pNbrStrategy){
		delete m_pNbrStrategy;
		m_pNbrStrategy = NULL;
	}
	if(m_poOfferIndex){
		delete m_poOfferIndex;
		m_poOfferIndex = NULL;
	}
	return;
}
