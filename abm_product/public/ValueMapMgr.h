/*VER: 1*/ 
#ifndef VALUE_MAP_MGR_H
#define VALUE_MAP_MGR_H

//2008-12-09
//apin
#include "EventSection.h"
#include "Environment.h"
#include "HashList.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

using namespace std;

class ValueMapData
{
  public:
 
    char m_sOrgValue[32];
    char m_sDestValue[32];
    int  m_iMapType;
    char m_sEffDate[16];
    char m_sExpDate[16];
    ValueMapData * pNext;

#ifndef PRIVATE_MEMORY
    int m_iAppID;
    int m_iFlowID;
    int m_iProcID;
    unsigned int m_iNextOffset;
#endif
};



class ValueMapMgr
{
  public:
	ValueMapMgr();

  public:
    static bool mapValue(EventSection *  pEvent,  int iType, 
        char * sOrg, char * sRet, int iMaxLen = 0);
    static void init();
        
    static HashList<ValueMapData *> * m_pHValueData;
    static ValueMapData * m_poValueMapData;
    static bool bInited;

        
};

#endif
