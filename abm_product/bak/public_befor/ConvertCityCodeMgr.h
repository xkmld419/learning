#ifndef __CONVERT_CITY_CODE_MGR__
#define __CONVERT_CITY_CODE_MGR__

#include "HashList.h"
#include "EventSection.h"
#include "CommonMacro.h"
 
class CityInfo
{
    public:
	char  m_sAreaCode[32];
	char  m_sProvCode[32];
	char  m_sRegionCode[32];
};


class ConvertCityCodeMgr {
public:
    bool convert(int iMode, char * i_sInCode, char * o_sOutCode );
    ConvertCityCodeMgr();
	~ConvertCityCodeMgr();

private:
    void load();
    void unLoad();

private:
    HashList<CityInfo *>  * m_pCityCodeIndex;
    HashList<CityInfo *>  * m_pAreaCodeIndex;
    struct CityInfo * m_pList;
};

#endif 
//__CONVERT_CITY_CODE_MGR__

