/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)process_j.c	1.0	2008/04/15	<AutoCreate>" */
#include "process.h"


Control *process_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl8,*Ctl5,*Ctl6,*Ctl14,*Ctl15;
    RECT rt; TFILE *pHead;
    ReadFile("process.rc", (TFILE **)&pHead);

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
    Ctl8   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,12, &rt);
    Ctl14  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,13, &rt);
    Ctl15  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl11 ->sData , "流程号  进程 程序名                时间         总量 状态 内存(M) cpu(%)" );
    strcpy( Ctl7  ->sData , "当前位置: 计费监控>> 进程管理>>" );
    strcpy( Ctl1  ->sData , "1.刷新" );
    strcpy( Ctl2  ->sData , "2.实时刷新" );
    strcpy( Ctl3  ->sData , "3.进程启动" );
    strcpy( Ctl4  ->sData , "4.安全退出" );
    strcpy( Ctl8  ->sData , "5.强制退出" );    
    strcpy( Ctl5  ->sData , "q.退出" );
    strcpy( Ctl14 ->sData , "APP_ID过滤:" );
    
    Ctl1  ->pKeyPress = (KEYPRESS)&processCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&processCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&processCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&processCtl4Press;
    Ctl8  ->pKeyPress = (KEYPRESS)&processCtl8Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&processCtl5Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&processCtl6Press;
    Ctl15 ->pKeyPress = (KEYPRESS)&processCtl15Press;
    Ctl1  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl8  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;    
    Ctl5  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl15 ->pHotKeyPress = (KEYPRESS)&process_SysHotKeyPress;
    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = '4' ;
    Ctl8  ->iHotKey = '5' ;
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
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl3,    Ctl8,    Ctl8,    Ctl8);
    CtlSetDir(Ctl8    ,    Ctl6,    NULL,    Ctl4,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl6,    NULL,    Ctl8,    Ctl1,   Ctl15,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl15,    Ctl1,    Ctl1,    Ctl5,    Ctl1,   Ctl14);
    CtlSetDir(Ctl14   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl15);
    CtlSetDir(Ctl15   ,    Ctl1,    Ctl6,    NULL,    NULL,    Ctl6,    NULL);
    FreeFileMem( pHead );
    process_entry((Control *)Ctl11);
    return ((Control *)Ctl11) ;
}

process::process()
{
	m_pForm = (Control *) process_init ();

}
process::~process()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int process::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

