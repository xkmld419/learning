#ifndef __HSS_DCC_FILE_H_INCLUDED_
#define __HSS_DCC_FILE_H_INLCUDED_

#include <vector>

class HSSObject;
class MmapFile;
class ABMException;

class HSSDccFile
{

public:

    HSSDccFile();
    
    ~HSSDccFile();
    
    int init(ABMException &oExp);

    int openFile(ABMException &oExp, const char *sFile);
    
    int getHSS(std::vector<HSSObject *> &vObj);
    
    int closeFile();
    
private:

    MmapFile *m_poMap;    
};

#endif/*__HSS_DCC_FILE_H_INLCUDED_*/

