/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMACCESS_H_HEADER_INCLUDED_BDBB0897
#define SHMACCESS_H_HEADER_INCLUDED_BDBB0897

#include "Exception.h"
#include "SimpleSHM.h"

const int MAX_SHM_EX_CNT = 8;

//##ModelId=4244276903D9
class SHMAccess
{
  public:
    //##ModelId=42442817022A
    SHMAccess(char *shmname);

    //##ModelId=424428170248
    SHMAccess(long shmkey);
    SHMAccess(long shmkey , long iNextSize, int iMaxExCnt );
    
    //##ModelId=424428170266
    ~SHMAccess();

    //##ModelId=424428BF005F
    //##Documentation
    //## 数据区是否存在
    bool exist();

    //##ModelId=424428CB0099
    //##Documentation
    //## 删除数据区
    void remove();
	//modify by jinx for app_guard
  public:
  	int connnum();
  	void initExpand(unsigned int * pExCnt, int iUnitSize, unsigned int * pSeg );
  	unsigned long expandit();
  	unsigned long totalSize();
    void setExpandNull();
  	void showExpandInfo();
	char *getMemAddr();
  	
  protected:
    //##ModelId=4244CAB6013E
    void create(unsigned long size);
  	bool reopen();


  protected:

    //##ModelId=4244C18402C0
    unsigned int *m_piSize;

    //##ModelId=424427B30367
    SimpleSHM *m_poSHM;
    SimpleSHM *m_poExSHM[MAX_SHM_EX_CNT];
    int m_iExShmCnt;
    int m_iMaxExCnt;
    long m_lNextSize;
    unsigned int *m_piExpandCnt;
    int m_iUnitSize;
    


    //##ModelId=4244285E0287
    char *m_sSHMName;
    char *m_sExSHMName[MAX_SHM_EX_CNT];
    //##ModelId=4244C558007A
    long m_lSHMKey;
    long m_lExSHMKey[MAX_SHM_EX_CNT];

#ifdef SHM_EXPAND_FREE_POS
    static char * g_sGlobalPos;
    static long g_lBlock;
#endif

public:

#ifdef SHM_EXPAND_GLOBAL_POS
    char *       m_sSegment[MAX_SHM_EX_CNT+1];
///    unsigned int m_iSegment[MAX_SHM_EX_CNT+1];
    unsigned int * m_piSeg;
#endif

  private:
    //##ModelId=4244C70600D3
    void open();
    bool openEx(int i);

#ifdef SHM_EXPAND_FREE_POS
  	void refreshPos();
#endif

};



#endif /* SHMACCESS_H_HEADER_INCLUDED_BDBB0897 */
