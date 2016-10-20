/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ability_j.c	1.0	2011/07/15	<AutoCreate>" */
#include "ability.h"


Control *ability_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl6,*Ctl12,*Ctl13;
    RECT rt; TFILE *pHead;
    ReadFile("ability.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl11  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl11 ->sData , "能力编码   能力名称              网元编码      网元名称      状态" );
    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 能力管理>>" );
    strcpy( Ctl1  ->sData , "1.刷新" );
    strcpy( Ctl2  ->sData , "2.能力启用" );
    strcpy( Ctl3  ->sData , "3.能力停止" );
    strcpy( Ctl4  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "网元号过滤:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&abilityCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&abilityCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&abilityCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&abilityCtl4Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&abilityCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&abilityCtl13Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&ability_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl4,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl2,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl3,    Ctl1,   Ctl6,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,    Ctl1,    Ctl1,    Ctl3,    Ctl1,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,    NULL,    Ctl6,    NULL);

    FreeFileMem( pHead );

    ability_entry((Control *)Ctl11);

    return ((Control *)Ctl11) ;
}

ability::ability()
{
	m_pForm = (Control *) ability_init ();

}
ability::~ability()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int ability::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

