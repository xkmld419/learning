/*VER: 1*/ 
#ifndef BILLINGCYCLEADMIN_H_HEADER_INCLUDED_BD5BDEB0
#define BILLINGCYCLEADMIN_H_HEADER_INCLUDED_BD5BDEB0
#include "BillingCycleBase.h"
class BillingCycleSchedule;
class BillingCycleCfg;
class BillingCycle;

//##ModelId=42A3FAA50061
//##Documentation
//## 帐务周期的服务端类
class BillingCycleAdmin : public BillingCycleBase
{
  public:
    //##ModelId=42A3FFE2007E
    //##Documentation
    //## 构造函数：
    //## 如果数据没有加载，那么加载相关数据
    BillingCycleAdmin();

    //##ModelId=42A4000802EF
    //##Documentation
    //## 数据管理：
    //## 1，新增帐务周期，同时创建相关的表格。
    //## 2，帐期状态更改，改内存并且改数据库。
    //## 状态更改时，先改内存，再改数据库
    //## 
    //## 依据：
    //## 一，某帐期类型的开始时间，间隔类型
    //## 二，兼容周期的秒数
    int admin(
        //##Documentation
        //## 是否立刻返回：
        //## 如果是立刻返回的话，返回的是多少秒后要进行下一次管理。
        bool bBool = false);
    //##ModelId=42A56ED400A8
    BillingCycleSchedule *m_poSchedule;


  private:
    //##ModelId=42A4479203BC
    //##Documentation
    //## 执行第一次的加载数据
    void load();

    //##ModelId=42A447A803C8
    //##ModelId=42A447A803C8
    //##Documentation
    //## 制定维护计划
    void makeSchedule();

    
    //##ModelId=42A44AB402F7
    //##Documentation
    //## 插入一个帐务周期到内存
    //## 可能会导致其他帐期出内存
    int insert(BillingCycle &oBillingCycle);

    
    //##ModelId=42A50F520296
    void synCfg();
    //##ModelId=42A6A4DC0353
    void insertSchedule(BillingCycleSchedule *pSchedule);


    //##ModelId=42A6A50D03D6
    void newCycle(int iCycleType);

    //##ModelId=42A50EE7007F
    //##ModelId=42A6A51502A1
    void changeCycle(int iCycleType);

    int m_iCfgCount;


    //##ModelId=42A50ECB031E
    BillingCycleCfg *m_poCfgList;


};



#endif /* BILLINGCYCLEADMIN_H_HEADER_INCLUDED_BD5BDEB0 */
