/*VER: 1*/ 
/*
**
**  2007.12.12 caidk
**      created
**  
** 
*/


#include "NetworkMgr.h"
#include "Environment.h"
#include "Exception.h"
#include "TOCIQuery.h"
#include "Log.h"
#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

NetworkPriority NetworkPriority::NETWORK_NONE(0,0);
	
#ifndef PRIVATE_MEMORY
	
NetworkPriority * NetworkPriorityMgr::getHeadNetworkPriority()
{
	
	return (NetworkPriority*)G_PPARAMINFO->m_poNetworkPriorList;
}

#endif 


void NetworkPriorityMgr::load()
{
	unload ();
    char sql[2048];
    int count;
    NetworkPriority * pNetInfo;
	DEFINE_QUERY (qry);
	
	qry.setSQL ("select count(*) total from b_network_priority");
	qry.open();
	qry.next();
	count = qry.field(0).asInteger ();
	qry.close();
	if(count < 15)
	{
	    count = 15;
	}
	
	m_poNetworkPriList = new NetworkPriority[count];
	if (!m_poNetworkPriList) THROW(MBC_PUBLIC+1);
	m_poNetworkPriIndex = new HashList<NetworkPriority *>(count);
	if (!m_poNetworkPriIndex) THROW(MBC_PUBLIC+2);
	
	qry.setSQL ("SELECT A.NETWORK_SOURCE_ID, A.NETWORK_ID, A.PRIORITY "
                  "FROM b_network_priority A "
                  "ORDER BY NETWORK_SOURCE_ID, PRIORITY DESC");
                        
    qry.open();
    int iLastSourceID = -1;
    NetworkPriority * pLastPri = 0;
    while (qry.next()) {
        count--;
        pNetInfo = m_poNetworkPriList + count;
        pNetInfo->m_iNetworkSourceID = qry.field(0).asInteger();
        if(pNetInfo->m_iNetworkSourceID != iLastSourceID)
        {
            iLastSourceID = pNetInfo->m_iNetworkSourceID;
            m_poNetworkPriIndex->add(pNetInfo->m_iNetworkSourceID, pNetInfo);
        }
        else
        {
            pLastPri->m_poNext = pNetInfo;
        }
        pNetInfo->m_iNetworkID = qry.field(1).asInteger();
        pNetInfo->m_iPriority = qry.field(2).asInteger();
        pLastPri = pNetInfo;
        
    }
    qry.close();
	m_bUploaded = true;
}

//##ModelId=42369FF001FD
void NetworkPriorityMgr::unload()
{
	m_bUploaded = false;
	if(m_poNetworkPriList)
	{
	    delete[] m_poNetworkPriList;
	    m_poNetworkPriList = 0;
	}
	if(m_poNetworkPriIndex)
	{
	    delete m_poNetworkPriIndex;
	    m_poNetworkPriIndex = 0;
	}
}

#ifdef PRIVATE_MEMORY
NetworkPriority * NetworkPriorityMgr::getPriority(int iNetworkSourceID)
{
    NetworkPriority * pRet;
    if(!m_poNetworkPriIndex->get(iNetworkSourceID, &pRet))
    {
        return 0;
    }
    return pRet;
}
#else
NetworkPriority * NetworkPriorityMgr::getPriority(int iNetworkSourceID)
{
	NetworkPriority * pPrior=0;
	NetworkPriority * pNetworkPrior=0;
	unsigned int iIdx = 0;

	pPrior = (NetworkPriority*)G_PPARAMINFO->m_poNetworkPriorList;
	if(G_PPARAMINFO->m_poNetworkPriorIndex->get(iNetworkSourceID,&iIdx))
	{
		pNetworkPrior = pPrior + iIdx;
		return pNetworkPrior;
	}
	else
		return NULL;
}
#endif




NetworkPriorityMgr::NetworkPriorityMgr():m_poNetworkPriList(0),
m_poNetworkPriIndex(0),m_bUploaded(false)
{
}

#ifdef PRIVATE_MEMORY
NetworkPriorityMgr::~NetworkPriorityMgr()
{
    unload();
}
#else
NetworkPriorityMgr::~NetworkPriorityMgr()
{
}
#endif 

