/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablescatalog.h	1.0	2010/05/19	<AutoCreate>" */
#ifndef __abmtablescatalog_h__
#define __abmtablescatalog_h__

#include "controls.h"



class hbtablescatalog {
public:
	hbtablescatalog ();
	~hbtablescatalog ();
	int run ();
private:
	Control *m_pForm;
};
extern Control *hbtablescatalog_handle;
Control *hbtablescatalog_init();
int hbtablescatalog_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbtablescatalog_entry(Control *pHandle);
int hbtablescatalogCtl13Press(Control *pCtl,int ch,long lParam);
int hbtablescatalogCtl1Press(Control *pCtl,int ch,long lParam);
int hbtablescatalogCtl2Press(Control *pCtl,int ch,long lParam);
int hbtablescatalogCtl3Press(Control *pCtl,int ch,long lParam);

#endif

