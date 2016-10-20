/*VER: 1*/ 
/*
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
*/
/********************************************************************
	Copyright by AsiaInfo-Linkage
	File Name: TSQLIteQuery.cpp
	Created Date: 2011/3/23
	Author:	SUN JING YAO		
	Create Version: 0.0.01
	Create Version Date: 2011/3/23
********************************************************************/	

#include "TSQLIteQuery.h"
#include "Date.h"
//#include "basefun.h"

void funInStr(sqlite3_context* ctx, int nargs, sqlite3_value** values);
void funGetIpaddr(sqlite3_context* ctx, int nargs, sqlite3_value** values) ;  
void funSysDate(sqlite3_context* ctx, int nargs, sqlite3_value** values) ; 
void funNvl(sqlite3_context *ctx,int argc,sqlite3_value **values);
void funDecode(sqlite3_context *ctx,int argc,sqlite3_value **values);
void funTrim(sqlite3_context *ctx,int argc,sqlite3_value **values);

void SQLITEQuery::setfun()
{
	 if(!m_bConnect ){
	 	 printf("DB is not Connect--%s,%d\n",__FILE__,__LINE__ ) ;
	 	  throw ;
	 }	
  m_iErrorNo=sqlite3_create_function( m_pDB, "instr", -1, SQLITE_UTF8, NULL,
                             funInStr, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}  
   	
  m_iErrorNo=sqlite3_create_function( m_pDB, "getipaddr", -1, SQLITE_UTF8, NULL,
                             funGetIpaddr, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}   
   	   	                    
  m_iErrorNo=sqlite3_create_function( m_pDB, "sysdate", -1, SQLITE_UTF8, NULL,
                             funSysDate, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}   
   	
  m_iErrorNo=sqlite3_create_function( m_pDB, "nvl", -1, SQLITE_UTF8, NULL,
                             funNvl, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}  
   	
  m_iErrorNo=sqlite3_create_function( m_pDB, "decode", -1, SQLITE_UTF8, NULL,
                             funDecode, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}       
   	
  m_iErrorNo=sqlite3_create_function( m_pDB, "trim", -1, SQLITE_UTF8, NULL,
                             funTrim, NULL, NULL);
   if(m_iErrorNo != SQLITE_OK) 
   	{
   		printError();
   		throw;
   	}      		    	
}

/*
name :funGetIpaddr
function:Trans the ip(1.1.1.1) to number
input:char *ip
output:char * number
*/ 
void funGetIpaddr(sqlite3_context* ctx, int nargs, sqlite3_value** values)
{
  char buf[16+1]={0};
  if((const char*)sqlite3_value_text(values[0])==0) return ;
  strncpy(buf,(const char*)sqlite3_value_text(values[0]),16);
  char *pbegin=buf;
  char *p=0;
  char div='.';
  long num1=0;
  long num2=0;
  long num3=0;
  long num4=0;
  long lret=0;
  char sret[10+1]={0};
        
  p=strchr(pbegin,div);
  if(p) *p=0;
  num1=atol(pbegin);
  pbegin=p+1;
  
  p=strchr(pbegin,div);
  if(p) *p=0;
  num2=atol(pbegin);
  pbegin=p+1; 
  
  p=strchr(pbegin,div);
  if(p) *p=0;
  num3=atol(pbegin);
  pbegin=p+1;     
  
  if(pbegin) num4=atol(pbegin);
  	
  lret=num1*256*256*256+num2*256*256+num3*256+num4;
  //snprintf(sret,16+1,"%ld",lret);
  sqlite3_result_int64(ctx,lret);
  //sqlite3_result_text(ctx,sret,sizeof(sret),SQLITE_TRANSIENT );
  //return sret;
}

/*
name :funInStr
function:oracle::instr()
input:char *s1 ,char *s2
output:int num(the ponit of s2 in the s1)
*/ 
void funInStr(sqlite3_context* ctx, int nargs, sqlite3_value** values)
{
	 char buf1[256+1]={0};
   if((const char*)sqlite3_value_text(values[0])==0) return ;	 
   strncpy(buf1,(const char*)sqlite3_value_text(values[0]),256);
   char buf2[64+1]={0};
   strncpy(buf2,(const char*)sqlite3_value_text(values[1]),64);
   
   char *pBegin=buf1;
   char *p=0;
   p=strstr(buf1,buf2);
   if(p) {
   	int num=p-pBegin+1;
   	sqlite3_result_int(ctx,num);
   	return ;
  }
  sqlite3_result_int(ctx,0);
   return  ;
}

/*
name :funSysDate
function:oracle::sysdate
input:null
output:the nowtime
*/ 
void funSysDate(sqlite3_context* ctx, int nargs, sqlite3_value** values)
{
	  Date dt;
	  sqlite3_result_text(ctx,dt.toString("yyyy-mm-dd hh:mi:ss"),20,SQLITE_TRANSIENT );	  
}

/*
name :funSysDate
function:oracle::nvl()
input:value1 ,value2
output:value=value1?value1:valude2
*/ 
void funNvl(sqlite3_context *ctx,int argc,sqlite3_value **values)
{
  int i;
  for(i=0; i<argc; i++){
    if( SQLITE_NULL!=sqlite3_value_type(values[i]) ){
      sqlite3_result_value(ctx, values[i]);
      break;
    }
  }
}


/*
name :funDecode
function:oracle::decode()
input:value, if1, then1, if2,then2, if3,then3, . . . default 
output:if(valude==if<n>)return then<n> 
       else retrun default
*/ 
void funDecode(sqlite3_context *ctx,int argc,sqlite3_value **values)
{
	int i=argc%2;
	if(i!=0) {
		printf("the decode param's number is error,must be an even number!\n");
		printf("return the first value\n");
    sqlite3_result_value(ctx, values[0]);
    return ;		
  }
	
	int j=argc/2-1;	
	int m=0;
	int type=sqlite3_value_type(values[0]);
	
	// compare the f1...fn
	for(int k=0;k<j;k++)
	{
		switch(type){
		case SQLITE_NULL :
			m=k*2+1;
			if(sqlite3_value_type(values[m])==SQLITE_NULL)
				{
					sqlite3_result_value(ctx, values[m+1]);
					return ;
				}
			break;
		case SQLITE_INTEGER :
			m=k*2+1;
			if(sqlite3_value_int(values[m])==sqlite3_value_int(values[0]))
				{
					sqlite3_result_value(ctx, values[m+1]);
					return ;
				}	
		  break;		
		case SQLITE_TEXT :
			m=k*2+1;
			if(!strcmp((const char*)sqlite3_value_text(values[m]),(const char*)sqlite3_value_text(values[0])))
				{
					sqlite3_result_value(ctx, values[m+1]);
					return ;
				}	
			 break;
		 default:
			m=k*2+1;
			if(!strcmp((const char*)sqlite3_value_text(values[m]),(const char*)sqlite3_value_text(values[0])))
				{
					sqlite3_result_value(ctx, values[m+1]);
					return ;
				}	
			 break;		 	
		 	break;
		}  
		 				  
  }
		
  //f1..fn not compare ,return the default
	sqlite3_result_value(ctx, values[argc-1]);
	return ;
}

/*
name :funTrim
function:oracle::trim()
input:value
output:value(no space in head &tail)
*/ 
void funTrim(sqlite3_context *ctx,int argc,sqlite3_value **values)
{
  if((const char*)sqlite3_value_text(values[0])==0) return ;
  	
 	char buf[256]={0};
  if((const char*)sqlite3_value_text(values[0])==0) return ;
  //strncpy(buf,(const char*)sqlite3_value_text(values[0]),255);
  //Trim((char*)sqlite3_value_text(values[0]));
	sqlite3_result_value(ctx,values[0]);
	return ;
}
