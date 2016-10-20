/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hblogin_j.c	1.0	2010/05/06	<AutoCreate>" */
#include "abmlogin.h"

Control *hblogin_init()
{
    Control *Ctl1,*Ctl2,*Ctl3,*Ctl5,*Ctl7,*Ctl8,*Ctl9,*Ctl10,*Ctl11,*Ctl12,*Ctl13;
    RECT rt; TFILE *pHead;
    ReadFile("abmlogin.rc", (TFILE **)&pHead);

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
    
    GetSiteInfo( pHead, 8, &rt);
    Ctl11   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    
    GetSiteInfo( pHead, 9, &rt);
    Ctl12   = CtlInit(LABEL    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    GetSiteInfo( pHead, 10, &rt);
    Ctl13   = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    
    strcpy( Ctl3  ->sData , "abmaccess交互式查询" );
    strcpy( Ctl5  ->sData , "密码:" );
    strcpy( Ctl8  ->sData , "1.确认" );
    strcpy( Ctl9  ->sData , "q.退出" );
    strcpy( Ctl10 ->sData , "m.修改密码" );
    strcpy( Ctl12 ->sData , "工号:" );

    Ctl7  ->iAdditional = PASSWD ;
    Ctl13 ->iAdditional = STRING ;
    //Ctl13 ->bFrame = 1;

    Ctl7  ->pKeyPress = (KEYPRESS)&hbloginCtl7Press;
    Ctl8  ->pKeyPress = (KEYPRESS)&hbloginCtl8Press;
    Ctl9  ->pKeyPress = (KEYPRESS)&hbloginCtl9Press;
    Ctl10 ->pKeyPress = (KEYPRESS)&hbloginCtl10Press;
    
    Ctl13  ->pKeyPress = (KEYPRESS)&hbloginCtl13Press;

    Ctl7  ->pHotKeyPress = (KEYPRESS)&hblogin_SysHotKeyPress;
    Ctl8  ->pHotKeyPress = (KEYPRESS)&hblogin_SysHotKeyPress;
    Ctl9  ->pHotKeyPress = (KEYPRESS)&hblogin_SysHotKeyPress;
    Ctl10 ->pHotKeyPress = (KEYPRESS)&hblogin_SysHotKeyPress;
    
    Ctl13 ->pHotKeyPress = (KEYPRESS)&hblogin_SysHotKeyPress;

    Ctl8  ->iHotKey = '1' ;
    Ctl9  ->iHotKey = 'q' ;
    Ctl10 ->iHotKey = 'm' ;

    CtlSetDir(Ctl1    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl2); 
    CtlSetDir(Ctl2    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl3); 
    CtlSetDir(Ctl3    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl5); 
    CtlSetDir(Ctl5    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl13);
    CtlSetDir(Ctl13   ,    Ctl8,    Ctl7,    NULL,    NULL,    Ctl7,    Ctl7);
    CtlSetDir(Ctl7    ,    Ctl13,    Ctl8,    NULL,    NULL,    Ctl8,    Ctl8);
    CtlSetDir(Ctl8    ,    Ctl7,    NULL,   Ctl10,   Ctl9,    Ctl9,     Ctl9);
    CtlSetDir(Ctl9    ,    Ctl7,    NULL,    Ctl8,   Ctl10,   Ctl10,    Ctl10);
    CtlSetDir(Ctl10   ,    Ctl7,    NULL,    Ctl9,    Ctl8,   Ctl13,    Ctl11);
    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL);
     
    
    FreeFileMem( pHead );

    hblogin_entry((Control *)Ctl1);

    return ((Control *)Ctl1) ;
}

hblogin::hblogin()
{
	m_pForm = (Control *) hblogin_init ();

}
hblogin::~hblogin()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hblogin::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

