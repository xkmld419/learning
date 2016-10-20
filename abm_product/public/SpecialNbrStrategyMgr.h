/*VER: 3*/ 
#ifndef __LINKAGE_SPECIAL_STRATEGY_MGR__
#define __LINKAGE_SPECIAL_STRATEGY_MGR__

#include "StdEvent.h"
#include "CommonMacro.h"
#include "KeyTree.h"
#include "HashList.h"

class SpecialNbrStrategy
{
public:
	char	m_sAreaCode[MAX_CALLING_AREA_CODE_LEN];
	char	m_sAccNbr[MAX_CALLING_NBR_LEN];
	char	m_sEffDate[MAX_START_DATE_LEN];
	char	m_sExpDate[MAX_START_DATE_LEN];
	int	    m_iOfferID;
	int		m_iStrategyID;

	SpecialNbrStrategy * m_poNext;
	unsigned int m_iNext;
		
};

class SpecialNbrStrategyMgr {
public:
    bool getStrategyID(StdEvent * pEvent,int iOfferID,int &iStrategyID);
	bool checkOfferStrategy(int iOfferID);

    SpecialNbrStrategyMgr();
	~SpecialNbrStrategyMgr();

private:
    void load();
	void unLoad();

private:
	 KeyTree<SpecialNbrStrategy *>* m_pNbrStrategy;
	 HashList<int>  * m_poOfferIndex;
	 int m_iLen;
};

#endif
