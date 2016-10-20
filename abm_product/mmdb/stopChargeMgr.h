/*VER: 1*/ 
#ifndef STOP_ACCT_CHARGE_MGR
#define STOP_ACCT_CHARGE_MGR

#include <vector>
#include <algorithm>

#include "Environment.h"
#include "UserInfoReader.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "WfPublic.h"
#include "TransactionMgr.h"
#include "AcctItemAccu.h"

struct strStopBalanceNew
{
	long lAcctBalanceID;
  long lAcctID;
  long lServID;
  long lAcctItemGroupID;
  long lBalance;
  int  iBalanceType;
  long lUpper;
  int  iObjectType;///表示该帐本的抵扣顺序
  char sEffDate[20];
  char sExpDate[20];
  char sAdjustFlag[5];
};

struct strOweChargeNew
{
  long lAcctID;
  long lServID;
  int  iBillingCycleID;
  int  iAcctItemTypeID;//add
  long lCharge;
  long lBilledCharge;
};


class StopChargeMgr
{
  public:
 		StopChargeMgr(char sTableName[]);
 		~StopChargeMgr();
 		long getAcctBalance(long lAcctID,long lServID,int iLevel);
 		vector<struct strStopBalanceNew>  m_strBalanceNew;
 		void loadAcctBalance(long lAcctID);

 		long getHistoryOweCharge(long lAcctID,long lServID,char sState[],int iBillingCycleID);
 		vector<struct strOweChargeNew>  m_strOweCharge;
 		vector<struct strStopBalanceNew> m_strBalance;
 		void loadOweCharge(long lAcctID);
    void loadRtCharge(long lAcctID);
    void loadAggrCharge(long lServID,long lAcctID,int iBillingCycleID);
    long getBillBalance(long lAcctID,long lServID,int iBillingCycleID,long * plServItemBalance,long * plAcctItemBalance);
    long BalanceCompare(long lAcctID,long lServID,int iBillingCycleID,long * plServItemBalance,long * plAcctItemBalance);
    void counteractNegativeItemID(long &lSumCharge);
    //bool getBalanceItemMember(long lAcctItemGroupID,int  iAcctItemTypeID);
    void getCyclePayoutBalance(int iBillingCycleID,long lAcctBalanceID,long & lPayedBalance);
    void GetDBPartyRoleCharge(long lAcctID,long lServID,int iBillingCycleID,int iPartyRoleID);
    void printOweCharge();
    void printBalance();
    static long stopCharge;
  private:
    bool getBalanceItemMember(long lAcctItemGroupID,int  iAcctItemTypeID);
    char gsPreTableName[256];
    static bool sortServAcct(const ServAcctInfo & m1,const ServAcctInfo & m2);
};

#endif /* STOP_CHARGE_MGR */

