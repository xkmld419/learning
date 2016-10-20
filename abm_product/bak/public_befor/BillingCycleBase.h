/*VER: 1*/ 
#ifndef BILLINGCYCLEBASE_H_HEADER_INCLUDED_BD5BF7D0
#define BILLINGCYCLEBASE_H_HEADER_INCLUDED_BD5BF7D0

/*
	共享内存ID的定义
*/
//0X450		帐务周期数据区
#define	SHM_BILLINGCYCLE_DATA		(IpcKeyMgr::getIpcKey(-1,"SHM_BillingCycle_Data"))
//#define	SHM_BILLINGCYCLE_DATA		0X450
//0X451		帐务周期类型索引
#define	SHM_BILLINGCYCLE_TYPEINDEX	(IpcKeyMgr::getIpcKey(-1,"SHM_BillingCycle_Index"))
//#define	SHM_BILLINGCYCLE_TYPEINDEX	0X451

#include "BillingCycle.h"
#include "SHMData.h"
#include "SHMIntHashIndex.h"
#include "IpcKey.h"

//##ModelId=42A3C76B0069
class BillingCycleBase
{
  public:
    //##ModelId=42A410DE021A
    BillingCycleBase();

  protected:
    //##ModelId=42A3F16D0299
    static SHMData <BillingCycle> *m_poCycleData;

    //##ModelId=42A3F19C0156
    static SHMIntHashIndex *m_poTypeIndex;

    //##ModelId=42A3F68902A4
    static bool m_bAttached;

};



#endif /* BILLINGCYCLEBASE_H_HEADER_INCLUDED_BD5BF7D0 */
