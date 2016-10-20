/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Server.h"

extern "C" {

//获取共享缓冲地址
int GetShmAddress(TSHMCMA **ppTable,int iShmKey)
{
    int iShmID;
    
    //获取共享内存ID
    if ((iShmID = shmget(iShmKey,0,0))<0){
        return -1;
    }

    //get the buff addr.
    *ppTable = (TSHMCMA *)shmat(iShmID,(void *)0,0);
    
    if (-1 == (long)(*ppTable)){
        return -2;
    }

    return 0;
}

} // End extern "C"

