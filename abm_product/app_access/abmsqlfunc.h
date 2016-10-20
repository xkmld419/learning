/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbsqlfunc.h	1.0	2010/05/25	<AutoCreate>" */
#ifndef __abmsqlfunc_h__
#define __abmsqlfunc_h__

#include "controls.h"


class hbsqlfunc {
public:
	hbsqlfunc ();
	~hbsqlfunc ();
	int run ();
private:
	Control *m_pForm;
};

Control *hbsqlfunc_init();
int hbsqlfunc_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void Log( char const *format, ...) ;
void hbsqlfunc_entry(Control *pHandle);
int hbsqlfuncCtl13Press(Control *pCtl,int ch,long lParam);
int hbsqlfuncCtl1Press(Control *pCtl,int ch,long lParam);
int hbsqlfuncCtl2Press(Control *pCtl,int ch,long lParam);
int hbsqlfuncCtl3Press(Control *pCtl,int ch,long lParam);

#endif

