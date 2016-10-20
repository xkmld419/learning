/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablescatalog_f.c	1.0	2010/05/19	<AutoCreate>" */
#include "TOCIQuery.h"  
#include "Environment.h"
#include "abmtablescatalog.h"
#include "abmtablesmenu.h"
#include "abmtablesdata.h"
#define MaxLine 512

char hb_CataList[MaxLine][100] ;
char *hb_TmpList[MaxLine] ;
Control *hbtablescatalog_handle;

int getTalbesCatalog()
{
	char sql[512];
	int n=0 ,flag =1 ;	
  char *p ;
  char hbTmp[50] ,sEdittmp[100];
  memset(hbTmp ,0 ,50) ;
  memset(sEdittmp ,0 ,100) ;
  Control * hbList , *tHandle;
  hbList = GetCtlByID (hbtablesmenu_handle, 3) ;
  for(int i = 0 ; i<hbList->iAdditional ;i++)  {
	if(hbList->sData[i] == SELECTED )  {
		p = strtok(hbList->pData[i],".") ;
		break ;
		flag = 0 ;
	 }
  }
  tHandle = GetCtlByID (hbtablescatalog_handle, 13);
  strcpy(sEdittmp ,tHandle->sData) ;
  if(strlen(sEdittmp))  {
  	for(int i = 0 ;i<strlen(sEdittmp);i++){
  		if(sEdittmp[i]>='a' &&sEdittmp[i]<='z')
  			sEdittmp[i] = sEdittmp[i] - 32 ;
  		}
  	
  	sprintf(sql,"select hb_catalog_describe from hb_bill_tables_catalog where \
  	            hb_sum_id = %s and hb_catalog_name like '%%",p,sEdittmp) ;
  	strcat(sEdittmp,"%'") ;
  	strcat(sql,sEdittmp) ;
  	//prtmsg(hbtablescatalog_handle ,8,5,sql) ;
  }
  else 
  	sprintf(sql,"select hb_catalog_describe from hb_bill_tables_catalog where hb_sum_id = %s",p) ;
  if(!flag)    {
  	return 0 ;
  }
  try{
	TOCIQuery qry(Environment::getDBConn());
	
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())   {
		memset(hb_CataList[n],0,100) ;
		sprintf(hb_CataList[n],"%d.%s",(n+1),qry.field(0).asString()) ;
		hb_TmpList[n] = hb_CataList[n] ;
		n++;
		//prtmsg(hbtablescatalog_handle ,n,5,hb_TmpList[n]) ;
	}
	qry.close() ;
 }
	catch(...)
	{
		prtmsg(hbtablescatalog_handle ,5,5,sql) ;
		printf("执行sql语句失败：%s\n",sql) ;
		}
	return n ;
}

int hbtablescatalog_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hbtablescatalog_entry(Control *pHandle) /* hbtablescatalog 创建时触发调用 */
{   
    hbtablescatalog_handle = pHandle;
    /* Add your self code here: */ 
    Control *hbCataList ;
    int num ;
    hbCataList = GetCtlByID (hbtablescatalog_handle, 3) ;
    num = getTalbesCatalog() ;
    if(num >0 ) {
       hbCataList->iAdditional = num;
	     hbCataList->pData = hb_TmpList ;  
	  }
}

int hbtablescatalogCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 执行button的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    	Control *hbCataList ;
    	int flag = 1 ;
    	hbCataList = GetCtlByID (hbtablescatalog_handle, 3) ;
		  for(int i = 0 ; i<hbCataList->iAdditional ;i++)  {
			if(hbCataList->sData[i] == SELECTED )  {
			  flag =0 ;
				break ;
			 }
		  }
		  if(!flag)  {
			  	hbtablesdata td ;
			  	td.run() ;
			  	return FORM_KILL ;
			  }
		  else   
		  	prtmsg(pCtl,20,53,"请用空格键选择要查看的表") ;
    }
    return ch;
}

int hbtablescatalogCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    hbtablesmenu tm;
	  tm.run() ;
		return FORM_KILL;
	 }

    return ch;
}

int hbtablescatalogCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}

int hbtablescatalogCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */
    if(ch == '\r')  {
    	Control *hbCataList ;
      int num ;
      hbCataList = GetCtlByID (hbtablescatalog_handle, 3) ;
      num = getTalbesCatalog() ;
      //if(num >0 ) {
        hbCataList->iAdditional = num;
	      hbCataList->pData = hb_TmpList ;  
	   // }
	    CtlShow (hbCataList);
			  CtlAtv (pCtl, pCtl); 
	    //prtmsg(pCtl,20,3,"???") ;
    }
    return ch;
}


