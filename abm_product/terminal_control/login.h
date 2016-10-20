/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)login.h	1.0	2006/01/11	<AutoCreate>" */
#ifndef __login_h__
#define __login_h__

#include "controls.h"

class login {
public:
	login ();
	~login ();
	int run ();
private:
	Control *m_pForm;
};

Control *login_init();
int login_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void login_entry(Control *pHandle);
int loginCtl7Press(Control *pCtl,int ch,long lParam);
int loginCtl8Press(Control *pCtl,int ch,long lParam);
int loginCtl9Press(Control *pCtl,int ch,long lParam);
int loginCtl10Press(Control *pCtl,int ch,long lParam);

#endif

