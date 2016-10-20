/*VER: 1*/ 
/********************************************************************
	Copyright by 97Dept. of Wholewise, 2001-07-01
	File Name: TOCIQuery.cpp
	Created Date: 2001/12/19:13:42
	Author:	CHEN GONG JIAN		Create Version: 0.0.01
	Create Version Date: 2001/12/19

	purpose:	
	----------------------------------------------------------------------
		Author:		HU GUO BIAO		Modified Version:		
		Modified Version Date:	2001/12/19:13:43
		Function Changed:
			cxx: Warning: TOCIQuery.cpp, line 579: statement is unreachable
									break;
			------------------------^
			cxx: Warning: TOCIQuery.cpp, line 971: use of "=" where "==" may have been inted
					else if (db->errorNo == OCI_SUCCESS)
			-----------------^
		
		Author:		Chengj		
		Modified Version Date: 2002/01/19
		purpose:	
			1. Date Support
			2. Blob Support
			3. Prefetch data to improve performance
			4. Memory leak eliminate(use Rational Purify testing tool)
		----------------------------------------
		Author:		Yuzj		
		Modified Version Date: 2002/03/27
		purpose:	
			Modify function name:TOCIQuery::getParamsDef()

			error:
				该函数的算法存在问题,在提取SQL语句的参数时
			判断for循环结束的条件有可能存在内存越界的问题,
			在一定条件下会造成内存越界.

			任务单号:DX20020327003
		4)  Modified By Name of Programmer	Yeyh
		Date YYYY-MM-DD			2002-06-27
		Current Version: 0.0.0.5	Task nbr: IBSS20020518002
		Function Changed:
			setParameter(char *paramName, long paramValue, bool isOutput)
			修改long类型参数对应的oracle数据类型
		Other changes:
		
		5)  Modified By Name of Programmer	黄锦添 
		Date YYYY-MM-DD			2002-07-09
		Current Version: 0.0.0.6	Task nbr: IBSS20020709015
		Function Changed: 
			在TOCIQuery类中增加成员bExecuteFlag
			TOCIQuery::TOCIQuery()		将bExecuteFlag置为false
			TOCIQuery::Execute()		将bExecuteFlag置为true
			TOCIQuery::Commit() 		将bExecuteFlag置为false
			TOCIQuery::Rollback()		将bExecuteFlag置为false
			TOCIQuery::~TOCIQuery()		判断bExecuteFlag,若为true则向log中记录可能锁表 
		Other changes:
			包含文件userlog.h 
		.............

		6）Modified By Name of Programmer	YUZJ		
		Date YYYY-MM-DD			2002-07-09
		Current Version: 0.0.0.7	Task nbr: IBSS20020718003
		Function Changed: 
			getFieldsDef函数中有一处size赋值有误
		Other changes:

		7)  Modified By Name of Programmer	Unknow
		Current Version: 0.0.0.8	

		8)  Modified By Name of Programmer	RuanYJ
		Date YYYY-MM-DD			2004-03-18
		Current Version: 0.0.0.9	
		Function Changed: 
			新增 SetLongRawParameter (char *, char *, bool);
		9)  Modified By Name of Programmer 	Linjx
		Date YYYY-MM-DD			2005-06-07
		Current Version: 0.0.0.10	
		Function Changed: 
			新增	SetParameter(char *, void*, int iLen, bool);　对blob 数据单条入库
			新增	SetParamArray	(char *, void ** ,int ,int ,  bool);　对blob 数据多条入库	
			修改	asBlobBuffer	释放申请的空间	
			修改	在select blob 型数据时　fPrefetchRows=1;　BLOB 目前只能以一条一条的方式fetch　
					以后应该可以批量fetch
*********************************************************************/
#include <stdio.h>
#include <oci.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <oratypes.h>
#include <ocidfn.h>
#include <ocidem.h>
#include <ociapr.h>
#include "TOCIQuery.h"
//Added begin at ver : 0.0.0.6
//#define __DEBUG__

#ifdef __DEBUG__
#include <userlog.h>
#endif
//Added end at ver : 0.0.0.6
#include"MBC_ABM.h"

TOCIException::TOCIException(sword errNumb, OCIError *hErr, char const *cat, 
char const *sql)
{
	int nLen;
	sb4 errcode;

	nLen = strlen(cat);
	nLen = (nLen >= MAX_ERR_CAT_LENGTH)? MAX_ERR_CAT_LENGTH : nLen;
	strncpy(errCategory,cat,nLen);
	errCategory[nLen] = '\0';
	
	nLen = strlen(sql);
	nLen = nLen >= MAX_SQLSTMT_LENGTH ? MAX_SQLSTMT_LENGTH : nLen;
	strncpy(errSQL,sql,nLen);
	errSQL[nLen] = '\0';
	
	errNo = errNumb;

	(void)OCIErrorGet ((dvoid *) hErr, (ub4) 1, (text *) NULL, &errcode,
			errMessage, (ub4)sizeof(errMessage)-1, (ub4) OCI_HTYPE_ERROR);

	m_iErrCode = errcode;

}

int TOCIException::getErrCode() const
{
	return m_iErrCode;
}

TOCIException::TOCIException(const char *sql, const char* errFormat, ...)
{
	int nLen;
	nLen = strlen(sql);
	nLen = (nLen >= MAX_SQLSTMT_LENGTH) ? MAX_SQLSTMT_LENGTH :nLen;
	strncpy(errSQL,sql,nLen);
	errSQL[nLen] = '\0';

	va_list ap;
	va_start(ap, errFormat);
	vsprintf((char *)errMessage, errFormat, ap);	
	va_end(ap);

	m_iErrCode = 0;
}

TOCIException::TOCIException(int errNumb,  const char* errFormat, ...)
{
	m_iErrCode = errNumb ;
	va_list ap;
	va_start(ap, errFormat);
	vsprintf((char *)errMessage, errFormat, ap);	
	va_end(ap);
}

TOCIException::~TOCIException()
{
}

char *TOCIException::getErrSrc() const
{
	return( (char *)errSQL );
}

char *TOCIException::getErrMsg() const
{	
	return( (char *)errMessage);
}

//比较2个字符串是否相同(不考虑大小写)
bool inline compareStrNoCase(const char *ori, const char *des)
{
	int j,nLen1,nLen2;
	bool sameChar;
	nLen1 = strlen(ori);
	nLen2 = strlen(des);
	if (nLen1!=nLen2) return false;
	sameChar = true;
	for (j=0; j<nLen1; j++)
		sameChar = sameChar && ( toupper(ori[j]) == toupper(des[j]) );
	return sameChar;
}

/********* TConnection implementation *********/
TOCIDatabase::TOCIDatabase()
{
	sword errorNo;
	
	errorNo = OCIInitialize((ub4) OCI_DEFAULT|OCI_OBJECT,0, 0,0,0 );
	errorNo = errorNo + OCIEnvInit( (OCIEnv **) &hEnv, (ub4) OCI_DEFAULT,(size_t) 0, (dvoid **) 0 );
	errorNo = errorNo + OCIHandleAlloc( (dvoid *) hEnv, (dvoid **) &hDBSvc,(ub4) OCI_HTYPE_SVCCTX,(size_t) 0, (dvoid **) 0);
	errorNo = errorNo + OCIHandleAlloc( (dvoid *) hEnv, (dvoid **) &hDBErr,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);	
	errorNo = errorNo + OCIHandleAlloc( (dvoid *) hEnv, (dvoid **) &hSvr,(ub4) OCI_HTYPE_SERVER,(size_t) 0, (dvoid **) 0);
		
	if ( errorNo != 0 )
		throw TOCIException( "TOCIDatabase()", ERR_DB_INIT, __LINE__);
	fConnected = false;
	usr = NULL;
	pwd = NULL;
	tns = NULL;
}

TOCIDatabase::~TOCIDatabase()
{
	delete[] usr;
	delete[] tns;
	delete[] pwd;
	
	if (fConnected) 
		OCIServerDetach(hSvr, hDBErr, OCI_DEFAULT );
	
	OCIHandleFree(hSvr, OCI_HTYPE_SERVER);		
	OCIHandleFree(hDBSvc, OCI_HTYPE_SVCCTX);
	OCIHandleFree(hDBErr,OCI_HTYPE_ERROR);
	OCIHandleFree(hEnv,OCI_HTYPE_ENV);
}

void TOCIDatabase::setLogin(char *user, char *password, char *tnsString)
{
	if (fConnected)
		throw TOCIException("setLogin()", ERR_SET_LOGIN , __LINE__);
	
	int nLen;
	//保存外部传递的参数
	if ( usr != NULL) 
		delete[] usr;
	if (pwd != NULL)
		delete[] pwd;
	if (tns != NULL)
		delete[] tns;

	//保存外部传递的参数
	if (user)
	{
		nLen = strlen(user);
		usr = new char[nLen+1];
		strncpy(usr,user,nLen);
		usr[nLen] = '\0';
	}
	else
	{
		nLen = 0;
		usr = new char[1];
		usr[0] = '\0';
	}

	if (password)
	{
		nLen = strlen(password);
		pwd = new char[nLen+1];
		strncpy(pwd,password,nLen);
		pwd[nLen] = '\0';
	}
	else
	{
		nLen = 0;
		pwd = new char[1];
		pwd[0] = '\0';
	}
	
	if (tnsString)
	{
		nLen = strlen(tnsString);
		tns = new char[nLen+1];
		strncpy(tns,tnsString,nLen);
		tns[nLen] = '\0';
	}
	else	
	{
		nLen = 0;
		tns = new char[1];
		tns[0] = '\0';
	}
}

void TOCIDatabase::checkError()
{
	if (fErrorNo != OCI_SUCCESS) 
		throw TOCIException(fErrorNo, hDBErr, "Oracle OCI Call", "OCIDatabase");
	
}


bool TOCIDatabase::connect()
{
	sword errorNo;

	if (fConnected)
		return true;
	
	if ( (usr == NULL) || (tns==NULL) )
		throw TOCIException("connect()", ERR_CONNECT_NO_LOGIN_INFO, __LINE__);
	
	errorNo = OCIServerAttach(hSvr, hDBErr, (text *)tns, strlen(tns), 0);
	if (errorNo != OCI_SUCCESS)	
			   
		throw TOCIException(MBC_DB_LINK_FAIL, "数据库连接失败。请检查配置TNS配置[tns=%s]",tns); 	   
		//throw TOCIException(errorNo, hDBErr, "connect()", "try to connect Server");
	
	//modified: 2003.1
	fErrorNo = OCIHandleAlloc(hEnv, (dvoid **) &hUser,(ub4) OCI_HTYPE_SESSION,(size_t) 0, (dvoid **) 0);
	checkError();

	fErrorNo = OCIHandleAlloc(hEnv, (dvoid **)&hDBSvc, OCI_HTYPE_SVCCTX,0, 0);
	checkError();

	fErrorNo = OCIAttrSet (hDBSvc, OCI_HTYPE_SVCCTX, hSvr, 0, OCI_ATTR_SERVER, hDBErr);
	checkError();

	/* set the username/password in user handle */
	OCIAttrSet(hUser, OCI_HTYPE_SESSION, usr, strlen(usr),OCI_ATTR_USERNAME, hDBErr);
	OCIAttrSet(hUser, OCI_HTYPE_SESSION, pwd, strlen(pwd),OCI_ATTR_PASSWORD, hDBErr);

	// Set the Authentication handle in the service handle
	fErrorNo = OCIAttrSet(hDBSvc, OCI_HTYPE_SVCCTX, hUser, 0, OCI_ATTR_SESSION, hDBErr);

	fErrorNo= OCISessionBegin (hDBSvc, hDBErr, hUser, OCI_CRED_RDBMS, OCI_DEFAULT);
	if(fErrorNo!=OCI_SUCCESS)
	 throw TOCIException(MBC_DB_SERV_PWD_ERROR,"数据库连接用户名密码错误。请检查配置[Usr=%s,Pwd=%s]",usr,pwd);	
		   
	checkError();
	//Set Trans:
	//OCIAttrSet(hDBSvc, OCI_HTYPE_SVCCTX, hTrans, 0, OCI_ATTR_TRANS, hErr);

	return (fConnected = (errorNo == OCI_SUCCESS));
}

bool TOCIDatabase::connect(char *inUsr, char *inPwd, char *inTns)
{
	setLogin(inUsr, inPwd, inTns);
	return connect();
}

void TOCIDatabase::disconnect()
{
	if (!fConnected) return;
	OCISessionEnd (hDBSvc, hDBErr, hUser, OCI_DEFAULT);
	dword errorNo = OCIServerDetach(hSvr, hDBErr, OCI_DEFAULT );
	if (errorNo != OCI_SUCCESS)
		  throw TOCIException(MBC_DB_LINK_FAIL, "脱离数据库失败，检查数据库系统或是网络\n"); 	
			//throw TOCIException(errorNo, hDBErr,"Disconnect()", "OCIServerDetatch error");
	fConnected = false;
}

void TOCIDatabase::commit()
{
    OCITransCommit(hDBSvc, hDBErr, OCI_DEFAULT);
}

void TOCIDatabase::rollback()
{
	OCITransRollback(hDBSvc, hDBErr, OCI_DEFAULT);
}

TOCIField::TOCIField()
{
	//初始化列信息,有部分的初始化信息在Describe中进行
	name = NULL;
	hBlob = NULL;
	hDefine = (OCIDefine *) 0; ;
	fDataBuf = NULL;
	fDataIndicator = NULL;
	fParentQuery = NULL;

	fReturnDataLen = 0;
	size = 0;
	precision = 0;
	scale = 0;
	size = 0;
	iBlobBufCnt=0;
};

TOCIField::~TOCIField()
{
	if (fDataIndicator != NULL)
		delete[] fDataIndicator;
	if (name != NULL)
		delete[] name; 
	if (fDataBuf != NULL)
		delete[] fDataBuf;
	if (type == SQLT_BLOB)
		OCIDescriptorFree((dvoid *)hBlob, (ub4) OCI_DTYPE_LOB);
}

char* TOCIField::asString()
{
	int year, month, day, hour, minute, second;
	static char intStr[100];
	int status;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asString()", name);

	if ( isNULL() )
	{
		sprintf((char *)fStrBuffer, NULL_STRING);
		return (char *)fStrBuffer;
	}

	switch ( this->type )
	{
	case DATE_TYPE:
		this->asDateTimeInternal(year, month, day, hour, minute, second);
		sprintf((char *)fStrBuffer,"%04d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second);
		return (char *)fStrBuffer;
	case INT_TYPE:
		long intValue;
		if ( (status=OCINumberToInt(fParentQuery->hErr, (OCINumber *)(fDataBuf + (size+1) * fParentQuery->fCurrRow ),sizeof(intValue), OCI_NUMBER_SIGNED,&intValue))!= OCI_SUCCESS)
		{
			fParentQuery->checkError();
		}
		
		sprintf(intStr, "%ld", intValue);
		return intStr;
	case FLOAT_TYPE:
		//int status;
		double floatValue;
		if ( (status=OCINumberToReal(fParentQuery->hErr, (OCINumber *)(fDataBuf + (size+1) * fParentQuery->fCurrRow ),sizeof(floatValue), &floatValue))!= OCI_SUCCESS)
		{
			fParentQuery->checkError();
		}

		sprintf(intStr, "%f", floatValue);
		return intStr;		
		//return((char *)(fDataBuf + (size+1) * fParentQuery->fCurrRow));
	case STRING_TYPE:
	case ROWID_TYPE:
		return((char *)(fDataBuf + (size+1) * fParentQuery->fCurrRow));
	case SQLT_BLOB:
		sprintf((char *)fStrBuffer, "BLOB...");
		return (char *)fStrBuffer;
	default:
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asString()");
	}
}

bool TOCIField::isNULL()
{
	return (fDataIndicator[fParentQuery->fCurrRow]==-1);
}

void  TOCIField::asBlobFile(const char *fileName)
{
	ub4 offset = 1;
	ub1 buf[MAX_LOB_BUFFER_LENGTH];
	ub4 nActual = 0;	//实际读取数
	ub4 nTry = 0;		//试图读取数
	ub4 totalSize = 0;
	FILE *fileHandle;
	ub4 lobLength;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asBlobFile()", name);

	if (type != SQLT_BLOB)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asLobFile()");

	fileHandle = fopen( fileName, (const char *) "wb");
	fseek(fileHandle, 0, 0);
	/* set amount to be read per iteration */
	nTry = nActual = MAX_LOB_BUFFER_LENGTH;
	OCILobGetLength(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, &lobLength);

	while (nActual)
	{
		fParentQuery->fErrorNo = OCILobRead(fParentQuery->db->hDBSvc, fParentQuery->hErr, 
			hBlob, &nActual, (ub4)offset, (dvoid *) buf, (ub4) nTry, (dvoid *)0, 
			(sb4 (*)(dvoid *, CONST dvoid *, ub4, ub1)) 0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		fParentQuery->checkError();	  
		
		if (nActual<=0) break;
		
		totalSize += nActual;
		fwrite((void *)buf, (size_t)nActual, (size_t)1, fileHandle);
		offset += nActual;
	}
	fclose(fileHandle);
}

void  TOCIField::asBlobBuffer(unsigned char* &buf, unsigned int *lobLength)
{
	ub1 innerBuf[MAX_LOB_BUFFER_LENGTH];
	ub4 remainder, nActual, nTry;
	ub4  flushedAmount = 0, offset = 1;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asBlobBuffer()", name);

	if (type != SQLT_BLOB)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asLobBuffer()");

	OCILobGetLength(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, &remainder);
	*lobLength = nActual = nTry = remainder;

	try
	{
		if (iBlobBufCnt<remainder){
			if (buf)
				delete []buf;
			buf = new unsigned char[sizeof(ub1) * remainder];
			iBlobBufCnt=remainder;
		}
	}
	catch (...)
	{
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NOMORE_MEMORY_BE_ALLOCATED, "asBlobBuffer()", __LINE__);
	}

	nTry = nActual = MAX_LOB_BUFFER_LENGTH;
	while (remainder)
	{		
		fParentQuery->fErrorNo = OCILobRead(fParentQuery->db->hDBSvc, fParentQuery->hErr, 
			hBlob, &nActual, (ub4)offset, (dvoid *)innerBuf, (ub4) nTry, (dvoid *)0, 
			(sb4 (*)(dvoid *, CONST dvoid *, ub4, ub1)) 0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		fParentQuery->checkError();	  
		memcpy( (buf) + offset -1, innerBuf, nActual);
		if (nActual<=0) break;
		
		offset += nActual;
		remainder -= nActual;
	}
}

void  TOCIField::loadFromFile(const char *fileName)
{
	ub4 remainder, nActual, nTry, offset = 1;//从文件中读取的剩余数据量
	ub1 buf[MAX_LOB_BUFFER_LENGTH];
	ub4 LobLength;
	ub4  flushedAmount = 0;
	FILE *fileHandle ;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "LoadFromFile()", name);

	if (type != SQLT_BLOB)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "LoadFromFile()");

	if( (fileHandle = fopen(fileName, (const char *) "rb")) == NULL )
		throw TOCIException(fParentQuery->fSqlStmt, ERR_FILE_IO, "LoadFromFile()", fileName);
	
	fseek(fileHandle,0,SEEK_END);
	remainder = ftell(fileHandle);
	fseek(fileHandle, 0, 0);

	fParentQuery->fErrorNo = OCILobGetLength(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, &LobLength);
	fParentQuery->checkError();

	fParentQuery->fErrorNo = OCILobTrim(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, 0);
	fParentQuery->checkError();

	/* enable the BLOB locator for buffering operations */
	fParentQuery->fErrorNo = OCILobEnableBuffering(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob);
	fParentQuery->checkError();

	while ( (remainder > 0) && !feof(fileHandle))
	{
		nActual = nTry = (remainder > MAX_LOB_BUFFER_LENGTH) ? MAX_LOB_BUFFER_LENGTH : remainder;
		
		if (fread((void *)buf, (size_t)nTry, (size_t)1, fileHandle) != (size_t)1)
			throw TOCIException(fParentQuery->fSqlStmt, ERR_MEM_BUFFER_IO, name, fileName, __LINE__);
		
		fParentQuery->fErrorNo = OCILobWrite(fParentQuery->db->hDBSvc, fParentQuery->hErr, 
			hBlob, &nActual, offset, (dvoid *) buf, (ub4) nTry, OCI_ONE_PIECE, (dvoid *)0,
			(sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,	(ub2) 0, (ub1) SQLCS_IMPLICIT);
		if ( fParentQuery->fErrorNo != OCI_SUCCESS) 
		{
			fclose(fileHandle);
			fParentQuery->checkError();
		}

		flushedAmount += nTry;
		remainder -= nTry;
		offset += nTry;
		//incase the internal buffer is not big enough for the lob , flush the buffer content to db after some interval:
		if (flushedAmount >= LOB_FLUSH_BUFFER_SIZE)
		{
			flushedAmount = 0;
			fParentQuery->fErrorNo = OCILobFlushBuffer(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, OCI_LOB_BUFFER_NOFREE);
			fParentQuery->checkError();	
		}
	}

	if ( flushedAmount )
	{
			fParentQuery->fErrorNo = OCILobFlushBuffer(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, OCI_LOB_BUFFER_NOFREE);
			fParentQuery->checkError();	
	}
	fclose(fileHandle);
}

void  TOCIField::loadFromBuffer(unsigned char *buf, unsigned int bufLength)
{
	ub1 innerBuf[MAX_LOB_BUFFER_LENGTH];

	ub4 remainder, nActual, nTry;
	ub4  flushedAmount = 0, offset = 1;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "LoadFromBuffer()", name);

	if (type != SQLT_BLOB)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "LoadFromBuffer()");
	
	fParentQuery->fErrorNo = OCILobTrim(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, 0);
	fParentQuery->checkError();
	remainder = bufLength;

	/* enable the BLOB locator for buffering operations */
	fParentQuery->fErrorNo = OCILobEnableBuffering(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob);
	fParentQuery->checkError();

	while (remainder > 0)
	{
		nActual = nTry = (remainder > MAX_LOB_BUFFER_LENGTH) ? MAX_LOB_BUFFER_LENGTH : remainder;
		
		memcpy(innerBuf, buf + offset-1, nActual);

		fParentQuery->fErrorNo = OCILobWrite(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob,
			&nActual, offset, (dvoid *)innerBuf, (ub4)nTry, OCI_ONE_PIECE, (dvoid *)0,
			(sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,	(ub2) 0, (ub1) SQLCS_IMPLICIT);
		fParentQuery->checkError();
		
		flushedAmount += nTry;
		remainder -= nTry;
		offset += nTry;

		if (flushedAmount >= LOB_FLUSH_BUFFER_SIZE)
		{
			flushedAmount = 0;
			fParentQuery->fErrorNo = OCILobFlushBuffer(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, OCI_LOB_BUFFER_NOFREE);
			fParentQuery->checkError();	
		}
	}

	if ( flushedAmount )
	{
			fParentQuery->fErrorNo = OCILobFlushBuffer(fParentQuery->db->hDBSvc, fParentQuery->hErr, hBlob, OCI_LOB_BUFFER_NOFREE);
			fParentQuery->checkError();	
	}
}

double TOCIField::asFloat()
{
	double iRet; 
	int status;
	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asFloat()", name);

	if ( isNULL() )
		return 0;
	if ( (type == FLOAT_TYPE) || ( type == INT_TYPE) ){
		if ( (status=OCINumberToReal(fParentQuery->hErr, (OCINumber *)(fDataBuf + (size+1) * fParentQuery->fCurrRow ),sizeof(iRet), &iRet))!= OCI_SUCCESS)
		{
			fParentQuery->checkError();
		}
		return iRet;
	}
	
	else	throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asFloat()");
}

char* TOCIField::asDateTimeString()
{
	int year, month, day, hour, minute, second;

	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asDateTimeString()", name);

	if (type != DATE_TYPE)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asDateTimeString()");
	else
	{
		this->asDateTimeInternal(year, month, day, hour, minute, second);
		if ( year == 0 )
				sprintf( (char *)fStrBuffer, NULL_STRING);
		else	sprintf( (char *)fStrBuffer,"%d-%d-%d %d:%d:%d", year, month, day, hour, minute,second);
		return (char *)fStrBuffer;
	}
}

void	TOCIField::asDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second)
{
	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asDateTime()", name);

	if (type != DATE_TYPE)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asDateTime()");
	else this->asDateTimeInternal(year, month, day, hour, minute, second);
}

void	TOCIField::asDateTimeInternal(int &year, int &month, int &day, int &hour, int &minute, int &second)
{
	unsigned char cc,yy,mm,dd,hh,mi,ss;
	ub1	*data;

	if ( isNULL() )
	{
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
		return;
	}
	data = fDataBuf + 7 * (fParentQuery->fCurrRow);
	cc=data[0];	
	yy=data[1]; 
	mm=data[2]; 
	dd=data[3]; 
	hh=data[4]-1; 
	mi=data[5]-1; 
	ss=data[6]-1; 
	cc=(unsigned char)abs(cc-100);
	yy=(unsigned char)abs(yy-100);
	year = (unsigned int)cc*100 + (unsigned int) yy;
	month = mm;
	day = dd;
	hour = hh;
	minute = mi;
	second = ss;
}

int TOCIField::asInteger()
{	
	int iRet=0,status;
	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asInteger()", name);

	if (type != INT_TYPE && type != FLOAT_TYPE)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asInteger()");
	else {
		if ( (status=OCINumberToInt(fParentQuery->hErr, (OCINumber *)(fDataBuf + (size+1) * fParentQuery->fCurrRow ),sizeof(iRet), OCI_NUMBER_SIGNED,&iRet))!= OCI_SUCCESS)
		{
			fParentQuery->checkError();
		}
		return iRet;
	}
}

long TOCIField::asLong()
{	
	long iRet=0,status;
	if (fParentQuery->fBof || fParentQuery->fEof)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_NO_DATASET, "asLong()", name);

	if (type != INT_TYPE && type != FLOAT_TYPE)
		throw TOCIException(fParentQuery->fSqlStmt, ERR_DATA_TYPE_CONVERT, name, type, "asLong()");
	else {
		if ( (status=OCINumberToInt(fParentQuery->hErr, (OCINumber *)(fDataBuf + (size+1) * fParentQuery->fCurrRow ),sizeof(iRet), OCI_NUMBER_SIGNED, &iRet))!= OCI_SUCCESS)
		{
			fParentQuery->checkError();
		}
		return iRet;
	}
}

/*********** TOCIQuery Implementation************/
TOCIQuery::TOCIQuery(TOCIDatabase *oradb)
{
	if (! oradb->fConnected)
		throw TOCIException("", ERR_GENERAL, "TOCIQuery(TOCIDatabase &db): Can not declare a TOCIQuery when the database is not connected");

	fFetched = 0;
	fPrefetchRows = 1;
	fCurrRow = 0;
	fTotalRowsFetched = 0;
	fBof = false;
	fEof = false;
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    bExecuteFlag = false;
#endif
//Added end at ver : 0.0.0.6
	nTransTimes = 0;
	db = oradb;
	fActivated = false;
	fFieldCount = 0;
	fParamCount = 0;

	fSqlStmt = NULL;
	paramList = NULL;
	fieldList = NULL;


	fErrorNo = OCIHandleAlloc(db->hEnv, (dvoid **) &hErr,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);
	checkError();

	fErrorNo = OCIHandleAlloc(db->hEnv, (dvoid **)&hStmt, OCI_HTYPE_STMT, 0, 0);
	checkError();
	
	//fErrorNo = OCIHandleAlloc(db->hEnv, (dvoid **)&hTrans, OCI_HTYPE_TRANS, 0, 0);
	//checkError();

}

TOCIQuery::TOCIQuery(TOCIDatabase *oradb,TOCISession *session)
{
	if (! session->m_bActive)
		throw TOCIException("", ERR_GENERAL, "TOCIQuery(TOCIDatabase &db): Can not declare a TOCIQuery when the database is not connected");

	fFetched = 0;
	fPrefetchRows = 1;
	fCurrRow = 0;
	fTotalRowsFetched = 0;
	fBof = false;
	fEof = false;
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    bExecuteFlag = false;
#endif
//Added end at ver : 0.0.0.6
	nTransTimes = 0;
	db = oradb;
	fActivated = false;
	fFieldCount = 0;
	fParamCount = 0;

	fSqlStmt = NULL;
	paramList = NULL;
	fieldList = NULL;

/*	hUser = session->m_hSession;
*/
	hErr = session->m_hError;
	/*hSvc = session->m_hSrvCtx;
	*/
	fErrorNo = OCIHandleAlloc(db->hEnv, (dvoid **)&hStmt, OCI_HTYPE_STMT, 0, 0);
	checkError();
	
	//fErrorNo = OCIHandleAlloc(db->hEnv, (dvoid **)&hTrans, OCI_HTYPE_TRANS, 0, 0);
	//checkError();	
}


TOCIQuery::~TOCIQuery()
{
	if (fSqlStmt != NULL)
		delete[] fSqlStmt;
	if (fParamCount >0)
		delete[] paramList;
	if (fFieldCount >0)
		delete[] fieldList;
	if (nTransTimes)
	{
		//fErrorNo = OCITransRollback(db->hDBSvc, hErr, OCI_DEFAULT);
		//checkError();
	}
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    if(bExecuteFlag)
        userlog("TOCIQuery执行了Execute()但是没有提交或回滚,可能造成锁表。"); 
#endif
//Added end at ver : 0.0.0.6
/*
	OCISessionEnd (hSvc, hErr, hUser, OCI_DEFAULT);
*/
	OCIHandleFree(hStmt, OCI_HTYPE_STMT);
	//OCIHandleFree(hTrans,OCI_HTYPE_TRANS);
	/*OCIHandleFree(hSvc, OCI_HTYPE_SVCCTX);
	OCIHandleFree(hUser,OCI_HTYPE_SESSION);
	*/
	OCIHandleFree(hErr,OCI_HTYPE_ERROR);
}

int TOCIQuery::executeDebug(int iters)
{
    sb4 errcode;
    text errbuf[MAX_ERRMSG_LENGTH-1];
    bool exeResult = false;    //Added begin at ver : 0.0.0.6
#ifdef __DEBUG__   
 bExecuteFlag = true;
#endif    //Added end at ver : 0.0.0.6
    if (fSqlStmt == NULL)
        throw TOCIException("", ERR_GENERAL, "Execute(): sql statement is not presented"); 
   if   (this->fStmtType == OCI_STMT_SELECT)
        throw TOCIException( fSqlStmt, ERR_GENERAL, "Execute(): Can't Execute a select statement.");
   OCIError* errhp2= 0 ;
OCIHandleAlloc(db->hEnv, (dvoid **) &errhp2,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);
	 fErrorNo = OCIStmtExecute(db->hDBSvc, hStmt, errhp2, (ub4)iters, (ub4)0, 0, 0, OCI_BATCH_ERRORS); 
   unsigned nField = 0;  
   OCIAttrGet((dvoid*)hStmt, OCI_HTYPE_STMT, (dvoid *)&nField,(ub4*)0,OCI_ATTR_NUM_DML_ERRORS,errhp2); 
   if (fErrorNo!= OCI_SUCCESS)
    {
        int m_RowOff = 0;

        OCIError *errhndl=0;
       // OCIError* errhp2= 0 ;
        OCIError* errhp1 = 0;

 //       OCIHandleAlloc(db->hEnv, (dvoid **) &errhp2,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);
        OCIHandleAlloc(db->hEnv, (dvoid **) &errhndl,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);
        OCIHandleAlloc(db->hEnv, (dvoid **) &errhp1,(ub4) OCI_HTYPE_ERROR,(size_t) 0, (dvoid **) 0);

        int j =0 ;
        int *pInOff =0;
        int nRet =OCIParamGet(errhp2, OCI_HTYPE_ERROR, errhp2, (dvoid **)&errhndl, 0);
        nRet = OCIAttrGet(errhndl, OCI_HTYPE_ERROR, (dvoid *)&m_RowOff, 0,
                OCI_ATTR_DML_ROW_OFFSET,errhp1);
        j = m_RowOff;
        OCIHandleFree(errhndl,OCI_HTYPE_ERROR);
        OCIHandleFree(errhp2,OCI_HTYPE_ERROR);
        OCIHandleFree(errhp1,OCI_HTYPE_ERROR);
        return j;
    }else  
      return 0;
}

void TOCIQuery::close()
{
	if (! fActivated)
		return;
	
	if (fSqlStmt != NULL)
	{
		delete[] fSqlStmt;
		fSqlStmt = NULL;
	}
	if (fParamCount > 0)
	{
		delete[] paramList;
		paramList = NULL;
	}
	if (fFieldCount > 0)
	{
		delete[] fieldList;
		fieldList = NULL;
	}
	
	fFieldCount = 0;
	fParamCount = 0;
	fActivated = false;

	fFetched = 0;
	fPrefetchRows = PREFETCH_ROWS;
	fCurrRow = 0;
	fTotalRowsFetched = 0;
}

bool TOCIQuery::commit()
{
	bool exeSuccess = false;
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    bExecuteFlag = false;
#endif
//Added end at ver : 0.0.0.6
	fErrorNo = OCITransCommit(db->hDBSvc, hErr, OCI_DEFAULT);
	checkError();
	if (fErrorNo == OCI_SUCCESS)
		nTransTimes = 0;
	return (fErrorNo == OCI_SUCCESS);	
}

bool TOCIQuery::rollback()
{
	bool exeSuccess = false;
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    bExecuteFlag = false;
#endif
//Added end at ver : 0.0.0.6
	fErrorNo = OCITransRollback(db->hDBSvc, hErr, OCI_DEFAULT);
	if (fErrorNo == OCI_SUCCESS)
	{
		nTransTimes = 0;
		exeSuccess = true;
	}
	else
		checkError();
	return exeSuccess;
}

void TOCIQuery::getFieldsDef()
{
	char temp[20];
	
	TOCIField *pCurrField;

	OCIParam	*param = (OCIParam *)0;
	ub4			counter;			//计数器，在循环语句中实用
	ub4			nColumnCount;	//在分析sqlstmt后，获得的列数目
	//以下参数的长度参见"OCI Programmer's" Guide P208 , 6-10
	text   *columnName;				//字段名称
	ub4	 columnNameLength,j;		//字段名称长度 unsigned int
	ub2	 innerDataSize;			//数据长度 unsigned short
	ub2	 innerDataType;			//Oracle 内部数据类型 signed short
	ub2	 innerPrecision;			//包括小数点的总位数, ub1 is a bug in documentation?
	sb1    innerScale;				//小数点个数
	ub1    innerIsNULL;				//是否允许为空值
	
	if(fStmtType==OCI_STMT_SELECT)
			fErrorNo = OCIStmtExecute(db->hDBSvc, hStmt, hErr, (ub4)0, (ub4)0, 0, 0, OCI_DEFAULT);
	else	fErrorNo = OCIStmtExecute(db->hDBSvc, hStmt, hErr, (ub4)1, (ub4)0, 0, 0, OCI_DEFAULT);
	checkError(); 
	//在Execute后，可以获得列的个数和行的个数?
	//如果没有为select语句的返回值定义输出缓冲区，则hErr后的参数iters应当设置为0而不是>0;如果是非SELECT语句，此值>0;
	fErrorNo = OCIAttrGet((dvoid *)hStmt, (ub4)OCI_HTYPE_STMT, (dvoid *)&nColumnCount, (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, hErr);
	checkError();

	if (fFieldCount >0 )
	{
		delete[] fieldList;
		// paramList = NULL;
		fieldList = NULL;
	}
	fieldList = new TOCIField[nColumnCount];
	fFieldCount = nColumnCount;
	
	for(counter=1; counter<=nColumnCount ; counter ++)
	{
		fErrorNo = OCIParamGet(hStmt, OCI_HTYPE_STMT, hErr, (dvoid **)&param, counter);
		checkError();

		// column name and column name length
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid**)&columnName,(ub4 *)&columnNameLength, OCI_ATTR_NAME, hErr);
		checkError();

		// data length 
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid *)&innerDataSize, (ub4 *)0, OCI_ATTR_DATA_SIZE, hErr);
		checkError();

		// precision 
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid *)&innerPrecision, (ub4 *)0, OCI_ATTR_PRECISION, hErr);
		checkError();

		// scale 
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid *)&innerScale, (ub4 *)0, OCI_ATTR_SCALE, hErr);
		checkError();

		// isNULL
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid *)&innerIsNULL, (ub4 *)0, OCI_ATTR_IS_NULL, hErr);
		checkError();

		// data type:
		fErrorNo = OCIAttrGet((dvoid*)param, OCI_DTYPE_PARAM, (dvoid *)&innerDataType, (ub4 *)0, OCI_ATTR_DATA_TYPE, hErr);
		checkError();

		pCurrField = &fieldList[counter-1];

		pCurrField->name = new char[columnNameLength+1];
		if (pCurrField->name == NULL)
			throw TOCIException(fSqlStmt, ERR_NOMORE_MEMORY_BE_ALLOCATED, "getFieldsDef()", __LINE__);
		for (j=0; j<columnNameLength; j++)
			pCurrField->name[j] = columnName[j];
		pCurrField->name[columnNameLength] = '\0';

		pCurrField->nullable = innerIsNULL>0;

		pCurrField->type = innerDataType; //初始化为内部类型，用于错误返回
		pCurrField->fParentQuery = this;
		pCurrField->fDataIndicator = new sb2[fPrefetchRows];

		switch 	(innerDataType)
		{
		case SQLT_NUM://NUMBER_TYPE: 
			//Modify begin version：0.0.0.7
			if (! innerDataSize)
				pCurrField->size = 255;
			else	pCurrField->size = innerDataSize;
			//Modify end version:0.0.0.7
			pCurrField->precision = innerPrecision;
			pCurrField->scale = innerScale;
			//预先定义字符缓冲区，用于接收数值到字符转换的结果
			pCurrField->fDataBuf = new ub1[fPrefetchRows * (pCurrField->size+1)]; 
			if (innerScale == 0) //没有小数点，为整数
					pCurrField->type = INT_TYPE;
			else	pCurrField->type = FLOAT_TYPE;
			
			//绑定输出数据到缓冲区(整数类型绑定到整数缓冲区)
			fErrorNo = OCIDefineByPos(hStmt, &(pCurrField->hDefine), hErr, counter,
				(dvoid *)pCurrField->fDataBuf, pCurrField->size + 1, SQLT_VNU,
				(dvoid *)pCurrField->fDataIndicator, (ub2 *)0 , (ub2 *) 0, OCI_DEFAULT);
			checkError();
			break;
		case SQLT_DAT://DATE_TYPE:
			pCurrField->type = DATE_TYPE;
			pCurrField->size = 7;
			//绑定输出数据到缓冲区(date类型也是绑定到字符串缓冲区)
			pCurrField->fDataBuf = new ub1[fPrefetchRows *(pCurrField->size)];
			fErrorNo = OCIDefineByPos(hStmt, &(pCurrField->hDefine), hErr, counter,
				pCurrField->fDataBuf, 7, SQLT_DAT,
				(dvoid *)pCurrField->fDataIndicator, (ub2 *)0, (ub2 *) 0, OCI_DEFAULT);
			checkError();
			break;
		case SQLT_CHR: case SQLT_AFC: //DATA_TYPE_CHAR: case VARCHAR2_TYPE: 
			pCurrField->type = STRING_TYPE;
			/*if (innerDataSize>MAX_STRING_VALUE_LENGTH)
				pCurrField->size = MAX_STRING_VALUE_LENGTH;
			else pCurrField->size = innerDataSize;*/
			pCurrField->size = innerDataSize;  //以系统取得的字段长度作为数据的长度大小
			//绑定输出数据到缓冲区
			pCurrField->fDataBuf = new ub1[fPrefetchRows * (pCurrField->size+1)];
			fErrorNo = OCIDefineByPos(hStmt, &(pCurrField->hDefine), hErr, counter,
				pCurrField->fDataBuf, pCurrField->size+1, SQLT_STR,
				(dvoid *)pCurrField->fDataIndicator, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
			checkError(); 
			//if string type, provide string length as client needs:
			sprintf(temp, "WIDTH=\"%d\"", pCurrField->size);
			break;
		case SQLT_RDD:
			pCurrField->type = ROWID_TYPE;
			pCurrField->size = 18;
			//绑定输出数据到缓冲区
			pCurrField->fDataBuf = new ub1[fPrefetchRows * (pCurrField->size+1)];
			fErrorNo = OCIDefineByPos(hStmt, &(pCurrField->hDefine), hErr, counter,
				(dvoid *)pCurrField->fDataBuf, 
				pCurrField->size+1, SQLT_STR,
				(dvoid *)pCurrField->fDataIndicator, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT);
			checkError();
			break;
		case SQLT_BLOB:
			pCurrField->size = 4;
			fPrefetchRows=1;	//	BLOB 目前只能以一条一条的方式fetch
			//pCurrField->fDataBuf = new ub1[(pCurrField->size+1];
			pCurrField->type = SQLT_BLOB;
			fErrorNo = OCIDescriptorAlloc((dvoid *)db->hEnv, (dvoid **)&pCurrField->hBlob,
				(ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid **) 0);
			checkError();

			fErrorNo = OCIDefineByPos(hStmt, &(pCurrField->hDefine), hErr, counter,
				(dvoid *)&pCurrField->hBlob, (sb4)-1, SQLT_BLOB,
				(dvoid *)0,  (ub2 *)0, (ub2 *) 0, OCI_DEFAULT);
			checkError();
			break;

		default:
			throw TOCIException(fSqlStmt, ERR_DATA_TYPE_NOT_SUPPORT, pCurrField->name,innerDataType);
			break;
		} //end of data type convertion
	}//end of for loop every column
}

void TOCIQuery::setSQL(const char *inSqlstmt)
{
    if (! db->fConnected)
		throw TOCIException(inSqlstmt, ERR_GENERAL, "SetSQL(): can't set sqlstmt on disconnected Database");
	
	if (fActivated)
		throw TOCIException(inSqlstmt, ERR_GENERAL, "SetSQL(): can't set sqlstmt on opened state");
	
	//如果有已经分配空间给sqlstatement,则在Close()中已经释放；因为只有在Close()后才可以赋予SQLstatement值
	fActivated  = false;
	fTotalRowsFetched = 0;
	fEof = false;
	fOpened = false;
	
	//保存sql语句
	if (fSqlStmt != NULL)
		delete[] fSqlStmt;
	int nLen = strlen(inSqlstmt);
	fSqlStmt = new char[nLen + 1];
	if (fSqlStmt == NULL)
		throw TOCIException(inSqlstmt, ERR_NOMORE_MEMORY_BE_ALLOCATED, "SetSQL()", __LINE__);
	strcpy(fSqlStmt,inSqlstmt);
	fSqlStmt[nLen] = '\0';
	
	fErrorNo = OCIStmtPrepare(hStmt, hErr, (unsigned char *)fSqlStmt, strlen(fSqlStmt), OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
	fActivated = (fErrorNo == OCI_SUCCESS);
	checkError();

	fErrorNo = OCIAttrGet(hStmt, OCI_HTYPE_STMT, &(this->fStmtType),  0, OCI_ATTR_STMT_TYPE, hErr);
	checkError();
	getParamsDef();
}


TOCIParam *TOCIQuery::paramByName(const char *paramName)
{
	TOCIParam *para = NULL;
	bool found = false;
	int i;
	
	if (fSqlStmt == NULL)
		throw TOCIException(paramName, ERR_GENERAL, "paramByName(): sql statement is empty.");
	
	for(i=0; i<fParamCount; i++)
	{
		found = compareStrNoCase(param(i).name,paramName);
		if ( found )
			break;
	}
	if ( found ) 
		para = &paramList[i];
	else 
		throw TOCIException(fSqlStmt, ERR_PARAM_NOT_EXISTS, paramName);
	return para;
}

void TOCIQuery::setParameterNULL(const char *paramName)
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->dataType = SQLT_LNG;
	fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name), //you don't have to pass any value/value length if the parameter value is null, or may raise oci success with info
		-1, (ub1 *)0,(sword)0, para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	
	checkError();
}

void TOCIQuery::setParameter(const char *paramName, double paramValue, bool isOutput )
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_FLT;
	para->dblValue = paramValue;

	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)para->name,
		-1,  (ub1 *)&(para->dblValue),(sb4)sizeof(para->dblValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)para->name,
		strlen(para->name), (dvoid *)&(para->dblValue),(sb4)sizeof(para->dblValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

	checkError();
}

void TOCIQuery::setParameter(const char *paramName, long paramValue, bool isOutput)
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_INT;
	para->longValue = paramValue;
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1, (ub1 *)&(para->longValue),(sword)sizeof(para->longValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else 
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		strlen(para->name), (ub1 *)&(para->longValue),(sword)sizeof(para->longValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	
	checkError();
}

void TOCIQuery::setParameter(const char *paramName, int paramValue, bool isOutput )
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_INT;
	para->intValue = paramValue;
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1, (ub1 *)&(para->intValue),(sword)sizeof(para->intValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else 
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		strlen(para->name), (ub1 *)&(para->intValue),(sword)sizeof(para->intValue), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	
	checkError();
}

void TOCIQuery::setLongRawParameter(char *paramName, char* paramValue, bool isOutput )
{

	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_LBI;
	if (para->stringValue)
		delete[] para->stringValue;

	int nLen;

	if (isOutput)
	{
		nLen = MAX_STRING_VALUE_LENGTH-1; 
		para->stringValue = new char[nLen+1];
		para->stringValue[nLen] = '\0';
	}
	else 
	{
		if(paramValue != NULL)
		{
			nLen = strlen(paramValue);
			para->stringValue = new char[nLen+1];
			strncpy((char *)para->stringValue,paramValue,nLen);
			para->stringValue[nLen] = '\0';
		}
		else
			setParameterNULL(paramName);
	}
	
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(para->stringValue),(sb4)(nLen+1), 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		(sb4)strlen(para->name), (dvoid *)(para->stringValue),(sb4)(nLen+1), 
		para->dataType, (dvoid *)&para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	checkError();
}


void TOCIQuery::setCBlobParameter(char *paramName, void* paramValue, int iLen,bool isOutput )
{
	 TOCIParam *para = paramByName(paramName); //在ParamByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_CLOB;
	if (para->stringValue)
		delete[] para->stringValue;

	int nLen;

	if (isOutput)
	{
		throw(0);
	}
	else 
	{
		if(paramValue != NULL)
		{
			nLen = iLen;
			fErrorNo = OCIDescriptorAlloc((dvoid *)db->hEnv, (dvoid **)&hBlob,
                                 (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid **) 0);
			checkError();

			fErrorNo = OCILobCreateTemporary(db->hDBSvc, hErr, hBlob, (ub2)0,  SQLCS_IMPLICIT,
                           OCI_TEMP_CLOB, OCI_ATTR_NOCACHE, 
                           OCI_DURATION_SESSION);
			checkError();

			ub4 t = nLen;
			fErrorNo = OCILobWrite(db->hDBSvc, hErr, hBlob, &t, 1, (dvoid *)paramValue,
					nLen, OCI_ONE_PIECE, (dvoid *)0,
					    (sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0,
					    (ub2)0,  (ub1) SQLCS_IMPLICIT);
			checkError();
		}
		else
			setParameterNULL(paramName);
	}
	
	fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(&hBlob), -1, 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	checkError();
}

void TOCIQuery::freeCBlob()
{
	OCILobClose(db->hDBSvc, hErr, hBlob);

	OCIDescriptorFree(hBlob, OCI_DTYPE_LOB);
	
}

void TOCIQuery::setParameter(char *paramName, void* paramValue, int iLen,bool isOutput )
{
	 TOCIParam *para = paramByName(paramName); //在ParamByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_LBI;
	//para->dataType = SQLT_LVC;
	if (para->stringValue)
		delete[] para->stringValue;

	int nLen;

	if (isOutput)
	{
		nLen = MAX_STRING_VALUE_LENGTH; 
		para->stringValue = new char[nLen];
		para->stringValue[nLen] = '\0';
	}
	else 
	{
		if(paramValue != NULL)
		{
			nLen = iLen;
			para->stringValue = new char[nLen];
			memcpy((void *)para->stringValue,paramValue,nLen);
		}
		else
			setParameterNULL(paramName);
	}
	
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(para->stringValue),(sb4)(nLen), 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		(sb4)strlen(para->name), (dvoid *)(para->stringValue),(sb4)(nLen), 
		para->dataType, (dvoid *)&para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	checkError();
}

void TOCIQuery::setParameter(const char *paramName, const char* paramValue, bool isOutput )
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_STR;
	if (para->stringValue)
		delete[] para->stringValue;

	int nLen;

//Modify begin version:0.0.0.8
	if (isOutput)
	{
		nLen = MAX_STRING_VALUE_LENGTH-1; 
		para->stringValue = new char[nLen+1];
		para->stringValue[nLen] = '\0';
	}
	else 
	{
		if(paramValue != NULL)
		{
			nLen = strlen(paramValue);
			para->stringValue = new char[nLen+1];
			strncpy((char *)para->stringValue,paramValue,nLen);
			para->stringValue[nLen] = '\0';
		}
		else
			setParameterNULL(paramName);
	}
//Modify end version:0.0.0.8
	
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(para->stringValue),(sb4)(nLen+1), 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		(sb4)strlen(para->name), (dvoid *)(para->stringValue),(sb4)(nLen+1), 
		para->dataType, (dvoid *)&para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	checkError();
}


void TOCIQuery::setParamArray(char const *paramName, char ** paramValue,int iStructSize,int iStrSize,  bool isOutput)
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_STR;
	para->stringArray = paramValue;
	para->stringSize = iStrSize;
	
	
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(para->stringArray),(sb4)(para->stringSize), 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		(sb4)strlen(para->name), (dvoid *)(para->stringArray),(sb4)(para->stringSize), 
		para->dataType, (dvoid *)&para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	checkError();

	fErrorNo = OCIBindArrayOfStruct ( para->hBind, hErr,(ub4) iStructSize,0,0,0);
	checkError();
}
 	
void TOCIQuery::setParamArray(char const *paramName, int * paramValue, int iStructSize, bool isOutput )
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_INT;
	para->intArray = paramValue;
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1, (ub1 *)(para->intArray),(sword)sizeof(para->intArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else 
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		strlen(para->name),(ub1 *)(para->intArray),(sword)sizeof(para->intArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	
	checkError();
	fErrorNo = OCIBindArrayOfStruct ( para->hBind, hErr,(ub4) iStructSize,0,0,0);
	checkError();
}
 
void TOCIQuery::setParamArray(char const *paramName, double * paramValue, int iStructSize,bool isOutput) 
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_FLT;
	para->dblArray = paramValue;

	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)para->name,
		-1,  (ub1 *)(para->dblArray),(sb4)sizeof(para->dblArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)para->name,
		strlen(para->name), (dvoid *)(para->dblArray),(sb4)sizeof(para->dblArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

	checkError();
	fErrorNo = OCIBindArrayOfStruct ( para->hBind, hErr,(ub4) iStructSize,0,0,0);
	checkError();
}

void TOCIQuery::setParamArray(char const *paramName, long * paramValue, int iStructSize, bool isOutput )
{
	TOCIParam *para = paramByName(paramName); //在paramByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_INT;
	para->longArray = paramValue;
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1, (ub1 *)(para->longArray),(sword)sizeof(para->longArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	else 
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		strlen(para->name),(ub1 *)(para->longArray),(sword)sizeof(para->longArray[0]), 
		para->dataType, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	
	checkError();
	fErrorNo = OCIBindArrayOfStruct ( para->hBind, hErr,(ub4) iStructSize,0,0,0);
	checkError();
}

void TOCIQuery::setParamArray(char const *paramName, void ** paramValue,int iStructSize,int iStrSize,  bool isOutput)
{
	TOCIParam *para = paramByName(paramName); //在ParamByName中已经有判断参数不存在抛出异常
	
	para->fIsOutput = isOutput;
	para->dataType = SQLT_LBI;
	para->stringArray = (char **)paramValue;
	para->stringSize = iStrSize;
	
	
	if (isOutput)
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		-1,  (dvoid *)(para->stringArray),(sb4)(para->stringSize), 
		para->dataType, (dvoid *) &para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *)0, OCI_DEFAULT);
	else
		fErrorNo = OCIBindByName(hStmt, &para->hBind, hErr, (text *)(para->name),
		(sb4)strlen(para->name), (dvoid *)(para->stringArray),(sb4)(para->stringSize), 
		para->dataType, (dvoid *)&para->indicator, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
	checkError();

	fErrorNo = OCIBindArrayOfStruct ( para->hBind, hErr,(ub4) iStructSize,0,0,0);
	checkError();
}

void TOCIQuery::checkError()
{
	if (fErrorNo != OCI_SUCCESS) 
		throw TOCIException(fErrorNo, hErr, "Oracle OCI Call", fSqlStmt);
}

bool TOCIQuery::execute(int iters)
{
	sb4 errcode;
	text errbuf[MAX_ERRMSG_LENGTH-1];
	bool exeResult = false;
//Added begin at ver : 0.0.0.6
#ifdef __DEBUG__
    bExecuteFlag = true;
#endif
//Added end at ver : 0.0.0.6
	
	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "Execute(): sql statement is not presented");
	
	if	(this->fStmtType == OCI_STMT_SELECT)
		throw TOCIException( fSqlStmt, ERR_GENERAL, "Execute(): Can't Execute a select statement.");
	
	fErrorNo = OCIStmtExecute(db->hDBSvc, hStmt, hErr, (ub4)iters, (ub4)0, 0, 0, OCI_DEFAULT);
	
	OCIAttrGet((dvoid*)hStmt, OCI_HTYPE_STMT, (dvoid *)&fTotalRowsFetched, (ub4 *)0, OCI_ATTR_ROW_COUNT, hErr);
	nTransTimes ++;
	
    if (fErrorNo == OCI_SUCCESS)
		exeResult = true;
	else if ( fErrorNo == OCI_ERROR ) //以下允许返回空参数(1405)
	{
		OCIErrorGet (hErr, (ub4) 1, (text *) NULL, &errcode,
			errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
		if (errcode == 1405) 
			exeResult = true;
		else 
			checkError();
	}
	else 
	{
			checkError();
	}
	
	return exeResult;
}


void TOCIQuery::getParamsDef()
{
	char *params[MAX_PARAMS_COUNT];
	int i, in_literal, n, nParamLen,nFlag = 0;
	char *cp,*ph;
	char *sql;
	
	int nLen = strlen(this->fSqlStmt);
	sql = new char[nLen+1];
	strcpy(sql, this->fSqlStmt);
	sql[nLen] = '\0';

	if (fParamCount>0)
		delete[] paramList;

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
				throw TOCIException(fSqlStmt, ERR_CAPABILITY_NOT_YET_SUPPORT, " param count execedes max numbers, please refer to OCIQuery.h");
			nParamLen = strlen((char *)ph);

			params[i] = new char[nParamLen+1];
			strcpy(params[i],(char *)ph);
			params[i][nParamLen] = '\0';
			i++;
			if(nFlag == 1) break;
      }   
	}
	delete[] sql;

	fParamCount = i;
	if (fParamCount>0)
	{
		paramList = new TOCIParam[fParamCount];
		
		for (i=0; i<fParamCount; i++)
		{
			nParamLen = strlen(params[i]);
			paramList[i].name = new char[nParamLen+1];
			strncpy(paramList[i].name, params[i], nParamLen);
			paramList[i].name[nParamLen] = '\0';
			delete[] params[i];
		}
	}
}
void TOCIQuery::open(int prefetch_Row)
{
	fPrefetchRows = prefetch_Row;
	if (fOpened)
	{
		fCurrRow = 0;
		fFetched = 0;
		fCurrRow = 0;
		fTotalRowsFetched = 0;
	}
	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "open(): sql statement is empty.");

	if ( this->fStmtType !=OCI_STMT_SELECT)
		throw TOCIException( fSqlStmt, ERR_GENERAL, "Can't open none-select statement");

	getFieldsDef();
	fBof = true;
	fOpened = true;
}

int TOCIQuery::fieldCount()
{
	return fFieldCount;
}

int TOCIQuery::paramCount()
{
	return fParamCount;
}

TOCIField& TOCIQuery::field(int i)
{
	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "Field(i): sql statement is not presented");

	if ( (i>=0) && (i<fFieldCount) ) 
		return fieldList[i];
	else throw TOCIException(fSqlStmt , ERR_INDEX_OUT_OF_BOUND, "field index out of bound when call Field(i)");
}

TOCIField& TOCIQuery::field(char const *fieldName)
{
	int i;
	bool found = false;

	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "Field(*fieldName): sql statement is not presented");

	if (! fOpened)
		throw TOCIException(fSqlStmt, ERR_GENERAL, "can not access field before open");

	for(i=0; i<fFieldCount; i++)
	{
		found = compareStrNoCase(field(i).name,fieldName);
		if ( found )
			break;
	}
	if ( found ) 
		return fieldList[i];
	else 
		throw TOCIException(fSqlStmt, ERR_FIELD_NOT_EXISTS, fieldName);
}

TOCIParam& TOCIQuery::param(int index)
{
	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "Param(index): sql statement is not presented");

#ifdef debug
	printf("param i constructor\n");
#endif

	if ( (index>=0) && (index<fParamCount) ) 
		return paramList[index];
	else
		throw TOCIException(fSqlStmt , ERR_INDEX_OUT_OF_BOUND, "param index out of bound when call Param(i)");
}

TOCIParam& TOCIQuery::param(char *inName)
{
	int i;
	bool found = false;

	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "Param(paramName): sql statement is not presented");
	
	for(i=0; i<fParamCount; i++)
	{
		found = compareStrNoCase(paramList[i].name,inName);
		if (found)
			break;
	}
	if ( found ) 
		return paramList[i];
	else
		throw TOCIException(fSqlStmt, ERR_PARAM_NOT_EXISTS, (const char*)inName);
}

bool TOCIQuery::next()
{
	int fCanFetch = 1;					//当前记录指针的位置是否可以存取数据
	int tmpFetchedAllRows;

	sb4 errcode;
	text errbuf[MAX_ERRMSG_LENGTH];
	bool exeResult = true;

	if (fSqlStmt == NULL)
		throw TOCIException("", ERR_GENERAL, "next(): sql statement is not presented");
	
	if (!fOpened)
		throw TOCIException(fSqlStmt, ERR_GENERAL, "next(): can not access data before open it");
		
	fCurrRow ++ ;
	if( (fCurrRow == fFetched) && (fFetched < fPrefetchRows)) 
		fCanFetch=0;
	else if(fCurrRow==fFetched || ! fFetched)
	{
		fErrorNo = OCIStmtFetch(hStmt, hErr, (ub4)fPrefetchRows, (ub4) OCI_FETCH_NEXT, (ub4) OCI_DEFAULT);	
		tmpFetchedAllRows = fTotalRowsFetched;
		fErrorNo = OCIAttrGet((dvoid*)hStmt, OCI_HTYPE_STMT, (dvoid *)&fTotalRowsFetched, (ub4 *)0, OCI_ATTR_ROW_COUNT, hErr);
		fFetched = fTotalRowsFetched - tmpFetchedAllRows;
		if(fFetched) 
		{
			fCanFetch=1;
			fCurrRow=0;
		}
		else fCanFetch=0; 

		if (fErrorNo == OCI_SUCCESS)
			exeResult = true;
		else if (fErrorNo == OCI_NO_DATA)
			exeResult = false;
		else if ( fErrorNo == OCI_ERROR ) //以下允许返回空列(1405),修正：不可以返回被截断的列(1406)
		{
			OCIErrorGet (hErr, (ub4) 1, (text *) NULL, &errcode,
				errbuf, (ub4) sizeof(errbuf), (ub4) OCI_HTYPE_ERROR);
			if (errcode == 1405)
				exeResult = true;
			else checkError();
		}
		else	checkError();
	}

	fBof = false;
	fEof = (fFetched && !fCanFetch);
	return (exeResult && fCanFetch);
}


/****************** parameter implementation **************************/
TOCIParam::TOCIParam()
{
	fIsOutput = false;
	stringValue = NULL;
	indicator = 0;
	hBind = (OCIBind *) 0; 
}

TOCIParam::~TOCIParam()
{
	delete[] name;
	delete[] stringValue;
}

int TOCIParam::asInteger()
{
	if ( isNULL() )
		intValue = 0;

	if (dataType == SQLT_INT)
		return intValue;
	else	throw TOCIException("TOCIParam", ERR_READ_PARAM_DATA, name, "asInteger()");	
}

long TOCIParam::asLong()
{
	if ( isNULL() )
		longValue = 0;
	
	if (dataType == SQLT_LNG)
		return longValue;
	else	throw TOCIException("TOCIParam", ERR_READ_PARAM_DATA, name, "asLong()");	
}

double TOCIParam::asFloat()
{
	if ( isNULL() )
		dblValue = 0;

	if (dataType == SQLT_FLT)
		return dblValue;
	else	throw TOCIException("TOCIParam", ERR_READ_PARAM_DATA, name, "asFloat()");
}


char* TOCIParam::asString()
{
	if ( isNULL() )
		stringValue[0] = '\0';

	if (dataType == SQLT_STR)
		return stringValue;
	else	throw TOCIException("TOCIParam", ERR_READ_PARAM_DATA, name, "asString()");
}

bool TOCIParam::isNULL()
{
	if (! fIsOutput)
		throw TOCIException("TOCIParam, not an output parameter", ERR_READ_PARAM_DATA, name, "isNULL()");
	return (indicator == -1);
}

/*********************************************************************************
*TOCISession	implementation
*********************************************************************************/
TOCISession::TOCISession(TOCIDatabase *pDB)
{
	if(!pDB->fConnected)
		throw TOCIException("", ERR_GENERAL, "TOCISession(pDB): Can not create a TOCISession when the database is not connected");
	
	m_hSession = NULL;
	m_hSrvCtx = NULL;
	m_bActive = FALSE;
	
	OCIHandleAlloc((dvoid *)pDB->hEnv,(dvoid **)&m_hSession,(ub4)OCI_HTYPE_SESSION,(size_t)0,(dvoid **) 0);
	
	OCIHandleAlloc((dvoid *)pDB->hEnv,(dvoid **)&m_hSrvCtx,(ub4)OCI_HTYPE_SVCCTX,(size_t)0,(dvoid **) 0);

	OCIHandleAlloc((dvoid *)pDB->hEnv,(dvoid **)&m_hError,(ub4)OCI_HTYPE_ERROR,(size_t)0,(dvoid **)0);

	m_iErrorNo = OCIAttrSet(m_hSrvCtx,OCI_HTYPE_SVCCTX,pDB->hSvr,0,OCI_ATTR_SERVER,m_hError);
	checkError();
	
	//set the username/password in session handle
	m_iErrorNo = OCIAttrSet(m_hSession,OCI_HTYPE_SESSION,pDB->usr,strlen(pDB->usr),OCI_ATTR_USERNAME,m_hError);
	m_iErrorNo = OCIAttrSet(m_hSession,OCI_HTYPE_SESSION,pDB->pwd,strlen(pDB->pwd),OCI_ATTR_PASSWORD,m_hError);
	
	//set the Authentication handle in the server context handle
	m_iErrorNo = OCIAttrSet(m_hSrvCtx,OCI_HTYPE_SVCCTX,m_hSession,0,OCI_ATTR_SESSION,m_hError);
	checkError();

};

TOCISession::~TOCISession()
{
	if(m_bActive)
		sessionEnd();
	
	OCIHandleFree((dvoid *)m_hSession,(ub4)OCI_HTYPE_SESSION);
	OCIHandleFree((dvoid *)m_hSrvCtx,(ub4)OCI_HTYPE_SVCCTX);
	
}

void TOCISession::sessionBegin()
{
	if(m_bActive)
		return;
		
	//begin a session
	m_iErrorNo = OCISessionBegin(m_hSrvCtx,m_hError,m_hSession,OCI_CRED_RDBMS,(ub4)OCI_DEFAULT);
	checkError();

	m_bActive = TRUE;                            
}

void TOCISession::sessionEnd()
{
	if(!m_bActive)
		return;
	//end a session
	m_iErrorNo = OCISessionEnd(m_hSrvCtx,m_hError,m_hSession,OCI_DEFAULT);
	checkError();	
}
static void errprint(dvoid *errhp, ub4 htype, sb4 *errcodep)
{
  text errbuf[512];

  if (errhp)
  {
    sb4  errcode;

    if (errcodep == (sb4 *)0)
      errcodep = &errcode;

    (void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, errcodep,
                       errbuf, (ub4) sizeof(errbuf), htype);
    (void) printf("Error - %.*s\n", 512, errbuf);
  }
}

void TOCISession::checkError()
{

 	switch (m_iErrorNo)
 	 {
 	 case OCI_SUCCESS:
 	   break;
 	 case OCI_SUCCESS_WITH_INFO:
 	   (void) printf( "Error - OCI_SUCCESS_WITH_INFO\n");
 	   errprint(m_hError, OCI_HTYPE_ERROR, &m_iErrorNo);
 	   break;
 	 case OCI_NEED_DATA:
 	   (void) printf( "Error - OCI_NEED_DATA\n");
 	   break;
 	 case OCI_NO_DATA:
 	   (void) printf( "Error - OCI_NODATA\n");
 	   break;
 	 case OCI_ERROR:
 		errprint(m_hError, OCI_HTYPE_ERROR, &m_iErrorNo);
 	   break;
 	 case OCI_INVALID_HANDLE:
 	   (void) printf( "Error - OCI_INVALID_HANDLE\n");
 	   break;
 	 case OCI_STILL_EXECUTING:
 	   (void) printf( "Error - OCI_STILL_EXECUTE\n");
 	   break;
 	 case OCI_CONTINUE:
 	   (void) printf( "Error - OCI_CONTINUE\n");
 	   break;
 	 default:
 	   break;
 	 }	
}
