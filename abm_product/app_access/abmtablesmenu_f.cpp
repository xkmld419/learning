/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablesmenu_f.c	1.0	2010/05/19	<AutoCreate>" */
#include "TOCIQuery.h"  
#include "Environment.h"
#include "abmtablesmenu.h"
#include "abmmainmenu.h"
#include "abmtablescatalog.h"

#include <signal.h>
#include <curses.h>
#include <unistd.h>
#include <sys/types.h>

#define MaxNum 512 
char hb_List[MaxNum][200] ;
char *hb_Tmp[MaxNum] ;
Control *hbtablesmenu_handle;


int getTalbesMenu()
{
	char sql[512];
	int n=0 ;
	printf("nihao ");
	TOCIQuery qry(Environment::getDBConn());
	printf("nihao3333 ");
	sprintf(sql,"select hb_id,hb_sum_name from hb_bill_tables_sum order by hb_id") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())   {
		sprintf(hb_List[n],"%d.%s",qry.field(0).asInteger(),qry.field(1).asString()) ;
		hb_Tmp[n] = hb_List[n] ;
		n++;
	}
	return n ;
}
	

int hbtablesmenu_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hbtablesmenu_entry(Control *pHandle) /* hbtablesmenu 创建时触发调用 */
{   
    hbtablesmenu_handle = pHandle;
    /* Add your self code here: */
    Control *pTableMenu ;
	  int num ;
		pTableMenu = GetCtlByID (hbtablesmenu_handle, 3);
		num = getTalbesMenu() ;
		pTableMenu->iAdditional = num;
		pTableMenu->pData = hb_Tmp ; 
}

int hbtablesmenuCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 执行button的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    Control *lHandle ;
    int flag = 1 ;
		lHandle = GetCtlByID (hbtablesmenu_handle, 3);
		for(int i = 0 ; i<lHandle->iAdditional ;i++)  {
			if(lHandle->sData[i] == SELECTED )  {
				flag = 0 ;
				break ;
			 }
	  }
	  if(!flag)  {
	  
    hbtablescatalog tc ;
    tc.run();    	
	  return FORM_KILL;   }
	  else  
	  	prtmsg(pCtl,20,53,"请用空格键选择要查看的内容") ;
	}
    return ch;
}

int hbtablesmenuCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    hbmainmenu mm;
	  mm.run() ;
		return FORM_KILL;
	}
    return ch;
}

int hbtablesmenuCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}


