/*VER: 1*/ 
#ifndef CRM_CONTROL_H
#define CRM_CONTROL_H

#include "HashList.h"

class CRMControl
{
  public:
	CRMControl();
	~CRMControl();

  public:
	bool lock(bool bBolck=true);
	bool unlock();
  

  private:
	bool m_bLocked;

#ifdef CUST_INTERFACE_SERVICE_MODE
    char m_sState[8];
#endif
	HashList<int> * m_poHistory;
	
	bool m_bPartition;
};

#endif
