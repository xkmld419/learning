/**********************************************************************
朱逢胜
电信计费研发部
2010.07.16
互斥量锁
**********************************************************************/
#ifndef CMUTEXTLOCK_H_
#define CMUTEXTLOCK_H_
#include <pthread.h>
#include <iostream>
using namespace std;
class CmutexLock
{
public:
    CmutexLock();
    ~CmutexLock();
    void lock();
    void unlock();
protected:
private:
    pthread_mutex_t m_mutexLock;
};
#endif

