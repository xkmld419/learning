#include "CSemaphore.h"
#include <string.h>
#include <string>

#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun
{
	int 	val;
	struct 	semid_ds *buf;
	ushort *array;
};

#ifndef  SEM_A
#define   SEM_A   0200    /* alter permission */
#endif

#ifndef  SEM_R
#define   SEM_R   0400    /* read permission */
#endif


bool CSemaphore::P(bool bWait)
{
    short semflags = SEM_UNDO;
    if(!bWait) {
        semflags = IPC_NOWAIT | semflags;
    }

    struct sembuf Sops = { 0,-1,semflags };

	if(semop(m_iHandle, &Sops, 1) >= 0) {
	    return true;
	}
	else {
	    if(errno == EAGAIN) {
	    }
	    else if(errno == EIDRM) {
	    }
	    else if (errno == EINTR) {
	    }

	    return false;
    }
}

bool CSemaphore::V()
{
    struct sembuf Sops = { 0,+1,SEM_UNDO };

	semop(m_iHandle, &Sops, 1);

	return true;
}

bool CSemaphore::getSem(const char * sem_name, int init_count, int max_count)
{
    key_t SEM_KEY = atoi(sem_name);

    m_iHandle = semget( SEM_KEY, 1, SEM_R|SEM_A|IPC_CREAT|IPC_EXCL );

	if( m_iHandle >= 0 ){        
		union semun	Arg;
		Arg.val = 1;
		semctl( m_iHandle, 0, SETVAL, Arg );
	} else{                       
		m_iHandle = semget( SEM_KEY, 0, SEM_R|SEM_A|IPC_CREAT);
	}
	
	if( m_iHandle < 0 )
	    return false;
	else
	    return true;
}

//add by huff at 2011-03-09
bool CSemaphore::getSem(int sem_name, int init_count, int max_count)
{
    key_t SEM_KEY = sem_name;
    m_iHandle = semget( SEM_KEY, 1, SEM_R|SEM_A|IPC_CREAT|IPC_EXCL );
	if(m_iHandle >= 0 ){         
		union semun	Arg;
		Arg.val = 1;
		semctl( (int &) m_iHandle, 0, SETVAL, Arg );
	}
	else{                       
		m_iHandle = semget( SEM_KEY, 0, SEM_R|SEM_A|IPC_CREAT);
	}
	//�ж��Ƿ�ɹ�
	if( (int &) m_iHandle < 0 )
	    return false;
	else
	    return true;
}

bool CSemaphore::remove()
{

    union semun Arg;

	Arg.val=0;

	if(m_iHandle) semctl( m_iHandle, 0, IPC_RMID,Arg );

	return true;
}

CSemaphore::CSemaphore():m_iHandle(0)
{

}

CSemaphore::~CSemaphore()
{
}



