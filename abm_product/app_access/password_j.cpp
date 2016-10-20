/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)password_j.c	1.0	2005/03/15	<AutoCreate>" */
#include "password.h"


Control *password_init()
{
    Control *Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl5,*Ctl6,*Ctl7,*Ctl8,*Ctl9,*Ctl10,*Ctl11;
    RECT rt; TFILE *pHead;
    ReadFile("password.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl1   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl2   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl3   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl4   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl5   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl6   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl7   = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl8   = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl9   = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl10  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,10, &rt);
    Ctl11  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl3  ->sData , "修改密码" );
    strcpy( Ctl4  ->sData , "原密码:" );
    strcpy( Ctl5  ->sData , "新密码:" );
    strcpy( Ctl6  ->sData , "新密码:" );
    strcpy( Ctl10 ->sData , "确认" );
    strcpy( Ctl11 ->sData , "退出" );

    Ctl7  ->iAdditional = PASSWD ;
    Ctl8  ->iAdditional = PASSWD ;
    Ctl9  ->iAdditional = PASSWD ;

    Ctl7  ->pKeyPress = (KEYPRESS)&passwordCtl7Press;
    Ctl8  ->pKeyPress = (KEYPRESS)&passwordCtl8Press;
    Ctl9  ->pKeyPress = (KEYPRESS)&passwordCtl9Press;
    Ctl10 ->pKeyPress = (KEYPRESS)&passwordCtl10Press;
    Ctl11 ->pKeyPress = (KEYPRESS)&passwordCtl11Press;

    Ctl7  ->pHotKeyPress = (KEYPRESS)&password_SysHotKeyPress;
    Ctl8  ->pHotKeyPress = (KEYPRESS)&password_SysHotKeyPress;
    Ctl9  ->pHotKeyPress = (KEYPRESS)&password_SysHotKeyPress;
    Ctl10 ->pHotKeyPress = (KEYPRESS)&password_SysHotKeyPress;
    Ctl11 ->pHotKeyPress = (KEYPRESS)&password_SysHotKeyPress;

    CtlSetDir(Ctl1    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl2);
    CtlSetDir(Ctl2    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl3);
    CtlSetDir(Ctl3    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl4);
    CtlSetDir(Ctl4    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl5);
    CtlSetDir(Ctl5    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl6);
    CtlSetDir(Ctl6    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    Ctl8,    NULL,    NULL,    Ctl8,    Ctl8);
    CtlSetDir(Ctl8    ,    Ctl7,    Ctl9,    NULL,    NULL,    Ctl9,    Ctl9);
    CtlSetDir(Ctl9    ,    Ctl8,   Ctl10,    NULL,    NULL,   Ctl10,   Ctl10);
    CtlSetDir(Ctl10   ,    Ctl9,    NULL,    NULL,   Ctl11,   Ctl11,   Ctl11);
    CtlSetDir(Ctl11   ,    Ctl9,    NULL,   Ctl10,    NULL,    Ctl7,    NULL);

    FreeFileMem( pHead );

    password_entry((Control *)Ctl1);

    return ((Control *)Ctl1) ;
}

password::password()
{
	m_pForm = (Control *) password_init ();

}
password::~password()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int password::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

