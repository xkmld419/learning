
#ifndef _ACCU_SENDER_H_
#define _ACCU_SENDER_H_

#include "abmclnt.h"
#include "ABMInterfaceAdmin.h"
#include "OfferAccuMgr.h"
#include "ProdOfferAggr.h"

#include "Process.h"


/**
 * Òì²½´«Êä
 */
class AccuSender : public Process{

public:
    AccuSender();
    ~AccuSender();

    int run();

    //void runThread(void *pObject);
    void dataSend();
    //void dataReceiv();

private:

    static LocalCacheMgr *m_poLocalCacheMgr;
    static ABMIntfAdmin *m_poABMIntfAdmin;
    static OfferAccuMgr *m_poOfferAccuMgr;
    static ProdOfferDisctAggr *m_poProdOfferMgr;
};

#endif //#ifndef _ACCU_SENDER_H_


