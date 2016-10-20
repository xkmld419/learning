/*VER: 1*/ 
#include "Process.h"
#include "BillingCycleMgr.h"

class EndBillingCycle : public Process
{
  public:
	int run();
};

DEFINE_MAIN (EndBillingCycle);

int EndBillingCycle::run()
{
	BillingCycleMgr bcm;

	if (g_argc < 2) return 0;

	int iBillingCycleID = atoi (g_argv[1]);
	bcm.endBillingCycle (iBillingCycleID, true);

	return 0;
}
