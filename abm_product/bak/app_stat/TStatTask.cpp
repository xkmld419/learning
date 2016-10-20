/*VER: 1*/ 
#include "DynCreate.h"
#include "string.h"
#include "stat_public.h"
#include <time.h>

#include "trie_list_index.h"
class TFieldValue;
class TFieldAttr;
class TFieldRowAttr;
class TFieldRowValue;
class TStatTask;
class StatMonthList;
class TStatClass;
class TStatSave;
#include "TStatTask.h"
#include "TOCIQuery.h"

#define	STAT_DEBUG

#define	NAME_SIZE	41
#define	SQLBUFF		2000
#define SAVE_BUFSIZE  500
#define FIELD_CNT   30
#define	FIELD_SIZE  21
#define	MIN_INTERVAL_TIME 32  
#define MAX_INTERVAL_TIME 128 
#define INTERVAL_TIME 	256 	
#define MONTH_INTERVAL_TIME 512 
#define CHANGEDATA_LEVEL1 10000 
#define CHANGEDATA_LEVEL2 100000 
#define CHANGEDATA_LEVEL3 1000000 

#define TURNOUT_MINUSVALUE POINT_TYPE(-1)/2 
#define	MAX_POINT_VALUE	POINT_TYPE(-1)	
#define INC_POINT_VALUE 1000000 		

#define INSERT_TYPE_CLASS	"INS"
#define UPDATE_TYPE_CLASS	"UPD"

#define	RUN_MODE_NORMAL		"F0A"
#define	RUN_MODE_REBACK		"F0B"

enum{OLD_STATE,NEW_STATE,UPD_STATE,PAGE_STATE} ;


class TFieldAttr
{
public:
	TFieldAttr(){iIndexFlag=0;};
	~TFieldAttr(){};
	int		iFieldID;					
	char	sFieldName[NAME_SIZE];		
	char	ValType;					
	int		iIndexFlag;					
	TFieldValue	 tFieldValue;			
};

class TFieldRowAttr
{
public:	
	TFieldRowAttr(int ColNum);
	~TFieldRowAttr();
	char * GetCTime();
	char			cFlag;				
	time_t			tAccessTime;		
	TFieldValue		*aColValue;			
	TFieldRowValue 	*pNext;				
	TFieldRowValue 	*pPrev;				
};


class TFieldRowValue
{
public:
	TFieldRowValue(int ItemID,int ColNum);
	TFieldRowValue(int ItemID);
	~TFieldRowValue();
	
	int		ItemID;						
	TFieldRowAttr	*pRowAttr;			
	
};


class TStatTask{
public:
	TStatTask(int m_TaskID,int m_Month,int m_StatItemType,StatMonthList *pStatMonthList);
	~TStatTask();
	bool CheckColumn(char *sTable,TFieldAttr *);	
	void OutputResult();  				
    void ProcessStat();	  				
	void FreeResult();
	int  GetNextItemID();
	float GetFoundPercent();
	void InsertMemRowListTail(TFieldRowValue *);
		
	TFieldAttr  	*aFIdentHead;		
	TFieldAttr  	*aFResultHead;		
	TFieldAttr  	*aFHead;			
	int			  	iIdentFieldNum;		
	int				iResultFieldNum;	
	int				iFieldNum;			
	int			  	iMaxItemID;				
	TFieldRowValue 	*m_RowListHead;		
	TFieldRowValue  *m_RowListTail;		
	trie_t			*StatTrieIndex;		
	
	int  			m_TaskID;			
	int  			m_Month;   			
	int	 			m_StatItemType;		
	
	int  			m_ResultCount;		
	int				TotalCount;			
	int				iNotFoundCnt;		
	int				iFoundCnt;			
	float			fFoundPercent;		
	int				iIntervalTime;		
	char			m_TableName[NAME_SIZE];		
										
	TStatTask		*pNext;				
	StatMonthList	*pStatMonthList;	

};

class StatMonthList{
public:
	StatMonthList(int m_Month,int iStatClassID,TStatClass *pStatClass);
	~StatMonthList();
	void OutputResult();  				
    void ProcessStat();	  				
    void FreeResult();
	int				m_Month;			
	int				iStatClassID;		
	TStatTask 		*pStatTaskHead;		
	time_t			tAccessTime;		
	StatMonthList 	*pNext;				
	TStatClass		*pStatClass;		
	int				m_ResultCount;		
	int				TotalCount;			
	
};

class TStatSave {
public:
    TStatSave(TStatTask *pStatTask);
    ~TStatSave();
    void SetTableName(const char * sName);  
    void Insert(TFieldRowValue *);			
    void InsertFlush();      				
	void Update(TFieldRowValue *);			
	void UpdateFlush();						
private:
    int   Insert_RecCnt;                	
    int	  Update_RecCnt;					
    char  m_TableName[81];                	
	char  InsertBuffer[SAVE_BUFSIZE][FIELD_CNT][FIELD_SIZE];
	char  UpdateBuffer[SAVE_BUFSIZE][FIELD_CNT][FIELD_SIZE];
	TStatTask	*pStatTask;
};



static char * _FILE_NAME_="TStatTask.cpp";
static TStatSave*	gpStatSave;
extern TOCIDatabase gpDBLink,gpDBLink_Temp;







void my_new_error()
{
        cout<<"new error!内存已经不足!"<<endl;
        exit(1);
}






TFieldValue::TFieldValue()
{
	ValType=VT_UNKNOW;
	value.sValue=NULL;
}

TFieldValue::TFieldValue(long lValue)
{
	ValType=VT_INT;
	value.lValue=lValue;
}

TFieldValue::TFieldValue(char * sValue)
{
	if (ValType==VT_STRING && value.sValue!=NULL){
		delete []value.sValue;
		value.sValue=NULL;
	}
	
	ValType=VT_STRING;
	value.sValue=new char[strlen(sValue)+1];
	strcpy(value.sValue,sValue);
}

TFieldValue::TFieldValue(TFieldValue & A)
{
	
	if (A.ValType==VT_INT)
		SetValue(A.value.lValue);
	else if (A.ValType==VT_STRING)
		SetValue(A.value.sValue);
	else 
		ValType=A.ValType;
}

TFieldValue::~TFieldValue()
{
	if (ValType==VT_STRING){
		delete []value.sValue;
		value.sValue=NULL;
	}
	ValType==VT_UNKNOW;
}


void TFieldValue::SetValue(long lValue)
{
	ValType=VT_INT;
	value.lValue=lValue;
}


void TFieldValue::SetValue(char * sValue)
{
	if (ValType==VT_STRING && value.sValue!=NULL){
		delete []value.sValue;
		value.sValue=NULL;
	}
	ValType=VT_STRING;
	value.sValue=new char[strlen(sValue)+1];
	strcpy(value.sValue,sValue);
}

TFieldValue & TFieldValue::operator += (TFieldValue &d2 ) 
{
	
	if (ValType!=d2.ValType)
		throw TException("TStatTask:TFieldValue operator + 两个操作数的类型不一致!"); 
	
	if (ValType==VT_INT)
		SetValue(value.lValue+d2.value.lValue);
	else if (ValType==VT_STRING)
		throw TException("TStatTask:TFieldValue operator + 操作数的类型不能为字符型!"); 
	
	return *this;
}

TFieldValue & TFieldValue::operator = (TFieldValue & d2 ) 
{
	if (this==&d2)
		return *this;
		
	if (d2.ValType==VT_INT)
		SetValue(d2.value.lValue);
	else if (d2.ValType==VT_STRING)
		SetValue(d2.value.sValue);
		
	return *this;
}

bool		TFieldValue::operator == (TFieldValue & d2 ) const
{
	if (ValType!=d2.ValType)
		throw TException("TStatTask:TFieldValue operator == 两个操作数的类型不一致!"); 
	
	if (ValType==VT_INT) 
		if (value.lValue==d2.value.lValue )
			return true;
		else return false;
	else if (ValType==VT_STRING) 
		if ( !strcmp(value.sValue,d2.value.sValue) )
			return true;
		else return false;
}

bool		TFieldValue::operator != (TFieldValue & d2 ) const
{
	if (ValType!=d2.ValType)
		throw TException("TStatTask:TFieldValue operator == 两个操作数的类型不一致!"); 
	
	if (ValType==VT_INT) 
		if (value.lValue!=d2.value.lValue )
			return true;
		else return false;
	else if (ValType==VT_STRING) 
		if ( strcmp(value.sValue,d2.value.sValue) )
			return true;
		else return false;
}






void 		TFieldValue::SetDefaultValue(void)
{
	if (ValType==VT_INT)
		SetValue(-1);
	else if ( ValType==VT_STRING)
		SetValue("NULL");
}


void 		TFieldValue::AutoSetValue(long lvalue)
{
	char sTemp[100];
	if (ValType==VT_INT)
		SetValue(lvalue);
	else if ( ValType==VT_STRING){
		sprintf(sTemp,"%ld",lvalue);
		SetValue(sTemp);
	}
}

void 		TFieldValue::AutoSetValue(char* sValue)
{
	if (ValType==VT_INT)
		SetValue( atol(sValue) );
	else if ( ValType==VT_STRING)
		SetValue(sValue);
	
}

TFieldRowAttr::TFieldRowAttr(int ColNum)
{
	cFlag=NEW_STATE;
	tAccessTime=time(NULL);
	aColValue=new TFieldValue[ColNum];
	pNext=NULL;
	pPrev=NULL;
}

TFieldRowAttr::~TFieldRowAttr()
{
	cFlag=PAGE_STATE;
	delete []aColValue;
	aColValue=NULL;
	pNext=NULL;
	pPrev=NULL;
}

char * TFieldRowAttr::GetCTime()
{
	static char sTimeBuf[15];
	struct tm* ptm;
	
	ptm=localtime(&tAccessTime);
	strftime(sTimeBuf,sizeof(sTimeBuf),"%Y%m%d%H%M%S",ptm);
	return sTimeBuf;
}


TFieldRowValue::TFieldRowValue(int ItemID,int ColNum)
{
	this->ItemID=ItemID;
	pRowAttr=new TFieldRowAttr(ColNum);
	
}

TFieldRowValue::TFieldRowValue(int ItemID)
{
	this->ItemID=ItemID;
	pRowAttr=NULL;
}

TFieldRowValue::~TFieldRowValue()
{
	delete pRowAttr;
}
static void ClearBreakPoint(int iStatClassID)
{
	TOCIQuery qry(&gpDBLink_Temp);
	char sSql[SQLBUFF];
	
	try{
		sprintf(sSql,"UPDATE STAT_CLASS_DATA_SOURCE	SET BREAKPOINT=0 WHERE STAT_CLASS_ID=%d ", iStatClassID);
		qry.close();
		qry.setSQL(sSql);
		qry.execute();	
		cout<<sSql<<endl;
		gpDBLink_Temp.commit();
	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.GetErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
}

static void TrunCateTable(char * sTableName)
{
	TOCIQuery qry(&gpDBLink_Temp);
	char sSql[SQLBUFF];
	
	try{
		sprintf(sSql,"TRUNCATE TABLE %s ",sTableName);
		qry.close();
		qry.setSQL(sSql);
		qry.execute();	
		cout<<sSql<<endl;	
	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.GetErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
}



TStatTask::TStatTask(int m_TaskID,int m_Month,int m_StatItemType,StatMonthList *pStatMonthList)
{
	char m_BaseTableName[NAME_SIZE];	
	TOCIQuery qry(&gpDBLink);
	char sSql[SQLBUFF];
	int i,j;
	char sColumn[1000];
	char sIndexStr[500];
	char sIndExt[100];
	TFieldRowValue * pRowValue;
	int	 iUpLoadCnt;
	int	 iHaveIndexFlag;
		
	this->m_TaskID=m_TaskID;
	this->m_Month=m_Month;
	this->m_StatItemType=m_StatItemType;
	this->pStatMonthList=pStatMonthList;
	m_ResultCount=0;
	TotalCount=0;
	iNotFoundCnt=0;
	iFoundCnt=0;
	iIntervalTime=INTERVAL_TIME;
	pNext=NULL;
	StatTrieIndex=NULL;
	m_RowListHead=m_RowListTail=NULL;
	
	try{
		qry.close();
        qry.setSQL("SELECT BASE_TABLE_NAME FROM STAT_ITEM_TYPE WHERE STAT_ITEM_TYPE=:StatItemType");
        qry.setParameter("StatItemType",m_StatItemType);
        qry.open();
        
        if ( qry.next() )
        	strcpy ( m_BaseTableName, qry.field("BASE_TABLE_NAME").asString());
        else{
        	char sTemp[200];
			sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d]对应的基表不存在,请修改STAT_ITEM_TYPE!",
				_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID);
			throw TException(sTemp);
    	}
    	
        sprintf( m_TableName, "%s_%d_%d",m_BaseTableName,m_StatItemType,m_Month);
        sprintf ( sIndExt,"%d_%d",m_TaskID,m_Month);
        DupTable(m_BaseTableName,m_TableName,&gpDBLink_Temp);
        DupTableIndex(m_BaseTableName,m_TableName,sIndExt,&gpDBLink_Temp);
        
        if ( !strcmp(pStatMonthList->pStatClass->sStatClassType,UPDATE_TYPE_CLASS) )
        	TrunCateTable(m_TableName);
        
		qry.close();
		qry.setSQL("SELECT 	count(*) IDENT_CNT  FROM  STAT_IDENT_FIELD_DEF A,STAT_FIELD_DEF B WHERE A.FIELD_ID=B.FIELD_ID AND STAT_ITEM_TYPE = :StatItemType");
		qry.setParameter("StatItemType",m_StatItemType);
		qry.open();
		qry.next();
		iIdentFieldNum=qry.field("IDENT_CNT").asInteger();
		
		qry.close();
		qry.setSQL("SELECT 	count(*) RESULT_CNT FROM  STAT_RESULT_FIELD_DEF A,STAT_FIELD_DEF B WHERE A.FIELD_ID=B.FIELD_ID AND STAT_ITEM_TYPE = :StatItemType");
		qry.setParameter("StatItemType",m_StatItemType);
		qry.open();
		qry.next();
		iResultFieldNum=qry.field("RESULT_CNT").asInteger();
		
		iFieldNum=iIdentFieldNum+iResultFieldNum;
		
		if (iFieldNum+1>=FIELD_CNT){
			char sTemp[200];
			sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d],FIELD_CNT:[%d]统计域的域数太多了，请修改宏定义中FIELD_CNT的大小!",
				_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID,iFieldNum);
			throw TException(sTemp);
		}
		
		qry.close();
		sprintf(sSql,"SELECT MAX(ITEM_ID) MAX_ITEM_ID FROM  %s " , m_TableName);
		qry.setSQL(sSql);
		qry.open();
		
		if ( qry.next()){
        	iMaxItemID = qry.field("MAX_ITEM_ID").asInteger();
    	}
    	else {
        	iMaxItemID = 0;
    	}
    	qry.close();
		
		aFHead = new TFieldAttr[iFieldNum];
		aFIdentHead = aFHead;
		aFResultHead = &aFHead[iIdentFieldNum];
		
		
		sprintf(sSql ,"SELECT A.FIELD_ID FIELD_ID ,NAME,VALUE_TYPE,NVL(INDEX_FLAG,0) INDEX_FLAG \
			FROM  STAT_IDENT_FIELD_DEF A,STAT_FIELD_DEF B \
			WHERE A.FIELD_ID=B.FIELD_ID \
			AND STAT_ITEM_TYPE = %d  ORDER BY FIELD_ID ",m_StatItemType);
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		iHaveIndexFlag=0;
		for(i=0;qry.next();i++){
			
			aFIdentHead[i].iFieldID=qry.field("FIELD_ID").asInteger();				
			strcpy(aFIdentHead[i].sFieldName,qry.field("NAME").asString() );
			AllTrim(aFIdentHead[i].sFieldName);
			aFIdentHead[i].ValType=qry.field("VALUE_TYPE").asInteger();
			aFIdentHead[i].iIndexFlag=qry.field("INDEX_FLAG").asInteger();
			if (aFIdentHead[i].iIndexFlag==1)
				iHaveIndexFlag=1;
			
			CheckColumn(this->m_TableName,&aFIdentHead[i]);
		}
		
		if (iHaveIndexFlag==0)
			aFIdentHead[0].iIndexFlag=1;
			
		
		sprintf(sSql ,"SELECT A.FIELD_ID FIELD_ID ,NAME,VALUE_TYPE \
			FROM  STAT_RESULT_FIELD_DEF A,STAT_FIELD_DEF B \
			WHERE A.FIELD_ID=B.FIELD_ID \
			AND STAT_ITEM_TYPE = %d ORDER BY FIELD_ID ",m_StatItemType);
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		for(i=0;qry.next();i++){
			
			aFResultHead[i].iFieldID=qry.field("FIELD_ID").asInteger();				
			strcpy(aFResultHead[i].sFieldName,qry.field("NAME").asString() );
			AllTrim(aFResultHead[i].sFieldName);
			aFResultHead[i].ValType=qry.field("VALUE_TYPE").asInteger();
			if (aFResultHead[i].ValType==VT_STRING){
				char sTemp[200];
				sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d],FIELD_ID:[%d]作为统计结果域不能为字符型!",
						_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID,aFResultHead[i].iFieldID);
				throw TException(sTemp);
			}
			
			CheckColumn(this->m_TableName,&aFResultHead[i]);
		}
	
		
		if (iMaxItemID<=CHANGEDATA_LEVEL1){
			/*iUpLoadCnt=iMaxItemID;*/
			iUpLoadCnt=0;
		}
		else if (iMaxItemID<=CHANGEDATA_LEVEL2){
			iUpLoadCnt=iMaxItemID*5/10;
		}
		else if (iMaxItemID<=CHANGEDATA_LEVEL3){
			iUpLoadCnt=iMaxItemID*3/10;
		}
		else{
			iUpLoadCnt=iMaxItemID*2/10;
		}
		
		if (!pStatMonthList->pStatClass->iPageInOutMode)
			iUpLoadCnt=iMaxItemID;
			
		strcpy(sColumn,"ITEM_ID");
        for (i=0;i<iFieldNum;i++){
			strcat(sColumn,",");
			strcat(sColumn,aFHead[i].sFieldName);
		}
		sprintf(sSql,"SELECT %s FROM %s ORDER BY ACCESS_DATE ",sColumn,this->m_TableName);
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		
		for(j=0;qry.next();j++){
			
			
			sIndexStr[0]='\0';
			
			for (i=0;i<iIdentFieldNum;i++){
					char sTemp[FIELD_SIZE];
					if (aFIdentHead[i].iIndexFlag==0)
						continue;
					strcpy(sTemp,qry.field(aFIdentHead[i].sFieldName).asString());
					AllTrim(sTemp);
					strcat(sIndexStr,sTemp);
			}
			
			if (iMaxItemID-j<=iUpLoadCnt){
							
				pRowValue=new TFieldRowValue(qry.field("ITEM_ID").asInteger(),iFieldNum);
				
				InsertMemRowListTail(pRowValue);
				
				m_ResultCount++;
				pStatMonthList->m_ResultCount++;
				pStatMonthList->pStatClass->m_ResultCount++;
					
				for (i=0;i<iFieldNum;i++){
						if (aFHead[i].ValType==VT_INT){
							pRowValue->pRowAttr->aColValue[i].SetValue(atol( qry.field(aFHead[i].sFieldName).asString() ) );
						}	
						else if (aFHead[i].ValType==VT_STRING){
							pRowValue->pRowAttr->aColValue[i].SetValue( qry.field(aFHead[i].sFieldName).asString() );
							AllTrim(pRowValue->pRowAttr->aColValue[i].value.sValue);
						}	
				}
				
				pRowValue->pRowAttr->cFlag=OLD_STATE;

			}
			else{
				pRowValue=new TFieldRowValue(qry.field("ITEM_ID").asInteger());
			}	
			
			TotalCount++;
			pStatMonthList->TotalCount++;
			pStatMonthList->pStatClass->TotalCount++;
			
			if (InsertIndexTrieList(&StatTrieIndex,sIndexStr,strlen(sIndexStr),pRowValue,0)<0)
				throw TException("TStatTask:InsertIndexTrieList中分配指针失败"); 
				
			
		}
	}
	catch(TOCIException &oe){
		throw oe;
    }
}

static void free_RowValue(void * data)
{
	TFieldRowValue *pRowValue;
	
	pRowValue=(TFieldRowValue *) data;
	
	delete pRowValue;
}

TStatTask::~TStatTask()
{
	TFieldRowValue * pRowValue=NULL;
	
	pStatMonthList->m_ResultCount=pStatMonthList->m_ResultCount-m_ResultCount;
	pStatMonthList->TotalCount=pStatMonthList->TotalCount-TotalCount;
	
	pStatMonthList->pStatClass->m_ResultCount=pStatMonthList->pStatClass->m_ResultCount-m_ResultCount;
	pStatMonthList->pStatClass->TotalCount=pStatMonthList->pStatClass->TotalCount-TotalCount;
	
	delete []aFHead;
	FreeIndexTrieListAndData(&StatTrieIndex,free_RowValue);
		
}

void TStatTask::InsertMemRowListTail(TFieldRowValue *pRowValue)
{
	if (m_RowListTail==NULL&&m_RowListHead==NULL){
		m_RowListTail=m_RowListHead=pRowValue;
		pRowValue->pRowAttr->pNext=NULL;
		pRowValue->pRowAttr->pPrev=NULL;
	}
	else{
		m_RowListTail->pRowAttr->pNext=pRowValue;
		pRowValue->pRowAttr->pPrev=m_RowListTail;
		m_RowListTail=pRowValue;
		m_RowListTail->pRowAttr->pNext=NULL;
	}
}

int TStatTask::GetNextItemID()
{
	return ++iMaxItemID;
}


bool TStatTask::CheckColumn(char *sTableName,TFieldAttr * pTFieldAttr)
{
	char sAlter[SQLBUFF];
	TOCIQuery qry(&gpDBLink_Temp);
	
	try{
		char *sSQL="select count(*) CNT \
		FROM USER_TAB_COLUMNS \
		WHERE TABLE_NAME=upper(:vsTableName) \
		AND COLUMN_NAME=upper(:vsColumn) ";
		qry.close();
		qry.setSQL(sSQL);
		qry.setParameter("vsTableName",sTableName);
		qry.setParameter("vsColumn",pTFieldAttr->sFieldName);
		qry.open();
		if (qry.next()){
			if(!(qry.field("CNT").asInteger())){
			cout<<endl<<"表:["<<sTableName<<"] 字段["<<pTFieldAttr->sFieldName<<"]不存在"<<endl;
			
			sprintf(sAlter,"ALTER TABLE %s ADD %s VARCHAR2(20) ",sTableName,pTFieldAttr->sFieldName);
			qry.close();
			qry.setSQL(sAlter);
			qry.execute();
			cout<<endl<<"表：["<<sTableName<<"]字段["<<pTFieldAttr->sFieldName<<"]新建成功"<<endl;
			
			if (pTFieldAttr->ValType==VT_STRING)
				sprintf(sAlter,"UPDATE %s SET %s ='NULL' ",sTableName,pTFieldAttr->sFieldName);
			else if (pTFieldAttr->ValType==VT_INT)
				sprintf(sAlter,"UPDATE %s SET %s ='-1' ",sTableName,pTFieldAttr->sFieldName);
				
			qry.close();
			qry.setSQL(sAlter);
			qry.execute();
			cout<<endl<<"表：["<<sTableName<<"]字段["<<pTFieldAttr->sFieldName<<"]更新成功"<<endl;
			
			sprintf(sAlter,"ALTER TABLE %s MODIFY %s NOT NULL ",sTableName,pTFieldAttr->sFieldName);
			qry.close();
			qry.setSQL(sAlter);
			qry.execute();
			cout<<endl<<"表：["<<sTableName<<"]字段["<<pTFieldAttr->sFieldName<<"]修改属性成功"<<endl;
			}
		}

	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.GetErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
	return true;
}
static int  CmpValue(const void *data1 ,const void *data2)
{
		TStatTask * pStatTask;
		TFieldRowValue * pRowValue;
		TOCIQuery qry(&gpDBLink);
		char sSql[SQLBUFF],sTemp[100];
		int  i;
		
		pRowValue=(TFieldRowValue *)data1;
		pStatTask=(TStatTask *)data2;
		
		if ( pRowValue->pRowAttr==NULL ){ 
			pStatTask->iNotFoundCnt++;	
			try{
				qry.close();
				strcpy(sSql,"SELECT ITEM_ID");
				for (i=0;i<pStatTask->iFieldNum;i++){
					strcat(sSql,",");
					strcat(sSql,pStatTask->aFHead[i].sFieldName);
				}
				strcat(sSql," FROM ");
				strcat(sSql,pStatTask->m_TableName);
				strcat(sSql," WHERE ");
				strcat(sSql," ITEM_ID=");
				sprintf(sTemp,"%d",pRowValue->ItemID);
				strcat(sSql,sTemp);
				
				qry.setSQL(sSql);
        		qry.open();
        		
        		if (qry.next()){
					pRowValue->pRowAttr=new TFieldRowAttr(pStatTask->iFieldNum);
					
					pStatTask->m_ResultCount++;
					pStatTask->pStatMonthList->m_ResultCount++;
					pStatTask->pStatMonthList->pStatClass->m_ResultCount++;
					
					
					for (i=0;i<pStatTask->iFieldNum;i++){
						if (pStatTask->aFHead[i].ValType==VT_INT){
							pRowValue->pRowAttr->aColValue[i].SetValue(atol( qry.field(pStatTask->aFHead[i].sFieldName).asString() ) );
						}	
						else if (pStatTask->aFHead[i].ValType==VT_STRING){
							pRowValue->pRowAttr->aColValue[i].SetValue( qry.field(pStatTask->aFHead[i].sFieldName).asString() );
							AllTrim(pRowValue->pRowAttr->aColValue[i].value.sValue);
						}	
					}
					
					pRowValue->pRowAttr->cFlag=OLD_STATE;
					
					pStatTask->InsertMemRowListTail(pRowValue);

#ifdef STAT_DEBUG2
					cout<<pStatTask->m_TableName<<" ITEMID: "<<pRowValue->ItemID<<"--->PAGE IN"<<endl;
#endif				
				}			
				else{
					sprintf(sTemp,"[%s:%d]中在表[%s] 中ITEM_ID=[%d] 的记录不存在!",_FILE_NAME_,__LINE__,pStatTask->m_TableName,pRowValue->ItemID);
					throw TException(sTemp);  				
				}
				
				
			}
			catch(TOCIException &oe){
				throw oe;
    		}
    	}
    	else{
    		pStatTask->iFoundCnt++;	
    	}
    	
    	for (i=0;i<pStatTask->iIdentFieldNum;i++){
			if (pStatTask->aFIdentHead[i].tFieldValue!=pRowValue->pRowAttr->aColValue[i])
				break;
		}
		
		if (i==pStatTask->iIdentFieldNum)
			return 0;	
		else 
			return 1;	
}

static void OutputRecord(void * data)
{
	TFieldRowValue * pRowValue;
	
	pRowValue=(TFieldRowValue *) data;
	
	if (pRowValue->pRowAttr){ 
		switch (pRowValue->pRowAttr->cFlag){
			case NEW_STATE:
					gpStatSave->Insert(pRowValue);
					pRowValue->pRowAttr->cFlag=OLD_STATE;
					break;
			case UPD_STATE:
					gpStatSave->Update(pRowValue);
					pRowValue->pRowAttr->cFlag=OLD_STATE;
					break;
			case OLD_STATE:
					break;
			case PAGE_STATE:
					break;
		}
	}
}

void TStatTask::OutputResult()
{
	 gpStatSave=new TStatSave(this);
	 RefreshIndexTrieList(StatTrieIndex,OutputRecord);
	 gpStatSave->InsertFlush();
	 gpStatSave->UpdateFlush();
	 delete gpStatSave;
	 cout<<"OutputResult: "<<this->m_TableName<<endl;
}

float TStatTask::GetFoundPercent()
{
	 float fFoundCnt,fCnt;
	 
	 if (iNotFoundCnt+iFoundCnt!=0){
		fFoundCnt=iFoundCnt;
		fCnt=iNotFoundCnt+iFoundCnt;
	 	fFoundPercent=fFoundCnt/fCnt*100;
	 }
	 else{
	 	fFoundPercent=100; 
	 }

#ifdef STAT_DEBUG	 
	cout<<this->m_TableName<<" 命中	 		："<<iFoundCnt<<endl;
	cout<<this->m_TableName<<" 未命中		："<<iNotFoundCnt<<endl;
	cout<<this->m_TableName<<" 命中率		："<<fFoundPercent<<endl;
	cout<<this->m_TableName<<" 节点数		："<<m_ResultCount<<endl;
	cout<<this->m_TableName<<" 周期节点数	："<<pStatMonthList->m_ResultCount<<endl;
	cout<<this->m_TableName<<" 总节点数		："<<pStatMonthList->pStatClass->m_ResultCount<<endl;
#endif
	 return fFoundPercent;

}

void TStatTask::FreeResult()
{
	 TFieldRowValue *pRowValue;
	 
	 GetFoundPercent();
	 
	 iFoundCnt=iNotFoundCnt=0;
	 
	
	 if (TotalCount<=CHANGEDATA_LEVEL1)
	 	return;
	 else if (TotalCount<=CHANGEDATA_LEVEL2){
	 	if ( fFoundPercent<99.0 && iIntervalTime<MAX_INTERVAL_TIME ){
			iIntervalTime=iIntervalTime*2;
		}
		else if (fFoundPercent<99.5){
			iIntervalTime=iIntervalTime;
		}
	 	else if (iIntervalTime>MIN_INTERVAL_TIME){
			iIntervalTime=iIntervalTime/2;
		}
	 }
	 else if (TotalCount<=CHANGEDATA_LEVEL3){
	 	if (fFoundPercent<99.0 && iIntervalTime<MAX_INTERVAL_TIME ){
			iIntervalTime=iIntervalTime*2;
		}
		else if (fFoundPercent<99.5){
			iIntervalTime=iIntervalTime;
		}
	 	else if (iIntervalTime>MIN_INTERVAL_TIME){
			iIntervalTime=iIntervalTime/2;
		}
	 }
	 else{
	 	if (fFoundPercent<99.0 && iIntervalTime<MAX_INTERVAL_TIME ){
			iIntervalTime=iIntervalTime*2;
		}
		else if (fFoundPercent<99.5){
			iIntervalTime=iIntervalTime;
		}
	 	else if (iIntervalTime>MIN_INTERVAL_TIME){
			iIntervalTime=iIntervalTime/2;
		}
	 } 

#ifdef STAT_DEBUG
	cout<<this->m_TableName<<" IntervalTime="<<iIntervalTime<<endl;
#endif	 

	
	 for (;m_RowListHead;){
	 	
	 	if (pStatMonthList->pStatClass->NowTime  -  m_RowListHead->pRowAttr->tAccessTime > iIntervalTime ){
	 			pRowValue=m_RowListHead;
	 			m_RowListHead=m_RowListHead->pRowAttr->pNext;
	 			
	 			if (m_RowListHead)
	 				m_RowListHead->pRowAttr->pPrev=NULL;
	 			else
	 				m_RowListTail=NULL;
	 			
	 			delete pRowValue->pRowAttr;
				pRowValue->pRowAttr=NULL;
				
				m_ResultCount--;
	 			pStatMonthList->m_ResultCount--;
				pStatMonthList->pStatClass->m_ResultCount--;
#ifdef STAT_DEBUG2
				cout<<this->m_TableName<<" ItemID: "<<pRowValue->ItemID<<"--->PAGE OUT "<<endl;
#endif				
		}
		else
			break;
	 }
	 
}

void TStatTask::ProcessStat()
{
	 int  i;
	 int  iLoopFlag=1,iResult;
	 char sIndexStr[500],sTemp[15];
	 TFieldRowValue  *pRowValue;
	 for (;iLoopFlag;){
	 	iLoopFlag=0;
	 	sIndexStr[0]='\0';
	 	for (i=0;i<iIdentFieldNum;i++){
	 		aFIdentHead[i].tFieldValue.ValType=aFIdentHead[i].ValType;
	 		iResult=pStatMonthList->pStatClass->GetFieldValue(aFIdentHead[i].iFieldID,&aFIdentHead[i].tFieldValue);
	 		
	 		if (iResult<0){
	 			char sTemp[200];
				sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d],统计指标标识m_StatItemType:[%d],FIELD_ID:[%d]统计域的统计值的获取在GetFieldValue外部函数中未定义，请增加GetFieldValue中对它的处理!",
					_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID,m_StatItemType,aFIdentHead[i].iFieldID);
				throw TException(sTemp);
	 		}
	 		else if (iResult==1)
	 			iLoopFlag=iResult;
	 		
	 		if (aFIdentHead[i].ValType!=aFIdentHead[i].tFieldValue.ValType)
	 			throw TException("TStatTask:ProcessStat中统计域中类型错误"); 
	 		
	 		if (aFIdentHead[i].iIndexFlag==1){
			
	 			if (aFIdentHead[i].ValType==VT_INT){
	 				sprintf(sTemp,"%ld",aFIdentHead[i].tFieldValue.value.lValue);
	 				strcat(sIndexStr,sTemp);
	 			}
	 			else{
	 				AllTrim(aFIdentHead[i].tFieldValue.value.sValue);
	 				strcat(sIndexStr,aFIdentHead[i].tFieldValue.value.sValue);
	 			}
	 		}
	 	}
	 	
	 	for (i=0;i<iResultFieldNum;i++){
	 		aFResultHead[i].tFieldValue.ValType=aFResultHead[i].ValType;
	 		iResult=pStatMonthList->pStatClass->GetFieldValue(aFResultHead[i].iFieldID,&aFResultHead[i].tFieldValue);
	 		
	 		if (iResult<0){
	 			char sTemp[200];
				sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d],统计指标标识m_StatItemType:[%d],FIELD_ID:[%d]统计域的统计值的获取在GetFieldValue外部函数中未定义，请增加GetFieldValue中对它的处理!",
					_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID,m_StatItemType,aFResultHead[i].iFieldID);
				throw TException(sTemp);
	 		}
	 		else if ( (iResult==1)&&(iLoopFlag==0) ){
	 			char sTemp[200];
	 			sprintf(sTemp,"[%s:%d] 统计类号:[%d],统计任务号:[%d],统计指标标识m_StatItemType:[%d],FIELD_ID:[%d]统计值域会返回多次取值，不能脱离开能返回多次取值的统计域而单独存在，请修改统计指标配置!",
					_FILE_NAME_,__LINE__,pStatMonthList->pStatClass->iStatClassID,m_TaskID,m_StatItemType,aFResultHead[i].iFieldID);
					
	 			throw TException(sTemp); 
	 		}
	 		
	 		if (pStatMonthList->pStatClass->iRunMode==1){
	 				aFResultHead[i].tFieldValue.value.lValue=-aFResultHead[i].tFieldValue.value.lValue;
	 		}
	 		
	 	}
	 	
	 	pRowValue=(TFieldRowValue *)SearchIndexTrieList(StatTrieIndex,sIndexStr,strlen(sIndexStr),(void *)this,CmpValue);
	 	
	 	if (pRowValue){
	 		TFieldRowValue *pPrevRow,*pNextRow;
	 		for (i=iIdentFieldNum;i<iFieldNum;i++)
	 			pRowValue->pRowAttr->aColValue[i]+=aFHead[i].tFieldValue;
			
			if (pRowValue->pRowAttr->cFlag!=NEW_STATE)
				pRowValue->pRowAttr->cFlag=UPD_STATE;
			
			pRowValue->pRowAttr->tAccessTime=time(NULL);
			TouchTrieListByPriKey(&StatTrieIndex,sIndexStr,strlen(sIndexStr));
			
			
			pPrevRow=pRowValue->pRowAttr->pPrev;
			pNextRow=pRowValue->pRowAttr->pNext;
			
			if (pNextRow!=NULL){
			
				if (pPrevRow==NULL){
					m_RowListHead=pNextRow;
					pRowValue->pRowAttr->pNext=NULL;
					m_RowListHead->pRowAttr->pPrev=NULL;
				}
				else{
					pPrevRow->pRowAttr->pNext=pNextRow;
					pNextRow->pRowAttr->pPrev=pPrevRow;
					pRowValue->pRowAttr->pNext=NULL;
					pRowValue->pRowAttr->pPrev=NULL;
				}
				
				InsertMemRowListTail(pRowValue);
				
			}
			
		}	
	 	else{ 
			pRowValue=new TFieldRowValue(GetNextItemID(),iFieldNum);
			
			m_ResultCount++;
			pStatMonthList->m_ResultCount++;
			pStatMonthList->pStatClass->m_ResultCount++;
			
			TotalCount++;
			pStatMonthList->TotalCount++;
			pStatMonthList->pStatClass->TotalCount++;
			
			pRowValue->pRowAttr->cFlag=NEW_STATE;
			
			InsertMemRowListTail(pRowValue);
			
			for(i=0;i<iFieldNum;i++)
				pRowValue->pRowAttr->aColValue[i]=aFHead[i].tFieldValue;
			
			if (InsertIndexTrieList(&StatTrieIndex,sIndexStr,strlen(sIndexStr),pRowValue,1)<0)
					throw TException("TStatTask:InsertIndexTrieList中分配指针失败"); 	
	 	}
	 	
	 }
	  
}


StatMonthList::StatMonthList(int m_Month,int iStatClassID,TStatClass *pStatClass)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[SQLBUFF];
	TStatTask	*pStatTask;
	this->m_Month=m_Month;
	this->iStatClassID=iStatClassID;
	this->pNext=NULL;
	this->pStatClass=pStatClass;
	this->tAccessTime=time(NULL);
	this->pStatTaskHead=NULL;
	this->m_ResultCount=0;
	this->TotalCount=0;
	try{
		qry.close();
		sprintf(sSql,"SELECT TASK_ID,STAT_ITEM_TYPE FROM STAT_TASK WHERE STATUS='A' \
					AND TRUNC(BEGIN_DATE)<=TRUNC(SYSDATE) AND TRUNC(END_DATE)>=TRUNC(SYSDATE) \
					AND	STAT_ITEM_TYPE IN (SELECT STAT_ITEM_TYPE FROM STAT_ITEM_TYPE \
					WHERE STAT_CLASS_ID  = %d )", this->iStatClassID);
        qry.setSQL(sSql);
        qry.open();
        for(;qry.next();){
        	int iTaskID,iStatItemType;
        	iTaskID=qry.field("TASK_ID").asInteger();
        	iStatItemType=qry.field("STAT_ITEM_TYPE").asInteger();
        	pStatTask=new TStatTask(iTaskID,this->m_Month,iStatItemType,this);
        	pStatTask->pNext=pStatTaskHead;
        	pStatTaskHead=pStatTask;
        }
        qry.close();
	}
	catch(TOCIException &oe){
		throw oe;
    }
}

StatMonthList::~StatMonthList()
{
	TStatTask	*pStatTask;
	
	for(pStatTask=pStatTaskHead;pStatTask;){
		pStatTaskHead=pStatTask->pNext;
		delete pStatTask;
		pStatTask=pStatTaskHead;
	}
	this->pNext=NULL;
	this->pStatClass=NULL;
	this->pStatTaskHead=NULL;
}

void StatMonthList::OutputResult()
{
	TStatTask	*pStatTask;

	for(pStatTask=pStatTaskHead;pStatTask;pStatTask=pStatTask->pNext)
		pStatTask->OutputResult();
}

void StatMonthList::FreeResult()
{
	TStatTask	*pStatTask;

	for(pStatTask=pStatTaskHead;pStatTask;pStatTask=pStatTask->pNext)
		pStatTask->FreeResult();
}

void StatMonthList::ProcessStat()
{
	TStatTask	*pStatTask;
	
	tAccessTime=time(NULL);
	
	for(pStatTask=pStatTaskHead;pStatTask;pStatTask=pStatTask->pNext)
		pStatTask->ProcessStat();
	
	
}
    

TStatClass::TStatClass(int iStatClassID)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[SQLBUFF];
		
	InitTrieList();
	this->iStatClassID=iStatClassID;
	iRunMode=0;
	iPageInOutMode=1;
	iRecordNum=0;
	pStatMonthListHead=NULL;
	m_ResultCount=0;
	TotalCount=0;
	strcpy(sTableName," ");
	BreakPoint=0;
	bFlushBreakPointFlag=false;
	bOpenFlag=false;
	strcpy(sStatClassType,"NOP");
	
	sprintf(sSql,"SELECT  STAT_CLASS_TYPE FROM STAT_CLASS WHERE STAT_CLASS_ID=%d ",iStatClassID);
	
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	if ( !qry.next() ){ 
    	char sTemp[200];
		sprintf(sTemp,"[%s:%d] 统计类号:[%d]在StatClass表中没有配置，请增加配置!",_FILE_NAME_,__LINE__,iStatClassID);
		throw TException(sTemp);
    }
    
    strcpy(sStatClassType,qry.field("STAT_CLASS_TYPE").asString());
    AllTrim(sStatClassType);
    qry.close();
   	
   	if (!strcmp(sStatClassType,UPDATE_TYPE_CLASS) )
   		ClearBreakPoint(iStatClassID);
    
    cout<<"统计类号：["<<iStatClassID<<"] 统计类的类型：["<<sStatClassType<<"]"<<endl;
	
	
}

TStatClass::TStatClass()
{
	InitTrieList();
	iStatClassID=0;
	iRunMode=0;
	iPageInOutMode=1;
	iRecordNum=0;
	pStatMonthListHead=NULL;
	m_ResultCount=0;
	TotalCount=0;
	strcpy(sTableName," ");
	BreakPoint=0;
	bFlushBreakPointFlag=false;
	bOpenFlag=false;
	strcpy(sStatClassType,"NOP");
}

void TStatClass::SetClassID(int iStatClassID)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[SQLBUFF];
	
	this->iStatClassID=iStatClassID;
	
	sprintf(sSql,"SELECT  STAT_CLASS_TYPE FROM STAT_CLASS WHERE STAT_CLASS_ID=%d ",iStatClassID);
	
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	if ( !qry.next() ){ 
    	char sTemp[200];
		sprintf(sTemp,"[%s:%d] 统计类号:[%d]在StatClass表中没有配置，请增加配置!",_FILE_NAME_,__LINE__,iStatClassID);
		throw TException(sTemp);
    }
    
    strcpy(sStatClassType,qry.field("STAT_CLASS_TYPE").asString());
    AllTrim(sStatClassType);
    qry.close();
   
    cout<<"统计类号：["<<iStatClassID<<"] 统计类的类型：["<<sStatClassType<<"]"<<endl;
}

TStatClass::~TStatClass()
{
	StatMonthList	*pStatMonthList;
	
	for(pStatMonthList=pStatMonthListHead;pStatMonthList;){
		pStatMonthListHead=pStatMonthList->pNext;
		delete pStatMonthList;
		pStatMonthList=pStatMonthListHead;
	}
}

void TStatClass::SetRunMode(int iRunMode)
{
		this->iRunMode=iRunMode;
}

void TStatClass::SetPageInOutMode(int iPageInOutMode)
{
		this->iPageInOutMode=iPageInOutMode;
}


void TStatClass::OutputResult()
{
	StatMonthList	*pStatMonthList, *pStatMonthPreList;
	
	iOutPutFlag=true;
	
	NowTime=time(NULL);		
	
	for(pStatMonthList=pStatMonthListHead;pStatMonthList;pStatMonthList=pStatMonthList->pNext)
		pStatMonthList->OutputResult();
	
	for(pStatMonthList=pStatMonthListHead,pStatMonthPreList=NULL;pStatMonthList;){
		if (NowTime-pStatMonthList->tAccessTime>MONTH_INTERVAL_TIME){
			if (pStatMonthPreList==NULL){
				pStatMonthListHead=pStatMonthList->pNext;
				delete pStatMonthList;
				pStatMonthList=pStatMonthListHead;
			}
			else{
				pStatMonthPreList->pNext=pStatMonthList->pNext;
				delete pStatMonthList;
				pStatMonthList=pStatMonthPreList->pNext;
			}
		}
		else{
			pStatMonthPreList=pStatMonthList;
			pStatMonthList=pStatMonthPreList->pNext;
		}
	}
	
	SavePointToDB();
}

void TStatClass::FreeResult()
{
	StatMonthList	*pStatMonthList;
	
	if (!iOutPutFlag)
		OutputResult();
		
	NowTime=time(NULL);	
	
	if (!iPageInOutMode)
		return;
	
	for(pStatMonthList=pStatMonthListHead;pStatMonthList;pStatMonthList=pStatMonthList->pNext)
		pStatMonthList->FreeResult();
	
}

void TStatClass::ProcessStat()
{
	StatMonthList	*pStatMonthList;
	int m_Month;
	
	iOutPutFlag=false;
	
	m_Month=GetCycle();
	
	for(pStatMonthList=pStatMonthListHead;pStatMonthList;pStatMonthList=pStatMonthList->pNext)
		if (pStatMonthList->m_Month==m_Month)
			break;

	if 	(pStatMonthList==NULL){
		pStatMonthList=new StatMonthList(m_Month,iStatClassID,this);
		pStatMonthList->pNext=pStatMonthListHead;
		pStatMonthListHead=pStatMonthList;
	}
		
	pStatMonthList->ProcessStat();
	
	if ((++iRecordNum) %50000 == 0 )
		cout<<"已处理记录数:"<<iRecordNum<<endl;
}

void TStatClass::Commit()
{
	if ( !strcmp(sStatClassType,INSERT_TYPE_CLASS) )
		gpDBLink.commit();
	else if (!strcmp(sStatClassType,UPDATE_TYPE_CLASS) ){
		/*if (!bOpenFlag)*/	
			gpDBLink.commit();
	}
	else{
		throw TException("统计类型不正确，请修改STATE_CLASS中STATE_CLASS_TYPE 为 INS/UPD 类型。");
	}
}

bool TStatClass::GetRecord()
{
	if ( !strcmp(sStatClassType,INSERT_TYPE_CLASS) )
		return GetRecord_INS();
	else if (!strcmp(sStatClassType,UPDATE_TYPE_CLASS) )
		/*return GetRecord_UDP();*/
		return GetRecord_INS();
	else{
		throw TException("统计类型不正确，请修改STATE_CLASS中STATE_CLASS_TYPE 为 INS/UPD 类型。");
	}
}

bool TStatClass::GetRecord_UDP()
{
	try{
GetNextTableName_Lable2:		
		for (;;){
			if (!bOpenFlag){
				TOCIQuery qry(&gpDBLink);
				char sSql[SQLBUFF];
							
				if (iRunMode==0)
						sprintf(sSql,"SELECT TABLE_NAME,BREAKPOINT FROM STAT_CLASS_DATA_SOURCE WHERE TABLE_NAME>'%s' AND STATE='%s' AND STAT_CLASS_ID=%d ORDER BY TABLE_NAME",sTableName,RUN_MODE_NORMAL,iStatClassID);
				else if (iRunMode==1)
						sprintf(sSql,"SELECT TABLE_NAME,BREAKPOINT FROM STAT_CLASS_DATA_SOURCE WHERE TABLE_NAME>'%s' AND STATE='%s' AND STAT_CLASS_ID=%d ORDER BY TABLE_NAME",sTableName,RUN_MODE_REBACK,iStatClassID);
				else 
						throw TException("RUNMODE ERROR!");
						
				
				qry.close();
				qry.setSQL(sSql);
				qry.open();
    			
    			if ( !qry.next() ){ 
    				strcpy(sTableName," ");
    				bOpenFlag=false;
    				return false;
    			}
    			else{
    				strcpy(sTableName,qry.field("TABLE_NAME").asString());
    				AllTrim(sTableName);
    				bOpenFlag=true;
    				bOpenCurFlag=false;
    			}/*else end */
			
			} /* if end */
			
			break;
		}/* for end */
		
		for(;;){
			if (!bOpenCurFlag){
				OpenCursor(sTableName,0,0);
				bOpenCurFlag=true;
			}
			
			if ( GetRecordFromTable(lCurPoint) ){
				break;
			}
			else{
				CloseCursor();
				bOpenCurFlag=false;
				break;
			}
		}/* for end */
		
		if (!bOpenCurFlag){
			bOpenFlag=false;
			goto GetNextTableName_Lable2;						 
		}
		else 
			return true;
		
	}/* try end */
	catch(TOCIException &oe){
		throw oe;
    }
}

bool TStatClass::GetRecord_INS()
{
	try{
GetNextTableName_Lable:		
		for (;;){
			if (!bOpenFlag){
				TOCIQuery qry(&gpDBLink);
				char sSql[SQLBUFF];
				POINT_TYPE	lMaxPoint,lMinPoint;
				
				if (iRunMode==0)
						sprintf(sSql,"SELECT TABLE_NAME,BREAKPOINT FROM STAT_CLASS_DATA_SOURCE WHERE TABLE_NAME>'%s' AND STATE='%s' AND STAT_CLASS_ID=%d ORDER BY TABLE_NAME",sTableName,RUN_MODE_NORMAL,iStatClassID);
				else if (iRunMode==1)
						sprintf(sSql,"SELECT TABLE_NAME,BREAKPOINT FROM STAT_CLASS_DATA_SOURCE WHERE TABLE_NAME>'%s' AND STATE='%s' AND STAT_CLASS_ID=%d ORDER BY TABLE_NAME",sTableName,RUN_MODE_REBACK,iStatClassID);
				else 
						throw TException("RUNMODE ERROR!");
						
				
				qry.close();
				qry.setSQL(sSql);
				qry.open();
    			
    			if ( !qry.next() ){ 
    				strcpy(sTableName," ");
    				bOpenFlag=false;
    				return false;
    			}
    			else{
    				strcpy(sTableName,qry.field("TABLE_NAME").asString());
    				AllTrim(sTableName);
    				lBeginPoint=POINT_TYPE( qry.field("BREAKPOINT").asFloat() );
    				lMaxPoint=GetMaxPoint(sTableName,MAX_POINT_VALUE);
    				lMinPoint=GetMinPoint(sTableName,0);
    				
    				if (lMaxPoint==0 && lMinPoint==0)
    					continue;	
    				
    				if (lMaxPoint-lMinPoint>TURNOUT_MINUSVALUE)
    					bTurnOutFlag=true;
    				else
    					bTurnOutFlag=false;
    				
    				if (bTurnOutFlag){
    					lMaxPoint1=lMaxPoint;
    					lMinPoint1=GetMinPoint(sTableName,TURNOUT_MINUSVALUE);
    					lMaxPoint2=GetMaxPoint(sTableName,TURNOUT_MINUSVALUE);
    					lMinPoint2=lMinPoint;
    					
    					if (lBeginPoint==lMaxPoint2)
    						continue;
    					else if (lBeginPoint==0){
    						lBeginPoint=lMinPoint1-1;
    						lEndPoint=lMaxPoint1;
    					}
    					else if (lBeginPoint>=lMinPoint1){
    						lEndPoint=lMaxPoint1;
    					}
    					else if (lBeginPoint<lMaxPoint2){
    						lEndPoint=lMaxPoint2;
    					}
    				}
    				else{
    					lMaxPoint1=lMaxPoint;
    					lMinPoint1=lMinPoint;
    					lMaxPoint2=0;
    					lMinPoint2=0;
    					
    					if (lBeginPoint==lMaxPoint1)
    						continue;
    					else if (lBeginPoint==0)
    						lBeginPoint=lMinPoint1-1;
    					
    					lEndPoint=lMaxPoint1;
    			  					
    				}	
    				
    				bOpenFlag=true;
    				bOpenCurFlag=false;
    			}/*else end */
			
			} /* if end */
			
			break;
		}/* for end */
		
		for(;;){
			if (!bOpenCurFlag){
				if (lBeginPoint+INC_POINT_VALUE<lEndPoint)
					OpenCursor(sTableName,lBeginPoint+1,lBeginPoint+INC_POINT_VALUE);
				else 
					OpenCursor(sTableName,lBeginPoint+1,lEndPoint);
				
				bOpenCurFlag=true;
			}
			
			if ( GetRecordFromTable(lCurPoint) ){
				SavePoint(lCurPoint);
				break;
			}
			else{
				CloseCursor();
				
				if (lBeginPoint+INC_POINT_VALUE<lEndPoint){
					SavePoint(lBeginPoint+INC_POINT_VALUE);
					lBeginPoint=lBeginPoint+INC_POINT_VALUE;
					bOpenCurFlag=false;
					continue;
				}	
				else{
					SavePoint(lEndPoint);
					lBeginPoint=lEndPoint;
					bOpenCurFlag=false;
					if (bTurnOutFlag && lEndPoint==lMaxPoint1 ){
						lBeginPoint=lMinPoint2-1;
						lEndPoint=lMaxPoint2;
						continue;
					}	
					else 
						break;
				}
			}
			
		}/* for end */
		
		if (!bOpenCurFlag){
			bOpenFlag=false;
			SavePointToDB();
			goto GetNextTableName_Lable;						 
		}
		else 
			return true;
		
	}/* try end */
	catch(TOCIException &oe){
		throw oe;
    }
}

void TStatClass::SavePoint(POINT_TYPE BreakPoint)
{
	this->BreakPoint=BreakPoint;
	this->bFlushBreakPointFlag=true;
}

void TStatClass::SavePointToDB()
{
	try{
		if (bFlushBreakPointFlag){
			TOCIQuery qry(&gpDBLink);
			char sSql[SQLBUFF];
#ifdef BIT32OS		
			sprintf(sSql,"UPDATE STAT_CLASS_DATA_SOURCE SET BREAKPOINT=%llu WHERE TABLE_NAME='%s' AND STAT_CLASS_ID=%d ",BreakPoint,sTableName,iStatClassID);
#else /* BIT64OS */
			sprintf(sSql,"UPDATE STAT_CLASS_DATA_SOURCE SET BREAKPOINT=%lu WHERE TABLE_NAME='%s' AND STAT_CLASS_ID=%d ",BreakPoint,sTableName,iStatClassID);
#endif		
			qry.close();
			qry.setSQL(sSql);
			qry.execute();
			bFlushBreakPointFlag=false;
		}
		
		return ;
	}
	catch(TOCIException &oe){
		throw oe;
    }
}


TStatSave::TStatSave(TStatTask *pStatTask)
{
    this->pStatTask=pStatTask;
    Insert_RecCnt = 0;
    Update_RecCnt = 0;
   	strcpy ( m_TableName, pStatTask->m_TableName);
    
}

TStatSave::~TStatSave()
{
}

void TStatSave::SetTableName(const char * sName)
{
    strcpy ( m_TableName, sName);
}

void TStatSave::Insert(TFieldRowValue * pRowValue)
{
	int i;
	
	for(i = 0; i<pStatTask->iFieldNum;i++){
		switch(pRowValue->pRowAttr->aColValue[i].ValType){
			case VT_INT:
				sprintf( InsertBuffer[Insert_RecCnt][i],"%ld",pRowValue->pRowAttr->aColValue[i].value.lValue);
				break;
			case VT_STRING:
				sprintf( InsertBuffer[Insert_RecCnt][i],"%s",pRowValue->pRowAttr->aColValue[i].value.sValue);
				break;
		}
	}
	
	sprintf( InsertBuffer[Insert_RecCnt][i++],"%d",pRowValue->ItemID);
	sprintf( InsertBuffer[Insert_RecCnt][i++],"%s",pRowValue->pRowAttr->GetCTime());
	
    Insert_RecCnt++;
    if ( Insert_RecCnt == SAVE_BUFSIZE){
        InsertFlush();
    }
}
void TStatSave::InsertFlush()
{
	if ( Insert_RecCnt == 0)
        return;

    try{
		TOCIQuery qry(&gpDBLink);
		char sSql[2000];
		char sColumn[1000];
		char sColumnParam[1000];
		int i;
        
        strcpy(sColumn,"ITEM_ID");
		strcpy(sColumnParam,":ITEM_ID");
		strcat(sColumn,",MODI_DATE");
		strcat(sColumnParam,",SYSDATE");
		strcat(sColumn,",ACCESS_DATE");
		strcat(sColumnParam,",TO_DATE(:ACCESS_DATE,'YYYYMMDDHH24MISS')");
		
		for(i=0; i<pStatTask->iFieldNum;i++){
			strcat(sColumn,",");
			strcat(sColumn,pStatTask->aFHead[i].sFieldName);
			strcat(sColumnParam,",:");
			strcat(sColumnParam,pStatTask->aFHead[i].sFieldName);
		}
		
		sprintf(sSql,"Insert into %s( %s) Values(%s) ", m_TableName,sColumn,sColumnParam);
		qry.close();
		qry.setSQL(sSql);

       
		for(i = 0; i<pStatTask->iFieldNum;i++){
			qry.setParamArray(pStatTask->aFHead[i].sFieldName,   (char **)&(InsertBuffer[0][i]), sizeof (InsertBuffer[0]),sizeof(InsertBuffer[0][0]));
		}
		
		qry.setParamArray("ITEM_ID",   		(char **)&(InsertBuffer[0][i++]), sizeof (InsertBuffer[0]),sizeof(InsertBuffer[0][0]));
		qry.setParamArray("ACCESS_DATE",   	(char **)&(InsertBuffer[0][i++]), sizeof (InsertBuffer[0]),sizeof(InsertBuffer[0][0]));
		qry.execute( Insert_RecCnt);
		Insert_RecCnt = 0;
		
		//printf("SQL:%s\n", sSql);
	}
	catch(TOCIException &oe)
	{
		cout<<"TOCIException catched"<<endl;
		cout<<oe.GetErrMsg()<<endl;		
		cout<<oe.getErrSrc()<<endl;
		throw oe;
	}
}
void TStatSave::Update(TFieldRowValue * pRowValue)
{
	int i,j;
	
	for(i=0,j=pStatTask->iIdentFieldNum; j<pStatTask->iFieldNum;i++,j++){
		switch(pRowValue->pRowAttr->aColValue[j].ValType){
			case VT_INT:
				sprintf( UpdateBuffer[Update_RecCnt][i],"%ld",pRowValue->pRowAttr->aColValue[j].value.lValue);
				break;
			case VT_STRING:
				sprintf( UpdateBuffer[Update_RecCnt][i],"%s",pRowValue->pRowAttr->aColValue[j].value.sValue);
				break;
		}
	}
	
	sprintf( UpdateBuffer[Update_RecCnt][i++],"%d",pRowValue->ItemID);
	sprintf( UpdateBuffer[Update_RecCnt][i++],"%s",pRowValue->pRowAttr->GetCTime());
    Update_RecCnt++;
    if ( Update_RecCnt == SAVE_BUFSIZE){
        UpdateFlush();
    }
    
}
void TStatSave::UpdateFlush()
{
	if ( Update_RecCnt == 0)
        return;

    try{
		TOCIQuery qry(&gpDBLink);
		char sSql[2000];
		char sSetSql[1000];
		char sWhereSql[1000];
		int i;
        
        strcpy(sWhereSql,"ITEM_ID=:ITEM_ID");
		strcpy(sSetSql,"MODI_DATE=SYSDATE");
		strcat(sSetSql,",ACCESS_DATE=TO_DATE(:ACCESS_DATE,'YYYYMMDDHH24MISS')" );	
		for(i=0; i<pStatTask->iResultFieldNum;i++){
			strcat(sSetSql,",");
			strcat(sSetSql,pStatTask->aFResultHead[i].sFieldName);
			strcat(sSetSql,"=:");
			strcat(sSetSql,pStatTask->aFResultHead[i].sFieldName);
		}
		
		sprintf(sSql,"UPDATE %s SET %s WHERE %s ", m_TableName,sSetSql,sWhereSql);
		qry.close();
		qry.setSQL(sSql);

       
		for(i = 0; i<pStatTask->iResultFieldNum;i++){
			qry.setParamArray(pStatTask->aFResultHead[i].sFieldName,(char **)&(UpdateBuffer[0][i]), sizeof (UpdateBuffer[0]),sizeof(UpdateBuffer[0][0]));
		}
		qry.setParamArray("ITEM_ID",   (char **)&(UpdateBuffer[0][i++]), sizeof (UpdateBuffer[0]),sizeof(UpdateBuffer[0][0]));
		qry.setParamArray("ACCESS_DATE",   (char **)&(UpdateBuffer[0][i++]), sizeof (UpdateBuffer[0]),sizeof(UpdateBuffer[0][0]));
		
		qry.execute( Update_RecCnt);
		Update_RecCnt = 0;
		
		//printf("SQL:%s\n", sSql);
	}
	catch(TOCIException &oe)
	{
		cout<<"TOCIException catched"<<endl;
		cout<<oe.GetErrMsg()<<endl;		
		cout<<oe.getErrSrc()<<endl;
		throw oe;
	}
}
