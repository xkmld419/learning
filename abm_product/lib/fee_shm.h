/*VER: 1*/ 
#ifndef __FEE_SHM__

#define __FEE_SHM__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef key_t
//#define key_t int
typedef int	key_t; 
#endif

void *GetShm(key_t shmkey,int *piErrCode,void *p);

void *AllocShm(key_t shmkey,long iSize,int *piErrCode);

void *AllocOrGetShm(key_t shmkey,long iSize,int *piErrCode);

void FreeShm(key_t shmkey);

void DetachShm(void * p);

bool DetachShmNew(char *p);
int  GetShmNattch(key_t shmkey);

#endif

