/*VER: 1*/ 
#ifndef DATA_MAKER_H
#define DATA_MAKER_H

#include "Process.h"
#include "HashList.h"
#include "AttrTransMgr.h"


#include "Environment.h"

#include "Discount.h"
#include "DisctApplyer.h"
#include "PricingInfo.h"
#include "PricingInfoMgr.h"
#include "PricingMeasure.h"
#include "PricingResult.h"
#include "Tariff.h"
#include "EventIDMaker.h"

#include <vector>

using namespace std;


class DataMaker : public Process
{
  public:
    DataMaker();
    int run();

  private:
    void printUsage();
    bool prepare();
    void stat(StdEvent *pEvent);
    
  private:
    StdEvent m_poEvent[1];
//    AttrTransMgr * m_poAttrTrans;
//    BillingCycleMgr * m_poBCM;

    int  m_iEventType;
    int  m_iBillingCycle;
    Date m_oBeginDate;

};

#endif

