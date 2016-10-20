#ifndef _ABM_INTF_SYN_MGR_H_
#define _ABM_INTF_SYN_MGR_H_

#include "LocalCacheMgr.h"
#include "ABMInterfaceAdmin.h"
#include "abmclnt.h"
#include "OfferAccuMgr.h"
#include "ProdOfferAggr.h"
#include "Process.h"

class ABMIntfSynMgr : public Process {
public:
    ABMIntfSynMgr();
    ~ABMIntfSynMgr();
    int run();
    void dataReceiv();
private:
    static LocalCacheMgr *m_poLocalCacheMgr;
    static ABMIntfAdmin *m_poABMIntfAdmin;
    static OfferAccuMgr *m_poOfferAccuMgr;
    static ProdOfferDisctAggr *m_poProdOfferMgr;
};


#endif //#ifndef _ABM_INTF_SYN_MGR_H_

