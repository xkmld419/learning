/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbbilltables.h	1.0	2010/05/10	<AutoCreate>" */
#ifndef __abmbilltables_h__
#define __abmbilltables_h__

#include "controls.h"

class hbbilltables {
public:
	hbbilltables ();
	~hbbilltables ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbbilltables_init();
int hbbilltables_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbbilltables_entry(Control *pHandle);
int hbbilltablesCtl13Press(Control *pCtl,int ch,long lParam);
int hbbilltablesCtl1Press(Control *pCtl,int ch,long lParam);
int hbbilltablesCtl2Press(Control *pCtl,int ch,long lParam);
int hbbilltablesCtl3Press(Control *pCtl,int ch,long lParam);

#endif

