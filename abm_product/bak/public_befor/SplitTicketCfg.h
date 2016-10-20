/*VER: 1*/ 
#ifndef _SPLIT_TICKET_CFG_H_
#define _SPLIT_TICKET_CFG_H_
#include "Environment.h"
#include "TOCIQuery.h"
#include "CommonMacro.h"
#include "StdEvent.h"
#include <map>
typedef struct _STD_EVENT_PLUS_
{
    StdEvent m_oStdEvent;
    int nOperation;
}StdEventPlus;

using namespace std;

#ifndef PRIVATE_MEMORY
class SplitAcctItemGroup
{
public:
    int m_iAcctItemType;
    int m_iSplitGroupID;
    int m_iEventTypeID;
    unsigned int m_iNextEventOffset;
    unsigned int m_iNextAcctItemOffset;
};

class SplitAcctGroupEvent
{
public:
    int m_iSplitGroupID;
    int m_iEventTypeID;
    int m_iOperationID;
};
#endif

class SplitTicketCfg
{
public:
    static SplitTicketCfg* GetSplitTicketCfg();
    ~SplitTicketCfg(void);
public:
    //初始化拆分帐目类型配置
    bool Init();

    //拆分话单
    int SplitTicket(StdEvent * pStdEvent);

    int GetSplitCount(void);

    bool GetSplitTicket(StdEvent* pStdEvent,int nPos);

protected:
    SplitTicketCfg(void);
    static SplitTicketCfg* m_gpSplitTicketCfg;
private:
    StdEventPlus m_oStdEventPlus[MAX_CHARGE_NUM];

    EventTypeMgr* m_pEventTypeMgr;
    int m_nPos[MAX_CHARGE_NUM]; 
    inline int GetAcctItemGroupID(int nEventTypeID,int nAcctItemTypeID);
    inline int GetDesEventTypeID(int nSplitGroupID,int& nOperationID);

    map< int,map<int, int> > m_EventTypeIDMap;
    map<int,pair<int,int> >m_GroupIDMapEventType;
    map<int,int> m_GroupIDMapStdEvent;
    map<int,int>::iterator m_it;
    map<int,int>::iterator m_itEnd;
    map<int,pair<int,int> >::iterator m_GroupIDMapIt;
    map<int,int> m_GroupIDMapPos;
    int m_nCount;
};

#endif
