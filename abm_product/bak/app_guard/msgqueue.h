/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)msgqueue.h	1.0	2007/04/09	<AutoCreate>" */
#ifndef __msgqueue_h__
#define __msgqueue_h__

#include "controls.h"

class msgqueue {
public:
	msgqueue ();
	~msgqueue ();
	int run ();
private:
	Control *m_pForm;
};

Control *msgqueue_init();
int msgqueue_SysHotKeyPress(Control *pCtl, int ch, long lParam);
void msgqueue_entry(Control *pHandle);
int msgqueueCtl1Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl2Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl3Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl4Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl5Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl6Press(Control *pCtl,int ch,long lParam);
int msgqueueCtl13Press(Control *pCtl,int ch,long lParam);

#endif

