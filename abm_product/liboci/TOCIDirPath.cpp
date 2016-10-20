/*VER: 1*/ 
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <ctype.h>
#include <math.h>
#include <oratypes.h>
#include <ocidfn.h>
#include <ocidem.h>
#include <ociapr.h>

#include <oci.h>
#include <oci8dp.h>
#include "TOCIQuery.h"
#include "TOCIDirPath.h"
#include "strcasecmp.h"

using namespace std;

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

TOCIDirPath::TOCIDirPath(TOCIDatabase *pdbConnect,char *strTableName)
{
	if(!pdbConnect->fConnected)
		throw TOCIException("", ERR_GENERAL, "TOCIDirPath(pdbConnect): Can not create a TOCIDirPath when the database is not connected");
	
	// Initialize all data member
	m_hDirPathCtx = NULL;
	m_hError = NULL;
	m_hDirPathCA = NULL;
	m_hDirPathStr = NULL;
	m_FieldAssigned = NULL;
	m_strTableName[0] = '\0';
	m_strSchameName[0] = '\0';
	m_uMaxRows = 0;
	m_uMaxCols = 0;
	m_bPrepared = FALSE;
	m_uRowSize = 0;
	m_uBufSize = 0;
	m_uBufCurPos = 0;
	m_iRowOff = 0;
	m_iErrorNo = 0;
	m_lSaveCount = 0;

	m_inData = (void *)NULL;
	m_pDB = pdbConnect;

	//create a new session  and begin this session 
	/*
	m_pSession = new TOCISession(m_pDB);
	m_pSession->sessionBegin();
	*/
	//cout<<"session created"<<endl;		
	
	m_Query = new TOCIQuery(m_pDB /*,m_pSession*/);
	/*
	OCIHandleAlloc((dvoid *)m_pDB->hEnv,(dvoid **)&m_hError,
					(ub4)OCI_HTYPE_ERROR,(size_t)0,(dvoid **)0);
	*/
	m_hError = m_pDB->hDBErr;
	
	//allocate an initialize direct path context
	m_iErrorNo = OCIHandleAlloc((dvoid *)m_pDB->hEnv,(dvoid **)&m_hDirPathCtx,
								(ub4)OCI_HTYPE_DIRPATH_CTX,
								(size_t)0,(dvoid **)0);
	checkError();
	
	if(strTableName!=NULL)
		setTableName(strTableName);
	
	m_iBufferSize = 256*1024;
	//cout<<"setTableName ok"<<endl;
	
}

TOCIDirPath::~TOCIDirPath()
{
	if(m_FieldAssigned)
	{		
		delete []m_FieldAssigned;
	}	
	
	OCIHandleFree((dvoid *)m_hDirPathCA,(ub4)OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
	OCIHandleFree((dvoid *)m_hDirPathStr,(ub4)OCI_HTYPE_DIRPATH_STREAM);
	OCIHandleFree((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX);
	
	delete m_Query;
	/*m_pSession->sessionEnd();
	delete m_pSession;
	*/
}

OCIDate TOCIDirPath::strToOCIDate(char * str)
{

    OCIDate  date;
    uword invalid;
    if ( strcasecmp(str,"sysdate")==0){
        OCIDateSysDate(m_hError,&date);
    }
    else {
        int yyyy,mm,dd,hh24,mi,ss;
        yyyy = (str[0]-'0')*1000+(str[1]-'0')*100+(str[2]-'0')*10+str[3]-'0';
        mm   = (str[4]-'0')*10+str[5]-'0';
        dd   = (str[6]-'0')*10+str[7]-'0';
        hh24 = (str[8]-'0')*10+str[9]-'0'+1;
        mi   = (str[10]-'0')*10+str[11]-'0'+1;
        ss   = (str[12]-'0')*10+str[13]-'0'+1;
        OCIDateSetDate(&date,yyyy,mm,dd);
        OCIDateSetTime(&date,hh24,mi,ss);
        
        
    }
    if ( OCIDateCheck( m_hError,&date, &invalid)!=OCI_SUCCESS){
        cout<<"OCIDateCheck Error"<<endl;
        errprint(m_hError, OCI_HTYPE_ERROR, &m_iErrorNo);
        throw 1;
    };
    
    
    return date;
}


void TOCIDirPath::setTableName(char *strTableName)
{
	char  strSQL[256];
    if(!strTableName&&!*strTableName)
    	return;
    	
	strcpy(m_strTableName , strTableName);
	
	//create temp table instance for get table information
	sprintf(strSQL,"select * from %s where rownum<0",m_strTableName);
	
	m_Query->setSQL(strSQL);
	
	m_Query->open();	
	m_uMaxCols = m_Query->fieldCount();
	
	//get field list information
	if(m_FieldAssigned)
	{		
		delete []m_FieldAssigned;
	}
		
	m_FieldAssigned = new bool[m_uMaxCols];
		
}

void TOCIDirPath::setSchameName(char *strSchameName)
{
    if(!strSchameName&&!*strSchameName)
    	return;
    	
	strcpy(m_strSchameName , strSchameName);	
}

void TOCIDirPath::setBufferSize(int iSize)
{
	m_iBufferSize = iSize;
}

long TOCIDirPath::getSaveCount()
{
	return m_lSaveCount;
}

void TOCIDirPath::prepare()
{
	
	if(m_bPrepared) 
  		throw TOCIException("",ERR_GENERAL,"TOCIDriPath::prepare can't prepare on prepared object");

	//set DirPathCtx attribute OCI_ATTR_NAME value 
	m_iErrorNo = OCIAttrSet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
				(dvoid *)m_strTableName,(ub4)strlen(m_strTableName),
				(ub4)OCI_ATTR_NAME,m_hError);
	checkError();

	//set DirPathCtx attribute OCI_ATTR_NAME value 
	if(m_strSchameName[0])
	{
		m_iErrorNo = OCIAttrSet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
								(dvoid *)m_strSchameName,(ub4)strlen(m_strSchameName),
								(ub4)OCI_ATTR_SCHEMA_NAME,m_hError);
		checkError(); 
	}
	
	m_iErrorNo = OCIAttrSet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
							(dvoid *)&m_iBufferSize,(ub4)4,
							(ub4)OCI_ATTR_BUF_SIZE,m_hError);
	checkError(); 

	
	m_iErrorNo = OCIAttrSet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
				(dvoid *)&m_uMaxCols,(ub4)0,
				(ub4)OCI_ATTR_NUM_COLS,m_hError);
	checkError();
	
	//set parallel
/*	unsigned p=1; 
	m_iErrorNo = OCIAttrSet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
				(dvoid *)&p,(ub4)0,
				(ub4)OCI_ATTR_DIRPATH_PARALLEL,m_hError);
	checkError();
*/	
	
	//get columns parameter list			
	OCIParam	*ColListDescCtx;
	ub1			ParamType;	
	m_iErrorNo = OCIAttrGet((dvoid *)m_hDirPathCtx,(ub4)OCI_HTYPE_DIRPATH_CTX,
				(dvoid **)&ColListDescCtx,(ub4 *)0,
				(ub4)OCI_ATTR_LIST_COLUMNS,m_hError);
	checkError();
	//get attributes of the column parameter list
	m_iErrorNo = OCIAttrGet((dvoid *)ColListDescCtx,OCI_DTYPE_PARAM,
				(dvoid *)&ParamType,(ub4 *)0,
				OCI_ATTR_PTYPE,	m_hError);


	if(ParamType!=OCI_PTYPE_LIST)
		throw TOCIException("",ERR_GENERAL,"setTableName: expected ParamType of OCI_PTYPE_LIST");
	
	OCIParam	*ColDesc;
	
	char colName[256];
	int colType;
	int colSize;

	for(int i=0,pos=1; i<m_uMaxCols;i++,pos++)
	{
		
		m_iErrorNo = OCIParamGet((dvoid *)ColListDescCtx,(ub4)OCI_DTYPE_PARAM,
					m_hError,(dvoid **)&ColDesc,(ub1)pos);
		checkError();
		
		
		strcpy(colName,m_Query->field(i).name);

		switch((*m_Query).field(i).type)
		{
		case INT_TYPE:
			colSize = m_Query->field(i).precision;
			colType = SQLT_CHR;
			m_iErrorNo = OCIAttrSet((dvoid *)ColDesc,(ub4)OCI_DTYPE_PARAM,
					(dvoid *)&colSize,(ub4)0,
					(ub4)OCI_ATTR_PRECISION,m_hError);
			colSize = m_Query->field(i).size;
		
			break;
		case STRING_TYPE:
			colSize = m_Query->field(i).size;
			colType = SQLT_CHR;
			break;
		case DATE_TYPE:
		    colSize = 14;//m_Query->field(i).size;
		    //cout<<"OCIDate size:"<<sizeof(OCIDate)<<endl;
		    colType = SQLT_CHR;
		    char * datemask = "yyyymmddhh24miss";

            OCIAttrSet((dvoid *)ColDesc, (ub4)OCI_DTYPE_PARAM,
             (dvoid *)datemask,
             (ub4)0,
             (ub4)OCI_ATTR_DATEFORMAT, m_hError);

		    break;
		}

		//column name
		m_iErrorNo = OCIAttrSet((dvoid *)ColDesc,(ub4)OCI_DTYPE_PARAM,
					(dvoid *)colName,(ub4)strlen(colName),
					(ub4)OCI_ATTR_NAME,m_hError);
		checkError();
		//column data type
		m_iErrorNo = OCIAttrSet((dvoid *)ColDesc,(ub4)OCI_DTYPE_PARAM,
					(dvoid *)&colType,(ub4)0,
					(ub4)OCI_ATTR_DATA_TYPE,m_hError);
		checkError();
		//column max size 
		m_iErrorNo = OCIAttrSet((dvoid *)ColDesc,(ub4)OCI_DTYPE_PARAM,
					(dvoid *)&colSize,(ub4)0,
					(ub4)OCI_ATTR_DATA_SIZE,m_hError);		
		checkError();

		m_uRowSize += colSize;
		
		m_iErrorNo = OCIDescriptorFree((dvoid *)ColDesc,(ub4)OCI_DTYPE_PARAM);
		checkError();
	}
	
	m_iErrorNo = OCIDescriptorFree((dvoid *)ColListDescCtx,(ub4)OCI_DTYPE_PARAM);
	checkError();

  	m_iErrorNo = OCIDirPathPrepare(m_hDirPathCtx,m_pDB->hDBSvc,m_hError);
  	checkError();	

  	m_iErrorNo = OCIHandleAlloc((dvoid *)m_hDirPathCtx,(dvoid **)&m_hDirPathCA,
  									(ub4)OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
  									(size_t)0,(dvoid **)0);
  	checkError();
  	m_iErrorNo = OCIHandleAlloc((dvoid *)m_hDirPathCtx,(dvoid **)&m_hDirPathStr,
  									(ub4)OCI_HTYPE_DIRPATH_STREAM,
  									(size_t)0,(dvoid **)0);
  	checkError();
  	
  	m_iErrorNo = OCIAttrGet((dvoid *)m_hDirPathCA,(ub4)OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
  				(dvoid *)&m_uMaxRows,(ub4 *)0,
  				(ub4)OCI_ATTR_NUM_ROWS,m_hError);
  	checkError();
  	//cout<<"m_uMaxRows:"<<m_uMaxRows<<endl;
  	//allocate buff for input records
  	m_uBufSize = m_uMaxRows*m_uRowSize+INDATA_SAVE_GUARD;
  	m_inData = (void *)malloc(m_uBufSize);
  	if(!m_inData)
  		throw TOCIException("",ERR_GENERAL,"TOCIDirPath::prepare: malloc for indata FATAL");
  	
  	memset(m_inData,0,m_uBufSize);
  	
	m_iErrorNo = OCIDirPathColArrayReset(m_hDirPathCA,m_hError);
	checkError();

	m_iErrorNo = OCIDirPathStreamReset(m_hDirPathStr,m_hError);
	checkError();

  	m_bPrepared = TRUE;
}

void TOCIDirPath::append()
{
	if(m_iRowOff>0)
		for(int i=0;i<m_uMaxCols;i++)
			if(!m_FieldAssigned[i])
				setFieldNull(i);
	
	int m_iMaxRows = m_uMaxRows;

	if(m_iRowOff>=m_iMaxRows-1)
	{
		load(m_iRowOff);
		m_iRowOff = 0;
	}
	
	m_iRowOff++;
	for(int i=0;i<m_uMaxCols;i++)
		m_FieldAssigned[i] = FALSE;
}

int TOCIDirPath::load(unsigned uRows)
{
	int	iLoadCnt,iSaveOff;
	bool continue_flag=true;
	
	iSaveOff = 0;
	while (continue_flag){

    	m_iErrorNo = OCIDirPathColArrayToStream(m_hDirPathCA,m_hDirPathCtx,m_hDirPathStr,m_hError,m_iRowOff+1,iSaveOff+1);
    
    	if ( m_iErrorNo == OCI_CONTINUE){
    	    continue_flag = true;
    	}
    	else {
    	    checkError();
    	    continue_flag = false;
    	};
    
#ifdef _DEBUG_
    	m_iErrorNo = OCIAttrGet((dvoid *)m_hDirPathStr,OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
    							(dvoid *)&iLoadCnt,(ub4 *)0,
    							OCI_ATTR_ROW_COUNT,m_hError);
    	cout<<iLoadCnt<<" rows records have converted!"<<endl;
    	m_iErrorNo = OCIAttrGet((dvoid *)m_hDirPathStr,OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
    							(dvoid *)&iLoadCnt,(ub4 *)0,
    							OCI_ATTR_COL_COUNT,m_hError);
    	cout<<iLoadCnt<<" cols records have converted!"<<endl;
#endif /*_DEBUG */
    	m_iErrorNo =OCIDirPathLoadStream(m_hDirPathCtx,m_hDirPathStr,m_hError);
    	checkError();
    	
    	m_iErrorNo = OCIAttrGet((dvoid *)m_hDirPathStr,OCI_HTYPE_DIRPATH_STREAM,
    							(dvoid *)&iLoadCnt,(ub4 *)0,
    							OCI_ATTR_ROW_COUNT,m_hError);
    	iSaveOff += iLoadCnt;
    	m_lSaveCount += iLoadCnt;
    	checkError();
    	//cout<<iLoadCnt<<" records have loaded!"<<endl;
    	
    	//save!
    	
    	m_iErrorNo = OCIDirPathDataSave(m_hDirPathCtx,m_hError,(ub4)OCI_DIRPATH_DATASAVE_SAVEONLY);	
	    checkError();
    	m_iErrorNo = OCIDirPathStreamReset(m_hDirPathStr,m_hError);
    	checkError();
	}	

	m_iRowOff = 0;
	m_uBufCurPos = 0;

	m_iErrorNo = OCIDirPathColArrayReset(m_hDirPathCA,m_hError);
	checkError();


	return 0;//add return load rows
}

void TOCIDirPath::save()
{
	if(m_iRowOff>0)
		load(m_iRowOff);

	//m_iErrorNo = OCIDirPathDataSave(m_hDirPathCtx,m_hError,(ub4)OCI_DIRPATH_DATASAVE_SAVEONLY);	
	//checkError();
}

void TOCIDirPath::abort()
{
	OCIDirPathAbort(m_hDirPathCtx,m_hError);
}
	
void TOCIDirPath::finish()
{
	OCIDirPathFinish(m_hDirPathCtx,m_hError);
	checkError();
	m_bPrepared = FALSE;
	
}

	
void TOCIDirPath::setField(char *strFieldName,char *fieldValue)
{
	unsigned i = 0;
	for(;i<m_uMaxCols;i++)
		if(!strcasecmp(strFieldName,m_Query->field(i).name))
			break;
	
	if(i==m_uMaxCols)
		throw (" ", ERR_FIELD_NOT_EXISTS, strFieldName);
	
	setField(i,fieldValue);
}

void TOCIDirPath::setField(char *strFieldName,int	fieldValue)
{
	unsigned i = 0;
	for(;i<m_uMaxCols;i++)
		if(!strcasecmp(strFieldName,m_Query->field(i).name))
			break;
	
	if(i==m_uMaxCols)
		throw (" ", ERR_FIELD_NOT_EXISTS, strFieldName);
	
	setField(i,fieldValue);
}

void TOCIDirPath::setField(char *strFieldName,double	fieldValue)
{
	unsigned i = 0;
	for(;i<m_uMaxCols;i++)
		if(!strcasecmp(strFieldName,m_Query->field(i).name))
			break;
	
	if(i==m_uMaxCols)
		throw (" ", ERR_FIELD_NOT_EXISTS, strFieldName);
	
	setField(i,fieldValue);
}

void TOCIDirPath::setField(char *strFieldName,long	fieldValue)
{
	unsigned i = 0;
	for(;i<m_uMaxCols;i++)
		if(!strcasecmp(strFieldName,m_Query->field(i).name))
			break;
	
	if(i==m_uMaxCols)
		throw (" ", ERR_FIELD_NOT_EXISTS, strFieldName);
	
	setField(i,fieldValue);
}

void TOCIDirPath::setField(unsigned uIndex,char *fieldValue)
{
	ub1	cflg;
	unsigned clen;
		
	if(uIndex<0||uIndex>=m_uMaxCols)
		throw	TOCIException(" ",ERR_INDEX_OUT_OF_BOUND,"TOCIDirPath::setField");
	
	clen = strlen(fieldValue);
	
	if(clen>0)
		cflg = OCI_DIRPATH_COL_COMPLETE;
	else 
		cflg = OCI_DIRPATH_COL_NULL;	
		
	/*if ( m_Query->field(uIndex).type == DATE_TYPE){
	    OCIDate date;
	    clen = 14;
	    date = StrToOCIDate(fieldValue);
	    memcpy((char *)m_inData+m_uBufCurPos,(unsigned char *)&date,clen);
	}
	else*/
	    memcpy((char *)m_inData+m_uBufCurPos,fieldValue,clen);
	
	m_iErrorNo = OCIDirPathColArrayEntrySet(m_hDirPathCA,m_hError,
								m_iRowOff,uIndex,
								(unsigned char *)m_inData+m_uBufCurPos,(ub4)clen,cflg);
	checkError();
	m_uBufCurPos += clen;

	m_FieldAssigned[uIndex] = TRUE;	
}

void TOCIDirPath::setField(unsigned uIndex,int	fieldValue)
{
	
	if(uIndex<0||uIndex>=m_uMaxCols)
		throw(" ",ERR_INDEX_OUT_OF_BOUND,"TOCIDirPath::setField");
	
	ub1	cflg;
	cflg = OCI_DIRPATH_COL_COMPLETE;
	
	char temp[12];
	unsigned ulen;
	
	sprintf(temp,"%d",fieldValue);
	ulen = strlen(temp);
		
	memcpy((void *)((char *)m_inData+m_uBufCurPos),(void *)temp,ulen);
	
	m_iErrorNo = OCIDirPathColArrayEntrySet(m_hDirPathCA,m_hError,
								m_iRowOff,uIndex,
								(unsigned char *)m_inData+m_uBufCurPos,(ub4)ulen,cflg);
	checkError();
	m_uBufCurPos += ulen;

	m_FieldAssigned[uIndex] = TRUE;	
}

void TOCIDirPath::setField(unsigned uIndex,double	fieldValue)
{
	if(uIndex<0||uIndex>=m_uMaxCols)
		throw(" ",ERR_INDEX_OUT_OF_BOUND,"TOCIDirPath::setField");
	
	ub1	cflg;
	cflg = OCI_DIRPATH_COL_COMPLETE;

	memcpy((void *)((char *)m_inData+m_uBufCurPos),(void *)&fieldValue,sizeof(double));
	
	m_iErrorNo = OCIDirPathColArrayEntrySet(m_hDirPathCA,m_hError,
								m_iRowOff,uIndex,
								(unsigned char *)m_inData+m_uBufCurPos,(ub4)sizeof(double),cflg);
	checkError();
	m_uBufCurPos += sizeof(double);

	m_FieldAssigned[uIndex] = TRUE;	
}

void TOCIDirPath::setField(unsigned uIndex,long	fieldValue)
{
	if(uIndex<0||uIndex>=m_uMaxCols)
		throw(" ",ERR_INDEX_OUT_OF_BOUND,"TOCIDirPath::setField");
	
	ub1	cflg;
	cflg = OCI_DIRPATH_COL_COMPLETE;

	char temp[12];
	unsigned ulen;
	
	sprintf(temp,"%ld",fieldValue);
	ulen = strlen(temp);
	
	memcpy((void *)((char *)m_inData+m_uBufCurPos),(void *)temp,ulen);
	
	m_iErrorNo = OCIDirPathColArrayEntrySet(m_hDirPathCA,m_hError,
								m_iRowOff,uIndex,
								(unsigned char *)m_inData+m_uBufCurPos,(ub4)ulen,cflg);
	checkError();
	m_uBufCurPos += ulen;

	m_FieldAssigned[uIndex] = TRUE;								
}

void TOCIDirPath::setFieldNull(unsigned uIndex)
{
	if(uIndex<0||uIndex>=m_uMaxCols)
		throw(" ",ERR_INDEX_OUT_OF_BOUND,"TOCIDirPath::setField");

	ub1	cflg;
	cflg = OCI_DIRPATH_COL_NULL;
	
	m_iErrorNo = OCIDirPathColArrayEntrySet(m_hDirPathCA,m_hError,
								m_iRowOff,uIndex,
								(unsigned char *)NULL,(ub4)sizeof(long),cflg);
	checkError();
								
}



void TOCIDirPath::checkError()
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
