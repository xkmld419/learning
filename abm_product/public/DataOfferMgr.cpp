/*VER: 2*/ 

#include "DataOfferMgr.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include "ParamDefineMgr.h"

bool DataProductOfferMgr::m_bUploaded = false;

DataProductOfferMgr::DataProductOfferMgr()
{
	load();
	
	long iParam = ParamDefineMgr::getLongParam("RT_DATA_STOP", "JUDGE_INST_EFF_DATE");	
	
	if (iParam > 0)	
	{
		m_bJudgeInstEffDate = true;
	}else
	{
		m_bJudgeInstEffDate = false;
	}		

}

DataProductOfferMgr::~DataProductOfferMgr()
{
    unload();
}

void DataProductOfferMgr::load()
{
	unload ();
    DataProductOffer * pOfferInfo = NULL;
    DATA_OFFER_DETAIL_MAP * pOfferDetailMap = NULL;
    
	TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32] = {0};
	char sPwd[32] = {0};
	char sStr[32] = {0};
	
	Environment::getConnectInfo("COMM", sUser, sPwd, sStr);
	pConn->connect(sUser, sPwd, sStr);
	TOCIQuery qry(pConn);    
	
	qry.setSQL (" select control_id, offer_id, nvl(offer_detail_id, 0), ref_id, threshold_value, "
	            " stop_type, nvl(offset_cycle_end, 0), remind_offer_detail_id, remark, org_id,THRESHOLD_UNIT_NAME, "
	            " nvl(to_char(JUDGE_OFFER_INST_START_DATE, 'yyyymmddhh24miss'),'10000101000000'), REMIND_TYPE "
				" from B_DATA_CREDIT_CONTROL_OFFER "
				" order by offer_id,offer_detail_id,threshold_value desc ");
                        
    qry.open();
    
    while (qry.next())
    {
    	pOfferInfo = new DataProductOffer();
    	
        pOfferInfo->m_iControlID = qry.field(0).asInteger();
        pOfferInfo->m_iOfferID = qry.field(1).asInteger();
        pOfferInfo->m_iOfferDetailID = qry.field(2).asInteger();
        pOfferInfo->m_iRefID = qry.field(3).asInteger();
        pOfferInfo->m_lThresholdValue = qry.field(4).asLong();
        
        pOfferInfo->m_iStopType = qry.field(5).asInteger();
        pOfferInfo->m_iOffsetCycleEnd = qry.field(6).asLong();
        pOfferInfo->m_iRemindOfferDetailID = qry.field(7).asInteger();
        strcpy(pOfferInfo->m_sRemark, qry.field(8).asString());
        
        pOfferInfo->m_iOrgID = qry.field(9).asInteger();
        strcpy(pOfferInfo->m_sThresholdUnitName, qry.field(10).asString());
        strcpy(pOfferInfo->m_sJudgeInstStartDate, qry.field(11).asString());
        pOfferInfo->m_iRemindType = qry.field(12).asInteger();

        m_pOfferIter = m_poOfferInfoMap.find(pOfferInfo->m_iOfferID);
        if (m_pOfferIter != m_poOfferInfoMap.end())
        {
			pOfferDetailMap = m_pOfferIter->second;
        }else
        {
        	pOfferDetailMap = new DATA_OFFER_DETAIL_MAP();
        	m_poOfferInfoMap[pOfferInfo->m_iOfferID] = pOfferDetailMap;        	
        }	
        
        (*pOfferDetailMap)[pOfferInfo->m_iOfferDetailID] = pOfferInfo;
    }
    qry.close();
    
	pConn->disconnect();
	delete pConn;
	pConn = NULL;

	m_bUploaded = true;
}

void DataProductOfferMgr::unload()
{
	m_bUploaded = false;
	if (!m_poOfferInfoMap.empty())
	{
		for (m_pOfferIter = m_poOfferInfoMap.begin(); m_pOfferIter != m_poOfferInfoMap.end(); ++m_pOfferIter)	
		{
			if (!m_pOfferIter->second->empty())
			{
				//释放DATA_OFFER_DETAIL_MAP中DataProductOffer信息
				for (m_pOfferDetailIter = m_pOfferIter->second->begin(); m_pOfferDetailIter != m_pOfferIter->second->end(); ++m_pOfferDetailIter)
				{
					delete m_pOfferDetailIter->second;
					m_pOfferDetailIter->second = NULL;
				}
				m_pOfferIter->second->clear();
			}	
			
			//释放DATA_OFFER_DETAIL_MAP
			delete m_pOfferIter->second;
			m_pOfferIter->second = NULL;	 		
		}
		m_poOfferInfoMap.clear();
	}	
}

DataProductOffer * DataProductOfferMgr::getInfoByOfferInst(OfferInsQO *pOfferInst)
{
	int iOfferID = pOfferInst->m_poOfferIns->m_iOfferID;
	int iOfferDetailID = pOfferInst->m_poDetailIns->m_iMemberID;
	
	if (m_bJudgeInstEffDate)
	{
		return getInfoByOfferDetailID(iOfferID, iOfferDetailID, pOfferInst->m_poDetailIns->m_sEffDate);
	}	
	else
	{
		return getInfoByOfferDetailID(iOfferID, iOfferDetailID);
	}	
	
	return getInfoByOfferDetailID(iOfferID, iOfferDetailID)	;
}	

DataProductOffer * DataProductOfferMgr::getInfoByOfferID(int iOfferID)
{
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->begin();
    	return m_pOfferDetailIter->second;
    }
    
    return NULL;
}

DataProductOffer * DataProductOfferMgr::getInfoByOfferID(int iOfferID, char *sInstEffDate)
{
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->begin();
    	if (strncmp(sInstEffDate, m_pOfferDetailIter->second->m_sJudgeInstStartDate, 14) >= 0)
    	{	
    		return m_pOfferDetailIter->second;
    	}
    }
    
    return NULL;
}

DataProductOffer * DataProductOfferMgr::getInfoByOfferDetailID(int iOfferID, int iOfferDetailID)
{
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->find(iOfferDetailID);
    	
    	if (m_pOfferDetailIter != m_pOfferIter->second->end())
    	{
    		return m_pOfferDetailIter->second;
    	}
    }
    
    return NULL;	
}	

DataProductOffer * DataProductOfferMgr::getInfoByOfferDetailID(int iOfferID, int iOfferDetailID, char *sInstEffDate)
{
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->find(iOfferDetailID);
    	
    	if (m_pOfferDetailIter != m_pOfferIter->second->end())
    	{
    		if (strncmp(sInstEffDate, m_pOfferDetailIter->second->m_sJudgeInstStartDate, 14) >= 0)
    		{	
    			return m_pOfferDetailIter->second;
    		}	
    	}
    }
    
    return NULL;	
}	



bool DataProductOfferMgr::isDataOfferInst(int iOfferID)
{	
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
        return true;
    }
    
    return false;
}

bool DataProductOfferMgr::isDataOfferInstDetail(int iOfferID, int iOfferDetailID)
{	
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->find(iOfferDetailID);
    	if (m_pOfferDetailIter != m_pOfferIter->second->end())
    	{
    		return true;
    	}	
    }
    
    return false;
}

bool DataProductOfferMgr::isDataOfferInst(int iOfferID, char *sInstEffDate)
{	
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->begin();
    	if (strncmp(sInstEffDate, m_pOfferDetailIter->second->m_sJudgeInstStartDate, 14) >= 0)
    	{	
    		//订购生效时间大于等于m_sJudgeInstStartDate的用户才参与信控
        	return true;
        }
    }
    
    return false;
}

bool DataProductOfferMgr::isDataOfferInstDetail(int iOfferID, int iOfferDetailID, char *sInstEffDate)
{	
	m_pOfferIter = m_poOfferInfoMap.find(iOfferID);	
	
    if(m_pOfferIter != m_poOfferInfoMap.end())
    {
    	m_pOfferDetailIter = m_pOfferIter->second->find(iOfferDetailID);
    	if (m_pOfferDetailIter != m_pOfferIter->second->end())
    	{
    		if (strncmp(sInstEffDate, m_pOfferDetailIter->second->m_sJudgeInstStartDate, 14) >= 0)
    		{	
    			//订购生效时间大于m_sJudgeInstStartDate的用户才参与信控
        		return true;
        	}    		
    	}	
    }
    
    return false;
}
