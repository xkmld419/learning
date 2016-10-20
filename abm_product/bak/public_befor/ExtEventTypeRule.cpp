/*VER: 1*/ 
#include "ExtEventTypeRule.h"



SourceEventNode *ExtEventTypeRuleMgr::m_pHead = 0;

bool ExtEventTypeRuleMgr::bInit = false;
    
int  ExtEventTypeRuleMgr::m_iFirstOrgID = 0;

void ExtEventTypeRuleMgr::formatExtEventType (EventSection *_poEventSection)
{
    static Node *pRule;
    
    if (getExtEventTypeRule((StdEvent *)_poEventSection, &pRule)) {
        _poEventSection->m_iEventTypeID = pRule->m_iOutEventType;
    }
}

bool ExtEventTypeRuleMgr::getExtEventTypeRule (StdEvent const *_pEvent, Node ** _ppRuleNode)
{
    if (!bInit) init ();
    
    if (!m_pHead) return false;
    
    *_ppRuleNode = m_pHead->search (_pEvent);
    if (*_ppRuleNode) return true;
    
    return false;
}


void ExtEventTypeRuleMgr::init()
{
    if (bInit) return;
    bInit = true;
    
    DEFINE_QUERY(qry);
    
    qry.setSQL("select org_id from org where parent_org_id is null");
    qry.open();
    if (!qry.next())
          m_iFirstOrgID = 0;
    else  m_iFirstOrgID = qry.field(0).asInteger();
    qry.close();
    
    qry.setSQL (" select nvl(b.org_level_id,-1) level_1,  "
            " nvl(c.org_level_id,-1) level_2,   "
            " decode(in_event_type_id,null,1,0) s0,  "
            " length('@'||trim(calling_head_nbr)) s1, "
            " length('@'||trim(called_head_nbr)) s2, "
            " decode(switch_id,null,1,0) s5, "
            " decode(business_key,null,1,0) s6, " 
            " source_event_type,  "
            " nvl(in_event_type_id,0) in_event_type_id,  "
            " '@'||trim(calling_head_nbr) calling_head_nbr, "
            " '@'||trim(called_head_nbr) called_head_nbr, "
            " nvl(calling_org_id, :vFirstCallingOrg) calling_org_id,   "
            " nvl(called_org_id, :vFirstCalledOrg) called_org_id, "
            " nvl(switch_id,0) switch_id,  "
            " nvl(business_key,0) business_key, rule_id, " 
            " out_event_type_id   "
        " from b_ext_event_type_rule a, org b, org c   "
        " where a.calling_org_id = b.org_id(+) and a.called_org_id = c.org_id(+)   "
        " order by source_event_type, s0,  "
        "   s1 desc, calling_head_nbr, s2 desc, called_head_nbr, "
        "   level_1 desc, calling_org_id, level_2 desc, called_org_id,  "
        "   s5, s6");
    qry.setParameter ("vFirstCallingOrg", m_iFirstOrgID);
    qry.setParameter ("vFirstCalledOrg", m_iFirstOrgID);
    
    qry.open ();
    
    while (qry.next()) {
        
        ExtEventTypeRule extRule;
        
        extRule.m_iSourceEventType = qry.field("source_event_type").asInteger();
        extRule.m_iInEventType     = qry.field("in_event_type_id").asInteger();
        strcpy (extRule.m_sCallingHead, qry.field("calling_head_nbr").asString());
        strcpy (extRule.m_sCalledHead, qry.field("called_head_nbr").asString());
        extRule.m_iCallingOrgID    = qry.field("calling_org_id").asInteger();
        extRule.m_iCalledOrgID     = qry.field("called_org_id").asInteger();
        extRule.m_iSwitchID        = qry.field("switch_id").asInteger();
        extRule.m_iBusinessKey     = qry.field("business_key").asInteger();
        extRule.m_iOutEventType    = qry.field("out_event_type_id").asInteger();
        extRule.m_iRuleID          = qry.field("rule_id").asInteger();
        
        if (!m_pHead)
            m_pHead = new SourceEventNode();
        
        m_pHead->insertSourceEvent (&extRule);
    }   
}


Node * InEventNode::search (StdEvent const *_pEvent)
{
    if (m_iInEventType==0/*不限制*/ || m_iInEventType==_pEvent->m_iEventTypeID) 
    {
        //if (!m_pCallingHeadNode || !m_pCallingHeadTree) THROW(234);
        
        static CallingHeadNode *pCallingHeadNode;
        static char sKey[50];
        static Node *pNode;
        
        sprintf (sKey, "@%s", _pEvent->m_sCallingNBR);

        while (m_pCallingHeadTree->getMax (sKey, &pCallingHeadNode, sKey))
        {
            if (pNode = pCallingHeadNode->search (_pEvent)) 
                return pNode;
            
            sKey[strlen(sKey)-1] = 0;
            if (!sKey[0]) break;
        }
    }
    
    if (!m_pNext) return 0;
    return m_pNext->search (_pEvent);
}


Node * CallingHeadNode::search (StdEvent const *_pEvent)
{
    //if (!m_pCalledHeadNode || !m_pCalledHeadTree) THROW (456);
    
    static CalledHeadNode *pCalledHeadNode;
    static char sKey[60];
    static Node *pNode;
    
    sprintf (sKey, "@%s", _pEvent->m_sCalledNBR);
    
    while (m_pCalledHeadTree->getMax (sKey, &pCalledHeadNode, sKey))
    {
        if (pNode = pCalledHeadNode->search (_pEvent)) 
            return pNode;
        
        sKey[strlen(sKey)-1] = 0;
        if (!sKey[0]) return 0;
    }
    return 0;
}


Node * CalledHeadNode::search (StdEvent const *_pEvent)
{
    //if (!m_pCallingOrgNode || !m_pCallingOrgTree)  THROW (456);
    
    static char sKey[60];
    static OrgPath *pPath;
    static CallingOrgNode *pCallingOrgNode;
    static Node *pNode;
    static int iCallingOrgID;
    
    if (!(iCallingOrgID = _pEvent->m_oEventExt.m_iCallingOrgID))
        iCallingOrgID = ExtEventTypeRuleMgr::m_iFirstOrgID;
    
    if (!(pPath = OrgPathMgr::getOrgPath (iCallingOrgID))) return 0;
    strcpy (sKey, pPath->m_sOrgPath);

    while (m_pCallingOrgTree->getMax(sKey, &pCallingOrgNode, sKey)) 
    {
        if (pNode = pCallingOrgNode->search (_pEvent))
            return pNode;
        
        //找父节点org的路径
        if (!OrgPathMgr::getParentOrgPath (sKey, sKey))
            return 0;
    }
    
    return 0;
}

Node * CallingOrgNode::search (StdEvent const *_pEvent)
{
    //if (!m_pCalledOrgNode || !m_pCalledOrgTree) THROW (456);
    
    static char sKey[60];
    static OrgPath *pPath;
    static CalledOrgNode *pCalledOrgNode;
    static Node *pNode;
    static int iCalledOrgID;
    
    if (!(iCalledOrgID = _pEvent->m_oEventExt.m_iCalledOrgID))
        iCalledOrgID = ExtEventTypeRuleMgr::m_iFirstOrgID;
        
    if (!(pPath = OrgPathMgr::getOrgPath (iCalledOrgID))) return 0;
    strcpy (sKey, pPath->m_sOrgPath);
    
    while (m_pCalledOrgTree->getMax(sKey, &pCalledOrgNode, sKey)) 
    {
        if (pNode = pCalledOrgNode->search (_pEvent))
            return pNode;
        
        //找父节点org的路径
        if (!OrgPathMgr::getParentOrgPath (sKey, sKey))
            return 0;
    }
    
    return 0;
}
