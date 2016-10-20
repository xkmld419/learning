/*VER: 1*/ 
#ifndef __FILTER_BY_ACCT__
#define __FILTER_BY_ACCT__

//#include "ExtendFunctions.h"
#include <vector>
#include "EventSection.h"
#include "HashList.h"
#include "Environment.h"

//按合同号来过滤
class FilterByAcctMgr {

  public:
    static void filterByAcctID (EventSection *_poEventSection);
    static void load();
    FilterByAcctMgr();
  public:

    static HashList<int> m_hiAcctEventType;
    static bool bInit;
    static vector<long> m_vlAcctID;
};



#endif
