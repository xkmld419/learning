#ifndef __LINKAGE_SPLIT_MGR__
#define __LINKAGE_SPLIT_MGR__

#include "HashList.h"

struct SplitCfg
{
public:
	char m_sStartVal[16];
	char m_sEndVal[16];
	int  m_iSplitType;
};

class SplitMgr {

public:
    static bool queryBySplitID (int iSplitID,SplitCfg * pCfg);
	static bool checkBySplitID (int iSplitID,char * pVal);
    SplitMgr();
	~SplitMgr();

private:
    static void load();

private:
    static HashList<SplitCfg *>  * m_poSplitIndex;
	static SplitCfg * m_poSplitData;
    static bool bInit;

};

#endif
