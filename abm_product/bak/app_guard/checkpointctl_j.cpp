/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)checkpointctl_j.c	1.0	2010/09/30	<AutoCreate>" */
#include "checkpointctl.h"


Control *checkpointctl_init()
{
    Control *Ctl11,*Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl6,*Ctl12,*Ctl13;
    RECT rt; TFILE *pHead;
    ReadFile("checkpointctl.rc", (TFILE **)&pHead);

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
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl11 ->sData , "" );
    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> checkpoint信息>>" );
    strcpy( Ctl1  ->sData , "1.查看" );
    strcpy( Ctl2  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "时间过滤:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&checkpointctlCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&checkpointctlCtl2Press;    
    Ctl6  ->pKeyPress = (KEYPRESS)&checkpointctlCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&checkpointctlCtl13Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&checkpointctl_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&checkpointctl_SysHotKeyPress;    
    Ctl6  ->pHotKeyPress = (KEYPRESS)&checkpointctl_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&checkpointctl_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = 'q' ;
  

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl2,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl1,    Ctl13,   Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,    Ctl1,    Ctl1,    Ctl2,    Ctl1,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl6,    NULL,    Ctl6,    Ctl6,    NULL);

    FreeFileMem( pHead );

    checkpointctl_entry((Control *)Ctl11);

    return ((Control *)Ctl11) ;
}

checkpointctl::checkpointctl()
{
	m_pForm = (Control *) checkpointctl_init ();

}
checkpointctl::~checkpointctl()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int checkpointctl::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

