/*VER: 1*/
#ifndef SP_INFO_SEEK_H
#define SP_INFO_SEEK_H

//2009-2-16
//apin
#include "EventSection.h"
#include "Environment.h"
#include "HashList.h"
#include "Date.h"
#include "IDParser.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

using namespace std;

class SpInfoData
{
  public:
    char m_sPrdOfferID[24];
    char m_sPrdID[24];
    char m_sPolicyID[24];
    int  m_iFeeType;
//    char m_sEffDate[16];
//    char m_sExpDate[16];
//    SpInfoData * pNext; 

};



class SpInfoSeekMgr
{
  public:
	SpInfoSeekMgr();
	~SpInfoSeekMgr();

  public:
    static int getSPInfo(EventSection * pEvent, 
        Value * i_pInNBR, ID * o_pServCode, ID * o_pFeeType);
        
    static void init();
    static void load();
    static void unload();
        
    static HashList<SpInfoData *> * m_pHSpInfoData;
    static SpInfoData * m_poSpInfoData;
    static bool bInited;
    static Date m_oDate;
    static char m_sLastDate[16];
        
};

#endif
