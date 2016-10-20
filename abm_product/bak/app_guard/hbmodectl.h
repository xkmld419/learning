/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)hbmodectl.h	1.0	2011/05/26	<AutoCreate>" */
#ifndef __hbmodectl_h__
#define __hbmodectl_h__
#define __IN_HB_MONITOR__
#include "controls.h"

class hbmodectl {
public:
	hbmodectl ();
	~hbmodectl ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbmodectl_init();
int hbmodectl_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbmodectl_entry(Control *pHandle);
int hbmodectlCtl1Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl4Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl8Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl5Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl6Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl13Press(Control *pCtl,int ch,long lParam);
int hbmodectlCtl17Press(Control *pCtl,int ch,long lParam);

#endif

