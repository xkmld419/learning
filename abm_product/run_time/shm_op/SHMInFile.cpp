#include "ABMException.h"
#include "SHMParamInfo.h"
#include "FileV2.h"
#include "SHMInFile.h"

SHMInFile::SHMInFile()
{

};

SHMInFile::~SHMInFile()
{

}

int init(ABMException &oExp)
{
    if (SHM(openSHM(oExp)) != 0)
        return -1;
    return 0;
}

long hcodeInFile(const char *pDir, long lBatchID)
{
    if (!pDir)
        return -1;
    FileV2 oFile;    
    char sFileName[256];
    snprintf(sFileName, sizeof(sFileName), "%s/hss_hcode_info_%ld.data", pDir, lBatchID);
    if (oFile.openFile(sFileName) != 0)
        return -1;
        
    HCodeInfoStruct *pBegin = (HCodeInfoStruct *)(*SHM(m_poHCodeData));
    int iCnt = SHM(m_poHCodeData)->getCount() + 1;
    for (int i=1; i<=iCnt; ++i) {
         if (oFile.writeFile(*pBegin) == -1)
            return -1; 
        else
            pBegin ++;          
    }
    oFile.closeFile();
    return iCnt -1;
}

long minInFile(const char *pDir, long lBatchID)
{
    if (!pDir)
        return -1;
    FileV2 oFile;    
    char sFileName[256];
    snprintf(sFileName, sizeof(sFileName), "%s/hss_min_info_%ld.data", pDir, lBatchID);
    if (oFile.openFile(sFileName) != 0)
        return -1;
        
    MinInfoStruct *pBegin = (MinInfoStruct *)(*SHM(m_poMinData));
    int iCnt = SHM(m_poMinData)->getCount() + 1;
    for (int i=1; i<=iCnt; ++i) {
        if (oFile.writeFile(*pBegin) == -1)
            return -1; 
        else
            pBegin ++;           
    }
    oFile.closeFile();
    return iCnt -1;    
}
