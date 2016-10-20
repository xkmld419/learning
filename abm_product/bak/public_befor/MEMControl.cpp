/*VER: 1*/ 
#include "MEMControl.h"
#include <stdlib.h>
#include <unistd.h>

MEMControl::MEMControl()
{
	m_poLockSHM = new SimpleSHM (SHM_MEMControl, 1024);
	if (!m_poLockSHM) THROW (MBC_MEMControl+1);

	if (!m_poLockSHM->exist ()) {
		m_poLockSHM->open (true);
		int * p = (int *)((char *)(*m_poLockSHM));
		*p = 0;		
	}

	m_poLockSHM->open ();

	m_pLock = (int *)((char *)(*m_poLockSHM));
}

MEMControl::~MEMControl()
{
	if (m_poLockSHM) delete m_poLockSHM;
	m_poLockSHM = 0;
}

bool MEMControl::canRead()
{
	return !(*m_pLock);
}

void MEMControl::lockToRead()
{
	while (*m_pLock) {
		sleep (1);
	}
	return;
}

void MEMControl::lockToWrite()
{
	*m_pLock = 1;
	sleep (1);
}

void MEMControl::unlockWrite()
{
	*m_pLock = 0;
}

void MEMControl::unlockRead()
{
}

