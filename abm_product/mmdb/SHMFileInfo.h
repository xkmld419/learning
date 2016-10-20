/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef SHMFILEINFO_H_HEADER_INCLUDED
#define SHMFILEINFO_H_HEADER_INCLUDED

#include "UserInfo.h"
#include "UserInfoSHMFileCtl.h"


class SHMFileInfo
{
    public:
        SHMFileInfo();
        ~SHMFileInfo();
		
        static UserInfoBase * m_pSHMFileExternal;
		
        static void refleshPointer();
		
    private:
        static UserInfoBase * m_pSHMInternal;
        static UserInfoBase * m_pFileInternal;
		
        static SHMFileCtl * m_pSHMFileCtl;
};
#endif
