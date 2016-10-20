/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// 
// 
// 
// All rights reserved.

#include "LongEventTypeRule.h"
#include "EventSection.h"
#include <iostream>
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif 
using namespace std;

//##ModelId=42817EC803D9
LongEventTypeRule *LongEventTypeRuleMgr::m_poRule = NULL;

//##ModelId=4281AE010170
bool LongEventTypeRuleMgr::m_bInit = false;

//##ModelId=428180AA0011
#ifdef PRIVATE_MEMORY
LongEventTypeRuleMgr::LongEventTypeRuleMgr()
{
	loadRule();
}
#else
LongEventTypeRuleMgr::LongEventTypeRuleMgr()
{
}
#endif

//##ModelId=428180DC0366
#ifdef PRIVATE_MEMORY
LongEventTypeRuleMgr::~LongEventTypeRuleMgr()
{
	unloadRule();
}
#else
LongEventTypeRuleMgr::~LongEventTypeRuleMgr()
{
}
#endif

//##ModelId=428180E701F9
void LongEventTypeRuleMgr::loadRule()
{
	unloadRule();
	
	m_bInit = true;
	
	LongEventTypeRule **ppNextEvent = &m_poRule;
	LongEventTypeRule **ppNextRule = &m_poRule;
	LongEventTypeRule *pt = NULL;
	
	DEFINE_QUERY(qry);
	
	qry.close();
	qry.setSQL(" select decode(in_event_type_id,null,1,0) s0, "
		" decode(long_type_id,Null,1,0) s1, decode(roam_type_id,null,1,0) s2, "
		" decode(calling_sp_type_id,null,1,0) s3, decode(called_sp_type_id,null,1,0) s4, "
		" decode(calling_service_type_id,null,1,0) s5, decode(called_service_type_id,null,1,0) s6, "
		" decode(carrier_type_id,null,1,0) s7,decode(business_key,null,1,0) s8, "
		" rule_id, source_event_type, "
		" nvl(in_event_type_id,-1) in_event_type_id , "
		" nvl(long_type_id,-1) long_type_id, "
		" nvl(roam_type_id,-1) roam_type_id, "
		" nvl(calling_sp_type_id,-1) calling_sp_type_id, "
		" nvl(called_sp_type_id,-1) called_sp_type_id, "
		" nvl(calling_service_type_id,-1) calling_service_type_id, "
		" nvl(called_service_type_id,-1) called_service_type_id, "
		" nvl(carrier_type_id,-1) carrier_type_id, "
		" nvl(business_key,-1) business_key, out_event_type_id "
		" from b_long_event_type_rule "
		" order by source_event_type, s0,s1,s2,s3,s4,s5,s6,s7,s8 "
	);
	qry.open();
	
	int iLastSourceEventType = -1;
	
	while (qry.next()) {
		
		pt = new LongEventTypeRule;
		if (!pt)
			THROW(MBC_LongEvtRule+1);
		
		pt->m_iRuleID = qry.field("rule_id").asInteger();
		pt->m_iSourceEventType = qry.field("source_event_type").asInteger();
		pt->m_iInEventTypeID = qry.field("in_event_type_id").asInteger();
		pt->m_iLongTypeID = qry.field("long_type_id").asInteger();
		pt->m_iRoamTypeID = qry.field("roam_type_id").asInteger();
		pt->m_iCallingSPTypeID = qry.field("calling_sp_type_id").asInteger();
		pt->m_iCalledSPTypeID = qry.field("called_sp_type_id").asInteger();
		pt->m_iCallingServiceTypeID = qry.field("calling_service_type_id").asInteger();
		pt->m_iCalledServiceTypeID = qry.field("called_service_type_id").asInteger();
		pt->m_iCarrierTypeID = qry.field("carrier_type_id").asInteger();
		pt->m_iBusinessKey = qry.field("business_key").asInteger();
		pt->m_iOutEventTypeID = qry.field("out_event_type_id").asInteger();
		
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
void LongEventTypeRuleMgr::unloadRule()
{
	if (!m_bInit)
		return;
	
	delete m_poRule;
	m_poRule = NULL;
	m_bInit = false;
}

//##ModelId=428180F601D3
#ifdef PRIVATE_MEMORY
bool LongEventTypeRuleMgr::getRule(EventSection const *_pEvent, 
		LongEventTypeRule** _ppRule)
{
	if (!_pEvent)
		return false;
	
	LongEventTypeRule *pt;
	
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
		if (pt->m_iCallingSPTypeID != -1 && pt->m_iCallingSPTypeID != _pEvent->m_iCallingSPTypeID)
			continue;
		if (pt->m_iCalledSPTypeID != -1 && pt->m_iCalledSPTypeID != _pEvent->m_iCalledSPTypeID)
			continue;
		if (pt->m_iCallingServiceTypeID != -1 && pt->m_iCallingServiceTypeID != _pEvent->m_iCallingServiceTypeID)
			continue;
		if (pt->m_iCalledServiceTypeID != -1 && pt->m_iCalledServiceTypeID != _pEvent->m_iCalledServiceTypeID)
			continue;
		if (pt->m_iCarrierTypeID != -1 && pt->m_iCarrierTypeID != _pEvent->m_oEventExt.m_iCarrierTypeID)
			continue;
		if (pt->m_iBusinessKey != -1 && pt->m_iBusinessKey != _pEvent->m_iBusinessKey)
			continue;
		
		*_ppRule = pt;
		return true;
	}
		
	return false;
}
#else
bool LongEventTypeRuleMgr::getRule(EventSection const *_pEvent, 
		LongEventTypeRule** _ppRule)
{
	if (!_pEvent)
		return false;
	
	LongEventTypeRule *pt = 0;
    LongEventTypeRule *pRuleHead = 0;
    pRuleHead = (LongEventTypeRule *)G_PPARAMINFO->m_poLongEventTypeRuleList;

	
	//查找当前事件类型对应的链表
    for(pt = pRuleHead +1; pt != pRuleHead; pt = pRuleHead + pt->m_iNextEventOffset)
    {
        if (pt->m_iSourceEventType == _pEvent->m_oEventExt.m_iSourceEventType)
			break;
    }
	if (pt == pRuleHead) //没有找到查找当前事件类型对应的链表
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
		if (pt->m_iCallingSPTypeID != -1 && pt->m_iCallingSPTypeID != _pEvent->m_iCallingSPTypeID)
			continue;
		if (pt->m_iCalledSPTypeID != -1 && pt->m_iCalledSPTypeID != _pEvent->m_iCalledSPTypeID)
			continue;
		if (pt->m_iCallingServiceTypeID != -1 && pt->m_iCallingServiceTypeID != _pEvent->m_iCallingServiceTypeID)
			continue;
		if (pt->m_iCalledServiceTypeID != -1 && pt->m_iCalledServiceTypeID != _pEvent->m_iCalledServiceTypeID)
			continue;
		if (pt->m_iCarrierTypeID != -1 && pt->m_iCarrierTypeID != _pEvent->m_oEventExt.m_iCarrierTypeID)
			continue;
		if (pt->m_iBusinessKey != -1 && pt->m_iBusinessKey != _pEvent->m_iBusinessKey)
			continue;
		
		*_ppRule = pt;
		return true;
	}
		
	return false;
}
#endif


//##ModelId=4281844B00FD
int LongEventTypeRule::getRuleID() const
{
	return m_iRuleID;
}

//##ModelId=4281846300D0
int LongEventTypeRule::getOutEventTypeID() const
{
	return m_iOutEventTypeID;
}

//##ModelId=4281904801FB
#ifdef PRIVATE_MEMORY
LongEventTypeRule::~LongEventTypeRule()
{
	cout<<"\n -- del LongEventRuleID: "<< m_iRuleID <<endl;
	delete m_poNextRule;
	delete m_poNextEvent;
	m_poNextRule = NULL;
	m_poNextEvent = NULL;
}
#else
LongEventTypeRule::~LongEventTypeRule()
{
}
#endif

//##ModelId=42819E7E006B
#ifdef PRIVATE_MEMORY
LongEventTypeRule::LongEventTypeRule()
{
	m_poNextEvent = NULL;
	m_poNextRule = NULL;
}
#else
LongEventTypeRule::LongEventTypeRule()
{
}
#endif

