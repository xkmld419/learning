/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)mainmenu.h	1.0	2007/04/09	<AutoCreate>" */
#ifndef __mainmenu_h__
#define __mainmenu_h__

#include "controls.h"

#define PROCESS_NAME_LEN 21

class mainmenu {
public:
	mainmenu ();
	~mainmenu ();
	int run ();
private:
	Control *m_pForm;
};

Control *mainmenu_init();
int mainmenu_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void mainmenu_entry(Control *pHandle);
int mainmenuCtl13Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl1Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl10Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl2Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl3Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl4Press(Control *pCtl,int ch,long lParam);
int mainmenuCtl6Press(Control *pCtl,int ch,long lParam);

#endif

