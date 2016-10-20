/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablesmenu_j.c	1.0	2010/05/19	<AutoCreate>" */
#include "abmtablesmenu.h"
//extern char hb_List[MaxNum][200] ;
//extern char *hb_Tmp[MaxNum] ;

Control *hbtablesmenu_init()
{
    Control *Ctl9,*Ctl12,*Ctl1,*Ctl2,*Ctl3,*Ctl7;
    RECT rt; TFILE *pHead;
    ReadFile("abmtablesmenu.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl3   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
     GetSiteInfo( pHead, 5, &rt);
    Ctl7   = CtlInit(LABEL    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    
    Ctl3->bSingle = 1 ;

    strcpy( Ctl12 ->sData ,"ABM系统表名浏览" );
    strcpy( Ctl7 ->sData , "当前位置: abmaccess菜单>>ABM系统表名浏览" );
    strcpy( Ctl1  ->sData , "1.执行" );
    strcpy( Ctl2  ->sData , "q.退出" );

    Ctl1  ->pKeyPress = (KEYPRESS)&hbtablesmenuCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&hbtablesmenuCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&hbtablesmenuCtl3Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&hbtablesmenu_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&hbtablesmenu_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&hbtablesmenu_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl3  ->bFrame  = 1 ;

    Ctl3  ->bSingle = 0 ;

    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl3,    NULL,    Ctl2,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl3,    NULL,    Ctl1,    Ctl1,    Ctl1,    Ctl3);
    CtlSetDir(Ctl3    ,    NULL,    Ctl1,    Ctl1,    Ctl2,    Ctl1,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL);

    FreeFileMem( pHead );

    hbtablesmenu_entry((Control *)Ctl9);

    return ((Control *)Ctl9) ;
}

hbtablesmenu::hbtablesmenu()
{
	m_pForm = (Control *) hbtablesmenu_init ();

}
hbtablesmenu::~hbtablesmenu()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hbtablesmenu::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

