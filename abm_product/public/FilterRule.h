/*VER: 1*/ 
#ifndef FILTER_RULE_H
#define FILTER_RULE_H

#include "StdEvent.h"
#include "KeyTree.h"

#define MAX_LONG_TYPE 9

#define FILTER_SWITCHID		1
#define FILTER_TRUNCKIN		2
#define FILTER_TRUNCKOUT	3
#define FILTER_GATHERPOT	4
#define FILTER_AREACODE		5
#define FILTER_CALLING		6
#define FILTER_CALLED		7
class FilterRuleResult {
  public:
	FilterRuleResult();
	FilterRuleResult(int iEventTypeID, char * sEffDate, char * sExpDate);

	int m_iEventTypeID;
	char m_sEffDate[16];
	char m_sExpDate[16];

};

class FilterRuleCheckValue
{
	public:
			FilterRuleCheckValue();
			~FilterRuleCheckValue();
			FilterRuleResult* getFilterRuleResult(StdEvent * pEvent);
			unsigned int m_iFilterRuleResult;//得到的偏移
			int filter(StdEvent * pEvent);		
	public:			
			int  m_iSwitchLongType;
			int  m_iSwitchID;
			char m_sTrunkIn[128];
			char m_sTrunkOut[128];
			char m_sSwitchAreaCode[128];
			char m_sOrgCallingNBR[64];
			//char m_sOrgCalledNBR[64];
			
			int  m_iPot;
			FilterRuleResult m_oFilterRuleResult;
			bool m_bDefault; // 是否使用过默认值
			int  m_iDefault; // 1-SwitchID ;2-TrunkIn ;3- TrunkOut;4-GatherPot;5-AreaCode;6-Calling;7-Called
	private:
			bool checkSwitchIDIndex(StdEvent * pEvent);
			bool checkTrunkInIndex(StdEvent * pEvent);
			bool checkTrunkOutIndex(StdEvent * pEvent);
			bool checkGatherPotIndex(StdEvent * pEvent);
			bool checkAreaCodeIndex(StdEvent * pEvent);
			bool checkCallingIndex(StdEvent * pEvent);
			bool checkCalledIndex(StdEvent * pEvent);
};


class RuleCheckCalled {
  public:
	inline int check(StdEvent * pEvent);
	
	void insert(char * sOrgCalledNbr, FilterRuleResult * pResult);

	RuleCheckCalled();

  private:
	KeyTree<FilterRuleResult *> m_oCheck;
	FilterRuleResult * m_poDefaultResult;
};

class RuleCheckCalling {
  public:
	inline int check(StdEvent * pEvent);
	
	void insert(char * sOrgCallingNbr, char * sOrgCalledNbr, FilterRuleResult * pResult);
	RuleCheckCalling();

  private:
	KeyTree<RuleCheckCalled *> m_oCheck;
	RuleCheckCalled * m_poDefaultCheck;
};


class RuleCheckAreaCode {
  public:
	inline int check(StdEvent * pEvent);

	void insert(char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckAreaCode();

  private:
	KeyTree<RuleCheckCalling *> m_oCheck;
	RuleCheckCalling * m_poDefaultCheck;
};


class RuleCheckGatherPot{
  public:
	inline int check(StdEvent * pEvent);

	void insert(int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckGatherPot();

  private:
	KeyTree<RuleCheckAreaCode *> m_oCheck;
	RuleCheckAreaCode * m_poDefaultCheck;
};



class RuleCheckTrunkOut{
  public:
	inline int check(StdEvent * pEvent);

	void insert(char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckTrunkOut();

  private:
	KeyTree<RuleCheckGatherPot *> m_oCheck;
	RuleCheckGatherPot * m_poDefaultCheck;
};



class RuleCheckTrunkIn{
  public:
	inline int check(StdEvent * pEvent);

	void insert(char * sTrunkIn, char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckTrunkIn();

  private:
	KeyTree<RuleCheckTrunkOut *> m_oCheck;
	RuleCheckTrunkOut * m_poDefaultCheck;
};


class RuleCheckSwitchID{
  public:
	inline int check(StdEvent * pEvent);

	void insert(int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckSwitchID();

  private:
	KeyTree<RuleCheckTrunkIn *> m_oCheck;
	RuleCheckTrunkIn * m_poDefaultCheck;
};



class RuleCheckLongType {
  public:
	int check(StdEvent * pEvent);

	void insert(int iLongType, int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
			FilterRuleResult * pResult);

	RuleCheckLongType();
	~RuleCheckLongType();
  private:
	RuleCheckSwitchID  * m_poCheck[MAX_LONG_TYPE];
	RuleCheckSwitchID  * m_poDefaultCheck;
};


class FilterRule {
  public:
  	FilterRule();
	~FilterRule();
	void load();
	void unload();
	int  filter(StdEvent * pEvent);

  private:
	RuleCheckLongType m_oCheck;
};

#endif
