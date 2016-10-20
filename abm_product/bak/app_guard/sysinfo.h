/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sysinfo.h	1.0	2011/07/06	<AutoCreate>" */
#ifndef __sysinfo_h__
#define __sysinfo_h__

#include "controls.h"

class sysinfo {
public:
	sysinfo ();
	~sysinfo ();
	int run ();
private:
	Control *m_pForm;
};
class TabSpace
{
public:
    TabSpace(void):m_iSize(0),m_iUsed(0),m_iUtili(0)
    {};
    ~TabSpace(void){};
public:
    char m_strName[128];
    char m_strType[128];
    char m_strExtent[128];
    int m_iSize;
    int m_iUsed;
    int m_iUtili;
};

Control *sysinfo_init();
int sysinfo_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void sysinfo_entry(Control *pHandle);
int sysinfoCtl1Press(Control *pCtl,int ch,long lParam);
int sysinfoCtl2Press(Control *pCtl,int ch,long lParam);
int sysinfoCtl3Press(Control *pCtl,int ch,long lParam);
int sysinfoCtl4Press(Control *pCtl,int ch,long lParam);
int sysinfoCtl6Press(Control *pCtl,int ch,long lParam);

#endif

