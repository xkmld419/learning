/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// 
// 
// 
// All rights reserved.

#include "LocalEventTypeRule.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif


//##ModelId=4281BC4603CB
int LocalEventTypeRule::getRuleID() const
{
    return m_iRuleID;
}

//##ModelId=4281BC5C014C
int LocalEventTypeRule::getOutEventTypeID() const
{
    return m_iOutEventTypeID;
}

//##ModelId=4281BC890237
LocalEventTypeRule::~LocalEventTypeRule()
{
#ifdef PRIVATE_MEMORY
    cout<<"\n -- del LocalEventRuleID: "<< m_iRuleID <<endl;
    delete m_poNextRule;
    delete m_poNextSourceEvent;
    m_poNextRule = NULL;
    m_poNextSourceEvent = NULL;
#endif
}

//
void LocalEventTypeRule::addTree(KeyTree <LocalEventTypeRule *> *pTree,
        int _OrgID, LocalEventTypeRule *pt)
{
    OrgPath *pOrgPath = OrgPathMgr::getOrgPath (_OrgID);
    
    if (!pOrgPath)
        THROW(MBC_LocalEvtRule+4);
    
    pTree->add (pOrgPath->m_sOrgPath, pt);
}



//##ModelId=4281BE2D021D
LocalEventTypeRuleMgr::LocalEventTypeRuleMgr()
{
#ifdef PRIVATE_MEMORY
    loadRule();
#endif
}

//##ModelId=4281BE3A02DA
LocalEventTypeRuleMgr::~LocalEventTypeRuleMgr()
{

#ifdef PRIVATE_MEMORY
    unloadRule();
#endif
    
    delete m_poOrgMgr;
    
    m_poOrgMgr = NULL;
}

//##ModelId=4281BE5201B2
void LocalEventTypeRuleMgr::loadRule()
{
    unloadRule();
    
    m_poOrgMgr = new OrgMgr;
    if (!m_poOrgMgr)
        THROW(MBC_LocalEvtRule+1);
    
    m_bInit = true;
    
    LocalEventTypeRule **ppNextEvent = &m_poRule;
    LocalEventTypeRule **ppNextRule = &m_poRule;
    LocalEventTypeRule *pt = NULL;
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL(    " select nvl(b.org_level_id,-1) level_1, nvl(c.org_level_id,-1) level_2, " 
        " decode(in_event_type_id,null,1,0) s0,"
        " decode(calling_sp_type_id,null,1,0) s1, decode(called_sp_type_id,null,1,0) s2, "
        " decode(calling_service_type_id,null,1,0) s3, decode(called_service_type_id,null,1,0) s4, "
        " decode(carrier_type_id,null,1,0) s5,decode(business_key,null,1,0) s6,"
        " rule_id, source_event_type,"
        " nvl(in_event_type_id,-1) in_event_type_id, "
        " nvl(calling_org_id,-1) calling_org_id, "
        " nvl(called_org_id,-1) called_org_id, "
        " nvl(calling_sp_type_id,-1) calling_sp_type_id, "
        " nvl(called_sp_type_id,-1) called_sp_type_id, "
        " nvl(calling_service_type_id,-1) calling_service_type_id, "
        " nvl(called_service_type_id,-1) called_service_type_id, "
        " nvl(carrier_type_id,-1) carrier_type_id, "
        " nvl(business_key,-1) business_key, out_event_type_id "
        " from b_local_event_type_rule a, org b, org c "
        " where a.calling_org_id = b.org_id(+) and a.called_org_id = c.org_id(+) "
        " order by source_event_type, s0,"
        "     level_1 desc, level_2 desc, calling_org_id, called_org_id,"
        "    s1,s2,s3,s4,s5,s6 "
    );
    qry.open();
    
    int iLastSourceEventType = -1;
    
    while (qry.next()) {
        
        pt = new LocalEventTypeRule;
        if (!pt)
            THROW(MBC_LocalEvtRule+2);
        
        pt->m_iRuleID = qry.field("rule_id").asInteger();
        pt->m_iSourceEventType = qry.field("source_event_type").asInteger();
        pt->m_iInEventTypeID = qry.field("in_event_type_id").asInteger();
        pt->m_iCallingOrgID = qry.field("calling_org_id").asInteger();
        pt->m_iCalledOrgID = qry.field("called_org_id").asInteger();
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
            ppNextEvent = &(pt->m_poNextSourceEvent);
        }
        else {
            (*ppNextRule) = pt;
        }
        ppNextRule = &(pt->m_poNextRule);
    }
    
    qry.close();
}


//##ModelId=4281BE86036F
#ifdef PRIVATE_MEMORY
bool LocalEventTypeRuleMgr::getRule(StdEvent const *_pEvent, 
        LocalEventTypeRule ** _ppRule)
{
    return get_Rule_V2(_pEvent, _ppRule);
    
    if (!_pEvent)
        return false;
    
    LocalEventTypeRule *pt;

    if (!m_bInit)
        loadRule();

    //查找当前源事件类型对应的链表
    for (pt = m_poRule; pt; pt = pt->m_poNextSourceEvent) {
        if (pt->m_iSourceEventType == _pEvent->m_oEventExt.m_iSourceEventType)
            break;
    }
    if (!pt) 
        return false;
    
    //查找当前事件类型链表中对应的规则
    for ( ; pt; pt = pt->m_poNextRule) {
        
        if (pt->m_iInEventTypeID != -1 && pt->m_iInEventTypeID != _pEvent->m_iEventTypeID)
            continue;
        
        if (pt->m_iCallingOrgID != -1 && 
                !m_poOrgMgr->getBelong(_pEvent->m_oEventExt.m_iCallingOrgID,pt->m_iCallingOrgID))
            continue;        
        if (pt->m_iCalledOrgID != -1 && 
                !m_poOrgMgr->getBelong(_pEvent->m_oEventExt.m_iCalledOrgID,pt->m_iCalledOrgID))
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
bool LocalEventTypeRuleMgr::getRule(StdEvent const *_pEvent, 
        LocalEventTypeRule ** _ppRule)
{

	if (!_pEvent)
   		return false;

	LocalEventTypeRule *pRule=NULL,*pRuleHead=NULL;
	OrgPath *pPath=NULL,*pPathHead=NULL;
	char sCallingOrgPath[50],sCalledOrgPath[50];

	pRuleHead =(LocalEventTypeRule *)G_PPARAMINFO->m_poLocalEventTypeRuleList;
	pPathHead =(OrgPath *)G_PPARAMINFO->m_poOrgPathList;
		
	char key[20];
	unsigned int index=0,index1=0;
	sprintf(key,"%d_%d",_pEvent->m_oEventExt.m_iSourceEventType,_pEvent->m_iEventTypeID);
	if(!G_PPARAMINFO->m_poLocalEventTypeRuleIndex->get(key,&index))
	{
		sprintf(key,"%d_%d",_pEvent->m_oEventExt.m_iSourceEventType,-1);
		if(!G_PPARAMINFO->m_poLocalEventTypeRuleIndex->get(key,&index))
			return false;
	}
	pRule = pRuleHead + index;
	
	for(;pRule!=pRuleHead;pRule=pRuleHead + pRule->m_iNextOffset)
	{
        
        if (pRule->m_iCallingOrgID != -1 )
        {
        	if(G_PPARAMINFO->m_poOrgPathIndex->get(_pEvent->m_oEventExt.m_iCallingOrgID,&index1))
			{	
				strcpy(sCallingOrgPath,pPathHead[index1].m_sOrgPath);
				if(strncmp(sCallingOrgPath,pRule->m_sCallingOrgPath,strlen(pRule->m_sCallingOrgPath))!=0)
					continue;
        	}
			else
				continue;
	
        }

        if (pRule->m_iCalledOrgID != -1 )
        {
        	if(G_PPARAMINFO->m_poOrgPathIndex->get(_pEvent->m_oEventExt.m_iCalledOrgID,&index1))
			{	
				strcpy(sCalledOrgPath,pPathHead[index1].m_sOrgPath);
				if(strncmp(sCalledOrgPath,pRule->m_sCalledOrgPath,strlen(pRule->m_sCalledOrgPath))!=0)
					continue;
        	}
			else
				continue;
	
        }
        
        if (pRule->m_iCallingSPTypeID != -1 && pRule->m_iCallingSPTypeID != _pEvent->m_iCallingSPTypeID)
            continue;
        if (pRule->m_iCalledSPTypeID != -1 && pRule->m_iCalledSPTypeID != _pEvent->m_iCalledSPTypeID)
            continue;
        if (pRule->m_iCallingServiceTypeID != -1 && pRule->m_iCallingServiceTypeID != _pEvent->m_iCallingServiceTypeID)
            continue;
        if (pRule->m_iCalledServiceTypeID != -1 && pRule->m_iCalledServiceTypeID != _pEvent->m_iCalledServiceTypeID)
            continue;
        if (pRule->m_iCarrierTypeID != -1 && pRule->m_iCarrierTypeID != _pEvent->m_oEventExt.m_iCarrierTypeID)
            continue;
        if (pRule->m_iBusinessKey != -1 && pRule->m_iBusinessKey != _pEvent->m_iBusinessKey)
            continue;
        
        *_ppRule = pRule;
        return true;
	}

	return false;

}
#endif

//##ModelId=4281BE730119
void LocalEventTypeRuleMgr::unloadRule()
{
    if (!m_bInit)
        return;
    
    delete m_poRule;
    m_poRule = NULL;
    m_bInit = false;
}


//##ModelId=4281BF760220
bool LocalEventTypeRuleMgr::m_bInit = false;

//##ModelId=4281BEFF01ED
LocalEventTypeRule *LocalEventTypeRuleMgr::m_poRule = NULL;

//##ModelId=4281C5E003C6
OrgMgr *LocalEventTypeRuleMgr::m_poOrgMgr = NULL;

int LocalEventTypeRuleMgr::m_iFirstOrgID = 0;

//-----------------2007.03.27 added by yks------------------

//## 形成5叉树结构(并且按主、被叫OrgID的"路径"建树)
void LocalEventTypeRuleMgr::load_Rule_V2()
{
    if (m_bInit) return;
    m_bInit = true;
    
    if (!m_poOrgMgr)  m_poOrgMgr = new OrgMgr;
    if (!m_poOrgMgr)  THROW(MBC_LocalEvtRule+1);
    
    //临时指针
    LocalEventTypeRule **ppNextSourceEvent = &m_poRule;
    LocalEventTypeRule **ppNextInEventType = &m_poRule;
    LocalEventTypeRule **ppNextCallingOrgID = &m_poRule;
    LocalEventTypeRule **ppNextCalledOrgID  = &m_poRule;
    LocalEventTypeRule **ppNextRule = &m_poRule;
    
    LocalEventTypeRule **pp = &m_poRule;
    
    LocalEventTypeRule *pt = NULL;
    
    DEFINE_QUERY(qry);
    
    qry.setSQL("select org_id from org where parent_org_id is null");
    qry.open();
    if (!qry.next())
          m_iFirstOrgID = 0;
    else  m_iFirstOrgID = qry.field(0).asInteger();
    qry.close();
    
    qry.setSQL(    " select nvl(b.org_level_id,-1) level_1, nvl(c.org_level_id,-1) level_2, " 
        " decode(in_event_type_id,null,1,0) s0,"
        " decode(calling_sp_type_id,null,1,0) s1, decode(called_sp_type_id,null,1,0) s2, "
        " decode(calling_service_type_id,null,1,0) s3, decode(called_service_type_id,null,1,0) s4, "
        " decode(carrier_type_id,null,1,0) s5,decode(business_key,null,1,0) s6,"
        " rule_id, source_event_type,"
        " nvl(in_event_type_id,-1) in_event_type_id, "
        " nvl(calling_org_id,:vCallingFirst) calling_org_id, "
        " nvl(called_org_id,:vCalledFirst) called_org_id, "
        " nvl(calling_sp_type_id,-1) calling_sp_type_id, "
        " nvl(called_sp_type_id,-1) called_sp_type_id, "
        " nvl(calling_service_type_id,-1) calling_service_type_id, "
        " nvl(called_service_type_id,-1) called_service_type_id, "
        " nvl(carrier_type_id,-1) carrier_type_id, "
        " nvl(business_key,-1) business_key, out_event_type_id "
        " from b_local_event_type_rule a, org b, org c "
        " where a.calling_org_id = b.org_id(+) and a.called_org_id = c.org_id(+) "
        " order by source_event_type, s0,"
        "     level_1 desc, calling_org_id, level_2 desc, called_org_id,"
        "    s1,s2,s3,s4,s5,s6 "
    );
    qry.setParameter("vCallingFirst",m_iFirstOrgID);
    qry.setParameter("vCalledFirst",m_iFirstOrgID);
    qry.open();
    
    int iLastSourceEventType = -2;
    int iLastInEventTypeID = -2;
    int iLastCallingOrgID = -2;
    int iLastCalledOrgID = -2;
    
    KeyTree <LocalEventTypeRule *> *pCurrentCallingTree = 0;
    KeyTree <LocalEventTypeRule *> *pCurrentCalledTree = 0;
    
    while (qry.next()) {
        
        pt = new LocalEventTypeRule;
        if (!pt)
            THROW(MBC_LocalEvtRule+2);
        
        pt->m_iRuleID = qry.field("rule_id").asInteger();
        pt->m_iSourceEventType = qry.field("source_event_type").asInteger();
        pt->m_iInEventTypeID = qry.field("in_event_type_id").asInteger();
        pt->m_iCallingOrgID = qry.field("calling_org_id").asInteger();
        pt->m_iCalledOrgID = qry.field("called_org_id").asInteger();
        pt->m_iCallingSPTypeID = qry.field("calling_sp_type_id").asInteger();
        pt->m_iCalledSPTypeID = qry.field("called_sp_type_id").asInteger();
        pt->m_iCallingServiceTypeID = qry.field("calling_service_type_id").asInteger();
        pt->m_iCalledServiceTypeID = qry.field("called_service_type_id").asInteger();
        pt->m_iCarrierTypeID = qry.field("carrier_type_id").asInteger();
        pt->m_iBusinessKey = qry.field("business_key").asInteger();
        pt->m_iOutEventTypeID = qry.field("out_event_type_id").asInteger();
        
        if (pt->m_iSourceEventType != iLastSourceEventType) { //##如果和当前源事件类型不同
            iLastSourceEventType = pt->m_iSourceEventType;
            (*ppNextSourceEvent) = pt;
            ppNextSourceEvent = &(pt->m_poNextSourceEvent);
            ppNextInEventType = &(pt->m_poNextInEventType);
            ppNextCallingOrgID = &(pt->m_poNextCallingOrgID);
            ppNextCalledOrgID  = &(pt->m_poNextCalledOrgID);
            ppNextRule = &(pt->m_poNextRule);
            iLastInEventTypeID = pt->m_iInEventTypeID; 
            iLastCallingOrgID = pt->m_iCallingOrgID; 
            iLastCalledOrgID = pt->m_iCalledOrgID; 
            
            //主被叫Org加入建树
            pt->m_pCallingTree = new KeyTree<LocalEventTypeRule *>;
            pCurrentCallingTree = pt->m_pCallingTree;
            pt->addTree (pCurrentCallingTree, pt->m_iCallingOrgID, pt);
            
            pt->m_pCalledTree = new KeyTree<LocalEventTypeRule *>;
            pCurrentCalledTree = pt->m_pCalledTree;
            pt->addTree (pCurrentCalledTree, pt->m_iCalledOrgID, pt);            
        }
        else {
            if (pt->m_iInEventTypeID != iLastInEventTypeID) { //和当前输入事件类型不同
                iLastInEventTypeID = pt->m_iInEventTypeID;
                (*ppNextInEventType) = pt;
                ppNextInEventType = &(pt->m_poNextInEventType);
                ppNextCallingOrgID = &(pt->m_poNextCallingOrgID);
                ppNextCalledOrgID  = &(pt->m_poNextCalledOrgID);
                ppNextRule = &(pt->m_poNextRule);
                
                //主被叫Org加入建树
                pt->m_pCallingTree = new KeyTree<LocalEventTypeRule *>;
                pCurrentCallingTree = pt->m_pCallingTree;
                pt->addTree (pCurrentCallingTree, pt->m_iCallingOrgID, pt);
                
                pt->m_pCalledTree = new KeyTree<LocalEventTypeRule *>;
                pCurrentCalledTree = pt->m_pCalledTree;
                pt->addTree (pCurrentCalledTree, pt->m_iCalledOrgID, pt);
                
                iLastCallingOrgID = pt->m_iCallingOrgID;
                iLastCalledOrgID = pt->m_iCalledOrgID;

            }
            else {
                if (pt->m_iCallingOrgID != iLastCallingOrgID) {//和当前的主叫OrgID不同
                    iLastCallingOrgID = pt->m_iCallingOrgID;
                    (*ppNextCallingOrgID) = pt;
                    ppNextCallingOrgID = &(pt->m_poNextCallingOrgID);
                    ppNextCalledOrgID  = &(pt->m_poNextCalledOrgID);
                    ppNextRule = &(pt->m_poNextRule);
                    
                    //当前输入事件类型下的新的主叫OrgID, 加入建树
                    pt->addTree (pCurrentCallingTree, pt->m_iCallingOrgID, pt);
                    
                    //同时被叫加入建树
                    pt->m_pCalledTree = new KeyTree<LocalEventTypeRule *>;
                    pCurrentCalledTree = pt->m_pCalledTree;
                    pt->addTree (pCurrentCalledTree, pt->m_iCalledOrgID, pt);
                    
                    iLastCalledOrgID = pt->m_iCalledOrgID;
                }
                else { 
                    if (pt->m_iCalledOrgID != iLastCalledOrgID) { //和当前被叫OrgID不同
                        iLastCalledOrgID = pt->m_iCalledOrgID;
                        (*ppNextCalledOrgID) = pt;
                        ppNextCalledOrgID = &(pt->m_poNextCalledOrgID);
                        ppNextRule = &(pt->m_poNextRule);
                        
                        //当前的主叫OrgID下的新的被叫OrgID,加入建树
                        pt->addTree (pCurrentCalledTree, pt->m_iCalledOrgID, pt);
                    }
                    else { //分支要素全部相同,直接链接
                        (*ppNextRule) = pt;
                        ppNextRule = &(pt->m_poNextRule);
                    }
                }
            }
        }
    }
    qry.close ();
    //m_poRule->print();
}

bool LocalEventTypeRuleMgr::get_Rule_V2(StdEvent const *_pEvent, 
        LocalEventTypeRule ** _ppRule)
{
    static bool bRet;
    
    if (!_pEvent)
        return false;
    
    LocalEventTypeRule *pSourceRule;
    
    if (!m_bInit)
        load_Rule_V2();

    //查找当前源事件类型对应的链表
    for (pSourceRule=m_poRule; pSourceRule; pSourceRule=pSourceRule->m_poNextSourceEvent) {
        if (pSourceRule->m_iSourceEventType == _pEvent->m_oEventExt.m_iSourceEventType)
            break;
    }
    if (!pSourceRule) 
        return false;
    
    //临时存放主、被叫OrgPath 
    char sCallingOrgPath[60], sCalledOrgPath[60];
    
    //尝试寻找一个合适的 输入事件类型 规则链
    for (LocalEventTypeRule *pInEvtRule=pSourceRule; pInEvtRule; 
            pInEvtRule=pInEvtRule->m_poNextInEventType)
    {   
        if (pInEvtRule->m_iInEventTypeID!= -1 && pInEvtRule->m_iInEventTypeID!=_pEvent->m_iEventTypeID)
            continue;
        
        int  iCallingOrgID = _pEvent->m_oEventExt.m_iCallingOrgID;
        if (!iCallingOrgID) iCallingOrgID = m_iFirstOrgID; //输入无主叫Org, 默认为根节点
            
        if (!getEventOrgPath (iCallingOrgID, sCallingOrgPath))
            return false;
            
        // 尝试寻找当前输入事件类型对应的 主叫ORG规则链 (按OrgLevel层次由深到潜循环)
        for ( ; ; ) {
            if (!pInEvtRule->m_pCallingTree) break;
            LocalEventTypeRule *pCallingOrgRule;
            bRet = pInEvtRule->m_pCallingTree->getMax (
                            sCallingOrgPath, &pCallingOrgRule, sCallingOrgPath);
            if (!bRet) { //##当前输入事件类型下找不到合适的主叫Org路径
                break;
            }
            
            //pCallingOrgRule 链上查找 被叫ORG
            int iCalledOrgID = _pEvent->m_oEventExt.m_iCalledOrgID;
            if (!iCalledOrgID) iCalledOrgID = m_iFirstOrgID; //输入无被叫Org, 默认为根节点
            if (!getEventOrgPath (iCalledOrgID, sCalledOrgPath)) {
                return false;
            }
            
            for (;;) {
                if (!pCallingOrgRule->m_pCalledTree) break;
                LocalEventTypeRule *pCalledOrgRule;
                bRet = pCallingOrgRule->m_pCalledTree->getMax (
                                sCalledOrgPath, &pCalledOrgRule, sCalledOrgPath);
                if (!bRet) {
                    break;
                }
                
                //pCalledOrgRule 链上查找适合的规则
                for (LocalEventTypeRule *pt=pCalledOrgRule; pt; pt=pt->m_poNextRule) 
                {
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
                //取上一层次被叫Org
                if (!OrgPathMgr::getParentOrgPath (sCalledOrgPath, sCalledOrgPath))
                    break;
            }
            //取上一层次主叫Org
            if (!OrgPathMgr::getParentOrgPath (sCallingOrgPath, sCallingOrgPath))
                break;
        }
    }
    return false; //not found.
}


bool LocalEventTypeRuleMgr:: getEventOrgPath (int iOrg, char *sPath)
{
    OrgPath *pOrgPath = OrgPathMgr::getOrgPath (iOrg);
    
    if (!pOrgPath) {
        cout<<"\ngetEventOrgPath err  OrgID="<<iOrg<<endl; 
        return false;
    }
    
    strcpy (sPath, pOrgPath->m_sOrgPath);
    
    return true;
}


void LocalEventTypeRule::print()
{
    cout<<"\n-------------------"<<endl;
    static int iLine=1;
    cout<<"L:"<<iLine++<<" SourceEvt:"<<m_iSourceEventType;
    cout<<" InEvent:"<<m_iInEventTypeID;
    cout<<" CallingOrg:"<<m_iCallingOrgID;
    cout<<" CalledOrg:"<<m_iCalledOrgID;
    
    if (m_poNextRule)
        m_poNextRule->print();
    
    if (m_poNextCalledOrgID)
        m_poNextCalledOrgID->print();
    
    if (m_poNextCallingOrgID)
        m_poNextCallingOrgID->print();
    if (m_poNextInEventType)
        m_poNextInEventType->print();
    if (m_poNextSourceEvent)
        m_poNextSourceEvent->print();
}


////////////////////////////////////////////////////////////////////

bool OrgPathMgr::m_bLoaded = false;

OrgMgr *OrgPathMgr::m_poOrgMgr = 0;

OrgPath * OrgPathMgr::m_poOrgPathList = 0;

KeyTree<OrgPath*> * OrgPathMgr::m_poOrgPathTree = 0;

void OrgPathMgr::init ()
{
    m_bLoaded = true;
    m_poOrgMgr = new OrgMgr();
    
    DEFINE_QUERY (qry);
    
    qry.setSQL ("select count(*) from org");
    qry.open ();
    qry.next ();
    int iCnt = qry.field(0).asInteger () + 16;
    qry.close ();
    
    if (m_poOrgPathList) delete m_poOrgPathList;
    m_poOrgPathList = new OrgPath[iCnt];
    if (m_poOrgPathTree) delete m_poOrgPathTree;
    m_poOrgPathTree = new KeyTree<OrgPath*>;
    
    qry.setSQL ("SELECT ORG_ID,rownum row_id from (select org_id FROM ORG order by org_level_id)");
    qry.open ();
    
    int iPos = 0;
    for (iPos = 0; qry.next() && iPos<iCnt; iPos++) {
        m_poOrgPathList[iPos].m_iOrgID = qry.field("ORG_ID").asInteger();
        m_poOrgPathList[iPos].m_iRowID = qry.field("row_id").asInteger();
        m_poOrgPathTree->add (m_poOrgPathList[iPos].m_iOrgID, &(m_poOrgPathList[iPos]));
    }
    qry.close();
    
    iCnt = iPos;
    for (iPos = 0; iPos<iCnt; iPos++) {
        char sTemp[60];
        int  iCurrentOrgID = m_poOrgPathList[iPos].m_iOrgID;
        sprintf (m_poOrgPathList[iPos].m_sOrgPath,"%d|", m_poOrgPathList[iPos].m_iRowID);
        
        for (;;) {
            int iParentID = m_poOrgMgr->getParentID (iCurrentOrgID);
            if (iParentID < 0)
                break;
            else
                iCurrentOrgID = iParentID;
            
            OrgPath *pOrgPath = 0;
            if (!m_poOrgPathTree->get (iCurrentOrgID,&pOrgPath)) 
                THROW(MBC_LocalEvtRule+3);
            
            sprintf (sTemp, "%d|%s", pOrgPath->m_iRowID , m_poOrgPathList[iPos].m_sOrgPath);
            strcpy (m_poOrgPathList[iPos].m_sOrgPath, sTemp);
        }
    }
}

#ifdef PRIVATE_MEMORY
OrgPath * OrgPathMgr::getOrgPath (int _iOrgID)
{
    if (!m_bLoaded)
        init ();
    
    OrgPath *pOrgPath = 0;
    
    if (!m_poOrgPathTree->get (_iOrgID,&pOrgPath))
        return 0;
    
    return pOrgPath;
}
#else
OrgPath * OrgPathMgr::getOrgPath (int _iOrgID)
{
	unsigned int indx1;
	
	OrgPath *pOrgPath = 0;

	pOrgPath = (OrgPath*)G_PPARAMINFO->m_poOrgPathList;
	if(G_PPARAMINFO->m_poOrgPathIndex->get(_iOrgID,&indx1))
		return pOrgPath + indx1;

    return NULL;
}
#endif


