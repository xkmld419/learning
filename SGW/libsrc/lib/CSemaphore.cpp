/*VER: 3*/ 
#include "CSemaphore.h"
#include <string.h>
#include <string>
#include <stdlib.h>
//#include "Log.h"
//#include "MBC.h"
#ifdef __OS_WIN32__
#include <windows.h>
#endif

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>


#ifdef  DEF_LINUX 
    #define SEM_A 0200 /* alter permission*/
    #define SEM_R 0400 /* read permission*/
#endif

#define	MAX_READ		64

union semun
{  
	int 	val;  
	struct 	semid_ds *buf;  
	ushort *array;  
};
#endif

bool CSemaphore::P_W(bool bWait)
{
#ifdef __OS_WIN32__
    if( ::WaitForSingleObject(_Handle, INFINITE) == WAIT_FAILED ){
        return false;
    }
    else
        return true;
#endif 
#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    short semflags = SEM_UNDO;
    if(!bWait)
    {
        semflags = IPC_NOWAIT | semflags;
    }
    struct sembuf Sops = { 0,-MAX_READ,semflags };
	if(semop((int &) _Handle, &Sops, 1) >= 0)
	{
	    return true;
	}
	else
	{
	    if(errno == EAGAIN)
	    {

	    }
	    else if(errno == EIDRM)
	    {
			//ALARMLOG28(0,MBC_CLASS_Fatal, 6,"%s%s","此信号量被删除:",m_sSemName);
	    }
	    else if (errno == EINTR)
	    {
			//ALARMLOG28(0,MBC_CLASS_Fatal, 6,"%s%s","信号互斥量值为负数:",m_sSemName);
	    }
	    return false;
    }
#endif
	return true;
}

bool CSemaphore::P(bool bWait)
{
#ifdef __OS_WIN32__
    if( ::WaitForSingleObject(_Handle, INFINITE) == WAIT_FAILED ){
		//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s"," 获取信号量失败",m_sSemName);
        return false;
    }
    else
        return true;
#endif 
#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    short semflags = SEM_UNDO;
    if(!bWait)
    {
        semflags = IPC_NOWAIT | semflags;
    }
    struct sembuf Sops = { 0,-1,semflags };
	if(semop((int &) _Handle, &Sops, 1) >= 0)
	{
	    return true;
	}
	else
	{
	    if(errno == EAGAIN)
	    {
			//ALARMLOG28(0,MBC_CLASS_Fatal, 6,"%s%s","信号处理失败[NOWAIT]:",m_sSemName);
	    }
	    else if(errno == EIDRM)
	    {
			//ALARMLOG28(0,MBC_CLASS_Fatal, 6,"%s%s","此信号量被删除:",m_sSemName);
	    }
	    else if (errno == EINTR)
	    {
			//ALARMLOG28(0,MBC_CLASS_Fatal, 6,"%s%s","信号互斥量值为负数:",m_sSemName);
	    }
	    return false;
    }
#endif
	return true;
}

bool CSemaphore::V_W()
{
#ifdef __OS_WIN32__
    if( !::ReleaseSemaphore(_Handle, 1, 0) ){
		//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s"," 释放信号量失败",m_sSemName);
        return false;
    }
    else
        return true;
#endif

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    struct sembuf Sops = { 0,+MAX_READ,SEM_UNDO };
	semop((int &) _Handle, &Sops, 1);
	return true;
#endif
	return true;
}

bool CSemaphore::V()
{
#ifdef __OS_WIN32__
    if( !::ReleaseSemaphore(_Handle, 1, 0) ){
		//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s"," 释放信号量失败",m_sSemName);
        return false;
    }
    else
        return true;
#endif

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    struct sembuf Sops = { 0,+1,SEM_UNDO };
	semop((int &) _Handle, &Sops, 1);
	return true;
#endif
	return true;
}

bool CSemaphore::getSemEx(int  isem_key, int init_count, int max_count)
{
sprintf(m_sSemName,"%d",isem_key);
m_sSemName[strlen(m_sSemName)]=0;
/*windows上的信号量*/
#ifdef __OS_WIN32__
	char sKeySem[64]={0};
	sprintf (sKeySem,"%d",isem_key);
    _Handle = ::CreateSemaphore(NULL, init_count, max_count, sKeySem);
    if( _Handle == NULL ){
		//ALARMLOG28(0,MBC_CLASS_Fatal, 7,"%s:%d","获取锁资源失败",isem_key);
        return false;
	}
    else
        return true;
#endif

/*unix 上的信号量*/
#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    key_t SEM_KEY = isem_key;
    (int &) _Handle = semget( SEM_KEY, 1, SEM_R|SEM_A|IPC_CREAT|IPC_EXCL );
	if( (int &) _Handle >= 0 ){           //服务端使用
		union semun	Arg;
		Arg.val = MAX_READ;
		semctl( (int &) _Handle, 0, SETVAL, Arg );
	}
	else{                       //客户端使用
		(int &) _Handle = semget( SEM_KEY, 0, SEM_R|SEM_A|IPC_CREAT);
	}
	//判断是否成功
	if( (int &) _Handle < 0 ){
		//ALARMLOG28(0,MBC_CLASS_Fatal, 7,"%s:%d","获取锁资源失败",isem_key);
	    return false;
	}
	else
	    return true;
#endif
	return true;
}
bool CSemaphore::getSemEx(const char * sem_name, int init_count, int max_count)
{
strncpy(m_sSemName,sem_name,18);
m_sSemName[18]=0;
/*windows上的信号量*/
#ifdef __OS_WIN32__
    _Handle = ::CreateSemaphore(NULL, init_count, max_count, sem_name);
    if( _Handle == NULL ){
		//ALARMLOG28(0,MBC_CLASS_Fatal, 7,"%s:%d","获取锁资源失败",sem_name);
        return false;
    }else
        return true;
#endif

/*unix 上的信号量*/
#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    key_t SEM_KEY = atoi(sem_name);
    (int &) _Handle = semget( SEM_KEY, 1, SEM_R|SEM_A|IPC_CREAT|IPC_EXCL );
	if( (int &) _Handle >= 0 ){           //服务端使用
		union semun	Arg;
		Arg.val = MAX_READ;
		semctl( (int &) _Handle, 0, SETVAL, Arg );
	}
	else{                       //客户端使用
		(int &) _Handle = semget( SEM_KEY, 0, SEM_R|SEM_A|IPC_CREAT);
	}
	//判断是否成功
	if( (int &) _Handle < 0 ){
		//ALARMLOG28(0,MBC_CLASS_Fatal, 7,"%s:%d","获取锁资源失败",sem_name);
	    return false;
	}
	else
	    return true;
#endif
	return true;
}

bool CSemaphore::getSem(const char * sem_name, int init_count, int max_count)
{
strncpy(m_sSemName,sem_name,18);
m_sSemName[18]=0;

/*windows上的信号量*/
#ifdef __OS_WIN32__
    _Handle = ::CreateSemaphore(NULL, init_count, max_count, sem_name);
    if( _Handle == NULL ){
		//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s","创建信号量失败",sem_name);
        return false;
   } else {
       return true;
   }
#endif

/*unix 上的信号量*/
#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    key_t SEM_KEY = atoi(sem_name);
    (int &) _Handle = semget( SEM_KEY, 1, SEM_R|SEM_A|IPC_CREAT|IPC_EXCL );
	if( (int &) _Handle >= 0 ){           //服务端使用
		union semun	Arg;
		Arg.val = 1;
		semctl( (int &) _Handle, 0, SETVAL, Arg );
	}
	else{                       //客户端使用
		(int &) _Handle = semget( SEM_KEY, 0, SEM_R|SEM_A|IPC_CREAT);
	}
	//判断是否成功
	if( (int &) _Handle < 0 ){
		//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s","创建信号量失败",sem_name);
	    return false;
	} else {
	    return true;
	}
#endif
	return true;
}

bool CSemaphore::close()
{
#ifdef __OS_WIN32__
    if(_Handle != NULL){
        if ( !::CloseHandle(_Handle) ){
			//ALARMLOG28(0,MBC_CLASS_Memory, 122,"%s:%s"," 关闭信号量失败",m_sSemName);
            return false;
        } else {
            return true;
		}
    }
    else
        return true;
#endif

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
    union semun Arg;
	Arg.val=0;
	if( _Handle != NULL )
	    semctl( (int &) _Handle, 0, IPC_RMID,Arg );
	return true;
#endif
	return true;
}

CSemaphore::CSemaphore():_Handle(NULL)
{
	memset(m_sSemName,'\0',sizeof(m_sSemName));
}

CSemaphore::~CSemaphore()
{
}
