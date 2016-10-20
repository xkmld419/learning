/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)mainmenu_j.c	1.0	2011/06/30	<AutoCreate>" */
#include "mainmenu.h"


Control *mainmenu_init()
{
    Control *Ctl12,*Ctl13,*Ctl7,*Ctl9,*Ctl1,*Ctl10,*Ctl2,*Ctl3,*Ctl5,*Ctl4,*Ctl19,*Ctl18,*Ctl17,*Ctl15,*Ctl6;
    RECT rt; TFILE *pHead;
    ReadFile("mainmenu.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl10  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,10, &rt);
    Ctl19  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,11, &rt);
    Ctl18  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,12, &rt);
    Ctl17  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,13, &rt);
    Ctl15  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,14, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl12 ->sData , "功能过滤:" );
    strcpy( Ctl7  ->sData , "当前位置: hssmonitor>>HSS监控>>" );
    strcpy( Ctl1  ->sData , "1.消息队列" );
    strcpy( Ctl10 ->sData , "2.进程状态" );
    strcpy( Ctl2  ->sData , "3.性能统计" );
    strcpy( Ctl3  ->sData , "4.参数" );
    strcpy( Ctl5  ->sData , "5.系统显示" );
    strcpy( Ctl4  ->sData , "q.退出" );
    strcpy( Ctl19 ->sData , "6.落地信息" );
    strcpy( Ctl18 ->sData , "7.日志查看" );
    strcpy( Ctl17 ->sData , "8.模式切换" );
    strcpy( Ctl15 ->sData , "9.能力管理" );

    Ctl13 ->pKeyPress = (KEYPRESS)&mainmenuCtl13Press;
    Ctl1  ->pKeyPress = (KEYPRESS)&mainmenuCtl1Press;
    Ctl10 ->pKeyPress = (KEYPRESS)&mainmenuCtl10Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&mainmenuCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&mainmenuCtl3Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&mainmenuCtl5Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&mainmenuCtl4Press;
    Ctl19 ->pKeyPress = (KEYPRESS)&mainmenuCtl19Press;
    Ctl18 ->pKeyPress = (KEYPRESS)&mainmenuCtl18Press;
    Ctl17 ->pKeyPress = (KEYPRESS)&mainmenuCtl17Press;
    Ctl15 ->pKeyPress = (KEYPRESS)&mainmenuCtl15Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&mainmenuCtl6Press;

    Ctl13 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl1  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl10 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl19 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl18 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl17 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl15 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl10 ->iHotKey = '2' ;
    Ctl2  ->iHotKey = '3' ;
    Ctl3  ->iHotKey = '4' ;
    Ctl5  ->iHotKey = '5' ;
    Ctl4  ->iHotKey = 'q' ;
    Ctl19 ->iHotKey = '6' ;
    Ctl18 ->iHotKey = '7' ;
    Ctl17 ->iHotKey = '8' ;
    Ctl15 ->iHotKey = '9' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl1,    NULL,    NULL,    Ctl1,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,   Ctl13,   Ctl19,    Ctl4,   Ctl10,   Ctl10,   Ctl10);
    CtlSetDir(Ctl10   ,   Ctl13,   Ctl18,    Ctl1,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,   Ctl13,   Ctl17,   Ctl10,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,   Ctl13,   Ctl15,    Ctl2,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,   Ctl13,    NULL,    Ctl3,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,   Ctl13,    NULL,    Ctl5,    Ctl1,   Ctl19,   Ctl19);
    CtlSetDir(Ctl19   ,    Ctl1,    NULL,   Ctl15,   Ctl18,   Ctl18,   Ctl18);
    CtlSetDir(Ctl18   ,   Ctl10,    NULL,   Ctl19,   Ctl17,   Ctl17,   Ctl17);
    CtlSetDir(Ctl17   ,    Ctl2,    NULL,   Ctl18,   Ctl15,   Ctl15,   Ctl15);
    CtlSetDir(Ctl15   ,    Ctl3,    NULL,   Ctl17,   Ctl19,   Ctl6,    Ctl6);
    CtlSetDir(Ctl6    ,    NULL,    Ctl1,    Ctl1,    Ctl4,    Ctl1,    NULL);

    FreeFileMem( pHead );

    mainmenu_entry((Control *)Ctl12);

    return ((Control *)Ctl12) ;
}

mainmenu::mainmenu()
{
	m_pForm = (Control *) mainmenu_init ();

}
mainmenu::~mainmenu()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int mainmenu::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

