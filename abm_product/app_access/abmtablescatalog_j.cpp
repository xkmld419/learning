/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablescatalog_j.c	1.0	2010/05/19	<AutoCreate>" */
#include "abmtablescatalog.h"
#include "abmtablesmenu.h"


Control *hbtablescatalog_init()
{
    Control *Ctl9,*Ctl12,*Ctl1,*Ctl2,*Ctl3,*Ctl7,*Ctl11,*Ctl13;
    RECT rt; TFILE *pHead;
    ReadFile("abmtablescatalog.rc", (TFILE **)&pHead);

    GetSiteInfo( pHead, 0, &rt);
    Ctl9   = CtlInit(BOX     ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 1, &rt);
    Ctl12  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 2, &rt);
    Ctl11  = CtlInit(LABEL   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 3, &rt);
    Ctl13  = CtlInit(EDIT   ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 4, &rt);
    Ctl1   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 5, &rt);
    Ctl2   = CtlInit(BUTTON  ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 6, &rt);
    Ctl3   = CtlInit(LIST    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    GetSiteInfo( pHead, 7, &rt);
    Ctl7   = CtlInit(LABEL    ,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);
    
    /*Control * hbList = hbtablesmenu_handle ;
    char *p ;
	  memset(hbTmp ,0 ,50) ;
    hbList = GetCtlByID (hbtablesmenu_handle, 3) ;
    for(int i = 0 ; i<hbList->iAdditional ;i++)  {
    	if(hbList->sData[i] == SELECTED )  {
    		p = strchr(hbList->pData[i],'.') ;
    		strcpy(hbTmp,p+1) ;
    		break ;
    	}
    }   
     
    strcat(hbTmp,"列表") ;  */
    
    strcpy( Ctl12 ->sData , "列 表" );
    strcpy( Ctl11 ->sData , "选择列表:" );
    strcpy( Ctl7 ->sData , "当前位置:abmaccess菜单>>HB系统表名浏览>>列表" );
    strcpy( Ctl1  ->sData , "1.查询" );
    strcpy( Ctl2  ->sData , "q.退出" );
    
    Ctl13 ->pKeyPress = (KEYPRESS)&hbtablescatalogCtl13Press;
    Ctl1  ->pKeyPress = (KEYPRESS)&hbtablescatalogCtl1Press;
    Ctl2  ->pKeyPress = (KEYPRESS)&hbtablescatalogCtl2Press;
    Ctl3  ->pKeyPress = (KEYPRESS)&hbtablescatalogCtl3Press;

    Ctl13 ->pHotKeyPress = (KEYPRESS)&hbtablescatalog_SysHotKeyPress;
    Ctl1  ->pHotKeyPress = (KEYPRESS)&hbtablescatalog_SysHotKeyPress;    
    Ctl2  ->pHotKeyPress = (KEYPRESS)&hbtablescatalog_SysHotKeyPress;
    Ctl3  ->pHotKeyPress = (KEYPRESS)&hbtablescatalog_SysHotKeyPress;

    Ctl1  ->iHotKey = '1' ;
    Ctl2  ->iHotKey = 'q' ;

    Ctl9  ->bFrame  = 1 ;
    Ctl3  ->bFrame  = 1 ;

    Ctl3  ->bSingle = 0 ;

    CtlSetDir(Ctl9    ,    NULL,    NULL,    NULL,    NULL,    NULL,   Ctl12);
    CtlSetDir(Ctl12   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl11);
    CtlSetDir(Ctl11   ,    NULL,    NULL,    NULL,    NULL,    NULL,    Ctl13);
    CtlSetDir(Ctl13   ,    Ctl1,    Ctl3,    NULL,    NULL,    NULL,    Ctl1);    
    CtlSetDir(Ctl1    ,    Ctl3,    Ctl13,    Ctl2,    Ctl2,    Ctl2,    Ctl2);
    CtlSetDir(Ctl2    ,    Ctl3,    Ctl13,    Ctl1,    Ctl1,    Ctl1,    Ctl3);
    CtlSetDir(Ctl3    ,    Ctl13,    Ctl1,    Ctl1,    Ctl2,    Ctl1,    Ctl7);
    CtlSetDir(Ctl7    ,    NULL,    NULL,    NULL,    NULL,    NULL,    NULL);

    FreeFileMem( pHead );

    hbtablescatalog_entry((Control *)Ctl9);

    return ((Control *)Ctl9) ;
}

hbtablescatalog::hbtablescatalog()
{
	m_pForm = (Control *) hbtablescatalog_init ();

}
hbtablescatalog::~hbtablescatalog()
{
	FormKill (m_pForm);
	m_pForm = NULL;
}

int hbtablescatalog::run()
{
	if (FormRun (m_pForm) == FORM_KILL_OK)
		m_pForm = NULL;

	return FORM_KILL_OK;
}

