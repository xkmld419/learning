/*VER: 1*/ 
#ifndef BILLINGCYCLE_H_HEADER_INCLUDED_BD5BDDA8
#define BILLINGCYCLE_H_HEADER_INCLUDED_BD5BDDA8

#define SHM_BILLINGCYCLE_NUM 2048
#define MAX_BILLINGCYCLETYPE_NUM 64

/*	这个宏定义取帐期的模式为实时模式，如果没有这个宏，就是脱机模式	*/
#define REAL_BILLING_CYCLE
/*	结束	*/

#include "Date.h"


//##ModelId=4235284F035A
//##Documentation
//## 帐务周期信息
//## 私有内存
class BillingCycle
{
    friend class UserInfoInterface;

    friend class Serv;

  public:
    //##ModelId=424164500201
    int getBillingCycleID() const;

    //##ModelId=424164500251
    int getBillingCycleType() const;

    //##ModelId=424164500369
    char *getBlockDate() const;

    //##ModelId=42416450039B
    char *getDueDate() const;

    //##ModelId=4241645003D7
    char *getEndDate() const;

    //##ModelId=424164510021
    char *getStartDate() const;

    //##ModelId=424164510071
    char *getState() const;

    //##ModelId=4241645100AE
    char *getStateDate() const;

    bool isFirstDay(Date &d);
    bool isFirstDay(char *sDate);

    bool isLastDay(Date &d);
    bool isLastDay(char *sDate);

    //##ModelId=425B76C800D6
    int getDays();
    //##ModelId=425342B50168
    BillingCycle();

    //##ModelId=42A6E8B3033B
    int m_iBillingCycleID;
    //##ModelId=42A6E8B3034F
    int m_iBillingCycleType;
    //##ModelId=42A6E8B3036D
    int m_iLastID;
    //##ModelId=42A6E8B30381
    char m_sState[4];
    //##ModelId=42A6E8B3039F
    char m_sStartDate[16];
    //##ModelId=42A6E8B303B3
    char m_sEndDate[16];
    //##ModelId=42A6E8B303D1
    char m_sStateDate[16];
    //##ModelId=42A6E8B40011
    char m_sDueDate[16];
    //##ModelId=42A6E8B4002F
    char m_sBlockDate[16];
    //##ModelId=42A6E8B40039
    unsigned int m_iPre;


    //##ModelId=42A6E8B40057
    unsigned int m_iNext;

  
    //##ModelId=42A6E8B4009D
    int m_iParent;

};



#endif /* BILLINGCYCLE_H_HEADER_INCLUDED_BD5BDDA8 */
