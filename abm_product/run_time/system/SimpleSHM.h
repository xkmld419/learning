/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef ABM_SIMPLESHM_H_HEADER_INCLUDED_BDE237A3
#define ABM_SIMPLESHM_H_HEADER_INCLUDED_BDE237A3

#include "ABMException.h"

    
class SimpleSHM
{
  public:
    //##ModelId=41DCE78C0207
    void close();

    //##ModelId=41DCE78F00CB
    int remove(ABMException &oExp);

    //##ModelId=41DCE7CF031B
    bool exist(ABMException &oExp);

    //##ModelId=421DBD860141
    SimpleSHM(long key, unsigned long size);

    //##ModelId=421DBD93037A
    int open(ABMException &oExp, bool ifcreate, unsigned int iPermission);

    //##ModelId=4223C4890349
    unsigned long size(); 

    //##ModelId=4224288D0239
    ~SimpleSHM();
    //##ModelId=42807A2C0370
    int nattch();

    char * getPointer() { return m_pPointer; }

  private:
    //##ModelId=41DCE78501E9
    bool _create(ABMException &oExp, unsigned int iPermission);

    //##ModelId=41DCE7890299
    bool _open(ABMException &oExp, unsigned int iPermission);

    //##ModelId=41DCE7150256
    int m_iSysID;

    //##ModelId=421DBEBB03B2
    long m_lKey;

    //##ModelId=421DBECF0017
    bool m_bAttached;

    //##ModelId=421DBEDB03CC
    unsigned long m_ulSize;

    //##ModelId=421DCDC102AE
    char *m_pPointer;


};



#endif /* SIMPLESHM_H_HEADER_INCLUDED_BDE237A3 */
