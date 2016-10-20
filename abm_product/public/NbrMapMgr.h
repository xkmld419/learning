/*VER: 1*/ 
#ifndef NBR_MAP_MGR_H
#define NBR_MAP_MGR_H

//2008-8-7
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

class NbrMapData
{
  public:
    char m_sAccNbr[32];
    int  m_iMapType;
    char m_sNeedSameArea[16];
    char m_sRefValue[32];
    char m_sOutAreaCode[16];
    char m_sOutAccNbr[32];
    char m_sEffDate[16];
    char m_sExpDate[16];
    
    NbrMapData * pNext;
#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
#endif    
};



class NbrMapMgr
{
  public:
	NbrMapMgr();

  public:
	static bool mapNbr(EventSection *  pEventSection, int iType, char * sRef);
    static void init();
    static NbrMapData * getNbrData(int iType,char * sValue,char * sRef,EventSection * pEvent);
        
    static HashList<NbrMapData *> * m_pHNbrData[4];
    static NbrMapData * m_poNbrMapData;
    static bool bInited;

        
};

#endif
