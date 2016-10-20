#ifndef _OCIQuery_h
#define _OCIQuery_h

#include <oci.h>
//#include <stdio.h>
#include "TException.h"

typedef signed int dword;
const int PREFETCH_ROWS = 200;						//预先提取n行数据到缓冲区,减少网络流量
const	int MAX_STRING_VALUE_LENGTH = 255;			//返回的字符串最大的列、返回过程参数长度
const	int MAX_LOB_BUFFER_LENGTH = 1024;			//LOB数据缓冲区的最大空间
const	int MAX_ERRMSG_LENGTH = 1024;					//错误信息的最大长度
const	int MAX_SQLSTMT_LENGTH = 1024;				//出现错误的SQL语句长度
const	int MAX_PARAMS_COUNT = 100;						//参数最大数目
const	int MAX_ERR_CAT_LENGTH = 50;					//错误分类长度
const	int LOB_FLUSH_BUFFER_SIZE = 400*1024;		//LOB数据积累到此量时，写入数据库

//error message definination:
const	char* const ERR_GENERAL = "General Error: %s"; //throw TOCIException("TOCIQuery(TOCIDatabase &db)", ERR_GENERAL, "Can not declare a TOCIQuery when the database is not connected");
const	char* const ERR_INDEX_OUT_OF_BOUND = "%s";    //throw TOCIException(fSqlStmt , ERR_INDEX_OUT_OF_BOUND, "field index out of bound when call Field(i)");	
const	char* const ERR_DB_INIT = "OCI: OCI handles init fail in TDatabase constructor: @line:%d";
const	char* const ERR_SET_LOGIN = "OCI: You can only set login infomation on disconnect status: line %d";
const	char* const ERR_CONNECT_NO_LOGIN_INFO = "No login information provided before Connect(), call SetLogin first, line:%d";
const	char* const ERR_NO_DATASET = "OCI: function:%s , Result Dataset is on Bof/Eof. field:%s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asBlobBuffer()", name);
const	char* const ERR_DATA_TYPE_CONVERT = "Data type convertion error: field:%s data type:%d can not be access by %s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asLobBuffer()");
const	char* const ERR_NOMORE_MEMORY_BE_ALLOCATED = "no more memory can be allocate when :%s, source code:%d"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_NOMORE_MEMORY_BE_ALLOCATED, "asBlobBuffer()", __LINE__);
const	char* const ERR_FILE_IO = "%s: can not open file:%s"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_FILE_IO, "loadFromFile()", fileName);
const	char* const ERR_MEM_BUFFER_IO = "asBlobWriter() error: read from file to buffer, field:%s, file:%s, @line:%d"; //throw TOCIException(fParentQuery->fSqlStmt, ERR_MEM_BUFFER_IO, name, fileName, __LINE__);
const	char* const ERR_DATA_TYPE_NOT_SUPPORT = "field:%s, datatype:%d not yet supported"; //, pCurrField->name,innerDataType);
const	char* const ERR_PARAM_NOT_EXISTS = "param:%s does not exists."; //throw TOCIException(fSqlStmt, ERR_PARAM_NO_EXISTS, paramName, "check spelling error");
const	char* const ERR_FIELD_NOT_EXISTS = "field:%s does not exists.";
const	char* const ERR_INVALID_METHOD_CALL	= "%s: invalid call method:%s";
const	char* const ERR_CAPABILITY_NOT_YET_SUPPORT = "capability not support yet:%s"; //例如参数个数超越范围
const	char* const ERR_READ_PARAM_DATA = "read parameter value data type error, parameter name:%s, method:%s";

//const define:
const char* const NULL_STRING = "";
const int NULL_NUMBER = 0;

/* classes defined in this file: */
class TOCIException;
class TOCIDatabase;
class TOCIQuery;
class TOCIField;
class TOCIParam;
class TOCISession;
class TOCIDirPath;	//define in TOCIDirPath.h

class TOCIException : public TException
{
public:
	char *getErrMsg() const;
	char *getErrSrc() const;
	int getErrCode() const;

public:
	TOCIException(sword errNumb, OCIError *err, char *cat, char *sql);//执行OCI函数发生的错误
	TOCIException(const char *sql, const char* errFormat, ...);	
	~TOCIException();

private:
	char	errCategory[MAX_ERR_CAT_LENGTH+1];	//错误分类
	text	errMessage[MAX_ERRMSG_LENGTH+1];		//错误信息
	char	errSQL[MAX_SQLSTMT_LENGTH+1];			//发生错误的sql语句
	int	errNo;										//错误号
	int	m_iErrCode;
};

class TOCIDatabase
{
friend class TOCIQuery;
friend class TOCISession;
friend class TOCIDirPath;
public:

public:
	TOCIDatabase();
	~TOCIDatabase();
	void setLogin(char *user, char *password, char *tnsString) ;
	bool connect();
	bool connect(char *usr, char *pwd, char *tns) ;
	void disconnect();
	void commit();
	void rollback();
	OCISession *hUser;
	//OCISvcCtx *hSvc;						//服务
	OCISvcCtx *hDBSvc;		//用于登录链接服务器	

private:
	char *usr, *pwd, *tns;	//登录名
	bool fConnected;			//在Connect中是否连接成功
	sword fErrorNo;						//错误号
	void checkError() ;					//用于判断当前的语句是否正确执行，如果有错误则把错误信息放入errMsg;
	
	OCIError *hDBErr;
	
	OCIEnv *hEnv;
	OCIServer *hSvr;
};

class TOCIParam
{
friend class TOCIQuery;
public:
	~TOCIParam();
	char	*name;

	int		asInteger();	//读取返回参数值
	double	asFloat();
	long		asLong();
	char		*asString();
	bool		isNULL();
private:
	TOCIParam();
	ub2		dataType;
	
	int		intValue;
	double	dblValue;			//存储输入和输出的值(缓冲区)
	long		longValue;			//long数据缓冲区
	char		*stringValue;		//字符串返回缓冲区
	
	int    * intArray;    //INT数组
	double * dblArray;    //DOUBLE数组
	long   * longArray;  //LONG数组
	char ** stringArray; //STRING数组
	int   stringSize;    //string数组中的string大小
	
	bool		fIsOutput;			//是否是输出参数.默认是输入参数
	sb2		indicator;			//在返回值时候是否为空
	OCIBind  *hBind;
};

class TOCIField
{
friend class TOCIQuery;

public:	
	~TOCIField(); 
	char	*name;				//字段名称
	long	size;					//数据长度
	long	type;					//数据类型 in(INT_TYPE,FLOAT_TYPE,DATE_TYPE,STRING_TYPE,ROWID_TYPE)
	int	precision;			//数值总长度
	int	scale;				//数值中小数点个数
	bool	nullable;			//字段定义时是否允许为空值--为了和其他的相一致

	bool		isNULL();			//在fetch过程中该列的数据是否为空
	char		*asString();
	double	asFloat();
	int		asInteger();
	long		asLong();
	//Blob处理
	void		asBlobFile(const char *fileName);			//读取到file中
	void		loadFromFile(const char *fileName);			//写入到blob中
	void		loadFromBuffer(unsigned char *buf, unsigned int bufLength);	//把LOB的内容用缓冲区内容替代
	void		asBlobBuffer(unsigned char* &buf, unsigned int *bufLength);	//保存到缓冲区,缓冲区的大小自动创建，并返回缓冲区大小*bufLength.
	
	//日期处理
	char		*asDateTimeString();	//把日期型的列以HH:MM:DD HH24:MI格式读取,使用asString()读取的日期型数据类型为HH:MM:DD
	void		asDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second); //返回日期的各个部分
private:
	//日期处理
	void		asDateTimeInternal(int &year, int &month, int &day, int &hour, int &minute, int &second); //返回日期的各个部分,没有作其他校验，只是内部调用					
	TOCIQuery *fParentQuery;					//指向该Field所属于的Query
	TOCIField();
	//数据缓冲区,分别为字符串、整数、浮点数分配空间
	ub1	fStrBuffer[MAX_STRING_VALUE_LENGTH];	//用于保存转换为字符串后的值
	ub1	*fDataBuf;						//在分析字段时候获得空间max(该列的最大长度,MAX_STRING_VALUE_LENGTH), 在Destructor中释放
	OCILobLocator *hBlob;				//支持LOB
	sb2	*fDataIndicator;				//在defineByPos中使用，用于在fetch时察看是否有字段值返回、字段值是否被截断;valueIsNULL, isTruncated根据此值获得结果

	ub2   fReturnDataLen;				//读取数据时返回的真实长度
	ub2	fInternalType;					//Oracle内部数据类型
	ub2	fRequestType;					//在读取数据时候的请求数据类型
	OCIDefine *hDefine;					//用于读取列信息
	int	  iBlobBufCnt;					//支持LOB缓冲的长度
};

class TOCIQuery
{
friend class TOCIField;

public:
	//主要功能
	void close();													//关闭SQL语句，以准备接收下一个sql语句
	void setSQL(const char *inSqlstmt);								//设置Sqlstatement
	void open(int prefetchRows=PREFETCH_ROWS);			//打开SQL SELECT语句返回结果集
	bool next();													//移动到下一个记录
	bool execute(int iters=1);	                          //执行非SELECT语句,没有返回结果集
	bool commit();													//事务提交
	bool rollback();												//事务回滚
	int  rowsAffected() { return fTotalRowsFetched;};	//DELETE/UPDATE/INSERT语句修改的记录数目
	int  getSQLCode() { return fErrorNo;};				//返回Oracle执行结果代码

	//与列信息相关				
	int fieldCount();												//总共有几个列
	TOCIField& field(int index)		;						//返回第i个列信息
	TOCIField& field(char *fieldName) ;						//根据列名(不分大小写)返回列信息; 建议使用Field(int i)获得更高的效率
	
	//与参数信息相关
	TOCIParam& param(long index);								//返回第i个列信息
	TOCIParam& param(char *paramName);						//根据列名(不分大小写)返回列信息; 建议使用Field(int i)获得更高的效率
	int paramCount();
																		//以下是设置参数值
	void setParameter(const char *paramName, const char* paramValue, bool isOutput = false); 	
	void setParameter(const char *paramName, int paramValue, bool isOutput = false); 
	void setParameter(const char *paramName, double paramValue, bool isOutput = false) ;
	void setParameter(const char *paramName, long paramValue, bool isOutput = false);
	void setParameterNULL(const char *paramName);

//add by Liaogd 2007/12/26
//modify by jiangmj 2008/1/11
	void setParameter(const long iPos, const char* paramValue, bool isOutput = false);
	/* 新增 long raw 处理 */
    void setLongRawParameter(char *paramName, char* paramValue, bool isOutput = false);
	/* 新增 blob 处理　*/
    void setParameter(char *paramName, void* paramValue, int iLen,bool isOutput = false);

    //数组操作
	void setParamArray(char *paramName, char ** paramValue,int iStructSize,int iStrSize ,bool isOutput = false); 	
	void setParamArray(char *paramName, int * paramValue,int iStructSize, bool isOutput = false); 
	void setParamArray(char *paramName, double * paramValue, int iStructSize,bool isOutput = false) ;
	void setParamArray(char *paramName, long * paramValue, int iStructSize,bool isOutput = false);
	/* 新增blob处理　*/
	void setParamArray(char *paramName, void ** paramValue,int iStructSize,int iStrSize,  bool isOutput=false);
	
	
	//##ModelId=424248FC01E0
  int get_fFieldCount() const;  /*  2005.03.24  为了新SQLAGENT服务增加   */
  //##ModelId=4242496102F3
  int get_fParamCount() const;
  //##ModelId=4242511200B0
  ub2 get_fStmtType() const;
	
	//constructor & destructor
	TOCIQuery(TOCIDatabase *oradb);
	TOCIQuery(TOCIDatabase *oradb,TOCISession *session);
	~TOCIQuery();
	
private:
	char *fSqlStmt;						//保存open的Select语句，可以方便调试
	ub2 fStmtType;							//***ub2!!! 保存sqlstmt的类型:SELECT/UPDATE/DELETE/INSERT/CREATE/BEGIN/ALTER...
	bool fActivated;						//是否已经处于打开状态，在调用OCIStmtPrepare成功后为True

	unsigned	fFetched;					//0..prefetchRows
	unsigned	fPrefetchRows;				//1.. 
	unsigned	fCurrRow;					//0..fetched */
	unsigned	fTotalRowsFetched;		//rows fetched from the start

	int fFieldCount;						//字段个数
	TOCIDatabase *db;						//此query属于哪个Dabase,在Constructor中创建
	TOCIField *fieldList;				//在内部保存的所有字段信息
	void getFieldsDef();					//获得字段信息,并为字段分配取值的缓冲区

	TOCIParam *paramByName(const char *paramName);//在内部使用，直接返回参数的指针
	//ADD BY LIAOGD 2007/12/26
	TOCIParam *paramByIndex(const long iIndex);//在内部使用，直接返回参数的指针
	
	void checkError() ;					//用于判断当前的语句是否正确执行，如果有错误则把错误信息放入errMsg;

	int fParamCount;						//参数个数
	TOCIParam *paramList;				//所有参数设置的信息
	void getParamsDef();					//在setSQL时候获得参数信息
	int nTransTimes;						//是否曾经执行过Execute()事务操作，以便与回滚.

	//OCITrans* hTrans;
	OCIStmt *hStmt;						//用于分析sql语句的handle
	OCIError *hErr;						//错误处理
	sword fErrorNo;						//错误号
	bool fEof;								//在Fetch时候，已经达到最后一个记录,防止已经读到最后一个记录后，又fetch发生的错误
	bool fBof;								//在Open()时候为True,在Next()如果有数据为false;用于判断用户是否可以从缓冲区中读取列值,该部分尚未完成
	bool fOpened;							//数据集是否打开	
//#ifdef __DEBUG__
//    bool bExecuteFlag;
//#endif
};

class TOCISession
{	
public:
	TOCISession(TOCIDatabase *pDB);
	~TOCISession();
	
	void sessionBegin();
	void sessionEnd();

private:
	
	OCISvcCtx	*m_hSrvCtx;
	OCISession	*m_hSession;
	OCIError	*m_hError;
	
	bool	m_bActive;
	
	int	m_iErrorNo;
	
	void checkError();

friend	class TOCIDirPath;
friend  class TOCIQuery;	
};

#endif
