#include "CRWLock.h"
#include <assert.h>
CRWLock::CRWLock(int iSemKey,bool bLock)
{
	bool bRet = m_Lock.getSemEx(iSemKey);
	assert(bRet);
	if(bLock)
		writeLock();	
	else
		readLock();
}
CRWLock::CRWLock(const char* sem_name,bool bLock)
{
	bool bRet = m_Lock.getSemEx(sem_name);
	assert(bRet);
	if(bLock)
		writeLock();	
	else
		readLock();
}

CRWLock::~CRWLock()
{
	if(m_bRW)
		writeUnLock();
	else
		readUnLock();
}

bool CRWLock::readLock(void)
{
	m_bRW=false;
	if(!m_Lock.P())
		return false;
	return true;
}

bool CRWLock::readUnLock(void)
{
	return m_Lock.V();
}

bool CRWLock::writeLock(void)
{
	m_bRW=true;
	if(!m_Lock.P_W())
		return false;
	return true;
}

bool CRWLock::writeUnLock(void)
{
	return m_Lock.V_W();
}
