/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)paramset.h	1.0	2010/07/06	<AutoCreate>" */
#ifndef __paramset_h__
#define __paramset_h__

#include "controls.h"

class paramset {
public:
	paramset ();
	~paramset ();
	int run ();
private:
	Control *m_pForm;
};

Control *paramset_init();
int paramset_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void paramset_entry(Control *pHandle);
int paramsetCtl1Press(Control *pCtl,int ch,long lParam);
int paramsetCtl2Press(Control *pCtl,int ch,long lParam);
int paramsetCtl3Press(Control *pCtl,int ch,long lParam);
int paramsetCtl4Press(Control *pCtl,int ch,long lParam);
int paramsetCtl5Press(Control *pCtl,int ch,long lParam);
int paramsetCtl6Press(Control *pCtl,int ch,long lParam);
int paramsetCtl13Press(Control *pCtl,int ch,long lParam);
int paramsetCtl15Press(Control *pCtl,int ch,long lParam);
int paramsetCtl17Press(Control *pCtl,int ch,long lParam);
int paramsetCtl19Press(Control *pCtl,int ch,long lParam);

#endif

