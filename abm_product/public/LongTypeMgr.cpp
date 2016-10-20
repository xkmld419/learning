/*VER: 1*/ 
#include "LongTypeMgr.h"
#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

#ifdef PRIVATE_MEMORY
bool LongTypeMgr::getLongType (EventSection *pEvt)
{
    int iRet;
    
    if (!m_poLongType)
        loadLongTypeRule ();
    
    if (m_poLongType->get (pEvt->m_oEventExt.m_iCalledLongGroupTypeID, &iRet))
    {
        pEvt->m_oEventExt.m_iLongTypeID = iRet;
        return true;
    }
    
    return false;
}
#else
bool LongTypeMgr::getLongType (EventSection *pEvt)
{
    unsigned int iRet = 0;
    
    if (G_PPARAMINFO->m_poLongTypeIndex->get (pEvt->m_oEventExt.m_iCalledLongGroupTypeID, &iRet))
    {
        pEvt->m_oEventExt.m_iLongTypeID = iRet;
        return true;
    }
    
    return false;
}
#endif

void LongTypeMgr::loadLongTypeRule ()
{
    if (m_poLongType)
        delete m_poLongType;
    
    m_poLongType = new HashList<int> (13);
    
    if (!m_poLongType) THROW(1234);
    
    DEFINE_QUERY (qry);
    
    qry.setSQL ("select CALLED_LONG_GROUP_TYPE_ID, LONG_TYPE_ID from B_LONG_TYPE_RULE");
    qry.open ();
    
    while (qry.next()) {
        int iGroup = qry.field(0).asInteger();
        int iLongType = qry.field(1).asInteger();
        
        m_poLongType->add (iGroup, iLongType);
    }
    qry.close();
}


HashList<int> *LongTypeMgr::m_poLongType = 0;
