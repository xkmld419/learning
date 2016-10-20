/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)logctl_j.c	1.0	2011/05/29	<AutoCreate>" */
#include "logctl.h"


Control *logctl_init()
{
    Control *Ctl7,*Ctl9,*Ctl1,*Ctl3,*Ctl6,*Ctl12,*Ctl13,*Ctl14,*Ctl15;
    RECT rt; TFILE *pHead;
    ReadFile("logctl.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl14  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl15  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 系统日志查看>>" );
    strcpy( Ctl1  ->sData , "1.查看" );
    strcpy( Ctl3  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "系统日志:" );
    strcpy( Ctl14 ->sData , "时间:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&logctlCtl1Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&logctlCtl3Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&logctlCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&logctlCtl13Press;
    Ctl15 ->pKeyPress = (KEYPRESS)&logctlCtl15Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&logctl_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&logctl_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&logctl_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&logctl_SysHotKeyPress;
    Ctl15 ->pHotKeyPress = (KEYPRESS)&logctl_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl3  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl3,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl1,    Ctl1,    Ctl1,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,    Ctl1,    Ctl1,    Ctl1,    Ctl1,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,   Ctl15,    Ctl6,   Ctl14);
    CtlSetDir(Ctl14   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl15);
    CtlSetDir(Ctl15   ,    Ctl1,    Ctl6,   Ctl13,    Ctl6,    Ctl6,    NULL);

    FreeFileMem( pHead );

    logctl_entry((Control *)Ctl7);

    return ((Control *)Ctl7) ;
}

logctl::logctl()
{
	m_pForm = (Control *) logctl_init ();

}
logctl::~logctl()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int logctl::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

