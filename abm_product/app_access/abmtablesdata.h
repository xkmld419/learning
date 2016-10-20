/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablesdata.h	1.0	2010/05/19	<AutoCreate>" */
#ifndef __abmtablesdata_h__
#define __abmtablesdata_h__

#include "controls.h"
#define MaxNum 512 



class hbtablesdata {
public:
	hbtablesdata ();
	~hbtablesdata ();
	int run ();
private:
	Control *m_pForm;
};
extern Control *hbtablesdata_handle;
Control *hbtablesdata_init();
int hbtablesdata_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbtablesdata_entry(Control *pHandle);
int hbtablesdataCtl13Press(Control *pCtl,int ch,long lParam);
int hbtablesdataCtl1Press(Control *pCtl,int ch,long lParam);
int hbtablesdataCtl2Press(Control *pCtl,int ch,long lParam);
int hbtablesdataCtl3Press(Control *pCtl,int ch,long lParam);

#endif

