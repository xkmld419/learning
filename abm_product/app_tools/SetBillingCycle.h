/*VER: 1*/ 
# ifndef SET_BILLING_CYCLE_H
# define SET_BILLING_CYCLE_H

#include "BillingCycleAdmin.h"
#include "Process.h"


class SetBillingCycle : public BillingCycleAdmin
{
  public:
	SetBillingCycle();

  public:
	void setBillingCycle(int iBillingCycleID, char sState[],int iOrgId);
    void PrintBillingCycle(int iBillingCycleTypeID, char sTime[]);
	int UpdateOcsCloseCycle(int OrgId,int BillingCycleId);
	void updateCycleStat(int iBillingCycleID,int iFlag);
};

class SetBillingCycleMgr : public Process
{
  public:
	int run();
};

# endif

