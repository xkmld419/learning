/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef WF_SERVER_H__
#define WF_SERVER_H__

#include "WfPublic.h"

extern "C" {

//获取共享缓冲地址
int GetShmAddress(TSHMCMA **ppTable,int iShmKey);

}

#endif


