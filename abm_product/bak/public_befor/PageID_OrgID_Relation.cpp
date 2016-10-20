/*VER: 1*/ 
#include "PageID_OrgID_Relation.h"

bool IpPageConvToOrg::m_bInited = false;

KeyTree<CheckIpPageItem> * IpPageConvToOrg::m_pIpPageTree = 0;

void IpPageConvToOrg::load ()
{
    if (!m_bInited)
        m_bInited = true;
    else  return;
    
    m_pIpPageTree = new KeyTree<CheckIpPageItem>;
    
    DEFINE_QUERY (qry);
    
    qry.setSQL ("select ip||'#'||Page_id, org_id, length(ip)+2 from b_PageID_Orgid_relation");
    
    qry.open ();
    
    while (qry.next ())
    {
        CheckIpPageItem oItem;
        
        oItem.m_lOrgID = qry.field(1).asLong();
        oItem.m_iMinCheckLen = qry.field(2).asInteger();
        
        m_pIpPageTree->add (qry.field(0).asString(), oItem);
    }
    
    qry.close ();
}


bool IpPageConvToOrg::convert (EventSection *pEvt)
{
    if (!m_bInited)
        load ();
    
    static char sKey[30];
    //static long lOrgID;
    static CheckIpPageItem oItem;
    static int iMatchLen;
    
    sprintf (sKey, "%s#%d", pEvt->m_sNASIp, pEvt->m_iNASPortID);
    
    if (!m_pIpPageTree->getMax (sKey, &oItem, &iMatchLen))
        return false;
    
    if (iMatchLen < oItem.m_iMinCheckLen)
        return false;
    
    pEvt->m_oEventExt.m_iCallingOrgID = oItem.m_lOrgID;
    
    return true;
}


