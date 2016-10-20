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
//for chinese code (utf8->gbk) add by jx 20110919 
//#define SQLITE_UTF8_CODE_
//如果sqlite以UTF8存放数据，在Make.defines里新增SQLITE_UTF8_CODE_宏
#include <iconv.h>
#ifdef  DEF_LINUX
#define SQLITE_UTF_ "utf-8"
#define SQLITE_GBK_ "gb2312"
#endif
#ifdef  DEF_HP
#define SQLITE_UTF_ "utf8"
#define SQLITE_GBK_ "hp15CN"
#endif
#ifdef  DEF_AIX
#define SQLITE_UTF_ "UTF-8"
#define SQLITE_GBK_ "GB18030"
#endif

//add by jx 20110919
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
iconv_t cd;
int rc;
char **pin = &inbuf;
char **pout = &outbuf;

size_t inlen_=inlen;
size_t outlen_=outlen;

cd = iconv_open(to_charset,from_charset);
if (cd==0) 
	return -1;
memset(outbuf,0,outlen);
if (iconv(cd,pin,&inlen_,pout,&outlen_)==-1) 
{
	iconv_close(cd);
	return -1;
}

iconv_close(cd);
return 0;
}

//UTF->GB2312
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert(SQLITE_UTF_,SQLITE_GBK_,inbuf,inlen,outbuf,outlen);
}

//GB2312->UTF
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert(SQLITE_GBK_,SQLITE_UTF_,inbuf,inlen,outbuf,outlen);
}


char sTableName[NAME_LENGTH]={0};
static char chnull='\0';

int auth( void* x, int type, const char* a, const char* b, 
                 const char* c, const char* d );

SITEException::SITEException(int code, const char* errFormat, ...)
{

	m_iErrCode = code;	
	va_list ap;
	va_start(ap, errFormat);
	vsprintf((char *)errMessage, errFormat, ap);	
	va_end(ap);

}

SITEException::~SITEException()
{
}

int SITEException::getErrCode() const
{
	return m_iErrCode;
}

char *SITEException::getErrSrc() const
{
	return( (char *)errSQL );
}

char *SITEException::getErrMsg() const
{	
	return( (char *)errMessage);
}

long SQLITEQuery::getKey(char *sDBName)
{
	//取信号量锁
	char sIni[1024] = {0};
	char *p =0;
	if ((p=getenv ("APP_HOME")) == NULL)
		sprintf (sIni, "/cbilling/cbill_test/component");
	else
		sprintf (sIni, "%s/etc/sgwconfig", p);	
	char sKey[1024] = {0};
	char sTemp[32] = {0};
	sprintf (sKey,"litekey.%s",sDBName);
  ReadIni oIni;
  
  oIni.readIniString(sIni,"memory","litekey.lockflag",sTemp,"0");
  if(atoi(sTemp) == 0)
  	m_bLockFlag = false;
  else
  	m_bLockFlag = true;
  
  memset(sTemp,0,sizeof(sTemp));
  
  oIni.readIniString(sIni,"memory",sKey,sTemp,"9999527");
  m_lshmkey = atol(sTemp);
  if(pSem)
  	delete pSem;
  sprintf(sTemp,"%ld",m_lshmkey);
	pSem = new CSemaphore();
	if(!pSem->getSem(sTemp))
	{
		delete pSem;
		pSem = 0;
		return -1;
	}
	return m_lshmkey;
}

void SQLITEQuery::lock()
{
	if(!m_bLockFlag)
		return;
	if(pSem)
		pSem->P();
}

void SQLITEQuery::unlock()
{
	if(!m_bLockFlag)
		return;
	if(pSem)
		pSem->V();
}
                
SQLITEQuery::SQLITEQuery()
{
   sqlite3 *m_pDB = NULL;
   char * m_errMsg = NULL;	
   memset(m_sSql,0,sizeof(m_sSql));   
   memset(m_sDataName,0,sizeof(m_sDataName));
   char sHome[256]={0};
   char *p;
   m_bLockFlag = false;
   
   pSem = 0;
	 m_lshmkey = 0;
	 
   if ((p=getenv ("APP_HOME")) == NULL)
       sprintf (sHome, "/cbilling/cbill_test/component");
    else
       sprintf (sHome, "%s/data/shmdata", p);	
   
   sprintf(m_sDataName,"%s/sgwite.db", sHome);
	 
	 if(getKey("sgwite.db") < 0)
	 		throw SITEException(SQLITE_PERM,"Can't Get Lock_Key filebase %s:--%s,%d\n","sgwite.db",__FILE__,__LINE__ );
	 
   init();
   openDB(m_sDataName); 
   m_pDate= new char[MAX_FIELD_LEN];
}

SQLITEQuery::SQLITEQuery(const char *sName)
{
   sqlite3 *m_pDB = NULL;
   char * m_errMsg = NULL;
   memset(m_sSql,0,sizeof(m_sSql));
   memset(m_sDataName,0,sizeof(m_sDataName));
   
   char sHome[256]={0};
   char *p;
   
   pSem = 0;
	 m_lshmkey = 0;

   if ((p=getenv ("APP_HOME")) == NULL)
       sprintf (sHome, "/cbilling/cbill_test/component");
    else
       sprintf (sHome, "%s/data/shmdata", p);	   
       
   sprintf(m_sDataName,"%s/%s", sHome,sName);

	 if(getKey(sName) < 0)
	 	throw SITEException(SQLITE_PERM,"Can't Get Lock_Key filebase %s:--%s,%d\n",sName,__FILE__,__LINE__ );
   
   init();
   openDB(m_sDataName);
   	m_pDate= new char[MAX_FIELD_LEN];
}

SQLITEQuery::~SQLITEQuery()
{
	closeDB();
   if(m_pDB != NULL){
   	  //delete m_pDB ;
   	  m_pDB = NULL ;
   	} 
   	
   if(m_errMsg != NULL){
   	  //delete m_errMsg ;
   	  m_errMsg = NULL ;
   	} 
   	
   if(m_pSql !=NULL){
   	  delete m_pSql ;
   	  m_pSql=NULL;
   	}
   if(m_pDate !=NULL){
   	delete m_pDate;
   	m_pDate = NULL;
   }
   if(pSem)
   {
   	pSem->V();
   	delete pSem;
   	pSem = NULL;
   }
}

void SQLITEQuery::init()
{
	m_lRowNum = 0 ;
  m_lCurrNum = 0 ;
	m_bConnect =false ;
	m_bActivated =false ;
  m_bOpened = false ;	
  m_bTransaction = false ;
  m_iParamCount = 0 ;  
  m_iCol=0 ;    
  m_pSql = NULL;
  m_mParamMap.clear();
  m_mOutParamMap.clear();  
}
	
int  SQLITEQuery::openDB(const char *sDBName)
{
    m_iErrorNo = sqlite3_open(sDBName, &m_pDB);
    if(m_iErrorNo != SQLITE_OK) {
        printf("Can't open filebase %s: %s--%s,%d\n",sDBName,sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
        sqlite3_close(m_pDB);
        throw SITEException(m_iErrorNo,"Can't open filebase %s: %s--%s,%d\n",sDBName,sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
    }
    m_bConnect =  true ;
    m_bActivated = false ;
    setfun();
    return m_iErrorNo;	
}

int SQLITEQuery::closeDB()
{
	if(m_bTransaction)
		{
			m_iErrorNo = sqlite3_exec(m_pDB, "commit transaction", 0, 0, &m_errMsg ); //提交事务
			if(m_iErrorNo != SQLITE_OK) {
				printf("FQL commit error: %s--%s,%d\n", m_errMsg,__FILE__,__LINE__ );
	  	  throw SITEException(m_iErrorNo,"FQL commit error: %s--%s,%d\n", m_errMsg,__FILE__,__LINE__ );
	  	}
	  m_bTransaction = false ;//关闭事务
	  }		
	  
	 if(m_bActivated ) {
	    m_iErrorNo=sqlite3_finalize(m_stmt);
	    m_bActivated =  false ;
	 	}		  	
	 	
	  m_iErrorNo=sqlite3_close(m_pDB);
	  if(m_iErrorNo != SQLITE_OK){
	  		printf("Can't close  filebase: %s--%s,%d\n",sqlite3_errmsg(m_pDB),__FILE__,__LINE__  );
	  		throw SITEException(m_iErrorNo,"Can't close  filebase: %s--%s,%d\n",sqlite3_errmsg(m_pDB),__FILE__,__LINE__  );
	  	}
	 m_bConnect = false ; 	
	 return  m_iErrorNo;
}

int SQLITEQuery::setSQL(const char *inSqlstmt)								//设置Sqlstatement
{
	return setSQL((char*)inSqlstmt);
}

int SQLITEQuery::setSQL(char *inSqlstmt)
{

	 	if(!m_bConnect ){
	 		printf("fileDB is not Connect--%s,%d\n",__FILE__,__LINE__ ) ;
	 	  throw SITEException(m_iErrorNo,"fileDB is not Connect--%s,%d\n",__FILE__,__LINE__ ) ;
	 	  }
			  		  
		//pSem->P();
		lock();
	 //m_iErrorNo=sqlite3_set_authorizer(m_pDB, auth, m_pDate);
	  
	 int nLen = strlen(inSqlstmt);
	 if(nLen + 512>=MAX_ITESQL_LENGTH )
	 	{
	   if (m_pSql != NULL) delete[] m_pSql;
	   m_pSql = new char[nLen + 512];	 
	   if(m_pSql == NULL) throw ;
#ifdef SQLITE_UTF8_CODE_
		int iRet = 0;
		memset(m_pSql,0,nLen + 512);
		iRet = g2u(inSqlstmt,nLen,m_pSql,nLen+512);
		if( iRet == -1 )
			strcpy(m_pSql,inSqlstmt);
#else	   	
	   strcpy(m_pSql,	inSqlstmt);
#endif
     m_iErrorNo = sqlite3_prepare(m_pDB, m_pSql, (int)strlen(m_pSql), &m_stmt, &m_tail);
   }	   
	 else{		 	
	   memset(m_sSql,0,sizeof(m_sSql));
#ifdef SQLITE_UTF8_CODE_
		int iRet = 0;
		iRet = g2u(inSqlstmt,nLen,m_sSql,MAX_ITESQL_LENGTH);
		if( iRet == -1 )
			strcpy(m_sSql,inSqlstmt);
#else	   	
		strncpy(m_sSql,inSqlstmt,MAX_ITESQL_LENGTH-1);
#endif	   

     m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);	   
	 }
	 
   if(m_iErrorNo==SQLITE_BUSY ||m_iErrorNo == SQLITE_LOCKED)
   	{
      m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
      m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);	         
    }   		
	/*   
   while(m_iErrorNo == SQLITE_BUSY)
   {
   
    usleep(10000);
    if(nLen>2047){
       m_iErrorNo = sqlite3_prepare(m_pDB, m_pSql, (int)strlen(m_pSql), &m_stmt, &m_tail);
    }else{
      m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);
    }   
   }
   */
   if(m_iErrorNo != SQLITE_OK) {
        printf("FQL setfql error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
        printf("Tht FQL is :%s--%s,%d\n",m_sSql,__FILE__,__LINE__ );
        throw SITEException(m_iErrorNo,"FQL setfql(%s) error: %s--%s,%d\n", m_sSql,sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
    }
   m_bActivated =  true ;
   m_bTransaction =false;
   getParamsDef();  
   //cout<<"需要设置参数的个数为"<<m_iParamCount<<endl;
   return m_iErrorNo; 		 
}

int SQLITEQuery::setSqlWaitLock(char *inSqlstmt)
{

	 	if(!m_bConnect ){
	 		printf("fileDB is not Connect--%s,%d\n",__FILE__,__LINE__ ) ;
	 	  throw SITEException(m_iErrorNo,"fileDB is not Connect--%s,%d\n",__FILE__,__LINE__ ) ;
	 	  }

	 //m_iErrorNo=sqlite3_set_authorizer(m_pDB, auth, m_pDate);
		//pSem->P();	  
	 lock();
	 int nLen = strlen(inSqlstmt);
	 if(nLen + 512>=MAX_ITESQL_LENGTH )
	 	{
	   if (m_pSql != NULL) delete[] m_pSql;
	   m_pSql = new char[nLen + 512];	 
	   if(m_pSql == NULL) throw ;
#ifdef SQLITE_UTF8_CODE_
		int iRet = 0;
		memset(m_pSql,0,nLen + 512);
		iRet = g2u(inSqlstmt,nLen,m_pSql,nLen+512);
		if( iRet == -1 )
			strcpy(m_pSql,inSqlstmt);
#else	   	
	   strcpy(m_pSql,	inSqlstmt);
#endif
     m_iErrorNo = sqlite3_prepare(m_pDB, m_pSql, (int)strlen(m_pSql), &m_stmt, &m_tail);
   }	   
	 else{		 	
	   memset(m_sSql,0,sizeof(m_sSql));
#ifdef SQLITE_UTF8_CODE_
		int iRet = 0;
		iRet = g2u(inSqlstmt,nLen,m_sSql,MAX_ITESQL_LENGTH);
		if( iRet == -1 )
			strcpy(m_sSql,inSqlstmt);
#else	   	
		strncpy(m_sSql,inSqlstmt,MAX_ITESQL_LENGTH-1);
#endif	   

     m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);	   
	 }
	 
   if(m_iErrorNo==SQLITE_BUSY ||m_iErrorNo == SQLITE_LOCKED)
   	{
      m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
      m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);	         
    }   		
   
   while(m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_BUSY)
   {
    printf("FQL setfql is busy: %s\n", sqlite3_errmsg(m_pDB));
    usleep(10000);
    if(nLen>2047){
       m_iErrorNo = sqlite3_prepare(m_pDB, m_pSql, (int)strlen(m_pSql), &m_stmt, &m_tail);
    }else{
      m_iErrorNo = sqlite3_prepare(m_pDB, m_sSql, (int)strlen(m_sSql), &m_stmt, &m_tail);
    }   
   }

   m_bActivated =  true ;
   m_bTransaction =false;
   getParamsDef();  
   //cout<<"需要设置参数的个数为"<<m_iParamCount<<endl;
   return m_iErrorNo; 		 
}
								
int SQLITEQuery::open()			//打开SQL SELECT语句返回结果集
{
	 m_bOpened = false ;
	 if(!m_bActivated) {
	 	  printf("The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	}
	  m_iErrorNo=sqlite3_step(m_stmt);
    	
	  while(m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_BUSY)
			{
				printf("FQL open  is busy: %s\n", sqlite3_errmsg(m_pDB));
				m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
				m_iErrorNo=sqlite3_step(m_stmt);
			}
	 if(m_iErrorNo != SQLITE_ROW)
	 	{
	   	return 0;
	 	}	
   //m_lRowNum=lFetchNum; 
	 m_mOutParamMap.clear();
	 char cName[128]={0};
   int inCols = sqlite3_column_count(m_stmt);
   for(int i=0;i<inCols;i++)
   {
    memset(cName,0,sizeof(cName));
	  strncpy(cName,sqlite3_column_name(m_stmt,i),127); 	
	  MakeLower(cName);
   	string sParam(cName); 
		 m_mOutParamMap.insert(pair<string,int>(sParam,i));  
	 } 		  
   m_lCurrNum=1 ; //  第一条数据
   m_bOpened = true ;
   return m_iErrorNo ;
}


	
bool SQLITEQuery::next()
{
		if(!m_bOpened ) return false ;
	  if(m_lCurrNum==1) { //第一条 直接返回 第一次调用step时存在了
	  	m_lCurrNum++;
	  	return true ;
	  }
	  //if(m_lCurrNum == m_lRowNum ) return false ;	  	
	  m_iErrorNo = sqlite3_step(m_stmt);
	  while(m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_BUSY)
			{
				printf("FQL next  is busy: %s\n", sqlite3_errmsg(m_pDB));
				m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
				m_iErrorNo=sqlite3_step(m_stmt);
			}	  
    if(m_iErrorNo != SQLITE_ROW ) {
    	 m_bOpened = false ;
      // printf("Have not next Data: %s\n", sqlite3_errmsg(m_pDB));
       return false ;
     }
    m_lCurrNum++;
   return true ;
}

bool SQLITEQuery::execute()
{
	 if(!m_bActivated) {
	 	  printf("The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	}
	 
	 if(!m_bTransaction )
	 	{   //打开事务上锁，如果有其他事务已经锁住，则无限期进行等待
  		  	
		 	char sTrans[128]={0}; 
		 
		  m_iErrorNo=sqlite3_exec(m_pDB, "begin immediate transaction", 0, 0, &m_errMsg );
		  while(m_iErrorNo==SQLITE_BUSY ||m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_ERROR ){
		    usleep(10000);
		  	m_iErrorNo=sqlite3_exec(m_pDB,"begin immediate transaction", 0, 0, &m_errMsg );
		  }
		  
			if(m_iErrorNo != SQLITE_OK) {
				printf("FQL begin transaction(table:%s) error: %s--%s,%d\n", m_sTableName,m_errMsg,__FILE__,__LINE__ );
	  	  printError();
	  	  throw SITEException(m_iErrorNo,"FQL begin transaction(table:%s) error: %s--%s,%d\n", m_sTableName,m_errMsg,__FILE__,__LINE__ );
	  	}	 	
	  	m_bTransaction = true ;
	  }
	  		 	
	 m_iErrorNo=sqlite3_step(m_stmt);
	 if(m_iErrorNo != SQLITE_DONE)
	 	{
	 		 printf("FQL excute(%s) error: %s--%s,%d\n",m_sSql, sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 printError();
	 		 rollback();
	 		 throw SITEException(m_iErrorNo,"FQL excute(%s) error: %s--%s,%d\n",m_sSql, sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}
	 return true ;
}

int SQLITEQuery::close()
{
  if((!m_bActivated) && (!m_bOpened) )
  {
  	if(!m_pDate)
  	{
			delete m_pDate;
			m_pDate = 0;
		}
		//pSem->V();
		unlock();
		return false;
  }
  
  
	 if(!m_bActivated ) {
	 	  printf("FQL Prepare is not sucess ,need not finaliz--%s,%d\n",__FILE__,__LINE__ );
	 	  //pSem->V();
	 	  unlock();
	 	  throw SITEException(m_iErrorNo,"FQL Prepare is not sucess ,need not finaliz--%s,%d\n",__FILE__,__LINE__ );
	 	}	
	  m_iErrorNo=sqlite3_finalize(m_stmt);
	  if(m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_BUSY)
			{
				m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
	      m_iErrorNo=sqlite3_finalize(m_stmt);				
			}
	  if(m_iErrorNo!=SQLITE_OK){
	  	printf("FQL close error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	  	printError();
	  	//pSem->V();
	  	unlock();
	  	throw SITEException(m_iErrorNo,"FQL close error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	  }
    m_bOpened = false ;	  
    m_bActivated = false ;
  	if(!m_pDate)
  	{
			delete m_pDate;
			m_pDate = 0;
		}
		//pSem->V();
		unlock();
	  return m_iErrorNo ;
}

bool SQLITEQuery::reset()
{
	 if(!m_bActivated) {
	 	  printf("The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	 throw SITEException(m_iErrorNo,"The FQL is not set--%s,%d\n",__FILE__,__LINE__ );
	 	}
	 m_iErrorNo=sqlite3_reset(m_stmt);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL reset error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 printError();
	 		 throw SITEException(m_iErrorNo,"FQL reset error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}		
	 return true ;	
}

	
bool SQLITEQuery::commit()
{
	if(m_bTransaction)
		{
			char sTrans[128]={0};
			sprintf(sTrans,"commit %s","transaction");
			m_iErrorNo = sqlite3_exec(m_pDB, sTrans, 0, 0, &m_errMsg ); //提交事务
			while(m_iErrorNo == SQLITE_LOCKED || m_iErrorNo ==SQLITE_BUSY)
				{
					m_iErrorNo=sqlite3_busy_timeout(m_pDB,200);
          m_iErrorNo = sqlite3_exec(m_pDB, sTrans, 0, 0, &m_errMsg ); 					
				}
			if(m_iErrorNo != SQLITE_OK) {
				printf("FQL commit(table:%s) error: %s--%s,%d\n",m_sTableName, m_errMsg,__FILE__,__LINE__ );
				printError();
				pSem->V();
	  	  throw SITEException(m_iErrorNo,"FQL commit(table:%s) error: %s--%s,%d\n",m_sTableName, m_errMsg,__FILE__,__LINE__ );
	  	}
	  m_bTransaction = false ;//关闭事务
	  return true;
	  }
	return false ;
}

bool SQLITEQuery::rollback()
{

	if(m_bTransaction)
		{
			m_iErrorNo = sqlite3_exec(m_pDB, "rollback  transaction", 0, 0, &m_errMsg ); //提交事务
			if(m_iErrorNo != SQLITE_OK) {
				printf("FQL rollback  error: %s--%s,%d\n", m_errMsg,__FILE__,__LINE__ );
				printError();
			 pSem->V();
	  	 throw SITEException(m_iErrorNo,"FQL rollback  error: %s--%s,%d\n", m_errMsg,__FILE__,__LINE__ );
	  	}
	  m_bTransaction = false ;//关闭事务	  	
	  return true;
	  }
	return false ;
}

int SQLITEQuery::getFieldAsInteger(int iCol)
{
	int ret = sqlite3_column_int(m_stmt,iCol) ;
	return ret ;
		
}

long SQLITEQuery::getFieldAsLong(int iCol)	
{
	long ret = (long)sqlite3_column_int(m_stmt, iCol);
	return ret ;
}

char* SQLITEQuery::getFieldAsString(int iCol)	
{
	char *ret = (char *)sqlite3_column_text(m_stmt,  iCol);
#ifdef SQLITE_UTF8_CODE_
	int iRet = 0;
	if(!m_pDate)
		m_pDate = new char[MAX_FIELD_LEN];
	memset(m_pDate,0,MAX_FIELD_LEN);
	iRet = u2g(ret,strlen(ret),m_pDate,MAX_FIELD_LEN);
	if( iRet != -1 )
		ret = m_pDate;
#endif
	return ret ;

}

double SQLITEQuery::getFieldAsFloat(int iCol)
{
	double ret = sqlite3_column_double(m_stmt, iCol);
	return ret ;
}

void SQLITEQuery::setParameter(int iCol,int value)
{
	 m_iErrorNo = sqlite3_bind_int(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}	
}

void SQLITEQuery::setParameter(int iCol,long value)
{
	 m_iErrorNo = sqlite3_bind_int(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}		
}

void SQLITEQuery::setParameter(int iCol,const char *value)
{
	 char *p=(char *)value ;
	 int length=0;
	 while(*p){
	 	 p++;
	 	 length++;
	 }
#ifdef SQLITE_UTF8_CODE_
	int iRet = 0;
	if(!m_pDate)
		m_pDate = new char[MAX_FIELD_LEN];
	memset(m_pDate,0,MAX_FIELD_LEN);
	iRet = g2u((char *)value,length,m_pDate,MAX_FIELD_LEN);
	if( iRet != -1 )
		m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, value,length, NULL);
	else
		m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, m_pDate,strlen(m_pDate), NULL);	
#else
	 m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, value,length, NULL);
#endif
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}		
}

void SQLITEQuery::setParameter(int iCol,double value)
{
	 m_iErrorNo = sqlite3_bind_double(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}			
}

void SQLITEQuery::printError()
{

 	switch (m_iErrorNo)
 	 {
 	 	
 	  case SQLITE_OK         :  // 0;  返回成功
 	  	break ;
    case SQLITE_ERROR      :  // 1;  SQL错误或错误的数据库
    	printf("LinkageFileBase Error --FQL ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;
    case SQLITE_INTERNAL   :  // 2;  An internal logic error in SQLite
    	printf("LinkageFileBase Error --An internal logic error in SQLite .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_PERM       :  // 3;  拒绝访问
    	printf("LinkageFileBase Error --Refuse Access .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_ABORT      :  // 4;  回调函数请求中断
    	printf("LinkageFileBase Error --CallBack ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_BUSY       :  // 5;  数据库文件被锁
    	printf("LinkageFileBase Error --File Locked .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_LOCKED     :  // 6;  数据库中的一个表被锁
    	printf("LinkageFileBase Error --Tbale Locked .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_NOMEM      :  // 7;  内存分配失败
    	printf("LinkageFileBase Error --MEM Malloc ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_READONLY   :  // 8;  试图对一个只读数据库进行写操作
    	printf("LinkageFileBase Error --UPdata Onle Read DB .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_INTERRUPT  :  // 9;  由sqlite_interrupt()结束操作
    	printf("LinkageFileBase Error --Interrupt  .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_IOERR      :  // 10; 磁盘I/O发生错误
    	printf("LinkageFileBase Error --DISK I/O ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_CORRUPT    :  // 11; 数据库磁盘镜像畸形
    	printf("LinkageFileBase Error --DISK Mirror Error .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_NOTFOUND   :  // 12; (Internal Only)表或记录不存在
    	printf("LinkageFileBase Error --Table No Exist .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_FULL       :  // 13; 数据库满插入失败
    	printf("LinkageFileBase Error --Insert ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_CANTOPEN   :  // 14; 不能打开数据库文件
    	printf("LinkageFileBase Error --Open DB File  ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_PROTOCOL   :  // 15; 数据库锁定协议错误
    	printf("LinkageFileBase Error --DB Locked ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_EMPTY      :  // 16; (Internal Only)数据库表为空
    	printf("LinkageFileBase Error --Table Is Empty .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_SCHEMA     :  // 17; 数据库模式改变
    	printf("LinkageFileBase Error --DB Mode Change .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_TOOBIG     :  // 18; 对一个表数据行过多
    	printf("LinkageFileBase Error --COlNums TOO Much .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_CONSTRAINT :  // 19; 由于约束冲突而中止
    	printf("LinkageFileBase Error --CONSTRAINT ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_MISMATCH   :  // 20; 数据类型不匹配
    	printf("LinkageFileBase Error --Date match ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_MISUSE     :  // 21; 数据库错误使用
    	printf("LinkageFileBase Error --DB Use ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_NOLFS      :  // 22; 使用主机操作系统不支持的特性
    	printf("LinkageFileBase Error --Host NOLFS ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_AUTH       :  // 23; 非法授权
    	printf("LinkageFileBase Error --COPY Right  ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_FORMAT     :  // 24; 辅助数据库格式错误
    	printf("LinkageFileBase Error --FORMAT ERROR .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    
    case SQLITE_RANGE      :  // 25; 2nd parameter to sqlite_bind out of range
    	printf("LinkageFileBase Error --Parameter to sqlite_bind out of range .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;    	
    case SQLITE_NOTADB     :  // 26; 打开的不是一个数据库文件
      printf("LinkageFileBase Error --Open File Is not DBFile .err_no:%d. err_des:%s",m_iErrorNo,sqlite3_errmsg(m_pDB));
    	break;
    default :
    	break;
 	 }	
}

void SQLITEQuery::getParamsDef()
{
const int MAX_PARAMS_COUNT =100;
	char *params[MAX_PARAMS_COUNT];
	int i, in_literal, n, nParamLen,nFlag = 0;
	char *cp,*ph;
	char *sql;
	
	int nLen = strlen(this->m_sSql);
	sql = new char[nLen+1];
	strcpy(sql, this->m_sSql);
	sql[nLen] = '\0';

  m_mParamMap.clear();
  int iIndex =1;
	// Find and bind input variables for placeholders. 
	for (i = 0, in_literal = false, cp = sql; *cp != 0; cp++)
	{

      if (*cp == '\'')
			in_literal = ~in_literal;
      if (*cp == ':' && *(cp+1) != '=' && !in_literal)
      {

			for ( ph = ++cp, n = 0;  *cp && (isalnum(*cp) || *cp == '_'); cp++, n++);
			if(*cp == 0) 
				nFlag = 1;
			else 
				*cp = 0;

			if ( i > MAX_PARAMS_COUNT)
				throw  ;
			nParamLen = strlen((char *)ph);

			params[i] = new char[nParamLen+1];
			strcpy(params[i],(char *)ph);
			params[i][nParamLen] = '\0';
			string sParam(params[i]);
			m_mParamMap.insert(pair<string,int>(sParam,iIndex));
			i++;
			iIndex++;
			if(nFlag == 1) break;
      }   
	}
	m_iParamCount=m_mParamMap.size();
	delete[] sql;
}


void SQLITEQuery::setParameter(const char *paramName,int value)
{
	 string sParam(paramName);
	 int iCol=1;	 
	 m_mIterator=m_mParamMap.find(sParam);
	 if(m_mIterator!=m_mParamMap.end()){
	   iCol=m_mIterator->second;
	 }
	 else {
	 	  printf("The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	}
	 m_iErrorNo = sqlite3_bind_int(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	}	
}

void SQLITEQuery::setParameter(const char *paramName,long value)
{
	 string sParam(paramName);
	 int iCol=1;
	 m_mIterator=m_mParamMap.find(sParam);	 
	 if(m_mIterator!=m_mParamMap.end()){
	   iCol=m_mIterator->second;
	 }
	 else {
	 	  printf("The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	}
	 m_iErrorNo = sqlite3_bind_int(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}		
}

void SQLITEQuery::setParameter(const char *paramName,const char *value)
{
	 string sParam(paramName);
	 int iCol=1;	 
	 m_mIterator=m_mParamMap.find(sParam);
	 if(m_mIterator!=m_mParamMap.end()){
	   iCol=m_mIterator->second;
	 }
	 else {
	 	  printf("The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	}
	 	
	 char *p=(char *)value ;
	 int length=0;
	 while(*p){
	 	 p++;
	 	 length++;
	 }
#ifdef SQLITE_UTF8_CODE_
	int iRet = 0;
	if(!m_pDate)
		m_pDate = new char[MAX_FIELD_LEN];
	memset(m_pDate,0,MAX_FIELD_LEN);
	iRet = g2u((char *)value,length,m_pDate,MAX_FIELD_LEN);
	if( iRet != -1 )
		m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, value,length, NULL);
	else
		m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, m_pDate,strlen(m_pDate), NULL);	
#else
	 m_iErrorNo = sqlite3_bind_text(m_stmt, iCol, value,length, NULL);
#endif
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}		
}

void SQLITEQuery::setParameter(const char *paramName,double value)
{
	 string sParam(paramName);
	 int iCol=1;	 
	 m_mIterator=m_mParamMap.find(sParam);
	 if(m_mIterator!=m_mParamMap.end()){
	   iCol=m_mIterator->second;
	 }
	 else {
	 	  printf("The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", paramName,__FILE__,__LINE__ );
	 	}
	 	
	 m_iErrorNo = sqlite3_bind_double(m_stmt, iCol, value);
	 if(m_iErrorNo != SQLITE_OK)
	 	{
	 		 printf("FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 		 throw SITEException(m_iErrorNo,"FQL bind data error: %s--%s,%d\n", sqlite3_errmsg(m_pDB),__FILE__,__LINE__ );
	 	}			
}

SQLITEQuery& SQLITEQuery::field(int index)
{
	m_iCol=index;
	return (*this);
}

SQLITEQuery& SQLITEQuery::field(const char *name)
{
	  char cName[128]={0};
	  strncpy(cName,name,127);
	  MakeLower(cName);
	 string sParam(cName);
	 m_mIterator=m_mOutParamMap.find(sParam);
	 if(m_mIterator!=m_mOutParamMap.end()){
	    this->m_iCol=m_mIterator->second;
	    //printf("the first value is %s, the second valude is %d\n",m_mIterator->first.c_str(),m_mIterator->second);
	 }
	 else {
	 	  printf("The set Param %s is not exist in the FQL!--%s,%d\n", name,__FILE__,__LINE__ );
	 	  throw SITEException(m_iErrorNo,"The set Param %s is not exist in the FQL!--%s,%d\n", name,__FILE__,__LINE__ );
	 	}	 
	 return (*this);
}

char* SQLITEQuery::asString()
{
	char *ret = (char *)sqlite3_column_text(m_stmt,  m_iCol);
	if(ret==0) return &chnull;
#ifdef SQLITE_UTF8_CODE_
	int iRet = 0;
	if(!m_pDate)
		m_pDate = new char[MAX_FIELD_LEN];
	memset(m_pDate,0,MAX_FIELD_LEN);
	iRet = u2g(ret,strlen(ret),m_pDate,MAX_FIELD_LEN);
	if( iRet != -1 )
		ret = m_pDate;
#endif
	return ret ;	
}

double SQLITEQuery::asFloat()
{
	double ret = sqlite3_column_double(m_stmt, m_iCol);
	return ret ;	
}

int	SQLITEQuery::asInteger()
{
	int ret = sqlite3_column_int(m_stmt,m_iCol) ;
	return ret ;
}

long	SQLITEQuery::asLong()
{
	long ret = (long)sqlite3_column_int(m_stmt, m_iCol);
	return ret ;
}	

/*
int auth( void* x, int type, const char* a, const char* b, 
                 const char* c, const char* d )
{   
	int ret=SQLITE_OK;

	switch(type){
	 case SQLITE_CREATE_INDEX :
   case SQLITE_CREATE_TEMP_INDEX :	 
   case SQLITE_DROP_INDEX :   
   case SQLITE_DROP_TEMP_INDEX :
   	 strncpy(sIndexName,a,NAME_LENGTH);
     strncpy(sTableName,b,NAME_LENGTH); 
     break; 	 
   	   			
   case SQLITE_CREATE_TABLE :
   case SQLITE_CREATE_TEMP_TABLE :
   case SQLITE_DELETE :
   case SQLITE_DROP_TABLE :
   case SQLITE_DROP_TEMP_TABLE :
     strncpy(sTableName,b,NAME_LENGTH); 
     break; 	 
        	   	
   case SQLITE_CREATE_TEMP_VIEW :
   case SQLITE_CREATE_VIEW : 
   case SQLITE_DROP_TEMP_VIEW :
   case SQLITE_DROP_VIEW :
   	 strncpy(sViewName,a,NAME_LENGTH);
     strncpy(sTableName,b,NAME_LENGTH); 
     break; 	 
        	   	   	
   case SQLITE_CREATE_TEMP_TRIGGER :   
   case SQLITE_CREATE_TRIGGER :
   case SQLITE_DROP_TEMP_TRIGGER :    	
   case SQLITE_DROP_TRIGGER :
   	 strncpy(sTriggerName,a,NAME_LENGTH);
     strncpy(sTableName,b,NAME_LENGTH); 
     break; 	 

   case SQLITE_INSERT :
   case SQLITE_UPDATE :
     strncpy(sTableName,a,NAME_LENGTH); 
     x=sTableName;
     pDate=sTableName;
     break; 	 

   case SQLITE_PRAGMA :
   case SQLITE_READ :
   case SQLITE_SELECT :
   case SQLITE_TRANSACTION :
   case SQLITE_ATTACH :
   case SQLITE_DETACH :
   default :
   	 break;
  }

  return ret ;
	
} 
*/               

