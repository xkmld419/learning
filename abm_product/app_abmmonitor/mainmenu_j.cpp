/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)mainmenu_j.c	1.0	2007/04/09	<AutoCreate>" */
#include "mainmenu.h"

int g_iopen = 0;

Control *mainmenu_init()
{
    Control *Ctl12,*Ctl13,*Ctl7,*Ctl9,*Ctl1,*Ctl10,*Ctl2,*Ctl3,*Ctl4,*Ctl6;
    RECT rt; TFILE *pHead;
    int i = 0;
    //更换焦点
		if( g_iopen ) {
        ReadFile("mainmenu_s.rc", (TFILE **)&pHead);		
		} else {
		    ReadFile("mainmenu.rc", (TFILE **)&pHead);
		}

		if( !g_iopen ) {		
	      GetSiteInfo( pHead, i++, &rt);
	      Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
	      GetSiteInfo( pHead, i++, &rt);
	      Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
  	}
    GetSiteInfo( pHead, i++, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl10  = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, i++, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
		if( g_iopen ) {		
	      GetSiteInfo( pHead, i++, &rt);
	      Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
	      GetSiteInfo( pHead, i++, &rt);
	      Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
  	}
    strcpy( Ctl12 ->sData , "APP号过滤:" );
    strcpy( Ctl7  ->sData , "当前位置: ABM监控>>" );
    strcpy( Ctl1  ->sData , "1.消息队列" );
    strcpy( Ctl10 ->sData , "2.共享内存" );
    strcpy( Ctl2  ->sData , "3.进程管理" );
    strcpy( Ctl3  ->sData , "4.处理状态" );
    strcpy( Ctl4  ->sData , "q.退出" );

    Ctl13 ->pKeyPress = (KEYPRESS)&mainmenuCtl13Press;
    Ctl1  ->pKeyPress = (KEYPRESS)&mainmenuCtl1Press;
    Ctl10 ->pKeyPress = (KEYPRESS)&mainmenuCtl10Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&mainmenuCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&mainmenuCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&mainmenuCtl4Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&mainmenuCtl6Press;

    Ctl13 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl1  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl10 ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&mainmenu_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl10 ->iHotKey = '2' ;
    Ctl2  ->iHotKey = '3' ;
    Ctl3  ->iHotKey = '4' ;
    Ctl4  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,   Ctl13,    NULL,    Ctl4,   Ctl10,   Ctl10,   Ctl10);
    CtlSetDir(Ctl10   ,   Ctl13,    NULL,    Ctl1,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,   Ctl13,    NULL,   Ctl10,    Ctl4,    Ctl4,    Ctl4);
   // CtlSetDir(Ctl3    ,   Ctl13,    NULL,    Ctl2,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,   Ctl13,    NULL,    Ctl2,    Ctl1,   Ctl13,    Ctl6);
    if( !g_iopen ) {
	      CtlSetDir(Ctl6    ,    NULL,    Ctl1,    Ctl1,    Ctl4,    Ctl1,    NULL);
	      CtlSetDir(Ctl13   ,    Ctl1,    Ctl1,    NULL,    NULL,    Ctl1,    Ctl7);
		} else {
			  CtlSetDir(Ctl6    ,    NULL,    Ctl1,    Ctl1,    Ctl4,    Ctl1,    Ctl12);
	      CtlSetDir(Ctl13   ,    Ctl1,    Ctl1,    NULL,    NULL,    Ctl1,    NULL);
		}
    FreeFileMem( pHead );
		if( g_iopen ) {
	      mainmenu_entry((Control *)Ctl7);
			  return ((Control *)Ctl7) ;
		}
		g_iopen = 1;
		
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

