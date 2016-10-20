/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef BALANCEMGR_H_HEADER_INCLUDED
#define BALANCEMGR_H_HEADER_INCLUDED

#include <vector>
#include "SHMData.h"
#include "SHMIntHashIndex.h"

using namespace std;

//#define BALANCEBUF_DATA_COUNT	3000000			//缓冲的数据段长度
#define BALANCEBUF_DATA_COUNT	(ParamDefineMgr::getLongParam("MEMORY_DB", "BALANCE_DATA_COUNT"))

struct BalanceData{					//记录结构
	long lAcctID;					//帐户标识
	long lServID;					//用户标识
	int iAcctItemTypeID;				//帐目类型标识
	int iBalance;					//欠费
	unsigned int iNextAcctOffset;
	unsigned int iNextServOffset;
};

class BalanceMgr
{
  public:
	BalanceMgr();

	~BalanceMgr();

	int getServBalance(long lServID);
	int getAcctBalance(long lAcctID);
	long GetAcctUServBalance(long lServID,long lAcctID);
	void load();
	void unload();
  int UpdateBalanceMemory();
  int getAcctBalanceV(long lAcctID,vector<struct BalanceData> & vData);
  void printBalanceMemory();
  private:
	static SHMData<BalanceData> * m_poBalanceData;
	static SHMIntHashIndex * m_poBalanceAcctIndex;
	static SHMIntHashIndex * m_poBalanceServIndex;

  private:
	static bool m_bUploaded;

};

#endif /* BALANCEMGR_H_HEADER_INCLUDED */
