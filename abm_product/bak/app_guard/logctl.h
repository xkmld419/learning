/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)logctl.h	1.0	2011/05/29	<AutoCreate>" */
#ifndef __logctl_h__
#define __logctl_h__

#include "controls.h"

class logctl {
public:
	logctl ();
	~logctl ();
	int run ();
private:
	Control *m_pForm;
};

Control *logctl_init();
int logctl_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void logctl_entry(Control *pHandle);
int logctlCtl1Press(Control *pCtl,int ch,long lParam);
int logctlCtl3Press(Control *pCtl,int ch,long lParam);
int logctlCtl6Press(Control *pCtl,int ch,long lParam);
int logctlCtl13Press(Control *pCtl,int ch,long lParam);
int logctlCtl15Press(Control *pCtl,int ch,long lParam);

#endif

