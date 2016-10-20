#include "HSSDccFile.h"
#include "MmapFile.h"
#include "hssobject.h"
#include "ABMException.h"

HSSDccFile::HSSDccFile()
{
    m_poMap = NULL;
}

HSSDccFile::~HSSDccFile()
{
    if (m_poMap != NULL)
        delete m_poMap;
}

int HSSDccFile::init(ABMException &oExp)
{
    if ((m_poMap=new MmapFile) == NULL) {
        ADD_EXCEPT0(oExp, "HSSDccFile::init malloc failed!");
        return -1;
    }
    return 0;
}

int HSSDccFile::openFile(ABMException &oExp, const char *sFile)
{
    return m_poMap->openMap(oExp, sFile);
}

int HSSDccFile::getHSS(std::vector<HSSObject *> &vObj)
{
    HSSObject *pData = (HSSObject *)m_poMap->getMap();
    int iSize = m_poMap->getSize();
    char *p;
    while (iSize > 0) {
        vObj.push_back(pData);
        p = (char *)pData;
        iSize -= pData->m_iSize;
        pData = (HSSObject *)(p + pData->m_iSize);
        //iSize -= pData->m_iSize;
    }
    return 0;
}


int HSSDccFile::closeFile()
{
    return m_poMap->closeMap();   
}
    
