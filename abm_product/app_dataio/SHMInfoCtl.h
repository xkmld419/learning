#ifndef __SHM_INFO_CTL_H_INCLUDED_
#define __SHM_INFO_CTL_H_INCLUDED_

#include "ABMException.h"
#include "SHMParamInfo.h"
#include "TimesTenAccess.h"
#include "TTTns.h"

struct MinNbrStruct
{
    char m_sMinNbr[16];    
};


class SHMInfoCtl
{
public:
    static unsigned int getCount(int iTableType)
    {
    		if(iTableType==TABLE_MIN_INFO)
        	return SHM(m_poMinData)->getCount();
    		if(iTableType==TABLE_IMSI_INFO)
        	return SHM(m_poIMSIData)->getCount();
        else //if(iTableType==TABLE_HCODE_INFO || iTableType==TABLE_LOCAL_INFO)
        	return SHM(m_poHCodeData)->getCount();
    };

    static bool getHeadData(HCodeInfoStruct **pData, int iPos)
    {
        *pData = (*(SHM(m_poHCodeData))) + iPos;
				
        return true;
    };

    static bool getMinData(MinInfoStruct **pData, int iPos)
    {
        *pData = (*(SHM(m_poMinData))) + iPos;
				
        return true;
    };

    static bool getImsiData(IMSIInfoStruct **pData, int iPos)
    {
        *pData = (*(SHM(m_poIMSIData))) + iPos;
				
        return true;
    };
    
public:
		static int create();
		static int attach();
		
		static void getAllMin(const char *sBegin, const char *sEnd, std::vector<MinNbrStruct> &vec);
		static int loadHeadCodeIn(TimesTenCMD *pTmp,bool bShow);
		static int loadMinInfoIn(TimesTenCMD *pTmp,bool bShow);
		static int loadLocalHeadIn(TimesTenCMD *pTmp,bool bShow);
		static int loadImsiInfoIn(TimesTenCMD *pTmp,bool bShow);
		
		static int outHeadToTable(TimesTenCMD *pTmp, unsigned int count, HCodeInfoStruct *pStart);
		static int outMinToTable(TimesTenCMD *pTmp, unsigned int count, MinInfoStruct *pStart);
		static int outImsiToTable(TimesTenCMD *pTmp, unsigned int count, IMSIInfoStruct *pStart);
		
		static bool updateHCodeInfo(HCodeInfoStruct *pData,bool &bNew,bool bReplace);
		static bool updateMinInfo(MinInfoStruct *pData,bool &bNew,bool bReplace);
		static bool updateImsiInfo(IMSIInfoStruct *pData,bool &bNew,bool bReplace);
		
		static void showHCodeInfo(HCodeInfoStruct &oData);
		static void showMinInfo(MinInfoStruct &oData);
		static void showImsiInfo(IMSIInfoStruct &oData);
};


#endif/*__SHM_INFO_CTL_H_INCLUDED_*/
