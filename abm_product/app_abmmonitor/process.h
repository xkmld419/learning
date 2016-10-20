/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)process.h	1.0	2008/04/15	<AutoCreate>" */
#ifndef __process_h__
#define __process_h__

#include "controls.h"

class process {
public:
	process ();
	~process ();
	int run ();
private:
	Control *m_pForm;
};

Control *process_init();
int process_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void process_entry(Control *pHandle);
int processCtl1Press(Control *pCtl,int ch,long lParam);
int processCtl2Press(Control *pCtl,int ch,long lParam);
int processCtl3Press(Control *pCtl,int ch,long lParam);
int processCtl4Press(Control *pCtl,int ch,long lParam);
int processCtl8Press(Control *pCtl,int ch,long lParam);
int processCtl5Press(Control *pCtl,int ch,long lParam);
int processCtl6Press(Control *pCtl,int ch,long lParam);
int processCtl13Press(Control *pCtl,int ch,long lParam);
int processCtl15Press(Control *pCtl,int ch,long lParam);

#endif

