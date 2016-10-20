/*VER: 1*/ 
#ifndef FILTER_SIMPLE_MGR
#define FILTER_SIMPLE_MGR

#include <vector>
#include <algorithm>

#include "Environment.h"
#include "UserInfoReader.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "WfPublic.h"
#include "TransactionMgr.h"
#include "AcctItemAccu.h"
#include "AcctItemOweMgr.h"
#include "BalanceMgr.h"
#include "ParamInfoInterface.h"
#include "rt_make_stop.h"

#ifdef ABM_BALANCE
#include "abmclnt.h"
#include "abmaccess.h"
#endif

class FilterSimpleMgr
{
	public:
		FilterSimpleMgr();
		~FilterSimpleMgr();
		
		vector<struct BalanceData>  m_vBalance;
		vector<struct AcctItemOweData> m_vOweChargeTmp,m_vOweCharge;

		AcctItemOweMgr * m_pOweMgr;
		BalanceMgr * m_pBalanceMgr;

		long getFilterBalance(long lAcctID,long lServID,int iBillingCycleID);
		void loadOweCharge(long lAcctID);
		void loadAcctBalance(long lAcctID);
		void loadAggrCharge(long lAcctID,long lServID,int iBillingCycleID);
		long calcFilterBalance(long lAcctID,long lServID,int iBillingCycleID);
		long mergeOweCharge();
		static bool sortServCharge(const AcctItemOweData & m1,const AcctItemOweData & m2);
		static bool sortServBalance(const BalanceData & m1,const BalanceData & m2);

#ifdef ABM_BALANCE
    ABM *m_pABM;
		long getFilterBalanceByABM(long lAcctID,long lServID,int iBillingCycleID);
		void loadAcctBalanceByABM(long lAcctID);
		void loadAggrChargeByABM(long lAcctID,long lServID,int iBillingCycleID);
#endif
	private:

};

#endif 


