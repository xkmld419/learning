/*VER: 1*/ 
#ifndef BILLINGCYCLESCHEDULE_H_HEADER_INCLUDED_BD5BFFF5
#define BILLINGCYCLESCHEDULE_H_HEADER_INCLUDED_BD5BFFF5

#include "Date.h"
//##ModelId=42A407F401FB
//##Documentation
//## 帐期管理指令
class BillingCycleSchedule
{
  public:

    //##ModelId=42A4101002BF
    //##Documentation
    //## 判断当前任务是否执行
    bool check();

    //##ModelId=42A4081C028F
    //##Documentation
    //## op：
    //## 1，new 一个帐务周期类型的帐务周期的相关操作
    //## 2，update 一个帐务周期的状态
    int m_iOperation;
    //##ModelId=42A4086E0279
    //##Documentation
    //## 当new的时候，
    //## 这个里面放帐期类型
    //## 当update的时候，
    //## 这个里面放帐期标识
    int m_iCycleType;
    //##ModelId=42A40E9E005A
    Date m_oDate;
    //##ModelId=42A56E6800DF
    BillingCycleSchedule *m_poNext;


  private:





};



#endif /* BILLINGCYCLESCHEDULE_H_HEADER_INCLUDED_BD5BFFF5 */
