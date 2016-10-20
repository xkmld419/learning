#include "MutexLock.h"
CmutexLock::CmutexLock()
{
    pthread_mutex_init(&m_mutexLock, NULL);

}

CmutexLock::~CmutexLock()
{
    pthread_mutex_destroy( &m_mutexLock );
}

void CmutexLock::lock()
{
    pthread_mutex_lock( &m_mutexLock );

}

void CmutexLock::unlock()
{
    pthread_mutex_unlock( &m_mutexLock );
}
