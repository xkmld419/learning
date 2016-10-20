/*VER: 3*/ 
#include "AcctItemMgr.h"
#include "SimpleMatrix.h"
#include "Environment.h"
#include "Exception.h"
#include <string.h>
#include <stdio.h>
#include <vector>

#include "MBC.h"
#include "Log.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=4248E6400256
void AcctItemMgr::load()
{
	unload ();
	
	loadItem ();
	loadGroupMeberB();
	loadBasicGroupMeberB();
	analyzeItem();
	loadGroup ();
	loadGroupB ();
	m_bUploaded = true;
	//可以先释放一些不需要的空间，避免浪费
	if(m_poItemParentIndex){
		delete m_poItemParentIndex;
		m_poItemParentIndex = 0;
	}
	if(m_poItemParentList){
		delete [] m_poItemParentList;
		m_poItemParentList = 0;
	}
	if(m_poItemChildIndex){
		delete m_poItemChildIndex;
		m_poItemChildIndex = 0;
	}
	if(m_poItemChildList){
		delete [] m_poItemChildList;
		m_poItemChildList = 0;
	}
	if(m_poGroupMemberBIndex){
		delete m_poGroupMemberBIndex;
		m_poGroupMemberBIndex = 0;
	}
	if(m_poGroupMebmerBList){
		delete  [] m_poGroupMebmerBList;
		m_poGroupMebmerBList = 0;
	}
	if(m_poBasicGroupMemberBIndex){
		delete m_poBasicGroupMemberBIndex;
		m_poBasicGroupMemberBIndex = 0;
	}
	if(m_poBasicGroupMebmerBList){
		delete [] m_poBasicGroupMebmerBList;
		m_poBasicGroupMebmerBList = 0;
	}
	if(m_poItemTemp){
		delete m_poItemTemp;
		m_poItemTemp = 0;
	}	
}

//##ModelId=4248E64302A0
void AcctItemMgr::unload()
{
	m_bUploaded = true;

	if (m_poItemSerial)
		delete m_poItemSerial;

	if (m_poGroupSerial)
		delete m_poGroupSerial;

	if (m_poGroupSerialB)
		delete m_poGroupSerialB;

	if (m_poMatrix)
		delete m_poMatrix;

	if (m_poMatrixB)
		delete m_poMatrixB;

	if (m_poIndex)
		delete m_poIndex;

	if (m_poAcctItemList)
		delete [] m_poAcctItemList;


	m_poItemSerial = 0;
	m_poGroupSerial = 0;
	m_poGroupSerialB = 0;
	m_poMatrix = 0;
	m_poMatrixB = 0;
	m_poIndex = 0;
	m_poAcctItemList = 0;
	m_iCount = 0;
}

//##ModelId=4248E65203D8
#ifdef PRIVATE_MEMORY
bool AcctItemMgr::getBelongGroup(int acctitem, int acctitemgroup)
{
	int k, l;
	if (!(m_poItemSerial->get(acctitem, &k)) ||
		!(m_poGroupSerial->get(acctitemgroup, &l)) )
		return false;

	return m_poMatrix->getBelong (k, l);
}
#else
bool AcctItemMgr::getBelongGroup(int acctitem, int acctitemgroup)
{
	unsigned int iIdx = 0;
    ItemGroupMember *pItemMember = 0;
	char sKey[32] = {0};
	sprintf(sKey,"%d_%d",acctitemgroup,acctitem);
    if(!(G_PPARAMINFO->m_poAcctItemGroupStrIndex->get(sKey, &iIdx)))
    {
        return false;
    }
    else
    {
        /*pItemMember = G_PPARAMINFO->m_poAcctItemGroupList + iIdx;
        while(1)
        {
            if(pItemMember->m_iItemTypeID == acctitem)
                return true;
            else if(pItemMember->m_iNextOffset != 0)
                pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupList + pItemMember->m_iNextOffset;
            else
                return false;
        }*/
		return true;
    }
}
#endif

#ifdef PRIVATE_MEMORY
bool AcctItemMgr::getBelongGroupB(int acctitem, int bacctitemgroup, bool bDisct)
{
	int k, l;
	if (!(m_poItemSerial->get(acctitem, &k)) ||
		!(m_poGroupSerialB->get(bacctitemgroup, &l)) )
		return false;


    switch (m_piIncludeDiscount[l]) {
      case 0:   
        if (bDisct) return false;
        return m_poMatrixB->getBelong (k, l);

        break;

      case 1:
        return m_poMatrixB->getBelong (k, l);
        break;

      case 2: {
        if (bDisct) {
            AcctItem *p = m_poAcctItemList + k;

            p = p->m_poOther;

            if (!p) return false;
            k = p->m_iOffset;
        }

        return m_poMatrixB->getBelong (k, l);
        break;
      }

      default:
        break;
    }

    return false;
}
#else
bool AcctItemMgr::getBelongGroupB(int acctitem, int bacctitemgroup, bool bDisct)
{
	unsigned int iIdx = 0;
    unsigned int iIdx2 = 0;
    unsigned int iOffset = 0;
    ItemGroupMember *pItemMember = 0;
    AcctItem        *pAcctItem = 0;
    unsigned int iDiscount= -1;
    if(G_PPARAMINFO->m_poAcctItemIncludeDisctIndex->get(bacctitemgroup, &iDiscount))
    {
        switch(iDiscount)
        {
            case 0:
            {
                if(bDisct)
                    return false;
                else
                {
					char sKey[32] = {0};
					sprintf(sKey,"%d_%d",bacctitemgroup,acctitem);
                    if(!(G_PPARAMINFO->m_poAcctItemGroupBStrIndex->get(sKey, &iIdx)))
                    {
                        return false;
                    }
                    else
                    {
                        /*pItemMember = G_PPARAMINFO->m_poAcctItemGroupBList + iIdx;
                        while(1)
                        {
                            if(pItemMember->m_iItemTypeID == acctitem)
                                return true;
                            else if(pItemMember->m_iNextOffset != 0)
                                pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + pItemMember->m_iNextOffset;
                            else
                                return false;
                        }*/
						return true;
                    }
                }
            }
            case 1:
            {
				char sKey[32] = {0};
				sprintf(sKey,"%d_%d",bacctitemgroup,acctitem);
                if(!(G_PPARAMINFO->m_poAcctItemGroupBStrIndex->get(sKey, &iIdx)))
                {
                    return false;
                }
                else
                {
                    /*pItemMember = G_PPARAMINFO->m_poAcctItemGroupBList + iIdx;
                    while(1)
                    {
                        if(pItemMember->m_iItemTypeID == acctitem)
                            return true;
                        else if(pItemMember->m_iNextOffset != 0)
                            pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + pItemMember->m_iNextOffset;
                        else
                            return false;
                    }*/
					return true;
                }
            }
            case 2:
            {
                if(bDisct)
                {
                    if(!(G_PPARAMINFO->m_poAcctItemIndex->get(acctitem, &iIdx2)))
                    {
                        return false;
                    }
                    else
                    {
                        pAcctItem = G_PPARAMINFO->m_poAcctItemList + iIdx2;
                        if(pAcctItem->m_iOtherOffset == 0)
                            return false;
                        else
                            pAcctItem = G_PPARAMINFO->m_poAcctItemList + pAcctItem->m_iOtherOffset;

                        int iAcctItemID = pAcctItem->m_iAcctItemID;
						//
						char sKey[32] = {0};
						sprintf(sKey,"%d_%d",bacctitemgroup,iAcctItemID);
                        if(!(G_PPARAMINFO->m_poAcctItemGroupBStrIndex->get(sKey, &iIdx)))
                        {
                            return false;
                        }
                        else
                        {
                            /*pItemMember = G_PPARAMINFO->m_poAcctItemGroupBList + iIdx;
                            while(1)
                            {
                                if(pItemMember->m_iItemTypeID == iAcctItemID)
                                    return true;
                                else if(pItemMember->m_iNextOffset != 0)
                                    pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + pItemMember->m_iNextOffset;
                                else
                                    return false;
                            }*/
							return true;
                        }
                    }
                }
            }
            default:
                return false;
                
        }
    }
    else
        return false;
}
#endif

#ifdef PRIVATE_MEMORY
bool AcctItemMgr::getBelongGroupB(int acctitem, int bacctitemgroup)
{
	int k, l;
	if (!(m_poItemSerial->get(acctitem, &k)) ||
		!(m_poGroupSerialB->get(bacctitemgroup, &l)) )
		return false;

	return m_poMatrixB->getBelong (k, l);
}
#else
bool AcctItemMgr::getBelongGroupB(int acctitem, int bacctitemgroup)
{
    unsigned int iIdx = 0;
    ItemGroupMember *pItemMember = 0;
	//
	char sKey[32] = {0};
	sprintf(sKey,"%d_%d",bacctitemgroup,acctitem);
    if(!(G_PPARAMINFO->m_poAcctItemGroupBStrIndex->get(sKey, &iIdx)))
    {
        return false;
    }
    else
    {
        /*pItemMember = G_PPARAMINFO->m_poAcctItemGroupBList + iIdx;
        while(1)
        {
            if(pItemMember->m_iItemTypeID == acctitem)
                return true;
            else if(pItemMember->m_iNextOffset != 0)
                pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + pItemMember->m_iNextOffset;
            else
                return false;
        }*/
		return true;
    }
}
bool AcctItemMgr::getBelongGroupB(int acctitem, int bacctitemgroup, ParamInfoMgr* pParamInfoMgr)
{
    unsigned int iIdx = 0;
    ItemGroupMember *pItemMember = 0;
	//
	char sKey[32] = {0};
	sprintf(sKey,"%d_%d",bacctitemgroup,acctitem);
    if(!(pParamInfoMgr->m_poAcctItemGroupBStrIndex->get(sKey, &iIdx)))
    {
        return false;
    }
    else
    {
        /*pItemMember = pParamInfoMgr->m_poAcctItemGroupBList + iIdx;
        while(1)
        {
            if(pItemMember->m_iItemTypeID == acctitem)
                return true;
            else if(pItemMember->m_iNextOffset != 0)
                pItemMember = (ItemGroupMember *)pParamInfoMgr->m_poAcctItemGroupBList + pItemMember->m_iNextOffset;
            else
                return false;
        }*/
		return true;
    }
}
#endif

//##ModelId=4248E8FC0306
HashList<int> *AcctItemMgr::m_poItemSerial = 0;

//##ModelId=4248E92401D7
HashList<int> *AcctItemMgr::m_poGroupSerial = 0;

HashList<int> *AcctItemMgr::m_poGroupSerialB = 0;

//##ModelId=4248E8DF0160
SimpleMatrix *AcctItemMgr::m_poMatrix = 0;

SimpleMatrix *AcctItemMgr::m_poMatrixB = 0;

int AcctItemMgr::m_iCount = 0;

int *AcctItemMgr::m_piIncludeDiscount = 0;

ItemGroupMember **AcctItemMgr::m_pBasicMember = 0;
ItemGroupMember **AcctItemMgr::m_pDisctMember = 0;


//##ModelId=4248E9C003B2
#ifdef PRIVATE_MEMORY
AcctItemMgr::AcctItemMgr()
{
	if (!m_bUploaded)
		load();
}
#else
AcctItemMgr::AcctItemMgr()
{
}
#endif
AcctItemMgr::AcctItemMgr(int iMode)
{
}
//##ModelId=4248E9D20065
AcctItemMgr::~AcctItemMgr()
{
}

//##ModelId=42639DC8008F
#ifdef PRIVATE_MEMORY
bool AcctItemMgr::getBelong(int iChildItem, int iParentItem)
{
	AcctItem * pAcctItem = 0;

	if (m_poIndex->get (iChildItem, &pAcctItem)) {

		if (iChildItem == iParentItem) return true;

		while (pAcctItem) {
			if (pAcctItem->m_iParentID == iParentItem) {
				return true;
			}

			pAcctItem = pAcctItem->m_poParent;
		}
	}

	return false;
}
#else
bool AcctItemMgr::getBelong(int iChildItem, int iParentItem)
{
    unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(!(G_PPARAMINFO->m_poAcctItemIndex->get(iChildItem, &iIdx)))
        return false;
    else if(iChildItem == iParentItem)
        return true;
    else
    {
        /*pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        while(1)
        {
            if(pAcctItem->m_iAcctItemID == iParentItem)
                return true;
            else if(pAcctItem->m_iParentOffset != 0)
                pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + pAcctItem->m_iParentOffset;
            else
                return false;
        }*/
		char sKey[32] = {0};
		sprintf(sKey,"%d_%d",iChildItem,iParentItem);
		unsigned int i = 0;
		return G_PPARAMINFO->m_poAcctItemStrIndex->get(sKey,&i);
    }
}
#endif

//##ModelId=42639E4800B1
#ifdef PRIVATE_MEMORY
int AcctItemMgr::getParent(int iChildItem)
{
	AcctItem * pAcctItem = 0;
	if (m_poIndex->get (iChildItem, &pAcctItem)) {
		return pAcctItem->m_iParentID;
	}

	return -1;
}
#else
int AcctItemMgr::getParent(int iChildItem)
{
    unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(G_PPARAMINFO->m_poAcctItemIndex->get(iChildItem, &iIdx))
    {
        pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        return pAcctItem->m_iParentID;
    }
    else
    {
        return -1;
    }
}
#endif

//##ModelId=42639F2B03A7
AcctItem::AcctItem()
:m_iAcctItemID(0), m_iParentID(0), m_poParent(0), m_poOther(0)
{
	memset (m_sAcctItemCode ,0, sizeof (m_sAcctItemCode));
}


//##ModelId=42639EA40168
bool AcctItemMgr::m_bUploaded = false;

//##ModelId=42639F8F036F
HashList<AcctItem *> *AcctItemMgr::m_poIndex = 0;

//##ModelId=42639F53020A
AcctItem *AcctItemMgr::m_poAcctItemList = 0;

HashList<int> * AcctItemMgr::m_poItemParentIndex = 0;
ItemGroupMember * AcctItemMgr::m_poItemParentList = 0;

HashList<int> * AcctItemMgr::m_poItemChildIndex = 0;
AcctItemChild * AcctItemMgr::m_poItemChildList = 0;

HashList<int> * AcctItemMgr::m_poGroupMemberBIndex = 0;
ItemGroupMemberB * AcctItemMgr::m_poGroupMebmerBList = 0;

HashList<int> * AcctItemMgr::m_poBasicGroupMemberBIndex = 0;
ItemGroupMemberB * AcctItemMgr::m_poBasicGroupMebmerBList = 0;


void AcctItemMgr::loadGroupB()
{
	int i, j, k, l;
	int m = 1;
	int curGroup = 0;
	int iNum = 0;
	char sql[256];

	TOCIQuery qry(Environment::getDBConn());

	strcpy (sql, "select count(distinct item_group_id) "
                " from b_acct_item_group_member");
	qry.setSQL (sql);
	qry.open (); qry.next ();
	j = qry.field(0).asInteger ();
	qry.close ();

	m_poGroupSerialB = new HashList<int> ((j>>1) | 1);
	m_poMatrixB = new SimpleMatrix (m_iCount, j, false);

    m_piIncludeDiscount = (int *)malloc (sizeof (int) * (j+8));
	m_pBasicMember = (ItemGroupMember **)malloc (
                            sizeof (ItemGroupMember *) * (j+8));
    m_pDisctMember = (ItemGroupMember **)malloc (
                            sizeof (ItemGroupMember *) * (j+8));
	m_poItemTemp =  new HashList<int> (MAX_ACCTITEM_PERGROUP);

	m_poYes =  new HashList<int> (MAX_ACCTITEM_PERGROUP);
	m_poNo =  new HashList<int> (MAX_ACCTITEM_PERGROUP);
	m_poChildYes =  new HashList<int> (MAX_ACCTITEM_PERGROUP);
	m_poChildNo =  new HashList<int> (MAX_ACCTITEM_PERGROUP);

	if (!m_poGroupSerialB || !m_poMatrixB || !m_piIncludeDiscount
            || !m_pBasicMember || !m_pDisctMember ||!m_poItemTemp) {
        THROW (MBC_AcctItemMgr + 1);
    }

	if(!m_poYes || !m_poNo || !m_poChildYes || !m_poChildNo){
		THROW (MBC_AcctItemMgr + 1);
	}

    memset (m_piIncludeDiscount, 0, sizeof (int) * (j+8));
    memset (m_pBasicMember, 0, sizeof (ItemGroupMember *) * (j+8));
    memset (m_pDisctMember, 0, sizeof (ItemGroupMember *) * (j+8));

	strcpy (sql, "select distinct s.item_group_id,nvl(s.include_discount_item, 1) from b_acct_item_group s"
		" where exists (select 1 from b_acct_item_group_member t where t.item_group_id=s.item_group_id) ");
	qry.setSQL (sql);
	qry.open ();
	while (qry.next ()) {
		//i = qry.field(0).asInteger ();
		j = qry.field(0).asInteger ();
		if (!(m_poGroupSerialB->get(j, &l))) {
			l = curGroup++; 	
			m_poGroupSerialB->add (j, l);
		}
        m_piIncludeDiscount[l] = qry.field(1).asInteger ();
		iNum = getGroupDetail(j);
		m_poItemTemp->clearAll();
		for(int s=0;s<iNum;s++)
		{
			i = m_aoAcctItemID[s];
			if (!(m_poItemSerial->get(i, &k))) {
				//THROW (MBC_AcctItemMgr+11);
				continue;
			}
			//dup process
			if(m_poItemTemp->get(i,&m)){
				continue;
			}else{
				m_poItemTemp->add(i,m);
			}
			m_poMatrixB->setBelong (k, l);
			/*  先不考虑释放    */
			ItemGroupMember *p = new ItemGroupMember ();

			if (!p) {
				THROW (MBC_AcctItemMgr + 1);
			}

			p->m_iItemTypeID = i;

			if ((m_poAcctItemList[k].m_sAcctItemCode[0] | 0x20)=='b') {
				p->m_poNext = m_pDisctMember[l];
				m_pDisctMember[l] = p;
			} else {
				p->m_poNext = m_pBasicMember [l];
				m_pBasicMember[l] = p;
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

//##ModelId=42639E8800F0
void AcctItemMgr::loadGroup()
{
	int i, j, k, l;
	int curGroup = 0;
	char sql[2048];

	TOCIQuery qry(Environment::getDBConn());

	strcpy (sql, "SELECT COUNT(DISTINCT ACCT_ITEM_TYPE_ID) COUNT1,"
			"COUNT(DISTINCT ITEM_GROUP_ID) COUNT2 "
			"FROM ACCT_ITEM_GROUP_MEMBER");
	qry.setSQL (sql);
	qry.open ();
	qry.next ();
	i = qry.field(0).asInteger ();
	j = qry.field(1).asInteger ();
	qry.close ();

	m_poGroupSerial = new HashList<int> ((j>>1) | 1);
	m_poMatrix = new SimpleMatrix (m_iCount, j, false);

	if (!m_poGroupSerial || !m_poMatrix)
		THROW(MBC_AcctItemMgr+1);

	sprintf (sql, "SELECT ACCT_ITEM_TYPE_ID, ITEM_GROUP_ID "
			"FROM ACCT_ITEM_GROUP_MEMBER "
			"WHERE STATE='%s'", "10A");
	qry.setSQL (sql);
	qry.open ();
	while (qry.next ()) {
		i = qry.field(0).asInteger ();
		j = qry.field(1).asInteger ();
		if (!(m_poItemSerial->get(i, &k))) {
		//	这里正是运行的时候应该throw，因为现在acct_item_type有两套表
		//	THROW (MBC_AcctItemMgr+11);
			continue;
		}

		if (!(m_poGroupSerial->get(j, &l))) {
			l = curGroup++; 	
			m_poGroupSerial->add (j, l);
		}

		m_poMatrix->setBelong (k, l);
	}
	qry.close ();

}

//##ModelId=42639E8E02E3
void AcctItemMgr::loadItem()
{
	int count;
	AcctItem * pAcctItem;
	ItemGroupMember * pMemberParent;
	char * s;
	char sTemp[16];

	int i = 0,j = 0;
	int iVal;

	TOCIQuery qry (Environment::getDBConn ());
	
	qry.setSQL ("select count(*) from acct_item_type");
	qry.open ();
	qry.next ();

	count = qry.field(0).asInteger () + 16;

	qry.close ();

	m_poItemSerial = new HashList<int> ((count>>1) | 1);
	m_poIndex = new HashList<AcctItem *> ((count>>1) | 1);
	m_poItemParentIndex = new HashList<int>(count-15);

	if (!m_poIndex || !m_poItemSerial || !m_poItemParentIndex) THROW(MBC_AcctItemMgr+1);

	m_poAcctItemList = new AcctItem[count];
	m_poItemParentList =  new ItemGroupMember[count-15];
	if (!m_poAcctItemList||!m_poItemParentList) THROW(MBC_AcctItemMgr+1);

	
	qry.setSQL ("select acct_item_type_id, nvl(summary_acct_item_type_id, -1), acct_item_type_code, "
			"nvl(party_role_id,0) party_role_id from acct_item_type order by acct_item_type_code");
	qry.open ();

	while (qry.next ()) {
		if (i == count) THROW(MBC_AcctItemMgr+2);

		pAcctItem = m_poAcctItemList + i;

		pAcctItem->m_iAcctItemID = qry.field(0).asInteger ();
		pAcctItem->m_iParentID = qry.field(1).asInteger ();
        pAcctItem->m_iOffset = i;

		strcpy (pAcctItem->m_sAcctItemCode, qry.field(2).asString ());
		
		pAcctItem->m_iPartyRoleID = qry.field(3).asInteger ();
		
		s = pAcctItem->m_sAcctItemCode;
		if(pAcctItem->m_iParentID!=-1){
			iVal = 0;
			pMemberParent = m_poItemParentList+j;
			pMemberParent->m_iItemTypeID=pAcctItem->m_iAcctItemID;
			pMemberParent->m_poNext = 0;
			if(m_poItemParentIndex->get(pAcctItem->m_iParentID,&iVal)){				
				pMemberParent->m_poNext = &m_poItemParentList[iVal];
			}
			m_poItemParentIndex->add(pAcctItem->m_iParentID,j);
			j ++;
		}
		m_poIndex->add (pAcctItem->m_iAcctItemID, pAcctItem);
		m_poItemSerial->add  (pAcctItem->m_iAcctItemID, i);

		i++;
	}

	qry.close ();
	
	count = i;

	m_iCount = i;

	for (i=0; i<count; i++) {
		pAcctItem = m_poAcctItemList + i;

		if (pAcctItem->m_iParentID != -1) {
			m_poIndex->get (pAcctItem->m_iParentID, &(pAcctItem->m_poParent));
		}

		strncpy (sTemp, pAcctItem->m_sAcctItemCode, sizeof(sTemp));

		if (sTemp[0] == 'A' || sTemp[0] == 'B')
		{
			int iMin = 0;
			int iMax = m_iCount-1;
			int iMid;
			int iTemp;
			
			sTemp[0] = (sTemp[0] == 'A') ? 'B' : 'A';

			while (iMin <= iMax) {

				iMid = (iMin + iMax) / 2;
				iTemp = strcmp (m_poAcctItemList[iMid].m_sAcctItemCode, sTemp);
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

void AcctItemMgr::loadGroupMeberB()
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

	m_poGroupMemberBIndex = new HashList<int> (count);

	if (!m_poGroupMemberBIndex) THROW(MBC_AcctItemMgr+1);

	m_poGroupMebmerBList = new ItemGroupMemberB[count];
	if (!m_poGroupMebmerBList) THROW(MBC_AcctItemMgr+1);

	
	qry.setSQL ("select nvl(item_group_id,0) item_group_id, acct_item_object_type,"
		"nvl(acct_item_object_id,0) acct_item_object_id,nvl(exclude_flag, 0) exclude_flag "
			"from b_acct_item_group_member order by item_group_id");
	qry.open ();

	while (qry.next ()) {
		if (i == count) THROW(MBC_AcctItemMgr+2);

		p = m_poGroupMebmerBList + i;

		lTemp = qry.field(0).asLong();
		strcpy(strFlag,qry.field(1).asString());
		/*if(strFlag[0]|0x20 == 'a'){
			p->m_iObjectType = 1;
		}else{
			p->m_iObjectType = 2;
		}*/
		p->m_iObjectType = 0;
		switch(strFlag[0]|0x20){
			case 'a':
				p->m_iObjectType = 1;
				break;
			case 'b':
				p->m_iObjectType = 2;
				break;
		}
		p->m_iItemObjectID = qry.field(2).asInteger ();
		p->m_iExcludeFlag = qry.field(3).asInteger();
		p->m_poNext = 0;
		if(lTemp == lLastTemp){
			p->m_poNext=pTemp;
		}
		m_poGroupMemberBIndex->add(lTemp,i);
		i++;
		lLastTemp = lTemp;
		pTemp = p;
	}
	qry.close ();
	return;
}

void AcctItemMgr::loadBasicGroupMeberB()
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

	m_poBasicGroupMemberBIndex = new HashList<int> (count);

	if (!m_poBasicGroupMemberBIndex) THROW(MBC_AcctItemMgr+1);

	m_poBasicGroupMebmerBList = new ItemGroupMemberB[count];
	if (!m_poBasicGroupMebmerBList) THROW(MBC_AcctItemMgr+1);

	
	qry.setSQL ("select nvl(item_group_id,0) item_group_id, nvl(acct_item_type_id,0) acct_item_type_id, "
		"nvl(exclude_flag, 0) exclude_flag "
			"from b_basic_acct_item_group_member order by item_group_id");
	qry.open ();

	while (qry.next ()) {
		if (i == count) THROW(MBC_AcctItemMgr+2);

		p = m_poBasicGroupMebmerBList + i;

		lTemp = qry.field(0).asLong();

		p->m_iItemObjectID = qry.field(1).asInteger ();
		p->m_iExcludeFlag = qry.field(2).asInteger();
		p->m_poNext = 0;
		if(lTemp == lLastTemp){
			p->m_poNext=pTemp;
		}
		m_poBasicGroupMemberBIndex->add(lTemp,i);
		i++;
		lLastTemp = lTemp;
		pTemp = p;
	}
	qry.close ();
	return;
}

void AcctItemMgr::analyzeItem()
{
	AcctItemChild * pItemChild;
	AcctItem * pAcctItem;
	int i = 0;

	m_poItemChildIndex = new HashList<int> (m_iCount+1);
	if (!m_poItemChildIndex ) THROW(MBC_AcctItemMgr+1);

	m_poItemChildList = new AcctItemChild[m_iCount+1];
	if (!m_poItemChildList) THROW(MBC_AcctItemMgr+1);

	memset(m_poItemChildList,0,sizeof(AcctItemChild)*(m_iCount+1));
	for (i=0; i<m_iCount; i++) {
		pItemChild = m_poItemChildList + i;
		pAcctItem = m_poAcctItemList + i;
		m_iIndex = 0;
		pItemChild->m_iAcctItemID = pAcctItem->m_iAcctItemID;
		pItemChild->m_iChildNum = getItemChild(pAcctItem->m_iAcctItemID,pItemChild->m_aoChildItemID);
		m_poItemChildIndex->add(pAcctItem->m_iAcctItemID,i);
	}
}

int AcctItemMgr::getItemChild(int iAcctItemID,int * pItemChild)
{
	int iTemp = 0;
	if(m_poItemParentIndex->get(iAcctItemID,&iTemp)){
		getItemChild(m_poItemParentList[iTemp].m_iItemTypeID,pItemChild);
		ItemGroupMember * p = m_poItemParentList[iTemp].m_poNext;
		while(p){
			getItemChild(p->m_iItemTypeID,pItemChild);
			p = p->m_poNext;
		}
	}else{
		*(pItemChild+m_iIndex) = iAcctItemID;
		m_iIndex++;
	}
	return m_iIndex;
}

int AcctItemMgr::getGroupDetail(long lGroupID)
{
	vector<int> vYes;
	vector<int> vNo;
	vector<int> vChildYes;
	vector<int> vChildNo;
	ItemGroupMemberB * p;
	ItemGroupMemberB * pOther;
	AcctItemChild * pChild;
	int iVal = 0,iNum = 0;
	int m = 1;
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));
	m_poYes->clearAll();
	m_poNo->clearAll();
	m_poChildYes->clearAll();
	m_poChildNo->clearAll();
	//优惠节点包含和不包含
	if(m_poGroupMemberBIndex->get(lGroupID,&iVal)){
		p = m_poGroupMebmerBList+iVal;
		while(p){
			if(p->m_iObjectType == 1){
				if(m_poItemChildIndex->get(p->m_iItemObjectID,&iVal)){
					pChild = m_poItemChildList+iVal;
					if(p->m_iExcludeFlag==0){
						for(int i=0;i<pChild->m_iChildNum;i++){
							if(m_poYes->get(pChild->m_aoChildItemID[i],&m)){
								continue;
							}else{
								vYes.push_back(pChild->m_aoChildItemID[i]);
								m_poYes->add(pChild->m_aoChildItemID[i],m);
							}
						}
					}else{
						for(int i=0;i<pChild->m_iChildNum;i++){
							if(m_poNo->get(pChild->m_aoChildItemID[i],&m)){
								continue;
							}else{
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
		p = m_poGroupMebmerBList+iVal;
		while(p){
			if(p->m_iObjectType == 2){
				if(m_poBasicGroupMemberBIndex->get(p->m_iItemObjectID,&iVal)){
					pOther = m_poBasicGroupMebmerBList + iVal;
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
	while(iterYes!=vYes.end()){
		while(iterNo!=vNo.end()){
			if(*iterNo == *iterYes){
				bNo = true;
				break;
			}
			iterNo++;
		}
		if(!bNo){
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

//##ModelId=42841D1B02B4
#ifdef PRIVATE_MEMORY
int AcctItemMgr::getDisctItem(int iNormalItemType)
{
	AcctItem * pAcctItem = 0;
	if (m_poIndex->get (iNormalItemType, &pAcctItem)) {
		return pAcctItem->m_poOther->m_iAcctItemID;
	}

	THROW (MBC_AcctItemMgr+10);
}
#else
int AcctItemMgr::getDisctItem(int iNormalItemType)
{
	unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(G_PPARAMINFO->m_poAcctItemIndex->get(iNormalItemType, &iIdx))
    {
        pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        if(pAcctItem->m_iOtherOffset != 0)
        {
            pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + pAcctItem->m_iOtherOffset;
            return pAcctItem->m_iAcctItemID;
        }
        else
            THROW (MBC_AcctItemMgr+10);
    }
    else
        THROW (MBC_AcctItemMgr+10);
}
#endif

#ifdef PRIVATE_MEMORY
bool AcctItemMgr::isDisctItem(int iAcctItemType)
{
	AcctItem * pAcctItem = 0;

	return (m_poIndex->get (iAcctItemType, &pAcctItem) && pAcctItem->m_sAcctItemCode[0]=='B');
}
#else
bool AcctItemMgr::isDisctItem(int iAcctItemType)
{
	unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(G_PPARAMINFO->m_poAcctItemIndex->get(iAcctItemType, &iIdx))
    {
        pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        if(pAcctItem->m_sAcctItemCode[0]=='B')
        {
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
#endif

#ifdef PRIVATE_MEMORY
bool AcctItemMgr::isPartyRoleItem(int iAcctItemType,int iPartyRoleID)
{
	AcctItem * pAcctItem = 0;
	return (m_poIndex->get (iAcctItemType, &pAcctItem) && pAcctItem->m_iPartyRoleID==iPartyRoleID);
}
#else
bool AcctItemMgr::isPartyRoleItem(int iAcctItemType,int iPartyRoleID)
{
	unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(G_PPARAMINFO->m_poAcctItemIndex->get(iAcctItemType, &iIdx))
    {
        pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        if(pAcctItem->m_iPartyRoleID == iPartyRoleID)
        {
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
#endif

//##ModelId=42841D350352
#ifdef PRIVATE_MEMORY
int AcctItemMgr::reverseDisctItem(int iDisctItemType)
{
	AcctItem * pAcctItem = 0;
	if (m_poIndex->get (iDisctItemType, &pAcctItem)) {
		return pAcctItem->m_poOther->m_iAcctItemID;
	}

	THROW (MBC_AcctItemMgr+10);
}
#else
int AcctItemMgr::reverseDisctItem(int iDisctItemType)
{
	unsigned int iIdx = 0;
    AcctItem *pAcctItem = 0;
    if(G_PPARAMINFO->m_poAcctItemIndex->get(iDisctItemType, &iIdx))
    {
        pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + iIdx;
        if(pAcctItem->m_iOtherOffset != 0)
        {
            pAcctItem = (AcctItem *)G_PPARAMINFO->m_poAcctItemList + pAcctItem->m_iOtherOffset;
            return pAcctItem->m_iAcctItemID;
        }
        else
            THROW (MBC_AcctItemMgr+10);
    }
    else
        THROW (MBC_AcctItemMgr+10);
}
#endif

#ifdef PRIVATE_MEMORY
int * AcctItemMgr::getAcctItems(int iAcctItemGroupID, int * number)
{
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));

	int k;
	int *pInt ;
	int i = 0;

	if (m_poGroupSerial->get (iAcctItemGroupID, &k)) {
		pInt = m_poMatrix->getChilds (k);
		for (i=0; pInt[i]!=-1; i++) {
			m_aoAcctItemID[i] = m_poAcctItemList[pInt[i]].m_iAcctItemID;
		}
	}

	if (number) *number = i;

	m_aoAcctItemID[i] = -1;

	return m_aoAcctItemID;
}
#else
int * AcctItemMgr::getAcctItems(int iAcctItemGroupID, int * number)
{
    memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));
    int i = 0;
    
    unsigned int iIdx = 0;
    unsigned int iOffset = 0;
    ItemGroupMember *pItemMember = 0;
    if(G_PPARAMINFO->m_poAcctItemGroupIndex->get(iAcctItemGroupID, &iIdx))
    {
        pItemMember= (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupList + iIdx;
        m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
        i++;
        
        while(pItemMember->m_iNextOffset != 0)
        {
            iOffset = pItemMember->m_iNextOffset;
            pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupList + iOffset;
            
            m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
            i++;
        }
    }

    if(number) 
        *number = i;

    m_aoAcctItemID[i] = -1;
    return m_aoAcctItemID;
}
#endif

#ifdef PRIVATE_MEMORY
int * AcctItemMgr::getAcctItemsB(int iBAcctItemGroupID, int * number)
{
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));

	int k;
	//int *pInt ;
	int i = 0;

/*

	if (m_poGroupSerialB->get (iBAcctItemGroupID, &k)) {
		pInt = m_poMatrixB->getChilds (k);
		for (i=0; pInt[i]!=-1; i++) {
			m_aoAcctItemID[i] = m_poAcctItemList[pInt[i]].m_iAcctItemID;
		}
	}

	if (number) *number = i;
*/

    if (m_poGroupSerialB->get (iBAcctItemGroupID, &k)) {
        ItemGroupMember * p;

        p = m_pBasicMember [k];

        while (p) {
            m_aoAcctItemID[i] = p->m_iItemTypeID;
            p = p->m_poNext;
            i++;
        }

        p = m_pDisctMember [k];
        
        while (p) {
            m_aoAcctItemID[i] = p->m_iItemTypeID;
            p = p->m_poNext;
            i++;
        }
    }

    if (number) *number = i;

	m_aoAcctItemID[i] = -1;

	return m_aoAcctItemID;
}
#else
int * AcctItemMgr::getAcctItemsB(int iBAcctItemGroupID, int * number)
{
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));
    int i = 0;
    
    unsigned int iIdx = 0;
    unsigned int iOffset = 0;
    ItemGroupMember *pItemMember = 0;
    if(G_PPARAMINFO->m_poAcctItemGroupBIndex->get(iBAcctItemGroupID, &iIdx))
    {
        pItemMember= (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + iIdx;
        m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
        i++;
        
        while(pItemMember->m_iNextOffset != 0)
        {
            iOffset = pItemMember->m_iNextOffset;
            pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemGroupBList + iOffset;
            
            m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
            i++;
        }
    }

    if(number) 
        *number = i;

    m_aoAcctItemID[i] = -1;
    return m_aoAcctItemID;
}
#endif

#ifdef PRIVATE_MEMORY
int * AcctItemMgr::getBaseAcctItemsB(int iBAcctItemGroupID, int * number)
{
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));

	int k;
	int i = 0;

    if (m_poGroupSerialB->get (iBAcctItemGroupID, &k)) {
        ItemGroupMember * p;

        p = m_pBasicMember [k];

        while (p) {
            m_aoAcctItemID[i] = p->m_iItemTypeID;
            p = p->m_poNext;
            i++;
        }
    }

    if (number) *number = i;

	m_aoAcctItemID[i] = -1;

	return m_aoAcctItemID;
}
#else
int * AcctItemMgr::getBaseAcctItemsB(int iBAcctItemGroupID, int * number)
{
	memset (m_aoAcctItemID, 0, sizeof (m_aoAcctItemID));
    int i = 0;
    
    unsigned int iIdx = 0;
    unsigned int iOffset = 0;
    ItemGroupMember *pItemMember = 0;
    if(G_PPARAMINFO->m_poAcctItemBasicGroupBIndex->get(iBAcctItemGroupID, &iIdx))
    {
        pItemMember= (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemBasicGroupBList + iIdx;
        m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
        i++;
        
        while(pItemMember->m_iNextOffset != 0)
        {
            iOffset = pItemMember->m_iNextOffset;
            pItemMember = (ItemGroupMember *)G_PPARAMINFO->m_poAcctItemBasicGroupBList + iOffset;
            
            m_aoAcctItemID[i] = pItemMember->m_iItemTypeID;
            i++;
        }
    }

    if(number) 
        *number = i;

    m_aoAcctItemID[i] = -1;
    return m_aoAcctItemID;
}
#endif

