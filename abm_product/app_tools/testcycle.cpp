/*VER: 1*/ 
#include <iostream>
using namespace std;

#include "BillingCycle.h"
#include "BillingCycleMgr.h"
#include "Environment.h"
#include "Exception.h"

int main (int argc, char **argv)
{
	int i;

	try {
		BillingCycleMgr bcm;
		BillingCycle * p = bcm.getBillingCycle (1, "20050723010101");
		if (p) {
			cout << "当前帐期: " << p->getBillingCycleID () << endl;
		}

		if (argc == 1) return 0;

		p = bcm.getBillingCycle (atoi (argv[1]));
		if (!p) {
			cout << "帐期 " << atoi (argv[1]) << " 信息未找到" << endl;
			return 0;
		}

		cout << "帐期 " << atoi (argv[1]) << " 信息如下:" << endl;
		cout << "BillingCycleID:" << p->m_iBillingCycleID << endl;
		cout << "BillingCycleType:" << p->m_iBillingCycleType << endl;
		cout << "LastID:" << p->m_iLastID << endl;
		cout << "State:" << p->m_sState << endl;

		if (argc == 2) return 0;

		if (strcmp (argv[2], "10A") && strcmp (argv[2], "10B") && strcmp (argv[2], "10R")
			&& strcmp (argv[2], "10E") && strcmp (argv[2], "10C")) {

			cout << "状态只能为: 10A, 10B, 10C, 10E: " << argv[2] << endl;

			return 0;
		} 

		strcpy (p->m_sState, argv[2]);

		cout << "New State:" << p->m_sState << endl;

	} catch (TOCIException & e) {
		cout << e.getErrMsg () << endl;
		cout << e.getErrSrc () << endl;
	} catch (Exception & e) {
		cout << e.descript () << endl;
	} catch (exception & e) {
		cout << e.what () << endl;
	}

	return 0;
}
