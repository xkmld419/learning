
#ifndef _ABM_INTF_MGR_H_
#define _ABM_INTF_MGR_H_

#include "abmclnt.h"
#include "ABMInterfaceAdmin.h"
#include "OfferAccuMgr.h"
#include "ProdOfferAggr.h"

#include "Process.h"


/**
 * Òì²½´«Êä
 */
class ABMIntfMgr : public Process{

public:
    ABMIntfMgr();
    ~ABMIntfMgr();

    void createSubThread(ABMIntfMgr *pObject, int iMode);
    int run();

    //void runThread(void *pObject);
    void dataSend();
    void dataReceiv();

private:

    static LocalCacheMgr *m_poLocalCacheMgr;
    static ABMIntfAdmin *m_poABMIntfAdmin;
    static OfferAccuMgr *m_poOfferAccuMgr;
    static ProdOfferDisctAggr *m_poProdOfferMgr;
    static int thread_state ;
};

#endif //#ifndef _ABM_INTF_MGR_H_


