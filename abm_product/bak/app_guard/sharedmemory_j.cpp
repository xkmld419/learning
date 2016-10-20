/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sharedmemory_j.c	1.0	2007/04/13	<AutoCreate>" */
#include "sharedmemory.h"


Control *sharedmemory_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl5,*Ctl6;
    RECT rt; TFILE *pHead;
    ReadFile("sharedmemory.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl11  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
														 
    strcpy( Ctl11 ->sData , "共享内存ID 名称                     状态         共有/已用" );
    strcpy( Ctl7  ->sData , "当前位置: 计费监控>> 共享内存>>" );
    strcpy( Ctl1  ->sData , "1.刷新" );
    strcpy( Ctl5  ->sData , "q.退出" );

    Ctl1  ->pKeyPress = (KEYPRESS)&sharedmemoryCtl1Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&sharedmemoryCtl5Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&sharedmemoryCtl6Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&sharedmemory_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&sharedmemory_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&sharedmemory_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl5,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl6,    NULL,    Ctl1,    Ctl1,    Ctl6,    Ctl6);
    CtlSetDir(Ctl6    ,    NULL,    Ctl1,    Ctl1,    Ctl5,    Ctl1,    NULL);

    FreeFileMem( pHead );

    sharedmemory_entry((Control *)Ctl11);

    return ((Control *)Ctl11) ;
}

sharedmemory::sharedmemory()
{
	m_pForm = (Control *) sharedmemory_init ();

}
sharedmemory::~sharedmemory()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int sharedmemory::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

