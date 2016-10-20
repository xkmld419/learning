#include "PthreadMutex.h"

PthreadMutexT::PthreadMutexT()
{
	m_bAttrAttach = false;
	m_bMutexAttach = false;	
}

PthreadMutexT::~PthreadMutexT()
{
	if (m_bMutexAttach) {
		if (pthread_mutex_destroy(&m_mutex) != 0) {
			throw;	
		}	
	}
	if (m_bAttrAttach) {
		if (pthread_mutexattr_destroy(&m_attr_t) != 0) {
			throw;	
		}	
	}
}

int PthreadMutexT::init()
{
	if (pthread_mutexattr_init(&m_attr_t) != 0) {
  	    return -1;	
  	}  	
  	m_bAttrAttach = true;  	
  	if (pthread_mutexattr_settype(&m_attr_t, PTHREAD_MUTEX_ERRORCHECK) != 0) {
  	    return -1;   	
  	}
  	if (pthread_mutexattr_setprotocol(&m_attr_t, PTHREAD_PRIO_INHERIT) != 0) {
  	    //return -1; 
  	}
  	
  	if (pthread_mutex_init(&m_mutex, &m_attr_t) != 0) {
       return -1;
	}	
	m_bMutexAttach = true;
	
	return 0;	
}

int PthreadMutexT::lock()
{
	return pthread_mutex_lock(&m_mutex);	
}

int PthreadMutexT::trylock()
{
	return pthread_mutex_trylock(&m_mutex);	
} 

int PthreadMutexT::unlock()
{
	return pthread_mutex_unlock(&m_mutex);	
} 