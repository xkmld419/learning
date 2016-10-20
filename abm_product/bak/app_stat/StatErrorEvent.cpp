/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatErrorEvent.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"

extern TOCIDatabase gpDBLink;

StatError::StatError():TStatClass(OID_ERROR)
{
    pRead = NULL;
}

StatError::~StatError()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatError::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(EVENT_ID) MAXPOINT from %s where EVENT_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(EVENT_ID) MAXPOINT from %s where EVENT_ID<=%lu",sTableName,Point);
#endif
	
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	if (qry.next())
		Result=POINT_TYPE( qry.field("MAXPOINT").asFloat() );
	else 
		Result=0;
	qry.close();
	
	return Result;
		
}
							
POINT_TYPE StatError::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(EVENT_ID) MINPOINT from %s where EVENT_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(EVENT_ID) MINPOINT from %s where EVENT_ID>=%lu",sTableName,Point);
#endif
	
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	if (qry.next())
		Result=POINT_TYPE( qry.field("MINPOINT").asFloat() );
	else 
		Result=0;
	qry.close();
	
	return Result;
	
}
							
void StatError::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_ERROR_EVENT_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," EVENT_ID>=%llu AND EVENT_ID<=%llu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," EVENT_ID>=%lu AND EVENT_ID<=%lu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatError::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",__FILE__,__LINE__);
				throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_EVENT_ID();
    
    return true;
}

void StatError::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatError::GetCycle()
{
		char sTemp[15];
		int	 iResult;
		strncpy(sTemp,pRead->Get_STATE_DATE(),6);
		iResult=atoi(sTemp);
		return iResult;
}

int StatError::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatError::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case 	FA_ERROR_BILL_FLOW_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILL_FLOW_ID());
    		break;
    	case 	FA_ERROR_BILLING_ORG_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_ORG_ID());
    		break;
    	case 	FA_ERROR_START_DATE:
    		strncpy(sTemp,pRead->Get_START_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case 	FA_ERROR_SWITCH_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SWITCH_ID());
    		break;
    	case	FA_ERROR_FILE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_FILE_ID());
    		break;
    	case	FA_ERROR_CREATED_DATE:
    		strncpy(sTemp,pRead->Get_CREATED_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case 	FA_ERROR_EVENT_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_EVENT_STATE());
    		break;
    	case 	FA_ERROR_STATE_DATE:
    		strncpy(sTemp,pRead->Get_STATE_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case 	FA_ERROR_ERROR_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ERROR_ID());
    		break;
    	case	FA_DATA_PAGE:
    		if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(1);
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("PAGE:不能为string型，请修改统计配置!");
				break;
    	default	:
    		return -1;
    }
    return 0;
}


