/*VER: 1*/ 
#ifndef STATE_CONVERT_H
#define STATE_CONVERT_H

#include "HashList.h"

#define MAX_STATE_NUM	32

#ifndef PRIVATE_MEMORY
class StateConvert
{
public:
    char m_sServState[4];
    int  m_iProductID;
    int  m_iOfferID;
    char m_sBillingState[4];

    unsigned int m_iNextProdOffset;
    unsigned int m_iNextOfferOffset;
};
#endif

class StateConvertMgr
{
  public:
	char * getBillingState(char *sState, int iProductID, int iOfferID);

  public:
	StateConvertMgr();
	void load();
	void unload();

  private:
	int getOffset(char *sBillingState);

  private:

	static char m_poBuf[MAX_STATE_NUM][4];
	static HashList<HashList<HashList<int> *> *> * m_poIndex;

  private:
	static bool m_bUploaded;
};

#endif
