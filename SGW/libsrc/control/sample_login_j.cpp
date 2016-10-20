/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)login_j.c	1.0	2006/01/11	<AutoCreate>" */
#include "login.h"


Control *login_init()
{
    Control *Ctl1,*Ctl2,*Ctl3,*Ctl5,*Ctl7,*Ctl8,*Ctl9,*Ctl10;
    RECT rt; TFILE *pHead;
    ReadFile("login.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl1   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl2   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl3   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl5   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl7   = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl8   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl9   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl10  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl3  ->sData , "系统监控" );
    strcpy( Ctl5  ->sData , "密码:" );
    strcpy( Ctl8  ->sData , "确认" );
    strcpy( Ctl9  ->sData , "退出" );
    strcpy( Ctl10 ->sData , "修改密码" );

    Ctl7  ->iAdditional = PASSWD ;

    Ctl7  ->pKeyPress = (KEYPRESS)&loginCtl7Press;
    Ctl8  ->pKeyPress = (KEYPRESS)&loginCtl8Press;
    Ctl9  ->pKeyPress = (KEYPRESS)&loginCtl9Press;
    Ctl10 ->pKeyPress = (KEYPRESS)&loginCtl10Press;

    Ctl7  ->pHotKeyPress = (KEYPRESS)&login_SysHotKeyPress;
    Ctl8  ->pHotKeyPress = (KEYPRESS)&login_SysHotKeyPress;
    Ctl9  ->pHotKeyPress = (KEYPRESS)&login_SysHotKeyPress;
    Ctl10 ->pHotKeyPress = (KEYPRESS)&login_SysHotKeyPress;

    CtlSetDir(Ctl1    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl2);
    CtlSetDir(Ctl2    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl3);
    CtlSetDir(Ctl3    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl5);
    CtlSetDir(Ctl5    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    Ctl8,    NULL,    NULL,    Ctl8,    Ctl8);
    CtlSetDir(Ctl8    ,    Ctl7,    NULL,    NULL,    Ctl9,    Ctl9,    Ctl9);
    CtlSetDir(Ctl9    ,    Ctl7,    NULL,    Ctl8,   Ctl10,   Ctl10,   Ctl10);
    CtlSetDir(Ctl10   ,    Ctl7,    NULL,    Ctl9,    NULL,    Ctl7,    NULL);

    FreeFileMem( pHead );

    login_entry((Control *)Ctl1);

    return ((Control *)Ctl1) ;
}

login::login()
{
	m_pForm = (Control *) login_init ();

}
login::~login()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int login::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}


