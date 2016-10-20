/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)test1_j.c	1.0	2012/03/13	<AutoCreate>" */
#include "test1.h"


Control *test1_init()
{
    Control *Ctl9,*Ctl12,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl5;
    RECT rt; TFILE *pHead;
    ReadFile("test1.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl12 ->sData , "abmaccess菜单" );
    strcpy( Ctl1  ->sData , "1.ABM系统表名浏览" );
    strcpy( Ctl2  ->sData , "2.SQL功能" );
    strcpy( Ctl3  ->sData , "3.应用命令" );
    strcpy( Ctl4  ->sData , "4.清理过期文件" );
    strcpy( Ctl5  ->sData , "q.退出" );

    Ctl1  ->pKeyPress = (KEYPRESS)&test1Ctl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&test1Ctl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&test1Ctl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&test1Ctl4Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&test1Ctl5Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&test1_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&test1_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&test1_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&test1_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&test1_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = '4' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;

    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl4,    Ctl2,    NULL,    NULL,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl1,    Ctl3,    NULL,    NULL,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl2,    Ctl4,    NULL,    NULL,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl2,    Ctl4,    NULL,    NULL,    Ctl4,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl3,    Ctl1,    NULL,    NULL,    Ctl1,    NULL);

    FreeFileMem( pHead );

    test1_entry((Control *)Ctl9);

    return ((Control *)Ctl9) ;
}

test1::test1()
{
	m_pForm = (Control *) test1_init ();

}
test1::~test1()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int test1::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

