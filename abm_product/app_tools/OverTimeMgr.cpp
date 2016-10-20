#include "OverTimeMgr.h"
#include "interrupt.h"

bool OverTimeMgr::m_bExit = false;
ProdOfferDisctAggr *OverTimeMgr::m_poProdOfferMgr =0;
OfferAccuMgr *OverTimeMgr::m_poOfferAccuMgr = 0;
LocalCacheMgr *OverTimeMgr::m_poLocalCacheMgr;
AccuLockMgr *OverTimeMgr::m_poAccuLockMgr;    
ABMIntfAdmin *OverTimeMgr::m_poABMAdmin;      
OverTimeMgr::OverTimeMgr(){

    m_poAccuLockMgr = new AccuLockMgr();
    m_poLocalCacheMgr = new LocalCacheMgr();
    m_poABMAdmin = new ABMIntfAdmin();
    m_poProdOfferMgr = new ProdOfferDisctAggr();
    m_poOfferAccuMgr = new OfferAccuMgr();

    if ( !m_poAccuLockMgr || !m_poLocalCacheMgr || !m_poABMAdmin ||
         !m_poProdOfferMgr || !m_poOfferAccuMgr ) {
        Log::log(0, "申请资源失败!");
        THROW(1);
    }

    m_iTime = 0;
}

OverTimeMgr::~OverTimeMgr(){
    DEL_SIN ( m_poAccuLockMgr );
    DEL_SIN ( m_poLocalCacheMgr ) ;
    DEL_SIN ( m_poABMAdmin );
    DEL_SIN ( m_poProdOfferMgr );
    DEL_SIN ( m_poOfferAccuMgr );
}

int OverTimeMgr::run(){

    if ( !prepare() ) {
        return 0;
    }

    int iCount = 0, idle = 0;

    DenyInterrupt();
    while ( true ) {
        if ( m_bExit) {
            break;
        }
        if ( (0 != iCount) && (idle < m_iTime) ) {
            idle++;
            sleep(60);
            continue;
        }
        Date tempDate;
        tempDate.addMin(-m_iTime);
        m_oDate = Date( tempDate.toString() );

        while ( true ) {
            if (!localBufProc())
                break;
        }
        if ( m_bExit ) {
            break;
        }
        sleep(2);   //等待缓冲区解锁
        while ( true ) {
            if ( !accuLockProc() )
                break;
        }
        if ( m_bExit ) {
            break;
        }
        idle = 0;
        iCount++;

        if ( GetInterruptFlag() ) {
            break;
        }
    }
    return 0;
}

bool OverTimeMgr::localBufProc(){
    long lEventID = 0;

    abm_vec_e_a vEventData;
    abm_d_dv_map_a vMap;

    vector<EventAccuData *> vFailEventData;
    vector<EventAccuData *>::iterator itFE;
    map<DisctAccuData*, vector<AuditDisctAccuDetailData *> > vFailMap;
    map<DisctAccuData*, vector<AuditDisctAccuDetailData *> >::iterator itFBeg, itFEnd;
    vector<AuditDisctAccuDetailData *>::iterator itABeg, itAEnd;

    long lAggrOffset = 0;

    while ( true ) {
        if ( m_poLocalCacheMgr->overdue(m_oDate, lEventID) ) {
            vEventData.clear();
            vMap.clear();
            switch (m_poABMAdmin->queryAuditResult(lEventID, vEventData, vMap,
                                                   vFailEventData, vFailMap ) ) {
            case RET_SUC:
                {
                    m_poLocalCacheMgr->syncSuccess(lEventID, vEventData, vMap);
                    break;
                }
            case RET_UPD: 
                {
                    m_poLocalCacheMgr->syncFail(lEventID);

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
                    break;
                }
            default:
                {
                    m_poLocalCacheMgr->syncFail(lEventID);
                    Log::log(0, "稽核结果查询失败!");
                    break;
                }
            }
        } else {
            return false;
        }
        if ( GetInterruptFlag() ) {
            m_bExit = true;
            return false;
        }
    }

    return true;
}

bool OverTimeMgr::accuLockProc(){
    long lEventID = 0;

    while ( true ) {
        if ( m_poAccuLockMgr->overdue(m_oDate, lEventID) ) {
            ;
        } else {
            return false;
        }
        if (GetInterruptFlag()) {
            m_bExit = true;
            return false;
        }
    }

    return true;
}

bool OverTimeMgr::prepare(){
    bool bForce = false;
    bool bRet = true;
    if ( g_argc < 2) {
        printUsage();
    }

    for (int iTemp = 1; iTemp < g_argc; ++iTemp ) {
        switch ( g_argv[iTemp][1] | 0x20) {
        case 's':
            m_iTime = atoi(g_argv[++iTemp]);
            break;
        case 'f':
            bForce = true;
            break;
        default:
            printUsage();
            bRet = false;
            break;
        }
    }
    if ( m_iTime < MINIMUM_INTERVAL ) {
        if ( !bForce ) {
            Log::log(0, "您未指定或指定的超时管理运行时间间隔太小，采用默认值%d分!", 
                     DEFAULT_INTERVAL);
            m_iTime = DEFAULT_INTERVAL;
        }
    }

    return bRet;
}

void OverTimeMgr::printUsage(){
    Log::log( 0,"\t超时管理%s\n"
              "\t\t%s -s|-S minutes : 指定超时管理运行时间间隔(分)\n"
              "\t\t%s -f : 强制使用指定的运行时间间隔",
              g_argv[0], g_argv[0], g_argv[0] );
}

DEFINE_MAIN(OverTimeMgr);


