#ifndef   ACCTTIEMLOAD_H_HEADER_INCLUDED
#define   ACCTTIEMLOAD_H_HEADER_INCLUDED

#include "CommonMacro.h"
#include "HashList.h"
#include "AcctItemMgr.h"
#include "SHMData.h"
#define SHM_ACCTITEM_LOAD					IpcKeyMgr::getIpcKey (-1,"SHM_ACCTITEM_LOAD")

class AcctItemLoad
{
public:
    AcctItemLoad();
    ~AcctItemLoad();

    void loadAcctItem();

    void loadGroupMemberB();

    void loadBasicGroupMeberB();

    void analyzeItem();

    int getItemChild(int iAcctItemID,int * pItemChild);

    void loadGroup();

    int getGroupDetail(long lGroupID);

    void loadGroupB();


    AcctItem             *m_poAcctItemList;
    HashList<AcctItem *> *m_poIndex;
    HashList<int>        *m_poItemSerial;
    
    int m_iCount;
    
    ItemGroupMember      *m_poItemParentList;
    HashList<int>        *m_poItemParentIndex;

    ItemGroupMemberB     *m_poGroupMemberBList;
    HashList<int>        *m_poGroupMemberBIndex;

    ItemGroupMemberB     *m_poBasicGroupMemberBList;
    HashList<int>        *m_poBasicGroupMemberBIndex;

    AcctItemChild        *m_poItemChildList;
    HashList<int>        *m_poItemChildIndex;
    int m_iIndex;

    ItemGroupMember      *m_poGroupList;                //
    HashList<int>        *m_poGroupSerial;
    //SimpleMatrix       *m_poMatrix;

    ItemGroupMember      *m_poGroupListB;
    HashList<int>        *m_poGroupSerialB;            //计费自己的账目组
    //SimpleMatrix       *m_poMatrixB;                 //计费自己的账目组
    ItemGroupMember      *m_poGroupBasicMemberListB;
    HashList<int>        *m_poGroupBasicSerialB;
    ItemGroupMember      *m_poGroupDisctMemberListB;
    HashList<int>        *m_poGroupDisctSerialB;

    HashList<int> * m_poItemTemp;
    
    HashList<int> * m_poYes;
    HashList<int> * m_poNo;
    HashList<int> * m_poChildYes;
    HashList<int> * m_poChildNo;

    int m_aoAcctItemID[MAX_ACCTITEM_PERGROUP];

    bool m_bUploaded;
	SHMData<ItemGroupMember> *m_poItemLoad;
    
};




#endif    //ACCTTIEMLOAD_H_HEADER_INCLUDED

