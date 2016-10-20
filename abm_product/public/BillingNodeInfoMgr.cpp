#include "BillingNodeInfoMgr.h"
#include "Environment.h"
#include "Process.h"
#include "OrgMgrEx.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

BillingNodeInfo::BillingNodeInfo()
{
	//memset((char *)this, 0, sizeof(BillingNodeInfo));
	m_iNodeID = 0;
	m_iParentNodeID = 0;
	m_iOrgID = 0;
	m_iConditionID = 0;//其实不在需要condition了，相当于剥离到文件池的CONITION_ID
	
	m_poNext = NULL;
}	

BillingNodeInfo::~BillingNodeInfo()
{
	
}	

bool BillingNodeInfoMgr::m_bIsLoad = false;

BillingNodeInfoMgr::BillingNodeInfoMgr()
{
    #ifdef PRIVATE_MEMORY
	load();
    #endif
}	

BillingNodeInfoMgr::~BillingNodeInfoMgr()
{
    #ifdef PRIVATE_MEMORY
	unload();
    #endif
}	

#ifdef PRIVATE_MEMORY
//按org_id索引
bool BillingNodeInfoMgr::load()
{
	BillingNodeInfo *pTemp = NULL;
	DEFINE_QUERY(qry);
	
	//org_id都配置为200级别org
	qry.setSQL(" select node_id,parent_node_id,org_id,nvl(condtion_id,0) " 
			   " from b_billing_node_define t where t.node_level=100 "
			   " order by org_id,nvl(condtion_id,0) ");
	//目前暂不支撑condtion配置			   
			   
	qry.open();
	while (qry.next())			   
	{
		BillingNodeInfo *pBillingNodeInfo = new BillingNodeInfo();
		pBillingNodeInfo->m_iNodeID = qry.field(0).asInteger();
		pBillingNodeInfo->m_iParentNodeID = qry.field(1).asInteger();
		pBillingNodeInfo->m_iOrgID = qry.field(2).asInteger();
		pBillingNodeInfo->m_iConditionID = qry.field(3).asInteger();
		
		m_mIter = m_mapOrg_NodeInfo.find(pBillingNodeInfo->m_iOrgID);
		
		if (m_mIter == m_mapOrg_NodeInfo.end())
		{
			m_mapOrg_NodeInfo[pBillingNodeInfo->m_iOrgID] = pBillingNodeInfo;
		}else
		{
			pTemp =	m_mIter->second;
			while (pTemp->m_poNext != NULL)
			{
				pTemp = pTemp->m_poNext;
			}
			
			pTemp->m_poNext = pBillingNodeInfo;
		}	
	}
	qry.close();

	m_bIsLoad = true;
	
	return true;
}	

bool BillingNodeInfoMgr::unload()
{
	BillingNodeInfo *pTemp = NULL;
	BillingNodeInfo *pTemp1 = NULL;

	for (m_mIter = m_mapOrg_NodeInfo.begin(); m_mIter != m_mapOrg_NodeInfo.end(); ++m_mIter)
	{
		pTemp =	m_mIter->second;
		while(pTemp != NULL)
		{
			pTemp1 = pTemp;
			pTemp = pTemp->m_poNext;
			delete pTemp1;
			pTemp1 = NULL;
		}
	}
	m_bIsLoad = false;
	
	return true;
}	
#endif

//先默认事件获取了billing_org_id获取到了
bool BillingNodeInfoMgr::getNodeID(EventSection *pEvent)
{
	static OrgMgrEx *pOrgMgrEx = OrgMgrEx::GetInstance();
	
	int iLevelOrgID = pOrgMgrEx->getLevelParentID(pEvent->m_iBillingOrgID, 200);
	
	if (iLevelOrgID == -1)
	{
		Log::log(0, "OrgID:%d未求取到200级orgID", pEvent->m_iBillingOrgID);
		return false;
	}	
    #ifdef PRIVATE_MEMORY
	m_mIter = m_mapOrg_NodeInfo.find(iLevelOrgID);
	
	if (m_mIter == m_mapOrg_NodeInfo.end())
	{
		Log::log(0, "OrgID:%d未求取到计费节点", iLevelOrgID);
		return false;//暂时先这样处理
	}	
	BillingNodeInfo *pTemp = m_mIter->second;
    #else
    unsigned int k = 0;
    if (!G_PPARAMINFO->m_poBillingNodeIndex->get(iLevelOrgID, &k) ) {
        Log::log(0, "OrgID:%d未求取到计费节点", iLevelOrgID);
        return false;//暂时先这样处理
    }
    BillingNodeInfo *pTemp = &(G_PPARAMINFO->m_poBillingNodeList[k]);
    #endif
    while (pTemp && pTemp->m_iConditionID != 0) {
		if (Process::m_poConditionMgr->check(pTemp->m_iConditionID, pEvent))
			break;
			
        #ifdef PRIVATE_MEMORY
		pTemp = m_mIter->second->m_poNext;
        #else 
        pTemp = pTemp->m_poNext;
        #endif
	}	
	
	if (pTemp == NULL)
	{
		Log::log(0, "OrgID:%d未求取到计费节点", iLevelOrgID);		
		return false;
	}	
	
	pEvent->m_oEventExt.m_iNodeID = pTemp->m_iNodeID;
	return true;
}	

////////////////////////////////////////////////////////////////////////////////

ProcessNodeInfo::ProcessNodeInfo()
{
	m_iProcessNodeID = 0;
	m_iListenPortID	= 0;
}
	
ProcessNodeInfo::ProcessNodeInfo(int iProcessNodeID)
{
	init(iProcessNodeID);
}	

ProcessNodeInfo::~ProcessNodeInfo()
{
	
}	

bool ProcessNodeInfo::init(int iProcessNodeID)
{
	m_iProcessNodeID = iProcessNodeID;
	
	DEFINE_QUERY(qry);
	
	qry.setSQL("select LISTEN_PORT,FILE_POOL_ID from b_process_node_info t where t.process_node_id=:node_id");
	qry.setParameter("node_id", iProcessNodeID);
	
	qry.open();
	
	if(qry.next())
	{
		m_iListenPortID = qry.field(0).asInteger();
		m_iFilePoolID = qry.field(1).asInteger();
		qry.close();
		return true;
	}
	
	qry.close();
	return false;
}	

SwitchInfoMgr::SwitchInfoMgr()
{
	m_pAreaCodeSwithInfo = new HashList<HashList<TSwitchInfo *> *>(20);	
}	
		
SwitchInfoMgr::~SwitchInfoMgr()
{
	
}	

TSwitchInfo* SwitchInfoMgr::getSwitchInfo(int iFileTypeID, int iNodeID)
{
	HashList<TSwitchInfo *> *pSwitchInfoHash = NULL;
	
	if (!m_pAreaCodeSwithInfo->get(iNodeID, &pSwitchInfoHash))
	{
		pSwitchInfoHash = loadAreaSwithInfo(iNodeID);
	}
	
	TSwitchInfo* pTSwitchInfo = NULL;
	if(!pSwitchInfoHash->get(iFileTypeID, &pTSwitchInfo))
	{
		return NULL;
	}
	
	return pTSwitchInfo;
}	

HashList<TSwitchInfo *> *SwitchInfoMgr::loadAreaSwithInfo(int iNodeID)
{
	HashList<TSwitchInfo *> *pFileTypeSwitchInfoHash = new HashList<TSwitchInfo *>(50);
	
    DEFINE_QUERY(qry);

    qry.setSQL("select switch_id,file_type_id,area_code,source_event_type,switch_type_id, "
        " nvl(network_id, 0) network_id, nvl(file_Type_id, -1) file_Type_id "
        " from b_switch_info where node_id = :node_id "
        " and file_type_id <> -1 ");
        
	qry.setParameter("node_id", iNodeID);
	qry.open();
	while(qry.next())
	{
        int iFileTypeID = qry.field("file_type_id").asInteger();
				
		TSwitchInfo *pSwitchInfo = new TSwitchInfo();
        pSwitchInfo->m_iSwitchID = qry.field("switch_id").asInteger();
        pSwitchInfo->m_iSourceEventType = qry.field("source_event_type").asInteger();
        pSwitchInfo->m_iSwitchTypeID = qry.field("switch_type_id").asInteger();
        strcpy(pSwitchInfo->m_sAreaCode, qry.field("area_code").asString());
        pSwitchInfo->m_iNetworkID = qry.field("network_id").asInteger();

        pFileTypeSwitchInfoHash->add(iFileTypeID, pSwitchInfo);        
	}   
	
	qry.close();     
	
	m_pAreaCodeSwithInfo->add(iNodeID, pFileTypeSwitchInfoHash);
	
	return pFileTypeSwitchInfoHash;
}	
