/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbclear.h	1.0	2010/08/30	<AutoCreate>" */
#ifndef __abmclear_h__
#define __abmclear_h__

#include "controls.h"
//#include "abmclearfile.h"

class hbclear {
public:
	hbclear ();
	~hbclear ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbclear_init();
int initlist() ;
int hbclear_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hbclear_entry(Control *pHandle);
int hbclearCtl13Press(Control *pCtl,int ch,long lParam);
int hbclearCtl1Press(Control *pCtl,int ch,long lParam);
int hbclearCtl2Press(Control *pCtl,int ch,long lParam);
int hbclearCtl3Press(Control *pCtl,int ch,long lParam);

#endif

