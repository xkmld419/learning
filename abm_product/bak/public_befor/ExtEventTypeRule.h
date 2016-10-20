/*VER: 1*/ 
#ifndef __EXT_EVENT_TYPE__RULE_
#define __EXT_EVENT_TYPE__RULE_

#include "LocalEventTypeRule.h"
#include "KeyTree.h"

#define NBR_HEAD__LEN 31

class ExtEventTypeRule {
  public:
    int  m_iSourceEventType;
    int  m_iInEventType;
    char m_sCallingHead[NBR_HEAD__LEN];
    char m_sCalledHead[NBR_HEAD__LEN];
    int  m_iCallingOrgID;
    int  m_iCalledOrgID;
    int  m_iSwitchID;
    int  m_iBusinessKey;
    int  m_iOutEventType;
    
    int  m_iRuleID;
};

class Node {
  public:
    int  m_iSwitchID;
    int  m_iBusinessKey;
    int  m_iOutEventType;
    int  m_iRuleID;
    Node * m_pNext;
    
  public:
    Node () {m_iSwitchID = -1; m_iBusinessKey=-1; m_iOutEventType=-1; m_pNext=0;}
    
    void insertNode (ExtEventTypeRule *pRule) 
    {
        if (m_iSwitchID == -1) {
            m_iSwitchID = pRule->m_iSwitchID;
            m_iBusinessKey = pRule->m_iBusinessKey;
            m_iOutEventType = pRule->m_iOutEventType;
            m_iRuleID = pRule->m_iRuleID;
            return;
        }
        if (!m_pNext) {
            m_pNext = new Node();
            m_pNext->insertNode (pRule);
            return;
        }
        m_pNext->insertNode (pRule);
    }

    Node * search (StdEvent const *_pEvent)
    {
        if ( (m_iSwitchID==0 || m_iSwitchID==_pEvent->m_iSwitchID) &&
             (m_iBusinessKey==0 || m_iBusinessKey==_pEvent->m_iBusinessKey)
        ) {
            return (Node *)(this);
        }
        if (!m_pNext) return 0;
        return m_pNext->search (_pEvent);
    }
};

class CalledOrgNode {
  public:
    int  m_iCalledOrgID;
    Node *m_pNode;
    CalledOrgNode *m_pNext;
    
  public:
    CalledOrgNode() {m_iCalledOrgID=-1; m_pNode=0; m_pNext=0;}
    
    //如果新产生CalledOrgNode节点,就返回新节点地址, 否则返回0
    CalledOrgNode * insertCalledOrg (ExtEventTypeRule *pRule) 
    {
        if (m_iCalledOrgID == -1) {//新节点首次赋值
            m_iCalledOrgID = pRule->m_iCalledOrgID;
            m_pNode = new Node();
            m_pNode->insertNode (pRule);
            return 0;
        }
        if (m_iCalledOrgID == pRule->m_iCalledOrgID) {
            if (!m_pNode) THROW(123);
            m_pNode->insertNode (pRule);
            return 0;
        }
        if (!m_pNext) {
            m_pNext = new CalledOrgNode(); //新产生节点
            m_pNext->insertCalledOrg (pRule);
            return m_pNext;
        }
        return m_pNext->insertCalledOrg (pRule);
    }

    Node * search (StdEvent const *_pEvent)
    {
        //if (!m_pNode)  THROW (456);
        return m_pNode->search (_pEvent);
    }
};

class CallingOrgNode {
  public:
    int  m_iCallingOrgID;
    CalledOrgNode *m_pCalledOrgNode;
    KeyTree <CalledOrgNode *> *m_pCalledOrgTree;
    CallingOrgNode *m_pNext;
    
    OrgMgr m_oOrgMgr;
    
  public:
    CallingOrgNode() {m_iCallingOrgID=-1; m_pCalledOrgNode=0; m_pNext=0; m_pCalledOrgTree=0;}
    
    CallingOrgNode * insertCallingOrg (ExtEventTypeRule *pRule) 
    {
        OrgPath *pPath = OrgPathMgr::getOrgPath (pRule->m_iCalledOrgID);
        if (!pPath) THROW (123);
        if (m_iCallingOrgID == -1) {
            m_iCallingOrgID = pRule->m_iCallingOrgID;
            m_pCalledOrgNode = new CalledOrgNode();
            m_pCalledOrgTree = new KeyTree<CalledOrgNode *>;
            m_pCalledOrgNode->insertCalledOrg (pRule);
            m_pCalledOrgTree->add (pPath->m_sOrgPath, m_pCalledOrgNode);
            return 0;
        }
        if (m_iCallingOrgID == pRule->m_iCallingOrgID) {
            if (!m_pCalledOrgNode) THROW (123);
            if (CalledOrgNode *p = m_pCalledOrgNode->insertCalledOrg (pRule))
                m_pCalledOrgTree->add (pPath->m_sOrgPath, p);
            return 0;
        }
        if (!m_pNext) {
            m_pNext = new CallingOrgNode();
            m_pNext->insertCallingOrg (pRule);
            return m_pNext;
        }
        return m_pNext->insertCallingOrg (pRule);
    }
    
    Node * search (StdEvent const *_pEvent);
};

class CalledHeadNode {
  public:
    char m_sCalledHead[NBR_HEAD__LEN];
    CallingOrgNode *m_pCallingOrgNode;
    KeyTree <CallingOrgNode *> *m_pCallingOrgTree;
    CalledHeadNode *m_pNext;
    
    OrgMgr m_oOrgMgr;
    
  public:
    CalledHeadNode() {m_pCallingOrgNode =0; m_pNext=0; m_pCallingOrgTree=0;}
    
    CalledHeadNode * insertCalledHead (ExtEventTypeRule *pRule) 
    {
        OrgPath *pPath = OrgPathMgr::getOrgPath (pRule->m_iCallingOrgID);
        if (!pPath) THROW (123);
        if (!m_pCallingOrgNode) {
            strcpy (m_sCalledHead, pRule->m_sCalledHead);
            m_pCallingOrgNode = new CallingOrgNode ();
            m_pCallingOrgTree = new KeyTree<CallingOrgNode *>;
            m_pCallingOrgNode->insertCallingOrg (pRule);
            m_pCallingOrgTree->add (pPath->m_sOrgPath, m_pCallingOrgNode);
            return 0;
        }
        if (!strcmp(m_sCalledHead, pRule->m_sCalledHead)) {
            if (!m_pCallingOrgNode) THROW (123);
            if (CallingOrgNode *p = m_pCallingOrgNode->insertCallingOrg(pRule))
                m_pCallingOrgTree->add (pPath->m_sOrgPath, p);
            return 0;
        }
        if (!m_pNext) {
            m_pNext = new CalledHeadNode();
            m_pNext->insertCalledHead (pRule);
            return m_pNext;
        }
        return m_pNext->insertCalledHead (pRule);
    }

    Node * search (StdEvent const *_pEvent);
};

class CallingHeadNode {
  public:
    char m_sCallingHead[NBR_HEAD__LEN];
    CalledHeadNode *m_pCalledHeadNode;
    KeyTree <CalledHeadNode *> *m_pCalledHeadTree;
    CallingHeadNode *m_pNext;
    
  public:
    CallingHeadNode() {m_pCalledHeadNode=0; m_pNext=0; m_pCalledHeadTree=0;}
    
    CallingHeadNode * insertCallingHead (ExtEventTypeRule *pRule) 
    {
        if (!m_pCalledHeadNode) {
            strcpy (m_sCallingHead, pRule->m_sCallingHead);
            m_pCalledHeadNode = new CalledHeadNode();
            m_pCalledHeadTree = new KeyTree<CalledHeadNode *>;
            m_pCalledHeadNode->insertCalledHead (pRule);
            m_pCalledHeadTree->add (pRule->m_sCalledHead, m_pCalledHeadNode);
            return 0;
        }
        if (!strcmp(m_sCallingHead, pRule->m_sCallingHead)) {
            if (!m_pCalledHeadNode) THROW (123);
            if (CalledHeadNode *p = m_pCalledHeadNode->insertCalledHead(pRule))
                m_pCalledHeadTree->add (pRule->m_sCalledHead, p);
            return 0;
        }
        if (!m_pNext) {
            m_pNext = new CallingHeadNode();
            m_pNext->insertCallingHead (pRule);
            return m_pNext;
        }
        return m_pNext->insertCallingHead (pRule);
    }
    
    Node * search (StdEvent const *_pEvent);
};

class InEventNode {
  public:
    int  m_iInEventType;
    CallingHeadNode *m_pCallingHeadNode;
    KeyTree <CallingHeadNode *> *m_pCallingHeadTree;
    InEventNode *m_pNext;
    
  public:
    InEventNode() {m_iInEventType = -1; m_pCallingHeadNode=0; m_pNext=0; m_pCallingHeadTree=0;}
    
    InEventNode * insertInEvent (ExtEventTypeRule *pRule) 
    {
        if (!m_pCallingHeadNode) {
            m_iInEventType = pRule->m_iInEventType;
            m_pCallingHeadNode = new CallingHeadNode();
            m_pCallingHeadTree = new KeyTree<CallingHeadNode *>;
            m_pCallingHeadNode->insertCallingHead (pRule);
            m_pCallingHeadTree->add (pRule->m_sCallingHead, m_pCallingHeadNode);
            return 0;
        }
        if (m_iInEventType == pRule->m_iInEventType) {
            if (!m_pCallingHeadNode) THROW (123);
            if (CallingHeadNode *p = m_pCallingHeadNode->insertCallingHead(pRule))
                m_pCallingHeadTree->add (pRule->m_sCallingHead, p);
            return 0;
        }
        if (!m_pNext) {
            m_pNext = new InEventNode();
            m_pNext->insertInEvent (pRule);
            return m_pNext;
        }
        return m_pNext->insertInEvent (pRule);
    }

    Node * search (StdEvent const *_pEvent);
};

class SourceEventNode {
  public:
    int  m_iSourceEventType;
    InEventNode *m_pInEventNode;
    SourceEventNode *m_pNext;
    
  public:
    SourceEventNode() {m_iSourceEventType=0; m_pInEventNode=0; m_pNext=0;}
    
    void insertSourceEvent (ExtEventTypeRule *pRule) 
    {
        if (!m_pInEventNode) {
            m_iSourceEventType = pRule->m_iSourceEventType;
            m_pInEventNode = new InEventNode();
            m_pInEventNode->insertInEvent (pRule);
            return;
        }
        if (m_iSourceEventType == pRule->m_iSourceEventType) {
            if (!m_pInEventNode) THROW (123);
            m_pInEventNode->insertInEvent (pRule);
            return;
        }
        if (!m_pNext) {
            m_pNext = new SourceEventNode();
            m_pNext->insertSourceEvent (pRule);
            return;
        }
        m_pNext->insertSourceEvent (pRule);
    }
    
    Node * search (StdEvent const *_pEvent)
    {
        if (_pEvent->m_oEventExt.m_iSourceEventType == m_iSourceEventType) {
            //if (!m_pInEventNode) THROW (234);
            return m_pInEventNode->search (_pEvent);
        }
        if (!m_pNext) return 0;
        return m_pNext->search (_pEvent);
    }
};


class ExtEventTypeRuleMgr {

  public:
    static void formatExtEventType (EventSection *_poEventSection);
    
    static bool getExtEventTypeRule (StdEvent const *_pEvent, Node ** _ppRuleNode);
    
    static void init();
    
  public:
    
    static SourceEventNode *m_pHead;
    static bool bInit;
    
    static int m_iFirstOrgID;
};



#endif
