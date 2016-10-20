
#ifndef _ACCU_RECEIVER_H_
#define _ACCU_RECEIVER_H_

#include "abmclnt.h"
#include "ABMInterfaceAdmin.h"
#include "OfferAccuMgr.h"
#include "ProdOfferAggr.h"

#include "Process.h"


/**
 * Òì²½´«Êä
 */
class AccuReceiver : public Process{

public:
    AccuReceiver();
    ~AccuReceiver();

    int run();

    //void runThread(void *pObject);
    //void dataSend();
    void dataReceiv();

private:

    static LocalCacheMgr *m_poLocalCacheMgr;
    static ABMIntfAdmin *m_poABMIntfAdmin;
    static OfferAccuMgr *m_poOfferAccuMgr;
    static ProdOfferDisctAggr *m_poProdOfferMgr;
};

#endif //#ifndef _ACCU_RECEIVER_H_


