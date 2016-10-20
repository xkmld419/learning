/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMACCESS_H_HEADER_INCLUDED_BDBB0897
#define SHMACCESS_H_HEADER_INCLUDED_BDBB0897

#include "Exception.h"
#include "SimpleSHM.h"


//##ModelId=4244276903D9
class SHMAccess
{
public:
    //##ModelId=42442817022A
    SHMAccess(ABMException &oExp,char *shmname);

    //##ModelId=424428170248
    SHMAccess(ABMException &oExp,long shmkey);

    //##ModelId=424428170266
    ~SHMAccess();

    //##ModelId=424428BF005F
    //##Documentation
    //## 数据区是否存在
    bool exist();

    int getError() {
        return m_iError;
    }

    //##ModelId=424428CB0099
    //##Documentation
    //## 删除数据区
    void remove(ABMException &oExp);
    //modify by jinx for app_guard
public:
    //判断共享内存的链接个数
    int connnum();

    //##ModelId=4244CAB6013E
    void create(ABMException &oExp,unsigned long size,
        unsigned int iPermission = 0777);


protected:
    //##ModelId=4244C18402C0
    unsigned int *m_piSize;

    //##ModelId=424427B30367
    SimpleSHM *m_poSHM;

    int m_iError;

private:
    //##ModelId=4244C70600D3
    void open(ABMException &oExp);

    //##ModelId=4244285E0287
    char *m_sSHMName;
    //##ModelId=4244C558007A
    long m_lSHMKey;
};



#endif /* SHMACCESS_H_HEADER_INCLUDED_BDBB0897 */
