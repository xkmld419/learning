/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)password.h	1.0	2005/03/15	<AutoCreate>" */
#ifndef __password_h__
#define __password_h__

#include "controls.h"

class password {
public:
	password ();
	~password ();
	int run ();
private:
	Control *m_pForm;
};

Control *password_init();
int password_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void password_entry(Control *pHandle);
int passwordCtl7Press(Control *pCtl,int ch,long lParam);
int passwordCtl8Press(Control *pCtl,int ch,long lParam);
int passwordCtl9Press(Control *pCtl,int ch,long lParam);
int passwordCtl10Press(Control *pCtl,int ch,long lParam);
int passwordCtl11Press(Control *pCtl,int ch,long lParam);

#endif

