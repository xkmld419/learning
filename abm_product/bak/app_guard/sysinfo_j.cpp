/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sysinfo_j.c	1.0	2011/07/06	<AutoCreate>" */
#include "sysinfo.h"


Control *sysinfo_init()
{
    Control *Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl6,*Ctl12;
    RECT rt; TFILE *pHead;
    ReadFile("sysinfo.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 系统信息>>" );
    strcpy( Ctl1  ->sData , "1.系统性能" );
    strcpy( Ctl2  ->sData , "2.磁盘空间" );
    strcpy( Ctl3  ->sData , "3.表空间" );
    strcpy( Ctl4  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "系统信息:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&sysinfoCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&sysinfoCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&sysinfoCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&sysinfoCtl4Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&sysinfoCtl6Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&sysinfo_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&sysinfo_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&sysinfo_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&sysinfo_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&sysinfo_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl4,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl2,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl3,    Ctl1,    Ctl1,    Ctl6);
    CtlSetDir(Ctl6    ,    Ctl1,    Ctl1,    Ctl1,    Ctl1,    Ctl1,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL);

    FreeFileMem( pHead );

    sysinfo_entry((Control *)Ctl7);

    return ((Control *)Ctl7) ;
}

sysinfo::sysinfo()
{
	m_pForm = (Control *) sysinfo_init ();

}
sysinfo::~sysinfo()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int sysinfo::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

