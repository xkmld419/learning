#include "ABMIntfMgr.h"
#include <pthread.h>
#include "interrupt.h"
#include "errorcode.h"


void* runThreadSend(void *pObject){
    ABMIntfMgr *pInterface = (ABMIntfMgr *)pObject;

    try {
        pInterface->dataSend();
        pthread_exit(NULL);
    } catch (...) {
        pthread_exit((void*)"dataSend执行失败!");
    }

    return;
}
void *runThreadReceiv(void *pObject){
    ABMIntfMgr *pInterface = (ABMIntfMgr *)pObject;
    try {
        pInterface->dataReceiv();
        pthread_exit(NULL);
    } catch (...) {
        pthread_exit((void*)"dataReceiv执行失败!");
    }

    return;
}



LocalCacheMgr *ABMIntfMgr::m_poLocalCacheMgr=0;
ABMIntfAdmin *ABMIntfMgr::m_poABMIntfAdmin=0;
OfferAccuMgr *ABMIntfMgr::m_poOfferAccuMgr=0;
ProdOfferDisctAggr *ABMIntfMgr::m_poProdOfferMgr=0;
int ABMIntfMgr::thread_state=0;

ABMIntfMgr::ABMIntfMgr(){
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

ABMIntfMgr::~ABMIntfMgr(){
    DEL_SIN(m_poLocalCacheMgr);
    DEL_SIN(m_poABMIntfAdmin);
    DEL_SIN(m_poOfferAccuMgr);
    DEL_SIN(m_poProdOfferMgr);
}


void ABMIntfMgr::createSubThread(ABMIntfMgr *pObject, int iMode){
    pthread_t threadid;

    switch ( iMode) {
    case 0:
        if ( pthread_create(&threadid, NULL, runThreadSend, (void *)pObject) ) {
            Log::log(0, "创建线程失败!");
            thread_state = -1;
        }
        break;
    case 1:
        if ( pthread_create(&threadid, NULL, runThreadReceiv, (void *)pObject) ) {
            Log::log(0, "创建线程失败!");
            thread_state = -1;
        }
        break;
    default:
        break;
    }

    return;
}

int ABMIntfMgr::run(){

    createSubThread(this, 0);
    createSubThread(this, 1);

    DenyInterrupt();
    while ( !GetInterruptFlag() ) {
        if ( -1 == thread_state ) {
            THROW(1);
        }
        if ( GetInterruptFlag() ) {
            break;
        }
        sleep(5);
    }

    return 0;
}

void ABMIntfMgr::dataSend(){

    try {
        pthread_detach( pthread_self() );
    } catch (...) {
        Log::log(0,"线程分离失败!");
    }

    thread_state++;

    long lEventID;
    hb_vec_e vEventAccu;
    hb_vec_d_r vDisctResult;

    vector<AuditEventAccuData *> vSucEventData;
    map<AuditDisctAccuData *, vector<AuditDisctAccuDetailData *> > vSucMap;
    vSucEventData.clear();
    vSucMap.clear();
    while ( true ) {
        vEventAccu.clear();
        vDisctResult.clear();
        lEventID = 0;
        
        //Debug...
        #ifdef __BLOW__
        usleep(1000);
        printf("%s sleep for a while \n", __FUNCTION__);
        #endif

        if ( !m_poLocalCacheMgr->getSyncAccu(vEventAccu, vDisctResult, lEventID) ) {
            usleep(1000);
            continue;
        }
        // if( vEventAccu.empty() && vDisctResult.empty() ){
        //     m_poLocalCacheMgr->syncSuccess(lEventID, vSucEventData, vSucMap);
        //     continue;
        // }

        if ( !m_poABMIntfAdmin->asynAuditAllAccu( vEventAccu, vDisctResult, lEventID ) ) {
            m_poLocalCacheMgr->syncFail(lEventID);
            Log::log(0, "发送事件[ %ld ]的事件、优惠累积量数据到ABM失败!", lEventID);
        }
        //Debug...
        #ifdef __BLOW__
        usleep(1000);
        printf("%s sleep for a while \n", __FUNCTION__);
        #endif
    }

    return;
}

void ABMIntfMgr::dataReceiv(){

    try {
        pthread_detach( pthread_self() );
    } catch (...) {
        Log::log(0,"线程分离失败!");
    }
    thread_state++;

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

    while (true) {
        vSucEventData.clear();
        vSucMap.clear();
        vFailEventData.clear();
        vFailMap.clear();
        lEventID = 0;
        switch (iRet = m_poABMIntfAdmin->asynGetAuditAllResult(vSucEventData, vSucMap, 
                                                               vFailEventData, vFailMap, 
                                                               lEventID) ) {
        case RET_SUC:
            {
                m_poLocalCacheMgr->syncSuccess(lEventID, vSucEventData, vSucMap);
            }
            break;
        case RET_UPD:
            {
                ////A 本地缓冲处理
                if ( m_poLocalCacheMgr->syncFail(lEventID) ){
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
        //Debug.
        #ifdef __BLOW__
        usleep(100000); 
        printf("%s sleep for a while\n", __FUNCTION__); 
        #endif
    }

    return;
}


DEFINE_MAIN(ABMIntfMgr);
