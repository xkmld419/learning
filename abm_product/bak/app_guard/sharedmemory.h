/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sharedmemory.h	1.0	2007/04/13	<AutoCreate>" */
#ifndef __sharedmemory_h__
#define __sharedmemory_h__

#include "controls.h"

class SharedMemInfo {
  public:
    long m_iSHMID;
    char m_sIpcName[101];
    char m_sSHMName[101];
    char m_sExistState[20];
    char m_sUsedInfo[101];
    int  m_iUserdPercent;
};

int SetSHMListData (SharedMemInfo *pMemInfo = 0x0);



#ifndef _ONLY_USE_SET_SHM_LIST_DATA

    class sharedmemory {
    public:
    	sharedmemory ();
    	~sharedmemory ();
    	int run ();
    private:
    	Control *m_pForm;
    };
    
    Control *sharedmemory_init();
    int sharedmemory_SysHotKeyPress(Control *pCtl, int ch, long lParam);
    void sharedmemory_entry(Control *pHandle);
    int sharedmemoryCtl1Press(Control *pCtl,int ch,long lParam);
    int sharedmemoryCtl5Press(Control *pCtl,int ch,long lParam);
    int sharedmemoryCtl6Press(Control *pCtl,int ch,long lParam);

#endif

#endif
