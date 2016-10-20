/*VER: 1*/ 
#include "Process.h"
#include "Environment.h"
#include "UserInfoReader.h"

class SeekAcctInfo : public UserInfoReader
{
  public:
	SeekAcctInfo() : UserInfoReader() {}

	void seekAcct(long lAcctID);
};

class SeekAcct : public Process
{
  public:
	int run() {
		if (g_argc < 2) {
			return 0;
		}

		SeekAcctInfo seekAcct;
		seekAcct.seekAcct (strtol (g_argv[1], 0, 10));

		return 0;
	}
};

DEFINE_MAIN (SeekAcct);

#ifdef SHM_FILE_USERINFO
void SeekAcctInfo::seekAcct (long lAcctID)
{
	unsigned int k;

	if (!m_pSHMFileExternal->m_poAcctIndex->get (lAcctID, &k)) {
		cout << "没有找到对应的帐户信息" << endl;
		return;
	}

	AcctInfo * pAcct = *m_pSHMFileExternal->m_poAcctData;

	cout << endl << endl;
	cout << "ACCT_ID:      " << pAcct[k].m_lAcctID << endl;
	cout << "CUST_ID:      " << pAcct[k].m_lCustID << endl;
	cout << "CREDIT_GRADE: " << pAcct[k].m_iCreditGrade << endl;
	cout << "CREDIT_AMOUNT:" << pAcct[k].m_iCreditAmount << endl;
	cout << "ACCT_STATE:   " << pAcct[k].m_sState << endl;
	cout << endl << endl;
		
	return;
}

#else
void SeekAcctInfo::seekAcct (long lAcctID)
{
	unsigned int k;

	if (!m_poAcctIndex->get (lAcctID, &k)) {
		cout << "没有找到对应的帐户信息" << endl;
		return;
	}

	AcctInfo * pAcct = *m_poAcctData;

	cout << endl << endl;
	cout << "ACCT_ID:      " << pAcct[k].m_lAcctID << endl;
	cout << "CUST_ID:      " << pAcct[k].m_lCustID << endl;
	cout << "CREDIT_GRADE: " << pAcct[k].m_iCreditGrade << endl;
	cout << "CREDIT_AMOUNT:" << pAcct[k].m_iCreditAmount << endl;
	cout << "ACCT_STATE:   " << pAcct[k].m_sState << endl;
	cout << endl << endl;
		
	return;
}
#endif
