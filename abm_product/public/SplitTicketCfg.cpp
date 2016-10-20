/*VER: 1*/ 
#include "SplitTicketCfg.h"
#include "Process.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

SplitTicketCfg* SplitTicketCfg::m_gpSplitTicketCfg = 0;
	
#ifdef PRIVATE_MEMORY	
SplitTicketCfg* SplitTicketCfg::GetSplitTicketCfg()
{
    if(SplitTicketCfg::m_gpSplitTicketCfg ==0)
    {
        SplitTicketCfg::m_gpSplitTicketCfg = new SplitTicketCfg();
        SplitTicketCfg::m_gpSplitTicketCfg->Init();
    }
    return SplitTicketCfg::m_gpSplitTicketCfg;
}
#else
SplitTicketCfg* SplitTicketCfg::GetSplitTicketCfg()
{
    if(SplitTicketCfg::m_gpSplitTicketCfg ==0)
    {
        SplitTicketCfg::m_gpSplitTicketCfg = new SplitTicketCfg();
		SplitTicketCfg::m_gpSplitTicketCfg->Init();
    }
    return SplitTicketCfg::m_gpSplitTicketCfg;
}
#endif

#ifdef PRIVATE_MEMORY
SplitTicketCfg::SplitTicketCfg(void)
{
    m_EventTypeIDMap.clear();
    memset(&m_oStdEventPlus[0],0,sizeof(m_oStdEventPlus[MAX_CHARGE_NUM]));
    m_nCount = 0;
    m_pEventTypeMgr = 0;
    memset(m_nPos,0,sizeof(int[MAX_CHARGE_NUM]));
}

#else
SplitTicketCfg::SplitTicketCfg(void)
{
    
    m_nCount = 0;
    m_pEventTypeMgr = 0;
}
#endif

#ifdef PRIVATE_MEMORY
SplitTicketCfg::~SplitTicketCfg(void)
{
    m_EventTypeIDMap.clear();
    this->m_pEventTypeMgr->unload();
}
#else
SplitTicketCfg::~SplitTicketCfg(void)
{
}
#endif

#ifdef PRIVATE_MEMORY
bool SplitTicketCfg::Init()
{
    TOCIQuery qry(Environment::getDBConn());

    qry.setSQL("SELECT ACCT_ITEM_TYPE_ID,SPLIT_GROUP_ID,nvl(EVENT_TYPE_ID,-1) FROM B_SPLIT_ACCT_ITEM_GROUP ");
    qry.open();
    int nEventTypeID=0;
    int nAcctItemType=0;
    int nSplitGroupID = 0;
    map< int,map<int, int> >::iterator it;
    map<int,int>::iterator itTmp;
    while(qry.next())
    {
        nEventTypeID = qry.field(2).asInteger();
        it = m_EventTypeIDMap.find(nEventTypeID);
        nAcctItemType = qry.field(0).asInteger();
        nSplitGroupID = qry.field(1).asInteger();
        if(it == m_EventTypeIDMap.end())
        {
            map<int,int>map_Temp;
            map_Temp[nAcctItemType]=nSplitGroupID;
            m_EventTypeIDMap[nEventTypeID]= map_Temp;
        }else
        {
            it->second.insert(map<int,int>::value_type(nAcctItemType, nSplitGroupID));
        }
    }
    qry.close();

    qry.setSQL("SELECT SPLIT_GROUP_ID,NVL(EVENT_TYPE_ID,0),NVL(OPERATION_ID,0) FROM B_SPLIT_ACCT_GROUP2EVENT_TYPE ");
    qry.open();
    int nGroupID = 0;
    int nDesEvenTypeID = 0;
    int nOperationID = 0;
    while(qry.next())
    {
        nGroupID = qry.field(0).asInteger();
        nDesEvenTypeID = qry.field(1).asInteger();
        nOperationID = qry.field(2).asInteger();
        m_GroupIDMapEventType.insert(map<int,pair<int,int> >::value_type(nGroupID,pair<int,int>(nDesEvenTypeID,nOperationID)));
    }
    qry.close();
    m_pEventTypeMgr = new EventTypeMgr();
    m_pEventTypeMgr->load();
    return true;
}
#else
bool SplitTicketCfg::Init()
{
	m_pEventTypeMgr = new EventTypeMgr();
}
#endif

#ifdef PRIVATE_MEMORY
int SplitTicketCfg::GetAcctItemGroupID(int nEventTypeID,int nAcctItemTypeID)
{
    map<int,map<int,int> >::iterator it = m_EventTypeIDMap.find(nEventTypeID);
    map<int,int >::iterator itEnd;
    if(it != m_EventTypeIDMap.end())
    {
        map<int,int>::iterator itTemp = it->second.find(nAcctItemTypeID);
        if(itTemp == it->second.end())
            return -1;
        else
            return itTemp->second;
    }else
    {
        it = m_EventTypeIDMap.begin();
        for( ; it != m_EventTypeIDMap.end();it++)
        {
        		if(!m_pEventTypeMgr)
        				return -1;
            if(m_pEventTypeMgr->getBelong(nEventTypeID,it->first))
            {
                map<int,int>::iterator itTemp = it->second.find(nAcctItemTypeID);
                if(itTemp == it->second.end())
                    return -1;
                else
                    return itTemp->second;
            }
        }
        return -1;
    }
}
#else
int SplitTicketCfg::GetAcctItemGroupID(int nEventTypeID,int nAcctItemTypeID)
{
    unsigned int iIdx = 0;
    int iTotal = 0;
    SplitAcctItemGroup *pSplitAcctItemGroup = 0;
    SplitAcctItemGroup *pHead = G_PPARAMINFO->m_poSplitAcctItemGroupList;

    if(G_PPARAMINFO->m_poSplitAcctItemGroupIndex->get(nEventTypeID, &iIdx))
    {
        pSplitAcctItemGroup = pHead + iIdx;
        while(pSplitAcctItemGroup != pHead)
        {
            if(pSplitAcctItemGroup->m_iAcctItemType == nAcctItemTypeID)
            {
                return pSplitAcctItemGroup->m_iSplitGroupID;
            }
            else
            {
                pSplitAcctItemGroup = pHead + pSplitAcctItemGroup->m_iNextAcctItemOffset;
            }
        }
        return -1;
    }
    else
    {
        iTotal = G_PPARAMINFO->m_poSplitAcctItemGroupData->getCount();
        pSplitAcctItemGroup = pHead + iTotal;
        while(pSplitAcctItemGroup != pHead)
        {
            if(!m_pEventTypeMgr)
                return -1;
            
            if(m_pEventTypeMgr->getBelong(nEventTypeID, pSplitAcctItemGroup->m_iEventTypeID))
            {
                while(pSplitAcctItemGroup != pHead)
                {
                    if(pSplitAcctItemGroup->m_iAcctItemType == nAcctItemTypeID)
                    {
                        return pSplitAcctItemGroup->m_iSplitGroupID;
                    }
                    else
                    {
                        pSplitAcctItemGroup = pHead + pSplitAcctItemGroup->m_iNextAcctItemOffset;
                    }
                }
                return -1;
            }
            else
            {
                pSplitAcctItemGroup = pHead + pSplitAcctItemGroup->m_iNextEventOffset;
            }
        }
        return -1;
    }

}
#endif

#ifdef PRIVATE_MEMORY
int SplitTicketCfg::GetDesEventTypeID(int nSplitGroupID,int& nOperationID)
{
    int nRetEventTypeID = 0;
    m_GroupIDMapIt = m_GroupIDMapEventType.find(nSplitGroupID);
    if(m_GroupIDMapIt != m_GroupIDMapEventType.end())
    {
        nRetEventTypeID = m_GroupIDMapIt->second.first;
        nOperationID = m_GroupIDMapIt->second.second;
        return nRetEventTypeID;
    }
    else
        return 0;
}
#else
int SplitTicketCfg::GetDesEventTypeID(int nSplitGroupID,int& nOperationID)
{
    unsigned int iIdx = 0;
    int nRetEventTypeID = 0;
    SplitAcctGroupEvent *pSplitAcctGroupEvent = 0;
    SplitAcctGroupEvent *pHead = G_PPARAMINFO->m_poSplitAcctGroupEventList;

    if(G_PPARAMINFO->m_poSplitAcctGroupEventIndex->get(nSplitGroupID, &iIdx))
    {
        pSplitAcctGroupEvent = pHead + iIdx;
        nRetEventTypeID = pSplitAcctGroupEvent->m_iEventTypeID;
        nOperationID = pSplitAcctGroupEvent->m_iOperationID;
        return nRetEventTypeID;
    }
    else
        return 0;
    

}
#endif

int SplitTicketCfg::SplitTicket(StdEvent* pStdEvent)
{
    memset(&m_oStdEventPlus,0,sizeof(StdEventPlus)*MAX_CHARGE_NUM);
    if(0 == pStdEvent)
        return 0;
    int nTicketNum = 0;
    m_GroupIDMapStdEvent.clear();
    m_itEnd= m_GroupIDMapStdEvent.end();
    int nGroupID = 0;
    int nPos = 0;
    int nDesEventTypeID = 0;
    int nSrcEventTypeID = pStdEvent->m_iEventTypeID;
    memset(m_nPos,0,sizeof(int[MAX_CHARGE_NUM]));
    for(int i =0; i<MAX_CHARGE_NUM ;i++)
    {
        if( 0==pStdEvent->m_oChargeInfo[i].m_iAcctItemTypeID)
            continue;
	nGroupID = GetAcctItemGroupID(pStdEvent->m_iEventTypeID,pStdEvent->m_oChargeInfo[i].m_iAcctItemTypeID);
        m_it = m_GroupIDMapStdEvent.find(nGroupID);
        if(m_it == m_itEnd)
        {
            m_GroupIDMapStdEvent.insert(map<int,int>::value_type(nGroupID,nTicketNum));
            m_oStdEventPlus[nTicketNum].m_oStdEvent = *pStdEvent;
	    memset(&m_oStdEventPlus[nTicketNum].m_oStdEvent.m_oChargeInfo[0],0,sizeof(m_oStdEventPlus[nTicketNum].m_oStdEvent.m_oChargeInfo));
            m_oStdEventPlus[nTicketNum].m_oStdEvent.m_oChargeInfo[0] = pStdEvent->m_oChargeInfo[i];
            
            int nOperationID = 0;
            nDesEventTypeID = GetDesEventTypeID(nGroupID,nOperationID);

            if(nDesEventTypeID !=0)
            {
                m_oStdEventPlus[nTicketNum].m_oStdEvent.m_iEventTypeID = nDesEventTypeID;
                m_oStdEventPlus[nTicketNum].nOperation = nOperationID;
            }
            m_nPos[nTicketNum]++; 
		nTicketNum++;
            nPos = 1;
        }else
        {
            //找到已经存在的
            m_oStdEventPlus[m_it->second].m_oStdEvent.m_oChargeInfo[m_nPos[m_it->second]]=pStdEvent->m_oChargeInfo[i];
            m_nPos[m_it->second]++;
	    nPos++;
        }
    }
    if(nTicketNum == 1)
        m_oStdEventPlus[0].m_oStdEvent.m_iEventTypeID = nSrcEventTypeID;
    m_nCount = nTicketNum;
    return nTicketNum;

}

int SplitTicketCfg::GetSplitCount()
{
    return m_nCount;
}

bool SplitTicketCfg::GetSplitTicket(StdEvent* pStdEvent,int nPos)
{
    if(nPos >=MAX_CHARGE_NUM)
        return false;
    *pStdEvent = m_oStdEventPlus[nPos].m_oStdEvent;
    if(m_oStdEventPlus[nPos].nOperation !=0)
    {
        Process::m_poOperationMgr->execute(m_oStdEventPlus[nPos].nOperation,pStdEvent);
    }
    return true;
}

