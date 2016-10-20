/*
* HSS内存参数, 参考计费 ParamInfo
* Version 1.0 at 20110620
*/

#ifndef __SHM_PARAM_INFO_H_INCLUDED_
#define __SHM_PARAM_INFO_H_INCLUDED_

#include "SHMData.h"
#include "SHMStringTreeIndex.h"
#include "SHMParamStruct.h"

class ABMException;

#define SHM(x) SHMParamInfo::x

class SHMParamInfo
{
public:

    SHMParamInfo();
    
    ~SHMParamInfo();
    
    static int openSHM(ABMException &oExp, bool bIfCrt=false);
    
    //根据号头 返回HSS_HCODE_INFO表内容
    static bool getHeadCode(char *pHead, HCodeInfoStruct &oValue, long lTime=0);
    
    //根据MIN 返回HSS_MIN_INFO表内容
    static bool getMinInfo(char *psMinBegin, MinInfoStruct &oValue, long lTime=0);
    
    //根据IMSI查找返回HSS_CTOG_IMSI_INFO表内容
    static bool getIMSI(char *psIMSIBegin, IMSIInfoStruct &oValue, long lTime=0);
    
private:
    
    static int openHCode(ABMException &oExp, bool bIfCrt);
    
    static int openMinInfo(ABMException &oExp, bool bIfCrt);
    
    static int openIMSIInfo(ABMException &oExp, bool bIfCrt);
    
public:    
    
    //HSS_HCODE_INFO
    static SHMData<HCodeInfoStruct> *m_poHCodeData;
    static SHMStringTreeIndex       *m_poHCodeIndex;
    
    //HSS_MIN_INFO
    static SHMData<MinInfoStruct> *m_poMinData;
    static SHMStringTreeIndex     *m_poMinIndex;
    
    //HSS_CTOG_IMSI_INFO
    static SHMData<IMSIInfoStruct> *m_poIMSIData;
    static SHMStringTreeIndex      *m_poIMSIIndex;    
};


#endif/*__SHM_PARAM_INFO_H_INCLUDED_*/
