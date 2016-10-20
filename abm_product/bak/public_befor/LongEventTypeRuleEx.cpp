/*VER: 3*/ 
#include "LongEventTypeRuleEx.h"
#include "EventSection.h"
#include <iostream>
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif 
using namespace std;

//##ModelId=42817EC803D9
LongEventTypeRuleEx *LongEventTypeRuleMgrEx::m_poRule = NULL;

//##ModelId=4281AE010170
bool LongEventTypeRuleMgrEx::m_bInit = false;

//##ModelId=428180AA0011
#ifdef PRIVATE_MEMORY
LongEventTypeRuleMgrEx::LongEventTypeRuleMgrEx()
{
	loadRule();
}
#else
LongEventTypeRuleMgrEx::LongEventTypeRuleMgrEx()
{
}
#endif

//##ModelId=428180DC0366
#ifdef PRIVATE_MEMORY
LongEventTypeRuleMgrEx::~LongEventTypeRuleMgrEx()
{
	unloadRule();
}
#else
LongEventTypeRuleMgrEx::~LongEventTypeRuleMgrEx()
{
}
#endif

//##ModelId=428180E701F9
void LongEventTypeRuleMgrEx::loadRule()
{
	unloadRule();
	
	m_bInit = true;
	
	LongEventTypeRuleEx **ppNextEvent = &m_poRule;
	LongEventTypeRuleEx **ppNextRule = &m_poRule;
	LongEventTypeRuleEx *pt = NULL;
	
	DEFINE_QUERY(qry);
	
	qry.close();
	qry.setSQL(" select decode(in_event_type_id,null,1,0) s0, "
		 "decode(long_type_id,Null,1,0) s1, decode(roam_type_id,null,1,0) s2, "
		 "decode(PARTY_ROLE_ID,null,1,0) s3, decode(PARTY_ID,null,1,0) s4, "
		 "decode(carrier_type_id,null,1,0) s7,decode(business_key,null,1,0) s8, " 
		 "rule_id, source_event_type,  "
		 "nvl(in_event_type_id,-1) in_event_type_id , " 
		 "nvl(long_type_id,-1) long_type_id, "
		 "nvl(roam_type_id,-1) roam_type_id,  "
		 "nvl(   PARTY_ROLE_ID   ,-1) PARTY_ROLE_ID, "
		 "nvl(   PARTY_ID   ,-1) PARTY_ID, "
		 "nvl(carrier_type_id,-1) carrier_type_id, "
		 "nvl(business_key,-1) business_key, out_event_type_id, "
         "nvl(call_type_id,-1) calltypeid "
		 "from B_TONE_EVENT_TYPE_RULE "
		 "order by source_event_type, s0,s1,s2,s4,s3,s7,s8  "
	);
	qry.open();
	
	int iLastSourceEventType = -1;
	
	while (qry.next()) {
		
		pt = new LongEventTypeRuleEx;
		if (!pt)
			THROW(MBC_LongEvtRule+1);
		
		pt->m_iRuleID = qry.field("rule_id").asInteger();
		pt->m_iSourceEventType = qry.field("source_event_type").asInteger();
		pt->m_iInEventTypeID = qry.field("in_event_type_id").asInteger();
		pt->m_iLongTypeID = qry.field("long_type_id").asInteger();
		pt->m_iRoamTypeID = qry.field("roam_type_id").asInteger();
        pt->m_iPartyRuleID =  qry.field("PARTY_ROLE_ID").asInteger();
        pt->m_iPartyID = qry.field("PARTY_ID").asInteger();
		pt->m_iCarrierTypeID = qry.field("carrier_type_id").asInteger();
		pt->m_iBusinessKey = qry.field("business_key").asInteger();
		pt->m_iOutEventTypeID = qry.field("out_event_type_id").asInteger();
        pt->m_iCallTypeID = qry.field("calltypeid").asInteger();
		
		if (pt->m_iSourceEventType != iLastSourceEventType) {
			iLastSourceEventType = pt->m_iSourceEventType;
			(*ppNextEvent) = pt;
			ppNextEvent = &(pt->m_poNextEvent);
		}
		else {
			(*ppNextRule) = pt;
		}
		ppNextRule = &(pt->m_poNextRule);
	}
	
}

//##ModelId=428180EF029B
void LongEventTypeRuleMgrEx::unloadRule()
{
	if (!m_bInit)
		return;
	
	delete m_poRule;
	m_poRule = NULL;
	m_bInit = false;
}

#ifdef PRIVATE_MEMORY
bool LongEventTypeRuleMgrEx::reloadRule()
{
	if (!m_bInit)
		return true;
	
	unloadRule();
	loadRule();
	return true;
	
}
#else
bool LongEventTypeRuleMgrEx::reloadRule()
{
	return true;
}
#endif


//##ModelId=428180F601D3
#ifdef PRIVATE_MEMORY
bool LongEventTypeRuleMgrEx::getRule(EventSection const *_pEvent, 
		LongEventTypeRuleEx** _ppRule)
{
	if (!_pEvent)
		return false;
	
	LongEventTypeRuleEx *pt;
	
	if (!m_bInit)
		loadRule();
	
	//查找当前事件类型对应的链表
	for (pt = m_poRule; pt; pt = pt->m_poNextEvent) {
		if (pt->m_iSourceEventType == _pEvent->m_oEventExt.m_iSourceEventType)
			break;
	}
	if (!pt) 
		return false;
	
	//查找当前事件类型链表中对应的规则
	for ( ; pt; pt = pt->m_poNextRule) {
		
		if (pt->m_iInEventTypeID != -1 && pt->m_iInEventTypeID != _pEvent->m_iEventTypeID)
			continue;
			
		if (pt->m_iLongTypeID != -1 && pt->m_iLongTypeID != _pEvent->m_oEventExt.m_iLongTypeID)
			continue;		
		if (pt->m_iRoamTypeID != -1 && pt->m_iRoamTypeID != _pEvent->m_oEventExt.m_iRoamTypeID)
			continue;
      if(_pEvent->m_iCallTypeID== CALLTYPE_MTC)
        {
            if (pt->m_iPartyID != -1 && pt->m_iPartyID != _pEvent->m_iCallingSPTypeID)
			continue;
        }else
        {
            if (pt->m_iPartyID != -1 && pt->m_iPartyID != _pEvent->m_iCalledSPTypeID)
			continue;
        }
		/*if (pt->m_iCalledSPTypeID != -1 && pt->m_iCalledSPTypeID != _pEvent->m_iCalledSPTypeID)
			continue;*/
        if (pt->m_iPartyRuleID != -1 && pt->m_iPartyRuleID != _pEvent->m_iParterID)
			continue;
		/*if (pt->m_iCalledServiceTypeID != -1 && pt->m_iCalledServiceTypeID != _pEvent->m_iCalledServiceTypeID)
			continue;*/

		if (pt->m_iCarrierTypeID != -1 && pt->m_iCarrierTypeID != _pEvent->m_oEventExt.m_iCarrierTypeID)
			continue;
		if (pt->m_iBusinessKey != -1 && pt->m_iBusinessKey != _pEvent->m_iBusinessKey)
			continue;
        if (pt->m_iCallTypeID !=-1 && pt->m_iCallTypeID != _pEvent->m_iCallTypeID)
            continue;
		
		*_ppRule = pt;
		return true;
	}
		
	return false;
}
#else
bool LongEventTypeRuleMgrEx::getRule(EventSection const *_pEvent, 
		LongEventTypeRuleEx** _ppRule)
{
	if (!_pEvent)
		return false;
	
	LongEventTypeRuleEx *pt = 0;
    LongEventTypeRuleEx *pRuleHead = 0;
    pRuleHead = (LongEventTypeRuleEx *)G_PPARAMINFO->m_poLongEventTypeRuleExList;

	
	//查找当前事件类型对应的链表
	for(pt = pRuleHead + 1; pt != pRuleHead; pt = pRuleHead + pt->m_iNextEventOffset) 
    {
		if(pt->m_iSourceEventType == _pEvent->m_oEventExt.m_iSourceEventType)
			break;
	}
	if(pt == pRuleHead)
		return false;
	
	//查找当前事件类型链表中对应的规则
	for( ; pt != pRuleHead; pt = pRuleHead + pt->m_iNextRuleOffset)
    {
		
		if (pt->m_iInEventTypeID != -1 && pt->m_iInEventTypeID != _pEvent->m_iEventTypeID)
			continue;
			
		if (pt->m_iLongTypeID != -1 && pt->m_iLongTypeID != _pEvent->m_oEventExt.m_iLongTypeID)
			continue;		
		if (pt->m_iRoamTypeID != -1 && pt->m_iRoamTypeID != _pEvent->m_oEventExt.m_iRoamTypeID)
			continue;
        if(_pEvent->m_iCallTypeID== CALLTYPE_MTC)
        {
            if (pt->m_iPartyID != -1 && pt->m_iPartyID != _pEvent->m_iCallingSPTypeID)
			continue;
        }
        else
        {
            if (pt->m_iPartyID != -1 && pt->m_iPartyID != _pEvent->m_iCalledSPTypeID)
			continue;
        }

        if (pt->m_iPartyRuleID != -1 && pt->m_iPartyRuleID != _pEvent->m_iParterID)
			continue;

		if (pt->m_iCarrierTypeID != -1 && pt->m_iCarrierTypeID != _pEvent->m_oEventExt.m_iCarrierTypeID)
			continue;
		if (pt->m_iBusinessKey != -1 && pt->m_iBusinessKey != _pEvent->m_iBusinessKey)
			continue;
        if (pt->m_iCallTypeID !=-1 && pt->m_iCallTypeID != _pEvent->m_iCallTypeID)
            continue;
		
		*_ppRule = pt;
		return true;
	}
		
	return false;
}
#endif

//##ModelId=4281844B00FD
int LongEventTypeRuleEx::getRuleID() const
{
	return m_iRuleID;
}

//##ModelId=4281846300D0
int LongEventTypeRuleEx::getOutEventTypeID() const
{
	return m_iOutEventTypeID;
}

//##ModelId=4281904801FB
#ifdef PRIVATE_MEMORY
LongEventTypeRuleEx::~LongEventTypeRuleEx()
{
	cout<<"\n -- del LongEventRuleID: "<< m_iRuleID <<endl;
	if(m_poNextRule) delete m_poNextRule;
	if(m_poNextEvent) delete m_poNextEvent;
	m_poNextRule = NULL;
	m_poNextEvent = NULL;
}
#else
LongEventTypeRuleEx::~LongEventTypeRuleEx()
{
}
#endif

//##ModelId=42819E7E006B
#ifdef PRIVATE_MEMORY
LongEventTypeRuleEx::LongEventTypeRuleEx()
{
	m_poNextEvent = NULL;
	m_poNextRule = NULL;
}
#else
LongEventTypeRuleEx::LongEventTypeRuleEx()
{
}
#endif

