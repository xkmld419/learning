/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbappcommand.h	1.0	2010/05/11	<AutoCreate>" */
#ifndef __abmappcommand_h__
#define __abmappcommand_h__

#include "controls.h"

class hbappcommand {
public:
	hbappcommand ();
	~hbappcommand ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbappcommand_init();
int hbappcommand_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbappcommand_entry(Control *pHandle);
int hbappcommandCtl13Press(Control *pCtl,int ch,long lParam);
int hbappcommandCtl1Press(Control *pCtl,int ch,long lParam);
int hbappcommandCtl2Press(Control *pCtl,int ch,long lParam);
int hbappcommandCtl3Press(Control *pCtl,int ch,long lParam);

#endif

