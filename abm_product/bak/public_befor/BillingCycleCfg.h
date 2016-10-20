/*VER: 1*/ 
#ifndef BILLINGCYCLECFG_H_HEADER_INCLUDED_BD5ABEFF
#define BILLINGCYCLECFG_H_HEADER_INCLUDED_BD5ABEFF

#include "Date.h"
//##ModelId=42A50B33018A
//##Documentation
//## 配置信息
class BillingCycleCfg
{
  public:
    //##ModelId=42A6DD89011E
    BillingCycleCfg();

    //##ModelId=42A50BB2002F
    //##Documentation
    //## 帐期类型
    int m_iCycleType;


    //##ModelId=42A69F5900FB
    Date m_oLastDate;

    //##ModelId=42A6DCC703DC
    int m_iDelaySec;

};



#endif /* BILLINGCYCLECFG_H_HEADER_INCLUDED_BD5ABEFF */
