/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ticket_j.c	1.0	2007/04/09	<AutoCreate>" */
#include "ticket.h"


Control *ticket_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl6;
    RECT rt; TFILE *pHead;
    ReadFile("ticket.rc", (TFILE **)&pHead);

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
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
   // GetSiteInfo( pHead, 7, &rt);
    //Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    //GetSiteInfo( pHead, 8, &rt);
    //Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl11 ->sData , "网元编码  接收消息数       返回消息数       拒绝处理      处理失败" );
    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 性能统计>>" );
    strcpy( Ctl1  ->sData , "1.刷新" );
    strcpy( Ctl2  ->sData , "2.实时刷新" );
    strcpy( Ctl3  ->sData , "q.退出" );
   // strcpy( Ctl12 ->sData , "流程号过滤:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&ticketCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&ticketCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&ticketCtl3Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&ticketCtl6Press;
    //Ctl13 ->pKeyPress = (KEYPRESS)&ticketCtl13Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&ticket_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&ticket_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&ticket_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&ticket_SysHotKeyPress;
   // Ctl13 ->pHotKeyPress = (KEYPRESS)&ticket_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl3,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl2,    Ctl1,    Ctl1,    Ctl6);
    CtlSetDir(Ctl6    ,    NULL,    Ctl1,    Ctl1,    Ctl3,    Ctl1,   NULL);
    //CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
   // CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,    NULL,    Ctl6,    NULL);

    FreeFileMem( pHead );

    ticket_entry((Control *)Ctl11);

    return ((Control *)Ctl11) ;
}

ticket::ticket()
{
	m_pForm = (Control *) ticket_init ();

}
ticket::~ticket()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int ticket::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

