/*VER: 4*/ 
#ifndef ACCTFEEGETTER_H_
#define ACCTFEEGETTER_H_
#include "TransactionMgr.h"
#include "UserInfoReader.h"

struct AcctItemDataEx {
    long m_lAcctItemID;
    long m_lServID;
    long m_lAcctID;
    long m_lOfferInstID;
    int  m_iBillingCycleID;
    long m_lValue;
    int  m_iPayMeasure;
    int  m_iPayItemTypeID;
    char m_sStateDate[16];
    int  m_iItemSourceID;
	int  m_iItemClassID;
	int m_iOfferID;
    unsigned int m_iServNext;
};

class AcctFeeGetter :public UserInfoReader,public AggrMgr
{
  public:
  
	long getFee(long lAcctID, int iBillingCycleID);
	
	//此处的vector <long> 用于存放帐户下所有用户的帐目项的内存地址
	long getFee(long lAcctID, int iBillingCycleID,vector<long> &rvAcctItemAggrData);
	
	long getFee(long lAcctID, long lServID,int iBillingCycleID,vector<long> &rvAcctItemAggrData);

	long getServFee(long lServID, int iBillingCycleID,vector<long> &rvAcctItemAggrData);

	int getOfferID(long lProdOfferInstID);

	long getFeeEx(long lAcctID, int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx);

	long getFeeEx(long lAcctID, long lServID,int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx);

	long getServFeeEx(long lServID, int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx);

  public:
	
	AcctFeeGetter();

private:
	void dataConvert(vector<long> &vInAcctItemAggrData,vector<AcctItemDataEx> &vOutAcctItemDataEx);
};

#endif /*ACCTFEEGETTER_H_*/
