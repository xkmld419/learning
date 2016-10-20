/*VER: 2*/ 
#ifndef BILLINGCYCLEMGR_H_HEADER_INCLUDED_BD5BA9DC
#define BILLINGCYCLEMGR_H_HEADER_INCLUDED_BD5BA9DC
#include <vector>
#include "BillingCycleBase.h"
class BillingCycle;

using namespace std;

//##ModelId=42451C810237
//##Documentation
//## 帐务周期客户端类
class BillingCycleMgr : public BillingCycleBase
{
  public:
    //##ModelId=42451C960210
    BillingCycleMgr();

    //##ModelId=42451C960242
    ~BillingCycleMgr();

    //##ModelId=42451C9602BA
    BillingCycle *getBillingCycle(int iCycleType, char *sTime);

    BillingCycle *getOccurBillingCycle(int iCycleType, char *sTime);

    //##ModelId=4250DF7B00DE
    int getBillingCycles(vector <BillingCycle *> &ret, int cycleType, char *beginDate, char *endDate);

    //##ModelId=425B765D0137
    BillingCycle *getBillingCycle(int iBillingCycleID);

    //##
    BillingCycle *getPreBillingCycle(int iBillingCycleID);

    int diffBillingCycle(int iBillingCycle1, int iBillingCycle2);

    bool endBillingCycle(int iBillingCycleID, bool changeDB=false);

    BillingCycle *getCurBillingCycle(int iCycleType, int iOffset);
};



#endif /* BILLINGCYCLEMGR_H_HEADER_INCLUDED_BD5BA9DC */
