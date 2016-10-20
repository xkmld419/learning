/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbtablesdata_f.c	1.0	2010/05/19	<AutoCreate>" */
#include "TOCIQuery.h"  
#include "Environment.h"
#include "abmtablesdata.h"
#include "abmtablescatalog.h"
#include "Log.h"
#include "abmmainmenu.h"
Control *hbtablesdata_handle;

char hb_DataList[MaxNum][200] ;
char *hb_DataTmp[MaxNum] ;

//将备注中的换行符 制表符 换成 空格
char *hbReplace(char *sStr)
{
	if ( sStr == NULL )
	    return NULL;
  for(int i=0;i < strlen(sStr);i++){
  	if(sStr[i]=='\n')
  			sStr[i]=' ';
  }


	return sStr;
}

int getTableDataList()
{
	/*char sql[512],sSql[512];
	char sEditdata[100]={0};
	char FieldName[100] ={0} ;
	int n=0 ;	
  char *p ;
  char hbTmpName[50] ;
  memset(hbTmpName ,0 ,50) ;
  Control * hbCataList ;
  Control *eHandle , *lHandle;
	eHandle = GetCtlByID (hbtablesdata_handle, 13);
	strcpy(sEditdata,eHandle->sData);
	
	if(!sEditdata[0]){
		prtmsg(hbtablescatalog_handle ,7,5,"请在编辑框输入要查询的表名");
		return 0 ;
	}
	
	lHandle = GetCtlByID (hbtablesdata_handle, 3);	
  hbCataList = GetCtlByID (hbtablescatalog_handle, 3);
  
  try{  
	  TOCIDatabase * pConn = new TOCIDatabase();
	 	char sUser[32]={0};
		char sPwd[32]={0};
		char sStr[32]={0};
		char FieldBuff[1024] ={0};
		char tField[100] ={0};
		Environment::getConnectInfo("BILL", sUser, sPwd, sStr);
		if( ! pConn->connect(sUser, sPwd, sStr) ){
		    Log::log(0, "创建数据库连接失败！");
		    delete pConn;
		    THROW (1); 
		}
		TOCIQuery qQry(pConn);
	
		memset(sSql,0,512) ;
		sprintf(sSql,"select column_name,comments  from all_col_comments where table_name in( '%s') \
				            and (owner = 'COMM' or owner = 'BILL' or owner = 'ACCT') order by table_name ",sEditdata) ;
		qQry.setSQL(sSql) ;
		qQry.open() ;
		int j;
		while(qQry.next ())
		{ 
				memset(FieldName,0,sizeof(FieldName)) ;
				sprintf(FieldName,"%s",qQry.field(0).asString()) ;
				sprintf(hb_DataList[n],"%s" ,FieldName);
				hb_DataTmp[n]=hb_DataList[n];
				n++;
		}
		hb_DataTmp[n]=hb_DataList[n] ;
		n++ ;

	
 }
	catch(TOCIException &e)
	{
		//prtmsg(hbtablescatalog_handle ,5,5,sSql) ;
		prtmsg(hbtablescatalog_handle ,7,5,e.getErrMsg()) ;
		//prtmsg(hbtablescatalog_handle ,10,5,FieldName) ;
		//printf("执行sql语句失败：%s\n",sql) ;
		}
	return n ; */
	return 0;
}

int getTableDataListB()
{
	char sql[512],sSql[512];
	char sEditdata[100]={0};
	char FieldName[100] ={0};
	char CommentName[100] ={0};
	char sName[200] ={0};
	int n=0;	
  char *p;
  char hbTmpName[50]={0};
  memset(hbTmpName ,0 ,50);
  
 try{ 
	  Control *hbCataList ;
	  Control *eHandle , *lHandle;
		eHandle = GetCtlByID (hbtablesdata_handle, 13);
		lHandle = GetCtlByID (hbtablesdata_handle, 3);	
	  hbCataList = GetCtlByID (hbtablescatalog_handle, 3) ;
  
	  for(int i = 0 ; i<hbCataList->iAdditional ;i++)  {
	  	
			if(hbCataList->sData[i] == SELECTED ) {
				  p = strchr(hbCataList->pData[i],'.') ;		
			 }else continue;
		    
			TOCIQuery qry(Environment::getDBConn());
			sprintf(sql,"select hb_catalog_name from hb_bill_tables_catalog where hb_catalog_describe = '%s'",p+1) ;
			qry.setSQL(sql);
			qry.open();
			if(qry.next()){
				sprintf(hbTmpName,"%s",qry.field(0).asString()) ;
			}else{
				char comments[100] = {0};
				sprintf(comments,"异常：[%s]没找到",p+1);
				prtmsg(hbtablescatalog_handle ,7,5,comments) ;
				return 0;	
			}
			qry.close() ;
			hbtrim(hbTmpName);
			
			for(int j = 0 ;j<strlen(hbTmpName) ;j++)  {
				if(hbTmpName[j]>='a'&&hbTmpName[j]<='z')
					hbTmpName[j]=hbTmpName[j]-32 ;
		  }
	  
      TOCIDatabase * pConn = new TOCIDatabase();
		 	char sUser[32]={0};
			char sPwd[32]={0};
			char sStr[32]={0};
			char FieldBuff[1024] ={0};
			char tField[100] ={0};
			Environment::getConnectInfo("BILL", sUser, sPwd, sStr);
			if( ! pConn->connect(sUser, sPwd, sStr) ){
			    Log::log(0, "创建数据库连接失败！");
			    delete pConn;
			    THROW (1); 
			}
			TOCIQuery qQry(pConn);
			strcat(eHandle->sData,hbTmpName);
			strcat(eHandle->sData," ");
			sprintf(hb_DataList[n],"表[%s]的结构[COLUMN_NAME]",hbTmpName) ;
			hb_DataTmp[n]=hb_DataList[n];
			n++;
			memset(hb_DataList[n],0,200);
			sprintf(hb_DataList[n],"%s"
			          ,"------------------------------");
			hb_DataTmp[n]=hb_DataList[n];
			n++;
			
			sprintf(sSql,"select column_name,comments  from all_col_comments where table_name = '%s' \
				            and (owner = 'HSS' or owner = 'BILL' or owner = 'COMM' ) ",hbTmpName) ;
			qQry.setSQL(sSql) ;
			qQry.open() ;
			while(qQry.next()) {
				memset(sName,0,sizeof(sName)) ;
				memset(FieldName,0,sizeof(FieldName)) ;
				memset(CommentName,0,sizeof(CommentName)) ;
				sprintf(FieldName,"%s",qQry.field(0).asString()) ;
				sprintf(hb_DataList[n],"%s" ,FieldName);
				hb_DataTmp[n]=hb_DataList[n];
				n++;
			
			}
			memset(hb_DataList[n],0,200) ;
			snprintf(hb_DataList[n],5,"%s","\n") ;
			hb_DataTmp[n]=hb_DataList[n];
		  n++;
			qQry.close() ;
		}		
  }
	
	catch(TOCIException &e){
		prtmsg(hbtablescatalog_handle ,7,5,e.getErrMsg()) ;
	}	
	catch(...) {
    	prtmsg(hbtablescatalog_handle ,7,5,"异常错误") ;
  }
	return n ;
}
int hbtablesdata_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hbtablesdata_entry(Control *pHandle) /* hbtablesdata 创建时触发调用 */
{   
    hbtablesdata_handle = pHandle;
    /* Add your self code here: */
        Control *pDatalist;
	
			 pDatalist = GetCtlByID (hbtablesdata_handle, 3);
			int num = 0 ;
			//flag = 1;
			num = getTableDataListB() ;
		
			if(num > 0)  {
				pDatalist->iAdditional = num;
				pDatalist->pData = hb_DataTmp ;
				CtlShow (pDatalist);
			 //CtlAtv (pCtl, pCtl); }
			}
		


}

int hbtablesdataCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 数据的按键消息处理 */
    /* Add your self code here: */

    return ch;
}

int hbtablesdataCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 查看button的按键消息处理 */
    /* Add your self code here: */
    Control *pDatalist;
		if(ch =='\r')    {
			 pDatalist = GetCtlByID (hbtablesdata_handle, 3);
			int num = 0 ;
			//flag = 1;
			num = getTableDataList() ;
		
			if(num > 0)  {
				pDatalist->iAdditional = num;
				pDatalist->pData = hb_DataTmp ;
				CtlShow (pDatalist);
			 CtlAtv (pCtl, pCtl); }
			}

    return ch;
}

int hbtablesdataCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
     if (ch == '\r') {
    hbtablescatalog tc;
	  tc.run() ;
		return FORM_KILL;
	 }

    return ch;
}

int hbtablesdataCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}


