/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fee_shm.h"
#include <errno.h>

#ifdef WIN32

#include <windows.h>
#define  SHM_FILE_PREFIX  "SHMBILL_"


void *AllocShm(key_t shmkey,long iSize,int *piErrCode)
{
    char sShmName[100];
    void *pShm=NULL; 
    HANDLE hMapObject = NULL;

    sprintf(sShmName, "%s_%d",SHM_FILE_PREFIX, shmkey);

    hMapObject = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,iSize,sShmName);

    if ( hMapObject != NULL) {
        pShm = MapViewOfFile ( hMapObject, FILE_MAP_WRITE, 0,0,0);

        if ( pShm == NULL){
            CloseHandle(hMapObject);
            *piErrCode = -1;
            return NULL;
        }
        else {
            memset ( pShm , 0, iSize);
        }
    }
    else {
        *piErrCode = -1;
        return pShm;
    }
    return pShm;

}

void *GetShm(key_t shmkey,int *piErrCode, void * p)
{
    char sShmName[100];
    void *pShm = NULL; 
    HANDLE hMapObject = NULL;
    *piErrCode = 0;

    sprintf(sShmName, "%s_%d",SHM_FILE_PREFIX, shmkey);

    hMapObject = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,0,sShmName);

    if ( hMapObject != NULL) {
        pShm = MapViewOfFile ( hMapObject, FILE_MAP_WRITE, 0,0,0);

        if ( pShm == NULL){
            CloseHandle(hMapObject);
            *piErrCode = -1;
            return NULL;
        }
    }
    else {
        *piErrCode = -1;
        return pShm;
    }
    return pShm;
}
void *AllocOrGetShm(key_t shmkey,long iSize,int *piErrCode)
{
    char sShmName[100];
    void *pShm; 
    HANDLE hMapObject = NULL;

    sprintf(sShmName, "%s_%d",SHM_FILE_PREFIX, shmkey);

    hMapObject = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,*piErrCode,sShmName);

    if ( hMapObject != NULL) {
        pShm = MapViewOfFile ( hMapObject, FILE_MAP_WRITE, 0,0,0);

        if ( pShm == NULL){
            CloseHandle(hMapObject);
            return NULL;
        }
    }
    return pShm;

}

void FreeShm(key_t shmkey)
{
    return;
}

void DetachShm(void * p)
{
    //UnmapViewOfFile(p);
}

int GetShmNattch(key_t shmkey)
{
    return -1;
}

#else

#include <sys/ipc.h>
#include <sys/shm.h>

//20101215 wangs 应江苏项目要求（OCS使用），权限由640改为644
void *AllocShm(key_t shmkey,long iSize,int *piErrCode)
{
    void *pShm; 
    int shmid;
    if((shmid=shmget(shmkey,iSize,0664|IPC_EXCL|IPC_CREAT))==-1){
        *piErrCode=1;
        perror("allocshm0");
        return NULL;
    }
    pShm=shmat(shmid,NULL,0);
    *piErrCode=((long)pShm==-1)?1:0;
    if(*piErrCode)perror("allocshm1");
/*    *p=pShm;*/
    
/*#ifdef TEST
    printf("------------TEST 1------------\n");
    printf("attached address:%x.\n",(int)(*p));
    printf("---------TEST END-----------\n");
#endif  */ 
    
    return pShm;
}
void *AllocOrGetShm(key_t shmkey,long iSize,int *piErrCode)
{
    void *pShm; 
    int shmid;
    if((shmid=shmget(shmkey,iSize,664|IPC_EXCL|IPC_CREAT))==-1){
        if(errno==EEXIST)
            shmid=shmget(shmkey,0,0);
        else{
            *piErrCode=1;
            return NULL;
        }
    }
    pShm=shmat(shmid,0,0);
    *piErrCode=((long)pShm==-1)?1:0;
    return pShm;
}
void FreeShm(key_t shmkey)
{
    int shmid;
    shmid=shmget(shmkey,0,0); /* 取key为shmkey的share memory id */
    
    shmctl(shmid,IPC_RMID,NULL); /* 若key为shmkey的share memory已存在，则删除它并释放所占资源*/
}
void *GetShm(key_t shmkey,int *piErrCode,void *p)
{
    void *pShm;
    int shmid;
    if((shmid=shmget(shmkey,0,0664))==-1){
        *piErrCode=1;
        return piErrCode;
    }
    pShm=shmat(shmid,p,0400);
    *piErrCode=((long)pShm==-1)?1:0;
    return pShm;
}
void DetachShm(void * p)
{
    shmdt((char *)p);
}

bool DetachShmNew(char *p)
{
	int iRes = shmdt(p);
	if(iRes == 0)
		return true;
    else
		return false;
}
int GetShmNattch(key_t shmkey)
{
    int shmid;
    struct shmid_ds ds;
    shmid=shmget(shmkey,0,0);    /* 取key为shmkey的share memory id */
    if(shmctl(shmid, IPC_STAT, &ds) == -1)
        return -1;
    else
        return ds.shm_nattch;
}


#endif


