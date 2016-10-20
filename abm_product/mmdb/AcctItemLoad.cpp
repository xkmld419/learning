#ifndef PRIVATE_MEMORY
#include "AcctItemLoad.h"
#include "SimpleMatrix.h"
#include "Environment.h"
#include "Exception.h"
#include <string.h>
#include <stdio.h>
#include <vector>

#include "MBC.h"
#include "Log.h"

AcctItemLoad::AcctItemLoad()
{
	m_poItemLoad = 0;
    loadAcctItem();
    loadGroupMemberB();
    loadBasicGroupMeberB();
    analyzeItem();
    loadGroup();
    loadGroupB();
    
    m_bUploaded = true;
    
}

AcctItemLoad::~AcctItemLoad()
{
    
    if(m_poAcctItemList)
        delete m_poAcctItemList;
    if(m_poIndex)
        delete m_poIndex;
    if(m_poItemSerial)
        delete m_poItemSerial;
    if(m_poItemParentList)
        delete m_poItemParentList;
    if(m_poItemParentIndex)
        delete m_poItemParentIndex;
    if(m_poGroupMemberBList)
        delete m_poGroupMemberBList;
    if(m_poGroupMemberBIndex)
        delete m_poGroupMemberBIndex;
    if(m_poBasicGroupMemberBList)
        delete m_poBasicGroupMemberBList;
    if(m_poBasicGroupMemberBIndex)
        delete m_poBasicGroupMemberBIndex;
    if(m_poItemChildList)
        delete m_poItemChildList;
    if(m_poItemChildIndex)
        delete m_poItemChildIndex;
    if(m_poGroupList)
        delete m_poGroupList;
    if(m_poGroupSerial)
        delete m_poGroupSerial;
    //if(m_poGroupListB)
    //    delete m_poGroupListB;
    if(m_poGroupSerialB)
        delete m_poGroupSerialB;
    //if(m_poGroupBasicMemberListB)
    //    delete m_poGroupBasicMemberListB;
    if(m_poGroupBasicSerialB)
        delete m_poGroupBasicSerialB;
    //if(m_poGroupDisctMemberListB)
    //    delete m_poGroupDisctMemberListB;
    if(m_poGroupDisctSerialB)
        delete m_poGroupDisctSerialB;
    if(m_poItemTemp)
        delete m_poItemTemp;
    
    m_poAcctItemList = 0;
    m_poIndex = 0;
    m_poItemSerial = 0;

    m_poItemParentList = 0;
    m_poItemParentIndex = 0;

    m_poGroupMemberBList = 0;
    m_poGroupMemberBIndex = 0;

    m_poBasicGroupMemberBList = 0;
    m_poBasicGroupMemberBIndex = 0;

    m_poItemChildList = 0;
    m_poItemChildIndex = 0;

    m_poGroupList = 0;
    m_poGroupSerial = 0;

    m_poGroupListB = 0;
    m_poGroupSerialB = 0;
    m_poGroupBasicMemberListB = 0;
    m_poGroupBasicSerialB = 0;
    m_poGroupDisctMemberListB = 0;
    m_poGroupDisctSerialB = 0;

    m_poItemTemp = 0;
    if(m_poItemLoad && m_poItemLoad->exist())
	{
		m_poItemLoad->remove();
	}
}

void AcctItemLoad::loadAcctItem()
{
    int count;
    AcctItem *pAcctItem;
    ItemGroupMember *pMemberParent;
    //char * s;
    char sTemp[16];

    int i = 0,j = 0;
    int iVal;

    TOCIQuery qry(Environment::getDBConn());
    
    qry.setSQL ("SELECT COUNT(*) FROM ACCT_ITEM_TYPE");
    qry.open ();
    qry.next ();

    count = qry.field(0).asInteger () + 16;

    qry.close ();

    m_iCount = 0;
    m_poItemSerial = new HashList<int> ((count>>1) | 1);
    m_poIndex = new HashList<AcctItem *> ((count>>1) | 1);
    m_poItemParentIndex = new HashList<int>(count-15);
    if(!m_poIndex || !m_poItemSerial || !m_poItemParentIndex)
        THROW(MBC_AcctItemMgr+1);

    m_poAcctItemList = new AcctItem[count];
    m_poItemParentList =  new ItemGroupMember[count-15];
    if(!m_poAcctItemList||!m_poItemParentList)
        THROW(MBC_AcctItemMgr+1);

    
    qry.setSQL("SELECT ACCT_ITEM_TYPE_ID, NVL(SUMMARY_ACCT_ITEM_TYPE_ID, -1), ACCT_ITEM_TYPE_CODE, "
            " NVL(PARTY_ROLE_ID,0) PARTY_ROLE_ID FROM ACCT_ITEM_TYPE ORDER BY ACCT_ITEM_TYPE_CODE");
    qry.open();

    while(qry.next ()) 
    {
        if(i == count)
            THROW(MBC_AcctItemMgr+2);

        pAcctItem = m_poAcctItemList + i;

        pAcctItem->m_iAcctItemID = qry.field(0).asInteger ();
        pAcctItem->m_iParentID = qry.field(1).asInteger ();
        pAcctItem->m_iOffset = i;               //自身偏移量
        strcpy(pAcctItem->m_sAcctItemCode, qry.field(2).asString());
        pAcctItem->m_iPartyRoleID = qry.field(3).asInteger();
        
        //s = pAcctItem->m_sAcctItemCode;
        
        if(pAcctItem->m_iParentID != -1)  //如果该结点的父结点id不为-1,存在父结点
        {
            iVal = 0;
            pMemberParent = m_poItemParentList + j;  //j从0开始,总是指示到链表中未使用的偏移量
            pMemberParent->m_iItemTypeID = pAcctItem->m_iAcctItemID;
            pMemberParent->m_poNext = 0;
            if(m_poItemParentIndex->get(pAcctItem->m_iParentID,&iVal))
            {             
                pMemberParent->m_poNext = &m_poItemParentList[iVal];
            }
            m_poItemParentIndex->add(pAcctItem->m_iParentID,j);  //直接覆盖原来的索引
            j++;  //j++
        }
        
        m_poIndex->add(pAcctItem->m_iAcctItemID, pAcctItem);  //m_iAcctItemID是主键,没有重复的
        m_poItemSerial->add(pAcctItem->m_iAcctItemID, i);

        i++;  //i++
    }

    qry.close ();
    
    count = i;     //m_iAcctItemID的数量

    m_iCount = i;  //m_iAcctItemID的数量

    for(i=0; i<count; i++) 
    {
        pAcctItem = m_poAcctItemList + i;

        if(pAcctItem->m_iParentID != -1) 
        {
            m_poIndex->get(pAcctItem->m_iParentID, &(pAcctItem->m_poParent));
        }

        strncpy(sTemp, pAcctItem->m_sAcctItemCode, sizeof(sTemp));
        if(sTemp[0] == 'A' || sTemp[0] == 'B')
        {
            int iMin = 0;
            int iMax = m_iCount-1;
            int iMid;
            int iTemp;
            
            sTemp[0] = (sTemp[0] == 'A') ? 'B' : 'A';

            while (iMin <= iMax)
            {
                iMid = (iMin + iMax) / 2;
                iTemp = strcmp(m_poAcctItemList[iMid].m_sAcctItemCode, sTemp);
                if (!iTemp) {
                    pAcctItem->m_poOther = m_poAcctItemList + iMid;
                    break;
                } else if (iTemp > 0) {
                    iMax = iMid - 1;
                } else {
                    iMin = iMid + 1;
                }
            }
        }
    }
}


void AcctItemLoad::loadGroupMemberB()
{
    int count;
    ItemGroupMemberB * p;
    ItemGroupMemberB * pTemp = 0;
    long lTemp = 0,lLastTemp = -1;
    int i = 0;
    char strFlag[3];
    TOCIQuery qry (Environment::getDBConn ());
    
    qry.setSQL ("select count(*) from b_acct_item_group_member");
    qry.open ();
    qry.next ();

    count = qry.field(0).asInteger () + 1;

    qry.close ();

    m_poGroupMemberBIndex = new HashList<int>(count);
    if(!m_poGroupMemberBIndex)
        THROW(MBC_AcctItemMgr+1);

    m_poGroupMemberBList = new ItemGroupMemberB[count];
    if(!m_poGroupMemberBList)
        THROW(MBC_AcctItemMgr+1);

    
    qry.setSQL ("select nvl(item_group_id,0) item_group_id, acct_item_object_type,"
        "nvl(acct_item_object_id,0) acct_item_object_id,nvl(exclude_flag, 0) exclude_flag "
            "from b_acct_item_group_member order by item_group_id");
    qry.open ();

    while (qry.next ()) 
    {
        if(i == count)
            THROW(MBC_AcctItemMgr+2);

        p = m_poGroupMemberBList + i;

        lTemp = qry.field(0).asLong();           //item_group_id
        strcpy(strFlag,qry.field(1).asString()); //acct_item_object_type
        
        p->m_iObjectType = 0;
        switch(strFlag[0]|0x20)
        {
            case 'a':
                p->m_iObjectType = 1;
                break;
            case 'b':
                p->m_iObjectType = 2;
                break;
        }
        p->m_iItemObjectID = qry.field(2).asInteger ();  //acct_item_object_id
        p->m_iExcludeFlag = qry.field(3).asInteger();    //exclude_flag
        p->m_poNext = 0;
        if(lTemp == lLastTemp){
            p->m_poNext=pTemp;
        }
        
        m_poGroupMemberBIndex->add(lTemp,i);    //索引 存放 组id到具体的结构索引
        
        i++;
        lLastTemp = lTemp;
        pTemp = p;
    }
    qry.close ();
    return;
}


void AcctItemLoad::loadBasicGroupMeberB()
{
    int count;
    ItemGroupMemberB * p;
    ItemGroupMemberB * pTemp = 0;
    long lTemp = 0,lLastTemp = -1;
    int i = 0;

    TOCIQuery qry (Environment::getDBConn ());
    
    qry.setSQL ("select count(*) from b_basic_acct_item_group_member");
    qry.open ();
    qry.next ();

    count = qry.field(0).asInteger () + 1;

    qry.close ();

    m_poBasicGroupMemberBIndex = new HashList<int>(count);
    if(!m_poBasicGroupMemberBIndex) 
        THROW(MBC_AcctItemMgr+1);

    m_poBasicGroupMemberBList = new ItemGroupMemberB[count];
    if(!m_poBasicGroupMemberBList) 
        THROW(MBC_AcctItemMgr+1);

    qry.setSQL("select nvl(item_group_id,0) item_group_id, nvl(acct_item_type_id,0) acct_item_type_id, "
            " nvl(exclude_flag, 0) exclude_flag "
            " from b_basic_acct_item_group_member order by item_group_id");
    qry.open();

    while(qry.next())
    {
        if(i == count) 
            THROW(MBC_AcctItemMgr+2);

        p = m_poBasicGroupMemberBList + i;

        lTemp = qry.field(0).asLong();

        p->m_iItemObjectID = qry.field(1).asInteger ();
        p->m_iExcludeFlag = qry.field(2).asInteger();
        p->m_poNext = 0;
        if(lTemp == lLastTemp){
            p->m_poNext=pTemp;
        }
        
        m_poBasicGroupMemberBIndex->add(lTemp,i); //索引 存放 组id到具体的结构索引
        
        i++;
        lLastTemp = lTemp;
        pTemp = p;
    }
    qry.close ();
    return;
}


void AcctItemLoad::analyzeItem()
{
    AcctItemChild * pItemChild;
    AcctItem * pAcctItem;
    int i = 0;

    m_poItemChildIndex = new HashList<int>(m_iCount+1);
    if(!m_poItemChildIndex ) 
        THROW(MBC_AcctItemMgr+1);

    m_poItemChildList = new AcctItemChild[m_iCount+1];
    if (!m_poItemChildList)
        THROW(MBC_AcctItemMgr+1);

    memset(m_poItemChildList,0,sizeof(AcctItemChild)*(m_iCount+1));
    
    for(i=0; i<m_iCount; i++) //对于每一条acct_item_type
    {
        pItemChild = m_poItemChildList + i;  //指到还没有处理的AcctItemChild链表
        pAcctItem = m_poAcctItemList + i;    //指到loaditem处理过的AcctItem链表
        m_iIndex = 0;
        pItemChild->m_iAcctItemID = pAcctItem->m_iAcctItemID;  //赋值每个m_iAcctItemID
        //计算每个m_iAcctItemID的孩子结点的个数,以及孩子结点的m_iAcctItemID,存放在m_aoChildItemID数组中
        pItemChild->m_iChildNum = getItemChild(pAcctItem->m_iAcctItemID,pItemChild->m_aoChildItemID);

        //保存m_iAcctItemID到m_poItemChildList的偏移量
        m_poItemChildIndex->add(pAcctItem->m_iAcctItemID,i);
    }
}


int AcctItemLoad::getItemChild(int iAcctItemID,int * pItemChild)
{
    int iTemp = 0;
    //iAcctItemID其实可以认为是组id
    //先从m_poItemParentIndex去取iAcctItemID在m_poItemParentList中的偏移量
    //如果取的到,则递归取他的孩子结点的孩子结点
    if(m_poItemParentIndex->get(iAcctItemID,&iTemp))
    {
        getItemChild(m_poItemParentList[iTemp].m_iItemTypeID, pItemChild);
        
        ItemGroupMember * p = m_poItemParentList[iTemp].m_poNext;
        while(p)
        {
            getItemChild(p->m_iItemTypeID,pItemChild);
            p = p->m_poNext;
        }
    }
    else
    {
        *(pItemChild+m_iIndex) = iAcctItemID;     //保存iAcctItemID到参数中
        m_iIndex++;                               //pItemChild的偏移量+1
    }
    return m_iIndex;
}


void AcctItemLoad::loadGroup()
{
    
    int iCount, iCountGroup;
    ItemGroupMember *pItemGroupMember = 0;
    int curGroup = 0;
    char sql[2048];

    TOCIQuery qry(Environment::getDBConn());

    strcpy (sql, "SELECT COUNT(ACCT_ITEM_TYPE_ID) COUNT1,"
            "COUNT(DISTINCT ITEM_GROUP_ID) COUNT2 "
            "FROM ACCT_ITEM_GROUP_MEMBER");
    qry.setSQL (sql);
    qry.open ();
    qry.next ();
    iCount = qry.field(0).asInteger ();  //ACCT_ITEM_TYPE_ID数
    iCountGroup = qry.field(1).asInteger ();  //ITEM_GROUP_ID数
    qry.close ();

    m_poGroupList =  new ItemGroupMember[iCount];
    m_poGroupSerial = new HashList<int>((iCountGroup>>1) | 1);
    //m_poMatrix = new SimpleMatrix (m_iCount, iCountGroup, false);

    if(!m_poGroupList || !m_poGroupSerial)
        THROW(MBC_AcctItemMgr+1);

    sprintf (sql, "SELECT ACCT_ITEM_TYPE_ID, ITEM_GROUP_ID "
            " FROM ACCT_ITEM_GROUP_MEMBER "
            " WHERE STATE='10A' "
            " ORDER BY ITEM_GROUP_ID, ACCT_ITEM_TYPE_ID");
    
    qry.setSQL(sql);
    qry.open();

    int i, j, k;
    i = 0;
    while (qry.next ())
    {
        pItemGroupMember = m_poGroupList + i;
        pItemGroupMember->m_iItemTypeID = qry.field(0).asInteger ();
        pItemGroupMember->m_iItemGroupID = qry.field(1).asInteger();
        pItemGroupMember->m_poNext = 0;
        pItemGroupMember->m_iNextOffset = -1;
         
        if (!(m_poItemSerial->get(pItemGroupMember->m_iItemTypeID, &j))) 
        {
            //这里正是运行的时候应该throw，因为现在acct_item_type有两套表
            //THROW (MBC_AcctItemMgr+11);
            continue;
        }

        if(m_poGroupSerial->get(pItemGroupMember->m_iItemGroupID, &k))
        {
            pItemGroupMember->m_poNext = &m_poGroupList[k];
            pItemGroupMember->m_iNextOffset = k;
        }
        m_poGroupSerial->add(pItemGroupMember->m_iItemGroupID, i);

        /*
        if(!(m_poGroupSerial->get(qry.field(1).asInteger(), &k)))  //找到该组的第一个成员 l是偏移量
        {
            m_poGroupSerial->add(qry.field(1).asInteger(), i);
        }
        else                                        //找到该组的其他成员
        {
            pItemGroupMember->m_poNext = (m_poGroupList + k)->m_poNext;
            pItemGroupMember->m_iNextOffset = (m_poGroupList + k)->m_iNextOffset;
            (m_poGroupList + k)->m_poNext = pItemGroupMember;
            (m_poGroupList + k)->m_iNextOffset = i;
        }
        */

        i++;
    }
    
    qry.close ();

}



int AcctItemLoad::getGroupDetail(long lGroupID)
{
    vector<int> vYes;
    vector<int> vNo;
    vector<int> vChildYes;
    vector<int> vChildNo;

    ItemGroupMemberB *p;
    ItemGroupMemberB *pOther;
    AcctItemChild    *pChild;

    int iVal = 0;
    int iNum = 0;
    int m = 1;

    memset(m_aoAcctItemID, 0, sizeof(m_aoAcctItemID));
    m_poYes->clearAll();
    m_poNo->clearAll();
    m_poChildYes->clearAll();
    m_poChildNo->clearAll();

    //优惠节点包含和不包含
    if(m_poGroupMemberBIndex->get(lGroupID,&iVal))
    {
        p = m_poGroupMemberBList+iVal;
        while(p)
        {
            if(p->m_iObjectType == 1)
            {
                if(m_poItemChildIndex->get(p->m_iItemObjectID,&iVal))
                {
                    pChild = m_poItemChildList+iVal;
                    if(p->m_iExcludeFlag==0)
                    {
                        for(int i=0;i<pChild->m_iChildNum;i++)
                        {
                            if(m_poYes->get(pChild->m_aoChildItemID[i],&m))
                            {
                                continue;
                            }
                            else
                            {
                                vYes.push_back(pChild->m_aoChildItemID[i]);
                                m_poYes->add(pChild->m_aoChildItemID[i],m);
                            }
                        }
                    }
                    else
                    {
                        for(int i=0;i<pChild->m_iChildNum;i++)
                        {
                            if(m_poNo->get(pChild->m_aoChildItemID[i],&m))
                            {
                                continue;
                            }
                            else
                            {
                                vNo.push_back(pChild->m_aoChildItemID[i]);
                                m_poNo->add(pChild->m_aoChildItemID[i],m);
                            }
                        }
                    }
                }
            }
            p = p->m_poNext;
        }
    }

    //基本节点包含和不包含
    if(m_poGroupMemberBIndex->get(lGroupID,&iVal)){
        p = m_poGroupMemberBList+iVal;
        while(p){
            if(p->m_iObjectType == 2){
                if(m_poBasicGroupMemberBIndex->get(p->m_iItemObjectID,&iVal)){
                    pOther = m_poBasicGroupMemberBList + iVal;
                    while(pOther){
                        if(m_poItemChildIndex->get(pOther->m_iItemObjectID,&iVal)){
                            pChild = m_poItemChildList+iVal;
                            if(pOther->m_iExcludeFlag == 0){
                                for (int i=0;i<pChild->m_iChildNum;i++){
                                    if(m_poChildYes->get(pChild->m_aoChildItemID[i],&m)){
                                        continue;
                                    }else{
                                        vChildYes.push_back(pChild->m_aoChildItemID[i]);
                                        m_poChildYes->add(pChild->m_aoChildItemID[i],m);
                                    }
                                }
                            }else{
                                for (int i=0;i<pChild->m_iChildNum;i++){
                                    if(m_poChildNo->get(pChild->m_aoChildItemID[i],&m)){
                                        continue;
                                    }else{
                                        vChildNo.push_back(pChild->m_aoChildItemID[i]);
                                        m_poChildNo->add(pChild->m_aoChildItemID[i],m);
                                    }
                                }
                            }
                        }
                        pOther = pOther->m_poNext;
                    }
                }
            }
            vector<int>::iterator iterChildYes=vChildYes.begin();
            vector<int>::iterator iterChildNo=vChildNo.begin();
            bool bChildNo = false;
            iNum = 0 ;
            while(iterChildYes!=vChildYes.end()){
                while(iterChildNo!=vChildNo.end()){
                    if(*iterChildYes == *iterChildNo){
                        bChildNo = true;
                        break;
                    }
                    iterChildNo++;
                }
                if(!bChildNo){
                    m_aoAcctItemID[iNum] = *iterChildYes;
                    iNum++;
                    if(iNum == MAX_ACCTITEM_PERGROUP){
                        Log::log(0,"ITEM_GROUP_ID[%d] > MAX_ACCTITEM_PERGROUP,RESTART APP PLEASE!",lGroupID);
                        THROW (MBC_AcctItemMgr + 3);
                    }
                }
                iterChildYes++;
                iterChildNo=vChildNo.begin();
                bChildNo = false;
            }
            if(p->m_iExcludeFlag ==0){
                for(int i=0;i<iNum;i++){
                    if(m_poYes->get(m_aoAcctItemID[i],&m)){
                        continue;
                    }else{
                        vYes.push_back(m_aoAcctItemID[i]);
                        m_poYes->add(m_aoAcctItemID[i],m);
                    }
                }
            }else{
                for(int i=0;i<iNum;i++){
                    if(m_poNo->get(m_aoAcctItemID[i],&m)){
                        continue;
                    }else{
                        vNo.push_back(m_aoAcctItemID[i]);
                        m_poNo->add(m_aoAcctItemID[i],m);
                    }
                }
            }
            vChildYes.clear();
            vChildNo.clear();
            m_poChildYes->clearAll();
            m_poChildNo->clearAll();
            p = p->m_poNext;
        }
    }

    
    vector<int>::iterator iterYes=vYes.begin();
    vector<int>::iterator iterNo=vNo.begin();
    bool bNo = false;
    iNum = 0 ;
    while(iterYes!=vYes.end())
    {
        while(iterNo!=vNo.end())
        {
            if(*iterNo == *iterYes)
            {
                bNo = true;
                break;
            }
            iterNo++;
        }
        if(!bNo)
        {
            m_aoAcctItemID[iNum] = *iterYes;
            iNum++;
            if(iNum == MAX_ACCTITEM_PERGROUP){
                Log::log(0,"ITEM_GROUP_ID[%d] > MAX_ACCTITEM_PERGROUP,RESTART APP PLEASE!",lGroupID);
                THROW (MBC_AcctItemMgr + 3);
            }
        }
        iterYes++;
        iterNo=vNo.begin();
        bNo = false;
    }
    
    return iNum;


}


void AcctItemLoad::loadGroupB()
{
    m_poItemLoad = new SHMData<ItemGroupMember>(SHM_ACCTITEM_LOAD);
    
    int i, j, k, l, m, n;
    m = 1;
    
    int iNum = 0;
    char sql[512];
    
    int iGroupCount;
    int iGroupID;
    int iAcctItemID;
    int iBasicIdx;
    int iDisctIdx;
    ItemGroupMember *pItemGroupMemberB = 0;
    ItemGroupMember *pItemBasicGroupMember = 0;
    ItemGroupMember *pItemDisctGroupMember = 0;

    TOCIQuery qry(Environment::getDBConn());

    strcpy (sql, "select count(distinct item_group_id) "
                 " from b_acct_item_group_member");
    qry.setSQL (sql);
    qry.open (); 
    qry.next ();

    iGroupCount = qry.field(0).asInteger();
    qry.close ();
	if(!m_poItemLoad->exist())
	{
		m_poItemLoad->create(iGroupCount*MAX_ACCTITEM_PERGROUP*3+3);
	}
	ItemGroupMember *m_poItemLoadList = (ItemGroupMember*)(*m_poItemLoad);
    //m_poGroupListB =  new ItemGroupMember[iGroupCount*MAX_ACCTITEM_PERGROUP];  //同一组的组成一个链表,不同组的放在一个数组
	m_poGroupListB =  m_poItemLoadList;
    m_poGroupSerialB = new HashList<int>((iGroupCount>>1) | 1);      //索引item_group_id

    //m_poGroupBasicMemberListB = new ItemGroupMember[iGroupCount*MAX_ACCTITEM_PERGROUP];
	m_poGroupBasicMemberListB = m_poItemLoadList + (iGroupCount*MAX_ACCTITEM_PERGROUP);
    //m_poGroupDisctMemberListB = new ItemGroupMember[iGroupCount*MAX_ACCTITEM_PERGROUP];
	m_poGroupDisctMemberListB = m_poItemLoadList + (iGroupCount*MAX_ACCTITEM_PERGROUP*2);
    m_poGroupBasicSerialB = new HashList<int>((iGroupCount>>1) | 1);
    m_poGroupDisctSerialB = new HashList<int>((iGroupCount>>1) | 1);

    m_poItemTemp =  new HashList<int>(MAX_ACCTITEM_PERGROUP);    //临时变量

    m_poYes =  new HashList<int>(MAX_ACCTITEM_PERGROUP);
    m_poNo =  new HashList<int>(MAX_ACCTITEM_PERGROUP);
    m_poChildYes =  new HashList<int>(MAX_ACCTITEM_PERGROUP);
    m_poChildNo =  new HashList<int>(MAX_ACCTITEM_PERGROUP);
    
    if (!m_poGroupSerialB || !m_poGroupBasicSerialB || !m_poGroupDisctSerialB
            || !m_poGroupListB || !m_poGroupBasicMemberListB || !m_poGroupDisctMemberListB ||!m_poItemTemp) 
    {
        THROW (MBC_AcctItemMgr + 1);
    }

    if(!m_poYes || !m_poNo || !m_poChildYes || !m_poChildNo){
        THROW (MBC_AcctItemMgr + 1);
    }

    strcpy (sql, "select distinct s.item_group_id,nvl(s.include_discount_item, 1) from b_acct_item_group s"
        " where exists (select 1 from b_acct_item_group_member t where t.item_group_id=s.item_group_id) ");
    qry.setSQL (sql);
    qry.open ();

    i = 0;
    iBasicIdx = 0;
    iDisctIdx = 0;
    while (qry.next ())   //对于每一个组获得组id后
    {

        //j = qry.field(0).asInteger ();                   //item_group_id 不重复
        iGroupID = qry.field(0).asInteger();

        //存放每个item_group的include_discount_item的值
        //也就是每个item_group在数组中对应一个序号,对应的include_discount_item的值
        //m_piIncludeDiscount[l] = qry.field(1).asInteger ();  //include_discount_item
        
        iNum = getGroupDetail(iGroupID);  //通过item_group_id查找该组含有的具体账目项个数
        
        m_poItemTemp->clearAll();             //临时变量,每次循环每个组id先清空
        
        for(int s=0;s<iNum;s++)               //
        {
            //AcctItemID数组,getGroupDetail把所有属于该组id的账目项全放进该数组
            iAcctItemID = m_aoAcctItemID[s];
            
            //通过AcctItemID去找,看能否找到该AcctItemID对应信息
            if (!(m_poItemSerial->get(iAcctItemID, &k)))  //k为偏移量 判断是否含有b时使用
            {         
                //THROW (MBC_AcctItemMgr+11);
                continue;         //找不到就退出循环,找下一个AcctItemID
            }
            
            //dup process
            //这里首先m_poItemTemp会是空的
            //通过AcctItemID找,找到则不对该AcctItemID进行处理
            //找不到则将AcctItemID加入该索引中
            //主要目的是为了去重
            if(m_poItemTemp->get(iAcctItemID,&m)){  //通过AcctItemID去找,找到退出循环
                continue;                 //如果找到,说明是重复的,以前处理过了,后面就不处理了
            }else{
                m_poItemTemp->add(iAcctItemID,m);   //找不到就将其AcctItemID加入,后面接着做处理
            }

            pItemGroupMemberB = m_poGroupListB + i;
            
            pItemGroupMemberB->m_iItemTypeID = iAcctItemID;
            pItemGroupMemberB->m_iItemGroupID = iGroupID;
            pItemGroupMemberB->m_poNext = 0;
            pItemGroupMemberB->m_iNextOffset = -1;

            if(m_poGroupSerialB->get(iGroupID, &l))
            {
                pItemGroupMemberB->m_poNext = &m_poGroupListB[l];
                pItemGroupMemberB->m_iNextOffset = l;
            }
            m_poGroupSerialB->add(iGroupID, i);

            /*
            if(!(m_poGroupSerialB->get(iGroupID, &l)))  //找到该组的第一个成员 l是偏移量
            {
                m_poGroupSerialB->add(iGroupID, i);
            }
            else                                        //找到该组的其他成员
            {
                pItemGroupMemberB->m_poNext = (m_poGroupListB + l)->m_poNext;
                pItemGroupMemberB->m_iNextOffset = (m_poGroupListB + l)->m_iNextOffset;
                (m_poGroupListB + l)->m_poNext = pItemGroupMemberB;
                (m_poGroupListB + l)->m_iNextOffset = i;
            }
            */
            i++;       //i++

            if((m_poAcctItemList[k].m_sAcctItemCode[0] | 0x20)=='b')
            {
                pItemDisctGroupMember = m_poGroupDisctMemberListB+ iDisctIdx;
                pItemDisctGroupMember->m_iItemTypeID = iAcctItemID;
                pItemDisctGroupMember->m_iItemGroupID = iGroupID;
                pItemDisctGroupMember->m_poNext = 0;
                pItemDisctGroupMember->m_iNextOffset = -1;
                if(m_poGroupDisctSerialB->get(iGroupID, &n))
                {
                    pItemDisctGroupMember->m_poNext = &m_poGroupDisctMemberListB[n];
                    pItemDisctGroupMember->m_iNextOffset = n;
                }
                m_poGroupDisctSerialB->add(iGroupID, iDisctIdx);
                /*
                if(!(m_poGroupDisctSerialB->get(iGroupID, &n)))
                {
                    m_poGroupDisctSerialB->add(iGroupID, iDisctIdx);
                }
                else
                {
                    pItemDisctGroupMember->m_poNext = (m_poGroupDisctMemberListB + n)->m_poNext;
                    pItemDisctGroupMember->m_iNextOffset = (m_poGroupDisctMemberListB + n)->m_iNextOffset;
                    (m_poGroupDisctMemberListB + n)->m_poNext = pItemDisctGroupMember;
                    (m_poGroupDisctMemberListB + n)->m_iNextOffset = iDisctIdx;
                }*/
                iDisctIdx++;
            }
            else
            {
                pItemBasicGroupMember = m_poGroupBasicMemberListB + iBasicIdx;
                pItemBasicGroupMember->m_iItemTypeID = iAcctItemID;
                pItemBasicGroupMember->m_iItemGroupID = iGroupID;
                pItemBasicGroupMember->m_poNext = 0;
                pItemBasicGroupMember->m_iNextOffset = -1;
                if(m_poGroupBasicSerialB->get(iGroupID, &n))
                {
                    pItemBasicGroupMember->m_poNext = &m_poGroupBasicMemberListB[n];
                    pItemBasicGroupMember->m_iNextOffset = n;
                }
                m_poGroupBasicSerialB->add(iGroupID, iBasicIdx);
                /*
                if(!(m_poGroupBasicSerialB->get(iGroupID, &n)))
                {
                    m_poGroupBasicSerialB->add(iGroupID, iBasicIdx);
                }
                else
                {
                    pItemBasicGroupMember->m_poNext = (m_poGroupBasicMemberListB + n)->m_poNext;
                    pItemBasicGroupMember->m_iNextOffset = (m_poGroupBasicMemberListB + n)->m_iNextOffset;
                    (m_poGroupBasicMemberListB + n)->m_poNext = pItemBasicGroupMember;
                    (m_poGroupBasicMemberListB + n)->m_iNextOffset = iBasicIdx;
                }*/
                iBasicIdx++;
            }
            
        }
        
    }
    qry.close ();
    qry.commit();

    if(m_poYes){
        delete m_poYes;
        m_poYes = 0;
    }
    if(m_poNo){
        delete m_poNo;
        m_poNo = 0;
    }
    if(m_poChildYes){
        delete m_poChildYes;
        m_poChildYes = 0;
    }
    if(m_poChildNo){
        delete m_poChildNo;
        m_poChildNo = 0;
    }
}

#endif




