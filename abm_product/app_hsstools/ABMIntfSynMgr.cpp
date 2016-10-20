#include "ABMIntfSynMgr.h"
#include "interrupt.h"
#include "errorcode.h"



LocalCacheMgr *ABMIntfSynMgr::m_poLocalCacheMgr=0;
ABMIntfAdmin *ABMIntfSynMgr::m_poABMIntfAdmin=0;
OfferAccuMgr *ABMIntfSynMgr::m_poOfferAccuMgr=0;
ProdOfferDisctAggr *ABMIntfSynMgr::m_poProdOfferMgr=0;

ABMIntfSynMgr::ABMIntfSynMgr(){

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

ABMIntfSynMgr::~ABMIntfSynMgr(){
    DEL_SIN(m_poLocalCacheMgr);
    DEL_SIN(m_poABMIntfAdmin);
    DEL_SIN(m_poOfferAccuMgr);
    DEL_SIN(m_poProdOfferMgr);
}


int ABMIntfSynMgr::run(){

    dataReceiv();

    return 0;
}

void ABMIntfSynMgr::dataReceiv(){

    abm_vec_e_s vEventData;
    abm_d_dv_map_s vMap;
    abm_vec_e_s_it itE;
    abm_d_dv_map_s_it itM;
    abm_vec_d_d_s_it itSBeg, itSEnd;
    long lAggrOffset = 0;

    DenyInterrupt();
    while ( !GetInterruptFlag() ) {
        vEventData.clear();
        vMap.clear();
        switch ( m_poABMIntfAdmin->realtimeGetAllResult(vEventData, vMap) ) {
        case RET_UPD:
            {
                ///调用本地累积量接口处理 
                itE = vEventData.begin();
                for (; itE != vEventData.end(); ++itE) {
                    m_poOfferAccuMgr->updateSynAccu( *itE );
                }
                itM = vMap.begin();
                for (; itM != vMap.end(); ++itM) {
                    lAggrOffset = m_poProdOfferMgr->updateSynOfferAggr(itM->first);
                    if (lAggrOffset <= 0) {
                        continue;
                    }
                    itSEnd = (itM->second).end();
                    for (itSBeg = (itM->second).begin(); itSBeg != itSEnd; ++itSBeg ) {
                        m_poProdOfferMgr->updateSynOfferDetail( lAggrOffset, *itSBeg);
                    }
                }
            }
            break;
        case ECODE_MQ_NOMSG:
            {
                usleep(100000);
                continue;
            }
            break;
        default:
            {
                Log::log(0, "Recieve data from ABM fail!");
            }
            break;
        }
        if ( GetInterruptFlag() ) {
            break;
        }

        #ifdef __BLOW__
        usleep(100000);
        printf(" %s sleep for a while...\n", __FUNCTION__);
        #endif __BLOW__
    }

    return ;
}


DEFINE_MAIN(ABMIntfSynMgr);
