/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ticket.h	1.0	2007/04/09	<AutoCreate>" */
#ifndef __ticket_h__
#define __ticket_h__

#include "controls.h"

class ticket {
public:
	ticket ();
	~ticket ();
	int run ();
private:
	Control *m_pForm;
};

Control *ticket_init();
int ticket_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void ticket_entry(Control *pHandle);
int ticketCtl1Press(Control *pCtl,int ch,long lParam);
int ticketCtl2Press(Control *pCtl,int ch,long lParam);
int ticketCtl3Press(Control *pCtl,int ch,long lParam);
int ticketCtl6Press(Control *pCtl,int ch,long lParam);
int ticketCtl13Press(Control *pCtl,int ch,long lParam);

#endif

