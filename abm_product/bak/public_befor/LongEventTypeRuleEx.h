/*VER: 2*/ 
#ifndef _LONG_EVENT_TYPE_RULE_EX_H_
#define _LONG_EVENT_TYPE_RULE_EX_H_

#include "Environment.h"


class LongEventTypeRuleMgrEx;

//##ModelId=4281782D02C9
//##Documentation
//## 长途事件类型判断规则
class LongEventTypeRuleEx
{
  friend class LongEventTypeRuleMgrEx;
  friend class ParamInfoMgr;
  friend class AssPriData;

  public:
    //##ModelId=4281844B00FD
    int getRuleID() const;

  
    //##ModelId=4281846300D0
    int getOutEventTypeID() const;

    //##ModelId=4281904801FB
    ~LongEventTypeRuleEx();

    //##ModelId=42819E7E006B
    LongEventTypeRuleEx();


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

    int m_iCarrierID;
    int m_iCallTypeID;
    int m_iPartyRuleID;
    int m_iPartyID;
    //##ModelId=42817C9501A9
    LongEventTypeRuleEx *m_poNextEvent;

    //##ModelId=42817CF4025A
    LongEventTypeRuleEx *m_poNextRule;

#ifndef PRIVATE_MEMORY
        unsigned int m_iNextEventOffset;
        unsigned int m_iNextRuleOffset;
#endif

};



//##ModelId=42817DFB02F8
//##Documentation
//## 长途类型判断管理类
class LongEventTypeRuleMgrEx
{
  public:
    //##ModelId=428180AA0011
    LongEventTypeRuleMgrEx();

  
    //##ModelId=428180DC0366
    ~LongEventTypeRuleMgrEx();

  
    //##ModelId=428180E701F9
    static void loadRule();

  
    //##ModelId=428180EF029B
    static void unloadRule();

    static bool reloadRule();
    
    //##ModelId=428180F601D3
    static bool getRule(EventSection const *_pEvent, 
    		LongEventTypeRuleEx** _ppRule);


  private:
    //##ModelId=42817EC803D9
    static LongEventTypeRuleEx *m_poRule;

    //##ModelId=4281AE010170
    static bool m_bInit;
    
};

#endif 

