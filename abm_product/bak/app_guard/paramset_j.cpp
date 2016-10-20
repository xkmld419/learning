/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)paramset_j.c	1.0	2010/07/06	<AutoCreate>" */
#include "paramset.h"


Control *paramset_init()
{
    Control *Ctl7,*Ctl9,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl5,*Ctl6,*Ctl12,*Ctl13,*Ctl14,*Ctl15,*Ctl16,*Ctl17,*Ctl18,*Ctl19;
    RECT rt; TFILE *pHead;
    ReadFile("paramset.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl7   = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl6   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 8, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 9, &rt);
    Ctl13  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,10, &rt);
    Ctl14  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,11, &rt);
    Ctl15  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,12, &rt);
    Ctl16  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,13, &rt);
    Ctl17  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,14, &rt);
    Ctl18  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead,15, &rt);
    Ctl19  = CtlInit(EDIT    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl7  ->sData , "当前位置: HSS监控>> 参数管理>>" );
    strcpy( Ctl1  ->sData , "1.查看" );
    strcpy( Ctl2  ->sData , "2.新增" );
    strcpy( Ctl3  ->sData , "3.删除" );
    strcpy( Ctl4  ->sData , "4.提交" );
    strcpy( Ctl5  ->sData , "q.退出" );
    strcpy( Ctl12 ->sData , "段过滤:" );
    strcpy( Ctl14 ->sData , "参数过滤:" );
    strcpy( Ctl16 ->sData , "参数新值:" );
    strcpy( Ctl18 ->sData , "生效日期:" );

    Ctl1  ->pKeyPress = (KEYPRESS)&paramsetCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&paramsetCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&paramsetCtl3Press;
    Ctl4  ->pKeyPress = (KEYPRESS)&paramsetCtl4Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&paramsetCtl5Press;
    Ctl6  ->pKeyPress = (KEYPRESS)&paramsetCtl6Press;
    Ctl13 ->pKeyPress = (KEYPRESS)&paramsetCtl13Press;
    Ctl15 ->pKeyPress = (KEYPRESS)&paramsetCtl15Press;
    Ctl17 ->pKeyPress = (KEYPRESS)&paramsetCtl17Press;
    Ctl19 ->pKeyPress = (KEYPRESS)&paramsetCtl19Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl4  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl6  ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl13 ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl15 ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl17 ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;
    Ctl19 ->pHotKeyPress = (KEYPRESS)&paramset_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    Ctl4  ->iHotKey = '4' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl6  ->bFrame  = 1 ;

    Ctl6  ->bSingle = 0 ;

    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl9);
    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl6,    NULL,    Ctl5,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl6,    NULL,    Ctl1,    Ctl3,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl6,    NULL,    Ctl2,    Ctl4,    Ctl4,    Ctl4);
    CtlSetDir(Ctl4    ,    Ctl6,    NULL,    Ctl3,    Ctl5,    Ctl5,    Ctl5);
    CtlSetDir(Ctl5    ,    Ctl6,    NULL,    Ctl4,    Ctl1,   Ctl13,    Ctl6);
    CtlSetDir(Ctl6    ,   Ctl13,   Ctl17,    Ctl1,   Ctl17,   Ctl17,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,   Ctl15,    NULL,   Ctl15,    Ctl6,   Ctl14);
    CtlSetDir(Ctl14   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl15);
    CtlSetDir(Ctl15   ,    Ctl1,    Ctl6,   Ctl13,    NULL,    Ctl6,   Ctl16);
    CtlSetDir(Ctl16   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl17);
    CtlSetDir(Ctl17   ,    Ctl6,   Ctl19,    Ctl6,   Ctl19,   Ctl19,   Ctl18);
    CtlSetDir(Ctl18   ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl19);
    CtlSetDir(Ctl19   ,   Ctl17,    Ctl1,   Ctl17,    Ctl1,    Ctl1,    NULL);

    FreeFileMem( pHead );

    paramset_entry((Control *)Ctl7);

    return ((Control *)Ctl7) ;
}

paramset::paramset()
{
	m_pForm = (Control *) paramset_init ();

}
paramset::~paramset()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int paramset::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

