/*VER: 1*/ 
#ifndef MEM_CONTROL_H
#define MEM_CONTROL_H

#include "SimpleSHM.h"

#define SHM_MEMControl	0x452
#define MBC_MEMControl  10000

class MEMControl
{
  public:
	MEMControl();
	~MEMControl();

  public:
	bool canRead();
	void lockToRead();
	void unlockRead();

	void lockToWrite();
	void unlockWrite();

  private:
	SimpleSHM * m_poLockSHM;
	int * m_pLock;
};

#endif
