/*VER: 1*/ 
#include "Process.h"
#include "TransactionMgr.h"

class SeekValue : public Process
{
  public:
	int run();

  public:
	SeekValue();
};

DEFINE_MAIN (SeekValue);

int SeekValue::run()
{
	int iAcctItemType;
	long lAcctID;
	int iBillingCycle;
	vector<ServAcctInfo> vPayInfo;
	vector<ServAcctInfo>::iterator iterPay;
	vector<long> vServID;
	vector<long>::iterator iter;

	long lAggrCharge = 0;

	if (g_argc < 4) return -1;

	iAcctItemType = atoi (g_argv[3]);
	iBillingCycle = atoi (g_argv[2]);
	lAcctID = strtol (g_argv[1], 0, 10);

        int * piAcctItems = G_PACCTITEMMGR->getAcctItemsB (iAcctItemType);
        int * piTemp;
	long lTemp;


	if (!G_PUSERINFO->getPayInfo (vPayInfo,lAcctID)) return -1;

	for (iterPay=vPayInfo.begin(); iterPay!=vPayInfo.end(); iterPay++) {
		piTemp = piAcctItems;
		while (*piTemp != -1) {
			if (G_PACCTITEMMGR->getBelongGroup (*piTemp,
						(*iterPay).m_iAcctItemGroup)) {
				lTemp = G_PTRANSMGR->selectAcctItemAggr ((*iterPay).m_lServID,
							iBillingCycle, *piTemp);
				if (lTemp) {
					lAggrCharge += lTemp;
				cout << (*iterPay).m_lServID << "|" << iBillingCycle <<"|" << *piTemp <<"|" <<
					lTemp <<endl;
				}
			}
			piTemp++;
		}

	}

	cout << "VALUE:" << lAggrCharge << endl;

}

SeekValue::SeekValue()
{
	Environment::useUserInfo ();
	
	if (!G_PTRANSMGR) {
		G_PTRANSMGR = new TransactionMgr ();
		if (!G_PTRANSMGR) THROW (10);
	}
}

