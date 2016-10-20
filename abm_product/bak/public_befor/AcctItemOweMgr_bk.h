/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef ACCTITEMOWEMGR_H_HEADER_INCLUDED
#define ACCTITEMOWEMGR_H_HEADER_INCLUDED

#include "SHMData.h"
#include "SHMIntHashIndex.h"

//#define ACCTITEMOWEBUF_DATA_COUNT	4000000			//缓冲的数据段长度
//20070827 此处注释掉，各省份在LocalDefine中去定义。
//因为0506欠费数据无法上载，所以3000000改为4000000

struct AcctItemOweData{					//记录结构
	long iAcctID;					//帐户标识
	long iServID;					//用户标识
	int iBillingCnt;			//帐期月数
	//int iAcctItemTypeID;				//帐目类型标识
	long lAmount;					//欠费
	unsigned int iNextAcctOffset;
	unsigned int iNextServOffset;
};

class AcctItemOweMgr
{
  public:
	AcctItemOweMgr();

	~AcctItemOweMgr();

	int getServOwe(long i_iServID);
	int getAcctOwe(long i_iAcctID);
	int getBillingCnt(long i_iAcctID);
	void load();
	void unload();
  private:
	static SHMData<AcctItemOweData> * m_poOweData;
	static SHMIntHashIndex * m_poOweAcctIndex;
	static SHMIntHashIndex * m_poOweServIndex;

  private:
	static bool m_bUploaded;

	//void getParameter();
};

#endif /* ACCTITEMOWEMGR_H_HEADER_INCLUDED */
