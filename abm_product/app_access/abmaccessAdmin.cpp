#include "abmaccessAdmin.h"
#include "GroupDroitMgr.h"

#define _FILE_NAME_  "abmaccessAdmin.cpp"

#include "checkUserInfo.h"


typedef multimap<string,string>::iterator iMap;
typedef multimap<string,string>::value_type valType;
bool checkpwd(char * pwd);
int getOriPassword (unsigned char sStr[]);
	
hbaccessAdmin::hbaccessAdmin()
{

}

hbaccessAdmin::~hbaccessAdmin(){
	
}


///////连接数据库 
void hbaccessAdmin::redirectDB(const char * sIniHeader)
{
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	memset(sUser, 0, sizeof(sUser));
	memset(sPwd, 0, sizeof(sPwd));
	memset(sStr, 0, sizeof(sStr));
		
	Environment::getConnectInfo(sIniHeader,sUser,sPwd,sStr);
	
	try {
		Environment::setDBLogin (sUser, sPwd, sStr);
		Environment::getDBConn (true);
	}
	 catch (...) {
	 	Log::log(0,"connect %s database fail",sIniHeader) ;
	}
} 

///////清理log表
void hbaccessAdmin::clearDataBase() {
	try {
		char t_Name[100] ={0} ;
		char t_Own[50] = {0} ;
		char t_Own1[50] ={0} ;
		char sql[512] ={0} ;
		char sql1[512] ={0} ;
		char sSql[512] ={0} ;
		char sSql1[512] ={0} ;

		multimap<string,string> t_map ;
		iMap imap ;
		vector<string> vec ;
		
		//redirectDB ("ABM");
		DEFINE_QUERY(qry) ;
		DEFINE_QUERY(qry1) ;
		
		sprintf(sql1,"select distinct owner FROM b_cleardb_cfg where owner is not null ") ;
		qry1.setSQL(sql1) ;
		qry1.open() ;
		while(qry1.next()) {
			memset(t_Own1,0,50) ;
			strcpy(t_Own1,qry1.field(0).asString()) ; 
			for(int i=0;i<strlen(t_Own1);i++)   {
    	  if(t_Own1[i]>='a'&&t_Own1[i]<='z')  {
    		  t_Own1[i]-=0X20 ;
    	  }
      }
			vec.push_back(t_Own1) ;
		}
		qry1.close() ;
		sprintf(sql,"select table_name,owner,nvl(flags,'1') FROM b_cleardb_cfg  \
		             where owner is not null") ;	
		qry.setSQL(sql) ;
		qry.open() ;
		while(qry.next())  {
			strcpy(t_Name,qry.field(0).asString()) ;
			strcpy(t_Own,qry.field(1).asString()) ;
			for(int i=0;i<strlen(t_Name);i++)   {
    	  if(t_Name[i]>='a'&&t_Name[i]<='z')  {
    		  t_Name[i]-=0X20 ;
    	  }
      }
      for(int i=0;i<strlen(t_Own);i++)   {
    	  if(t_Own[i]>='a'&&t_Own[i]<='z')  {
    		  t_Own[i]-=0X20 ;
    	  }
      }
      if(1 == qry.field(2).asInteger())		
		     t_map.insert(valType(string(t_Own),string(t_Name)) )  ;		  		  
		}		
		qry.close() ;
		vector<string>::iterator ivec ;
		for(ivec=vec.begin() ;ivec!=vec.end();ivec++) {
			//redirectDB((*ivec).c_str()) ;
			redirectDB("BILL");
			DEFINE_QUERY(qQry) ;
			DEFINE_QUERY(qQry1) ;
			int num = t_map.count((*ivec).c_str()) ;
			imap = t_map.find((*ivec).c_str()) ;
			for(int j = 0 ;j<num ; j++,imap++) {
				sprintf(sSql1,"select count(*) from all_tables where table_name ='%s' \
				               and owner = '%s'",(imap->second).c_str(),(*ivec).c_str()) ;
				qQry1.setSQL(sSql1) ;
				qQry1.open() ;
				qQry1.next() ;
				if( 0 == qQry1.field(0).asInteger() ) {
					Log::log(0,"the table %s not exist on %s database.",(imap->second).c_str(),(*ivec).c_str()) ;
					exit(0) ;
				}
				sprintf(sSql,"truncate table %s",(imap->second).c_str()) ;
				qQry.setSQL(sSql) ;
				//Log::log(0,"%s",sSql) ;
				qQry.execute();
		        qQry.commit();
		        qQry.close() ;
		        qQry1.commit();
		        qQry1.close();
			}
		}		
	}
	catch (TOCIException &oe) 
  {
      Log::log(0,"Error occured ... in clearDataBase() %s ",_FILE_NAME_);
      Log::log(0,"%s",oe.getErrMsg());
      throw oe;
  }
}

//用户名密码校验 成功返回1 否则返回0
int hbaccessAdmin::usrInfoCheck(char *sStaffID,char *sKeyWord)
{
	//UsrInfoCheckMgr usrInfoCheck;
	
	strcpy(m_sStaffId,sStaffID);
	strcpy(m_sPassed,sKeyWord);
	
	//if(CheckUserInfo::checkUerInfo(sStaffID,sKeyWord)){
		if(checkpwd(m_sPassed)){
		printf("Hello %s, Welcome to abmaccess console! \n",sStaffID);
		
#ifdef HBACCESS_DEBUG 
	  printf("工号: %s \n",sStaffID) ;
	  printf("密码: %s \n",sKeyWord) ;
#endif
	
		return 1;
		
	}else
		return 0;	
}

//用户/密码 校验
int hbaccessAdmin::usrInfoCheck(char *sInfo)
{
	char admin[100],passd[100] ;
	char *p;
	memset(admin,0,100);
	memset(passd,0,100);
	p = strrchr(sInfo,'/') ;	
	if(!p)
	{
  	printf(" 对不起登陆失败,请输入格式正确的工号密码\n") ;
  	return -1;
  }
	strncpy(admin,sInfo,p - sInfo) ;
	strcpy(passd,p + 1 ) ;

	return usrInfoCheck(admin,passd);
}

//设置sql语句 失败返回0 ,成功返回1
int hbaccessAdmin::setSQL(char *field, char * line)
{
	char *p ;
	char tmp[100] ;
	memset(tmp,0,100) ;
	char name[100],sql[100] ;
	memset(name,0,100) ;
	char sSql[200] ={0};
	strcpy(tmp,field) ;
	p=strchr(tmp,'=') ;
	if(!p){
		Log::log(0,"用户[%s]您好,您输入的脚本[%s]格式不对！",m_sStaffId,m_sOrder) ;
		//operateLog(HBACCESS_OPT_ADD,HBACCESS_STATE_FALSE,m_sOrder,"输入的脚本格式不对");
	  return 0 ;
	}
	strncpy(name,tmp,p-tmp);

#ifdef HBACCESS_DEBUG 
	  printf("field=: %s \n",field) ;
	  printf("name=: %s \n",name) ;
	  printf("line=: %s \n",line) ;
#endif

	try{
			TOCIQuery qry(Environment::getDBConn());
			sprintf(sSql,"select * from hbaccess where hbaccess_name = '%s'",name) ;
			qry.setSQL(sSql) ;
			qry.open() ;
			if(qry.next())  {
				Log::log(0,"用户[%s]您好,您输入的脚本[%s]已存在,请先删除！",m_sStaffId,name);
				//operateLog(HBACCESS_OPT_ADD,HBACCESS_STATE_FALSE,m_sOrder,"输入的脚本已存在,请先删除");
				return 0 ;
			}
		  qry.close() ;
		  memset(sSql,0,200) ;
	    sprintf(sSql,"insert into HBACCESS(HBACCESS_NAME ,HBACCESS_SQL,remark) "
	    "values(:HBACCESS_NAME,:HBACCESS_SQL,:HBACCESS_SQL)" );
		  qry.setSQL (sSql);
		  qry.setParameter("HBACCESS_NAME",name);
		  qry.setParameter("HBACCESS_SQL",line);
			if(qry.execute() == true)
		  {
		      qry.commit();
		      char sTemp[200] = {0};
				  sprintf(sTemp,"insert into hbaccess (hbaccess_name ,hbaccess_sql,remark)\
	               values('%s','%s','%s')",name,line,line);
				  hbDBLog(m_sStaffId,HBACCESS_OPT_INS,HBACCESS_STATE_OK,sTemp,m_sOrder);
				  hbFileLog(m_sStaffId,HBACCESS_OPT_INS,HBACCESS_STATE_OK,sTemp,m_sOrder);
		      return 1;
		  }
		  else
		  {
		      qry.rollback();
		      return 0;
		  }
	 }
    catch(TOCIException &oe)
    {
      Log::log(0,"收到TOCIException异常！\n[TOCIEXCEPTION]: TOCIException catched\n"
	                      "  *Err: %s\n  *SQL: %s", oe.getErrMsg(), oe.getErrSrc());
	  	Log::log(0,"执行sql语句失败：%s\n",sSql) ;  
    }
    catch(...)
    {
    	Log::log(0,"执行sql语句失败：%s\n",sSql) ;
    }
    return 0 ;
}

//删除SQL 成功返回1 否则返回0
int hbaccessAdmin::delSQL(char *name) 
{
	char *p;
	char sql[200] ={0};
	char sSql[200] ={0};
	try {
		TOCIQuery qry(Environment::getDBConn());
		sprintf(sql,"select * from hbaccess where hbaccess_name = '%s'",name ) ;
		qry.setSQL(sql) ;
		qry.open() ;
		
		if(qry.next()){
			TOCIQuery qQry(Environment::getDBConn());
			sprintf(sSql,"delete from hbaccess where hbaccess_name = '%s'",name) ;
			qQry.setSQL(sSql);				
			qQry.execute();
			qQry.commit();
			qQry.close();
			char sTemp[200] = {0};
			sprintf(sTemp,"delete from hbaccess where hbaccess_name = '%s'",name);
			hbDBLog(m_sStaffId,HBACCESS_OPT_DEL,HBACCESS_STATE_OK,sTemp,m_sOrder);
			hbFileLog(m_sStaffId,HBACCESS_OPT_DEL,HBACCESS_STATE_OK,sTemp,m_sOrder);					  
			Log::log(0,"用户[%s]您好,删除脚本[%s]成功！",m_sStaffId,name);
			//operateLog(HBACCESS_OPT_DEL,HBACCESS_STATE_OK,m_sOrder,"删除脚本成功");
			qry.close() ;
			return 1 ;
		}else {
			Log::log(0,"用户[%s]您好,[%s]该脚本不存在,无法删除！",m_sStaffId,name) ;
			//operateLog(HBACCESS_OPT_DEL,HBACCESS_STATE_FALSE,m_sOrder,"该脚本不存在,无法删除");
			qry.close() ;
			return 0 ;
		}		
	}
  catch(TOCIException &oe)
  {  
  	  Log::log(0, "收到TOCIException异常！\n[TOCIEXCEPTION]: TOCIException catched\n"
	                      "  *Err: %s\n  *SQL: %s", oe.getErrMsg(), oe.getErrSrc());
	  	Log::log(0,"执行sql语句失败：%s\n",sSql) ;  
  }
  catch(...)
  {
  	  Log::log(0,"执行sql语句失败：%s\n",sSql) ;
  }
  return 0 ;
}

//显示字段 正常返回1 否则返回0
int hbaccessAdmin::showField(char *sql)  
{
	for(int i=0;i<strlen(sql);i++)   
  {
  	if(sql[i]>='A'&&sql[i]<='Z')
  		          sql[i]|=0X20 ;  	
  }
	char *p,*q ;
	char sTableName[100]={0};
	char sSql[500] ;
	int num=0,j;
	memset(sSql,0,500) ;
	p = strstr(sql,"from") ;
  p = p+4 ;
  while(*p){
    if(*p ==' ')
        p=p+1 ;
    else
        break ;
  }
  q = strtok(p," ");
  strcpy(sTableName,q);
  
  for(int i=0;i<strlen(sTableName);i++)   {
    if(sTableName[i]>='a'&&sTableName[i]<='z')
      sTableName[i] = sTableName[i] - 32;
  }
 
  TOCIQuery qry(Environment::getDBConn());
	memset(sSql,0,500);
  sprintf(sSql,"select column_name from all_tab_columns \
   where table_name = '%s'  and (owner = 'COMM' or owner = 'BILL' or owner = 'ACCT')\
      order by column_id",sTableName);
  qry.setSQL(sSql);
  qry.open();
  while(qry.next())  {
		j=0 ;
		num++;
		while(qry.fieldCount()>j) 
		{
				if(strlen(qry.field(j).asString()) == 0)  
					  	printf(" %-16s" ,"NULL\0") ;	
				else
				      printf(" %-15s",qry.field(j).asString()) ;	
				j++;
	  }
  }	
  qry.close() ;
  printf("\n");
  if (num ==0){
  	 //printf("表[%s]的列名没找到\n",sTableName);
  	 return 0;	
  }
  return 1 ;	
}

//显示字段 正常返回1 否则返回0
int hbaccessAdmin::showFieldAd(char *sql)  
{
	int j=0;
	char sUser[32];
  char sPwd[32];
  char sStr[32];
  Environment::getConnectInfo("BILL",sUser,sPwd,sStr);
      
  try {
      DB_LINK->disconnect ();
      Environment::setDBLogin (sUser, sPwd, sStr);
      Environment::getDBConn (true);
  } catch (...) {
  }
  
	TOCIQuery qry(Environment::getDBConn());
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  
	{
		j=0 ;
		while(qry.fieldCount()>j) 
		{
				if(strlen(qry.field(j).asString()) == 0)  
					  	printf(" %-16s" ,"NULL\0") ;	
				else
				      printf(" %-15s",qry.field(j).asString()) ;	
				j++;
	  }
	}
	qry.close();
	printf("\n");
	return 1;
}

//
int hbaccessAdmin::dealcomm(char *orderName,char *paramId)
{
	char sSql[1024]={0};
	int cnt = 0;
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
	sprintf(sSql,"select count(*) from b_command_cfg where comm_name = '%s' ",orderName ) ;
  qQry.setSQL(sSql);
	qQry.open();
	if(qQry.next()){
		cnt=qQry.field(0).asInteger() ;
	}
	qQry.close() ;
	
	FILE *fp ;
	char buffer[512]={0};
	if(cnt > 0){		
			if(!m_bUserFlag){		
				  if(strcmp(orderName,"setbillingcycle") == 0 
				 	 && strcmp(paramId,"-u") == 0){
				 	  Log::log(0, "对不起该用户没有修改账期的权限");
				 	  return 0;
		      }
		   }
		   //char sOrder[512]={0};
		   char *ch =strchr(m_sOrder,' ');
		   char *ch1=strchr(ch+1,' ');
		   
		   if((fp = popen(ch1 ,"r"))!=NULL) {		   	
		   		 while(fgets(buffer,512,fp)) {  
			
					  	if(strlen(buffer) == 0)
					  		 continue ;
					  	printf(" %s \n",buffer);
					  }					  
					  pclose(fp);
		
		   }else{
		   	  printf("error: %s", strerror(errno));
		   }
		 return 0;
		
	}else {
		return 1;
	}

}

//执行sql语句 正常返回1
int hbaccessAdmin::executeOrder( char * orderName,char *paramId,char *paramId2) 
{
	
	if(dealcomm(orderName,paramId) == 0){
		return 2;
	}
	char sql1[200],sql2[2000];
	char ssql[2048];
	char sString[1000];
	char sqlfield[1024];
	memset(sql1,0,200);
	memset(sqlfield,0,1024);
	memset(sql2,0,2000);
	memset(ssql,0,2048);
	
	char sUser[32];
  char sPwd[32];
  char sStr[32];
  Environment::getConnectInfo("BILL",sUser,sPwd,sStr);
      
  try {
      DB_LINK->disconnect ();
      Environment::setDBLogin (sUser, sPwd, sStr);
      Environment::getDBConn (true);
  } catch (...) {
  
  }
  
	try {
		
		TOCIQuery qry(Environment::getDBConn());
	  sprintf(sql1,"select hbaccess_sql,hbaccess_field_name from HBACCESS where hbaccess_name = '%s' ",orderName );	  
		qry.setSQL (sql1);
		qry.open() ;
		if(!qry.next())  {
			Log::log(0,"%s 您好,您输入的命令[%s]不存在",m_sStaffId,m_sOrder) ;
			return -1 ;
		}	
		
		 //特殊处理 查询详单
    if(strcmp(orderName,"getDetailBillByAccnbr")==0){
    	
    	if(m_iargc < 5){
    		Log::log(0,"%s 您好,您的输入[%s]不完整，请输入命令[%s]的参数",m_sStaffId,m_sOrder,orderName);
    	  Log::log(0,"正确命令 abmaccess -s getDetailBillByAccnbr 业务类型 业务号码");
			  return -1 ;
			}
			
			if(m_iargc > 5){
    		Log::log(0,"%s 您好,您的输入[%s]太长，命令[%s]只有两个参数\n",m_sStaffId,m_sOrder,orderName);
			  Log::log(0,"正确命令 abmaccess -s getDetailBillByAccnbr 业务类型 业务号码");
			  return -1 ;
			}
			
    	if(specialOrder(orderName,paramId,paramId2) == 0){
    	  Log::log(0,"%s 您好,查询详单失败[%s]",m_sStaffId,m_sOrder) ;
    		return 0;	
    	}
    	
    	return 1;
    }
    
     //特殊处理 查询账单
    if(strcmp(orderName,"getChargeBillByAccnbr")==0){
    	
    	if(m_iargc < 5){
    		Log::log(0,"%s 您好,您的输入[%s]不完整，请输入命令[%s]的参数\n",m_sStaffId,m_sOrder,orderName);
			  Log::log(0,"正确命令 abmaccess -s getChargeBillByAccnbr 业务号码");
			  return -1 ;
			}
			
			if(m_iargc > 5){
    		Log::log(0,"%s 您好,您的输入[%s]太长，命令[%s]只有一个参数\n",m_sStaffId,m_sOrder,orderName);
			  Log::log(0,"正确命令 abmaccess -s getChargeBillByAccnbr 业务号码");
			  return -1 ;
			}			
			
    	if(specialOrder(orderName,paramId,paramId2) == 0){
    		Log::log(0,"%s 您好,查询帐单失败[%s]",m_sStaffId,m_sOrder) ;
    		return 0;	
    	}
    	return 1;
    }
    
    
		strcpy(sql2,qry.field(0).asString());
		strcpy(sqlfield,qry.field(1).asString());
    qry.close();
    if(strlen(sql2)< 2){
    	Log::log(0,"%s 您好,您输入的命令[%s]没有配置,请检查！",m_sStaffId,orderName) ;
			return 0 ;
    }   

    if(strstr(sql2,"%s") || strstr(sql2,"%S")){
    	
      if(m_iargc < 4){
	      Log::log(0,"%s 您好,您的输入[%s]不完整，请输入命令[%s]的参数",m_sStaffId,m_sOrder,orderName);
			  return -1 ;
			  
			}else if (m_iargc > 4){
				Log::log(0,"%s 您好,您的输入[%s]太长，命令[%s]只有一个参数",m_sStaffId,m_sOrder,orderName);
			  return -1 ;
			  
			}else{
				sprintf(ssql,sql2,paramId );
			}
			
    }else {  
    	 strcpy(ssql,sql2);
    }

/*	  for(int k=0,i=0;(k<strlen("select")+1)&&(i<strlen(ssql));i++)   
	  {
    	if(ssql[i]>='A'&&ssql[i]<='Z')  {
    		k++;
    		ssql[i]|=0X20 ;
    	}
    }//把大写字母转换成小写字母*/
    
	  qry.setSQL(ssql);	
	  if(strstr(ssql,"select")){
	  	if(strlen(sqlfield) >= 5)
	  	  showFieldAd(sqlfield);
	  	else 	
	  	 	showField(ssql);
	  	 	
			qry.open() ;
			int j;
			while(qry.next ())
			{ 
				j=0 ;
				while(qry.fieldCount()>j) {
					if(strlen(qry.field(j).asString()) == 0) {						    					   
						 printf(" %-16s","NULL\0") ;						   
					}else {
					   printf(" %-15s ", qry.field(j).asString()) ;		
					}
					
					j++ ;
				}
				printf("\n") ;			
			}	
	  }else{
	  	if(qry.execute() == true){
		       qry.commit();
		       Log::log(0,"%s 您好,执行命令[%s]成功",m_sStaffId,m_sOrder) ;
		  }else{
		       qry.rollback();
		       Log::log(0,"%s 您好,执行命令[%s]失败",m_sStaffId,m_sOrder) ;
		  }	
	  }
	  qry.close();	 
	 }
	 
	  catch(TOCIException &oe){ 
	  	sprintf (sString, "收到TOCIException异常！\n[TOCIEXCEPTION]: TOCIException catched\n"
	                      "  *Err: %s\n  *SQL: %s", oe.getErrMsg(), oe.getErrSrc());
	    Log::log (0, sString); 
	  	Log::log(0,"执行sql语句失败：%s\n",ssql) ;
	  }
	  catch(...){
	  	Log::log(0,"执行sql语句失败：%s\n",ssql) ;
	  }
	  return 1 ;
}

//执行sql语句
int hbaccessAdmin::specialOrder( char * orderName,char *paramId1,char *paramId2) 
{
	int cnt;
	char sString[1024];
	char sUser[32];
  char sPwd[32];
  char sStr[32];
  Environment::getConnectInfo("BILL",sUser,sPwd,sStr);
      
  try {
      DB_LINK->disconnect ();
      Environment::setDBLogin (sUser, sPwd, sStr);
      Environment::getDBConn (true);
  } catch (...) {
  }
  
	try {
		char sql[200];
		char sql1[200];	
		char sql2[1024];	
		char sql3[1024];	
		
		if(strcmp(orderName ,"getDetailBillByAccnbr") == 0){
			
			TOCIQuery qry(Environment::getDBConn());
			sprintf(sql,"select remark from b_event_table_type where event_table_type_id = %s",paramId1);	  
			qry.setSQL (sql);
			qry.open() ;
			if(!qry.next()){
					Log::log(0,"%s 您好,不认识的业务类型[%s]",m_sStaffId,paramId1) ;
					return 0 ;
		  }
		  	
		  printf("详单 :%s \n",qry.field(0).asString());
		  qry.close();
		  
		  sprintf(sql1,"select billing_cycle_id from billing_cycle where state = '10R' and billing_cycle_type_id =1");	  
			qry.setSQL (sql1);
			qry.open() ;
			if(!qry.next()){
					Log::log(0,"%s 您好,找不到当前账期",m_sStaffId) ;
					return 0 ;
		  }	
		  sprintf(sql2,"select distinct table_name  from b_inst_table_list \
		            where billing_cycle_id = %s and table_type = %s \
                   and billflow_id>=100"
         ,qry.field(0).asString(),paramId1); 
      
     /* sprintf(sql2,"select distinct table_name  from b_inst_table_list \
					 where billing_cycle_id = %s  and table_type = %s and  billflow_id > 90 \
					   and org_id in (select a.org_id from org a \
					     start with a.org_id in (select org_id \
					       from serv_location where serv_id in \
					        (select serv_id  from serv where acc_nbr = %s)) \
					          connect by prior a.parent_org_id = a.org_id) "
         ,qry.field(0).asString(),paramId1, paramId2); */
         
      qry.close();
      
      qry.setSQL (sql2);
			qry.open() ;
			if(!qry.next()){
					Log::log(0,"%s 您好,找不到业务类型配置表",m_sStaffId) ;
					return 0 ;
		  }	
		  printf("%-20s","主叫号码\0");
		  printf("%-20s","被叫号码\0");
		  printf("%-20s","开始时间\0");
		  printf("%-20s","时长\0");
		  printf("%-20s\n","金额\0");
		  
		  sprintf(sql3,"select a.calling_nbr,a.called_nbr,a.start_date,a.duration,a.offer_id4 from %s a where billing_nbr = %s"
         ,qry.field(0).asString(), paramId2);
      qry.close();
      
      qry.setSQL (sql3);
			qry.open() ;
      int j;
      cnt = 0;
			while(qry.next ())
			{ 
				j=0 ;
				cnt ++;
				while(qry.fieldCount()>j) {
					if(strlen(qry.field(j).asString()) == 0) {						    					   
						 printf(" %-21s","NULL\0") ;						   
					}else {
					   printf(" %-20s ", qry.field(j).asString()) ;		
					}
					
					j++ ;
				}
				printf("\n") ;			
			}	
			 qry.close();
			 if(cnt == 0) printf("\nHELLO %s,NO DATA FOUND\n",m_sStaffId);
		   printf("\n") ;	
		   
		}else if(strcmp(orderName ,"getChargeBillByAccnbr") == 0){
			
			TOCIQuery qry(Environment::getDBConn());
			sprintf(sql1,"select billing_cycle_id from billing_cycle where state = '10R' and billing_cycle_type_id =1");	  
			qry.setSQL (sql1);
			qry.open() ;
			if(!qry.next()){
					Log::log(0,"%s 您好,找不到当前账期",m_sStaffId) ;
					return 0 ;
		  }	
		  
  		sprintf(sql2,"select a.billing_cycle_id,a.acct_item_type_id,a.charge from \
  		              acct_item_aggr_%s a \
                    where a.serv_id in(select serv_id \
                    from serv where acc_nbr = %s) "
              ,paramId2,paramId1);
      qry.close();
      
      printf("%-20s","账期\0");
		  printf("%-20s","账目类型\0");
		  printf("%-20s","金额\n");
		  printf("\n");
		  qry.setSQL (sql2);
			qry.open() ;
      int j;
      cnt = 0;
			while(qry.next ())
			{ 
				j=0 ;
				cnt ++;
				while(qry.fieldCount()>j) {
					if(strlen(qry.field(j).asString()) == 0) {						    					   
						 printf(" %-21s","NULL\0") ;						   
					}else {
					   printf(" %-20s ", qry.field(j).asString()) ;		
					}
					
					j++ ;
				}
				printf("\n") ;			
			}	
			 qry.close();
			 if(cnt == 0) printf("\nHELLO %s,NO DATA FOUND\n",m_sStaffId);
			 printf("\n") ;
		  
		}else{
		
        Log::log(0,"%s 您好,不认识的命令[%s]",m_sStaffId,m_sOrder) ;
				return 0 ;
	 
	   }
	 }
	  catch(TOCIException &oe){ 
	  	sprintf (sString, "收到TOCIException异常！\n[TOCIEXCEPTION]: TOCIException catched\n"
	                      "  *Err: %s\n  *SQL: %s", oe.getErrMsg(), oe.getErrSrc());
	    Log::log (0, sString); 
	  	//Log::log(0,"执行sql语句失败：%s\n",ssql) ;
	  }
	  catch(...){
	  	//Log::log(0,"执行sql语句失败：%s\n",ssql) ;
	  }
	  return 1 ;
}

void hbaccessAdmin::printHbaccess()   
{
	char sql[200]= {0} ;
	char fieldLine[100] = {0} ;
	TOCIQuery qry(Environment::getDBConn());
  sprintf(sql,"select column_name from user_tab_columns where table_name = 'HBACCESS' order by column_id") ;
  qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		if(!fieldLine[0])
		   sprintf(fieldLine,"%s     ",qry.field(0).asString()) ;
		else
			 sprintf(fieldLine,"%s    %s",fieldLine,qry.field(0).asString()) ;
	}
				 printf("%s\n",fieldLine) ;
	qry.close() ;
	memset(sql,0,200) ;
	sprintf(sql,"select * from hbaccess") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		printf("%-20s  %s\n",qry.field(0).asString(),qry.field(1).asString()) ;
	}
	qry.close() ;
}

void hbaccessAdmin::printHbaccessad()   
{
	
  char sUser[32];
  char sPwd[32];
  char sStr[32];
  Environment::getConnectInfo("BILL",sUser,sPwd,sStr);
      
  try {
      DB_LINK->disconnect ();
      Environment::setDBLogin (sUser, sPwd, sStr);
      Environment::getDBConn (true);
  } catch (...) {
  }
  
	char sql[200]= {0} ;
	  
	printf("abmaccess -s 可用命令集\n") ;
	TOCIQuery qry(Environment::getDBConn());
	memset(sql,0,200) ;
	sprintf(sql,"select hbaccess_name,remark from hbaccess order by hbaccess_id,remark ,hbaccess_name") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		printf("%-35s    %s\n",qry.field(0).asString(),qry.field(1).asString()) ;
	}
	qry.close() ;
}
	
int hbaccessAdmin::run()
{
  /*
  GroupDroitMgr *GMDroitMgr = new GroupDroitMgr();
  strcpy(m_sStaffId,GMDroitMgr->getUsrName());
  m_bUserFlag = GMDroitMgr->IsRootUsr();
  */
  m_bUserFlag=true;
  printf("Hello %s, Welcome to abmaccess console! \n",m_sStaffId);
	while(1)
  {
    clearParam();		
	  if(cirPre() <0 )  break;		
  }
  
  return 0;
} 

int hbaccessAdmin::cirPre()
{
   int res = 0;
   char sOrder[ARG_VAL_LEN*ARG_NUM];
	 
	 memset(m_sOrder,'\0',sizeof(m_sOrder));  	
	 memset(sOrder,'\0',sizeof(sOrder)); 
	 printf("abmaccess#[%s]$",m_sStaffId);
	 
	 fflush(stdout);
	 //errno = 0;
	 gets(sOrder);
	 
	 trimEcho(sOrder,m_sOrder);
	 
	 if(m_sOrder[0] == '\0') return 0;	 
	 
	 if(strlen(m_sOrder)>ARG_VAL_LEN*ARG_NUM){
	 	 printf("命令长度超长,请输入正确的命令! \n");
		 return 0;
	 }
   
   if(strcmp(m_sOrder,"help") == 0){
        printfManual();
		    return 0;
   }
   
   if((strcmp(m_sOrder,"quit") == 0)||(strcmp(m_sOrder,"q") == 0) ||(strcmp(m_sOrder,"exit") == 0)){
		  printf("abmaccess正常退出,欢迎再次登陆! \n");
		  //operateLog(HBACCESS_OPT_OTHER,HBACCESS_STATE_OK,"QUIT","abmaccess正常退出");
		  return -1;
   }
   
   if((strcmp(m_sOrder,"QUIT") == 0)||(strcmp(m_sOrder,"Q") == 0) ||(strcmp(m_sOrder,"EXIT") == 0)){
		  printf("abmaccess正常退出,欢迎再次登陆! \n");
		  //operateLog(HBACCESS_OPT_OTHER,HBACCESS_STATE_OK,"QUIT","abmaccess正常退出");
		  return -1;
   
   }else if(m_sOrder[0] != '\0'){
	   
	    getTagString();
      deal();    
   }
    return 0;
}

int hbaccessAdmin::deal( )
{
	if( m_iargc > 1 ) {  
		
		if(strcmp(m_sArgv[0],"abmaccess") != 0){			
		  printf("\n%s 您好, %s不是hbaccess控制台命令,请重新输入\n",m_sStaffId,m_sArgv[0]) ;
  	  printfManual();
		  return 1;
		}
		
		int opt ;
		char lPass[100];
		char tmpSql[200];
		char nameSql[200];
		Date dt ;
		
		if(strcmp(m_sArgv[1],"-l")==0) { //调用数据库表空间数据清理功能
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			clearDataBase() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-g")==0)  { //原始采集文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;			
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearGaherTaskFile() ;
			return 0 ;
		}
				
		if(strcmp(m_sArgv[1],"-c")==0)  {//清理校验错误文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearCheckErrorFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-r")==0&&m_iargc == 3)  {//排重历史文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearDupHisFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-m") == 0)  { //数据合并中间文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearDataMergeFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-w")==0)  {  //写文件历史文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearwdHisFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-t")==0)  {  //tdtrans可能留下来的文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearTdtransTmpFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-a")==0)  {  //asn编码前的备份文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearAsnBkFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-e")==0)  {  //租费套餐费回退备份的回退文件
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有清理数据的权限\n");
				return 0;	
			}
			
			if(dt.checkDate(m_sArgv[2])<0) {
				printf("时间参数输入有误，请按YYYYMMDD格式输入正确时间！\n") ;
				return 0 ;
			}
			ClearFile cf ;
			cf.setEndTime(m_sArgv[2]) ;
			cf.clearRSexbakFile() ;
			return 0 ;
		}
		
		if(strcmp(m_sArgv[1],"-p")==0||strcmp(m_sArgv[1],"-p")==0)    {//帮助文档
			printfManual() ;
			return 1 ;
		}
		
		if(strcmp(m_sArgv[1],"-s")==0 && m_iargc ==2) {
			printHbaccessad() ;
			return 0 ;
		}	
		
		if(strcmp(m_sArgv[1],"-s")==0||strcmp(m_sArgv[1],"-S")==0) {
			if(m_iargc >= 3) {
				if(executeOrder(m_sArgv[2],m_sArgv[3],m_sArgv[4]) < 0)  {
					  Log::log(0,"%s 您好,您执行的命令[%s] 失败!",m_sStaffId,m_sOrder);
						return 0 ;
				} else {
					  char sTemp[200] = {0};
					  sprintf(sTemp,"select * from hbaccess where hbaccess_name = '%s'",m_sArgv[2]);
					  hbDBLog(m_sStaffId,HBACCESS_OPT_SEL,HBACCESS_STATE_OK,sTemp,m_sOrder);
					  hbFileLog(m_sStaffId,HBACCESS_OPT_SEL,HBACCESS_STATE_OK,sTemp,m_sOrder);
					  return 1;
				}
			} else  {
				printf("%s 您好 ,请检查您输入的命令\n",m_sStaffId) ;
				printHbaccessad() ;
				return 0 ;
			}
		  return 1 ;
		}
		
		//ex:hbaccess  -u getallproduct=SQL "select * from PRODUCT"		
		if(	strcmp(m_sArgv[1],"-u")==0||strcmp(m_sArgv[1],"-U")==0){
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有设置脚本的权限\n");
				return 0;	
			}
			int flag ;
			char temp[ARG_VAL_LEN];	
			memset(temp,'\0',sizeof(temp)); 
			
			if(m_sArgv[3][0]=='"')
			   strcpy(temp,m_sArgv[3]+1);
			 else
			 	 strcpy(temp,m_sArgv[3]);
			 	 
	    strcpy(tmpSql,temp);	
	    for(int k =4 ;k < m_iargc ;k++){
	    	strcat(tmpSql," ");
	    	memset(temp,'\0',sizeof(temp)); 
	    	
	    	if(k == m_iargc-1 && m_sArgv[k][strlen(m_sArgv[k])-1]=='"')
	    		strncpy(temp,m_sArgv[k],strlen(m_sArgv[k])-1);
	    	else
	    	  strcpy(temp,m_sArgv[k]);
	    	  
	      strcat(tmpSql,temp);		
	    }
	    
#ifdef HBACCESS_DEBUG 
     printf("%s\n",tmpSql) ;
#endif    
	    if(tmpSql[strlen(tmpSql) - 1 ] == ':') tmpSql[strlen(tmpSql) - 1 ] = '\0';
	    if(tmpSql[strlen(tmpSql) - 1 ] == '"') tmpSql[strlen(tmpSql) - 1 ] = '\0';
			if(!setSQL(m_sArgv[2],tmpSql)) {
				 Log::log(0,"用户[%s]设置SQL脚本:%s 失败 ",m_sStaffId,tmpSql) ;
				 return 0 ;
			}else{				 
		     Log::log(0,"用户[%s]设置SQL脚本:%s 成功",m_sStaffId,tmpSql) ;
		     //operateLog(HBACCESS_OPT_ADD,HBACCESS_STATE_OK,m_sOrder,"设置SQL脚本成功");
		     return 1;
			}
			return 1;
		}
		
		//hbaccess -d getallproduct
		if(	strcmp(m_sArgv[1],"-d")==0||strcmp(m_sArgv[1],"-D")==0)  {
			if(!m_bUserFlag){
			  printf("%s","对不起,该用户没有删除脚本的权限\n");
				return 0;	
			}
			if(!delSQL(m_sArgv[2])) {
				return 0 ;
			} else 	return 1;
		
		}else{
			printf(" abmaccess %s 不存在,请重新输入\n",m_sArgv[1]) ;
  	  printfManual();
		  return 1;
		}
		
		int pid ;
		int *status = 0 ;
		int filedes[2] ;
		if((pid=fork())==-1){
			printf("fork process error");
			return 0 ;
			
		}else if(pid == 0 ) {	
			close(filedes[0]) ;
		  dup2(filedes[1], STDOUT_FILENO) ;
		  wait(status) ;
			exit(0) ;
		}	
				
  }else{
		  printf("\n%s 您好, %s不是abmaccess控制台命令,请重新输入\n",m_sStaffId,m_sArgv[0]) ;
  	  printfManual();
		  return 1;
  }  
  
  return 0;
} 

//将录入的命令分割
int hbaccessAdmin::getTagString()
{
	 int i = 0;
   char *p = strchr(m_sOrder,' ');
   char *p1 = m_sOrder;
   
   while(p)
   {
      char sTmp2[ARG_VAL_LEN] = {0};
      strncpy(sTmp2,p1,p-p1); 

		  if(strcmp(sTmp2,"") != 0){
		 	  strcpy(m_sArgv[i++],sTmp2);
		  }
		  	  
			p++;
      p1=p;
      p = strchr(p1,' ');
   }
   
   strcpy(m_sArgv[i++],p1);
		  
   m_iargc = i;
   
#ifdef HBACCESS_DEBUG 
	for(int j = 0;j< m_iargc;j++)
	{
	  printf("j:= %d \n",j);
	  printf("m_sArgv:= %s \n",m_sArgv[j]);
	}
#endif

   return  1;
}

int hbaccessAdmin::clearParam()
{
	 for(int i =0;i<ARG_NUM;i++)
	 {
	 	 m_sArgv[i][0] = '\0';		
	 }
	 
	 m_iargc = 0;
	 
	 return 1;
}

//删除s1中的连续空格 只保留一个 然后赋值给s2
void hbaccessAdmin::trimEcho(char *s1, char *s2)
{
     int i = 0,j = 0,blank;
     while (s1[i] != '\0')
     {
           blank = 0;
           while (s1[i] == ' ')
           {
                  i++; blank++;
           }
           if (blank >= 1 ) s2[j++] = ' ';
           s2[j++] = s1[i++];
     }
     
     s2[j] = '\0';
     int k = strlen(s2)-1;
     
#ifdef HBACCESS_DEBUG 
     cout<<strlen(s2)<<endl;
#endif

     while(k >= 0){     	     	
     	  if(s2[k] == ' ')  s2[k--] = '\0';      	 
        else break;
     }
          
#ifdef HBACCESS_DEBUG 
     cout<<strlen(s2)<<endl;
#endif

     k=0;
     while(k < strlen(s2)){     	     	
     	  if(s2[k++] == ' ')  continue;      	 
        else break;
     }
          
     strcpy(s2,s2+k-1);
          
#ifdef HBACCESS_DEBUG 
     cout<<strlen(s2)<<endl;
#endif

}

void hbaccessAdmin::operateLog(int type,char *result,char *cmd,char *res)
{
#ifdef HBACCESS_DB_LOG
   char sql[2000];
   memset(sql,0,2000);
   DEFINE_QUERY (qry);
   try{
	   	 sprintf(sql,"insert into hbaccess_operate_log(staff_id,operate_type,\
	                       operate_result,operate_cmd,operate_date,reason)\
						    values('%s',%d,'%s','%s',sysdate,'%s')"
	           ,m_sStaffId,type,result,cmd,res);
        qry.setSQL(sql);
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


bool checkpwd(char * pwd)
{
	  char sOriPassword[50];
	  getOriPassword ((unsigned char *)sOriPassword);
    if (strcmp (sOriPassword, pwd)==0 ) 
    	return true;
    	
    return false;
 }

int getOriPassword (unsigned char sStr[])
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
