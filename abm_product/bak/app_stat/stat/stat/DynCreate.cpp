/*VER: 1*/ 
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "DynCreate.h"
#include "TOCIQuery.h"
#include <ctype.h>

using namespace std;

extern TOCIDatabase gpDBLink;

static char * _FILE_NAME_= "DynCreate.cpp";

typedef struct TableStorageStruct
{
	char	sTableName[31];
	char	sTableSpaceName[31];
	int	iPctFree;
	int	iPctUsed;
	int  	iIniTrans;
	int	iMaxTrans;
	int 	iInitialExtent;
	int	iNextExtent;
	int	iMinExtents;
	int	iMaxExtents;
	int	iPctIncrease;
}TABLE_STORAGE;

typedef struct IndexStorage
{
	char sTableSpaceName[31];
	char sIndexName[31];
	char sUniqueness[10];
	int	iIniTrans;
	int	iMaxTrans;
	int	iInitialExtent;
	int	iNextExtent;
	int	iMinExtents;
	int	iMaxExtents;
	int	iPctIncrease;
}INDEX_STORAGE;

static void strupr(char * sTemp)
{
	int i;
	for (i=0;i<strlen(sTemp);i++)
		sTemp[i]=toupper(sTemp[i]);
}

static void S_DupTable(const char *sTemplate,const char *sTableName,TOCIDatabase *pDBLink)
{
		TABLE_STORAGE tabStorage;
		char  sDynamic[2000];
		TOCIQuery qry(pDBLink);
			
	    static char * sQrySql =  "SELECT \
	     TABLE_NAME,\
	     TABLESPACE_NAME,\
	     PCT_FREE,\
	     PCT_USED,\
	     INI_TRANS,\
	     MAX_TRANS,\
	     INITIAL_EXTENT,\
	     NEXT_EXTENT,\
	     MIN_EXTENTS,\
	     MAX_EXTENTS,\
	     PCT_INCREASE\
	    FROM USER_TABLES\
	    WHERE TABLE_NAME = :sTemplate";
		
	    strupr((char *) sTemplate);
	    strupr((char *) sTableName);
	    
	    try {
	        //判断表是否存在
	        qry.close();
	        qry.setSQL("SELECT * FROM TAB WHERE TNAME=:TableName");
	        qry.setParameter("TableName",sTableName);
	        qry.open();
	        if ( qry.next()){
	            //表已存在
	            qry.close();
	            return;
	        }
		qry.close();
	        
	        qry.setSQL(sQrySql);
	        
	        qry.setParameter("sTemplate", sTemplate);
	        
	        qry.open();
	        
	        if ( !qry.next())
	            throw TException("DUP_TABLE:TEMPLATE TABLE not exists");
	        
	        strcpy ( tabStorage.sTableName, qry.field("TABLE_NAME").asString());
	        strcpy ( tabStorage.sTableSpaceName, qry.field("TABLESPACE_NAME").asString());
	        tabStorage.iPctFree = qry.field("PCT_FREE").asInteger();
	        tabStorage.iPctUsed = qry.field("PCT_USED").asInteger();
	        tabStorage.iIniTrans = qry.field("INI_TRANS").asInteger();
	        tabStorage.iMaxTrans = qry.field("MAX_TRANS").asInteger();
	        tabStorage.iInitialExtent = qry.field("INITIAL_EXTENT").asInteger();
	        tabStorage.iNextExtent = qry.field("NEXT_EXTENT").asInteger();
	        tabStorage.iMinExtents = qry.field("MIN_EXTENTS").asInteger();
	        tabStorage.iMaxExtents = qry.field("MAX_EXTENTS").asInteger();
	        tabStorage.iPctIncrease = qry.field("PCT_INCREASE").asInteger();
	        qry.close();
	        sprintf(sDynamic,
	        "CREATE TABLE %s  \
	        PCTFREE %d \
	        PCTUSED %d \
	        INITRANS %d \
	        MAXTRANS %d \
	        TABLESPACE %s \
	        STORAGE (INITIAL %d \
	        NEXT %d \
	        PCTINCREASE %d \
	        MINEXTENTS %d \
	        MAXEXTENTS %d) \
	        as SELECT * FROM %s WHERE rownum < 1",
	        sTableName,
	        tabStorage.iPctFree,
	        tabStorage.iPctUsed,
	        tabStorage.iIniTrans,
	        tabStorage.iMaxTrans,
	        tabStorage.sTableSpaceName,
	        tabStorage.iInitialExtent,
	        tabStorage.iNextExtent,
	        tabStorage.iPctIncrease,
	        tabStorage.iMinExtents,
	        tabStorage.iMaxExtents,
	        sTemplate);
	        qry.close(); 
	        qry.setSQL(sDynamic);
	        qry.execute();
	
	
	    }
		catch (TOCIException &oe) {
		    
			cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			cout<<qry.getSQLCode()<<endl;
		}
	
}

static void  S_DupTableIndex(const char * sTemplateTable,const char * sTableName,const char * sIndExt,TOCIDatabase *pDBLink)
{
	
		INDEX_STORAGE indStorage;
	    TOCIQuery qry(pDBLink);
		
		char sDynamic[2000];
		
		static char * sIndexSql = " SELECT\
	    TABLESPACE_NAME,\
	    INDEX_NAME,\
	    UNIQUENESS,\
	    INI_TRANS,\
	    MAX_TRANS,\
	    INITIAL_EXTENT,\
	    NEXT_EXTENT,\
	    MIN_EXTENTS,\
	    MAX_EXTENTS,\
	    PCT_INCREASE\
	    FROM \
	    USER_INDEXES \
	    WHERE \
	    TABLE_NAME = :sTemplateTable";
	
	    static char * sColumnSql = "SELECT\
	    COLUMN_NAME\
	    FROM \
	    USER_IND_COLUMNS \
	    WHERE \
	    INDEX_NAME=:sIndexName\
	    ORDER BY COLUMN_POSITION";
	
		strupr((char *) sTemplateTable);
	    strupr((char *) sTableName);
	    strupr((char *) sIndExt);
	    
	    try {
		qry.close();
	
	        qry.setSQL(sIndexSql);
	        
	        qry.setParameter("sTemplateTable", sTemplateTable);
	        
	        qry.open();
	        
	        while (qry.next()){
	            strcpy ( indStorage.sIndexName, qry.field("INDEX_NAME").asString());
	            strcpy ( indStorage.sTableSpaceName, qry.field("TABLESPACE_NAME").asString());
	            strcpy ( indStorage.sUniqueness, qry.field("UNIQUENESS").asString());
	            
	            indStorage.iIniTrans = qry.field("INI_TRANS").asInteger();
	            indStorage.iMaxTrans = qry.field("MAX_TRANS").asInteger();
	            indStorage.iInitialExtent = qry.field("INITIAL_EXTENT").asInteger();
	            indStorage.iNextExtent = qry.field("NEXT_EXTENT").asInteger();
	            indStorage.iMinExtents = qry.field("MIN_EXTENTS").asInteger();
	            indStorage.iMaxExtents = qry.field("MAX_EXTENTS").asInteger();
	            indStorage.iPctIncrease = qry.field("PCT_INCREASE").asInteger();
	
	            
	            //判断索引是否存在
	            TOCIQuery qry4(pDBLink);
	            char sIndexName[31];
	            sprintf(sIndexName,"%s_%s",indStorage.sIndexName,sIndExt);
	            qry4.close();
	            qry4.setSQL("SELECT * FROM USER_INDEXES WHERE INDEX_NAME=:IndexName");
	            qry4.setParameter("IndexName",sIndexName);
	            qry4.open();
	            if ( qry4.next()){
	                //索引已存在
	                qry4.close();
	                return;
	            }
	
	            TOCIQuery qry2(pDBLink);
	    		char  sIndColumns[300];
	            qry2.close(); 
	            qry2.setSQL(sColumnSql);
	            qry2.setParameter("sIndexName", indStorage.sIndexName);
	            qry2.open();
	            sIndColumns[0] = '\0';
	            
	            while (qry2.next()){
	    			strcat( sIndColumns, qry2.field("COLUMN_NAME").asString());
	    			strcat( sIndColumns, ",");
	            }
	            qry2.close();
	            
	            if( strncmp(indStorage.sUniqueness,"UNIQUE",6) )
					indStorage.sUniqueness[0] = '\0';
				sIndColumns[ strlen(sIndColumns)-1] = '\0';
		 		
		 		sprintf (sDynamic, 
				"CREATE %s INDEX %s_%s ON %s (%s) \
				INITRANS %d \
				MAXTRANS %d \
				TABLESPACE %s \
				STORAGE (INITIAL %d \
				NEXT %d \
				PCTINCREASE %d \
				MINEXTENTS %d \
				MAXEXTENTS %d)", 
				indStorage.sUniqueness,
				indStorage.sIndexName,
				sIndExt, 
				sTableName,
				sIndColumns,
				indStorage.iIniTrans,
				indStorage.iMaxTrans,
				indStorage.sTableSpaceName,
				indStorage.iInitialExtent,
				indStorage.iNextExtent,
				indStorage.iPctIncrease,
				indStorage.iMinExtents,
				indStorage.iMaxExtents
				);
				
				TOCIQuery qry3(pDBLink);
			        qry3.close();	
				qry3.setSQL(sDynamic);
				qry3.execute();
	        }
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
	
}

void  DupTable(const char *sTemplate,const char *sTableName)
{
	S_DupTable(sTemplate,sTableName,&gpDBLink);
}

void  DupTable(const char *sTemplate,const char *sTableName,TOCIDatabase *pDBLink)
{
	S_DupTable(sTemplate,sTableName,pDBLink);
}

void  DupTableIndex(const char * sTemplateTable,const char * sTableName,const char * sIndExt)
{
	S_DupTableIndex(sTemplateTable,sTableName,sIndExt,&gpDBLink);
}

void  DupTableIndex(const char * sTemplateTable,const char * sTableName,const char * sIndExt,TOCIDatabase *pDBLink)
{
	S_DupTableIndex(sTemplateTable,sTableName,sIndExt,pDBLink);
}


