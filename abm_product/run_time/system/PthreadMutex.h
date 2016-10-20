/*
*	version js at 2010-06-03
*
*/
#ifndef PTHREADMUTEX_H_HEADER_INCLUDED
#define PTHREADMUTEX_H_HEADER_INCLUDED

#include <pthread.h>

class PthreadMutexT
{
	
public:
	
	PthreadMutexT();
	
	~PthreadMutexT();
	
	int init();
	
	int lock();
	
	int trylock();
	
	int unlock();
	
private:
	
	bool m_bAttrAttach;
	
	bool m_bMutexAttach;
	
	pthread_mutexattr_t m_attr_t;
	
	pthread_mutex_t m_mutex;

};

#endif /*PTHREADMUTEX_H_HEADER_INCLUDED*/