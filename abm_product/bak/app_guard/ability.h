/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ability.h	1.0	2011/07/15	<AutoCreate>" */
#ifndef __ability_h__
#define __ability_h__


#include "controls.h"

class ability {
public:
	ability ();
	~ability ();
	int run ();
private:
	Control *m_pForm;
};

Control *ability_init();
int ability_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void ability_entry(Control *pHandle);
int abilityCtl1Press(Control *pCtl,int ch,long lParam);
int abilityCtl2Press(Control *pCtl,int ch,long lParam);
int abilityCtl3Press(Control *pCtl,int ch,long lParam);
int abilityCtl4Press(Control *pCtl,int ch,long lParam);
int abilityCtl6Press(Control *pCtl,int ch,long lParam);
int abilityCtl13Press(Control *pCtl,int ch,long lParam);

#endif

