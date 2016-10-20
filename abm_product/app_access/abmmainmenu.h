/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbmainmenu.h	1.0	2010/05/06	<AutoCreate>" */
#ifndef __abmmainmenu_h__
#define __abmmainmenu_h__

#include "controls.h"

class hbmainmenu {
public:
	hbmainmenu ();
	~hbmainmenu ();
	int run ();
       // struct bool UserFlag;
       // struct char sFullStaffID[50];
private:
	Control *m_pForm;
//	bool UserFlag;
//	char sFullStaffID[50];
};
extern bool UserFlag;
extern char sFullStaffID[50];
char * hbtrim( char *sp ) ;
Control *hbmainmenu_init();
int hbmainmenu_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbmainmenu_entry(Control *pHandle);
int hbmainmenuCtl1Press(Control *pCtl,int ch,long lParam);
int hbmainmenuCtl2Press(Control *pCtl,int ch,long lParam);
int hbmainmenuCtl3Press(Control *pCtl,int ch,long lParam);
int hbmainmenuCtl4Press(Control *pCtl,int ch,long lParam);
int hbmainmenuCtl5Press(Control *pCtl,int ch,long lParam);


#endif

