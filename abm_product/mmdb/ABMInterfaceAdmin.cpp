
#include "ABMInterfaceAdmin.h"
#ifdef HAVE_ABM
#include "errorcode.h"
#endif

#define MEM0(x) memset( (void *)(&x), 0, sizeof(x) )




LocalCacheMgr *ABMIntfAdmin::m_poLocalCacheMgr=0;
OfferAccuMgr *ABMIntfAdmin::m_poOfferAccuMgr=0;
ABM *ABMIntfAdmin::m_poABMClient=0;

ABMIntfAdmin::ABMIntfAdmin(){
    m_poLocalCacheMgr = new LocalCacheMgr();
    m_poOfferAccuMgr = new OfferAccuMgr();
    m_poABMClient = new ABM();

    if ( !m_poLocalCacheMgr || !m_poOfferAccuMgr || !m_poABMClient) {
        Log::log(0, "申请资源失败!");
        THROW(1);
    }
    if ( m_poABMClient->init() ) {
        Log::log(0, "ABM Client init() FAILED!");
    }
}

ABMIntfAdmin::~ABMIntfAdmin(){
    DEL_SIN(m_poLocalCacheMgr);
    DEL_SIN(m_poOfferAccuMgr);
    DEL_SIN(m_poABMClient);
}

long ABMIntfAdmin::getAccuByInst( long lOfferInstID, long lServID, long lMemberID,
                                  int iAccuTypeID,long lCycleInstID, long lCycleTypeID,
                                  long lBeginTime, long lEndTime, char *sSplitby , 
                                  int iBillingCycleID){
    long lValue = 0;
    EventAccuCond oData;
    abm_vec_e vData;

    MEM0(oData);

    oData.m_lOfferInstID    = lOfferInstID;
    oData.m_lInstMemberID   = lMemberID;
    oData.m_lServID         = lServID;
    oData.m_iAccuTypeID     = iAccuTypeID;
    oData.m_lCycleTypeID    = lCycleTypeID;
    oData.m_lCycleBeginTime = lBeginTime;
    oData.m_lCycleEndTime   = lEndTime;
    oData.m_iBillingCycleID = iBillingCycleID;
    strcpy(oData.m_sSplitBy, sSplitby);

    if ( synQryEventAccu(&oData, vData) ) {
        abm_vec_e_it itr = vData.begin();
        for (; itr != vData.end(); ++itr) {
            lValue += (*itr)->m_lValue;
        }
    }

    return lValue ;
}

int ABMIntfAdmin::getAllAccuByInst(long lOfferInstID,long lServID,long lMemberID,
                                   int iAccuTypeID,long lCycleInstID, long lCycleTypeID, 
                                   long lBeginTime, long lEndTime, vector<AccuData > &vOut,
                                   int iMergeFlag, int iBillingCycleID){
    bool bRet = false;
    EventAccuCond oData;
    AccuData oAccuData;
    abm_vec_e vTemp;

    MEM0(oData);
    MEM0(oAccuData);

    oData.m_lOfferInstID = lOfferInstID;
    oData.m_lInstMemberID = lMemberID;
    oData.m_lServID = lServID;
    oData.m_iAccuTypeID = iAccuTypeID;
    oData.m_lCycleTypeID = lCycleTypeID;
    oData.m_lCycleBeginTime = lBeginTime;
    oData.m_lCycleEndTime = lEndTime;
    oData.m_iBillingCycleID = iBillingCycleID;
    strcpy(oData.m_sSplitBy, QUERY_SCOPE_ALL);

    if ( synQryEventAccu(&oData, vTemp)) {
        if ( (1== iMergeFlag) && (vTemp.size() > 1) ) {
            accuABM2HB(vTemp[0], &oAccuData, lCycleInstID);
            vOut.push_back(oAccuData);
            for ( int i = vTemp.size()-1; i>0; --i) {
                vOut[0].m_lValue += vTemp[i]->m_lValue;
            }
        } else if ( (2 == iMergeFlag) && (vTemp.size()>1) ) {
            bool bFind = false;

            for (int i=vTemp.size()-1; i>=0; --i) {
                bFind = false;
                for (int j=vOut.size()-1; j>=0; --j) {
                    if ( !strcmp(vTemp[i]->m_sSplitBy,vOut[j].m_sSplitby) ) {
                        vOut[j].m_lValue += vTemp[i]->m_lValue;
                        bFind = true;
                    }
                }
                if (!bFind) {
                    accuABM2HB(vTemp[i], &oAccuData, lCycleInstID);
                    vOut.push_back(oAccuData);
                }
            }
        } else {
            for ( int i = 0; i<vTemp.size(); ++i) {
                accuABM2HB(vTemp[i], &oAccuData, lCycleInstID);
                vOut.push_back(oAccuData);
            }
        }
    }

    return vOut.size();
}

bool ABMIntfAdmin::getAll(long lOfferInstID,  vector<AccuData > &vAccuData, 
                          int iBillingCycleID ){

    bool bRet = false;
    EventAccuCond oData;
    AccuData oAccuData;
    abm_vec_e vTemp;
    abm_d_dv_map vResult;

    MEM0(oData);
    MEM0(oAccuData);

    oData.m_lAccuInstID = QUERY_SCOPE_NEG;
    oData.m_lInstMemberID = QUERY_SCOPE_NEG;
    oData.m_lServID = QUERY_SCOPE_NEG;
    oData.m_iAccuTypeID = QUERY_SCOPE_NEG;
    oData.m_lCycleTypeID = QUERY_SCOPE_NEG;
    oData.m_lCycleBeginTime = QUERY_SCOPE_NEG;
    oData.m_lCycleEndTime = QUERY_SCOPE_NEG;
    strcpy(oData.m_sSplitBy, QUERY_SCOPE_ALL);

    oData.m_lOfferInstID = lOfferInstID;
    oData.m_iBillingCycleID = iBillingCycleID;

    if (bRet = synQryAll(&oData, vTemp,0, vResult)) {
        abm_vec_e_it itr = vTemp.begin();
        for ( ;itr != vTemp.end(); ++itr) {
            accuABM2HB(*itr, &oAccuData);
            vAccuData.push_back(oAccuData);
        }
    }

    return bRet;
}

void ABMIntfAdmin::accuABM2HB(EventAccuData *pABMData, AccuData *pHBData, 
                              long lCycleInstID){

    CycleInstance *pCycle = 0;
    char sBegin[32]={0}, sEnd[32]={0};

    pHBData->m_lAccuInstID = pABMData->m_lAccuInstID;
    pHBData->m_lOfferInstID  = pABMData->m_lOfferInstID;    
    pHBData->m_lMemberID = pABMData->m_lInstMemberID;   
    pHBData->m_lServID = pABMData->m_lServID;         
    pHBData->m_iAccuTypeID = pABMData->m_iAccuTypeID;     
    sprintf(sBegin, "%ld", pABMData->m_lCycleBeginTime); 
    sprintf(sEnd, "%ld", pABMData->m_lCycleEndTime);   
    strcpy(pHBData->m_sSplitby, pABMData->m_sSplitBy);
    #ifdef SPLIT_ACCU_BY_BILLING_CYCLE
    pHBData->m_iBillingCycleID = pABMData->m_iBillingCycleID;
    #endif
    pHBData->m_lValue = pABMData->m_lValue;     
    pHBData->m_lVersionID =pABMData->m_lVersionID;

    if ( lCycleInstID ) {
        pHBData->m_lCycleInstID = lCycleInstID;
    } else {
        if ( m_poOfferAccuMgr->getAccCycle( pABMData->m_lCycleTypeID,
                                            sBegin, sEnd, pCycle) ) {
            if (pCycle) {
                pHBData->m_lCycleInstID = pCycle->m_lCycleInstID;
            } else {
                pHBData->m_lCycleInstID = 0;
            }
        } else {
            pHBData->m_lCycleInstID = 0; 
        }
    }

    return;
}

bool ABMIntfAdmin::synQryEventAccu(EventAccuCond *pData, abm_vec_e &vOut){

    QryAccuCmd *pCmd = new QryAccuCmd();
    QryAccuRlt *pResult = new QryAccuRlt();

    if ( !pCmd || !pResult) {
        DEL_SIN(pCmd);
        DEL_SIN(pResult);
        Log::log(0, "申请资源失败!");
        THROW(1);
    }
    pCmd->clear();
    if ( !pCmd->addEvent(*pData) ) {
        DEL_SIN(pCmd);
        DEL_SIN(pResult);
        Log::log(0, "添加查询条件失败!");
        return false;
    }
    m_poABMClient->beginTrans();
    if ( m_poABMClient->synExecute(pCmd, pResult) ||
         pResult->getResultCode() ) {
        DEL_SIN(pCmd);
        DEL_SIN(pResult);
        Log::log(0, "查询失败!");
        return false;
    }

    pResult->getEvent(vOut); 

    DEL_SIN(pCmd);
    DEL_SIN(pResult);

    return true;
}

bool ABMIntfAdmin::updateAccByInst(long lOfferInstID, long lMemberID, long lServID, 
                                   int iAccuTypeID, long lCycleTypeID, long lBeginTime,
                                   long lEndTime, long lValue,
                                   int iBillingCycleID, char *sSplitby,
                                   char *sAccuLevel, bool bReplace, long lVersionID) {
    EventAccuData oData;

    MEM0(oData);

    //sAccuLevel is useless 

    oData.m_lOfferInstID = lOfferInstID;
    oData.m_lInstMemberID = lMemberID;
    oData.m_lServID = lServID;
    oData.m_iAccuTypeID = iAccuTypeID;
    oData.m_lCycleTypeID = lCycleTypeID;
    oData.m_lCycleBeginTime = lBeginTime;
    oData.m_lCycleEndTime = lEndTime;
    oData.m_iBillingCycleID = iBillingCycleID;
    strcpy(oData.m_sSplitBy, sSplitby);
    oData.m_lValue = lValue;
    oData.m_lVersionID = lVersionID;

    return synUpdEventAccu(&oData, bReplace);
}

bool ABMIntfAdmin::synUpdEventAccu(EventAccuData *pData, bool bReplace){

    if ( !pData ) {
        Log::log(0, "输入数据有误!");
        return false;
    }

    if ( bReplace ) {   //目前只提供了覆盖的支持
        UpdateAccuCmd *pCmd = new UpdateAccuCmd();
        UpdateAccuRlt *pResult = new UpdateAccuRlt();

        if ( !pCmd || !pResult) {
            DEL_SIN(pCmd);
            DEL_SIN(pResult);
            Log::log(0, "申请资源失败!");
            THROW(1);
        }
        pCmd->clear();
        if ( !pCmd->addEvent(*pData) ) {
            DEL_SIN(pCmd);
            DEL_SIN(pResult);
            Log::log(0, "添加查询条件失败!");
            return false;
        }
        m_poABMClient->beginTrans();
        if ( m_poABMClient->synExecute(pCmd, pResult) ||
             pResult->getResultCode() ) {
            DEL_SIN(pCmd);
            DEL_SIN(pResult);
            Log::log(0, "查询失败!");
            return false;
        }
        DEL_SIN(pCmd);
        DEL_SIN(pResult);
    } else {

        return false;
    }


    return true;
}

void ABMIntfAdmin::printOfferInst(long lOfferInstID){
    vector<AccuData> vAccuData;

    if (getAll(lOfferInstID, vAccuData)) {
        vector<AccuData>::iterator itr = vAccuData.begin();
        printf("OfferInstID ...\n");
        for (; itr != vAccuData.end(); ++itr) {
            ;
        }
    }
}

bool ABMIntfAdmin::getAllDisctAggr( ProdOfferAggrData *pData, Value *pResult){          
    bool bRet = false;
    DisctAccuCond oData;
    abm_vec_d vDisct;

    MEM0(oData);

    oData.m_lOfferInstID      =pData->m_lProdOfferID;     
    oData.m_iBillingCycleID   =pData->m_iBillingCycleID;  
    #ifdef ORG_BILLINGCYCLE    
    oData.m_iOrgBillingCycleID=pData->m_iOrgBillingCycleID;
    #else
    oData.m_iOrgBillingCycleID = QUERY_SCOPE_NEG;
    #endif                 

    oData.m_iCombineID        =pData->m_iProdOfferCombine;       
    oData.m_iMeasureID        =pData->m_iPricingMeasure;       
    oData.m_iItemGroupID      =pData->m_iItemGroupID;     
    oData.m_iInType           =pData->m_iInType;          
    oData.m_lInValue          =pData->m_lInValue;      

    if ( bRet = synQryDisctAggr(&oData, vDisct) ) {
        if (vDisct.size()) {
            pResult->m_lValue = abs(vDisct[0]->m_lDisctValue);
        } else {
            pResult->m_lValue = 0;
        }
        pResult->m_iType = NUMBER;
    }

    return bRet;
}

bool ABMIntfAdmin::getAllItemAggr( ProdOfferAggrData *pData, Value *pResult){          
    bool bRet = false;
    DisctAccuCond oData;
    abm_vec_d vDisct;

    MEM0(oData);

    oData.m_lOfferInstID      =pData->m_lProdOfferID;     
    oData.m_iBillingCycleID   =pData->m_iBillingCycleID;  
    #ifdef ORG_BILLINGCYCLE    
    oData.m_iOrgBillingCycleID=pData->m_iOrgBillingCycleID;
    #else
    oData.m_iOrgBillingCycleID = QUERY_SCOPE_NEG;
    #endif                 

    oData.m_iCombineID        =pData->m_iProdOfferCombine;       
    oData.m_iMeasureID        =pData->m_iPricingMeasure;       
    oData.m_iItemGroupID      =pData->m_iItemGroupID;     
    oData.m_iInType           =pData->m_iInType;          
    oData.m_lInValue          =pData->m_lInValue;      

    if ( bRet = synQryDisctAggr(&oData, vDisct) ) {
        if ( vDisct.size() ) {
            pResult->m_lValue = vDisct[0]->m_lValue;
        } else {
            pResult->m_lValue = 0;
        }
        pResult->m_iType = NUMBER;
    }

    return bRet;
}

bool ABMIntfAdmin::getAllDisct( long lOfferInstID, int iBillingCycleID, int iCombineID, 
                                int iMeasureID, int iItemGroupID, int iInType, 
                                long lInValue, DisctAccuData *pDisctData, 
                                abm_vec_d_d &vDisctDetail, int iOrgBillingCycleID) {          
    DisctAccuCond oData;
    abm_d_dv_map vResult;

    MEM0(oData);

    oData.m_lOfferInstID      = lOfferInstID;     
    oData.m_iBillingCycleID   = iBillingCycleID;  
    oData.m_iOrgBillingCycleID= iOrgBillingCycleID;
    oData.m_iCombineID        = iCombineID;       
    oData.m_iMeasureID        = iMeasureID;       
    oData.m_iItemGroupID      = iItemGroupID;     
    oData.m_iInType           = iInType;          
    oData.m_lInValue          = lInValue;      

    if (!synQryDisct(&oData, vResult) ) {
        return false;
    }

    if ( vResult.begin() != vResult.end() ) {
        pDisctData = (vResult.begin())->first;
        vDisctDetail = (vResult.begin())->second;
    }

    return true;
}

bool ABMIntfAdmin::getItemDetail(ProdOfferTotalData *pData, Value *pValue){

    DisctAccuData *pDisct = NULL;
    abm_vec_d_d vDisctDetail;

    if ( !getAllDisct(pData->m_lProdOfferID,
                      pData->m_iBillingCycleID,
                      pData->m_iProdOfferCombine,
                      pData->m_iPricingMeasure,
                      pData->m_iItemGroupID,
                      pData->m_iInType,
                      pData->m_lInValue,
                      pDisct, 
                      vDisctDetail
                #ifdef ORG_BILLINGCYCLE
                      ,pData->m_iOrgBillingCycleID
                #endif
                     ) ) {

        return false;
    }

    abm_vec_d_d_it itDCur = vDisctDetail.begin(), itDEnd = vDisctDetail.end();
    for (; itDCur != itDEnd; ++itDCur) {
        if ( (*itDCur)->m_lServID != pData->m_lServID )
            continue;
        if ( (*itDCur)->m_iAcctItemTypeID != pData->m_iAcctItemTypeID )
            continue;

        if ( pData->m_bInOutType ) {
            pValue->m_lValue = (*itDCur)->m_lValue;
        } else {
            pValue->m_lValue = (*itDCur)->m_lDisctValue;
        }
        pValue->m_iType = NUMBER;

        break;
    }

    return true;
}

bool ABMIntfAdmin::getAllAggr(long lOfferInstID,int iBillingCycleID, 
                              vector<ProdOfferTotalData>& voPOTData,
                              int iOrgBillingCycleID, long lServID ) {

    bool bRet = false;
    DisctAccuCond oData;
    ProdOfferTotalData oPOTData;
    abm_vec_e vTemp;
    abm_d_dv_map vResult;
    abm_vec_d_d vDisctDetail;
    abm_vec_d_d_it itDD;

    MEM0(oData);
    MEM0(oPOTData);

    oData.m_iCombineID        =QUERY_SCOPE_NEG;       
    oData.m_iMeasureID        =QUERY_SCOPE_NEG;     
    oData.m_iItemGroupID      =QUERY_SCOPE_NEG;
    oData.m_iInType           =QUERY_SCOPE_NEG;
    oData.m_lInValue          =QUERY_SCOPE_NEG;

    oData.m_lOfferInstID      = lOfferInstID;     
    oData.m_iBillingCycleID   = iBillingCycleID;  
    oData.m_iOrgBillingCycleID= iOrgBillingCycleID;

    if ( bRet = synQryAll(0, vTemp, &oData, vResult) ) {
        abm_d_dv_map_it itRCur, itREnd = vResult.end();
        for ( itRCur = vResult.begin(); itRCur != itREnd; ++itRCur) {
            oPOTData.m_lItemDisctSeq = itRCur->first->m_lItemDisctSeq;
            oPOTData.m_lProdOfferID = lOfferInstID; 
            oPOTData.m_iBillingCycleID = iBillingCycleID;
            oPOTData.m_iProdOfferCombine = itRCur->first->m_iCombineID;
            oPOTData.m_iPricingMeasure = itRCur->first->m_iMeasureID;  
            oPOTData.m_iItemGroupID =itRCur->first->m_iItemGroupID;     
            oPOTData.m_iInType = itRCur->first->m_iInType;         
            oPOTData.m_lInValue = itRCur->first->m_lInValue;      
            oPOTData.m_bNewRec = true;       
            #ifdef ORG_BILLINGCYCLE
            oPOTData.m_iOrgBillingCycleID = iOrgBillingCycelID;
            #endif

            vDisctDetail = itRCur->second;
            itDD = vDisctDetail.begin();
            for (; itDD != vDisctDetail.end(); ++itDD) {
                if ( (lServID && (*itDD)->m_lServID == lServID) || 
                     (lServID ==0) ) {
                    oPOTData.m_lServID = lServID;
                    oPOTData.m_iAcctItemTypeID  = (*itDD)->m_iAcctItemTypeID;
                    oPOTData.m_lValue  = (*itDD)->m_lValue;
                    oPOTData.m_bInOutType  = true;
                    voPOTData.push_back(oPOTData);

                    oPOTData.m_lValue  = (*itDD)->m_lDisctValue;
                    oPOTData.m_bInOutType  = false;
                    voPOTData.push_back(oPOTData);
                }
            }
        }

    }

    return bRet;
}

bool ABMIntfAdmin::synQryDisctAggr(DisctAccuCond *pCondition, abm_vec_d &vDisctAggr){

    QryAccuCmd *m_poCmd = new QryAccuCmd();
    QryAccuRlt *m_poResult = new QryAccuRlt();

    if ( !m_poCmd || !m_poResult) {
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        Log::log(0,"申请资源失败!");
        THROW(1);
    }
    m_poCmd->clear();
    m_poABMClient->beginTrans();
    if ( !m_poCmd->addDisct(*pCondition) ||
         m_poABMClient->synExecute(m_poCmd, m_poResult) ||
         m_poResult->getResultCode()
       ) {
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        Log::log(0, "查询失败!");
        return false;
    }

    m_poResult->getDisct(vDisctAggr);

    DEL_SIN(m_poCmd);
    DEL_SIN(m_poResult);

    return true;
}

bool ABMIntfAdmin::synQryDisct(DisctAccuCond *pCondition, abm_d_dv_map &vMap ){

    QryAccuCmd *m_poCmd = new QryAccuCmd();
    QryAccuRlt *m_poResult = new QryAccuRlt();

    if ( !m_poCmd || !m_poResult) {
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        Log::log(0, "申请资源失败!");
        THROW(1);
    }

    abm_vec_d vDisctAggr;

    m_poCmd->clear();
    if ( !m_poCmd->addDisct(*pCondition) ) {
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        Log::log(0,"添加查询条件失败!");
        return false;
    }
    m_poABMClient->beginTrans();
    if ( m_poABMClient->synExecute(m_poCmd, m_poResult) ||
         m_poResult->getResultCode() ) {
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        Log::log(0, "查询失败!");
        return false;
    }

    if ( !m_poResult->getDisct(vDisctAggr)) {   //没有记录
        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
        return true;
    }
    abm_vec_d_it itDCur = vDisctAggr.begin(), itDEnd = vDisctAggr.end();
    abm_vec_d_d vTemp;
    for ( ; itDCur != itDEnd; ++itDCur ) {
        vTemp.clear();
        if ( !m_poResult->getDisctDetail(*itDCur, vTemp) ) {
            Log::log(0, "ERROR：没有明细记录!");
            continue;
        }
        vMap.insert(pair<DisctAccuData *, abm_vec_d_d > (*itDCur, vTemp) );
    }

    DEL_SIN(m_poCmd);
    DEL_SIN(m_poResult);

    return true;
}

bool ABMIntfAdmin::updateDisct(ProdOfferAggrData *pAggr,ProdOfferDetailData *pDetail,
                               bool bReplace){

    if ( !pAggr || !pDetail ) {
        return false;
    }

    bool bRet = false;
    DisctAccuData oDisct;
    DisctAccuDetailData oDisctDetail;
    abm_vec_d_d vDisctDetail;

    MEM0(oDisct);
    MEM0(oDisctDetail);

    oDisct.m_lOfferInstID       = pAggr->m_lProdOfferID;
    oDisct.m_iBillingCycleID    = pAggr->m_iBillingCycleID;
    #ifdef ORG_BILLINGCYCLE      
    oDisct.m_iOrgBillingCycleID = pAggr->m_iOrgBillingCycleID;
    #else
    oDisct.m_iOrgBillingCycleID = QUERY_SCOPE_NEG;
    #endif
    oDisct.m_iCombineID         = pAggr->m_iProdOfferCombine;
    oDisct.m_iMeasureID         = pAggr->m_iPricingMeasure;
    oDisct.m_iItemGroupID       = pAggr->m_iItemGroupID;
    oDisct.m_iInType            = pAggr->m_iInType;
    oDisct.m_lInValue           = pAggr->m_lInValue;
    oDisct.m_lValue             = pAggr->m_lValue;
    oDisct.m_lDisctValue        = pAggr->m_lDisctValue;
    oDisct.m_lVersionID         = pAggr->m_lVersionID;

    oDisctDetail.m_iAcctItemTypeID = pDetail->m_iAcctItemTypeID;
    oDisctDetail.m_iBillingCycleID = pAggr->m_iBillingCycleID;/////
    oDisctDetail.m_lServID         = pDetail->m_lServID;
    oDisctDetail.m_lValue          = pDetail->m_lInCharge;
    oDisctDetail.m_lDisctValue     = pDetail->m_lOutCharge;
    oDisctDetail.m_lVersionID      = pDetail->m_lVersionID;

    vDisctDetail.push_back(&oDisctDetail);

    return synUpdDisct(&oDisct, vDisctDetail, bReplace);
}

bool ABMIntfAdmin::synUpdDisct(DisctAccuData *pAggr, abm_vec_d_d &vDetail,
                               bool bReplace){

    if ( !pAggr || vDetail.empty() ) {
        Log::log(0, "输入错误!");
        return false;
    }

    if ( bReplace ) {
        UploadAccuCmd *m_poCmd = new UploadAccuCmd();
        UploadAccuRlt *m_poResult = new UploadAccuRlt();

        if ( !m_poCmd || !m_poResult) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            Log::log(0, "申请资源失败!");
            return false;
        }
        m_poCmd->clear();
        if ( !m_poCmd->addDisct(*pAggr) ) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            Log::log(0, "添加数据失败!");
            return false;
        }

        abm_vec_d_d_it itD = vDetail.begin();
        for ( ; itD != vDetail.end(); ++itD) {
            if ( !m_poCmd->addDisctDetail( *(*itD) ) ) {
                DEL_SIN(m_poCmd);
                DEL_SIN(m_poResult);
                Log::log(0, "添加数据失败!");
                return false;
            }
        }
        m_poABMClient->beginTrans();
        if ( m_poABMClient->synExecute(m_poCmd, m_poResult) ||
             m_poResult->getResultCode() ) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            Log::log(0, "更新失败!");
            return false;
        }

        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
    } else {
        UpdateAccuCmd *m_poCmd = new UpdateAccuCmd();
        UpdateAccuRlt *m_poResult = new UpdateAccuRlt();

        if ( !m_poCmd || !m_poResult) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            return false;
        }
        m_poCmd->clear();

        if ( !m_poCmd->addDisct(*pAggr) ) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            return false;
        }

        abm_vec_d_d_it itD = vDetail.begin();
        for ( ; itD != vDetail.end(); ++itD) {
            if ( !m_poCmd->addDisctDetail( *(*itD) ) ) {
                DEL_SIN(m_poCmd);
                DEL_SIN(m_poResult);
                return false;
            }
        }
        m_poABMClient->beginTrans();
        if ( m_poABMClient->synExecute(m_poCmd, m_poResult) ||
             m_poResult->getResultCode() ) {
            DEL_SIN(m_poCmd);
            DEL_SIN(m_poResult);
            return false;
        }

        DEL_SIN(m_poCmd);
        DEL_SIN(m_poResult);
    }

    return true;
}

bool ABMIntfAdmin::asynAuditAllAccu(hb_vec_e &vEventAccu, hb_vec_d_r &vDisctResult, 
                                    const long &lEventID) {
    static AuditAccuCmd *m_poCmd =new  AuditAccuCmd();
    void *pTemp = 0;

    if ( !m_poCmd ) {
        return false;
    }

    m_poCmd->clear(lEventID);   ////设置TransID

    hb_vec_e_it it = vEventAccu.begin();
    for (; it != vEventAccu.end(); ++it) {
        pTemp = static_cast<void*>(*it);
        if ( !m_poCmd->addEvent( *static_cast<AuditEventAccuCond*> (pTemp) ) ) {
            //DEL_SIN(m_poCmd);
            Log::log(0, "添加更新条件失败!");
            return false;
        }
    }

    hb_vec_d_r_it itRCur, itREnd = vDisctResult.end();
    hb_vec_d_d vDetail;
    hb_vec_d_d_it itDCur, itDEnd;

    for ( itRCur = vDisctResult.begin(); itRCur != itREnd; ++itRCur) {
        pTemp = static_cast<void*>((*itRCur).m_pDisct);
        if ( !m_poCmd->addDisct( *static_cast<AuditDisctAccuCond*> (pTemp) ) ) {
            Log::log(0, "添加更新条件失败!");
            //DEL_SIN(m_poCmd);
            return false;
        }
        vDetail = (*itRCur).m_vDetail;
        itDEnd = vDetail.end();
        for ( itDCur = vDetail.begin(); itDCur != itDEnd; itDCur++) {
            pTemp = static_cast<void*>(*itDCur);
            if ( !m_poCmd->addDisctDetail( *static_cast<AuditDisctAccuDetailCond *> (pTemp) ) ) {
                //DEL_SIN(m_poCmd);
                Log::log(0, "添加更新条件失败!");
                return false;
            }
        }
    }
    m_poABMClient->beginTrans();
    if ( m_poABMClient->asynExecute(m_poCmd, GROUP_ASYN) ) {
        //DEL_SIN(m_poCmd);
        Log::log(0, "命令执行失败!");
        return false;
    }

    //DEL_SIN(m_poCmd);

    return true;
}


int ABMIntfAdmin::asynGetAuditAllResult(vector<AuditEventAccuData *> &vSucEventData, 
                                        map<AuditDisctAccuData *, vector<AuditDisctAccuDetailData *> > &vSucMap,
                                        vector<EventAccuData *> &vFailEventData,
                                        map<DisctAccuData*, vector<AuditDisctAccuDetailData *> > &vFailMap,
                                        long &lEventID){

    int iRet ;
    static abm_vec_d_a vSucDisct;
    static abm_vec_d vFailDisct;
    static abm_vec_d_d_a vDisctDetail;
    static abm_vec_d_a_it it;

    AuditAccuRlt oRlt;
    static AuditAccuRlt *pRlt = &oRlt;  //haha
    static AuditAccuFailRlt01 * pFailRlt = (AuditAccuFailRlt01 *)pRlt;

    if ( !pRlt || !pFailRlt) {
        Log::log(0, "申请资源失败!");
        THROW(1);
    }

    //m_poABMClient->beginTrans();
    iRet = m_poABMClient->asynGetResult(pRlt, GROUP_ASYN, 0); //非阻塞调用
    lEventID = pRlt->getTransID();
    if ( 0 != iRet ) {
        if ( ECODE_MQ_NOMSG == iRet ) {
            return iRet;
        }
        return RET_OTR;
    }

    if ( iRet = pRlt->getResultCode() ) {
        if ( ECODE_ACCU_AUDIT_FAIL == iRet ) {//稽核更新失败，更新本地的
            vFailDisct.clear();
            pFailRlt->getEvent(vFailEventData) ;
            iRet = pFailRlt->getDisct(vFailDisct);
            while ( iRet ) {
                vDisctDetail.clear();
                pFailRlt->getDisctDetail(vFailDisct[iRet-1], vDisctDetail);
                vFailMap.insert( pair<DisctAccuData*, abm_vec_d_d_a >
                                 (vFailDisct[--iRet], vDisctDetail) );
            }
            //DEL_SIN(m_poResult);
            return RET_UPD;
        } else {
            //DEL_SIN(m_poResult);
            return RET_OTR;
        }
    } else {    //更新成功的和新增的记录返回;或者没有消息记录
        vSucDisct.clear();
        pRlt->getEvent(vSucEventData) ;
        iRet = pRlt->getDisct(vSucDisct);
        while ( iRet ) {
            vDisctDetail.clear();
            pRlt->getDisctDetail(vSucDisct[iRet-1], vDisctDetail);
            vSucMap.insert( pair<AuditDisctAccuData*, abm_vec_d_d_a >
                            (vSucDisct[--iRet], vDisctDetail) );
        }

        if ( vSucEventData.empty() && vSucMap.empty() ) {//没有消息记录的时候走这里
            return ECODE_MQ_NOMSG;
        }
        //DEL_SIN(m_poResult);
        return RET_SUC;
    }
}


int ABMIntfAdmin::realtimeGetAllResult(abm_vec_e_s &vEventAccu, abm_d_dv_map_s &vMap){

    int iRet ;
    static abm_vec_d_s vDisct;
    static abm_vec_d_d_s vDisctDetail;
    static abm_vec_d_s_it it;

    static SynAccuRlt *m_poResult = new SynAccuRlt();

    if (!m_poResult) {
        Log::log(0, "申请资源失败!");
        THROW(1);
    }

    //m_poABMClient->beginTrans();
    iRet = m_poABMClient->asynGetResult(m_poResult, 0);
    if ( 0 != iRet ) {
        if ( ECODE_MQ_NOMSG == iRet ) {
            //DEL_SIN(m_poResult);
            return iRet ;
        }
        //DEL_SIN(m_poResult);
        return RET_OTR;
    }

    if ( m_poResult->getResultCode() ) {
        //DEL_SIN(m_poResult);
        return RET_OTR;
    }

    vDisct.clear();
    m_poResult->getEvent(vEventAccu);
    iRet = m_poResult->getDisct(vDisct);
    while ( iRet ) {
        vDisctDetail.clear();
        m_poResult->getDisctDetail(vDisct[iRet - 1], vDisctDetail);
        vMap.insert( pair<SynDisctAccuData*, abm_vec_d_d_s >
                     (vDisct[--iRet], vDisctDetail) );
    }
    if( vEventAccu.empty() && vDisct.empty() ){
        return ECODE_MQ_NOMSG;
    }
    //DEL_SIN(m_poResult);
    return RET_UPD;
}


bool ABMIntfAdmin::synQryAll(EventAccuCond *pEventCond, abm_vec_e &vEventAccu, 
                             DisctAccuCond *pDisctCond, abm_d_dv_map &mDisctAccu){

    if ( !pEventCond && !pDisctCond ) {
        return false;
    }

    AlmQryAccuCmd *pCmd = new AlmQryAccuCmd();
    AlmQryAccuRlt *pRlt = new AlmQryAccuRlt();

    if ( !pCmd || !pRlt) {
        DEL_SIN(pCmd);
        DEL_SIN(pRlt);
        Log::log(0, "申请资源失败!");
        return false;
    }
    pCmd->clear();
    if ( pEventCond ) {
        if ( !pCmd->addEvent(*pEventCond) ) {
            DEL_SIN(pCmd);
            DEL_SIN(pRlt);
            return false;
        }
    } else { //
        if ( !pCmd->addDisct(*pDisctCond) ) {
            DEL_SIN(pCmd);
            DEL_SIN(pRlt);
            return false;
        }
    }

    m_poABMClient->beginTrans();
    if ( m_poABMClient->synExecute(pCmd, pRlt) ||
         pRlt->getResultCode() ) {
        DEL_SIN(pCmd);
        DEL_SIN(pRlt);
        return false;
    }

    static abm_vec_d vDisct;
    static abm_vec_d_d vDisctDetail;
    int iRet = 0;

    vDisct.clear();
    if ( pEventCond ) {
        pRlt->getEvent(vEventAccu);
    } else {
        iRet = pRlt->getDisct(vDisct);
        while ( iRet ) {
            vDisctDetail.clear();
            pRlt->getDisctDetail(vDisct[iRet - 1], vDisctDetail);
            mDisctAccu.insert( pair<DisctAccuData*, abm_vec_d_d >
                               (vDisct[--iRet], vDisctDetail) );
        }
    }

    DEL_SIN(pCmd);
    DEL_SIN(pRlt);

    return true;
}

int ABMIntfAdmin::queryAuditResult( const long &lEventID, abm_vec_e_a & vSucEventData, 
                                     abm_d_dv_map_a &vSucMap,
                                     vector<EventAccuData *> &vFailEventData,
                                     map<DisctAccuData*, vector<AuditDisctAccuDetailData *> > &vFailMap){

    static QryAuditAccuCmd *pCmd = new QryAuditAccuCmd;
    QryAuditAccuRlt oResult;
    static QryAuditAccuRlt *pResult = &oResult;
    static AuditAccuFailRtl *pFailResult = (AuditAccuFailRtl*)(pResult);

    static hb_vec_e vEventAccu;
    static hb_vec_d_r vDisctResult;
    static abm_vec_d_a vSucDisct;
    static abm_vec_d vFailDisct;
    static abm_vec_d_d_a vDisctDetail;

    void *pTemp = 0;
    int iRet = 0;

    if (!pCmd ) {
        DEL_SIN(pCmd);
        Log::log(0, "申请资源失败!");
        THROW(1);
    }

    pCmd->clear(lEventID);
    vEventAccu.clear();
    vDisctResult.clear();
    if ( !m_poLocalCacheMgr->getSyncAccuByEventID(lEventID, vEventAccu, vDisctResult) ) {
        return RET_OTR;
    } else {
        hb_vec_e_it it = vEventAccu.begin();
        for (; it != vEventAccu.end(); ++it) {
            pTemp = static_cast<void*>(*it);
            if ( !pCmd->addEvent( *static_cast<AuditEventAccuCond*> (pTemp) ) ) {
                Log::log(0, "添加更新查询条件失败!");
                return RET_OTR;
            }
        }

        hb_vec_d_r_it itRCur, itREnd = vDisctResult.end();
        hb_vec_d_d vDetail;
        hb_vec_d_d_it itDCur, itDEnd;

        for ( itRCur = vDisctResult.begin(); itRCur != itREnd; ++itRCur) {
            pTemp = static_cast<void*>((*itRCur).m_pDisct);
            if ( !pCmd->addDisct( *static_cast<AuditDisctAccuCond*> (pTemp) ) ) {
                Log::log(0, "添加更新查询条件失败!");
                return RET_OTR;
            }
            vDetail = (*itRCur).m_vDetail;
            itDEnd = vDetail.end();
            for ( itDCur = vDetail.begin(); itDCur != itDEnd; itDCur++) {
                pTemp = static_cast<void*>(*itDCur);
                if ( !pCmd->addDisctDetail( *static_cast<AuditDisctAccuDetailCond *> (pTemp) ) ) {
                    Log::log(0, "添加更新查询条件失败!");
                    return RET_OTR;
                }
            }
        }
    }
    m_poABMClient->beginTrans();
    if ( m_poABMClient->synExecute(pCmd, pResult) ) {
        Log::log(0, "ABM执行失败!");
        return RET_OTR;
    }

    if ( iRet = pResult->getResultCode() ) {
        if ( ECODE_ACCU_AUDIT_FAIL == iRet ) {//稽核更新失败，更新本地的
            vFailDisct.clear();
            pFailResult->getEvent(vFailEventData) ;
            iRet = pFailResult->getDisct(vFailDisct);
            while ( iRet ) {
                vDisctDetail.clear();
                pFailResult->getDisctDetail(vFailDisct[iRet-1], vDisctDetail);
                vFailMap.insert( pair<DisctAccuData*, abm_vec_d_d_a >
                                 (vFailDisct[--iRet], vDisctDetail) );
            }
            return RET_UPD;
        } else {
            return RET_OTR;
        }
    } else {    //更新成功的和新增的记录返回;或者没有消息记录
        vSucDisct.clear();
        pResult->getEvent(vSucEventData) ;
        iRet = pResult->getDisct(vSucDisct);
        while ( iRet ) {
            vDisctDetail.clear();
            pResult->getDisctDetail(vSucDisct[iRet-1], vDisctDetail);
            vSucMap.insert( pair<AuditDisctAccuData*, abm_vec_d_d_a >
                            (vSucDisct[--iRet], vDisctDetail) );
        }

        return RET_SUC;
    }

    return RET_OTR;
}


