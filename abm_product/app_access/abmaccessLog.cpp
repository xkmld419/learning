#include "abmaccessLog.h"
#include "ReadIni.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "abmmainmenu.h"
#ifdef erase
#undef erase
#endif
#ifdef clear
#undef clear
#endif
#ifdef move
#undef move
#endif

#include "Log.h"
#include "ThreeLogGroup.h"


using namespace std;
#define MBC_DB_OP_FAIL          146
#define  MBC_ACTION_MODIFY_DB                  100007

ThreeLogGroup * gpLogg =0 ;

//入参:工号,操作类型,操作结果,操作命令,原因说明
void hbDBLog(char *staffID,int type,char *result,char *sCmd,char *res)
{
#ifdef HBACCESS_DB_LOG
try{
	
	 if (type ==5 ){
	 	
	 	   char sql[2000];
		   memset(sql,0,2000);
		   DEFINE_QUERY (qry);
		   
		 	 strcpy(sql,"insert into hbaccess_operate_log(staff_id,operate_type,\
		                     operate_result,operate_cmd,operate_date,reason,table_name)\
						    values(:staff_id,:operate_type,:operate_result,\
						            :operate_cmd,sysdate,:reason,:table_name)"
		          );
		    qry.setSQL(sql);
		    qry.setParameter("staff_id",staffID);
		    qry.setParameter("operate_type",type);
		    qry.setParameter("operate_result",result);
		    qry.setParameter("operate_cmd",sCmd);
		    qry.setParameter("reason",res);
		    qry.setParameter("table_name","COMMAND");
		    qry.execute();
		    qry.commit();
		    qry.close();
		    
		    return;
	 	}
	 	
	 	
	  int cnt = 0;
	  char cmd[100];
	  strcpy(cmd,sCmd);
	  
	  if ( ! cmd ) return;
	  	
	  char chTmp[10] = {0};
	  char *p;
	  int i = 0;
	  
	  p = strtok(cmd," ");  
	  
	  if (!p ) return;
	  
	  while(*p) {
	  	 //if(*p == '(') break;
	      *p |= 0x20 ;	      
	      chTmp[i] = *p ;
	      i++ ;
	      p++;
	  }  
	    
	  if(strcmp(chTmp,"select") == 0) {	
	  	  char tmp[100] ={0} ;
				if((p = strstr(sCmd,"from")))  {
			    	p = p+4 ;
			  }else 	return ;
			   
			  strcpy(tmp,p);
			  hbtrim(tmp);
			  p = strchr(tmp,' ') ;
			  if(p)  {
			   	*p = '\0' ;
			  }
			  
			  p = tmp;	
	  }else  if(strcmp(chTmp,"insert") == 0) {  
	  	
	  	  p  = strtok(NULL," ");
	  	  
	  	  if (!p ) return ;
	  	  	
	  	  p  = strtok(NULL," "); 
	  	  int j=0;
	  	  while(j<strlen(p)) {
		  	  if(p[j] == '(') {
		  	 	   p[j] = '\0';
		  	 	   break;
		  	  }
		      j++;
		    } 
	  	  if(p[strlen(p)-1] == '(')  p[strlen(p)-1] == '\0';
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
   
   memset(sql,0,2000);
   strcpy(sql,"select count(*) from tab where tname = upper(:TNAME)");
   qry.setSQL(sql);
   qry.setParameter("TNAME",p);
   qry.open ();
   if(qry.next ()) cnt = qry.field(0).asInteger();   	
   if(cnt < 1 ) {
   	  qry.close();
    	return ;
   }
   qry.close();
   
 	 strcpy(sql,"insert into hbaccess_operate_log(staff_id,operate_type,\
                     operate_result,operate_cmd,operate_date,reason,table_name)\
				    values(:staff_id,:operate_type,:operate_result,\
				            :operate_cmd,sysdate,:reason,:table_name)"
          );
    qry.setSQL(sql);
    qry.setParameter("staff_id",staffID);
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

#endif	
}


//入参:工号,操作类型,操作结果,操作命令,原因说明
void hbFileLog(char *sStaffID,int type,char *result,char *sCmd,char *res)
{
#ifdef HBACCESS_FILE_LOG
try{
	  
	  if(type == 5){
    
    char sTemp[10000];
    Date dt;
    sprintf (sTemp, "OPERATE_TYPE:[%s] TABLE_NAME:[%s] STAFF_ID[%s] OPEATE_DATE:[%s] CONTENT:[%s] REMARK:[%s] \n"
            ,"other","command",sStaffID, dt.toString("yyyy-mm-dd hh:mi:ss"),sCmd,"执行命令");
    /*if(!Log::m_pologg)
        Log::m_pologg = new ThreeLogGroupMgr();
    Log::m_pologg->LogGroupWriteFile(HBACCESS,sTemp);//*/
    return;	
    }

    if(gpLogg==0) {
    	gpLogg = new ThreeLogGroup();
   	  gpLogg->m_iLogProid=-1;
   	  gpLogg->m_iLogAppid=-1;
   	}
   	
	  int cnt = 0;
	  char cmd[100];
	  strcpy(cmd,sCmd);
	  
	  if ( ! cmd ) return;
	  	
	  char chTmp[10] = {0};
	  char *p;
	  int i = 0;
	  
	  p = strtok(cmd," ");  
	  
	  if (!p ) return;
	  
	  while(*p) {
	  	 //if(*p == '(') break;
	      *p |= 0x20 ;
	      chTmp[i] = *p ;
	      i++ ;
	      p++;
	  }  
	    
	  if(strcmp(chTmp,"select") == 0) {	  	
	  	  char tmp[100] ={0} ;
				if((p = strstr(sCmd,"from")))  {
			    	p = p+4 ;
			  }else 	return ;
			   
			  strcpy(tmp,p);
			  hbtrim(tmp);
			  p = strchr(tmp,' ') ;
			  if(p)  {
			   	*p = '\0' ;
			  }
			   
			  p = tmp;
	  }else  if(strcmp(chTmp,"insert") == 0) {  
	  	
	  	  p  = strtok(NULL," ");
	  	  
	  	  if (!p ) return ;
	  	  	
	  	  p  = strtok(NULL," ");  
	  	  
	  	  int j=0;
		  	while(j<strlen(p)) {
		  	  if(p[j] == '(') {
		  	 	   p[j] = '\0';
		  	 	   break;
		  	  }
		      j++;
		    }
	  	  if(p[strlen(p)-1] == '(')  p[strlen(p)-1] == '\0';
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
   
   memset(sql,0,2000);
   strcpy(sql,"select count(*) from tab where tname = upper(:TNAME)");
   qry.setSQL(sql);
   qry.setParameter("TNAME",p);
   qry.open ();
   if(qry.next ()) cnt = qry.field(0).asInteger();   	
   if(cnt < 1 ) {
   	  qry.close();
    	return ;
   }
   qry.close();
   
    char sTemp[10000];
    char sType[20];
    if(type == 1){
    	strcpy(sType,"insert");
    }else if(type == 2){
    	strcpy(sType,"delete");
    }else  if(type == 3){
    	strcpy(sType,"update");
    }else  if(type == 4){
    	strcpy(sType,"select");
    }else  {
    	strcpy(sType,"other");
    }
    
    Date dt;
    sprintf (sTemp, "OPERATE_TYPE:[%s] TABLE_NAME:[%s] STAFF_ID[%s] OPEATE_DATE:[%s] CONTENT:[%s] REMARK:[%s] \n"
            ,sType,p,sStaffID, dt.toString("yyyy-mm-dd hh:mi:ss"),sCmd,res);
    /*if(!Log::m_pologg)
        Log::m_pologg = new ThreeLogGroupMgr();
    Log::m_pologg->LogGroupWriteFile(HBACCESS,sTemp);//*/
    
    int iCode=0;
    int iLevel=4;
    int iClass=3;
    if(strcmp(result,HBACCESS_STATE_OK)==0)
    {
    	 iCode=MBC_ACTION_MODIFY_DB ;
    	 iLevel=LOG_LEVEL_INFO;
    	 iClass=LOG_CLASS_INFO;
    	}
    if(strcmp(result,HBACCESS_STATE_FALSE)==0)
    {
    	 iCode=MBC_DB_EXECUTE_FAIL ;
    	 iLevel=LOG_LEVEL_ERROR;
    	 iClass=LOG_CLASS_ERROR;    
    }	   	 
   if(type==1 || type==2 || type==3) 
   	{
      gpLogg->log(iCode,iLevel,iClass,LOG_TYPE_DATEDB,type,p,sCmd); 
    }
    
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

#endif
}