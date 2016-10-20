/*VER: 4*/ 
#ifndef STOP_ACCT_BALANCE_MGR
#define STOP_ACCT_BALANCE_MGR

#include <vector>
#include "Environment.h"
#include "UserInfoReader.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "WfPublic.h"

struct strStopAcctBalance
{
  long lAcctID;
  long lServID;
  long lAcctItemGroupID;
  long lBalance;
  long lUpper;
  char sEffDate[20];
  char sExpDate[20];
  bool isCreidt;
};

struct strOweCharge
{
  long lAcctID;
  long lServID;
  int  iBillingCycleID;
  long lCharge;
};

class StopBalanceMgr
{
  public:
 		StopBalanceMgr();
 		~StopBalanceMgr();
 		long getAcctBalance(long lAcctID,long lServID,int iLevel,int iCredit = 1);
 		vector<struct strStopAcctBalance>  m_strBalance;
 		void loadAcctBalance(long lAcctID,int iBillingCnt);

 		long getHistoryOweCharge(long lAcctID,long lServID,char sState[],int iBillingCycleID);
 		vector<struct strOweCharge>  m_strOweCharge;
 		void loadOweCharge(long lAcctID, int iPayRoleIDFlag = 0);

 		long getHistoryOweChargeZz(long lAcctID,long lServID,char sState[],int iBillingCycleID);
 		vector<struct strOweCharge>  m_strOweChargeZz;
 		void loadOweChargeZz(long lAcctID, int iPayRoleIDFlag = 0);
  private:
    
};

#endif /* STOP_BALANCE_MGR */

