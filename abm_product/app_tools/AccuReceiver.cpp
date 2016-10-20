#include "AccuReceiver.h"
#include <pthread.h>
#include "interrupt.h"
#include "errorcode.h"
#include "CommonMacro.h"



LocalCacheMgr *AccuReceiver::m_poLocalCacheMgr=0;
ABMIntfAdmin *AccuReceiver::m_poABMIntfAdmin=0;
OfferAccuMgr *AccuReceiver::m_poOfferAccuMgr=0;
ProdOfferDisctAggr *AccuReceiver::m_poProdOfferMgr=0;

AccuReceiver::AccuReceiver(){
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

AccuReceiver::~AccuReceiver(){
    DEL_SIN(m_poLocalCacheMgr);
    DEL_SIN(m_poABMIntfAdmin);
    DEL_SIN(m_poOfferAccuMgr);
    DEL_SIN(m_poProdOfferMgr);
}


int AccuReceiver::run(){

    dataReceiv();


    return 0;
}


void AccuReceiver::dataReceiv(){

    int iRet ;
    long lEventID;
    vector<AuditEventAccuData *> vSucEventData;
    map<AuditDisctAccuData *, vector<AuditDisctAccuDetailData *> > vSucMap;

    vector<EventAccuData *> vFailEventData;
    vector<EventAccuData *>::iterator itFE;
    map<DisctAccuData*, vector<AuditDisctAccuDetailData *> > vFailMap;
    map<DisctAccuData*, vector<AuditDisctAccuDetailData *> >::iterator itFBeg, itFEnd;
    vector<AuditDisctAccuDetailData *>::iterator itABeg, itAEnd;

    long lAggrOffset = 0;


    DenyInterrupt();
    while (!GetInterruptFlag() ) {
        vSucEventData.clear();
        vSucMap.clear();
        vFailEventData.clear();
        vFailMap.clear();
        lEventID = 0;

        iRet = m_poABMIntfAdmin->asynGetAuditAllResult(vSucEventData, vSucMap, 
                                                       vFailEventData, vFailMap, 
                                                       lEventID);
        switch ( iRet ) {
        case RET_SUC:
            {
                m_poLocalCacheMgr->syncSuccess(lEventID, vSucEventData, vSucMap);
            }
            break;
        case RET_UPD:
            {
                ////A 本地缓冲处理
                if ( m_poLocalCacheMgr->syncFail(lEventID) ) {
                    //continue;
                }
                ////B 调用本地累积量接口处理 
                itFE = vFailEventData.begin();
                for (; itFE != vFailEventData.end(); ++itFE) {
                    m_poOfferAccuMgr->updateAuditAccu( *itFE );
                }
                itFBeg = vFailMap.begin();
                itFEnd = vFailMap.end();
                for (; itFBeg != itFEnd; ++itFBeg) {
                    if ( (lAggrOffset = m_poProdOfferMgr->updateAuditOfferAggr(itFBeg->first)) <= 0 ) {
                        continue;
                    }
                    itAEnd = (itFBeg->second).end();
                    for (itABeg = (itFBeg->second).begin(); itABeg != itAEnd; ++itABeg ) {
                        m_poProdOfferMgr->updateAuditOfferDetail( lAggrOffset, *itABeg);
                    }
                }
            }
            break;
        case ECODE_MQ_NOMSG:
            {
                usleep(1000);
                continue;
            }
            break;
        default:
            {
                m_poLocalCacheMgr->syncFail(lEventID);
                Log::log(0, "从ABM接受事件[ %ld ]的事件、优惠累积量失败!", lEventID);
            }
            break;
        }

        if ( GetInterruptFlag() ) {
            break;
        }
        //Debug.
        #ifdef __BLOW__
        usleep(100000); 
        printf("%s sleep for a while\n", __FUNCTION__); 
        #endif
    }

    return;
}


DEFINE_MAIN(AccuReceiver);
