/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef ACCT_OWE_MGR_HDEAR
#define ACCT_OWE_MGR_HDEAR

#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMData_B.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_A.h"
#include "IpcKey.h"
#include "BillingCycleMgr.h"
#include "AcctItemMgr.h"
#include "OrgMgr.h"
#include "IpcKey.h"
#include "Date.h"
#include "UserInfoCtl.h"
#include"Process.h"


class AcctItemOweMgr:public UserInfoAdmin
{
  public:
	AcctItemOweMgr();

	~AcctItemOweMgr();

	  void  init( bool);

	  int getServOwe(long i_iServID);

	  int getAcctOwe(long i_iAcctID);

	  int getBillingCnt(long i_iAcctID);
	  	  
	  /*加载垃圾数据*/
	  void load();
	  
	  /*卸载欠费数据*/
	  void unload();
	  int updateOweMemory();
	  int getAcctOweV(long lAcctID,vector<AcctItemOweData> & vData);	  
///*	  
    template < template<class > class dataT,class Y ,template<class > class getT1,class Z1,template<class > class getT2,class Z2,template<class > class getT3,class Z3,template<class > class getT4,class Z4,template<class > class getT5,class Z5 >
    unsigned int tidyUserInfo(dataT<Y> * pData, const char * sParamCode,
    SHMIntHashIndex_A * pIdx1, getT1<Z1> * pGet1, int iGetOffset1, int iChkOffset1, 
    SHMIntHashIndex_A * pIdx2, getT2<Z2> * pGet2, int iGetOffset2, int iChkOffset2,
    SHMIntHashIndex_A * pIdx3, getT3<Z3> * pGet3, int iGetOffset3, int iChkOffset3,
    SHMIntHashIndex_A * pIdx4=0, getT4<Z4> * pGet4=(SHMData_A<int> *)0, int iGetOffset4=-1, int iChkOffset4=-1,
    SHMIntHashIndex_A * pIdx5=0, getT5<Z5> * pGet5=(SHMData_A<int> *)0, int iGetOffset5=-1, int iChkOffset5=-1);
//*/
    void revokeOwe();
	  
	  void printOweMemory();
  private:

#ifdef  USERINFO_REUSE_MODE
	 static SHMData_A<AcctItemOweData> * m_poOweData;
	 static SHMIntHashIndex_A * m_poOweAcctIndex;
	 static SHMIntHashIndex_A * m_poOweServIndex;
#else

	 static SHMData<AcctItemOweData> * m_poOweData;
	 static SHMIntHashIndex * m_poOweAcctIndex;
	 static SHMIntHashIndex * m_poOweServIndex;
#endif
	 static bool m_bUploaded;
  };
 #endif
