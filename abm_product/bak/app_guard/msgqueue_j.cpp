/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)msgqueue_j.c	1.0	2007/04/09	<AutoCreate>" */
#include "msgqueue.h"


Control *msgqueue_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl5,*Ctl6,*Ctl12,*Ctl13,*Ctl0;
    RECT rt; TFILE *pHead;
    ReadFile("msgqueue.rc", (TFILE **)&pHead);

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
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,10, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl11 ->sData , "流程号  进程 程序名              队列ID     状态    消息数  占用(%)" );
    //strcpy( Ctl11 ->sData , "流程号  进程 程序名              队列ID     状态    消息数 " );
    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 队列管理>>" );
    strcpy( Ctl1  ->sData , "1.刷新" );
    strcpy( Ctl2  ->sData , "2.实时刷新" );
    strcpy( Ctl3  ->sData , "3.创建队列" );
    strcpy( Ctl4  ->sData , "4.删除队列" );
    strcpy( Ctl5  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "流程号过滤:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&msgqueueCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&msgqueueCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&msgqueueCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&msgqueueCtl4Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&msgqueueCtl5Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&msgqueueCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&msgqueueCtl13Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&msgqueue_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = '4' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl5,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl2,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl3,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl6,    NULL,    Ctl4,    Ctl1,   Ctl13,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,    Ctl1,    Ctl1,    Ctl5,    Ctl1,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,    NULL,    Ctl6,    NULL);
    FreeFileMem( pHead );

    msgqueue_entry((Control *)Ctl11);

    return ((Control *)Ctl11) ;
}

msgqueue::msgqueue()
{
	m_pForm = (Control *) msgqueue_init ();

}
msgqueue::~msgqueue()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int msgqueue::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

