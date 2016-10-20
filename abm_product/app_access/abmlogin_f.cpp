/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hblogin_f.c	1.0	2010/05/06	<AutoCreate>" */
#include "abmlogin.h"
#include "ProcessInfo.h"
#include "abmmainmenu.h"
#include "password.h"
#include "encode.h"
#include "ReadIni.h"
#include "TOCIQuery.h"
#include "Environment.h"
//#include "UsrInfoCheckMgr.h"
#include "checkUserInfo.h"

Control *hblogin_handle;

THeadInfo *pInfoHead = NULL;
TProcessInfo *pProcInfoHead = NULL;


void hbaccessLog(int type,char *result,char *cmd,char *res)
{
	  char sCmd[100];
	  strcpy(sCmd,cmd);
	  
	  if ( ! cmd ) return;
	  	
	  char chTmp[10] = {0};
	  char *p;
	  int i = 0;
	  
	  p = strtok(cmd," ");  
	  
	  if (!p ) return ;
	  
	  while(*p) {
	      *p |= 0x20 ;
	      chTmp[i] = *p ;
	      i++ ;
	      p++;
	  }  
	    
	  if(strcmp(chTmp,"select") == 0) {
	  	
	  	  return ;
	  }else  if(strcmp(chTmp,"insert") == 0) {  
	  	
	  	  p  = strtok(NULL," ");
	  	  
	  	  if (!p ) return ;
	  	  	
	  	  p  = strtok(NULL," ");  
	  	  if (!p ) return ;
	  
	  }else  if(strcmp(chTmp,"update") == 0) {   
	  	
	  	 p  = strtok(NULL," "); 
	  	 if (!p ) return ;
	  
	  }else if(strcmp(chTmp,"delete") == 0) { 
	  	
	  	 p  = strtok(NULL," "); 
	  	 
	  	 if (!p ) return ;
	  	 	
	  	 p  = strtok(NULL," ");   
	  	 
	  	 if (!p ) return ;
	  
	  }else {
	  	 return ;
	  }
	  
	 if(p[strlen(p)-1] == ';') p[strlen(p)-1]='\0';
   char sql[2000];
   memset(sql,0,2000);
   DEFINE_QUERY (qry);
   try{
	   	 strcpy(sql,"insert into hbaccess_operate_log(staff_id,operate_type,\
	                       operate_result,operate_cmd,operate_date,reason,table_name)\
						    values(:staff_id,:operate_type,:operate_result,\
						            :operate_cmd,sysdate,:reason,:table_name)"
	            );
        qry.setSQL(sql);
        qry.setParameter("staff_id",sFullStaffID);
        qry.setParameter("operate_type",type);
        qry.setParameter("operate_result",result);
        qry.setParameter("operate_cmd",sCmd);
        qry.setParameter("reason",res);
        qry.setParameter("table_name",p);
        qry.execute();
        qry.commit();
        qry.close();
   }
    //# OCI Exception
    catch (TOCIException &e) {
        cout<<"[EXCEPTION]: TOCIException catched"<<endl;
        cout<<"  *Err: "<<e.getErrMsg()<<endl;
        cout<<"  *SQL: "<<e.getErrSrc()<<endl;
    }
    //# Exception
    catch (Exception &e) {
        cout << "收到Exception退出]"  << endl;
        cout <<"[EXCEPTION]:: " << e.descript () << endl;
    }

    //# Other Exception
    catch (...) {
        cout << "[EXCEPTION]: Exception catched" <<endl;
    }
	
}

int GetOriPassword (unsigned char sStr[])
{
	FILE *fp;
	unsigned char sBuff[256];
	int i, iNum;
	
	fp = fopen ("xqdata", "rb");
	if (fp == NULL) {
		sStr[0] = 0;
		return 0;
	}

	memset (sBuff, 0, 256);
	fread (sBuff, 256, 1, fp);
	iNum = strlen ((char *)sBuff);
	for (i=0; i<iNum; i++) {
		sStr[i] = ~sBuff[i];
	}
	sStr[i] = 0;
	fclose (fp);

	return 0;
}

int SetPassword (unsigned char sStr[])
{
	FILE *fp;
	int i, iLen;
	char sBuff[256];

	iLen = strlen ((char *)sStr);
	fp = fopen ("xqdata", "wb");
	if (fp == NULL) {
		perror ("SetPassword");
		return -1;
	}
	
	for (i=0; i<iLen; i++) {
		sBuff[i] = ~sStr[i];
	}
	sBuff[i] = 0;
	fwrite (sBuff, strlen ((char *)sBuff), 1, fp);

	fclose (fp);
	return 0;
}


/////////////////////////////////////////

int GetOriPassword (unsigned char sStr[]);
int SetPassword (unsigned char sStr[]);

int hblogin_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hblogin_entry(Control *pHandle) /* hblogin 创建时触发调用 */
{   
    hblogin_handle = pHandle;
    /* Add your self code here: */


}

int hbloginCtl7Press(Control *pCtl,int ch,long lParam)
{   /* 密码输入框的按键消息处理 */
    /* Add your self code here: */


    return ch;
}

int hbloginCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 工号输入框的按键消息处理 */
    /* Add your staff ID here: */


    return ch;
}

int hbloginCtl8Press(Control *pCtl,int ch,long lParam)
{   /* 确认按钮的按键消息处理 */
    /* Add your self code here: */
	  unsigned char sCurStaffID[50], sCurPassword[50];

    if (ch != '\r') return ch;
    	
    strcpy ((char *)sCurPassword, (char *) GetCtlByID (hblogin_handle, 7)->sData);
    strcpy ((char *)sCurStaffID, (char *) GetCtlByID (hblogin_handle, 13)->sData);
    strcpy(ssFullStaffID,(char *)sCurStaffID);

		//UsrInfoCheckMgr usrInfoCheck;
		if(checkPwd((char *)sCurPassword)){
		//if(CheckUserInfo::checkUerInfo((char *)sCurStaffID,(char *)sCurPassword) ){
			
			hbmainmenu mm;
			return mm.run();
	   	
		}else{

     prtmsg (pCtl, pCtl->iTop+2, pCtl->iLeft + 2, "工号不存在或者密码错误!");
 
		}
		
		
    return ch;
}

int hbloginCtl9Press(Control *pCtl,int ch,long lParam)
{   /* 退出按钮的按键消息处理 */
    /* Add your self code here: */
	if (ch == '\r') {
		return FORM_KILL;
	}

    return ch;
}

int hbloginCtl10Press(Control *pCtl,int ch,long lParam)
{   /* 改密码按钮的按键消息处理 */
    /* Add your self code here: */

    if (ch == '\r') {
	password pw;
	pw.run();
	FormShow (hblogin_handle);
	return ID (7);
    }

    return ch;
}


bool checkPwd(char * pwd)
{
	  char sOriPassword[50];
	  GetOriPassword ((unsigned char *)sOriPassword);

    if (strcmp (sOriPassword, pwd)==0 ) 
    	return true;
    	
    return false;
 }
