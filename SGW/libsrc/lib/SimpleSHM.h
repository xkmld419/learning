/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SIMPLESHM_H_HEADER_INCLUDED_BDE237A3
#define SIMPLESHM_H_HEADER_INCLUDED_BDE237A3
#include <string>
#include "Exception.h"
using namespace std;
#ifdef DEF_AIX123
#define SHM_EXPAND_FREE_POS
#else
#undef SHM_EXPAND_FREE_POS
#define SHM_EXPAND_GLOBAL_POS
#endif


//##ModelId=41DB83D00175
class SimpleSHM
{
  public:
    //##ModelId=41DCE78C0207
    void close();

    //##ModelId=41DCE78F00CB
    void remove();

    //##ModelId=41DCE7CF031B
    bool exist();
	long getIPCKey();
    //##ModelId=421DBD860141
    SimpleSHM(long key, unsigned long size, char * pNeed =0 );

    //##ModelId=421DBD8D0259
    SimpleSHM(char *name, unsigned long size, bool bShm = true, char * pNeed =0 );

    //##ModelId=421DBD93037A
    void open(bool ifcreate = true);

    //##ModelId=421DBDEC0043
    operator char *();

    //##ModelId=4223C4890349
    unsigned long size(); 
    unsigned long getLastSize();
    void setLastSize(unsigned long ulsize ) ;
  	 time_t  getShmTime();
        
    //##ModelId=4224288D0239
    ~SimpleSHM();
    //##ModelId=42807A2C0370
    int nattch();
#ifdef	_GTEST_DEBUG_
	long getSysID();
#endif
    
    unsigned long getlba();
    unsigned long expand( unsigned long xSize );

  private:
    //##ModelId=41DCE78501E9
    bool _create();

    //##ModelId=41DCE7890299
    bool _open();

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
    char * m_pNeedPos ;
    
    bool m_bSHMFile;
    string m_sFileName;
};

#define SHM_PERMISSION	0666

#endif /* SIMPLESHM_H_HEADER_INCLUDED_BDE237A3 */
