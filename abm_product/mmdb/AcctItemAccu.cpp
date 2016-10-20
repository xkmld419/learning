/*VER: 5*/ 
#include "AcctItemAccu.h"
#include "DataIncrementMgr.h"
#include <stdlib.h>
#include "Environment.h"
#include "SeqMgrExt.h"
#include "LogDB.h"

bool AcctItemBase::m_bAttached = false;
AcctItemData * AcctItemBase::m_poAcctItem = 0;

#ifdef MEM_REUSE_MODE
SHMData_B<AcctItemData> * AcctItemBase::m_poAcctItemData = 0;
SHMIntHashIndex_KR * AcctItemBase::m_poServIndex = 0;
#else
SHMData<AcctItemData> * AcctItemBase::m_poAcctItemData = 0;
SHMIntHashIndex * AcctItemBase::m_poServIndex = 0;
#endif


CSemaphore * AcctItemAccuMgr::m_poDataLock = 0;
CSemaphore * AcctItemAccuMgr::m_poIndexLock = 0;


#ifdef USE_SERV_LOCK
CSemaphore * AcctItemAccuMgr::m_poServLock[8] = {0, 0, 0, 0, 0, 0, 0, 0};
#endif

int getFlowID()
{
	int iRet;

	char * p = getenv("BILLFLOW_ID");

	if (!p) {
		p = getenv ("PROCESS_ID");
		if (!p) {
			return -1;
		}

		int iProcID = atoi (p);

		DEFINE_QUERY (qry);
		char sSQL[2048];
		sprintf (sSQL, "select app_id, billflow_id from wf_process "
				"where process_id=%d", iProcID);
		qry.setSQL(sSQL); qry.open();
		if (qry.next()) {
			iRet = qry.field(1).asInteger();
		} else {
                	iRet = -1;
		}
		qry.close ();

	} else {
		iRet = atoi (p);
	}

	return iRet;
}


AcctItemBase::AcctItemBase()
{
    if (m_bAttached)
        return;

    m_bAttached = true;	

#ifdef MEM_REUSE_MODE
    ATTACH_DATA_B(m_poAcctItemData, AcctItemData, ACCT_ITEM_DATA);
    ATTACH_INT_INDEX_KR(m_poServIndex, ACCT_ITEM_SERV_INDEX);
#else
    ATTACH_DATA(m_poAcctItemData, AcctItemData, ACCT_ITEM_DATA);
    ATTACH_INT_INDEX(m_poServIndex, ACCT_ITEM_SERV_INDEX);
#endif


    if (m_bAttached)
        bindData ();
        
     
}

void AcctItemBase::bindData()
{
    m_poAcctItem = (AcctItemData *)(*m_poAcctItemData);
}

void AcctItemBase::freeAll()
{
    if (m_poAcctItemData) {
        delete m_poAcctItemData;
        m_poAcctItemData = 0;
    }

    if (m_poServIndex) {
        delete m_poServIndex;
        m_poServIndex = 0;
    }
}

AcctItemAccuMgr::AcctItemAccuMgr() : AcctItemBase ()
{
    if (!m_bAttached) {
        THROW (MBC_AcctItemAccu+1);
    }

    m_poSeqExt = new SeqMgrExt ("acct_item_id_big_seq", 10000);
    if (!m_poSeqExt) {
        THROW (MBC_AcctItemAccu+2);
    }

    char sTemp[32];

    if (!m_poDataLock) {
        m_poDataLock = new CSemaphore ();
        sprintf (sTemp, "%d", ACCT_ITEM_DATA_LOCK);
        m_poDataLock->getSem (sTemp, 1, 1);
    }

    if (!m_poIndexLock) {
        m_poIndexLock = new CSemaphore ();
        sprintf (sTemp, "%d", ACCT_ITEM_INDEX_LOCK);
        m_poIndexLock->getSem (sTemp, 1, 1);
    }

#ifdef USE_SERV_LOCK

    int i;
    for (i=0; i<8; i++) {
        if (m_poServLock[i]) continue;

        sprintf (sTemp, "SEM_AcctItemServ%d", i+1);
        int iKey = IpcKeyMgr::getIpcKey (-1, sTemp);
        sprintf (sTemp, "%d", iKey);

        m_poServLock[i] = new CSemaphore ();
        m_poServLock[i]->getSem (sTemp, 1, 1);
    }

#endif

}

long AcctItemAccuMgr::update(AcctItemAddAggrData *pData, bool &bNew, bool isModifyAcct)
{
    long lServID = pData->lServID;
    long lAcctID = pData->lAcctID;
    long lOfferInstID = pData->lOfferInstID;
    int  iBillingCycleID = pData->iBillingCycleID;
    #ifdef ORG_BILLINGCYCLE
    int iOrgBillingCycleID = pData->m_iOrgBillingCycleID;
    #endif
    int  iPayMeasure = pData->iPayMeasure;
    int  iPayItemTypeID = pData->iPayItemTypeID;
    char *sDate = pData->sStateDate;
    int  iItemSourceID = pData->iItemSourceID;
	int  iItemClassID = pData->iItemClassID;

    unsigned int i;
    int iLock = lServID & 7;

#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif

#ifdef USE_SERV_LOCK
    m_poServLock[iLock]->P ();
#endif

    if (!m_poServIndex->get (lServID, &i)) {

        m_poDataLock->P ();
        
#ifdef MEM_REUSE_MODE
        i = m_poAcctItemData->malloc (iBillingCycleID );
        if(!i){
            m_poIndexLock->P ();
            revoke( m_poAcctItemData->getAutoRevokeKey(), true );
            m_poIndexLock->V ();
            i = m_poAcctItemData->malloc (iBillingCycleID );
        }
#else
        i = m_poAcctItemData->malloc ();

#endif

        m_poDataLock->V ();

        if (!i) {
        #ifdef USE_SERV_LOCK
            m_poServLock[iLock]->V ();
        #endif

            THROW (MBC_AcctItemAccu+3);
        }


        AcctItemData * p = m_poAcctItem + i;
        p->m_lAcctItemID = m_poSeqExt->getNextVal ();
        p->m_lServID = lServID;
        p->m_lAcctID = lAcctID;
        p->m_lOfferInstID = lOfferInstID;
        p->m_iBillingCycleID = iBillingCycleID;
        p->m_lValue = pData->lCharge;
        p->m_iPayMeasure = iPayMeasure;
        p->m_iPayItemTypeID = iPayItemTypeID;
        p->m_iItemSourceID = iItemSourceID;
        p->m_iItemClassID = iItemClassID;
    #ifdef ORG_BILLINGCYCLE
        p->m_iOrgBillingCycleID = iOrgBillingCycleID;
    #endif
        strcpy (p->m_sStateDate, sDate);

        m_poIndexLock->P ();
        
#ifdef MEM_REUSE_MODE
        m_poServIndex->get(lServID, & p->m_iServNext);
        m_poServIndex->add (lServID, i);
#else
        m_poServIndex->add (lServID, i);
#endif

        m_poIndexLock->V ();

#ifdef USE_SERV_LOCK
        m_poServLock[iLock]->V ();
#endif

        bNew = true;

        return p->m_lAcctItemID;
    }

    unsigned int j;

    for (; i; i=m_poAcctItem[i].m_iServNext) {
        j = i;
        AcctItemData * p = m_poAcctItem + i;
        if (p->m_iPayItemTypeID != iPayItemTypeID) continue;
        if (p->m_iPayMeasure != iPayMeasure) continue;
        if (p->m_lOfferInstID != lOfferInstID) continue;
        if (p->m_iBillingCycleID != iBillingCycleID) continue;
        if (p->m_iItemSourceID != iItemSourceID) continue;
        if (p->m_iItemClassID != iItemClassID) continue;
    #ifdef ORG_BILLINGCYCLE
        if (p->m_iOrgBillingCycleID != iOrgBillingCycleID)
            continue;
    #endif
        
        p->m_lValue += pData->lCharge;

		//		if(isModifyAcct)
						p->m_lAcctID = lAcctID;

        if (strcmp (sDate, p->m_sStateDate) > 0) {
            strcpy (p->m_sStateDate, sDate);
        } else {
            strcpy (sDate, p->m_sStateDate);
        }

#ifdef USE_SERV_LOCK
        m_poServLock[iLock]->V ();
#endif

        bNew = false;
        return p->m_lAcctItemID;
    }

    m_poDataLock->P ();
#ifdef MEM_REUSE_MODE
    i = m_poAcctItemData->malloc ( iBillingCycleID );
    if(!i){
        m_poIndexLock->P ();
        revoke( m_poAcctItemData->getAutoRevokeKey(), true );
        m_poIndexLock->V ();
        i = m_poAcctItemData->malloc (iBillingCycleID );
    }
#else
    i = m_poAcctItemData->malloc ();
#endif

    m_poDataLock->V ();

    if (!i) {
#ifdef USE_SERV_LOCK
        m_poServLock[iLock]->V ();
#endif
        THROW (MBC_AcctItemAccu+3);
    }

    AcctItemData * p = m_poAcctItem + i;
    p->m_lAcctItemID = m_poSeqExt->getNextVal ();
    p->m_lServID = lServID;
    p->m_lAcctID = lAcctID;
    p->m_lOfferInstID = lOfferInstID;
    p->m_iBillingCycleID = iBillingCycleID;
    p->m_lValue = pData->lCharge;
    p->m_iPayMeasure = iPayMeasure;
    p->m_iPayItemTypeID = iPayItemTypeID;
    p->m_iItemSourceID = iItemSourceID;
	p->m_iItemClassID = iItemClassID;
#ifdef ORG_BILLINGCYCLE
    p->m_iOrgBillingCycleID = iOrgBillingCycleID;
#endif
    strcpy (p->m_sStateDate, sDate);

#ifdef MEM_REUSE_MODE
    p->m_iServNext = 0;
    m_poIndexLock->P ();
    m_poServIndex->get(lServID, & p->m_iServNext);
    m_poServIndex->add (lServID, i);

    m_poIndexLock->V ();
#else
    m_poAcctItem[j].m_iServNext = i;
#endif
       

#ifdef USE_SERV_LOCK
    m_poServLock[iLock]->V ();
#endif
    
    bNew = true;
    return p->m_lAcctItemID;
}

bool AcctItemAccuMgr::update(AcctItemData *pData,bool &bNew,bool bReplace)
{
	if(!pData) return false;
	unsigned int i,j;
	
#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif

	if (!m_poServIndex->get (pData->m_lServID, &i))
	{
        m_poDataLock->P ();
#ifdef MEM_REUSE_MODE
        i = m_poAcctItemData->malloc (pData->m_iBillingCycleID);
        if(!i){
            m_poIndexLock->P ();
            revoke( m_poAcctItemData->getAutoRevokeKey(), true );
            m_poIndexLock->V ();
            i = m_poAcctItemData->malloc (pData->m_iBillingCycleID );
        }
#else
        i = m_poAcctItemData->malloc ();
#endif
        m_poDataLock->V ();

        if (!i){
            THROW (MBC_AcctItemAccu+3);
        }
        AcctItemData * p = m_poAcctItem + i;
		memcpy(p,pData,sizeof(AcctItemData));       
		p->m_iServNext = 0;       
        m_poIndexLock->P ();
#ifdef MEM_REUSE_MODE
        m_poServIndex->get(pData->m_lServID, & p->m_iServNext);
        if(p->m_iBillingCycleID)m_poServIndex->add (pData->m_lServID, i);
#else
        m_poServIndex->add (pData->m_lServID, i);
#endif        
        m_poIndexLock->V ();
        bNew = true;
		return true;
    }
    for(; i; i=m_poAcctItem[i].m_iServNext)
	{
		j = i;
		AcctItemData * p = m_poAcctItem + i;
		if (p->m_iPayItemTypeID != pData->m_iPayItemTypeID) continue;
		if (p->m_iPayMeasure != pData->m_iPayMeasure) continue;
		if (p->m_lOfferInstID != pData->m_lOfferInstID) continue;
		if (p->m_iBillingCycleID != pData->m_iBillingCycleID) continue;
		if (p->m_iItemSourceID != pData->m_iItemSourceID) continue;
		if (p->m_iItemClassID != pData->m_iItemClassID) continue;
        if (p->m_lAcctItemID != pData->m_lAcctItemID) continue;
        #ifdef ORG_BILLINGCYCLE
        if (p->m_iOrgBillingCycleID != pData->m_iOrgBillingCycleID)
            continue;
        #endif
		if(bReplace){
			p->m_lValue = pData->m_lValue;
		}else{
			p->m_lValue += pData->m_lValue;
		}
		p->m_lAcctID = pData->m_lAcctID;

		if (strcmp (pData->m_sStateDate, p->m_sStateDate) > 0) {
            strcpy (p->m_sStateDate, pData->m_sStateDate);
        }
        bNew = false;
		return true;
    }

    m_poDataLock->P ();
#ifdef MEM_REUSE_MODE
    i = m_poAcctItemData->malloc (pData->m_iBillingCycleID );
    if(!i){
        m_poIndexLock->P ();
        revoke( m_poAcctItemData->getAutoRevokeKey(), true );
        m_poIndexLock->V ();
        i = m_poAcctItemData->malloc (pData->m_iBillingCycleID );
    }

#else
    i = m_poAcctItemData->malloc ();
#endif

    m_poDataLock->V ();

    if (!i){
        THROW (MBC_AcctItemAccu+3);
    }
    AcctItemData * p = m_poAcctItem + i;
	memcpy(p,pData,sizeof(AcctItemData));    

#ifdef MEM_REUSE_MODE
	p->m_iServNext = 0;
    m_poIndexLock->P ();
    m_poServIndex->get(pData->m_lServID, & p->m_iServNext);
    m_poServIndex->add (pData->m_lServID, i);
    m_poIndexLock->V ();

#else
    m_poAcctItem[j].m_iServNext = i;
#endif



    bNew = true;
	return true;
}

long AcctItemAccuMgr::get(long lServID, int iBillingCycleID, int iAcctItemTypeID, 
                         int iOrgBillingCycleID)
{
    unsigned int i;

#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif

    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    long lValue = 0;

    if (iAcctItemTypeID) {
        for (; i; i=m_poAcctItem[i].m_iServNext) {
            AcctItemData *p = m_poAcctItem + i;
            if (p->m_iBillingCycleID==iBillingCycleID &&
                    p->m_iPayMeasure==PAY_MEASURE_MONEY && 
                p->m_iPayItemTypeID==iAcctItemTypeID
                    #ifdef ORG_BILLINGCYCLE
                &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
                    #endif
               ) {
                lValue += p->m_lValue;

            }
        }
    } else {
        for (; i; i=m_poAcctItem[i].m_iServNext) {
            AcctItemData *p = m_poAcctItem + i;
            if (p->m_iBillingCycleID==iBillingCycleID &&
                p->m_iPayMeasure==PAY_MEASURE_MONEY
                    #ifdef ORG_BILLINGCYCLE
                &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
                    #endif
               ) {
                lValue += p->m_lValue;
            }
        }
    }

    return lValue;
}

long AcctItemAccuMgr::getAcctItemDataPartyRoleID(long lServID, int iBillingCycleID, 
                                                int iPartyRoleID, int iOrgBillingCycleID)
{
    unsigned int i;

#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif

    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    long lValue = 0;
		for (; i; i=m_poAcctItem[i].m_iServNext) {
            AcctItemData *p = m_poAcctItem + i;
            if (p->m_iBillingCycleID==iBillingCycleID &&
            p->m_iPayMeasure==PAY_MEASURE_MONEY 
                #ifdef ORG_BILLINGCYCLE
            &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
                #endif
           ) {
            if (G_PACCTITEMMGR->isPartyRoleItem (p->m_iPayItemTypeID,iPartyRoleID))
                lValue += p->m_lValue;
            }
    }
    return lValue;
}

int AcctItemAccuMgr::getDisct(long lServID, int iBillingCycleID,
                              vector<AcctItemData *> & v, int iOrgBillingCycleID)
{
    unsigned int i;
#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif
    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    int iRet (0);
    for (; i; i=m_poAcctItem[i].m_iServNext) {
        AcctItemData *p = m_poAcctItem + i;
        if (p->m_iBillingCycleID == iBillingCycleID &&
            p->m_iPayMeasure==PAY_MEASURE_MONEY
                #ifdef ORG_BILLINGCYCLE
            &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
                #endif
           ) {
            if (G_PACCTITEMMGR->isDisctItem (p->m_iPayItemTypeID)) {
                v.push_back (p);
                iRet++;
            }
        }
    }

    return iRet;
}

int AcctItemAccuMgr::getAll(long lServID, int iBillingCycleID,
                            vector<AcctItemData *> & v, int iOrgBillingCycleID)
{
    unsigned int i;
#ifdef MEM_REUSE_MODE
    m_poAcctItemData->checkSafe();
#endif
    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    int iRet (0);

    for (; i; i=m_poAcctItem[i].m_iServNext) {
        AcctItemData *p = m_poAcctItem + i;
        if (p->m_iBillingCycleID == iBillingCycleID &&
                p->m_iPayMeasure==PAY_MEASURE_MONEY
                #ifdef ORG_BILLINGCYCLE            
            && p->m_iOrgBillingCycleID == iOrgBillingCycleID
                #endif
           ) {
                v.push_back (p);
                iRet++;
        }
    }
    return iRet;
}
int AcctItemAccuMgr::getHis(long lServID, vector < AcctItemData * > & v,
                            int iOrgBillingCycleID)
{
    unsigned int i;

    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    int iRet (0);

    for (; i; i=m_poAcctItem[i].m_iServNext) {
        AcctItemData *p = m_poAcctItem + i;
/*
        if (p->m_iBillingCycleID == iBillingCycleID &&
            p->m_iPayMeasure==PAY_MEASURE_MONEY
                #ifdef ORG_BILLINGCYCLE            
            && p->m_iOrgBillingCycleID == iOrgBillingCycleID
                #endif
           ) {
                v.push_back (p);
                iRet++;
        }
    }

    return iRet;
}

int AcctItemAccuMgr::getHis(long lServID, vector < AcctItemData * > & v,
                            int iOrgBillingCycleID)
{
    unsigned int i;

    if (!m_poServIndex->get (lServID, &i)) {
        return 0;
    }

    int iRet (0);

    for (; i; i=m_poAcctItem[i].m_iServNext) {
        AcctItemData *p = m_poAcctItem + i;
*/
        if (p->m_iPayMeasure==PAY_MEASURE_MONEY
                #ifdef ORG_BILLINGCYCLE
            &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
                #endif
           ) {
                v.push_back (p);
                iRet++;
        }
    }

    return iRet;    
}


bool AcctItemAccuMgr::checkExist(long lServID, int iBillingCycleID, 
                                 int iItemSourceID, int iOrgBillingCycleID ){
    unsigned int i ;

    if (!m_poServIndex->get (lServID, &i)) {
        return false;
    }

    for (; i; i=m_poAcctItem[i].m_iServNext) {
        AcctItemData *p = m_poAcctItem + i;
        if (p->m_iBillingCycleID == iBillingCycleID
            && p->m_iItemSourceID == iItemSourceID
            #ifdef ORG_BILLINGCYCLE
            &&p->m_iOrgBillingCycleID == iOrgBillingCycleID
            #endif
           ) {
            return true;
        }
    }

    return false;
}

AcctItemCtl::AcctItemCtl() : AcctItemBase()
{
	 
}

bool AcctItemCtl::GetCurProcNum()
{
	bool bResult = true;
	
	if(( m_poAcctItemData->connnum() > 1 )
		|| ( m_poServIndex->connnum() > 1 )
		)
	{
		cout << "检测到其他进程在使用总账,现在还不能加载/删除总账...\n\n\n";
		cout << "如果你确实需要加载/删除，请输入命令强制加载/删除" << endl;
	}
	else
	{
		bResult = false;
	}

	return bResult;

}


bool AcctItemCtl::exist()
{
    return m_bAttached;
}

void AcctItemCtl::remove()
{
    if (m_poAcctItemData->exist ()) {
        m_poAcctItemData->remove ();
    }

    if (m_poServIndex->exist ()) {
        m_poServIndex->remove ();
    }
}

void AcctItemCtl::create()
{
    m_poAcctItemData->create (ACCT_ITEM_COUNT);
    m_poServIndex->create (ACCT_ITEM_INDEX_COUNT);

    bindData ();
    m_bAttached = true;
}

int AcctItemCtl::out(char const *sTable)
{
    unsigned int iTotal=0;
    unsigned int count=0,allCount = 0;
    AcctItemData *pStart=0;

#ifdef MEM_REUSE_MODE

    if( m_poAcctItemData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return 0;
    }

    SHM_KEY_BLOCK_ARRAY pBlock[1];
    memset(pBlock, 0 , sizeof(SHM_KEY_BLOCK_ARRAY) );
    m_poAcctItemData->getBlockInfo(pBlock);
    allCount = m_poAcctItemData->getCount();

    for(int i=0; i< SHM_BLOCK_NUM; i++){
        if(  pBlock->m_aBlock[i].m_iKey ){
            if(i==0){
                count = pBlock->m_aBlock[i].m_iUsedCnt - 1 ;
                pStart = m_poAcctItem + 1;
            }else{
                count = pBlock->m_aBlock[i].m_iUsedCnt - pBlock->m_aBlock[i-1].m_iTotalCnt;
                pStart = m_poAcctItem + pBlock->m_aBlock[i-1].m_iTotalCnt;                          
            }			
            outToTable(sTable, count, pStart,allCount);
			allCount -=count;
            iTotal += count;
        }
    }
    
    count = m_poAcctItemData->getCount();
       
    Log::log (0, "总共导出 %u 条", iTotal);
    
    if( count!= iTotal){
        Log::log (0, "[警告！]检测到导出过程中内存数据有变化，现有 %u 条", count);
    }    
        
    return 0;
#else

    return outToTable( sTable, m_poAcctItemData->getCount (), m_poAcctItem + 1,m_poAcctItemData->getCount ());
    
#endif

}


int AcctItemCtl::outToTable(char const *sTable, unsigned int count, AcctItemData *pStart,unsigned int AllCount)
{
    char sSQL[4096];

#ifdef ACCT_ITEM_AGGR_OFFER_ID
/*
    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, offer_id, item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
             ":item_class_id, :cor_billing_cycle_id)", sTable);
    #else 
*/
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
            "charge, acct_id, offer_inst_id, item_source_id, "
            "measure_type, state_date, offer_id, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
	":item_class_id)", sTable);
//    #endif

#else
/*
    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id,"
             ":cor_billing_cycle_id)", sTable);
    #else
*/ 
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id)", sTable);
//    #endif
#endif


    DEFINE_QUERY (qry);

    qry.setSQL (sSQL);

//--xgs     count = m_poAcctItemData->getCount ();
    int const batchnum = 20000;


//--xgs 	pStart = m_poAcctItem + 1;

#ifdef ACCT_ITEM_AGGR_OFFER_ID

    int * iOfferID = new int[batchnum];
    UserInfoInterface *pInterface = new UserInfoInterface ();

#endif

    int i(0);

    while (count) {

        qry.setParamArray (
            "acct_item_id", &(pStart->m_lAcctItemID), sizeof (AcctItemData));
        qry.setParamArray (
            "serv_id", &(pStart->m_lServID), sizeof (AcctItemData));
        qry.setParamArray (
            "billing_cycle_id", &(pStart->m_iBillingCycleID),
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "acct_item_type_id", &(pStart->m_iPayItemTypeID), 
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "charge", &(pStart->m_lValue), sizeof (AcctItemData));
        qry.setParamArray (
            "acct_id", &(pStart->m_lAcctID), sizeof (AcctItemData));
        qry.setParamArray (
            "offer_inst_id", &(pStart->m_lOfferInstID), sizeof (AcctItemData));
        qry.setParamArray (
           "item_source_id", &(pStart->m_iItemSourceID), sizeof (AcctItemData));
        qry.setParamArray (
            "measure_type", &(pStart->m_iPayMeasure), sizeof (AcctItemData));
        qry.setParamArray (
            "state_date", (char **)&(pStart->m_sStateDate), sizeof (AcctItemData),
			sizeof (pStart->m_sStateDate) );
        qry.setParamArray (
           "item_class_id", &(pStart->m_iItemClassID), sizeof (AcctItemData));
/*
        #ifdef ORG_BILLINGCYCLE
        qry.setParamArray("cor_billing_cycle_id", &(pStart->m_iOrgBillingCycleID),
                          sizeof(pStart->m_iOrgBillingCycleID) );
        #endif
*/
        int iNum = batchnum > count ? count : batchnum;

#ifdef ACCT_ITEM_AGGR_OFFER_ID

        int iTemp;
        for (iTemp=0; iTemp<iNum; iTemp++)  {
            ProdOfferIns *pIns = 
                pInterface->getOfferIns (pStart[iTemp].m_lOfferInstID);

	//temp
	//    if (strncmp(pStart[iTemp].m_sStateDate, "2007", 4)) {
	//	strcpy (pStart[iTemp].m_sStateDate, "20070713000000");
	//    }

            if (pIns) {
                iOfferID[iTemp] = pIns->m_iOfferID;
            } else {
                iOfferID[iTemp] = 0;
            }
        }

        qry.setParamArray (
            "offer_id", iOfferID, sizeof (int));

#endif
        qry.execute (iNum);
        qry.commit ();

        count -= iNum;
        pStart += iNum;
		AllCount -= iNum;

        i++;

        if (!count || !(i&1))
            Log::log (0, "剩余 %d条", AllCount);
    }

    qry.close ();

    return 0;
}

int AcctItemCtl::load(char const *sTable)
{
#ifdef MEM_REUSE_MODE
    if( m_poAcctItemData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        PutPetriRet(1, "内存正在清理，不能加载");
        LogDB::logDB(849, 1, "内存正在清理，不能加载");
        return 0;
    }
#endif
    
    char sSQL[4096];
//    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "select acct_item_id, serv_id, acct_id, offer_inst_id,"
        "billing_cycle_id, charge, measure_type, acct_item_type_id,"
/*
             " item_source_id, state_date, nvl(item_class_id, 0), cor_billing_cycle_id "
             "from %s", sTable);
    #else
    sprintf (sSQL, "select acct_item_id, serv_id, acct_id, offer_inst_id,"
             "billing_cycle_id, charge, measure_type, acct_item_type_id,"
*/
        " item_source_id, state_date, nvl(item_class_id, 0) "
        "from %s", sTable);
//    #endif
    DEFINE_QUERY (qry);

    unsigned int i;
    AcctItemData * p;
    int j (0);
    

    Log::log (0, "开始加载%s", sTable);
	m_poAcctItemData-> setRevokeFlag(1);
    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
    	
 
  	AcctItemAccuMgr::m_poDataLock->P ();
   	 AcctItemAccuMgr::m_poIndexLock->P();
   	 int iLock = qry.field (1).asLong ()& 7;
#ifdef USE_SERV_LOCK
     	m_poServLock[iLock]->P();
#endif  
#ifdef MEM_REUSE_MODE
        i = m_poAcctItemData->malloc ( qry.field (4).asInteger () );
        if(!i){
            revoke( m_poAcctItemData->getAutoRevokeKey(), false );
            i = m_poAcctItemData->malloc (qry.field (4).asInteger () );
        }
#else
        i = m_poAcctItemData->malloc ();
#endif

        if (!i) {
            Log::log (0, "请增加AcctItemCount");
            THROW (MBC_AcctItemAccu + 3);
        }

        p = m_poAcctItem + i;
        
        p->m_lAcctItemID = qry.field (0).asLong ();
        p->m_lServID     = qry.field (1).asLong ();
      
        p->m_lAcctID     = qry.field (2).asLong ();
        p->m_lOfferInstID= qry.field (3).asLong ();

        p->m_iBillingCycleID = qry.field (4).asInteger ();

        p->m_lValue      = qry.field (5).asLong ();
        p->m_iPayMeasure = qry.field (6).asInteger ();

        p->m_iPayItemTypeID  = qry.field (7).asInteger ();
        
        p->m_iItemSourceID = qry.field (8).asInteger ();

        strcpy (p->m_sStateDate, qry.field (9).asString ());

        p->m_iItemClassID = qry.field (10).asInteger ();
/*
        #ifdef ORG_BILLINGCYCLE
        p->m_iOrgBillingCycleID = qry.field(11).asInteger();
        #endif
*/		 
      AcctItemAccuMgr::m_poDataLock->V ();
   	  AcctItemAccuMgr::m_poIndexLock->V();
      m_poServIndex->get (p->m_lServID, &(p->m_iServNext));
      m_poServIndex->add (p->m_lServID, i);
#ifdef USE_SERV_LOCK
      m_poServLock[iLock]->V();
#endif  
      j++;	
      m_poAcctItemData-> setRevokeFlag(0);
      if (!(j%300000)) {
      	Log::log (0, "\t已加载%d\t条", j);
       }
    
    }
       
    qry.close ();

    Log::log (0, "\t已加载%d\t条", j);

    return j;
}

void AcctItemCtl::load(int iFlowID, int iOrgID, int iBillingCycleID)
{
#ifdef MEM_REUSE_MODE
    if( m_poAcctItemData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        PutPetriRet(1, "内存正在清理，不能加载");
        LogDB::logDB(941, 1, "内存正在清理，不能加载");
        return ;
    }
#endif

    char sSQL[4096];
    sprintf (sSQL, "select table_name from b_inst_table_list_cfg "
        "where (billflow_id=%d or %d=0) "
        "and   (org_id=%d or %d=0) "
        "and   (billing_cycle_id=%d) and table_type=11 "
        "and   nvl(op_role, 'W')='W' " , 
        iFlowID, iFlowID, iOrgID, iOrgID, iBillingCycleID);
    DEFINE_QUERY (qry);

    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        load (qry.field (0).asString ());
    }
    qry.close ();
}

int AcctItemCtl::outForIvpn(char const *sTable, int iSourceID1, int iSourceID2, int iSourceID3,int iSourceID4,int iSourceID5)
{
    unsigned int iTotal=0;
    unsigned int count=0;
    AcctItemData *pStart=0;

#ifdef MEM_REUSE_MODE


    if( m_poAcctItemData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return 0;
    }
    
    SHM_KEY_BLOCK_ARRAY pBlock[1];
    memset(pBlock, 0 , sizeof(SHM_KEY_BLOCK_ARRAY) );
    m_poAcctItemData->getBlockInfo(pBlock);
    
    for(int i=0; i< SHM_BLOCK_NUM; i++){
        if(  pBlock->m_aBlock[i].m_iKey ){
            if(i==0){
                count = pBlock->m_aBlock[i].m_iUsedCnt - 1 ;
                pStart = m_poAcctItem + 1;
            }else{
                count = pBlock->m_aBlock[i].m_iUsedCnt - pBlock->m_aBlock[i-1].m_iTotalCnt;
                pStart = m_poAcctItem + pBlock->m_aBlock[i-1].m_iTotalCnt;                          
            }
            outTableForIvpn(sTable, iSourceID1, count, pStart,iSourceID2,  iSourceID3, iSourceID4,iSourceID5);
            iTotal += count;
        }
    }    

    count = m_poAcctItemData->getCount();
       
    if( count!= iTotal){
        Log::log (0, "[警告！]检测到导出过程中内存数据有变化");
    }    

#else

    return outTableForIvpn(sTable, iSourceID1, m_poAcctItemData->getCount(), m_poAcctItem + 1,iSourceID2, iSourceID3, iSourceID4,iSourceID5);
#endif
  
    return 0;
    
}


int AcctItemCtl::outTableForIvpn(char const *sTable, int iSourceID1,
      unsigned int count, AcctItemData *p , int iSourceID2 , int iSourceID3 ,int iSourceID4 ,int iSourceID5)
{
    char sSQL[4096];

#ifdef ACCT_ITEM_AGGR_OFFER_ID
/*
    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, offer_id, item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
             ":item_class_id, :cor_billing_cycle_id)", sTable);
    #else
*/ 
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
            "charge, acct_id, offer_inst_id, item_source_id, "
            "measure_type, state_date, offer_id, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
	":item_class_id)", sTable);
//    #endif
#else
/* 
    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, measure_type, state_date, "
             "item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id,"
             ":cor_billing_cycle_id)", sTable);
    #else
*/
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
    "charge, acct_id, offer_inst_id, item_source_id, measure_type, state_date, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id)", sTable);
//    #endif
#endif


    DEFINE_QUERY (qry);

    qry.setSQL (sSQL);

    int const batchnum = 1000;
	
	//只加载IVPN的部分
	AcctItemData *dataArray = new AcctItemData[batchnum];
	

#ifdef ACCT_ITEM_AGGR_OFFER_ID

    int * iOfferID = new int[batchnum];
    UserInfoInterface *pInterface = new UserInfoInterface ();

#endif

	int j =0 , k = 1;
    while (k<=count) {

		for (j=0; k<=count && j<batchnum; k++){
		//
			
			if ((p[k].m_iItemSourceID == iSourceID1 
				||p[k].m_iItemSourceID == iSourceID2
				||p[k].m_iItemSourceID == iSourceID3
				||p[k].m_iItemSourceID == iSourceID4
				||p[k].m_iItemSourceID == iSourceID5)&& p[k].m_lValue != 0)
				dataArray[j++] = p[k];	
		}
		AcctItemData *pStart = dataArray;

        qry.setParamArray (
            "acct_item_id", &(pStart->m_lAcctItemID), sizeof (AcctItemData));
        qry.setParamArray (
            "serv_id", &(pStart->m_lServID), sizeof (AcctItemData));
        qry.setParamArray (
            "billing_cycle_id", &(pStart->m_iBillingCycleID),
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "acct_item_type_id", &(pStart->m_iPayItemTypeID), 
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "charge", &(pStart->m_lValue), sizeof (AcctItemData));
        qry.setParamArray (
            "acct_id", &(pStart->m_lAcctID), sizeof (AcctItemData));
        qry.setParamArray (
            "offer_inst_id", &(pStart->m_lOfferInstID), sizeof (AcctItemData));
        qry.setParamArray (
           "item_source_id", &(pStart->m_iItemSourceID), sizeof (AcctItemData));
        qry.setParamArray (
            "measure_type", &(pStart->m_iPayMeasure), sizeof (AcctItemData));
        qry.setParamArray (
            "state_date", (char **)&(pStart->m_sStateDate), sizeof (AcctItemData),
			sizeof (pStart->m_sStateDate) );
        qry.setParamArray (
           "item_class_id", &(pStart->m_iItemClassID), sizeof (AcctItemData));
	/*
        #ifdef ORG_BILLINGCYCLE
        qry.setParamArray("cor_billing_cycle_id", &(pStart->m_iOrgBillingCycleID),
                          sizeof(pStart->m_iOrgBillingCycleID));
        #endif
 */
        int iNum = j;

#ifdef ACCT_ITEM_AGGR_OFFER_ID

        int iTemp;
        for (iTemp=0; iTemp<iNum; iTemp++)  {
            ProdOfferIns *pIns = 
                pInterface->getOfferIns (pStart[iTemp].m_lOfferInstID);

            if (pIns) {
                iOfferID[iTemp] = pIns->m_iOfferID;
            } else {
                iOfferID[iTemp] = 0;
            }
        }

        qry.setParamArray (
            "offer_id", iOfferID, sizeof (int));

#endif
		if (iNum){
			qry.execute (iNum);
			qry.commit ();
		}
		Log::log (0, "已导出 %d 条...", iNum);

        if (k<=count)
            Log::log (0, "还有 %d 条未处理...", count-k+1);
    }

    qry.close ();
	delete[] dataArray;

    return 0;
}


void AcctItemCtl::showAcctItem(AcctItemData &oData){
    static bool bFirst = true;
    if ( bFirst ) {
        printf("帐目编号    用户标识    帐户标识    商品实例   ");
        #ifdef ORG_BILLINGCYCLE
        printf("原始帐务周期    ");
        #endif
        printf("帐务周期    定价来源    定价类型    帐目类型    帐目值\n");
        bFirst = false; 
    }
    printf("%10ld %12ld %12ld %10ld ",
           oData.m_lAcctItemID,
           oData.m_lServID,
           oData.m_lAcctID,
           oData.m_lOfferInstID);
    #ifdef ORG_BILLINGCYCLE
    printf("%10d", oData.m_iOrgBillingCycleID);
    #endif
    printf(" %10d %10d %10d %10d %10ld\n",
           oData.m_iBillingCycleID,
           oData.m_iItemSourceID,
           oData.m_iItemClassID,
           oData.m_iPayItemTypeID,
           oData.m_lValue);
}
#ifdef MEM_REUSE_MODE


void AcctItemBase::revoke(const int iBillingCycleID, bool bLocked)
{
    long lkey;
    unsigned int iOffset;
    unsigned int iLastOffset;
    
    long lCnt = 0;
    int  iServCnt = 0;
    bool bServ = false;
    
    if(0 == iBillingCycleID ){
        cout<<"错误：输入账期为0."<<endl;
        return; 
    }

    BillingCycleMgr bcm;
    BillingCycle * pbc = bcm.getBillingCycle(iBillingCycleID);
    if(pbc && strcmp( "10R", pbc->getState() )==0  ){
        cout<<"告警：10R状态的账期不能被实时卸载。"<<endl;
        return;
    }
    
    if( !m_poAcctItemData->exist () ){
        cout<<"共享内存不存在."<<endl;
        return; 
    }


    ///先把索引区的给去掉
    if (m_poServIndex->exist ()) {
        
        m_poAcctItemData->setRevokeFlag(1);
        
        sleep(1);
        AcctItemAccuMgr * pMgr =0;
        if(!bLocked){
            pMgr = new AcctItemAccuMgr();
            pMgr->lockAllLock();
        }

        SHMIntHashIndex_KR::Iteration iter = m_poServIndex->getIteration();
        while( iter.next(lkey, iOffset) ){
            bServ = false;
            while( iOffset
              && iBillingCycleID == m_poAcctItem[iOffset].m_iBillingCycleID){

                iOffset =  m_poAcctItem[iOffset].m_iServNext;
                iter.setValue( iOffset );
                lCnt++;
                if(!bServ){
                    iServCnt++;
                    bServ = true;
                }
            }
            
            if( !iOffset )
                continue;
            
            iLastOffset = iOffset;
            iOffset =  m_poAcctItem[iOffset].m_iServNext;
            while( iOffset ){
                if(iBillingCycleID == m_poAcctItem[iOffset].m_iBillingCycleID){
                    iOffset =  m_poAcctItem[iOffset].m_iServNext;
                    m_poAcctItem[iLastOffset].m_iServNext =  iOffset;
                    lCnt++;
                    if(!bServ){
                        iServCnt++;
                        bServ = true;
                    }
                    continue;
                }
                iLastOffset = iOffset;
                iOffset =  m_poAcctItem[iOffset].m_iServNext;
            }
            
        }

        ////数据区卸掉重用        
        int iCnt2 = m_poAcctItemData->revoke ( iBillingCycleID );
    
    
        if(!bLocked && pMgr) pMgr->freeAllLock();
        m_poAcctItemData->setRevokeFlag(0);
        
        if(!bLocked){
            cout<<"共卸载 "<< lCnt <<" 条数据,涉及用户数 "<< iServCnt <<"。" << endl;
            cout << "key: "<< iBillingCycleID <<" 卸载 "<<iCnt2<< " 块数据区." << endl;
        }else{
            Log::log(0, "共卸载%ld条数据,涉及用户数%d,key:%d卸载%d块数据区.",
                lCnt,iServCnt,iBillingCycleID,iCnt2);
        }
        
        if(pMgr){ 
            delete pMgr;  
            pMgr = 0;
        }

    }
 

    
}


void AcctItemCtl::showDetail(bool bClearRF)
{
    if( !m_poAcctItemData->exist () ){
        cout<<"共享内存不存在."<<endl;
        return; 
    }
    
    m_poAcctItemData->showDetail();
    if(bClearRF)m_poAcctItemData->setRevokeFlag(0);
    
}

#endif

//add by zhaoziwei
int AcctItemCtl::out_new(char const *sTable)
{
	unsigned int count = m_poAcctItemData->getCount();
	m_iAllCount=count;
  AcctItemData *pStart = m_poAcctItem + 1;
  
	m_pMgrInstTable = new InstTableListMgrKerEx(sTable);
	if(m_pMgrInstTable->m_iCnt==1)
		return outToTableEx(m_pMgrInstTable->m_sTableName,pStart,count);
	else
	{
		Environment::useUserInfo();
	  while(count){
	  	CAcctData* pCache;
	  	int orgId = G_PUSERINFO->getServOrg(pStart->m_lServID,pStart->m_sStateDate);
	  	int billintCycleId=pStart->m_iBillingCycleID;
	  	
	  	int level=G_PORGMGR->getLevel(orgId);
	  	if(level>m_pMgrInstTable->m_iMinOrgLevel)
	  		orgId=G_PORGMGR->getLevelParentID(orgId,m_pMgrInstTable->m_iMinOrgLevel);
	  	
	  	pCache=GetAcctData(orgId,billintCycleId);
	  	pCache->AddData(pStart);
	  	
		 	count -= 1;
	  	pStart += 1;
	  }
		
		dealAcctData();
	}
	
	if(m_pMgrInstTable)
		delete m_pMgrInstTable;
}

bool AcctItemCtl::dealAcctData()
{
	map<string,CAcctData*>::iterator it = m_mapAcctData.begin();
	for(; it!= m_mapAcctData.end();it++)
	{
		char str[50];
		char *p;
		strcpy(str,(it->first).c_str());
		p = strtok(str,"_");
		int orgid=atoi(p);
		p = strtok(NULL,"_");
		int iBillingCycle=atoi(p);
		char sTable[64]={0};
		if(!m_pMgrInstTable->getTableName(orgid,iBillingCycle,sTable))
		{
			Log::log(0,"无法找到BillingCycleID=%d,TableType=11,OrgID=%d 对应的表",iBillingCycle,orgid);
    	assert(false);
    	return false;
		}
		
		unsigned int count = it->second->getCount();
		AcctItemData *m_pAcctDataOrg = (AcctItemData *)malloc(sizeof(AcctItemData)*count);
		memset(m_pAcctDataOrg,0,sizeof(AcctItemData)*count);
    AcctItemData *pTemp = m_pAcctDataOrg;
    vector<AcctItemData*>::iterator iter = it->second->m_listAcctItemData.begin();
    for(; iter!= it->second->m_listAcctItemData.end(); iter++)
    {
        //复制入库缓存
        memcpy(m_pAcctDataOrg,(*iter),sizeof(AcctItemData));
        m_pAcctDataOrg = m_pAcctDataOrg+1;//(char*)(m_pAcctDataOrg)+sizeof(AcctItemData);
    }
    m_pAcctDataOrg = pTemp;
    
    outToTableEx(sTable,m_pAcctDataOrg,count);
    
    if(m_pAcctDataOrg)
			free(m_pAcctDataOrg);
		if(it->second)
    	delete it->second;
	}
	
	return true;
}

CAcctData* AcctItemCtl::GetAcctData(int nOrgID,int iBillingCycleID)
{
		char sCacheKey[128] ={0};
		sprintf(sCacheKey,"%d_%d",nOrgID,iBillingCycleID);
		string strCacheKey=sCacheKey;
    if(m_mapAcctData.empty())
    {
        CAcctData* pNewCache = new CAcctData;
        m_mapAcctData[strCacheKey] = pNewCache;
        return pNewCache;
    }
    else
    {
        map<string,CAcctData*>::iterator it = m_mapAcctData.find(strCacheKey);
        if(it == m_mapAcctData.end())
        {
            CAcctData* pNewCache = new CAcctData;
            m_mapAcctData[strCacheKey] = pNewCache;
            return pNewCache;
        }
        else
        {
            return ((it)->second);
        }
    }
}

int AcctItemCtl::outToTableEx(char const *sTable,AcctItemData *pAcctData,int count)
{
    char sSQL[4096];

#ifdef ACCT_ITEM_AGGR_OFFER_ID

    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, offer_id, item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
             ":item_class_id, :cor_billing_cycle_id)", sTable);
    #else 
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
            "charge, acct_id, offer_inst_id, item_source_id, "
            "measure_type, state_date, offer_id, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :offer_id, "
	":item_class_id)", sTable);
    #endif

#else
    #ifdef ORG_BILLINGCYCLE
    sprintf (sSQL, "insert into %s "
             "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, item_class_id, cor_billing_cycle_id) "
             "values (:acct_item_id, :serv_id, :billing_cycle_id, "
             ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
             ":item_source_id, :measure_type, "
             "to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id,"
             ":cor_billing_cycle_id)", sTable);
    #else 
    sprintf (sSQL, "insert into %s "
        "(acct_item_id, serv_id, billing_cycle_id, acct_item_type_id, "
             "charge, acct_id, offer_inst_id, item_source_id, "
             "measure_type, state_date, item_class_id) "
        "values (:acct_item_id, :serv_id, :billing_cycle_id, "
        ":acct_item_type_id, :charge, :acct_id, :offer_inst_id, "
        ":item_source_id, :measure_type, "
	"to_date(:state_date, 'yyyymmddhh24miss'), :item_class_id)", sTable);
    #endif
#endif


    DEFINE_QUERY (qry);

    qry.setSQL (sSQL);

    //unsigned int allCount = m_poAcctItemData->getCount ();
    int const batchnum = 20000;


    //AcctItemData *pStart = m_poAcctItem + 1;
		AcctItemData *pStart = pAcctData;
		
#ifdef ACCT_ITEM_AGGR_OFFER_ID

    int * iOfferID = new int[batchnum];
    UserInfoInterface *pInterface = new UserInfoInterface ();

#endif

    int i(0);

    while (count) {

        qry.setParamArray (
            "acct_item_id", &(pStart->m_lAcctItemID), sizeof (AcctItemData));
        qry.setParamArray (
            "serv_id", &(pStart->m_lServID), sizeof (AcctItemData));
        qry.setParamArray (
            "billing_cycle_id", &(pStart->m_iBillingCycleID),
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "acct_item_type_id", &(pStart->m_iPayItemTypeID), 
                                                        sizeof (AcctItemData));
        qry.setParamArray (
            "charge", &(pStart->m_lValue), sizeof (AcctItemData));
        qry.setParamArray (
            "acct_id", &(pStart->m_lAcctID), sizeof (AcctItemData));
        qry.setParamArray (
            "offer_inst_id", &(pStart->m_lOfferInstID), sizeof (AcctItemData));
        qry.setParamArray (
           "item_source_id", &(pStart->m_iItemSourceID), sizeof (AcctItemData));
        qry.setParamArray (
            "measure_type", &(pStart->m_iPayMeasure), sizeof (AcctItemData));
        qry.setParamArray (
            "state_date", (char **)&(pStart->m_sStateDate), sizeof (AcctItemData),
			sizeof (pStart->m_sStateDate) );
        qry.setParamArray (
           "item_class_id", &(pStart->m_iItemClassID), sizeof (AcctItemData));

        #ifdef ORG_BILLINGCYCLE
        qry.setParamArray("cor_billing_cycle_id", &(pStart->m_iOrgBillingCycleID),
                          sizeof(pStart->m_iOrgBillingCycleID) );
        #endif
        int iNum = batchnum > count ? count : batchnum;

#ifdef ACCT_ITEM_AGGR_OFFER_ID

        int iTemp;
        for (iTemp=0; iTemp<iNum; iTemp++)  {
            ProdOfferIns *pIns = 
                pInterface->getOfferIns (pStart[iTemp].m_lOfferInstID);

	//temp
	//    if (strncmp(pStart[iTemp].m_sStateDate, "2007", 4)) {
	//	strcpy (pStart[iTemp].m_sStateDate, "20070713000000");
	//    }

            if (pIns) {
                iOfferID[iTemp] = pIns->m_iOfferID;
            } else {
                iOfferID[iTemp] = 0;
            }
        }

        qry.setParamArray (
            "offer_id", iOfferID, sizeof (int));

#endif
        qry.execute (iNum);
        qry.commit ();

        count -= iNum;
        pStart += iNum;
				m_iAllCount -= iNum;
        i++;

        if (!m_iAllCount || !(i&1))
            Log::log (0, "剩余 %d条", m_iAllCount);
    }

    qry.close ();
		
    return 0;
}
long AcctItemCtl::getAllfee(int iBillingCycleID)
{
    long lfee = 0;
	
#ifdef MEM_REUSE_MODE
	
    unsigned int count=0;
    AcctItemData *pStart=0;
    
    if( m_poAcctItemData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return 0;
    }

    SHM_KEY_BLOCK_ARRAY pBlock[1];
    memset(pBlock, 0 , sizeof(SHM_KEY_BLOCK_ARRAY) );
    m_poAcctItemData->getBlockInfo(pBlock);

    for(int i=0; i< SHM_BLOCK_NUM; i++){
        if(  pBlock->m_aBlock[i].m_iKey ){
            if(i==0){
                count = pBlock->m_aBlock[i].m_iUsedCnt - 1 ;
                pStart = m_poAcctItem + 1;
            }else{
                count = pBlock->m_aBlock[i].m_iUsedCnt - pBlock->m_aBlock[i-1].m_iTotalCnt;
                pStart = m_poAcctItem + pBlock->m_aBlock[i-1].m_iTotalCnt;                          
            }
            
            for(int j=1;j<=count;j++){
           	    if((pStart+j)->m_iBillingCycleID == iBillingCycleID)
           	       lfee = lfee + (pStart+j)->m_lValue; 	
           }
        }
    }    
#else
    unsigned int count = m_poAcctItemData->getCount ();
		AcctItemData *pInfo = m_poAcctItem;
		for(int i=1;i<=count;i++){
			if(pInfo[i].m_iBillingCycleID == iBillingCycleID)
			  lfee = lfee + pInfo[i].m_lValue; 	
		}
#endif 
		
    return lfee;
}

long AcctItemCtl::getAllfee(int iBillingCycleID,char *sDate)
{
    long lfee = 0;
    unsigned int count = m_poAcctItemData->getCount ();
		AcctItemData *pInfo = m_poAcctItem;
		for(int i=1;i<=count;i++){
			if(pInfo[i].m_iBillingCycleID == iBillingCycleID 
			 &&strcmp(pInfo[i].m_sStateDate,sDate) <=0)
					lfee = lfee + pInfo[i].m_lValue; 	
		}		
    return lfee;
}
bool AcctItemCtl::getConnProcNum(){
    bool bResult = true;

    if (( m_poAcctItemData->connnum() > 1 )
        || ( m_poServIndex->connnum() > 1 )
       ) {
        Log::log(0, "检测到其他进程在使用总账，现在还不能加载，请稍候再试。");
    } else {
        bResult = false;
    }

    return bResult;
}
