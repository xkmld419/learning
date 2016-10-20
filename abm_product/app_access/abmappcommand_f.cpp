/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbappcommand_f.c	1.0	2010/05/11	<AutoCreate>" */
#include "TOCIQuery.h"  
#include "Environment.h"
#include "Log.h"
#include "abmaccessLog.h"
#include "abmlogin.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#include "abmappcommand.h"
#include "abmmainmenu.h"
//增加登陆用户组鉴权
#include "GroupDroitMgr.h"

#define MAX_TABLE_NUM 500


Control *hbappcommand_handle;
char list_comm[MAX_TABLE_NUM][512] ;
char *tmp_comm[MAX_TABLE_NUM] ;

int checkCommand(char *m_Comm) 
{

	try{
	char t_Comm[500] ={0} ;
	char sSql[500] = {0} ;
	strcpy(t_Comm,m_Comm) ;
	int fg = -1;
	char * p =0 ;
	p = strchr(t_Comm,' ') ;
	if(p) p =strtok(t_Comm," ");
	else p=t_Comm;
	
	TOCIDatabase * pConn = new TOCIDatabase();
 	char sUser[32]={0};
	char sPwd[32]={0};
	char sStr[32]={0};
 
	Environment::getConnectInfo("BILL", sUser, sPwd, sStr);
	if( ! pConn->connect(sUser, sPwd, sStr) ){
	    Log::log(0, "创建数据库连接失败！");
	    delete pConn;
	    THROW (1); 
	}
	TOCIQuery qQry(pConn);
	sprintf(sSql,"select nvl(flags,0) from b_command_cfg where comm_name = '%s' ",p ) ;
	qQry.setSQL(sSql);
	qQry.open() ;
	if(qQry.next()){
		fg=qQry.field(0).asInteger() ;
	}
	qQry.close() ;
	if(fg >0) 
		return 1 ;
	else
		return -1 ;
	}
	catch(TOCIException &e)
	{
		return -1;
	}
}
	
int getAppCommand() 
{
	//try{
	int n =0 ,m=0;
	Control *lHandle,*tHandle ;
	tHandle = GetCtlByID (hbappcommand_handle, 13);
	lHandle = GetCtlByID (hbappcommand_handle, 3);
	char sEdittmp_comm[100] = {0} ;
	char buffer[512] ;
	char tBuf[512] ={0} ;
	memset(buffer,0,512) ;
	char *p = 0 ,*tp = 0;
	FILE *fp ;

	strcpy(sEdittmp_comm ,tHandle->sData) ;
	if(!sEdittmp_comm[0]) 
		return 0 ;
	
	/*
	GroupDroitMgr *MDroitMgr = new GroupDroitMgr();
	strcpy(sFullStaffID,MDroitMgr->getUsrName());
	UserFlag = MDroitMgr->IsRootUsr();
    
	hbDBLog(sFullStaffID,5,"COMMAND",sEdittmp_comm,"执行命令");
	hbFileLog(sFullStaffID,5,"COMMAND",sEdittmp_comm,"执行命令");
	*/
	UserFlag=true;
	/*        
	if(checkCommand(sEdittmp_comm)<= 0) {
		sprintf(list_comm[0],"%s命令不存在或者没有权限执行\n",sEdittmp_comm) ;
		tmp_comm[0] = list_comm[0] ;
		return 1 ; 
	}
	*/
	if(!UserFlag){
		 char sCMD[500]={0};
		 char *px;
		 strcpy(sCMD,sEdittmp_comm);
		 px = strchr(sCMD,' ') ;
		 if(px) px =strtok(sCMD," ");
		 else px=sCMD;
		 if(strcmp(px,"setbillingcycle") == 0){
		 	   px = strtok(NULL," ");
		 	   if(strcmp(px,"-u") ==0){
		 	   		sprintf(list_comm[0],"对不起该用户没有修改ABM的权限") ;
						tmp_comm[0] = list_comm[0] ;
						return 1 ; 
		 	   }
		 }
	}	
	
 try {    
	if((fp = popen(sEdittmp_comm ,"r"))!=NULL){    
      memset(buffer,0,512);
	  while(fgets(buffer,512,fp))
	  {  
	  	hbtrim(buffer);
	  	if(strlen(buffer) == 0)
	  		  continue ;
	  	tp = buffer ;
	  	int s=0;
	  	while(*tp !='\0')  {
			  		  		
			  	memset(list_comm[n],0,512) ;
			  	//strncpy(list_comm[n],tp,lHandle->iWidth-6) ;
			  	snprintf(list_comm[n],lHandle->iWidth-6,"%s%d%s%s","[",n,"]",tp);
			  	m = lHandle->iWidth-6 ;
			  	list_comm[n][m-1]='\0' ;
			  	tmp_comm[n] = list_comm[n] ;
			  	s=s+lHandle->iWidth-6;
			  	n++ ;
			    tp = &buffer[s] ;  
		  }
		}
		 /*if(*tp=='\0')
		 	continue ;
		  memset(list_comm[n],0,512) ;
		  strcpy(list_comm[n],tp) ;
		  tmp_comm[n] = list_comm[n] ;
		  n++ ;*/
	  pclose(fp);
 	}
 	else  {
 		//sprintf(list_comm[n],"%s","命令执行错误！") ;
 		sprintf(buffer,"error: %s", strerror(errno));
 		strcpy(list_comm[n],buffer) ;
 		tmp_comm[n] = list_comm[n] ;
 		n++;
 	}
 }
 catch (...) {
    prtmsg(hbappcommand_handle,5,5,"异常") ;
  }
 	 	//prtmsg(hbappcommand_handle,5,5,buffer) ;
    return	n ; 	
}

int ReadUsageFile()
{/*
	FILE *fd ;
	int n =0,m=0 ;
	char buffer[512] ;
	char pathname[512]={0} ;

	if((fd = fopen("abmappcommandusage.txt","r"))==NULL)  {
		printf("open hbappcommandusage.txt fail\n") ;
		return 0 ;
	}
	while(fgets(buffer,512,fd)){
		  memset(list_comm[n],0,512) ;
	  	strcpy(list_comm[n],buffer) ;
	  	m = strlen(buffer) ;
	  	list_comm[n][m-1]='\0' ;
	  	tmp_comm[n] = list_comm[n] ;
	  	n++ ;
		}
		fclose(fd) ;
		return n ;*/
		//char sSql[1024]={0};
	try{
		TOCIDatabase * pConn = new TOCIDatabase();
	 	char sUser[32]={0};
		char sPwd[32]={0};
		char sStr[32]={0};
		int n =0,m=0;
		Environment::getConnectInfo("BILL", sUser, sPwd, sStr);
		if( ! pConn->connect(sUser, sPwd, sStr) ){
		    Log::log(0, "创建数据库连接失败！");
		    delete pConn;
		    THROW (1); 
		}
		TOCIQuery qQry(pConn);
		//strcpy(sSql,"select remark from b_command_cfg order by remark" ) ;
	  qQry.setSQL("select remark,comm_name from b_command_cfg order by remark");
		qQry.open() ;
		while(qQry.next()){
			memset(list_comm[n],0,512) ;
	  	strcpy(list_comm[n],qQry.field(0).asString());
	  	tmp_comm[n] = list_comm[n] ;
	  	n++ ;
		}
		qQry.close() ;
		return n;
	}
	catch(TOCIException &e)
	{
		return 0;
	}
}
	
	

int hbappcommand_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hbappcommand_entry(Control *pHandle) /* hbappcommand 创建时触发调用 */
{   
    
    hbappcommand_handle = pHandle;
    return;
    /* Add your self code here: */
	  Control *pProlist ;
	  int num ;
		pProlist = GetCtlByID (hbappcommand_handle, 3);
		num = ReadUsageFile() ;  //从表里读取相关的工具列表
		pProlist->iAdditional = num;
		pProlist->pData = tmp_comm ;
}

int hbappcommandCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 应用命令的按键消息处理 */
    /* Add your self code here: */
    

    return ch;
}

int hbappcommandCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 执行button的按键消息处理 */
    /* Add your self code here: */
    Control *pProlist_comm =new Control ;
    Control *pTProlist_comm =new Control;

    char *chTmp[1];
    char chTmpa[1][512];
    char sEdittmp[100] ={0};
    if (ch == '\r') {
          pTProlist_comm=GetCtlByID (hbappcommand_handle, 13);
          strcpy(sEdittmp ,pTProlist_comm->sData) ;
          if(strcmp(sEdittmp,"")==0){
              sprintf(chTmpa[0],"Please enter the correct or complete command!");
              chTmp[0]=	chTmpa[0];
              pProlist_comm = GetCtlByID (hbappcommand_handle, 3);	
	          pProlist_comm->iAdditional = 1;
	          pProlist_comm->pData=chTmp;
	          CtlShow (pProlist_comm);
	          CtlAtv (pCtl, pCtl);
          } else { 
		     pProlist_comm = GetCtlByID (hbappcommand_handle, 3); 
			 int num = 0 ;
			//flag = 1;
			 num = getAppCommand() ;
			
		  //wrefresh(pProlist_comm->pWin);
			 if(num > 0)  {
				pProlist_comm->iAdditional = num;
				pProlist_comm->pData = tmp_comm ;
				CtlShow (pProlist_comm);
			    CtlAtv (pCtl, pCtl); 
			 } else {
			  sprintf(chTmpa[0],"Please enter the correct or complete command!");
              chTmp[0]=	chTmpa[0];
				//num = ReadUsageFile() ;
				//pProlist_comm->iAdditional = num;
				//pProlist_comm->pData = tmp_comm ;
				pProlist_comm->iAdditional = 1;
				pProlist_comm->pData = chTmp ;
				CtlShow (pProlist_comm);
			    CtlAtv (pCtl, pCtl); 
			 }
			//touchwin(pProlist_comm->pWin) ;
			//refresh();
	    //wrefresh(pProlist_comm->pWin);
	    //delwin(pProlist_comm->pWin);
			}
	    }			
    return ch;
}

int hbappcommandCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
	  hbmainmenu mm;
	  mm.run() ;
		return FORM_KILL;
	}


    return ch;
}

int hbappcommandCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}


