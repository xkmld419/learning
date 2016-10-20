/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)checkpointctl.h	1.0	2010/09/30	<AutoCreate>" */
#ifndef __checkpointctl_h__
#define __checkpointctl_h__

#include "controls.h"

class checkpointctl {
public:
	checkpointctl ();
	~checkpointctl ();
	int run ();
private:
	Control *m_pForm;
};

Control *checkpointctl_init();
int checkpointctl_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void checkpointctl_entry(Control *pHandle);
int checkpointctlCtl1Press(Control *pCtl,int ch,long lParam);
int checkpointctlCtl2Press(Control *pCtl,int ch,long lParam);
int checkpointctlCtl6Press(Control *pCtl,int ch,long lParam);
int checkpointctlCtl13Press(Control *pCtl,int ch,long lParam);

#endif

