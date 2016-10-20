/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbappcommand_j.c	1.0	2010/05/11	<AutoCreate>" */
#include "abmappcommand.h"


Control *hbappcommand_init()
{
    Control *Ctl9,*Ctl12,*Ctl13,*Ctl1,*Ctl2,*Ctl3,*Ctl7;
    RECT rt; TFILE *pHead;
    ReadFile("abmappcommand.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl3   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl7   = CtlInit(LABEL    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl12 ->sData , "输入应用命令:" );
    strcpy( Ctl7 ->sData , "当前位置: abmaccess菜单>> 应用命令" );
    strcpy( Ctl1  ->sData , "1.执行" );
    strcpy( Ctl2  ->sData , "q.退出" );

    Ctl13 ->pKeyPress = (KEYPRESS)&hbappcommandCtl13Press;
    Ctl1  ->pKeyPress = (KEYPRESS)&hbappcommandCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&hbappcommandCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&hbappcommandCtl3Press;

    Ctl13 ->pHotKeyPress = (KEYPRESS)&hbappcommand_SysHotKeyPress;
    Ctl1  ->pHotKeyPress = (KEYPRESS)&hbappcommand_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&hbappcommand_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&hbappcommand_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl3  ->bFrame  = 1 ;

    Ctl3  ->bSingle = 0 ;

    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl1,    Ctl1,    Ctl2,    Ctl1,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl3,    NULL,    Ctl2,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl3,    NULL,    Ctl1,    Ctl1,   Ctl13,    Ctl3);
    CtlSetDir(Ctl3    ,   Ctl13,    Ctl1,    Ctl1,    Ctl2,    Ctl1,    Ctl7);
    CtlSetDir(Ctl7    ,   NULL,    NULL,    NULL,    NULL,    NULL,    NULL);

    FreeFileMem( pHead );

    hbappcommand_entry((Control *)Ctl9);

    return ((Control *)Ctl9) ;
}

hbappcommand::hbappcommand()
{
	m_pForm = (Control *) hbappcommand_init ();

}
hbappcommand::~hbappcommand()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hbappcommand::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

