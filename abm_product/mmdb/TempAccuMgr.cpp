#include "TempAccuMgr.h"
#include <stdlib.h>
#include "Environment.h"
#include "OfferAccuMgr.h"
#include "ProdOfferAggr.h"
#include "Log.h"
#include "CommonMacro.h"

TempAccuMgr::TempAccuMgr(OfferAccuMgr * pAccuMgr)
{
	if (!G_POPRODOFFFERDISCT)
		G_POPRODOFFFERDISCT = new ProdOfferDisctAggr();
	m_poAccuMgr = pAccuMgr;
	m_poAccuCfg = new AccuTypeCfg();
	m_voEventAccuBase.resize(128);
	m_voDisctAccu.resize(DISCT_AGGR_NUM);
	m_voEventAccu.resize(128);
	m_vEventAccuCond.resize(64);
	m_vDisctAccuCond.resize(64);
	m_lSeq = 0;
	m_lCycleInstID = 0;
	m_iCycleTypeID = 0;
	memset(m_sBeginTime,0,sizeof(m_sBeginTime));
	memset(m_sEndTime,0,sizeof(m_sEndTime));
}

void TempAccuMgr::init()
{
	m_voEventAccuBase.clear();
	m_voDisctAccu.clear();
	m_vEventAccuCond.clear();
	m_vDisctAccuCond.clear();
	m_voEventAccu.clear();
	//m_lSeq = 0; //内部用，不再每次清0
    //m_iCycleTypeID = 0;
	return;
}

void TempAccuMgr::addEventAccuCond(EventAccuQuery * pCond)
{
	m_vEventAccuCond.push_back(*pCond);
	return;
}

void TempAccuMgr::addDisctAccuCond(long lInstID)
{
	m_vDisctAccuCond.push_back(lInstID);
	return;
}

void TempAccuMgr::queryData()
{
	#ifdef HAVE_ABM
	int i ,j;
	EventAccuQuery * p;
	for(i=0;i<m_vEventAccuCond.size();i++)
	{
		p = &m_vEventAccuCond[i];
		for (j=0;j< p->m_viAccuTypeID.size();j++){
			AccuType *pAccu = m_poAccuCfg->getAccuType(p->m_viAccuTypeID[j]);
			if (!pAccu) continue;
			if(m_iCycleTypeID != pAccu->m_iCycleTypeID){
				m_lCycleInstID = m_poAccuMgr->getAccCycleByTime(pAccu->m_iCycleTypeID,
					G_PEVENT->m_sStartDate,m_sBeginTime,m_sEndTime);
                m_iCycleTypeID = pAccu->m_iCycleTypeID;
			}
			if (m_lCycleInstID >0){
				m_poAccuMgr->getAllAccByInst(p->m_lInstID,p->m_viAccuTypeID[j],G_PEVENT->m_iBillingCycleID,
					m_lCycleInstID,m_voEventAccuBase);
			}
		}
	}
	for(i=0;i<m_vDisctAccuCond.size();i++)
	{
		G_POPRODOFFFERDISCT->getAllAggrAndDetail(m_vDisctAccuCond[i],
			G_PEVENT->m_iBillingCycleID,m_voDisctAccu);
	}
	#endif

	return;
}

long TempAccuMgr::addRefEventAccuData(long lOfferInstID,int iAccuTypeID,long lCycleInstID,
		long lServID,long lMemberID,char sSplitby[])
{
	long lRet = 0;
	AccuCacheData *pData;
	int m = m_voEventAccu.size();
	for (int i=0;i<m;i++)
	{
		pData = &m_voEventAccu[i];
		if (pData->m_lOfferInstID == lOfferInstID &&
			pData->m_iAccuTypeID == iAccuTypeID &&
			pData->m_lCycleInstID == lCycleInstID &&
			pData->m_lServID == lServID &&
			pData->m_lMemberID == lMemberID &&
			strcmp(pData->m_sSplitby,sSplitby) == 0){
				return pData->m_lValue;
		}
	}
	static AccuCacheData oData;
	memset(&oData,0,sizeof(AccuData));
	m = m_voEventAccuBase.size();
	AccuData * pAccu;
	for(int i =0;i<m;i++)
	{
		pAccu = &m_voEventAccuBase[m];
		if (pAccu->m_lOfferInstID == lOfferInstID &&
			pAccu->m_iAccuTypeID == iAccuTypeID &&
			pAccu->m_lCycleInstID == lCycleInstID &&
			pAccu->m_lMemberID == lMemberID &&
			pAccu->m_lServID == lServID &&
			strcmp(pAccu->m_sSplitby,sSplitby) == 0)
		{
			oData.m_lAccuInstID = pAccu->m_lAccuInstID;
			oData.m_lABMKeyID = oData.m_lAccuInstID;
			oData.m_lValue = pAccu->m_lValue;
			oData.m_lOfferInstID = lOfferInstID;
			oData.m_iAccuTypeID = iAccuTypeID;
			oData.m_lCycleInstID = lCycleInstID;
			oData.m_lServID = lServID;
			oData.m_lMemberID = lMemberID;
			#ifdef SPLIT_ACCU_BY_BILLING_CYCLE
			oData.m_iBillingCycleID = pAccu->m_iBillingCycleID;
			#endif
			strncpy(oData.m_sSplitby,sSplitby,sizeof(oData.m_sSplitby));
			#ifdef ABM_FIELD_EXTEND	
			oData.m_lVersionID = pAccu->m_lVersionID;
			#endif	
			oData.m_cOperation = 'r';

			if (m_lCycleInstID != oData.m_lCycleInstID)
			{
				AccuType *pAccu = m_poAccuCfg->getAccuType(oData.m_iAccuTypeID);
				if (pAccu){
					m_lCycleInstID = m_poAccuMgr->getAccCycleByTime(pAccu->m_iCycleTypeID,
						G_PEVENT->m_sStartDate,m_sBeginTime,m_sEndTime);
					if (m_lCycleInstID >0){
						m_iCycleTypeID = pAccu->m_iCycleTypeID;
						oData.m_lCycleTypeID = m_iCycleTypeID;
						oData.m_lBeginDate = atol(m_sBeginTime);
						oData.m_lEndDate = atol(m_sEndTime);
					}
				}else{
					m_lCycleInstID = 0;
				}
			}else{
				oData.m_lCycleTypeID = m_iCycleTypeID;
				oData.m_lBeginDate = atol(m_sBeginTime);
				oData.m_lEndDate = atol(m_sEndTime);
			}
			m_voEventAccu.push_back(oData);
			lRet = pAccu->m_lValue;
			break;
		}
	}
	
	return lRet;
}

void TempAccuMgr::addUpdateEventAccuData(vector<EventAddAggrData> &voData)
{
	int i =0,m=0,n=0;
	bool bData = false;
	EventAddAggrData * pUpdateData;
	vector<AccuCacheData>::iterator iter;
	m = voData.size();
	for (i=0;i<m;i++)
	{
		pUpdateData = &voData[i];
		for (iter = m_voEventAccu.begin();iter!= m_voEventAccu.end(); iter++)
		{
			if ((*iter).m_lOfferInstID == pUpdateData->lOfferInstID &&
				(*iter).m_iAccuTypeID == pUpdateData->iAccTypeID &&
				(*iter).m_lCycleInstID == pUpdateData->lCycleInstID &&
				(*iter).m_lMemberID == pUpdateData->lInstMemberID &&
				(*iter).m_lServID == pUpdateData->lServID &&
				strcmp((*iter).m_sSplitby,pUpdateData->sSplitBy) == 0)
			{
				(*iter).m_lAddValue += pUpdateData->lValue; 
				if ('r' == (*iter).m_cOperation){
					(*iter).m_cOperation = 'u';
					(*iter).m_iEventSourceType = pUpdateData->iEventSourceType;
					(*iter).m_iFileID = pUpdateData->iFileID;
					(*iter).m_iOrgID = pUpdateData->iOrgID;
					(*iter).m_lEventID = pUpdateData->lEventID;
				}
				bData = true;
				break;
			}
		}
		if (!bData && iter == m_voEventAccu.end ())
		{
			AccuCacheData  oData;
			memset(&oData,0,sizeof(AccuCacheData));
			n = m_voEventAccuBase.size();
			AccuData * pAccu;
			bool bHave = false;
			for(int j =0;j<n;j++)
			{
				pAccu = &m_voEventAccuBase[j];
				if (pAccu->m_lOfferInstID == pUpdateData->lOfferInstID &&
					pAccu->m_iAccuTypeID == pUpdateData->iAccTypeID &&
					pAccu->m_lCycleInstID == pUpdateData->lCycleInstID &&
					pAccu->m_lMemberID == pUpdateData->lInstMemberID &&
					pAccu->m_lServID == pUpdateData->lServID &&
					strcmp(pAccu->m_sSplitby,pUpdateData->sSplitBy) == 0)
				{
					oData.m_lAccuInstID = pAccu->m_lAccuInstID;
					oData.m_lABMKeyID = oData.m_lAccuInstID;
					oData.m_lValue = pAccu->m_lValue;
					oData.m_lOfferInstID = pUpdateData->lOfferInstID;
					oData.m_iAccuTypeID = pUpdateData->iAccTypeID;
					oData.m_lCycleInstID = pUpdateData->lCycleInstID;
					oData.m_lServID = pUpdateData->lServID;
					oData.m_lMemberID = pUpdateData->lInstMemberID;
					#ifdef SPLIT_ACCU_BY_BILLING_CYCLE
					oData.m_iBillingCycleID = pAccu->m_iBillingCycleID;
					#endif
					strncpy(oData.m_sSplitby,pUpdateData->sSplitBy,sizeof(oData.m_sSplitby));
					oData.m_lAddValue = pUpdateData->lValue;
					#ifdef ABM_FIELD_EXTEND	
					oData.m_lVersionID = pAccu->m_lVersionID;
					#endif	
					oData.m_cOperation = 'u';
					bHave = true;
					break;
				}
			}
			if (!bHave)
			{
				oData.m_lAccuInstID = ++m_lSeq;
				oData.m_lABMKeyID = 0;
				oData.m_lVersionID = AGGR_VERSION_INITVAL;
				oData.m_lValue = 0;
				oData.m_lOfferInstID = pUpdateData->lOfferInstID;
				oData.m_iAccuTypeID = pUpdateData->iAccTypeID;
				oData.m_lCycleInstID = pUpdateData->lCycleInstID;
				oData.m_lServID = pUpdateData->lServID;
				oData.m_lMemberID = pUpdateData->lInstMemberID;
				oData.m_iBillingCycleID = pUpdateData->iBillingCycleID;
				strncpy(oData.m_sSplitby,pUpdateData->sSplitBy,sizeof(oData.m_sSplitby));
				oData.m_lAddValue = pUpdateData->lValue;
				oData.m_cOperation = 'i';
			}
			oData.m_iEventSourceType = pUpdateData->iEventSourceType;
			oData.m_iFileID = pUpdateData->iFileID;
			oData.m_iOrgID = pUpdateData->iOrgID;
			oData.m_lEventID = pUpdateData->lEventID;

			if (( 0 == m_lCycleInstID) || m_lCycleInstID != oData.m_lCycleInstID)
			{
				AccuType *pAccu = m_poAccuCfg->getAccuType(oData.m_iAccuTypeID);
				if (pAccu){
					m_lCycleInstID = m_poAccuMgr->getAccCycleByTime(pAccu->m_iCycleTypeID,
						G_PEVENT->m_sStartDate,m_sBeginTime,m_sEndTime);
					if (m_lCycleInstID >0){
						m_iCycleTypeID = pAccu->m_iCycleTypeID;
						oData.m_lCycleTypeID = m_iCycleTypeID;
						oData.m_lBeginDate = atol(m_sBeginTime);
						oData.m_lEndDate = atol(m_sEndTime);
					}
				}else{
					m_lCycleInstID = 0;
				}
			}else{
				oData.m_lCycleTypeID = m_iCycleTypeID;
				oData.m_lBeginDate = atol(m_sBeginTime);
				oData.m_lEndDate = atol(m_sEndTime);
			}

			m_voEventAccu.push_back(oData);
		}
	}
	return;
}

long TempAccuMgr::getRefDisctData(ProdOfferAggrData * pData)
{
	DisctAccu * pDisct;
	int m = m_voDisctAccu.size();
	for (int i=0;i<m;i++)
	{
		pDisct = &m_voDisctAccu[i];
		if (pDisct->m_oDisct.m_lOfferInstID == pData->m_lProdOfferID &&
			pDisct->m_oDisct.m_iBillingCycleID == pData->m_iBillingCycleID &&
			pDisct->m_oDisct.m_iCombineID == pData->m_iProdOfferCombine &&
			pDisct->m_oDisct.m_iMeasureID == pData->m_iPricingMeasure &&
			pDisct->m_oDisct.m_iItemGroupID == pData->m_iItemGroupID &&
			pDisct->m_oDisct.m_iInType  == pData->m_iInType &&
			pDisct->m_oDisct.m_lInValue == pData->m_lInValue)
		{
			if ('o' == pDisct->m_oDisct.m_cOperation){
				pDisct->m_oDisct.m_cOperation = 'r';
			}
			return (pDisct->m_oDisct.m_lValue + pDisct->m_oDisct.m_lAddValue);
		}
	}
	return 0;
}

void TempAccuMgr::addDisctData(ProdOfferTotalData * pData,int iOutNum)
{
	DisctAccu * pDisct;
	vector<DisctAccu>::iterator iter;
	vector<DisctDetailCacheData>::iterator iterDetail;
	for (iter = m_voDisctAccu.begin();iter!= m_voDisctAccu.end(); iter++)
	{
		if ((*iter).m_oDisct.m_lOfferInstID == pData->m_lProdOfferID &&
			(*iter).m_oDisct.m_iBillingCycleID == pData->m_iBillingCycleID &&
			(*iter).m_oDisct.m_iCombineID == pData->m_iProdOfferCombine &&
			(*iter).m_oDisct.m_iMeasureID == pData->m_iPricingMeasure &&
			(*iter).m_oDisct.m_iItemGroupID == pData->m_iItemGroupID &&
			(*iter).m_oDisct.m_iInType  == pData->m_iInType &&
			(*iter).m_oDisct.m_lInValue == pData->m_lInValue)
		{
			if ('o' == (*iter).m_oDisct.m_cOperation){
				(*iter).m_oDisct.m_cOperation = 'u';
			}
			if (pData->m_bInOutType)
			{
				if(pData->m_bNewRec){
					(*iter).m_oDisct.m_lAddValue += pData->m_lValue;
				}else{
					(*iter).m_oDisct.m_lAddValue = pData->m_lValue-(*iter).m_oDisct.m_lValue;
				}
			}else{
				if(pData->m_bNewRec){
					(*iter).m_oDisct.m_lAddDisctValue += pData->m_lValue;
				}else{
					(*iter).m_oDisct.m_lAddDisctValue += pData->m_lValue-(*iter).m_oDisct.m_lDisctValue;
				}
			}
			for (iterDetail = (*iter).m_voDetail.begin();iterDetail!= (*iter).m_voDetail.end(); iterDetail++)
			{
				if((*iterDetail).m_lServID == pData->m_lServID &&
					(*iterDetail).m_iAcctItemTypeID == pData->m_iAcctItemTypeID )
				{
					if ('o' == (*iterDetail).m_cOperation){
						(*iterDetail).m_cOperation = 'u';
					}
					if (pData->m_bInOutType)
					{
						if(pData->m_bNewRec){
							(*iterDetail).m_lAddValue += pData->m_lValue;
						}else{
							(*iterDetail).m_lAddValue = pData->m_lValue-(*iterDetail).m_lValue;
						}
					}else{
						if(pData->m_bNewRec){
							(*iterDetail).m_lAddDisctValue += pData->m_lValue;
						}else{
							(*iterDetail).m_lAddDisctValue += pData->m_lValue-(*iterDetail).m_lDisctValue;
						}
					}
					break;
				}
			}
			if(iterDetail == (*iter).m_voDetail.end())
			{
				static DisctDetailCacheData oDetailData;
				memset(&oDetailData,0,sizeof(DisctDetailCacheData));
				oDetailData.m_lServID = pData->m_lServID;
				oDetailData.m_iAcctItemTypeID = pData->m_iAcctItemTypeID;
				oDetailData.m_iBillingCycleID = pData->m_iBillingCycleID;
				oDetailData.m_lVersionID = AGGR_VERSION_INITVAL;
				oDetailData.m_lItemDetailSeq = ++m_lSeq;
				if (pData->m_bInOutType){
					oDetailData.m_lAddValue = pData->m_lValue;
				}else{
					oDetailData.m_lAddDisctValue = pData->m_lValue;
				}
				oDetailData.m_cOperation = 'i';
				oDetailData.m_lItemDisctSeq = (*iter).m_oDisct.m_lItemDisctSeq;
				oDetailData.m_lEventID = G_PEVENT->m_lEventID;
				oDetailData.m_iFileID = G_PEVENT->m_iFileID;
				oDetailData.m_iEventSourceType =  G_PEVENT->m_iEventSourceType;
				oDetailData.m_iOrgID =  G_PUSERINFO->getServOrg (oDetailData.m_lServID,G_PEVENT->m_sStartDate);

				(*iter).m_voDetail.push_back(oDetailData);
			}
			break;
		}
	}
	if (iter == m_voDisctAccu.end())
	{
		static DisctAccu oData;
		memset(&oData.m_oDisct,0,sizeof(oData.m_oDisct));
		oData.m_voDetail.clear();
		oData.m_oDisct.m_lOfferInstID = pData->m_lProdOfferID;
		oData.m_oDisct.m_iBillingCycleID = pData->m_iBillingCycleID;
		oData.m_oDisct.m_iCombineID = pData->m_iProdOfferCombine;
		oData.m_oDisct.m_iMeasureID = pData->m_iPricingMeasure;
		oData.m_oDisct.m_iItemGroupID = pData->m_iItemGroupID;
		oData.m_oDisct.m_iInType  = pData->m_iInType;
		oData.m_oDisct.m_lInValue = pData->m_lInValue;
		oData.m_oDisct.m_cOperation = 'i';
		oData.m_oDisct.m_lItemDisctSeq = ++m_lSeq;
		oData.m_oDisct.m_lVersionID = AGGR_VERSION_INITVAL;
		#ifdef ORG_BILLINGCYCLE
		oData.m_oDisct.m_iOrgBillingCycleID = G_PEVENT->m_iOrgBillingCycleID;
		#endif
		if (pData->m_bInOutType){
			oData.m_oDisct.m_lAddValue = pData->m_lValue;
		}else{
			oData.m_oDisct.m_lAddDisctValue = pData->m_lValue;
		}
		oData.m_oDisct.m_iFileID = G_PEVENT->m_iFileID;
		oData.m_oDisct.m_iEventSourceType = G_PEVENT->m_iEventSourceType;
		oData.m_oDisct.m_iOrgID = G_PEVENT->m_iBillingOrgID;

		static DisctDetailCacheData oDetailData;
		memset(&oDetailData,0,sizeof(DisctDetailCacheData));
		oDetailData.m_lServID = pData->m_lServID;
		oDetailData.m_iAcctItemTypeID = pData->m_iAcctItemTypeID;
		oDetailData.m_iBillingCycleID = pData->m_iBillingCycleID;
		oDetailData.m_lVersionID = AGGR_VERSION_INITVAL;
		oDetailData.m_lItemDetailSeq = ++m_lSeq;
		if (pData->m_bInOutType){
			oDetailData.m_lAddValue = pData->m_lValue;
		}else{
			oDetailData.m_lAddDisctValue = pData->m_lValue;
		}
		oDetailData.m_cOperation = 'i';
		oDetailData.m_lItemDisctSeq = oData.m_oDisct.m_lItemDisctSeq;
		oDetailData.m_lEventID = G_PEVENT->m_lEventID;
		oDetailData.m_iFileID = G_PEVENT->m_iFileID;
		oDetailData.m_iEventSourceType =  G_PEVENT->m_iEventSourceType;
		oDetailData.m_iOrgID =  G_PUSERINFO->getServOrg (oDetailData.m_lServID,G_PEVENT->m_sStartDate);

		oData.m_voDetail.push_back(oDetailData);
		m_voDisctAccu.push_back(oData);
		iOutNum = m_voDisctAccu.size();
	}
	return;
}

int TempAccuMgr::getDisctAggrNum()
{
	return  m_voDisctAccu.size()>DISCT_AGGR_NUM?m_voDisctAccu.size():DISCT_AGGR_NUM;
}

int TempAccuMgr::getDisctAggrHead(ProdOfferTotalData *pData,DisctAccu **ppDisctAccu)
{
	DisctAccu * pDisct;
	int m = m_voDisctAccu.size();
	for (int i=0;i<m;i++)
	{
		pDisct = &m_voDisctAccu[i];
		if (pDisct->m_oDisct.m_lOfferInstID == pData->m_lProdOfferID &&
			pDisct->m_oDisct.m_iBillingCycleID == pData->m_iBillingCycleID &&
			pDisct->m_oDisct.m_iCombineID == pData->m_iProdOfferCombine &&
			pDisct->m_oDisct.m_iMeasureID == pData->m_iPricingMeasure &&
			pDisct->m_oDisct.m_iItemGroupID == pData->m_iItemGroupID &&
			pDisct->m_oDisct.m_iInType  == pData->m_iInType &&
			pDisct->m_oDisct.m_lInValue == pData->m_lInValue)
		{
			*ppDisctAccu = &m_voDisctAccu[i];
			return 1;
		}
	}
	return 0;
}

bool TempAccuMgr::isAggrEmpty()
{
	return  (m_voDisctAccu.empty() &&  m_voEventAccu.empty());
}
