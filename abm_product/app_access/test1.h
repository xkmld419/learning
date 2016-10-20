/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)test1.h	1.0	2012/03/13	<AutoCreate>" */
#ifndef __test1_h__
#define __test1_h__

#include "controls.h"

class test1 {
public:
	test1 ();
	~test1 ();
	int run ();
private:
	Control *m_pForm;
};

Control *test1_init();
int test1_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void test1_entry(Control *pHandle);
int test1Ctl1Press(Control *pCtl,int ch,long lParam);
int test1Ctl2Press(Control *pCtl,int ch,long lParam);
int test1Ctl3Press(Control *pCtl,int ch,long lParam);
int test1Ctl4Press(Control *pCtl,int ch,long lParam);
int test1Ctl5Press(Control *pCtl,int ch,long lParam);

#endif

