/*VER: 1*/ 
#ifndef _SQLITEQuery_h
#define _SQLITEQuery_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>
//#include "exception.h"
#include "platform.h"
#include "../libite/sqlite3.h"
#include "../lib/CSemaphore.h"
#include "../lib/ReadIni.h"


#define  NAME_LENGTH 128 
#define  MAX_ITESQL_LENGTH 4096
#define  ITE_ERRMSG_LENGTH 2048
#define  MAX_FIELD_LEN 2048
class SITEException 
{
public:
	char *getErrMsg() const;
	char *getErrSrc() const;
	int  getErrCode() const;

public:
	SITEException(int code, const char* errFormat, ...);		
	~SITEException();

private:
	char	errMessage[ITE_ERRMSG_LENGTH+1];		//错误信息
	char	errSQL[MAX_ITESQL_LENGTH+1];			//发生错误的sql语句
	int	m_iErrCode;
};

class  SQLITEQuery
{
 public:
  	SQLITEQuery();
  	SQLITEQuery(const char *sName);
  	~SQLITEQuery();

 public:
 	 int  openDB(const char *sDBName);
    int closeDB() 	 ;
 	 void init();
	 int  setSQL(char *inSqlstmt);								//设置Sqlstatement
	 int  setSQL(const char *inSqlstmt);								//设置Sqlstatement
	 int  setSqlWaitLock(char *inSqlstmt);
	 int  open();			//打开SQL SELECT语句返回结果集
	 bool next();													//移动到下一个记录
	 bool execute();	                          //执行非SELECT语句,没有返回结果集
 	 int  close();													//关闭SQL语句，以准备接收下一个sql语句	 
	 bool commit();													//事务提交
	 bool rollback();		
	 int  prepare();
	 bool reset();
	 void printError();
	 
 
  int getFieldAsInteger(int iCol);
  long getFieldAsLong(int iCol)	;
  char* getFieldAsString(int iCol);
  double getFieldAsFloat(int iCol);

  void setParameter(int iCol,int value); 
  void setParameter(int iCol,long value);
  void setParameter(int iCol,const char *value);
  void setParameter(int iCol,double value); 
  
  void setParameter(const char *paramName,int value); 
  void setParameter(const char *paramName,long value);
  void setParameter(const char *paramName,const char *value);
  void setParameter(const char *paramName,double value);   
  
	SQLITEQuery& field(int index)		;	//返回第i个列信息	  
	SQLITEQuery& field(const char *name);	//根据列名(不分大小写)返回列信息; 建议使用Field(int i)获得更高的效率 
	char		*asString();
	double	asFloat();
	int		  asInteger();
	long		asLong();	
	
	void setfun();
private:
	inline void MakeLower(char * pchString)
  {
	  int iLen = strlen(pchString);

	  for(int i=0; i<iLen; i++)
	  {
		  pchString[i] = tolower(pchString[i]);
	  }
  }	;


 public:
   sqlite3 *m_pDB ;
   sqlite3_stmt *m_stmt;
   const char *m_tail;   
   char *m_errMsg ;
   char  m_sSql[MAX_ITESQL_LENGTH];
   char  *m_pSql ;
   char  m_sDataName[256];
   long  m_lRowNum ;
   long  m_lCurrNum ;
    
   int   m_iErrorNo ;   
   bool  m_bConnect ;
	 bool  m_bActivated;						//是否已经处于打开状态，在调用OCIStmtPrepare成功后为True      
	 bool  m_bOpened;							//数据集是否打开	
	 bool  m_bTransaction ;       //事务是否打开  。excute时打开commit/rollback时关闭。
	 
 private:
 	  char m_sTableName[NAME_LENGTH];
 	  char m_sIndexName[NAME_LENGTH];
 	  char m_sViewName[NAME_LENGTH];
 	  char m_sTriggerName[NAME_LENGTH];
	  char *m_pDate;
	  //新增信号量锁 by jx 20111006
	  long m_lshmkey;
	  CSemaphore *pSem;
	  bool m_bLockFlag;
	  long getKey(char *sDBName);
	  void lock();
	  void unlock();
	  
public:
	 int m_iParamCount ; //参数个数
	 int m_iCol ;
	 map<string,int> m_mParamMap ;
	 map<string,int> m_mOutParamMap ;	 
	 map<string,int>::iterator m_mIterator ;
	 void getParamsDef();					//在setSQL时候获得参数信息	 

};




#endif

   



