/*VER: 5*/ 
// Copyright (c) 2007,联创科技股份有限公司电信事业部
// All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "CommonMacro.h"
#include "../lib/trie.h"
#include "../lib/fee_shm.h"
#include "Environment.h"
#include "Process.h"
#include "Log.h"
#include "MBC.h"
#include "AcctItemMgr.h"
#include "EventTypeMgr.h"
#include "InstTableListMgr.h"
#include "ProdOfferAggr.h"
#include "AcctItemMgr.h"
#include "Value.h"
#include "PricingEngine.h"

    
/*
#define SHM_PROD_OFFER_COUNT 100000
#define SHM_PROD_OFFER_DETAIL_COUNT 100000
*/

#define AGGR_PARAM_SEGMENT "PRODOFFERAGGR"
#define AGGR_PARAM_DATA  "TOTALDATA"
#define AGGR_PARAM_DETAIL  "DETAILDATA"



bool ProdOfferAggrAttach::m_bAttached= false;
bool ProdOfferAggrAttach::m_bDetailAttached=false;
bool ProdOfferAggrAttach::m_bAttachIndex=false;
    
#ifdef MEM_REUSE_MODE
SHMData_B<ProdOfferAggrData> * ProdOfferAggrAttach::m_poProdOfferAggrData=0;
SHMData_B<ProdOfferDetailData> * ProdOfferAggrAttach::m_poProdOfferDetailData=0;
SHMIntHashIndex_KR * ProdOfferAggrAttach::m_poProdOfferAggrIndex=0;

#else

SHMData<ProdOfferAggrData> * ProdOfferAggrAttach::m_poProdOfferAggrData=0;
SHMData<ProdOfferDetailData> * ProdOfferAggrAttach::m_poProdOfferDetailData=0;
SHMIntHashIndex * ProdOfferAggrAttach::m_poProdOfferAggrIndex=0;

#endif
    
CSemaphore * ProdOfferDisctAggr::m_poAggrLock = 0;
CSemaphore * ProdOfferDisctAggr::m_poAggrDetailLock= 0;
CSemaphore * ProdOfferDisctAggr::m_poAggrIndexLock= 0;
SHMIntHashIndex*  ProdOfferAggrAttach::m_poProdOfferAggrSeqIndex=0;
bool	ProdOfferAggrAttach::m_bAttachSeqIndex=false; 
ProdOfferAggrAttach::ProdOfferAggrAttach()//Attach
{
    unsigned long ProdOfferAggrBufKey,ProdOfferDetailBufKey,ProdOfferDisctIndexKey;

    char sBuff[10];
        int iDataCount=0,iDetailCount=0;
        
        bool bAttached,bDetailAttached,bAttachIndex;

    if(m_bAttached==true && m_bDetailAttached == true && m_bAttachIndex == true)
        return;
    bAttached = true;
    bDetailAttached = true;
    bAttachIndex=true;

    ProdOfferDetailBufKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTBUF");
    ProdOfferAggrBufKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERTOTALBUF");
    ProdOfferDisctIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTINDEX");

#ifdef MEM_REUSE_MODE
    m_poProdOfferAggrData = new SHMData_B<ProdOfferAggrData> (ProdOfferAggrBufKey); 

#else
    m_poProdOfferAggrData = new SHMData<ProdOfferAggrData> (ProdOfferAggrBufKey); 

#endif
        if (!(m_poProdOfferAggrData)) { 
                THROW(MBC_ProdOfferAggr+11); 
        }
         if (!(m_poProdOfferAggrData->exist())) {
                bAttached = false;
        }
            
#ifdef MEM_REUSE_MODE
        m_poProdOfferDetailData = new SHMData_B<ProdOfferDetailData> (ProdOfferDetailBufKey); 

#else
        m_poProdOfferDetailData = new SHMData<ProdOfferDetailData> (ProdOfferDetailBufKey); 

#endif
        if (!(m_poProdOfferDetailData)) { 
                THROW(MBC_ProdOfferAggr+12); 
        }
        if (!(m_poProdOfferDetailData->exist())) { 
                bDetailAttached = false;
        }
        
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrIndex = new SHMIntHashIndex_KR (ProdOfferDisctIndexKey);

#else
        m_poProdOfferAggrIndex = new SHMIntHashIndex (ProdOfferDisctIndexKey);

#endif

        if (!(m_poProdOfferAggrIndex)) {
            THROW(MBC_ProdOfferAggr+13); 
        } 
        if (!(m_poProdOfferAggrIndex->exist())) {
                bAttachIndex = false;
        }
    //sunjy
	bool bAttachSeqIndex =false;   	
 unsigned long ProdOfferDisctSeqIndexKey ;
    ProdOfferDisctSeqIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTSEQINDEX"); 
	m_poProdOfferAggrSeqIndex = new SHMIntHashIndex (ProdOfferDisctSeqIndexKey);
    if (!(m_poProdOfferAggrSeqIndex)) {
        THROW(MBC_ProdOfferAggr+14); 
    }
    if (!(m_poProdOfferAggrSeqIndex->exist())) {
        bAttachSeqIndex = false;
    }
        if(bAttached == true)
            m_bAttached = true;
        if(bDetailAttached == true)
            m_bDetailAttached = true;
        if(bAttachIndex == true)
            m_bAttachIndex = true;
	if(bAttachSeqIndex == true )
		 m_bAttachSeqIndex = true;
}
void ProdOfferAggrAttach::FreeProdOfferAggrShm()
{
    if(m_bAttached==true){
        delete m_poProdOfferAggrData;
        m_poProdOfferAggrData= 0;
    }
    if(m_bDetailAttached==true){
        delete m_poProdOfferDetailData;
        m_poProdOfferDetailData= 0;
    }
    if(m_bAttachIndex==true)
    {
        delete m_poProdOfferAggrIndex;
        m_poProdOfferAggrIndex= 0;
    }
    if(m_bAttachSeqIndex==true)
    {
        delete m_poProdOfferAggrSeqIndex;
        m_poProdOfferAggrSeqIndex= 0;
    }    
}

void ProdOfferAggrAttach::ProdOfferAggrReset()
{
    unsigned long ProdOfferDisctIndexKey;
    char sBuff[10];
        int iDataCount=0;
    
    if(m_bAttached == true)
    {
        m_poProdOfferAggrData->reset();
    }
    if(m_bDetailAttached == true)
    {
        m_poProdOfferDetailData->reset();
    }
    if(m_bAttachIndex == true)
    {
        m_poProdOfferAggrIndex->remove();
        ProdOfferDisctIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTINDEX");


#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrIndex = new SHMIntHashIndex_KR (ProdOfferDisctIndexKey);
#else
        m_poProdOfferAggrIndex = new SHMIntHashIndex (ProdOfferDisctIndexKey);
#endif

        if (!(m_poProdOfferAggrIndex)) {
            FreeProdOfferAggrShm();
            THROW(MBC_ProdOfferAggr+1); 
        } 
        if (!(m_poProdOfferAggrIndex->exist())) {
                m_bAttachIndex = false;
        }
        if(m_bAttachIndex == false){
            if(!ParamDefineMgr::getParam(AGGR_PARAM_SEGMENT, AGGR_PARAM_DATA, sBuff))
            {
                THROW(MBC_ProdOfferAggr+9);
            }
            iDataCount = atoi(sBuff);
            m_poProdOfferAggrIndex->create (iDataCount);
    	}
    }
    //sunjy
    unsigned long ProdOfferDisctSeqIndexKey ;
    if(m_bAttachSeqIndex == true)
    {
        m_poProdOfferAggrSeqIndex->remove();
        ProdOfferDisctSeqIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTSEQINDEX");
        m_poProdOfferAggrSeqIndex = new SHMIntHashIndex (ProdOfferDisctSeqIndexKey);
    	if (!(m_poProdOfferAggrSeqIndex)) {
    	    FreeProdOfferAggrShm();
    	    THROW(MBC_ProdOfferAggr+1); 
    	} 
    	if (!(m_poProdOfferAggrSeqIndex->exist())) {
    		m_bAttachSeqIndex = false;
    	}
    	if(m_bAttachSeqIndex == false){
    	    if(!ParamDefineMgr::getParam(AGGR_PARAM_SEGMENT, AGGR_PARAM_DATA, sBuff))
            {
                THROW(MBC_ProdOfferAggr+9);
            }
            iDataCount = atoi(sBuff);
    	    m_poProdOfferAggrSeqIndex->create (iDataCount);
        }
    }
}

ProdOfferDisctAggr::ProdOfferDisctAggr():ProdOfferAggrAttach()
{
        if(m_bAttached == false)
    {
        THROW(MBC_ProdOfferAggr+14); 
    }
    if(m_bDetailAttached == false)
    {
        THROW(MBC_ProdOfferAggr+15); 
    }
    if(m_bAttachIndex == false)
    {
        THROW(MBC_ProdOfferAggr+16); 
    }
        
    if (!m_pItemDisctSeq) 
        m_pItemDisctSeq  = new SeqMgrExt ("ITEM_DISCT_SEQ", 10000);     

        if (!m_pItemDisctSeq) {
        THROW (MBC_ProdOfferAggr+17);
    }
    
    char sTemp[32];

    if (!m_poAggrLock) {
        m_poAggrLock = new CSemaphore ();
        sprintf (sTemp, "%d", IpcKeyMgr::getIpcKey(-1,"SEM_AGGRLOCK"));
        m_poAggrLock->getSem (sTemp, 1, 1);
    }

    if (!m_poAggrDetailLock) {
        m_poAggrDetailLock = new CSemaphore ();
        sprintf (sTemp, "%d", IpcKeyMgr::getIpcKey(-1,"SEM_AGGRDETAILLOCK"));
        m_poAggrDetailLock->getSem (sTemp, 1, 1);
    }

    if (!m_poAggrIndexLock) {
        m_poAggrIndexLock = new CSemaphore ();
        sprintf (sTemp, "%d", IpcKeyMgr::getIpcKey(-1,"SEM_AGGRINDEXLOCK"));
        m_poAggrIndexLock->getSem (sTemp, 1, 1);
    }
}

ProdOfferDisctAggr::~ProdOfferDisctAggr()
{
}

void ProdOfferDisctAggr::doProdOfferDisct(
ProdOfferTotalData * poAcctItem,
vector<ProdOfferAggrData > &v_ProdOfferAggr,
vector<ProdOfferDetailDataEx > &v_ProdOfferDetailData)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferDetailData * pProdOfferDetailCur;

        char sProdOfferID[13];
        unsigned int k=0;
        long lPre,lDetailPre,i,j,t,m;
        long lChangeFee;
        
        ProdOfferAggrData aProdOfferAggrData;
        ProdOfferDetailDataEx aProdOfferDetailDataEx;

    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    
        if (!m_poProdOfferAggrIndex->get(poAcctItem->m_lProdOfferID, &k)) {

            m_poAggrLock->P();
#ifdef MEM_REUSE_MODE
        i = m_poProdOfferAggrData->malloc (poAcctItem->m_iBillingCycleID );
        if(!i){
            m_poAggrDetailLock->P();
            m_poAggrIndexLock->P ();
            revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
            m_poAggrDetailLock->V();
            m_poAggrIndexLock->V ();
            i = m_poProdOfferAggrData->malloc (poAcctItem->m_iBillingCycleID );
        }

#else
                i = m_poProdOfferAggrData->malloc();
#endif
        m_poAggrLock->V();

        if (!i) {
            Log::log (0, "取实时优惠共享内存地址不足\n");
            THROW(MBC_ProdOfferAggr+21);
        }

        m_poAggrDetailLock->P();
#ifdef MEM_REUSE_MODE
        j = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
        if(!j){
            m_poAggrLock->P();
            m_poAggrIndexLock->P ();
            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
            m_poAggrLock->V();
            m_poAggrIndexLock->V ();
            j = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
        }

#else
        j = m_poProdOfferDetailData->malloc();
#endif
        m_poAggrDetailLock->V();

        if(!j){
            Log::log (0, "取实时优惠明细共享内存地址不足\n");
            THROW(MBC_ProdOfferAggr+22);    
        }
                if(poAcctItem->m_lItemDisctSeq>0)
                        pProdOfferAggrCur[i].m_lItemDisctSeq = poAcctItem->m_lItemDisctSeq;
                else
                pProdOfferAggrCur[i].m_lItemDisctSeq = m_pItemDisctSeq->getNextVal ();
        pProdOfferAggrCur[i].m_lProdOfferID = poAcctItem->m_lProdOfferID;
        pProdOfferAggrCur[i].m_iBillingCycleID = poAcctItem->m_iBillingCycleID;
        pProdOfferAggrCur[i].m_iProdOfferCombine = 
                                            poAcctItem->m_iProdOfferCombine;
        pProdOfferAggrCur[i].m_iPricingMeasure = poAcctItem->m_iPricingMeasure;
        pProdOfferAggrCur[i].m_iItemGroupID = poAcctItem->m_iItemGroupID;
        pProdOfferAggrCur[i].m_iInType = poAcctItem->m_iInType;
        pProdOfferAggrCur[i].m_lInValue = poAcctItem->m_lInValue;
        pProdOfferAggrCur[i].m_lValue = 0;
        if(poAcctItem->m_bInOutType == true){
            pProdOfferAggrCur[i].m_lValue = poAcctItem->m_lValue;
            pProdOfferAggrCur[i].m_lDisctValue= 0 ;
        }else{
                pProdOfferAggrCur[i].m_lValue = 0;
            pProdOfferAggrCur[i].m_lDisctValue= poAcctItem->m_lValue ;
        }
        pProdOfferAggrCur[i].m_lAggrOfferSet = -1;
        pProdOfferAggrCur[i].m_lDetailOfferSet = j;
        
        #ifdef ORG_BILLINGCYCLE
           pProdOfferAggrCur[i].m_iOrgBillingCycleID = poAcctItem->m_iOrgBillingCycleID;
        #endif
        pProdOfferDetailCur[j].m_lServID = poAcctItem->m_lServID;
        pProdOfferDetailCur[j].m_iAcctItemTypeID = 
                                            poAcctItem->m_iAcctItemTypeID;

        //pProdOfferDetailCur[j].m_bInOutType = poAcctItem->m_bInOutType;
        if(poAcctItem->m_bInOutType == true){
            pProdOfferDetailCur[j].m_lInCharge = poAcctItem->m_lValue;
            pProdOfferDetailCur[j].m_lOutCharge = 0;
        }else{
            pProdOfferDetailCur[j].m_lOutCharge = poAcctItem->m_lValue;
            pProdOfferDetailCur[j].m_lInCharge = 0;
        }
        pProdOfferDetailCur[j].m_lOffset = -1;

        //sunjy
        pProdOfferDetailCur[j].m_lOffsetCur=pProdOfferAggrCur[i].m_lItemDisctSeq  ;
        pProdOfferDetailCur[j].m_iBIllingCycleID=pProdOfferAggrCur[i].m_iBillingCycleID ;        
        m_poAggrIndexLock->P();
                m_poProdOfferAggrIndex->add (poAcctItem->m_lProdOfferID, i);
                m_poAggrIndexLock->V();

                //if(poAcctItem->m_bInOutType == false)
                {
                //增加变量输出
                memcpy(&aProdOfferAggrData, &pProdOfferAggrCur[i],
                                                sizeof(ProdOfferAggrData));

                memcpy(&aProdOfferDetailDataEx, &pProdOfferDetailCur[j],
                                                sizeof(ProdOfferDetailData));

                aProdOfferDetailDataEx.m_bNewRec = true;
                aProdOfferDetailDataEx.m_lOffset = 
                                        pProdOfferAggrCur[i].m_lItemDisctSeq;
            aProdOfferDetailDataEx.m_lOfferInstID = poAcctItem->m_lProdOfferID;

                aProdOfferAggrData.m_lAggrOfferSet=1;

            v_ProdOfferDetailData.push_back(aProdOfferDetailDataEx);

            v_ProdOfferAggr.push_back (aProdOfferAggrData);
        }
        }
        else
        {
            t=k;
            while(t>0){
                lPre = t;
                if(pProdOfferAggrCur[lPre].m_lProdOfferID == poAcctItem->m_lProdOfferID 
                && pProdOfferAggrCur[lPre].m_iBillingCycleID == poAcctItem->m_iBillingCycleID
                && pProdOfferAggrCur[lPre].m_iProdOfferCombine == poAcctItem->m_iProdOfferCombine
                && pProdOfferAggrCur[lPre].m_iPricingMeasure == poAcctItem->m_iPricingMeasure
                && pProdOfferAggrCur[lPre].m_iItemGroupID == poAcctItem->m_iItemGroupID
                && pProdOfferAggrCur[lPre].m_iInType == poAcctItem->m_iInType
                        && pProdOfferAggrCur[lPre].m_lInValue == poAcctItem->m_lInValue
			  #ifdef ORG_BILLINGCYCLE 
			    && pProdOfferAggrCur[lPre].m_iOrgBillingCycleID == poAcctItem->m_iOrgBillingCycleID
			  #endif			
                        ){

                memcpy(&aProdOfferAggrData,&pProdOfferAggrCur[lPre],
                                                    sizeof(ProdOfferAggrData));
                                if(poAcctItem->m_bInOutType == true) {
                        if(poAcctItem->m_bNewRec == true) {
                            pProdOfferAggrCur[lPre].m_lValue += 
                                                        poAcctItem->m_lValue;
                        } else {
                            pProdOfferAggrCur[lPre].m_lValue = poAcctItem->m_lValue;
                        }

                        aProdOfferAggrData.m_lValue = poAcctItem->m_lValue;
                        aProdOfferAggrData.m_lDisctValue = 0 ;
                        
                        m = pProdOfferAggrCur[lPre].m_lDetailOfferSet;

                        while(m>0){
                            lDetailPre = m;

                            if(pProdOfferDetailCur[lDetailPre].m_lServID == poAcctItem->m_lServID
                            && pProdOfferDetailCur[lDetailPre].m_iAcctItemTypeID == poAcctItem->m_iAcctItemTypeID){
                                if(poAcctItem->m_bNewRec == true){
                                    pProdOfferDetailCur[lDetailPre].m_lInCharge += poAcctItem->m_lValue;
                                }else{
                                    pProdOfferDetailCur[lDetailPre].m_lInCharge = poAcctItem->m_lValue;
                                }

                                memcpy(&aProdOfferDetailDataEx ,
                                        &pProdOfferDetailCur[lDetailPre],
                                        sizeof(ProdOfferDetailData));

                                aProdOfferDetailDataEx.m_lInCharge = 
                                                        poAcctItem->m_lValue;
                                aProdOfferDetailDataEx.m_lOutCharge = 0;
                                aProdOfferDetailDataEx.m_lOffset = 
                                        pProdOfferAggrCur[lPre].m_lItemDisctSeq;
                                aProdOfferDetailDataEx.m_bNewRec = false;
                            aProdOfferDetailDataEx.m_lOfferInstID = 
                                                    poAcctItem->m_lProdOfferID;
                                break;
                            }

                            m = pProdOfferDetailCur[lDetailPre].m_lOffset;
                        }

                        if(m<=0){

                            m_poAggrDetailLock->P();
#ifdef MEM_REUSE_MODE
                        m = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
                        if(!m){
                            m_poAggrLock->P();
                            m_poAggrIndexLock->P ();
                            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
                            m_poAggrLock->V();
                            m_poAggrIndexLock->V ();
                            m = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
                        }

#else
                            m = m_poProdOfferDetailData->malloc();
#endif
                            m_poAggrDetailLock->V();

                            if (!m) {
                            Log::log (0, "取实时优惠明细共享内存地址不足\n");
                                THROW(MBC_ProdOfferAggr+23);
                        }

                        pProdOfferDetailCur[m].m_lServID = poAcctItem->m_lServID;
                            pProdOfferDetailCur[m].m_iAcctItemTypeID = poAcctItem->m_iAcctItemTypeID;
                            pProdOfferDetailCur[m].m_lInCharge=poAcctItem->m_lValue;
                            pProdOfferDetailCur[m].m_lOutCharge = 0;
                            pProdOfferDetailCur[lDetailPre].m_lOffset=m;

                            memcpy(&aProdOfferDetailDataEx,
                                    &pProdOfferDetailCur[m],
                                    sizeof(ProdOfferDetailData));

                            aProdOfferDetailDataEx.m_lInCharge = 
                                                        poAcctItem->m_lValue;
                            aProdOfferDetailDataEx.m_lOffset =
                                     pProdOfferAggrCur[lPre].m_lItemDisctSeq;
                            aProdOfferDetailDataEx.m_bNewRec = true;
                        }
                        //增加变量输出
                           
                    aProdOfferAggrData.m_lAggrOfferSet=0;

                    aProdOfferDetailDataEx.m_lOfferInstID = 
                                     poAcctItem->m_lProdOfferID;

                        v_ProdOfferDetailData.push_back(aProdOfferDetailDataEx);
                        v_ProdOfferAggr.push_back (aProdOfferAggrData);
                    }else{
                        if(poAcctItem->m_bNewRec == true) {
                            pProdOfferAggrCur[lPre].m_lDisctValue += 
                                                        poAcctItem->m_lValue;
                        } else {
                            pProdOfferAggrCur[lPre].m_lDisctValue = poAcctItem->m_lValue;
                        }

                        aProdOfferAggrData.m_lDisctValue = poAcctItem->m_lValue;
                        aProdOfferAggrData.m_lValue = 0 ;
                        
                        m = pProdOfferAggrCur[lPre].m_lDetailOfferSet;
                        while(m>0){
                            lDetailPre = m;
                            if(pProdOfferDetailCur[lDetailPre].m_lServID == poAcctItem->m_lServID
                            && pProdOfferDetailCur[lDetailPre].m_iAcctItemTypeID == poAcctItem->m_iAcctItemTypeID){
                                memcpy(&aProdOfferDetailDataEx,&pProdOfferDetailCur[lDetailPre],sizeof(ProdOfferDetailData));




                                lChangeFee = poAcctItem->m_lValue - pProdOfferDetailCur[lDetailPre].m_lOutCharge;
                                
                                pProdOfferDetailCur[lDetailPre].m_lOutCharge = poAcctItem->m_lValue;
                                aProdOfferDetailDataEx.m_lOffset = pProdOfferAggrCur[lPre].m_lItemDisctSeq;
                                aProdOfferDetailDataEx.m_bNewRec = false;
                                break;
                            }
                            m = pProdOfferDetailCur[lDetailPre].m_lOffset;
                        }

                        if(m<=0){
                            m_poAggrDetailLock->P();
#ifdef MEM_REUSE_MODE

                        m = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
                        if(!m){
                            m_poAggrLock->P();
                            m_poAggrIndexLock->P ();
                            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
                            m_poAggrLock->V();
                            m_poAggrIndexLock->V ();
                            m = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
                        }                           

#else
                            m = m_poProdOfferDetailData->malloc();
#endif
                            m_poAggrDetailLock->V();
                            if (!m){
                             Log::log (0, "取实时优惠明细共享内存地址不足\n");
                                THROW(MBC_ProdOfferAggr+24);
                        }

                        pProdOfferDetailCur[m].m_lServID = poAcctItem->m_lServID;
                            pProdOfferDetailCur[m].m_iAcctItemTypeID = poAcctItem->m_iAcctItemTypeID;
                            pProdOfferDetailCur[m].m_lOutCharge=poAcctItem->m_lValue;
                            
                            lChangeFee = poAcctItem->m_lValue;
                            memcpy(&aProdOfferDetailDataEx,
                                        &pProdOfferDetailCur[m],
                                        sizeof(ProdOfferDetailData));
                            aProdOfferDetailDataEx.m_lOutCharge= lChangeFee;
                            aProdOfferDetailDataEx.m_lOffset = 
                                    pProdOfferAggrCur[lPre].m_lItemDisctSeq;
                            aProdOfferDetailDataEx.m_bNewRec = true;
                            pProdOfferDetailCur[lDetailPre].m_lOffset=m;
                        }
                        //增加变量输出
                        
                        aProdOfferDetailDataEx.m_lOutCharge = lChangeFee;
                        aProdOfferDetailDataEx.m_lInCharge = 0;
                    aProdOfferDetailDataEx.m_lOfferInstID = 
                                            poAcctItem->m_lProdOfferID;

                    v_ProdOfferDetailData.push_back(aProdOfferDetailDataEx);
                    aProdOfferAggrData.m_lAggrOfferSet=0;
                    v_ProdOfferAggr.push_back (aProdOfferAggrData);
                    }
                    
                    break;
                }
                t = pProdOfferAggrCur[lPre].m_lAggrOfferSet;
            }
            if(t<=0){
                m_poAggrLock->P();
#ifdef MEM_REUSE_MODE

            i = m_poProdOfferAggrData->malloc (poAcctItem->m_iBillingCycleID );
            if(!i){
                m_poAggrDetailLock->P();
                m_poAggrIndexLock->P ();
                revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
                m_poAggrDetailLock->V();
                m_poAggrIndexLock->V ();
                i = m_poProdOfferAggrData->malloc (poAcctItem->m_iBillingCycleID );
            }
#else
                i = m_poProdOfferAggrData->malloc();
#endif
            m_poAggrLock->V();
            if (!i) {
                Log::log (0, "取实时优惠共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+25);
            }
            m_poAggrDetailLock->P();
#ifdef MEM_REUSE_MODE

            j = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
            if(!j){
                m_poAggrLock->P();
                m_poAggrIndexLock->P ();
                revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
                m_poAggrLock->V();
                m_poAggrIndexLock->V ();
                j = m_poProdOfferDetailData->malloc (poAcctItem->m_iBillingCycleID );
            }

#else
            j = m_poProdOfferDetailData->malloc();
#endif
            m_poAggrDetailLock->V();
            if(!j){
                Log::log (0, "取实时优惠明细共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+26);
            }
                if(poAcctItem->m_lItemDisctSeq>0)
                                pProdOfferAggrCur[i].m_lItemDisctSeq = poAcctItem->m_lItemDisctSeq;
                        else
                        pProdOfferAggrCur[i].m_lItemDisctSeq = m_pItemDisctSeq->getNextVal ();
            //pProdOfferAggrCur[i].m_lItemDisctSeq = m_pItemDisctSeq->getNextVal ();
            pProdOfferAggrCur[i].m_lProdOfferID = poAcctItem->m_lProdOfferID;
            pProdOfferAggrCur[i].m_iBillingCycleID = poAcctItem->m_iBillingCycleID;
            pProdOfferAggrCur[i].m_iProdOfferCombine = poAcctItem->m_iProdOfferCombine;
            pProdOfferAggrCur[i].m_iPricingMeasure = poAcctItem->m_iPricingMeasure;
            pProdOfferAggrCur[i].m_iItemGroupID = poAcctItem->m_iItemGroupID;
            pProdOfferAggrCur[i].m_iInType = poAcctItem->m_iInType;
            pProdOfferAggrCur[i].m_lInValue = poAcctItem->m_lInValue;
            pProdOfferAggrCur[i].m_lValue = 0;
            if(poAcctItem->m_bInOutType == true){
                    pProdOfferAggrCur[i].m_lValue = poAcctItem->m_lValue;
                    pProdOfferAggrCur[i].m_lDisctValue= 0 ;
                }else{
                        pProdOfferAggrCur[i].m_lValue = 0;
                    pProdOfferAggrCur[i].m_lDisctValue= poAcctItem->m_lValue ;
                }
            #ifdef ORG_BILLINGCYCLE
              pProdOfferAggrCur[i].m_iOrgBillingCycleID = poAcctItem->m_iOrgBillingCycleID;
            #endif
            pProdOfferAggrCur[i].m_lDetailOfferSet = j;
            pProdOfferAggrCur[lPre].m_lAggrOfferSet = i;
            
            pProdOfferDetailCur[j].m_lServID = poAcctItem->m_lServID;
            pProdOfferDetailCur[j].m_iAcctItemTypeID = poAcctItem->m_iAcctItemTypeID;
            if(poAcctItem->m_bInOutType==true){
                pProdOfferDetailCur[j].m_lInCharge = poAcctItem->m_lValue;
                pProdOfferDetailCur[j].m_lOutCharge = 0;
            }else{
                pProdOfferDetailCur[j].m_lOutCharge = poAcctItem->m_lValue;
                pProdOfferDetailCur[j].m_lInCharge=0;
            }
            pProdOfferDetailCur[j].m_lOffset = -1;

            //sunjy
            pProdOfferDetailCur[j].m_lOffsetCur= pProdOfferAggrCur[i].m_lItemDisctSeq  ;
            pProdOfferDetailCur[j].m_iBIllingCycleID= pProdOfferAggrCur[i].m_iBillingCycleID ;
                //if(poAcctItem->m_bInOutType == false)
                {
                    //增加变量输出
                    memcpy(&aProdOfferAggrData, &pProdOfferAggrCur[i],
                                        sizeof(ProdOfferAggrData));
                aProdOfferAggrData.m_lAggrOfferSet=1;
                memcpy(&aProdOfferDetailDataEx,
                        &pProdOfferDetailCur[j],sizeof(ProdOfferDetailData));
                aProdOfferDetailDataEx.m_lOffset = 
                                    pProdOfferAggrCur[i].m_lItemDisctSeq;
                aProdOfferDetailDataEx.m_bNewRec = poAcctItem->m_bInOutType;
                aProdOfferDetailDataEx.m_lOutCharge=0;
                aProdOfferDetailDataEx.m_bNewRec = true;
                aProdOfferDetailDataEx.m_lOfferInstID = 
                                            poAcctItem->m_lProdOfferID;
                v_ProdOfferDetailData.push_back(aProdOfferDetailDataEx);

                v_ProdOfferAggr.push_back (aProdOfferAggrData);
                }
            }
        }
        
}

int ProdOfferDisctAggr::getAllItemAggr(ProdOfferAggrData * pProdAggrData, Value *pResult)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        unsigned int iOffset;
        long i;
        
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (!m_poProdOfferAggrIndex->get(pProdAggrData->m_lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    
        i = iOffset;
        while(i>0){
                if(pProdOfferAggrCur[i].m_lProdOfferID == pProdAggrData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdAggrData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iProdOfferCombine == pProdAggrData->m_iProdOfferCombine
            && pProdOfferAggrCur[i].m_iPricingMeasure == pProdAggrData->m_iPricingMeasure
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdAggrData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdAggrData->m_iInType
        && pProdOfferAggrCur[i].m_lInValue == pProdAggrData->m_lInValue
      #ifdef ORG_BILLINGCYCLE
        && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdAggrData->m_iOrgBillingCycleID
      #endif
      ){
                    pResult->m_lValue = pProdOfferAggrCur[i].m_lValue;
                    pResult->m_iType = NUMBER;
                    break;
                }       
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
        if(i<=0)
            return 0;   
        return 1;
}

//四川累积量查询需求增加获取优惠值
int ProdOfferDisctAggr::getAllDisctAggr(ProdOfferAggrData * pProdAggrData, Value *pResult) 
{
        ProdOfferAggrData * pProdOfferAggrCur;
        unsigned int iOffset;
        long i;
        
        if (!m_poProdOfferAggrIndex->get(pProdAggrData->m_lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    
        i = iOffset;
        while(i>0){
                if(pProdOfferAggrCur[i].m_lProdOfferID == pProdAggrData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdAggrData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iProdOfferCombine == pProdAggrData->m_iProdOfferCombine
            && pProdOfferAggrCur[i].m_iPricingMeasure == pProdAggrData->m_iPricingMeasure
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdAggrData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdAggrData->m_iInType
        && pProdOfferAggrCur[i].m_lInValue == pProdAggrData->m_lInValue
      #ifdef ORG_BILLINGCYCLE
        && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdAggrData->m_iOrgBillingCycleID
      #endif       
        ){
                    pResult->m_lValue = abs(pProdOfferAggrCur[i].m_lDisctValue);
                    pResult->m_iType = NUMBER;
                    break;
                }       
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
        if(i<=0)
            return 0;   
        return 1;
}

int ProdOfferDisctAggr::getAllDisctAggrB(ProdOfferAggrData * pProdAggrData, Value *pResult) 
{
    ProdOfferAggrData * pProdOfferAggrCur;
    unsigned int iOffset;
    long i;

    if (!m_poProdOfferAggrIndex->get(pProdAggrData->m_lProdOfferID, &iOffset)) {
        return 0;
    }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);

    i = iOffset;
    while (i>0) {
        if (pProdOfferAggrCur[i].m_lProdOfferID == pProdAggrData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdAggrData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdAggrData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdAggrData->m_iInType
            && pProdOfferAggrCur[i].m_lInValue == pProdAggrData->m_lInValue
      #ifdef ORG_BILLINGCYCLE
            && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdAggrData->m_iOrgBillingCycleID
      #endif       
           ) {
            pResult->m_lValue = abs(pProdOfferAggrCur[i].m_lDisctValue);
            pResult->m_iType = NUMBER;
            break;
        }
        i = pProdOfferAggrCur[i].m_lAggrOfferSet;
    }
    if (i<=0)
        return 0;
    return 1;
}
int ProdOfferDisctAggr::GetItemDetail(ProdOfferTotalData *pProdOfferData,Value *pResult)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferDetailData * pProdOfferDetailCur;
        unsigned int iOffset;
        long i,j;
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        
        if (!m_poProdOfferAggrIndex->get(pProdOfferData->m_lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    pResult->m_lValue = 0;
        i = iOffset;

        while(i>0){
                if(pProdOfferAggrCur[i].m_lProdOfferID == pProdOfferData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdOfferData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iProdOfferCombine == pProdOfferData->m_iProdOfferCombine
            && pProdOfferAggrCur[i].m_iPricingMeasure == pProdOfferData->m_iPricingMeasure
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdOfferData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdOfferData->m_iInType
        && pProdOfferAggrCur[i].m_lInValue == pProdOfferData->m_lInValue
      #ifdef ORG_BILLINGCYCLE
        && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdOfferData->m_iOrgBillingCycleID
      #endif       
      ){
            j = pProdOfferAggrCur[i].m_lDetailOfferSet;
            while(j>0){
                if(pProdOfferDetailCur[j].m_lServID == pProdOfferData->m_lServID
                    && pProdOfferDetailCur[j].m_iAcctItemTypeID == pProdOfferData->m_iAcctItemTypeID){
                    //&& pProdOfferDetailCur[j].m_bInOutType == pProdOfferData->m_bInOutType){
                        if(pProdOfferData->m_bInOutType ==true)
                            pResult->m_lValue = pProdOfferDetailCur[j].m_lInCharge;
                        else
                            pResult->m_lValue = pProdOfferDetailCur[j].m_lOutCharge;
                        pResult->m_iType = NUMBER;
                        return 1;
                    }
                    j = pProdOfferDetailCur[j].m_lOffset;
            }
                }
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }       
        return 0;
}

int ProdOfferDisctAggr::GetItemTotalHead(ProdOfferTotalData *pProdOfferData,ProdOfferAggrData **ppProdOfferAggrHead)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        unsigned int iOffset;
        long i;
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        
        if (!m_poProdOfferAggrIndex->get(pProdOfferData->m_lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);

        i = iOffset;

        while(i>0){
                if(pProdOfferAggrCur[i].m_lProdOfferID == pProdOfferData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdOfferData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iProdOfferCombine == pProdOfferData->m_iProdOfferCombine
            && pProdOfferAggrCur[i].m_iPricingMeasure == pProdOfferData->m_iPricingMeasure
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdOfferData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdOfferData->m_iInType
        && pProdOfferAggrCur[i].m_lInValue == pProdOfferData->m_lInValue
      #ifdef ORG_BILLINGCYCLE
        && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdOfferData->m_iOrgBillingCycleID
      #endif         
        ){
            *ppProdOfferAggrHead = &pProdOfferAggrCur[i];
            return 1;
                }
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }       
        return 0;
}

ProdOfferDetailData * ProdOfferDisctAggr::convertDetail(long lOffset)
{
    ProdOfferDetailData *pDetail;

    pDetail = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    // m_lOfferCur字段HB放本身的offset，OCS放detail记录的主键
    pDetail[lOffset].m_lOffsetCur=lOffset;

    return &pDetail[lOffset];
}

int ProdOfferDisctAggr::GetItemDetailHead(long lOffset,ProdOfferDetailData **ppProdOfferDetailHead)
{
        ProdOfferDetailData * pProdOfferDetailCur;
        
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    
    *ppProdOfferDetailHead = &pProdOfferDetailCur[lOffset];

    return 1;
}

int ProdOfferDisctAggr::GetItemDetailNum(long lOffset)
{
        ProdOfferDetailData * pProdOfferDetailCur;
        long j = lOffset;
        int iNum = 0;
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    
    while(j>0){
        iNum++;
        j = pProdOfferDetailCur[j].m_lOffset;
    }
    return iNum;
}

int ProdOfferDisctAggr::UpProdOfferAggr(
ProdOfferTotalData * pProdOfferData,
vector<ProdOfferAggrData > &v_ProdOfferAggr,
vector<ProdOfferDetailDataEx > &v_ProdOfferDetailData)
{
        unsigned int iOffset;
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferDetailData * pProdOfferDetailCur;
        
        bool bFlag;
        long i,j;
        pProdOfferData->m_lItemDisctSeq = 0;
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (!m_poProdOfferAggrIndex->get(pProdOfferData->m_lProdOfferID,&iOffset)) {
            doProdOfferDisct(pProdOfferData,v_ProdOfferAggr,v_ProdOfferDetailData);
        }
        else{
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
        pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
        i = iOffset;
        bFlag = false;
        while(i>0){
                if(pProdOfferAggrCur[i].m_lProdOfferID == pProdOfferData->m_lProdOfferID 
            && pProdOfferAggrCur[i].m_iBillingCycleID == pProdOfferData->m_iBillingCycleID
            && pProdOfferAggrCur[i].m_iProdOfferCombine == pProdOfferData->m_iProdOfferCombine
            && pProdOfferAggrCur[i].m_iPricingMeasure == pProdOfferData->m_iPricingMeasure
            && pProdOfferAggrCur[i].m_iItemGroupID == pProdOfferData->m_iItemGroupID
            && pProdOfferAggrCur[i].m_iInType == pProdOfferData->m_iInType
            && pProdOfferAggrCur[i].m_lInValue == pProdOfferData->m_lInValue
          #ifdef ORG_BILLINGCYCLE
           && pProdOfferAggrCur[i].m_iOrgBillingCycleID == pProdOfferData->m_iOrgBillingCycleID
          #endif            
            ){
                    pProdOfferAggrCur[i].m_lValue = pProdOfferData->m_lValue;
                    j = pProdOfferAggrCur[i].m_lDetailOfferSet;
                while(j>0){
                    j = pProdOfferAggrCur[i].m_lDetailOfferSet;
                    while(j>0){
                        if(pProdOfferDetailCur[j].m_lServID == pProdOfferData->m_lServID
                            && pProdOfferDetailCur[j].m_iAcctItemTypeID == pProdOfferData->m_iAcctItemTypeID){
                            //&& pProdOfferDetailCur[j].m_bInOutType == pProdOfferData->m_bInOutType){
                                if(pProdOfferData->m_bInOutType ==true)
                                    pProdOfferDetailCur[j].m_lInCharge = pProdOfferData->m_lValue;
                                else
                                    pProdOfferDetailCur[j].m_lOutCharge = pProdOfferData->m_lValue;
                                bFlag = true;
                                return 1;
                            }
                    }
                }
                }       
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
        if(bFlag == false)
            doProdOfferDisct(pProdOfferData,v_ProdOfferAggr,v_ProdOfferDetailData);
    }   
        return 0;
}

int ProdOfferDisctAggr::RefreshOfferAggr(
ProdOfferTotalData * pProdOfferData, bool bCreate)
{
        unsigned int iOffset;
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferDetailData * pProdOfferDetailCur;
        long i,j;
        vector<ProdOfferAggrData > v_ProdOfferAggr;
    vector<ProdOfferDetailDataEx > v_ProdOfferDetailData;

#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (!m_poProdOfferAggrIndex->get(pProdOfferData->m_lProdOfferID,&iOffset)) {
            if(bCreate){
            doProdOfferDisct(pProdOfferData,v_ProdOfferAggr,v_ProdOfferDetailData);
            v_ProdOfferAggr.clear ();
            v_ProdOfferDetailData.clear();
            return 2;
        }
        }
        else{
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
        pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
        i = iOffset;
        while(i>0){
                if(pProdOfferAggrCur[i].m_lItemDisctSeq == pProdOfferData->m_lItemDisctSeq ){
                    if(pProdOfferData->m_bInOutType ==true)
                        pProdOfferAggrCur[i].m_lValue = pProdOfferAggrCur[i].m_lValue-pProdOfferData->m_lValue;
                    else
                        pProdOfferAggrCur[i].m_lDisctValue = pProdOfferAggrCur[i].m_lDisctValue-pProdOfferData->m_lValue;
                        
                    j = pProdOfferAggrCur[i].m_lDetailOfferSet;
                while(j>0){
                    if(pProdOfferDetailCur[j].m_lServID == pProdOfferData->m_lServID
                    && pProdOfferDetailCur[j].m_iAcctItemTypeID == pProdOfferData->m_iAcctItemTypeID){
                        if(pProdOfferData->m_bInOutType ==true)
                            pProdOfferDetailCur[j].m_lInCharge = pProdOfferDetailCur[j].m_lInCharge-pProdOfferData->m_lValue;
                        else
                            pProdOfferDetailCur[j].m_lOutCharge = pProdOfferDetailCur[j].m_lOutCharge-pProdOfferData->m_lValue;

                        return 1;
                    }
                    j = pProdOfferDetailCur[j].m_lOffset;

                }
                m_poAggrDetailLock->P();
#ifdef MEM_REUSE_MODE

                j = m_poProdOfferDetailData->malloc (pProdOfferData->m_iBillingCycleID );
                if(!j){
                    m_poAggrLock->P();
                    m_poAggrIndexLock->P ();
                    revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
                    m_poAggrLock->V();
                    m_poAggrIndexLock->V ();
                    j = m_poProdOfferDetailData->malloc (pProdOfferData->m_iBillingCycleID );
                }                       

#else
                        j = m_poProdOfferDetailData->malloc();
#endif
                        m_poAggrDetailLock->V();
                        if(!j){
                            Log::log (0, "取实时优惠明细共享内存地址不足\n");
                            THROW(MBC_ProdOfferAggr+32);    
                        }
                        pProdOfferDetailCur[j].m_lServID = pProdOfferData->m_lServID;
                        pProdOfferDetailCur[j].m_iAcctItemTypeID = 
                                                            pProdOfferData->m_iAcctItemTypeID;

                        if(pProdOfferData->m_bInOutType == true){
                            pProdOfferDetailCur[j].m_lInCharge = -pProdOfferData->m_lValue;
                            pProdOfferDetailCur[j].m_lOutCharge = 0;
                        }else{
                            pProdOfferDetailCur[j].m_lOutCharge = -pProdOfferData->m_lValue;
                            pProdOfferDetailCur[j].m_lInCharge = 0;
                        }
                        pProdOfferDetailCur[j].m_lOffset = -1;
                        return 1;
                }       
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
    }   
        return 0;
}

void ProdOfferDisctAggr::getAll(long lServID,long lOfferInstID,int iBillingCycleID, 
      vector<ProdOfferTotalData>& voPOTData,int iOrgBillingCycleID)
{
    unsigned int iOffset;
    ProdOfferAggrData * pProdOfferAggrCur;
    ProdOfferDetailData * pProdOfferDetailCur;
    ProdOfferTotalData oPOTData;
    long i,j;
    voPOTData.clear();
 
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
    if (!m_poProdOfferAggrIndex->get(lOfferInstID,&iOffset)) {
        return;
    }

    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    i = iOffset;
    for(; i>0; i = pProdOfferAggrCur[i].m_lAggrOfferSet){
            if( lOfferInstID != pProdOfferAggrCur[i].m_lProdOfferID 
              ||iBillingCycleID != pProdOfferAggrCur[i].m_iBillingCycleID 
              #ifdef ORG_BILLINGCYCLE
                || iOrgBillingCycleID != pProdOfferAggrCur[i].m_iOrgBillingCycleID
              #endif
              )
                continue;

                oPOTData.m_lItemDisctSeq      = pProdOfferAggrCur[i].m_lItemDisctSeq;    
                oPOTData.m_lProdOfferID       = pProdOfferAggrCur[i].m_lProdOfferID;     
                oPOTData.m_iBillingCycleID    = pProdOfferAggrCur[i].m_iBillingCycleID;  
                oPOTData.m_iProdOfferCombine  = pProdOfferAggrCur[i].m_iProdOfferCombine;
                oPOTData.m_iPricingMeasure    = pProdOfferAggrCur[i].m_iPricingMeasure;  
                oPOTData.m_iItemGroupID       = pProdOfferAggrCur[i].m_iItemGroupID;     
                oPOTData.m_iInType            = pProdOfferAggrCur[i].m_iInType;          
                oPOTData.m_lInValue           = pProdOfferAggrCur[i].m_lInValue;         
                oPOTData.m_bNewRec            = true;  
                #ifdef ORG_BILLINGCYCLE
				           oPOTData.m_iOrgBillingCycleID = pProdOfferAggrCur[i].m_iOrgBillingCycleID;
				        #endif
            
            j = pProdOfferAggrCur[i].m_lDetailOfferSet;
            while(j>0){
                if(lServID == pProdOfferDetailCur[j].m_lServID ){
                    oPOTData.m_lServID = pProdOfferDetailCur[j].m_lServID;
                    oPOTData.m_iAcctItemTypeID  = pProdOfferDetailCur[j].m_iAcctItemTypeID;
                    oPOTData.m_lValue  = pProdOfferDetailCur[j].m_lInCharge;
                    oPOTData.m_bInOutType  = true;
                    voPOTData.push_back(oPOTData);
                    
                    oPOTData.m_lValue  = pProdOfferDetailCur[j].m_lOutCharge;
                    oPOTData.m_bInOutType  = false;
                    voPOTData.push_back(oPOTData);
                }
                j = pProdOfferDetailCur[j].m_lOffset;

            }
    }

return;

}

void ProdOfferDisctAggr::getAllByInst(long lOfferInstID,int iBillingCycleID, 
      vector<ProdOfferTotalData>& voPOTData,int iOrgBillingCycleID)
{
    unsigned int iOffset;
    ProdOfferAggrData * pProdOfferAggrCur;
    ProdOfferDetailData * pProdOfferDetailCur;
    ProdOfferTotalData oPOTData;
    long i,j;
    voPOTData.clear();
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
 
    if (!m_poProdOfferAggrIndex->get(lOfferInstID,&iOffset)) {
        return;
    }

    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    i = iOffset;
    for(; i>0; i = pProdOfferAggrCur[i].m_lAggrOfferSet){
            if( lOfferInstID != pProdOfferAggrCur[i].m_lProdOfferID 
              ||iBillingCycleID != pProdOfferAggrCur[i].m_iBillingCycleID 
              #ifdef ORG_BILLINGCYCLE
                || iOrgBillingCycleID != pProdOfferAggrCur[i].m_iOrgBillingCycleID
              #endif
              )
                continue;

                oPOTData.m_lItemDisctSeq      = pProdOfferAggrCur[i].m_lItemDisctSeq;    
                oPOTData.m_lProdOfferID       = pProdOfferAggrCur[i].m_lProdOfferID;     
                oPOTData.m_iBillingCycleID    = pProdOfferAggrCur[i].m_iBillingCycleID;  
                oPOTData.m_iProdOfferCombine  = pProdOfferAggrCur[i].m_iProdOfferCombine;
                oPOTData.m_iPricingMeasure    = pProdOfferAggrCur[i].m_iPricingMeasure;  
                oPOTData.m_iItemGroupID       = pProdOfferAggrCur[i].m_iItemGroupID;     
                oPOTData.m_iInType            = pProdOfferAggrCur[i].m_iInType;          
                oPOTData.m_lInValue           = pProdOfferAggrCur[i].m_lInValue;         
                oPOTData.m_bNewRec            = true;  
                #ifdef ORG_BILLINGCYCLE
				           oPOTData.m_iOrgBillingCycleID = pProdOfferAggrCur[i].m_iOrgBillingCycleID;
				        #endif 
            
            j = pProdOfferAggrCur[i].m_lDetailOfferSet;
            while(j>0){
                
                oPOTData.m_lServID = pProdOfferDetailCur[j].m_lServID;
                oPOTData.m_iAcctItemTypeID  = pProdOfferDetailCur[j].m_iAcctItemTypeID;
                oPOTData.m_lValue  = pProdOfferDetailCur[j].m_lInCharge;
                oPOTData.m_bInOutType  = true;
                voPOTData.push_back(oPOTData);
                
                oPOTData.m_lValue  = pProdOfferDetailCur[j].m_lOutCharge;
                oPOTData.m_bInOutType  = false;
                voPOTData.push_back(oPOTData);

                j = pProdOfferDetailCur[j].m_lOffset;

            }
    }

return;

}

//add by zhaoziwei
void ProdOfferDisctAggr::getAllDisctAggrByInst(long lOfferInstID,int iBillingCycleID, 
      vector<ProdOfferAggrData *>&voPOTData,int iOrgBillingCycleID)
{
    unsigned int iOffset;
    ProdOfferAggrData * pProdOfferAggrCur;
    long i;
    voPOTData.clear();
                
    if (!m_poProdOfferAggrIndex->get(lOfferInstID,&iOffset)) 
    {
        return;
    }

    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    i = iOffset;
    for(; i>0; i = pProdOfferAggrCur[i].m_lAggrOfferSet)
    {
            if( lOfferInstID != pProdOfferAggrCur[i].m_lProdOfferID 
              ||iBillingCycleID != pProdOfferAggrCur[i].m_iBillingCycleID 
              #ifdef ORG_BILLINGCYCLE
                || iOrgBillingCycleID != pProdOfferAggrCur[i].m_iOrgBillingCycleID
              #endif
              )
                continue;


                                                voPOTData.push_back(&pProdOfferAggrCur[i]);
    }

                return;
}

void ProdOfferDisctAggr::getAllDisctByInst(long lItemDisctSeq,long lOfferInstID,int iBillingCycleID, 
      vector<ProdOfferDetailData *>&voPOTData,int iOrgBillingCycleID)
{
    unsigned int iOffset;
    ProdOfferAggrData * pProdOfferAggrCur;
    ProdOfferDetailData * pProdOfferDetailCur;
    long i,j;
 
    if (!m_poProdOfferAggrIndex->get(lOfferInstID,&iOffset))
    {
        return;
    }
                
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    i = iOffset;
    for(; i>0; i = pProdOfferAggrCur[i].m_lAggrOfferSet)
    {
            if( lItemDisctSeq != pProdOfferAggrCur[i].m_lItemDisctSeq 
              ||iBillingCycleID != pProdOfferAggrCur[i].m_iBillingCycleID 
              #ifdef ORG_BILLINGCYCLE
                || iOrgBillingCycleID != pProdOfferAggrCur[i].m_iOrgBillingCycleID
              #endif
              )
                continue;
            
            j = pProdOfferAggrCur[i].m_lDetailOfferSet;
            while(j>0)
            {
                voPOTData.push_back(&pProdOfferDetailCur[j]);

                j = pProdOfferDetailCur[j].m_lOffset;
            }
    }

                return;
}
//end


bool ProdOfferDisctAggr::UpdateOfferAggrZero(ProdOfferAggrData * pProdAggrData)
{
        if(!pProdAggrData) return false;
        bool bRet = false;
        ProdOfferAggrData * pProdOfferAggrCur = NULL;
        ProdOfferDetailData *pProdOfferDetailCur = NULL;
        ProdOfferDetailData *pProdOfferDetailHead = NULL;
        ProdOfferDisctAggr aProdOffer;
        unsigned int iOffset;
        long lOffset;
        long i; 
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (!m_poProdOfferAggrIndex->get(pProdAggrData->m_lProdOfferID, &iOffset)) {
            return false;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
        pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
        i = iOffset;

        while(i>0)
        {
                if(pProdAggrData->m_lItemDisctSeq == pProdOfferAggrCur[i].m_lItemDisctSeq)
                {
                        pProdOfferAggrCur[i].m_lValue = 0;
                        pProdOfferAggrCur[i].m_lDisctValue = 0;
                        bRet = true;
                        break;
                }
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
        if(!bRet)
        {
                Log::log(0,"实例ID为[%ld]，主健为[%ld]的没有对应优惠状态记录，跳过",pProdAggrData->m_lProdOfferID,
                        pProdAggrData->m_lItemDisctSeq);
                return bRet;
        }
        lOffset = pProdOfferAggrCur[i].m_lDetailOfferSet;
        if(lOffset<=0)
        {
                Log::log(0,"实例ID为[%ld]，主健为[%ld]的没有优惠明细记录！",pProdAggrData->m_lProdOfferID,
                        pProdAggrData->m_lItemDisctSeq );
                return bRet;
        }
        while(lOffset>0)
        {
                pProdOfferDetailCur[lOffset].m_lInCharge = 0;
                pProdOfferDetailCur[lOffset].m_lOutCharge = 0;
                lOffset = pProdOfferDetailCur[lOffset].m_lOffset;
        }
        return bRet;
}

bool ProdOfferDisctAggr::UpdateOfferAggr(ProdOfferAggrData * pProdAggrData ,bool &bNew,bool bReplace)
{
        if(!pProdAggrData) return false;
        bool bRet = true;
        bNew = false;
        ProdOfferAggrData * pProdOfferAggrCur = NULL;
        unsigned int iOffset = 0;
        long i = 0,j = 0,lUpOffset = 0;
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (m_poProdOfferAggrIndex->get(pProdAggrData->m_lProdOfferID, &iOffset))
        {
                j = iOffset;
                while(j>0)
                {
                        if( pProdOfferAggrCur[j].m_lItemDisctSeq == pProdAggrData->m_lItemDisctSeq
                                && pProdOfferAggrCur[j].m_lProdOfferID == pProdAggrData->m_lProdOfferID 
                                && pProdOfferAggrCur[j].m_iBillingCycleID == pProdAggrData->m_iBillingCycleID
                                && pProdOfferAggrCur[j].m_iProdOfferCombine == pProdAggrData->m_iProdOfferCombine
                                && pProdOfferAggrCur[j].m_iPricingMeasure == pProdAggrData->m_iPricingMeasure
                                && pProdOfferAggrCur[j].m_iItemGroupID == pProdAggrData->m_iItemGroupID
                                && pProdOfferAggrCur[j].m_iInType == pProdAggrData->m_iInType
                                && pProdOfferAggrCur[j].m_lInValue == pProdAggrData->m_lInValue
				#ifdef ORG_BILLINGCYCLE
				  && pProdOfferAggrCur[j].m_iOrgBillingCycleID == pProdAggrData->m_iOrgBillingCycleID
				#endif
                                )
                        {
                                if(bReplace)
                                {
                                        pProdOfferAggrCur[j].m_lValue = pProdAggrData->m_lValue;
                                        pProdOfferAggrCur[j].m_lDisctValue = pProdAggrData ->m_lDisctValue;
                                }else
                                {
                                        pProdOfferAggrCur[j].m_lValue += pProdAggrData->m_lValue;
                                        pProdOfferAggrCur[j].m_lDisctValue += pProdAggrData ->m_lDisctValue;
                                }                               
                                return bRet;
                        }
                        lUpOffset = j;
                        j = pProdOfferAggrCur[j].m_lAggrOfferSet;
                }
        }
        bNew = true;

#ifdef MEM_REUSE_MODE

    i = m_poProdOfferAggrData->malloc (pProdAggrData->m_iBillingCycleID );
    if(!i){
        m_poAggrDetailLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
        m_poAggrDetailLock->V();
        m_poAggrIndexLock->V ();
        i = m_poProdOfferAggrData->malloc (pProdAggrData->m_iBillingCycleID );
    }
#else
        m_poAggrLock->P();
        i = m_poProdOfferAggrData->malloc();
        m_poAggrLock->V();
#endif
        
    if(!i)
        {
                Log::log (0, "取实时优惠共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+21);
        }
        pProdOfferAggrCur[i].m_lItemDisctSeq = pProdAggrData->m_lItemDisctSeq;
    pProdOfferAggrCur[i].m_lProdOfferID = pProdAggrData->m_lProdOfferID;
    pProdOfferAggrCur[i].m_iBillingCycleID = pProdAggrData->m_iBillingCycleID;
    pProdOfferAggrCur[i].m_iProdOfferCombine = pProdAggrData->m_iProdOfferCombine;
    pProdOfferAggrCur[i].m_iPricingMeasure = pProdAggrData->m_iPricingMeasure;
    pProdOfferAggrCur[i].m_iItemGroupID = pProdAggrData->m_iItemGroupID;
    pProdOfferAggrCur[i].m_iInType = pProdAggrData->m_iInType;
    pProdOfferAggrCur[i].m_lInValue = pProdAggrData->m_lInValue;
        pProdOfferAggrCur[i].m_lValue = pProdAggrData->m_lValue;
        pProdOfferAggrCur[i].m_lDisctValue = pProdAggrData->m_lDisctValue;
		#ifdef ORG_BILLINGCYCLE
		  pProdOfferAggrCur[i].m_iOrgBillingCycleID = pProdAggrData->m_iOrgBillingCycleID;
		#endif
    pProdOfferAggrCur[i].m_lDetailOfferSet = -1;
        if(lUpOffset>0)
        {
                pProdOfferAggrCur[lUpOffset].m_lAggrOfferSet = i;
        }else
        {
                pProdOfferAggrCur[i].m_lAggrOfferSet = -1;
                m_poAggrIndexLock->P();
                m_poProdOfferAggrIndex->add (pProdAggrData->m_lProdOfferID, i);
                m_poAggrIndexLock->V();
        }       
        return bRet;
}

bool ProdOfferDisctAggr::UpdateOfferDetail(ProdOfferDetailBase * pOfferDetailData,bool &bNew,bool bReplace)
{
        if(!pOfferDetailData) return false;
        bool bRet = false;
        bNew = false;
        ProdOfferAggrData * pProdOfferAggrCur = NULL;
        ProdOfferDetailData * pProdOfferDetailCur = NULL;
        ProdOfferAggrData * pProdOfferAggr;
        unsigned int iOffset = 0;
        long i = 0,j = 0;
        long lOffset = 0,lUpOffset = 0;
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        if (m_poProdOfferAggrIndex->get(pOfferDetailData->m_lOfferInstID, &iOffset))
        {
                i = iOffset;
                while(i>0)
                {
                        if( pProdOfferAggrCur[i].m_lItemDisctSeq == pOfferDetailData->m_lItemDisctSeq
                                && pProdOfferAggrCur[i].m_lProdOfferID == pOfferDetailData->m_lOfferInstID 
                                )
                        {
                                lOffset = pProdOfferAggrCur[i].m_lDetailOfferSet;
                                break;
                        }
                        i = pProdOfferAggrCur[i].m_lAggrOfferSet;
                }
        }
        if(lOffset == 0)
        {
                Log::log(0,"根据明细ID[%ld]、实例ID[%ld]的没有找到内存优惠记录，跳过本条！",
                        pOfferDetailData->m_lItemDisctSeq, pOfferDetailData->m_lOfferInstID );
                return bRet;
        }
        if(lOffset == -1)
        {
#ifdef MEM_REUSE_MODE

        j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
        if(!j){
            m_poAggrLock->P();
            m_poAggrIndexLock->P ();
            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
            m_poAggrLock->V();
            m_poAggrIndexLock->V ();
            j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
        }
#else
                m_poAggrDetailLock->P();
                j = m_poProdOfferDetailData->malloc();
                m_poAggrDetailLock->V();
#endif
                if(!j)
                {
                        Log::log (0, "取实时优惠明细共享内存地址不足\n");
                        THROW(MBC_ProdOfferAggr+22);
                }
                pProdOfferDetailCur[j].m_lServID = pOfferDetailData->m_lServID;
                pProdOfferDetailCur[j].m_iAcctItemTypeID = pOfferDetailData->m_iAcctItemTypeID;
                pProdOfferDetailCur[j].m_lInCharge = pOfferDetailData->m_lInCharge;
                pProdOfferDetailCur[j].m_lOutCharge = pOfferDetailData->m_lOutCharge;
                pProdOfferDetailCur[j].m_lOffset = -1;
                pProdOfferAggrCur[i].m_lDetailOfferSet = j;
                bNew = true;
                bRet = true;
                return bRet;
        }
        while(lOffset>0)
        {
                if(pProdOfferDetailCur[lOffset].m_lServID == pOfferDetailData->m_lServID
                        && pProdOfferDetailCur[lOffset].m_iAcctItemTypeID == pOfferDetailData->m_iAcctItemTypeID
                        )
                {
                        if(bReplace)
                        {
                                pProdOfferDetailCur[lOffset].m_lInCharge = pOfferDetailData->m_lInCharge;
                                pProdOfferDetailCur[lOffset].m_lOutCharge = pOfferDetailData->m_lOutCharge;
                        }else
                        {
                                pProdOfferDetailCur[lOffset].m_lInCharge += pOfferDetailData->m_lInCharge;
                                pProdOfferDetailCur[lOffset].m_lOutCharge += pOfferDetailData->m_lOutCharge;
                        }
                        bRet = true;
                        return bRet;
                }
                lUpOffset = lOffset;
                lOffset = pProdOfferDetailCur[lOffset].m_lOffset;
        }
#ifdef MEM_REUSE_MODE

    j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
    if(!j){
        m_poAggrLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
        m_poAggrLock->V();
        m_poAggrIndexLock->V ();
        j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
    }

#else
        j = m_poProdOfferDetailData->malloc();
#endif
        if(!j)
        {
                Log::log (0, "取实时优惠明细共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+22);
        }
        pProdOfferDetailCur[j].m_lServID = pOfferDetailData->m_lServID;
        pProdOfferDetailCur[j].m_iAcctItemTypeID = pOfferDetailData->m_iAcctItemTypeID;
        pProdOfferDetailCur[j].m_lInCharge = pOfferDetailData->m_lInCharge;
        pProdOfferDetailCur[j].m_lOutCharge = pOfferDetailData->m_lOutCharge;
        pProdOfferDetailCur[j].m_lOffset  = -1;
        pProdOfferDetailCur[lUpOffset].m_lOffset = j;
        bNew = true;
        bRet = true;
        return bRet;
}

#ifdef HAVE_ABM 
void ProdOfferDisctAggr::getAllAggrAndDetail(long lOfferInstID,int iBillingCycleID, 
      vector<DisctAccu>& vOutData)
{
    unsigned int iOffset;
    ProdOfferAggrData * pProdOfferAggrCur;
    ProdOfferDetailData * pProdOfferDetailCur;
        static DisctAccu oData;
        oData.m_voDetail.clear();
        memset(&oData.m_oDisct,0,sizeof(oData.m_oDisct));
    long i,j;
 
    if (!m_poProdOfferAggrIndex->get(lOfferInstID,&iOffset)) {
        return;
    }

    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
    i = iOffset;
        for(; i>0; i = pProdOfferAggrCur[i].m_lAggrOfferSet)
        {
                if( lOfferInstID != pProdOfferAggrCur[i].m_lProdOfferID 
                        ||iBillingCycleID != pProdOfferAggrCur[i].m_iBillingCycleID ){
                        continue;
                }

                oData.m_oDisct.m_lItemDisctSeq = pProdOfferAggrCur[i].m_lItemDisctSeq;
                oData.m_oDisct.m_lOfferInstID = pProdOfferAggrCur[i].m_lProdOfferID;
                oData.m_oDisct.m_iBillingCycleID = pProdOfferAggrCur[i].m_iBillingCycleID;
                oData.m_oDisct.m_iCombineID = pProdOfferAggrCur[i].m_iProdOfferCombine;
                oData.m_oDisct.m_iMeasureID = pProdOfferAggrCur[i].m_iPricingMeasure;
                oData.m_oDisct.m_iItemGroupID = pProdOfferAggrCur[i].m_iItemGroupID;
                oData.m_oDisct.m_iInType = pProdOfferAggrCur[i].m_iInType;
                oData.m_oDisct.m_lInValue = pProdOfferAggrCur[i].m_lInValue;
                oData.m_oDisct.m_lValue = pProdOfferAggrCur[i].m_lValue;
                oData.m_oDisct.m_lDisctValue = pProdOfferAggrCur[i].m_lDisctValue;
		oData.m_oDisct.m_lABMKeyID = oData.m_oDisct.m_lItemDisctSeq;
		oData.m_oDisct.m_lVersionID = pProdOfferAggrCur[i].m_lVersionID;
                #ifdef ORG_BILLINGCYCLE
                oData.m_oDisct.m_iOrgBillingCycleID = pProdOfferAggrCur[i].m_iOrgBillingCycleID;
                #endif
                oData.m_oDisct.m_cOperation = 'o';
                oData.m_oDisct.m_iFileID = G_PEVENT->m_iFileID;
                oData.m_oDisct.m_iEventSourceType = G_PEVENT->m_iEventSourceType;
                oData.m_oDisct.m_iOrgID = G_PEVENT->m_iBillingOrgID;
                
                j = pProdOfferAggrCur[i].m_lDetailOfferSet;
                while(j>0){
                        static DisctDetailCacheData detailData;
                        memset(&detailData,0,sizeof(DisctDetailCacheData));
                        detailData.m_lServID = pProdOfferDetailCur[j].m_lServID;
                        detailData.m_iAcctItemTypeID = pProdOfferDetailCur[j].m_iAcctItemTypeID;
                        detailData.m_lValue = pProdOfferDetailCur[j].m_lInCharge;
                        detailData.m_lDisctValue = pProdOfferDetailCur[j].m_lOutCharge;
                        detailData.m_iBillingCycleID = oData.m_oDisct.m_iBillingCycleID;
                        detailData.m_lItemDisctSeq = oData.m_oDisct.m_lItemDisctSeq;
			detailData.m_lItemDetailSeq = pProdOfferDetailCur[j].m_lItemDetailSeq;
			detailData.m_lABMKeyID = detailData.m_lItemDetailSeq;
			detailData.m_lVersionID = pProdOfferDetailCur[j].m_lVersionID;
			
                        detailData.m_cOperation = 'o';
                        detailData.m_lEventID = G_PEVENT->m_lEventID;
                        detailData.m_iFileID = G_PEVENT->m_iFileID;
                        detailData.m_iEventSourceType = G_PEVENT->m_iEventSourceType;
                        detailData.m_iOrgID = G_PUSERINFO->getServOrg (detailData.m_lServID,G_PEVENT->m_sStartDate);

                        oData.m_voDetail.push_back(detailData);
                        j = pProdOfferDetailCur[j].m_lOffset;
                }
                vOutData.push_back(oData);
        }
        return;
}

unsigned int ProdOfferDisctAggr::updateAggrByCache(DisctCacheData * pCacheData)
{
        if (!pCacheData) return;

        ProdOfferAggrData * pProdOfferAggrCur = NULL;
        unsigned int iOffset = 0;
        long i = 0,j = 0,lUpOffset = 0;
        bool bFind = false;
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        m_poProdOfferAggrIndex->get(pCacheData->m_lOfferInstID, &iOffset);
        if ( 'u' == pCacheData->m_cOperation)
        {
                if (0 == iOffset){
                        Log::log(0,"严重告警：主健为[%ld]，商品实例为[%ld]的优惠累积量，本地缓存中有，累积量共享内存中没有数据，请检查！",
                                pCacheData->m_lItemDisctSeq,pCacheData->m_lOfferInstID);
                        return 0;
                }
                j = iOffset;
                while(j>0)
                {
                        if (pProdOfferAggrCur[j].m_lItemDisctSeq == pCacheData->m_lItemDisctSeq)
                        {
                                if (pCacheData->m_lAddValue != 0){
                                        if (pProdOfferAggrCur[j].m_lValue != pCacheData->m_lValue){
                                                Log::log(0,"严重告警：主健为[%ld]，商品实例为[%ld]的优惠累积量，本地缓存与共享内存输入原始值不一致，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lOfferInstID);
                                                return j;
                                        }
                                        pProdOfferAggrCur[j].m_lValue += pCacheData->m_lAddValue;
                                        bFind = true;
                                }
                                if(pCacheData->m_lAddDisctValue != 0){
                                        if (pProdOfferAggrCur[j].m_lDisctValue != pCacheData->m_lDisctValue){
                                                Log::log(0,"严重告警：主健为[%ld]，商品实例为[%ld]的优惠累积量，本地缓存与共享内存输出原始值不一致，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lOfferInstID);
                                                return j;
                                        }
                                        pProdOfferAggrCur[j].m_lDisctValue += pCacheData->m_lAddDisctValue;
                                        bFind = true;
                                }
                                if(bFind){
                                        pProdOfferAggrCur[j].m_lVersionID = pCacheData->m_lVersionID;
                                        return j;
                                }
                                lUpOffset = j;
                        }
                        j = pProdOfferAggrCur[j].m_lAggrOfferSet;
                }
                Log::log(0,"严重告警：主健为[%ld]，商品实例为[%ld]的优惠累积量，本地缓存与共享内存没有找到主健一致的记录，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lOfferInstID);
                return lUpOffset;
        }
        while (iOffset>0){
                lUpOffset = iOffset;
                iOffset = pProdOfferAggrCur[j].m_lAggrOfferSet;
        }
        m_poAggrLock->P();
#ifdef MEM_REUSE_MODE
    i = m_poProdOfferAggrData->malloc (pCacheData->m_iBillingCycleID);
    if(!i){
        m_poAggrDetailLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
        m_poAggrDetailLock->V();
        m_poAggrIndexLock->V ();
        i = m_poProdOfferAggrData->malloc (pCacheData->m_iBillingCycleID);
    }
#else
        i = m_poProdOfferAggrData->malloc();
#endif
        m_poAggrLock->V();      
    if(!i)
        {
                Log::log (0, "取实时优惠共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+21);
        }
        pProdOfferAggrCur[i].m_lItemDisctSeq = pCacheData->m_lItemDisctSeq;
        pProdOfferAggrCur[i].m_lProdOfferID = pCacheData->m_lOfferInstID;
        pProdOfferAggrCur[i].m_iBillingCycleID = pCacheData->m_iBillingCycleID;
        pProdOfferAggrCur[i].m_iProdOfferCombine = pCacheData->m_iCombineID;
        pProdOfferAggrCur[i].m_iPricingMeasure = pCacheData->m_iMeasureID;
        pProdOfferAggrCur[i].m_iItemGroupID = pCacheData->m_iItemGroupID;
        pProdOfferAggrCur[i].m_iInType = pCacheData->m_iInType;
        pProdOfferAggrCur[i].m_lInValue = pCacheData->m_lInValue;
        pProdOfferAggrCur[i].m_lValue = pCacheData->m_lAddValue;
        pProdOfferAggrCur[i].m_lDisctValue = pCacheData->m_lAddDisctValue;      
        pProdOfferAggrCur[j].m_lVersionID = pCacheData->m_lVersionID;
/*
                #ifdef ORG_BILLINGCYCLE
                  pProdOfferAggrCur[i].m_iOrgBillingCycleID = pCacheData->m_iOrgBillingCycleID;
                #endif
*/
    pProdOfferAggrCur[i].m_lDetailOfferSet = -1;
        if(lUpOffset>0)
        {
                pProdOfferAggrCur[lUpOffset].m_lAggrOfferSet = i;
        }else
        {
                pProdOfferAggrCur[i].m_lAggrOfferSet = -1;
                m_poAggrIndexLock->P();
                m_poProdOfferAggrIndex->add (pCacheData->m_lOfferInstID, i);
                m_poAggrIndexLock->V();
        }
        
        return i;
}

void ProdOfferDisctAggr::updateDetailByCache(DisctDetailCacheData * pCacheData,unsigned int iAggOffset)
{
        if (!pCacheData) return;
        ProdOfferAggrData * pProdOfferAggrCur = NULL;
        ProdOfferDetailData * pProdOfferDetailCur = NULL;
        ProdOfferAggrData * pProdOfferAggr;
        unsigned int iOffset = 0;
        long i = 0,j = 0;
        long lOffset = 0,lUpOffset = 0;
        bool bFind = false;
        pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
#ifdef MEM_REUSE_MODE
        m_poProdOfferAggrData->checkSafe();
#endif
        lOffset = pProdOfferAggrCur[iAggOffset].m_lDetailOfferSet;
        if ( 'u' == pCacheData->m_cOperation)
        {
                if (lOffset <= 0){
                        Log::log(0,"严重告警：主健为[%ld]，用户ID为[%ld]，帐目类型为[%ld]的优惠明细，本地缓存中有，共享内存中没有数据，请检查！",
                                pCacheData->m_lItemDisctSeq,pCacheData->m_lServID,pCacheData->m_iAcctItemTypeID);
                        return;
                }
                j = lOffset;
                while(j>0)
                {
                        if (pProdOfferDetailCur[j].m_lServID == pCacheData->m_lServID &&
                                pProdOfferDetailCur[j].m_iAcctItemTypeID == pCacheData->m_iAcctItemTypeID)
                        {
                                if (pCacheData->m_lAddValue != 0){
                                        if (pProdOfferDetailCur[j].m_lInCharge != pCacheData->m_lValue){
                                                Log::log(0,"严重告警：主健为[%ld]，用户ID为[%ld]，帐目类型为[%ld]的优惠明细，本地缓存中与共享内存输入原始数据不一致，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lServID,pCacheData->m_iAcctItemTypeID);
                                                return;
                                        }
                                        pProdOfferDetailCur[j].m_lInCharge += pCacheData->m_lAddValue;
                                        bFind = true;
                                }
                                if(pCacheData->m_lAddDisctValue != 0){
                                        if (pProdOfferDetailCur[j].m_lOutCharge != pCacheData->m_lDisctValue){
                                                Log::log(0,"严重告警：主健为[%ld]，用户ID为[%ld]，帐目类型为[%ld]的优惠明细，本地缓存中有与共享内存输出原始数据不一致，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lServID,pCacheData->m_iAcctItemTypeID);
                                                return;
                                        }
                                        pProdOfferDetailCur[j].m_lOutCharge += pCacheData->m_lAddDisctValue;            
                                        bFind = true;
                                }
                                if(bFind){
                                        pProdOfferDetailCur[j].m_lVersionID = pCacheData->m_lVersionID;
                                        return;
                        }
                        }
                        j = pProdOfferDetailCur[j].m_lOffset;
                }
                Log::log(0,"严重告警：主健为[%ld]，用户ID为[%ld]，帐目类型为[%ld]的优惠明细，本地缓存的数据没有在共享内存中找到主健一致的数据，请检查！",
                                                        pCacheData->m_lItemDisctSeq,pCacheData->m_lServID,pCacheData->m_iAcctItemTypeID);
                return;
        }
        while (lOffset>0){
                lUpOffset = lOffset;
                lOffset = pProdOfferDetailCur[lOffset].m_lOffset;
        }
        m_poAggrDetailLock->P();
        #ifdef MEM_REUSE_MODE
    j = m_poProdOfferDetailData->malloc (pCacheData->m_iBillingCycleID);
    if(!j){
        m_poAggrLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
        m_poAggrLock->V();
        m_poAggrIndexLock->V ();
        j = m_poProdOfferDetailData->malloc (pCacheData->m_iBillingCycleID);
    }
        #else
        j = m_poProdOfferDetailData->malloc();
        #endif
        m_poAggrDetailLock->V();
        if(!j){
                Log::log (0, "取实时优惠明细共享内存地址不足\n");
                THROW(MBC_ProdOfferAggr+22);
        }
        pProdOfferDetailCur[j].m_lServID = pCacheData->m_lServID;
        pProdOfferDetailCur[j].m_iAcctItemTypeID = pCacheData->m_iAcctItemTypeID;
        pProdOfferDetailCur[j].m_lInCharge = pCacheData->m_lAddValue;
        pProdOfferDetailCur[j].m_lOutCharge = pCacheData->m_lAddDisctValue;
        pProdOfferDetailCur[j].m_lOffset = -1;
        if (pProdOfferAggrCur[iAggOffset].m_lDetailOfferSet<=0){
                pProdOfferAggrCur[iAggOffset].m_lDetailOfferSet = j;
        }else{
                pProdOfferDetailCur[lUpOffset].m_lOffset = j;
        }
        pProdOfferDetailCur[j].m_lVersionID = pCacheData->m_lVersionID;
        pProdOfferDetailCur[j].m_lItemDetailSeq = pCacheData->m_lItemDetailSeq;
        return;
}

long ProdOfferDisctAggr::updateAuditOfferAggr(DisctAccuData *pProdAggrData, bool isSync){
    if (!pProdAggrData) return -1;
    ProdOfferAggrData * pProdOfferAggrCur = NULL;
    unsigned int iOffset = 0;
    long i = 0,j = 0,lUpOffset = 0;
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    #ifdef MEM_REUSE_MODE
    m_poProdOfferAggrData->checkSafe();
#endif
    if (m_poProdOfferAggrIndex->get(pProdAggrData->m_lOfferInstID, &iOffset)) {
        j = iOffset;
        while (j>0) {
            if ( pProdOfferAggrCur[j].m_lProdOfferID == pProdAggrData->m_lOfferInstID 
                 && pProdOfferAggrCur[j].m_iBillingCycleID == pProdAggrData->m_iBillingCycleID
                 && pProdOfferAggrCur[j].m_iProdOfferCombine == pProdAggrData->m_iCombineID
                 && pProdOfferAggrCur[j].m_iPricingMeasure == pProdAggrData->m_iMeasureID
                 && pProdOfferAggrCur[j].m_iItemGroupID == pProdAggrData->m_iItemGroupID
                 && pProdOfferAggrCur[j].m_iInType == pProdAggrData->m_iInType
                 && pProdOfferAggrCur[j].m_lInValue == pProdAggrData->m_lInValue
                #ifdef ORG_BILLINGCYCLE
                 && pProdOfferAggrCur[j].m_iOrgBillingCycleID == pProdAggrData->m_iOrgBillingCycleID
                #endif
               ) {
                if(isSync){
                    if( pProdOfferAggrCur[j].m_lVersionID +1 != pProdAggrData->m_lVersionID ){
                        Log::log(0, "累积量[ItemDisctSeq=%ld]本地VersionID为[%ld],ABM端[%ld],"
                                    "不符合反向更新规则，更新失败!",
                                 pProdOfferAggrCur[j].m_lItemDisctSeq, pProdOfferAggrCur[j].m_lVersionID,
                                 pProdAggrData->m_lVersionID);
                        return -1;
                    }
                }
                if( pProdOfferAggrCur[j].m_lItemDisctSeq != pProdAggrData->m_lItemDisctSeq ){
                    Log::log(0, "累积量[ItemDisctSeq=%ld]与ABM[ItemDisctSeq=%ld ]端返回不一致,更新失败!",
                             pProdOfferAggrCur[j].m_lItemDisctSeq, pProdAggrData->m_lItemDisctSeq);
                    return -1;
                } else {
                    if( pProdOfferAggrCur[j].m_lVersionID > pProdAggrData->m_lVersionID ){
                        Log::log(0, "累积量[ItemDisctSeq=%ld]本地VersionID为[%ld],高于ABM端[%ld],更新失败!",
                             pProdOfferAggrCur[j].m_lItemDisctSeq, pProdOfferAggrCur[j].m_lVersionID,
                                 pProdAggrData->m_lVersionID);
                        return -1;
                    } else {
                        pProdOfferAggrCur[j].m_lValue = pProdAggrData->m_lValue;
                        pProdOfferAggrCur[j].m_lDisctValue = pProdAggrData->m_lDisctValue;
                        pProdOfferAggrCur[j].m_lVersionID = pProdAggrData->m_lVersionID;
                        return j;
                    }
                }
            }
            lUpOffset = j;
            j = pProdOfferAggrCur[j].m_lAggrOfferSet;
        }
    }
#ifdef MEM_REUSE_MODE

    i = m_poProdOfferAggrData->malloc (pProdAggrData->m_iBillingCycleID);
    if (!i) {
        m_poAggrDetailLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
        m_poAggrDetailLock->V();
        m_poAggrIndexLock->V ();
        i = m_poProdOfferAggrData->malloc (pProdAggrData->m_iBillingCycleID );
    }
#else
    m_poAggrLock->P();
    i = m_poProdOfferAggrData->malloc();
    m_poAggrLock->V();
#endif

    if (!i) {
        Log::log (0, "取实时优惠共享内存地址不足\n");
        THROW(MBC_ProdOfferAggr+21);
    }
    pProdOfferAggrCur[i].m_lItemDisctSeq = pProdAggrData->m_lABMKeyID;
    pProdOfferAggrCur[i].m_lProdOfferID = pProdAggrData->m_lOfferInstID;
    pProdOfferAggrCur[i].m_iBillingCycleID = pProdAggrData->m_iBillingCycleID;
    pProdOfferAggrCur[i].m_iProdOfferCombine = pProdAggrData->m_iCombineID;
    pProdOfferAggrCur[i].m_iPricingMeasure = pProdAggrData->m_iMeasureID;
    pProdOfferAggrCur[i].m_iItemGroupID = pProdAggrData->m_iItemGroupID;
    pProdOfferAggrCur[i].m_iInType = pProdAggrData->m_iInType;
    pProdOfferAggrCur[i].m_lInValue = pProdAggrData->m_lInValue;
    pProdOfferAggrCur[i].m_lValue = pProdAggrData->m_lValue;
    pProdOfferAggrCur[i].m_lDisctValue = pProdAggrData->m_lDisctValue;
    #ifdef ORG_BILLINGCYCLE
    pProdOfferAggrCur[i].m_iOrgBillingCycleID = pProdAggrData->m_iOrgBillingCycleID;
    #endif
    pProdOfferAggrCur[i].m_lVersionID = pProdAggrData->m_lVersionID;
    pProdOfferAggrCur[i].m_lDetailOfferSet = -1;
    if (lUpOffset>0) {
        pProdOfferAggrCur[lUpOffset].m_lAggrOfferSet = i;
    } else {
        pProdOfferAggrCur[i].m_lAggrOfferSet = -1;
        m_poAggrIndexLock->P();
        m_poProdOfferAggrIndex->add (pProdAggrData->m_lOfferInstID, i);
        m_poAggrIndexLock->V();
    }   

    return i;
}

bool ProdOfferDisctAggr::updateAuditOfferDetail(long lAggrOffset, 
                                                AuditDisctAccuDetailData *pOfferDetailData){
    if (!pOfferDetailData) return false;
    ProdOfferAggrData * pProdOfferAggrCur = NULL;
    ProdOfferDetailData * pProdOfferDetailCur = NULL;
    ProdOfferAggrData * pProdOfferAggr;
    unsigned int iOffset = 0;
    long i = 0,j = 0;
    long lOffset = 0,lUpOffset = 0;
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
#ifdef MEM_REUSE_MODE
    m_poProdOfferAggrData->checkSafe();
#endif
    lOffset = pProdOfferAggrCur[lAggrOffset].m_lDetailOfferSet;
    if (lOffset == -1) {
#ifdef MEM_REUSE_MODE
        j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
        if (!j) {
            m_poAggrLock->P();
            m_poAggrIndexLock->P ();
            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
            m_poAggrLock->V();
            m_poAggrIndexLock->V ();
            j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
        }
#else
        m_poAggrDetailLock->P();
        j = m_poProdOfferDetailData->malloc();
        m_poAggrDetailLock->V();
#endif
        if (!j) {
            Log::log (0, "取实时优惠明细共享内存地址不足\n");
            THROW(MBC_ProdOfferAggr+22);
        }
        pProdOfferDetailCur[j].m_lServID = pOfferDetailData->m_lServID;
        pProdOfferDetailCur[j].m_iAcctItemTypeID = pOfferDetailData->m_iAcctItemTypeID;
        pProdOfferDetailCur[j].m_lInCharge = pOfferDetailData->m_lValue;
        pProdOfferDetailCur[j].m_lOutCharge = pOfferDetailData->m_lDisctValue;
        pProdOfferDetailCur[j].m_lItemDetailSeq = pOfferDetailData->m_lABMKeyID;
        pProdOfferDetailCur[j].m_lVersionID = pOfferDetailData->m_lVersionID;
        pProdOfferDetailCur[j].m_lOffset = -1;
        pProdOfferAggrCur[lAggrOffset].m_lDetailOfferSet = j;
        return true;
    }
    while (lOffset>0) {
        if (pProdOfferDetailCur[lOffset].m_lServID == pOfferDetailData->m_lServID
            && pProdOfferDetailCur[lOffset].m_iAcctItemTypeID == pOfferDetailData->m_iAcctItemTypeID
           ) {
            if( pProdOfferDetailCur[lOffset].m_lItemDetailSeq != pOfferDetailData->m_lItemDetailSeq ){
                Log::log(0, "优惠累积量明细[ItemDetailSeq=%ld]与ABM端[ItemDetailSeq=%ld]不一致，更新失败!",
                         pProdOfferDetailCur[lOffset].m_lItemDetailSeq,
                         pOfferDetailData->m_lItemDetailSeq);

                return false;
            } else {
                if(pProdOfferDetailCur[lOffset].m_lVersionID > pOfferDetailData->m_lVersionID ){
                    Log::log(0, "优惠累积量明细[ItemDetailSeq=%ld]的本地VersionID为[%ld],高于ABM端[%ld]，更新失败!",
                             pProdOfferDetailCur[lOffset].m_lItemDetailSeq,
                             pProdOfferDetailCur[lOffset].m_lVersionID,
                             pOfferDetailData->m_lVersionID);
                    return false;
                } else {
                    pProdOfferDetailCur[lOffset].m_lInCharge = pOfferDetailData->m_lValue;
                    pProdOfferDetailCur[lOffset].m_lOutCharge = pOfferDetailData->m_lDisctValue;
                    pProdOfferDetailCur[lOffset].m_lVersionID = pOfferDetailData->m_lVersionID;
        
                    return true;
                }
            }
        }
        lUpOffset = lOffset;
        lOffset = pProdOfferDetailCur[lOffset].m_lOffset;
    }
#ifdef MEM_REUSE_MODE
    j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
    if (!j) {
        m_poAggrLock->P();
        m_poAggrIndexLock->P ();
        revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
        m_poAggrLock->V();
        m_poAggrIndexLock->V ();
        j = m_poProdOfferDetailData->malloc (pOfferDetailData->m_iBillingCycleID );
    }
#else
    j = m_poProdOfferDetailData->malloc();
#endif
    if (!j) {
        Log::log (0, "取实时优惠明细共享内存地址不足\n");
        THROW(MBC_ProdOfferAggr+22);
    }
    pProdOfferDetailCur[j].m_lServID = pOfferDetailData->m_lServID;
    pProdOfferDetailCur[j].m_iAcctItemTypeID = pOfferDetailData->m_iAcctItemTypeID;
    pProdOfferDetailCur[j].m_lInCharge = pOfferDetailData->m_lValue;
    pProdOfferDetailCur[j].m_lOutCharge = pOfferDetailData->m_lDisctValue;
    pProdOfferDetailCur[j].m_lItemDetailSeq = pOfferDetailData->m_lABMKeyID;
    pProdOfferDetailCur[j].m_lVersionID = pOfferDetailData->m_lVersionID;
    pProdOfferDetailCur[j].m_lOffset  = -1;
    pProdOfferDetailCur[lUpOffset].m_lOffset = j;

    return true;
}


long ProdOfferDisctAggr::updateSynOfferAggr(SynDisctAccuData *pProdAggrData){


    return updateAuditOfferAggr(pProdAggrData, true);
}

bool ProdOfferDisctAggr::updateSynOfferDetail(long lAggrOffset,
                                                                                          SynDisctAccuDetailData *pOfferDetailData ){


    return updateAuditOfferDetail(lAggrOffset, pOfferDetailData);
}

#endif

ProdOfferDisctMgr::ProdOfferDisctMgr():ProdOfferAggrAttach()
{

}
void ProdOfferDisctMgr::DelOfferAggr()
{
    if (m_poProdOfferAggrData->exist()) {
        m_poProdOfferAggrData->remove ();
    }

    if (m_poProdOfferDetailData->exist ()) {
        m_poProdOfferDetailData->remove ();
    }
    if (m_poProdOfferAggrIndex->exist()){
        m_poProdOfferAggrIndex->remove ();
    }
    
    //sunjy
    if (m_poProdOfferAggrSeqIndex->exist()) {
        m_poProdOfferAggrSeqIndex->remove ();
    }    
}
void ProdOfferDisctMgr::CreateOfferAggr()
{
    unsigned long ProdOfferAggrBufKey,ProdOfferDetailBufKey,ProdOfferDisctIndexKey;
    char sBuff[10];
        int iDataCount=0,iDetailCount=0;
        
    if(m_bAttached == true || m_bDetailAttached == true || m_bAttachIndex == true)
    {
        Log::log (0, "存在实时优惠累积量内存地址\n");
        THROW(MBC_ProdOfferAggr+10);
    }

    ProdOfferDetailBufKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTBUF");
    ProdOfferAggrBufKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERTOTALBUF");
    ProdOfferDisctIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTINDEX");

#ifdef MEM_REUSE_MODE
    m_poProdOfferAggrData = new SHMData_B<ProdOfferAggrData> (ProdOfferAggrBufKey); 

#else
    m_poProdOfferAggrData = new SHMData<ProdOfferAggrData> (ProdOfferAggrBufKey); 

#endif
        if (!(m_poProdOfferAggrData)) {
                FreeProdOfferAggrShm();
                THROW(MBC_ProdOfferAggr+1); 
        }
        if (!(m_poProdOfferAggrData->exist())) { 
                if(!ParamDefineMgr::getParam(AGGR_PARAM_SEGMENT, AGGR_PARAM_DATA, sBuff))
        {
            THROW(MBC_ProdOfferAggr+2);
        }
        iDataCount = atoi(sBuff);
            m_poProdOfferAggrData->create (iDataCount); 
        }
        else
        {
            THROW(MBC_ProdOfferAggr+3);
        }
  
#ifdef MEM_REUSE_MODE
        m_poProdOfferDetailData = new SHMData_B<ProdOfferDetailData> (ProdOfferDetailBufKey); 

#else
        m_poProdOfferDetailData = new SHMData<ProdOfferDetailData> (ProdOfferDetailBufKey); 

#endif
        if (!(m_poProdOfferDetailData)) { 
                FreeProdOfferAggrShm();
                THROW(MBC_ProdOfferAggr+4); 
        }
         if (!(m_poProdOfferDetailData->exist())) { 
                if(!ParamDefineMgr::getParam(AGGR_PARAM_SEGMENT, AGGR_PARAM_DETAIL, sBuff))
        {
            THROW(MBC_ProdOfferAggr+5);
        }
        iDetailCount = atoi(sBuff);
            m_poProdOfferDetailData->create (iDetailCount);
        }
        else
        {
            THROW(MBC_ProdOfferAggr+6); 
        }

#ifdef MEM_REUSE_MODE
    m_poProdOfferAggrIndex = new SHMIntHashIndex_KR (ProdOfferDisctIndexKey);
#else
    m_poProdOfferAggrIndex = new SHMIntHashIndex (ProdOfferDisctIndexKey);
#endif

        if (!(m_poProdOfferAggrIndex)) {
            FreeProdOfferAggrShm();
            THROW(MBC_ProdOfferAggr+7); 

        } 
        if (!(m_poProdOfferAggrIndex->exist())) {
                if(iDataCount>0)
                m_poProdOfferAggrIndex->create (iDataCount);
	    else
	        THROW(MBC_ProdOfferAggr+8);
	}
	else{
	    THROW(MBC_ProdOfferAggr+9);
	}
	//sunjy
	unsigned long ProdOfferDisctSeqIndexKey;
	ProdOfferDisctSeqIndexKey = IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTSEQINDEX");
    m_poProdOfferAggrSeqIndex = new SHMIntHashIndex (ProdOfferDisctSeqIndexKey);
	if (!(m_poProdOfferAggrSeqIndex)) {
	    FreeProdOfferAggrShm();
	    THROW(MBC_ProdOfferAggr+7); 
	} 
	if (!(m_poProdOfferAggrSeqIndex->exist())) {
		if(iDataCount>0)
	        m_poProdOfferAggrSeqIndex->create (iDataCount);
	    else
	        THROW(MBC_ProdOfferAggr+8);
	}
	else{
	    THROW(MBC_ProdOfferAggr+9);
	}		
}

void ProdOfferDisctMgr::LoadOfferAggr(int iFlowID, int iOrgID, int iBillingCycleID)
{
    char sSQL[4096];
    char sDetailTableName[68],sTableName[68];
    int iTableType;
    bool bDetail=false,bData=false;
    
    sprintf (sSQL, "select table_name,table_type from b_inst_table_list_cfg "
        "where (billflow_id=%d or %d=0) "
        "and   (org_id=%d or %d=0) "
        "and   (billing_cycle_id=%d) and table_type in(85,87) "
        "and   nvl(op_role, 'W')='W' "
        "order by billflow_id,org_id,billing_cycle_id", 
        iFlowID, iFlowID, iOrgID, iOrgID, iBillingCycleID);
    DEFINE_QUERY (qry);

    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        iTableType = qry.field (1).asInteger ();
        if(iTableType == 85){
            strcpy(sTableName,qry.field (0).asString ());
            bData=true;
        }
        if(iTableType == 87){
            strcpy(sDetailTableName,qry.field (0).asString ());
            bDetail = true;
        }
        if(bData && bDetail){
            LoadOfferAggr (sTableName,sDetailTableName);
            bData=false;
            bDetail=false;
        }
    }
    qry.close ();
}

int ProdOfferDisctMgr::LoadOfferAggr(char *sTableName,char *sDetailTableName)
{
    char sSQL[2048];
    ProdOfferTotalData aTotalData;
    ProdOfferDisctAggr aOffAggr;
    vector<ProdOfferAggrData > v_ProdOfferAggr;
    vector<ProdOfferDetailDataEx > v_ProdOfferDetailData;
    /*
    sprintf (sSQL, "select a.item_disct_seq,a.prod_offer_id,a.billing_cycle_id,a.prod_offer_combine, "
        "a.pricing_measure,a.item_group_id,a.in_type,a.in_value, "
        "b.serv_id,b.acct_item_type_id,b.in_charge,b.out_charge "
        "from %s a,%s b where b.item_disct_seq=a.item_disct_seq ", sTableName,sDetailTableName);
    */
    #ifdef ORG_BILLINGCYCLE
        sprintf (sSQL, "select a.item_disct_seq,a.offer_inst_id,a.billing_cycle_id,a.pricing_combine_id, "
        "a.measure_seq,a.item_group_id,a.owner_type ,a.owner_id, "
        "b.serv_id,b.acct_item_type_id,b.in_charge,b.out_charge,a.cor_billing_cycle_id "
        "from %s b,%s a  where b.item_disct_seq=a.item_disct_seq", sTableName,sDetailTableName);
    #else
        sprintf (sSQL, "select a.item_disct_seq,a.offer_inst_id,a.billing_cycle_id,a.pricing_combine_id, "
        "a.measure_seq,a.item_group_id,a.owner_type ,a.owner_id, "
        "b.serv_id,b.acct_item_type_id,b.in_charge,b.out_charge "
        "from %s b,%s a  where b.item_disct_seq=a.item_disct_seq", sTableName,sDetailTableName);
    #endif
    DEFINE_QUERY (qry);

    int j (0);

    Log::log (0, "开始加载total表%s，detail表：%s", sTableName,sDetailTableName);

    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        aTotalData.m_lItemDisctSeq = qry.field (0).asLong ();
        aTotalData.m_lProdOfferID = qry.field (1).asLong ();
        aTotalData.m_iBillingCycleID =qry.field (2).asInteger();
        aTotalData.m_iProdOfferCombine =qry.field (3).asInteger();
        aTotalData.m_iPricingMeasure =qry.field (4).asInteger();
        aTotalData.m_iItemGroupID = qry.field (5).asInteger();
        aTotalData.m_iInType = qry.field (6).asInteger();
        aTotalData.m_lInValue = qry.field (7).asLong();
        aTotalData.m_lServID = qry.field (8).asLong();
        aTotalData.m_iAcctItemTypeID = qry.field (9).asInteger();
        
        aTotalData.m_lValue = qry.field (10).asLong();
        #ifdef ORG_BILLINGCYCLE
          aTotalData.m_iOrgBillingCycleID = qry.field (12).asInteger();
        #endif
        aTotalData.m_bInOutType = true;
        aTotalData.m_bNewRec =  true;
        aOffAggr.doProdOfferDisct(&aTotalData,v_ProdOfferAggr,v_ProdOfferDetailData);
        aTotalData.m_lValue = qry.field (11).asLong();
        aTotalData.m_bInOutType = false;
        aTotalData.m_bNewRec = true;
        aOffAggr.doProdOfferDisct(&aTotalData,v_ProdOfferAggr,v_ProdOfferDetailData);
        j++;
        if(j%10000==0)
            Log::log (0, "\t已加载%d\t条", j);
    }
    qry.close ();

    Log::log (0, "\t已加载%d\t条", j);

    return j;
}

void ProdOfferDisctMgr::GetOfferAggr(char *sTableName,char *sDetailTableName)
{
        long iCount,i=1;
        long iAlreadyOK = 1;
        long iEnd=0;
        long lOffset;
        char sSql[2048],sSql2[2048];
        int iBillingCycleID;
        long lItemDisctSeq;
        int iCountoffer=0;
        int iCountdetail=0;
        
        ProdOfferDisctAggr aProdOffer;
        //--xgs DEFINE_QUERY(qry);
        //--xgs DEFINE_QUERY(qry1);
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferAggrData aProdOfferData;
        ProdOfferDetailData *pProdOfferDetailHead;
        ProdOfferDetailData *pProdOfferDetailCur;
        //--xgs
  vProdOfferAggrData.clear();
  vProdOfferDetailData.clear();
  
#ifdef MEM_REUSE_MODE
    if( m_poProdOfferAggrData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return ;
    }

#endif
        int const batchnum = 20000;
        
        iCount = m_poProdOfferAggrData->getCount();
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
/*   
  #ifdef ORG_BILLINGCYCLE   
    sprintf(sSql,"INSERT INTO %s (ITEM_DISCT_SEQ,OFFER_INST_ID,BILLING_CYCLE_ID,PRICING_COMBINE_ID,MEASURE_SEQ,ITEM_GROUP_ID,OWNER_TYPE,OWNER_ID,VALUE,DISCT_VALUE,COR_BILLING_CYCLE_ID)"
                                        " VALUES (:lItemDisctSeq,:lProdOfferID,:iBillingCycleID,:iProdOfferCombine,"
                                        ":iPricingMeasure,:iItemGroupID,:iInType,:lInValue,:iValue,:iDisctValue,:iOrgBillingCycleID)",sDetailTableName);
        #else
*/
        //--xgs    sprintf(sSql,"INSERT INTO %s (ITEM_DISCT_SEQ,OFFER_INST_ID,BILLING_CYCLE_ID,PRICING_COMBINE_ID,MEASURE_SEQ,ITEM_GROUP_ID,OWNER_TYPE,OWNER_ID,VALUE,DISCT_VALUE)"
        //--xgs                                 " VALUES (:lItemDisctSeq,:lProdOfferID,:iBillingCycleID,:iProdOfferCombine,"
        //--xgs                                 ":iPricingMeasure,:iItemGroupID,:iInType,:lInValue,:iValue,:iDisctValue)",sDetailTableName);
//      #endif
        //--xgs    qry.setSQL (sSql);
        //--xgs    sprintf(sSql2,"INSERT INTO %s(ITEM_DISCT_SEQ,SERV_ID,ACCT_ITEM_TYPE_ID,BILLING_CYCLE_ID,IN_CHARGE,OUT_CHARGE)"
        //--xgs                 " VALUES (:lItemDisctSeq,:lServID,:iAcctItemTypeID,:iBillingCycleID,"
        //--xgs                 ":iInCharge,:iOutCharge)",sTableName);
        //--xgs qry1.setSQL (sSql2);
    #ifdef MEM_REUSE_MODE
    SHM_KEY_BLOCK_ARRAY pBlock[1];
    memset(pBlock, 0 , sizeof(SHM_KEY_BLOCK_ARRAY) );
    m_poProdOfferAggrData->getBlockInfo(pBlock);
    for(int j=0; j< SHM_BLOCK_NUM; j++){
        if(  pBlock->m_aBlock[j].m_iKey ){
            if(j==0){
                iEnd = pBlock->m_aBlock[j].m_iUsedCnt ;
                i = 1;
            }else{
                iEnd = pBlock->m_aBlock[j].m_iUsedCnt ;
                i = pBlock->m_aBlock[j-1].m_iTotalCnt;                          
            }
    ////
    while(i<iEnd)

#else
    while(i<=iCount)
#endif
            {
                        ProdOfferAggrData poTmp;
                        memcpy(&poTmp,&pProdOfferAggrCur[i],sizeof(ProdOfferAggrData));
                        vProdOfferAggrData.push_back(poTmp);                    
/*      //--xgs
                qry.setParamArray (
            "lItemDisctSeq", &(pProdOfferAggrCur[i].m_lItemDisctSeq), sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "lProdOfferID", &(pProdOfferAggrCur[i].m_lProdOfferID), sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "iBillingCycleID", &(pProdOfferAggrCur[i].m_iBillingCycleID),sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "iProdOfferCombine",&(pProdOfferAggrCur[i].m_iProdOfferCombine),sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "iPricingMeasure",&(pProdOfferAggrCur[i].m_iPricingMeasure),sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "iItemGroupID",&(pProdOfferAggrCur[i].m_iItemGroupID),sizeof (ProdOfferAggrData));
        qry.setParamArray (
            "iInType",&(pProdOfferAggrCur[i].m_iInType),sizeof (ProdOfferAggrData));
            
        qry.setParamArray (
            "lInValue",&(pProdOfferAggrCur[i].m_lInValue),sizeof (ProdOfferAggrData));
            
        qry.setParamArray (
            "iValue",&(pProdOfferAggrCur[i].m_lValue),sizeof (ProdOfferAggrData));

        qry.setParamArray (
            "iDisctValue",&(pProdOfferAggrCur[i].m_lDisctValue),sizeof (ProdOfferAggrData));
*/            
/*
        #ifdef ORG_BILLINGCYCLE
          qry.setParamArray (
              "iOrgBillingCycleID",&(pProdOfferAggrCur[i].m_iOrgBillingCycleID),sizeof (ProdOfferAggrData));    
        #endif            
*/              
        //--xgs         qry.execute();
                /*qry.commit();*/
                
        aProdOffer.GetItemDetailHead(pProdOfferAggrCur[i].m_lDetailOfferSet,&pProdOfferDetailHead);
        
        if(!pProdOfferDetailHead){
/*      //--xgs
                        if(i%10000==0||i==iCount){
                    Log::log (0, "\t已导出记录%d\t条", i);
                    qry.commit();
                    qry1.commit();
               }
*/
               i++;
               iAlreadyOK++;
                 continue;
        }
        while(1){
                        ProdOfferDetailData poTmpDetail;
                        poTmpDetail.m_lOffsetCur      = pProdOfferAggrCur[i].m_iBillingCycleID;//借用一下
                        poTmpDetail.m_lOffset         = pProdOfferAggrCur[i].m_lItemDisctSeq;//借用一下
                        poTmpDetail.m_lServID         = pProdOfferDetailHead->m_lServID;
                        poTmpDetail.m_iAcctItemTypeID = pProdOfferDetailHead->m_iAcctItemTypeID;
                        poTmpDetail.m_lInCharge       = pProdOfferDetailHead->m_lInCharge;
                        poTmpDetail.m_lOutCharge      = pProdOfferDetailHead->m_lOutCharge;

                        vProdOfferDetailData.push_back(poTmpDetail);                    
/*      //--xgs
                qry1.setParamArray (
                "lItemDisctSeq", &(pProdOfferAggrCur[i].m_lItemDisctSeq), sizeof (ProdOfferDetailData));
            qry1.setParamArray (
                "lServID", &(pProdOfferDetailHead->m_lServID), sizeof (ProdOfferDetailData));
            qry1.setParamArray (
                "iAcctItemTypeID", &(pProdOfferDetailHead->m_iAcctItemTypeID), sizeof (ProdOfferDetailData));
            qry1.setParamArray (
                "iBillingCycleID", &(pProdOfferAggrCur[i].m_iBillingCycleID), sizeof (ProdOfferDetailData));
            qry1.setParamArray (
                "iInCharge", &(pProdOfferDetailHead->m_lInCharge), sizeof (ProdOfferDetailData));
            qry1.setParamArray (
                "iOutCharge", &(pProdOfferDetailHead->m_lOutCharge), sizeof (ProdOfferDetailData));
                        
                        
                        qry1.execute();
*/                      
                        /*qry1.commit();*/
                        lOffset = pProdOfferDetailHead->m_lOffset;
                        if (lOffset<=0)
                                break;
                        pProdOfferDetailHead = &pProdOfferDetailCur[lOffset];
        }
/*      //--xgs                 
        if(iAlreadyOK%10000==0||iAlreadyOK==iCount){
            Log::log (0, "\t已导出记录%d\t条", iAlreadyOK);
            qry.commit();
            qry1.commit();
       }
*/       

                        if ( ( vProdOfferAggrData.size() % 1000 ) == 0 ){
                                  iCountoffer  += vProdOfferAggrData.size();
                                  iCountdetail += vProdOfferDetailData.size();                            
                                        insertTableCommit(sTableName,sDetailTableName);
                                        Log::log(0,"已导出实例记录:%8d 条，已导出明细记录:%8d 条",iCountoffer,iCountdetail);
                        }
       i++;
       iAlreadyOK++;
    }
          iCountoffer  += vProdOfferAggrData.size();
          iCountdetail += vProdOfferDetailData.size();                            
                insertTableCommit(sTableName,sDetailTableName);
                Log::log(0,"已导出实例记录:%8d 条，已导出明细记录:%8d 条",iCountoffer,iCountdetail);
    
#ifdef MEM_REUSE_MODE
        } ///end if
    } ///end for

#endif
}

int ProdOfferDisctMgr::GetOfferAggrRec(long lProdOfferID)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        unsigned int iOffset;
        long i;
#ifdef MEM_REUSE_MODE
    if( m_poProdOfferAggrData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return 0;
    }

#endif
        
        if (!m_poProdOfferAggrIndex->get(lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);

        i = iOffset;

	#ifdef ORG_BILLINGCYCLE
	  printf ("ITEM_DISCT_S BILLING_C COMBINE_I ME ITEM_GROU OT OWNER_VALUE  VALUE     DISCT\n   ORG_BIL_C");
	  printf ("------------ --------- --------- -- --------- -- ------------ --------- --------- ---------\n");
	#else
	printf ("ITEM_DISCT_S BILLING_C COMBINE_I ME ITEM_GROU OT OWNER_VALUE  VALUE     DISCT\n");
	printf ("------------ --------- --------- -- --------- -- ------------ --------- ---------\n");
	#endif

        while(i>0){
                printf("%12ld ",pProdOfferAggrCur[i].m_lItemDisctSeq);
            printf("%9d ",pProdOfferAggrCur[i].m_iBillingCycleID);
            printf("%9d ",pProdOfferAggrCur[i].m_iProdOfferCombine);
            printf("%2d ",pProdOfferAggrCur[i].m_iPricingMeasure);
            printf("%9d ",pProdOfferAggrCur[i].m_iItemGroupID);
            printf("%2d ",pProdOfferAggrCur[i].m_iInType);
        printf("%12ld ",pProdOfferAggrCur[i].m_lInValue);
        printf("%12ld ",pProdOfferAggrCur[i].m_lValue);
    #ifdef ORG_BILLINGCYCLE
		  printf("%12ld ",pProdOfferAggrCur[i].m_lDisctValue);
		  printf("%9d\n",pProdOfferAggrCur[i].m_iOrgBillingCycleID);
		#else
      printf("%12ld\n",pProdOfferAggrCur[i].m_lDisctValue);
		#endif
		i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }       
        return 1;
}

int ProdOfferDisctMgr::GetOfferAggrDetail(long lItemDisctSeq,long lProdOfferID)
{
        ProdOfferAggrData * pProdOfferAggrCur;
        ProdOfferDetailData *pProdOfferDetailCur;
        ProdOfferDetailData *pProdOfferDetailHead;
        ProdOfferDisctAggr aProdOffer;
        unsigned int iOffset;
        long lOffset;
        long i;
#ifdef MEM_REUSE_MODE

    if( m_poProdOfferAggrData->ifSetRevokeFlag() ){
        Log::log (0, "正在进行内存整理，请稍后再试！" );
        return 0;
    }

#endif
        
        if (!m_poProdOfferAggrIndex->get(lProdOfferID, &iOffset)) {
            return 0;
        }
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
        pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
        i = iOffset;

        while(i>0){
                if(lItemDisctSeq == pProdOfferAggrCur[i].m_lItemDisctSeq )
                   break;
                i = pProdOfferAggrCur[i].m_lAggrOfferSet;
        }
        if(i<=0){
                printf("不存在SEQ:%ld的明细\n",lItemDisctSeq);
        return 0;
        }
        aProdOffer.GetItemDetailHead(pProdOfferAggrCur[i].m_lDetailOfferSet,&pProdOfferDetailHead);
        
    if(!pProdOfferDetailHead){
        
        printf("不存在SEQ:%ld的明细\n",lItemDisctSeq);
        return 0;
    }
        printf ("SERV_ID      ITEM_TYPE IN_CHARGE OUT_CHARG\n");
        printf ("------------ --------- --------- ---------\n");

        while(1){
        printf("%12ld ",pProdOfferDetailHead->m_lServID);
        printf("%9d ",pProdOfferDetailHead->m_iAcctItemTypeID);
        printf("%12ld ",pProdOfferDetailHead->m_lInCharge);
        printf("%12ld\n",pProdOfferDetailHead->m_lOutCharge);
                lOffset = pProdOfferDetailHead->m_lOffset;
                if (lOffset<=0)
                        break;
                pProdOfferDetailHead = &pProdOfferDetailCur[lOffset];
    }   
        return 1;
}

void ProdOfferDisctAggr::setAggrOutput(long lOffset,  ProdOfferAggrData *pAggr,  long lValue,int iAddedValue)
{
        if(pAggr!=NULL)
         {
            pAggr->m_lDisctValue =lValue;
          }
}

void ProdOfferDisctAggr::setDetailOutput(long lOffset,  ProdOfferDetailData *pDetail,  long lValue,  int iAddedValue)
{
         ProdOfferDetailData *ptmpDetail=pDetail;
         if(ptmpDetail==NULL)
                {
                         ptmpDetail = convertDetail (lOffset);
                }
                ptmpDetail->m_lOutCharge = lValue;
}


SeqMgrExt * ProdOfferDisctAggr::m_pItemDisctSeq = 0;


#ifdef MEM_REUSE_MODE

////
void ProdOfferAggrAttach::revoke(const int iBillingCycleID, bool bLockedAggr )
{
    long lkey=0;
    unsigned int uiOff=0;
    long iOffset=0;
    long iLastOffset=0;
    //////这里比较特殊，不是用0表示结尾！
    long lCnt = 0;
    
    int  iOfferInstCnt = 0;
    bool bOffer = false;


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

    
    if( !m_poProdOfferAggrData->exist () ){
        cout<<"错误：OFFER_AGGR共享内存不存在."<<endl;
        return; 
    }
    if( !m_poProdOfferDetailData->exist () ){
        cout<<"错误：OFFER_AGGR_DETAIL共享内存不存在."<<endl;
        return; 
    }
    

    ProdOfferAggrData * poProdOfferAggr = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    
    ///先把索引区的给去掉
    if (m_poProdOfferAggrIndex->exist() ) {
        
        m_poProdOfferAggrData->setRevokeFlag(1);
        m_poProdOfferDetailData->setRevokeFlag(1);
        
        
        sleep(1);
        ProdOfferDisctAggr * pMgr =0;
        if(!bLockedAggr){
            pMgr = new ProdOfferDisctAggr();
            pMgr->lockAllLock();
        }

        SHMIntHashIndex_KR::Iteration iter = m_poProdOfferAggrIndex->getIteration();

        while( iter.next(lkey, uiOff) ){
            bOffer = false;
            iOffset = uiOff;
            while( iOffset>0
              && iBillingCycleID == poProdOfferAggr[iOffset].m_iBillingCycleID){
                
                iOffset =  poProdOfferAggr[iOffset].m_lAggrOfferSet;
                if(iOffset <0)
                    iOffset = 0;

                iter.setValue( iOffset );

                lCnt++;
                if(!bOffer){
                    iOfferInstCnt++;
                    bOffer = true;
                }
            }
            
            if( !iOffset )
                continue;
            
            iLastOffset = iOffset;
            iOffset =  poProdOfferAggr[iOffset].m_lAggrOfferSet;
            while( iOffset>0 ){
                if(iBillingCycleID == poProdOfferAggr[iOffset].m_iBillingCycleID){
                    
                    iOffset =  poProdOfferAggr[iOffset].m_lAggrOfferSet;
                    poProdOfferAggr[iLastOffset].m_lAggrOfferSet =  iOffset;
                    lCnt++;
                    if(!bOffer){
                        iOfferInstCnt++;
                        bOffer = true;
                    }
                    continue;
                }
                iLastOffset = iOffset;
                iOffset =  poProdOfferAggr[iOffset].m_lAggrOfferSet;
            }
            
        }

        ////数据区卸掉重用        
        int iCntBlockAggr = m_poProdOfferAggrData->revoke ( iBillingCycleID );
        int iCntBlockDetail = m_poProdOfferDetailData->revoke ( iBillingCycleID );
    
        if(!bLockedAggr && pMgr) pMgr->freeAllLock();
        m_poProdOfferAggrData->setRevokeFlag(0);
        m_poProdOfferDetailData->setRevokeFlag(0);
        
        if(!bLockedAggr){
            cout<<"共卸载 "<< lCnt <<" 条acct_disct_aggr的数据,涉及商品实例数 "<< iOfferInstCnt <<"。" << endl;
            cout << "key: "<< iBillingCycleID <<" 卸载 "
                << iCntBlockAggr<< " 块aggr数据区, "
                << iCntBlockDetail <<"块detail数据区。" << endl;
        }else{
            Log::log(0, "共卸载%ld条acct_disct_aggr的数据,涉及商品实例数%d,key:%d卸载%d块aggr数据区,%d块detail数据区.",
                lCnt,iOfferInstCnt,iBillingCycleID,iCntBlockAggr,iCntBlockDetail);
        }
        
        if(pMgr){ 
            delete pMgr;  
            pMgr = 0;
        }

    }
 

    
}

void ProdOfferDisctMgr::showDetail(bool bClearRF)
{
    if( !m_poProdOfferAggrData->exist () ){
        cout<<"OFFER_AGGR 共享内存不存在."<<endl;
        return; 
    }
    cout<<"OFFER_AGGR:  ";
    m_poProdOfferAggrData->showDetail();
    


    if( !m_poProdOfferDetailData->exist () ){
        cout<<"OFFER_AGGR_DETAIL 共享内存不存在."<<endl;
        return; 
    }
    cout<<"OFFER_AGGR_DETAIL:  ";
    m_poProdOfferDetailData->showDetail();
    
    if(bClearRF ){
        m_poProdOfferAggrData->setRevokeFlag(0);
        m_poProdOfferDetailData->setRevokeFlag(0);
    }
}
#endif

void ProdOfferDisctMgr::insertTableCommit(char *sTableName,char *sDetailTableName)
{
        char sSql[2048]={0};
        char sSql2[2048]={0};

        DEFINE_QUERY(qry);
        DEFINE_QUERY(qry1);
        
        if(vProdOfferAggrData.size() != 0)      
        {
                sprintf(sSql,"INSERT INTO %s (ITEM_DISCT_SEQ,OFFER_INST_ID,BILLING_CYCLE_ID,PRICING_COMBINE_ID,MEASURE_SEQ,ITEM_GROUP_ID,OWNER_TYPE,OWNER_ID,VALUE,DISCT_VALUE)"
                        " VALUES (:lItemDisctSeq,:lProdOfferID,:iBillingCycleID,:iProdOfferCombine,"
                        ":iPricingMeasure,:iItemGroupID,:iInType,:lInValue,:iValue,:iDisctValue)",sDetailTableName);
                qry.setSQL (sSql);
        
                qry.setParamArray("lItemDisctSeq",    &(vProdOfferAggrData[0].m_lItemDisctSeq), sizeof (ProdOfferAggrData));
                qry.setParamArray("lProdOfferID",     &(vProdOfferAggrData[0].m_lProdOfferID), sizeof (ProdOfferAggrData));
                qry.setParamArray("iBillingCycleID",  &(vProdOfferAggrData[0].m_iBillingCycleID),sizeof (ProdOfferAggrData));
                qry.setParamArray("iProdOfferCombine",&(vProdOfferAggrData[0].m_iProdOfferCombine),sizeof (ProdOfferAggrData));
                qry.setParamArray("iPricingMeasure",  &(vProdOfferAggrData[0].m_iPricingMeasure),sizeof (ProdOfferAggrData));
                qry.setParamArray("iItemGroupID",     &(vProdOfferAggrData[0].m_iItemGroupID),sizeof (ProdOfferAggrData));
                qry.setParamArray("iInType",          &(vProdOfferAggrData[0].m_iInType),sizeof (ProdOfferAggrData));
                qry.setParamArray("lInValue",         &(vProdOfferAggrData[0].m_lInValue),sizeof (ProdOfferAggrData));
                qry.setParamArray("iValue",           &(vProdOfferAggrData[0].m_lValue),sizeof (ProdOfferAggrData));
                qry.setParamArray("iDisctValue",      &(vProdOfferAggrData[0].m_lDisctValue),sizeof (ProdOfferAggrData));
                qry.execute(vProdOfferAggrData.size()); 
                qry.commit();
                qry.close();    
                vProdOfferAggrData.clear();
        }       
        if(vProdOfferDetailData.size() != 0)    {
                sprintf(sSql2,"INSERT INTO %s(ITEM_DISCT_SEQ,SERV_ID,ACCT_ITEM_TYPE_ID,BILLING_CYCLE_ID,IN_CHARGE,OUT_CHARGE)"
                        " VALUES (:lItemDisctSeq,:lServID,:iAcctItemTypeID,:iBillingCycleID,"
                        ":iInCharge,:iOutCharge)",sTableName);
                qry1.setSQL (sSql2);
                qry1.setParamArray("lItemDisctSeq",  &(vProdOfferDetailData[0].m_lOffset), sizeof (ProdOfferDetailData));//借用一下
                qry1.setParamArray("iBillingCycleID",&(vProdOfferDetailData[0].m_lOffsetCur), sizeof (ProdOfferDetailData));//借用一下
                qry1.setParamArray("lServID",        &(vProdOfferDetailData[0].m_lServID), sizeof (ProdOfferDetailData));
                qry1.setParamArray("iAcctItemTypeID",&(vProdOfferDetailData[0].m_iAcctItemTypeID), sizeof (ProdOfferDetailData));
                qry1.setParamArray("iInCharge",      &(vProdOfferDetailData[0].m_lInCharge), sizeof (ProdOfferDetailData));
                qry1.setParamArray("iOutCharge",     &(vProdOfferDetailData[0].m_lOutCharge), sizeof (ProdOfferDetailData));
                qry1.execute(vProdOfferDetailData.size());      
                qry1.commit();
                qry1.close();   
                vProdOfferDetailData.clear();   
        }
        return; 
}
int ProdOfferDisctMgr::NewLoadOfferAggr(char *sTableName,char *sDetailTableName)
{
    char sSQL[2048];
    ProdOfferAggrData *pProdOfferAggrCur;
    ProdOfferDetailData *pProdOfferDetailCur;
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);
//  加载优惠累计
    sprintf (sSQL, "select a.item_disct_seq,a.offer_inst_id,a.billing_cycle_id,a.pricing_combine_id, "
             "a.measure_seq,a.item_group_id,a.owner_type ,a.owner_id,a.value,a.disct_value "
             "from %s a ", sTableName);
    DEFINE_QUERY (qry);
    unsigned i,j,k;
    int n (0);
    Log::log (0, "开始加载total表%s ", sTableName);
    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        //m_poAggrLock->P();
		#ifdef MEM_REUSE_MODE
      	  i = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
       		 if(!i){
            //lockAllLock();
            revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
            //freeAllLock();
            i = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
        	}

		#else
			i = m_poProdOfferAggrData->malloc();
		#endif
        //m_poAggrLock->V();
        pProdOfferAggrCur[i].m_lItemDisctSeq = qry.field (0).asLong ();
        pProdOfferAggrCur[i].m_lProdOfferID = qry.field (1).asLong ();
        pProdOfferAggrCur[i].m_iBillingCycleID = qry.field (2).asInteger();
        pProdOfferAggrCur[i].m_iProdOfferCombine = qry.field (3).asInteger();
        pProdOfferAggrCur[i].m_iPricingMeasure = qry.field (4).asInteger();
        pProdOfferAggrCur[i].m_iItemGroupID = qry.field (5).asInteger();
        pProdOfferAggrCur[i].m_iInType = qry.field (6).asInteger();
        pProdOfferAggrCur[i].m_lInValue = qry.field (7).asLong();
        pProdOfferAggrCur[i].m_lValue = qry.field (8).asInteger();
        pProdOfferAggrCur[i].m_lDisctValue = qry.field (9).asInteger();
        if (m_poProdOfferAggrIndex->get (pProdOfferAggrCur[i].m_lProdOfferID, &k)) {
            pProdOfferAggrCur[i].m_lAggrOfferSet = k;
        } else {
            pProdOfferAggrCur[i].m_lAggrOfferSet = 0;
        }
        m_poProdOfferAggrIndex->add (pProdOfferAggrCur[i].m_lProdOfferID, i);
        m_poProdOfferAggrSeqIndex->add (pProdOfferAggrCur[i].m_lItemDisctSeq, i);
        n++;
        if (n%10000==0)
            Log::log (0, "\t已加载AGGR表%d\t条", n);
    }
    qry.close ();
    Log::log (0, "\t共加载AGGR表%d\t条", n);
    //   加载优惠累计明细
    sprintf (sSQL, "select item_disct_seq,serv_id,acct_item_type_id,billing_cycle_id,in_charge,out_charge "
             "from %s b ", sDetailTableName);
    // DEFINE_QUERY (qry);
    Log::log (0, "开始加载detail表：%s", sTableName,sDetailTableName);
    n=0;
    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        //m_poAggrDetailLock->P();
 		#ifdef MEM_REUSE_MODE
       		 j = m_poProdOfferDetailData->malloc (qry.field (3).asInteger() );
        	if(!j){
            //lockAllLock();
            revoke( m_poProdOfferDetailData->getAutoRevokeKey(), true );
            //freeAllLock();
            j = m_poProdOfferDetailData->malloc (qry.field (3).asInteger() );
      	  }

		#else
        	j = m_poProdOfferDetailData->malloc();
		#endif  
        // m_poAggrDetailLock->V();
        pProdOfferDetailCur[j].m_lOffsetCur = qry.field (0).asLong ();
        pProdOfferDetailCur[j].m_lServID = qry.field (1).asLong ();
        pProdOfferDetailCur[j].m_iAcctItemTypeID =  qry.field (2).asInteger();
        pProdOfferDetailCur[j].m_iBIllingCycleID = qry.field (3).asInteger();
        pProdOfferDetailCur[j].m_lInCharge = qry.field (4).asInteger();
        pProdOfferDetailCur[j].m_lOutCharge = qry.field (5).asInteger();
        if (m_poProdOfferAggrSeqIndex->get (pProdOfferDetailCur[j].m_lOffsetCur, &k)) {
            pProdOfferDetailCur[j].m_lOffset=pProdOfferAggrCur[k].m_lDetailOfferSet;
            pProdOfferAggrCur[k].m_lDetailOfferSet = j;
        } else {
            pProdOfferDetailCur[j].m_lOffset=0;
        }
        n++;
        if (j%10000==0)
            Log::log (0, "\t已加载Detail表%d\t条", n);
    }
    qry.close ();
    Log::log (0, "\t共加载Detail表%d\t条", n);
    return j;
}
void ProdOfferDisctMgr::ExportAggr(char *sTableName)
{
    long iCount,i=1;
    long lOffset;
    char sSql[2048];
    int iBillingCycleID;
    long lItemDisctSeq;
    ProdOfferDisctAggr aProdOffer;
    DEFINE_QUERY(qry);
    ProdOfferAggrData * pProdOfferAggrCur;

    int const batchnum = 20000;

    iCount = m_poProdOfferAggrData->getCount();
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);

    sprintf(sSql,"INSERT INTO %s (ITEM_DISCT_SEQ,OFFER_INST_ID,BILLING_CYCLE_ID,PRICING_COMBINE_ID,"
            "MEASURE_SEQ,ITEM_GROUP_ID,OWNER_TYPE,OWNER_ID,VALUE,DISCT_VALUE)"
            " VALUES (:lItemDisctSeq,:lProdOfferID,:iBillingCycleID,:iProdOfferCombine,"
            ":iPricingMeasure,:iItemGroupID,:iInType,:lInValue,:iValue,:iDisctValue)",sTableName);

    qry.setSQL (sSql);
    while (i<=iCount) {

        qry.setParamArray (
                          "lItemDisctSeq", &(pProdOfferAggrCur[i].m_lItemDisctSeq), sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "lProdOfferID", &(pProdOfferAggrCur[i].m_lProdOfferID), sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "iBillingCycleID", &(pProdOfferAggrCur[i].m_iBillingCycleID),sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "iProdOfferCombine",&(pProdOfferAggrCur[i].m_iProdOfferCombine),sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "iPricingMeasure",&(pProdOfferAggrCur[i].m_iPricingMeasure),sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "iItemGroupID",&(pProdOfferAggrCur[i].m_iItemGroupID),sizeof (ProdOfferAggrData));
        qry.setParamArray (
                          "iInType",&(pProdOfferAggrCur[i].m_iInType),sizeof (ProdOfferAggrData));

        qry.setParamArray (
                          "lInValue",&(pProdOfferAggrCur[i].m_lInValue),sizeof (ProdOfferAggrData));

        qry.setParamArray (
                          "iValue",&(pProdOfferAggrCur[i].m_lValue),sizeof (ProdOfferAggrData));

        qry.setParamArray (
                          "iDisctValue",&(pProdOfferAggrCur[i].m_lDisctValue),sizeof (ProdOfferAggrData));

        qry.execute();
        /*qry.commit();*/

        //  aProdOffer.GetItemDetailHead(pProdOfferAggrCur[i].m_lDetailOfferSet,&pProdOfferDetailHead);
    
        showAggrData(pProdOfferAggrCur[i]);
        
        i++;
        if (i%10000==0||i==iCount) {
            Log::log (0, "\t已导出累计记录%d\t条", i);
            qry.commit();
        }
    }   
    qry.commit();
}
void ProdOfferDisctMgr::showAggrData(ProdOfferAggrData &oData){
    static bool bFirst = true;
    if ( bFirst ) {
        printf ("    序列号    帐务周期    实例标识     "
                #ifdef ORG_BILLINGCYCLE
                "原始帐务周期    "
                #endif
                "定价组合   度量方式   帐目组    输入类型"
                "    输入类型值    优惠前值    优惠掉的值\n");
        bFirst = false;
    }
    printf("%12ld ",oData.m_lItemDisctSeq);
    printf("%9d ",oData.m_iBillingCycleID);
    printf("%12ld", oData.m_lProdOfferID);
    #ifdef ORG_BILLINGCYCLE
    printf("%12d ", oData.m_iOrgBillingCycleID);
    #endif
    printf("%12d ",oData.m_iProdOfferCombine);
    printf("%9d ",oData.m_iPricingMeasure);
    printf("%10d ",oData.m_iItemGroupID);
    printf("%9d ",oData.m_iInType);
    printf("%12ld ",oData.m_lInValue);
    printf("%12d ",oData.m_lValue);
    printf("%12d",oData.m_lDisctValue);
    printf("%12ld\n", oData.m_lAggrOfferSet);
}
void ProdOfferDisctMgr::ExportDetail(char *sDetailTableName)
{
    long iCount,i=1;
    long lOffset;
    char sSql[2048];
    int iBillingCycleID;
    long lItemDisctSeq;
    DEFINE_QUERY(qry);
    ProdOfferDetailData *pProdOfferDetailCur;

    int const batchnum = 20000;

    iCount = m_poProdOfferDetailData->getCount();
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);

    sprintf(sSql,"INSERT INTO %s(ITEM_DISCT_SEQ,SERV_ID,ACCT_ITEM_TYPE_ID,BILLING_CYCLE_ID,IN_CHARGE,OUT_CHARGE)"
            " VALUES (:lItemDisctSeq,:lServID,:iAcctItemTypeID,:iBillingCycleID,"
            ":iInCharge,:iOutCharge)",sDetailTableName);
    qry.setSQL (sSql);
    while (i<=iCount) {


        qry.setParamArray (
                          "lItemDisctSeq", &(pProdOfferDetailCur[i].m_lOffsetCur), sizeof (ProdOfferDetailData));
        qry.setParamArray (
                          "lServID", &(pProdOfferDetailCur[i].m_lServID), sizeof (ProdOfferDetailData));
        qry.setParamArray (
                          "iAcctItemTypeID", &(pProdOfferDetailCur[i].m_iAcctItemTypeID), sizeof (ProdOfferDetailData));
        qry.setParamArray (
                          "iBillingCycleID", &(pProdOfferDetailCur[i].m_iBIllingCycleID), sizeof (ProdOfferDetailData));
        qry.setParamArray (
                          "iInCharge", &(pProdOfferDetailCur[i].m_lInCharge), sizeof (ProdOfferDetailData));
        qry.setParamArray (
                          "iOutCharge", &(pProdOfferDetailCur[i].m_lOutCharge), sizeof (ProdOfferDetailData));


        qry.execute();
  
        showDetailData(pProdOfferDetailCur[i]);
        

        if (i%10000==0||i==iCount) {
            Log::log (0, "\t已导出明细记录%d\t条", i);
            qry.commit();
        }
        i++;
    }
    qry.commit();
}

void ProdOfferDisctMgr::showDetailData(ProdOfferDetailData &oData){
    static bool bFirst = true;
    if ( bFirst ) {
        printf ("    用户标识    帐务周期    帐目类型    优惠前值    优惠后值\n");
        bFirst = false;
    }
    printf("%12ld ",oData.m_lServID);
    printf("%12d", oData.m_iBIllingCycleID);
    printf("%12d ",oData.m_iAcctItemTypeID);
    printf("%9d ",oData.m_lInCharge);
    printf("%9d\n",oData.m_lOutCharge);
}

int ProdOfferDisctMgr::loadOfferAggr(const char* sAggrTable){
    char sSQL[2048];
    ProdOfferAggrData *pProdOfferAggrCur;
    pProdOfferAggrCur = (ProdOfferAggrData *)(* m_poProdOfferAggrData);

//  加载优惠累计
    sprintf (sSQL, "select a.item_disct_seq,a.offer_inst_id,a.billing_cycle_id,a.pricing_combine_id, "
             "a.measure_seq,a.item_group_id,a.owner_type ,a.owner_id,a.value,a.disct_value "
             "from %s a ", sAggrTable);
    DEFINE_QUERY (qry);
    unsigned i,k;
    int n (0);

    Log::log (0, "开始加载优惠累积量表 %s ", sAggrTable);

    try{
    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
    	
       #ifdef MEM_REUSE_MODE
      	  i = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
       		 if(!i){
            //lockAllLock();
            revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
            //freeAllLock();
            i = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
        	}

		#else
			i = m_poProdOfferAggrData->malloc();
		#endif
        pProdOfferAggrCur[i].m_lItemDisctSeq = qry.field (0).asLong ();
        pProdOfferAggrCur[i].m_lProdOfferID = qry.field (1).asLong ();
        pProdOfferAggrCur[i].m_iBillingCycleID = qry.field (2).asInteger();
        pProdOfferAggrCur[i].m_iProdOfferCombine = qry.field (3).asInteger();
        pProdOfferAggrCur[i].m_iPricingMeasure = qry.field (4).asInteger();
        pProdOfferAggrCur[i].m_iItemGroupID = qry.field (5).asInteger();
        pProdOfferAggrCur[i].m_iInType = qry.field (6).asInteger();
        pProdOfferAggrCur[i].m_lInValue = qry.field (7).asLong();
        pProdOfferAggrCur[i].m_lValue = qry.field (8).asInteger();
        pProdOfferAggrCur[i].m_lDisctValue = qry.field (9).asInteger();

        if (m_poProdOfferAggrIndex->get (pProdOfferAggrCur[i].m_lProdOfferID, &k)) {
            pProdOfferAggrCur[i].m_lAggrOfferSet = k;
        } else {
            pProdOfferAggrCur[i].m_lAggrOfferSet = 0;
        }

        m_poProdOfferAggrIndex->add (pProdOfferAggrCur[i].m_lProdOfferID, i);
        m_poProdOfferAggrSeqIndex->add (pProdOfferAggrCur[i].m_lItemDisctSeq, i);

        n++;
 		showAggrData(pProdOfferAggrCur[i]);
    
        if (n%10000==0)
            Log::log (0, "\t已加载 %s 表 %d 条", sAggrTable, n);
    }
    qry.close ();
    }catch (...){
        ALARMLOG28(0, MBC_CLASS_Memory, 117,"导入数据ProdOfferAggrData[ItemDisctSeq = %ld ]异常!",
                   pProdOfferAggrCur[i].m_lItemDisctSeq);
    }
    Log::log (0, "\t共加载 %s 表 %d 条", sAggrTable, n);

    return n;
}
int ProdOfferDisctMgr::loadOfferDetail(const char* sDetailTable){
    char sSQL[2048];
    ProdOfferAggrData *pProdOfferAggrCur;
    ProdOfferDetailData *pProdOfferDetailCur;
    pProdOfferAggrCur = (ProdOfferAggrData *)(*m_poProdOfferAggrData);
    pProdOfferDetailCur = (ProdOfferDetailData *)(*m_poProdOfferDetailData);

    DEFINE_QUERY (qry);
    unsigned j,k;
    int n (0);

    //   加载优惠累计明细
    sprintf (sSQL, "select item_disct_seq,serv_id,acct_item_type_id,billing_cycle_id,in_charge,out_charge "
             "from %s b ", sDetailTable);
    // DEFINE_QUERY (qry);

    Log::log (0, "开始加载优惠明细表：%s", sDetailTable);
    n=0;
    try{
    qry.setSQL (sSQL); qry.open ();
    while (qry.next ()) {
        #ifdef MEM_REUSE_MODE
      	  j = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
       		 if(!j){
            //lockAllLock();
            revoke( m_poProdOfferAggrData->getAutoRevokeKey(), true );
            //freeAllLock();
            j = m_poProdOfferAggrData->malloc (qry.field (2).asInteger() );
        	}

		#else
			j = m_poProdOfferAggrData->malloc();
		#endif
        pProdOfferDetailCur[j].m_lOffsetCur = qry.field (0).asLong ();
        pProdOfferDetailCur[j].m_lServID = qry.field (1).asLong ();
        pProdOfferDetailCur[j].m_iAcctItemTypeID =  qry.field (2).asInteger();
        pProdOfferDetailCur[j].m_iBIllingCycleID = qry.field (3).asInteger();
        pProdOfferDetailCur[j].m_lInCharge = qry.field (4).asInteger();
        pProdOfferDetailCur[j].m_lOutCharge = qry.field (5).asInteger();
        if (m_poProdOfferAggrSeqIndex->get (pProdOfferDetailCur[j].m_lOffsetCur, &k)) {
            pProdOfferDetailCur[j].m_lOffset=pProdOfferAggrCur[k].m_lDetailOfferSet;
            pProdOfferAggrCur[k].m_lDetailOfferSet = j;
        } else {
            pProdOfferDetailCur[j].m_lOffset=0;
        }

        n++;
    
        showDetailData(pProdOfferDetailCur[j]);
        
        if (n%10000==0)
            Log::log (0, "\t已加载 %s 表 %d 条",sDetailTable, n);
    }
    qry.close ();
    }catch (...){
        ALARMLOG28(0, MBC_CLASS_Memory, 117,"导入数据ProdOfferDetailData[ItemDisctSeq = %ld ]异常!",
                   pProdOfferDetailCur[j].m_lOffsetCur);
    }
    Log::log (0, "\t共加载 %s 表 %d 条", sDetailTable, n);

    return n;
}
bool ProdOfferDisctMgr::getConnProcNum()
{
    bool bResult = true;

    if (( m_poProdOfferAggrData->connnum() > 1 )
        || ( m_poProdOfferDetailData->connnum() > 1 )
        || ( m_poProdOfferAggrIndex->connnum() > 1 )
       ) {
        Log::log(0, "检测到其它进程在使用优惠，现在还不能加载，请稍候再试。");
    } else {
        bResult = false;
    }
    return bResult;
}
bool ProdOfferDisctMgr::GetCurProcNum()
{
        bool bResult = true;
        
        if(( m_poProdOfferAggrData->connnum() > 1 )
                || ( m_poProdOfferAggrData->connnum() > 1 )
                || ( m_poProdOfferAggrIndex->connnum() > 1 )
                )
        {
                cout << "检测到其他进程在使用优惠,现在还不能加载/删除优惠...\n\n\n";
                cout << "如果你确实需要加载/删除，请输入命令强制加载/删除" << endl;
        }
        else
        {
                bResult = false;
        }

        return bResult;

}