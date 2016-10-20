/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablesmenu.h	1.0	2010/05/19	<AutoCreate>" */
#ifndef __abmtablesmenu_h__
#define __abmtablesmenu_h__

#include "controls.h"

static char hbTmp[50] ;
extern Control *hbtablesmenu_handle;

class hbtablesmenu {
public:
	hbtablesmenu ();
	~hbtablesmenu ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbtablesmenu_init();
int hbtablesmenu_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbtablesmenu_entry(Control *pHandle);
int hbtablesmenuCtl1Press(Control *pCtl,int ch,long lParam);
int hbtablesmenuCtl2Press(Control *pCtl,int ch,long lParam);
int hbtablesmenuCtl3Press(Control *pCtl,int ch,long lParam);

#endif

