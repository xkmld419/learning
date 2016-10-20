/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)hbmodectl_j.c	1.0	2011/05/26	<AutoCreate>" */
#include "hbmodectl.h"


Control *hbmodectl_init()
{
    Control *Ctl7,*Ctl9,*Ctl1,*Ctl4,*Ctl8,*Ctl5,*Ctl6,*Ctl12,*Ctl13,*Ctl16,*Ctl17;
    RECT rt; TFILE *pHead;
    ReadFile("hbmodectl.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl8   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl16  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 10, &rt);
    Ctl17  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 模式切换>>" );
    strcpy( Ctl1  ->sData , "1.查看" );
    strcpy( Ctl4  ->sData , "2.提交" );
    strcpy( Ctl8  ->sData , "3.单点模式" );
    strcpy( Ctl5  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "模式切换:" );
    strcpy( Ctl16 ->sData , "状态值:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&hbmodectlCtl1Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&hbmodectlCtl4Press;
    Ctl8  ->pKeyPress = (KEYPRESS)&hbmodectlCtl8Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&hbmodectlCtl5Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&hbmodectlCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&hbmodectlCtl13Press;
    Ctl17 ->pKeyPress = (KEYPRESS)&hbmodectlCtl17Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl8  ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;
    Ctl17 ->pHotKeyPress = (KEYPRESS)&hbmodectl_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl4  ->iHotKey = '2' ;
    Ctl8  ->iHotKey = '3' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl5,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl1,    Ctl8,    Ctl8,    Ctl8);
    CtlSetDir(Ctl8    ,    Ctl6,    NULL,    Ctl4,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl6,    NULL,    Ctl8,    Ctl1,   Ctl13,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,   Ctl17,    Ctl1,   Ctl17,   Ctl17,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,    Ctl6,    Ctl6,   Ctl16);
    CtlSetDir(Ctl16   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl17);
    CtlSetDir(Ctl17   ,    Ctl6,    Ctl1,    Ctl6,    Ctl1,    Ctl1,    NULL);

    FreeFileMem( pHead );

    hbmodectl_entry((Control *)Ctl7);

    return ((Control *)Ctl7) ;
}

hbmodectl::hbmodectl()
{
	m_pForm = (Control *) hbmodectl_init ();

}
hbmodectl::~hbmodectl()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hbmodectl::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

