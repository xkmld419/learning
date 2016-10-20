#ifndef OVERTIMEMGR_INCLUDED_HEADER_H
#define OVERTIMEMGR_INCLUDED_HEADER_H

/**
 * date :   2011-1-17 
 * func :   超时管理 
 */


#include "LocalCacheMgr.h"
#include "AccuLock.h"
#include "ABMInterfaceAdmin.h"
#include "Process.h"
#include "Date.h"

const int DEFAULT_INTERVAL = 30;    //默认超时时间间隔(分)
const int MINIMUM_INTERVAL = 5;     //最小超时时间间隔(分)

class OverTimeMgr : public Process {
public:
    OverTimeMgr();
    ~OverTimeMgr();

    int run();
    bool localBufProc();
    bool accuLockProc();

private:
    bool prepare();
    void printUsage();
private:
    Date m_oDate;       //超时界点：早于这个时间点的数据为超时数据

    static LocalCacheMgr *m_poLocalCacheMgr;
    static AccuLockMgr *m_poAccuLockMgr;
    static ABMIntfAdmin *m_poABMAdmin;
    
    static OfferAccuMgr *m_poOfferAccuMgr;
    static ProdOfferDisctAggr *m_poProdOfferMgr;

    int m_iTime;        //运行间隔(分)
    static bool m_bExit;
};


#endif //#ifndef OVERTIMEMGR_INCLUDED_HEADER_H

