/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
//  
// All rights reserved.

#ifndef LONGEVENTTYPERULE_H_HEADER_INCLUDED_BD7EB633
#define LONGEVENTTYPERULE_H_HEADER_INCLUDED_BD7EB633

#include "Environment.h"

class LongEventTypeRuleMgr;

//##ModelId=4281782D02C9
//##Documentation
//## 长途事件类型判断规则
class LongEventTypeRule
{
  friend class LongEventTypeRuleMgr;
  friend class ParamInfoMgr;
  friend class AssPriData;

  public:
    //##ModelId=4281844B00FD
    int getRuleID() const;

  
    //##ModelId=4281846300D0
    int getOutEventTypeID() const;

    //##ModelId=4281904801FB
    ~LongEventTypeRule();

    //##ModelId=42819E7E006B
    LongEventTypeRule();


  private:
	
	//add by yangks 05.06.16
	//源事件类型
	int m_iSourceEventType;
	
    //##ModelId=428179980358
    int m_iRuleID;

    //##ModelId=42817AA8001A
    int m_iInEventTypeID;

    //##ModelId=42817AE0013C
    int m_iLongTypeID;

    //##ModelId=42817B020326
    int m_iRoamTypeID;

    //##ModelId=42817B260260
    int m_iCallingSPTypeID;

    //##ModelId=42817B4200CF
    int m_iCalledSPTypeID;

    //##ModelId=42817B510302
    int m_iCallingServiceTypeID;

    //##ModelId=42817B650256
    int m_iCalledServiceTypeID;

    //##ModelId=42817B72027D
    int m_iCarrierTypeID;

    //##ModelId=42817B7D003E
    int m_iBusinessKey;

    //##ModelId=42817B9802DC
    int m_iOutEventTypeID;

    //##ModelId=42817C9501A9
    LongEventTypeRule *m_poNextEvent;

    //##ModelId=42817CF4025A
    LongEventTypeRule *m_poNextRule;

#ifndef PRIVATE_MEMORY
    unsigned int m_iNextEventOffset;
    unsigned int m_iNextRuleOffset;
#endif

};



//##ModelId=42817DFB02F8
//##Documentation
//## 长途类型判断管理类
class LongEventTypeRuleMgr
{
  public:
    //##ModelId=428180AA0011
    LongEventTypeRuleMgr();

  
    //##ModelId=428180DC0366
    ~LongEventTypeRuleMgr();

  
    //##ModelId=428180E701F9
    static void loadRule();

  
    //##ModelId=428180EF029B
    static void unloadRule();
    
    //##ModelId=428180F601D3
    static bool getRule(EventSection const *_pEvent, 
    		LongEventTypeRule** _ppRule);


  private:
    //##ModelId=42817EC803D9
    static LongEventTypeRule *m_poRule;

    //##ModelId=4281AE010170
    static bool m_bInit;
    
};

#endif /* LONGEVENTTYPERULE_H_HEADER_INCLUDED_BD7EB633 */
