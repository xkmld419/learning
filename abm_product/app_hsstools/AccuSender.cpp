#include "AccuSender.h"
#include <pthread.h>
#include "interrupt.h"
#include "errorcode.h"


LocalCacheMgr *AccuSender::m_poLocalCacheMgr=0;
ABMIntfAdmin *AccuSender::m_poABMIntfAdmin=0;
OfferAccuMgr *AccuSender::m_poOfferAccuMgr=0;
ProdOfferDisctAggr *AccuSender::m_poProdOfferMgr=0;

AccuSender::AccuSender(){
    m_poLocalCacheMgr = new LocalCacheMgr();
    m_poABMIntfAdmin = new ABMIntfAdmin();

    m_poOfferAccuMgr = new OfferAccuMgr();
    m_poProdOfferMgr = new ProdOfferDisctAggr();
    if ( !m_poLocalCacheMgr || !m_poABMIntfAdmin ||
         !m_poOfferAccuMgr || !m_poProdOfferMgr ) {
        Log::log(0, "申请资源失败!");
        THROW(1);
    }
}

AccuSender::~AccuSender(){
    DEL_SIN(m_poLocalCacheMgr);
    DEL_SIN(m_poABMIntfAdmin);
    DEL_SIN(m_poOfferAccuMgr);
    DEL_SIN(m_poProdOfferMgr);
}


int AccuSender::run(){

        dataSend();

    return 0;
}

void AccuSender::dataSend(){

    long lEventID;
    hb_vec_e vEventAccu;
    hb_vec_d_r vDisctResult;

    vector<AuditEventAccuData *> vSucEventData;
    map<AuditDisctAccuData *, vector<AuditDisctAccuDetailData *> > vSucMap;
    vSucEventData.clear();
    vSucMap.clear();

    DenyInterrupt();
    while ( !GetInterruptFlag() ) {
        vEventAccu.clear();
        vDisctResult.clear();
        lEventID = 0;

        if ( !m_poLocalCacheMgr->getSyncAccu(vEventAccu, vDisctResult, lEventID) ) {
            usleep(1000);
            continue;
        }
        //获取时已经做了检查
        //if ( vEventAccu.empty() && vDisctResult.empty() ) {
        //    m_poLocalCacheMgr->syncSuccess(lEventID, vSucEventData, vSucMap);
        //    continue;
        //}
        if ( !m_poABMIntfAdmin->asynAuditAllAccu( vEventAccu, vDisctResult, lEventID ) ) {
            m_poLocalCacheMgr->syncFail(lEventID);
            Log::log(0, "Send event[ %ld ] data to ABM server FAILED!", lEventID);
        }
        //Debug...
        #ifdef __BLOW__
        usleep(100000);
        printf("%s sleep for a while \n", __FUNCTION__);
        #endif
        if ( GetInterruptFlag() ) {
            break;
        }
    }

    return;
}


DEFINE_MAIN(AccuSender);
