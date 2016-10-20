/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hblogin.h	1.0	2010/05/06	<AutoCreate>" */
#ifndef __abmlogin_h__
#define __abmlogin_h__

#include "controls.h"

#ifdef erase
#undef erase
#endif
#ifdef clear
#undef clear
#endif
#ifdef move
#undef move
#endif

static char ssFullStaffID[50];

void hbaccessLog(int type,char *result,char *cmd,char *res);

class hblogin {
public:
	hblogin ();
	~hblogin ();
	int run ();
private:
	Control *m_pForm;
};

Control *hblogin_init();
int hblogin_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void hblogin_entry(Control *pHandle);
bool checkPwd(char * pwd);
int hbloginCtl7Press(Control *pCtl,int ch,long lParam);
int hbloginCtl8Press(Control *pCtl,int ch,long lParam);
int hbloginCtl9Press(Control *pCtl,int ch,long lParam);
int hbloginCtl10Press(Control *pCtl,int ch,long lParam);
int hbloginCtl13Press(Control *pCtl,int ch,long lParam);

#endif

