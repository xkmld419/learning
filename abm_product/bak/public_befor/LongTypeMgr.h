/*VER: 1*/ 
#include "EventSection.h"
#include "HashList.h"

class LongTypeMgr {

  public:
    static bool getLongType (EventSection *pEvt);   

    static void loadLongTypeRule ();

  private:
    static HashList<int> *m_poLongType;
};
