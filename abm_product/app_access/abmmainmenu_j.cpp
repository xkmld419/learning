/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbmainmenu_j.c	1.0	2010/05/06	<AutoCreate>" */
#include "abmmainmenu.h"

//增加登陆用户组鉴权
#include "GroupDroitMgr.h"


Control *hbmainmenu_init()
{
    Control *Ctl9,*Ctl12,*Ctl1,*Ctl2,*Ctl3,*Ctl4,*Ctl5;
    RECT rt; TFILE *pHead;
    ReadFile("abmmainmenu.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl3   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    //GetSiteInfo( pHead, 5, &rt);
    //Ctl4   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl5   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);

    strcpy( Ctl12 ->sData , "abmaccess菜单" );
    strcpy( Ctl1  ->sData , "1.ABM系统表名浏览" );
    strcpy( Ctl2  ->sData , "2.SQL功能" );
    strcpy( Ctl3  ->sData , "3.应用命令" );
   // strcpy( Ctl4  ->sData , "4.清理过期文件" );
    //strcpy( Ctl4  ->sData , "" );
    strcpy( Ctl5  ->sData , "q.退出" );

    Ctl1  ->pKeyPress = (KEYPRESS)&hbmainmenuCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&hbmainmenuCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&hbmainmenuCtl3Press;
    //Ctl4  ->pKeyPress = (KEYPRESS)&hbmainmenuCtl4Press;
    Ctl5  ->pKeyPress = (KEYPRESS)&hbmainmenuCtl5Press;

    Ctl1  ->pHotKeyPress = (KEYPRESS)&hbmainmenu_SysHotKeyPress;
    Ctl2  ->pHotKeyPress = (KEYPRESS)&hbmainmenu_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&hbmainmenu_SysHotKeyPress;
    //Ctl4  ->pHotKeyPress = (KEYPRESS)&hbmainmenu_SysHotKeyPress;
    Ctl5  ->pHotKeyPress = (KEYPRESS)&hbmainmenu_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = '2' ;
    Ctl3  ->iHotKey = '3' ;
    //Ctl4  ->iHotKey = '4' ;
    Ctl5  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;

    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl1);
    CtlSetDir(Ctl1    ,    Ctl5,    Ctl2,    NULL,    NULL,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl1,    Ctl3,    NULL,    NULL,    Ctl3,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl2,    Ctl5,    NULL,    NULL,    Ctl5,    Ctl5);
    //CtlSetDir(Ctl3    ,    Ctl2,    Ctl5,    NULL,    NULL,    Ctl4,    Ctl4);
   // CtlSetDir(Ctl4    ,    Ctl3,    Ctl5,    NULL,    NULL,    Ctl5,    Ctl5);
    //CtlSetDir(Ctl5    ,    Ctl3,    Ctl1,    NULL,    NULL,    Ctl1,    NULL);
    CtlSetDir(Ctl5    ,    Ctl3,    Ctl1,    NULL,    NULL,    Ctl1,    NULL);

    FreeFileMem( pHead );

    hbmainmenu_entry((Control *)Ctl9);

    return ((Control *)Ctl9) ;
}

hbmainmenu::hbmainmenu()
{
	m_pForm = (Control *) hbmainmenu_init ();

}
hbmainmenu::~hbmainmenu()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hbmainmenu::run()
{
	//GroupDroitMgr *MDroitMgr = new GroupDroitMgr();
	//strcpy(sFullStaffID,MDroitMgr->getUsrName());
	//UserFlag = MDroitMgr->IsRootUsr();
	UserFlag = true;
	
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

