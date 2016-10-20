#ifndef	_CRW_LOCK_H_
#define	_CRW_LOCK_H_
#include "CSemaphore.h"
class CRWLock
{
	public:
		CRWLock(const char * sem_name,bool bLock=false);
		CRWLock(int iSemKey,bool bLock=false);	
		~CRWLock();
		bool readLock(void);
		bool readUnLock(void);
		bool writeLock(void);
		bool writeUnLock(void);
	private:
		CRWLock(const CRWLock&);
		CRWLock& operator=(const CRWLock&);
		CSemaphore m_Lock;
		bool b;
		bool m_bRW;//false Read, true Write;
};
#define	READLOCK(LOCK)	CRWLock rLock(LOCK);

#define	WRITELOCK(LOCK)	CRWLock wLock(LOCK,true);

#endif

