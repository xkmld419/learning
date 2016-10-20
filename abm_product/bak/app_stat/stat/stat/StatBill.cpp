/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatBill.h"
#include "BillingCycleMgr.h"
#include "BssOrgMgr.h"
#define _FILE_NAME_ "StatBill.cpp"
#include "TBL_STAFF_CLASS.h"
#include "Date.h"
extern TOCIDatabase gpDBLink;
static char			sStatDate[9];

StatBill::StatBill():TStatClass(OID_BILL)
{
    pRead = NULL;
}

StatBill::~StatBill()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatBill::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(BILL_ID) MAXPOINT from %s where BILL_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(BILL_ID) MAXPOINT from %s where BILL_ID<=%lu",sTableName,Point);
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
							
POINT_TYPE StatBill::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(BILL_ID) MINPOINT from %s where BILL_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(BILL_ID) MINPOINT from %s where BILL_ID>=%lu",sTableName,Point);
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
							
void StatBill::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_BILL_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," BILL_ID>=%llu AND BILL_ID<=%llu ORDER BY BILL_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," BILL_ID>=%lu AND BILL_ID<=%lu ORDER BY BILL_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatBill::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",_FILE_NAME_,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_BILL_ID();
    
     
	/*    
    if (interface.getServ(serv,AcctBalance.Get_SERV_ID(),AcctBalance.Get_STATE_DATE()) ){
    	pServ=&serv;
    	pservinfo=serv.getServInfo();
    	pcustinfo=serv.getCustInfo();
	}
	else{
		pServ=NULL;
		pservinfo=NULL;
		pcustinfo=NULL;
	}
	*/
	return true;
}

void StatBill::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatBill::GetCycle()
{
	static  int	iResult=0;
	char	sTemp[9],sSql[256];
	
	if (iResult!=0)
		return iResult;
	
	sprintf(sSql,"SELECT TO_CHAR(SYSDATE,'YYYYMMDD') NOWDATE FROM DUAL");
	
	try{
		TOCIQuery qry(&gpDBLink);
		qry.close();
		qry.setSQL(sSql);
		qry.open();	
		qry.next();
		strcpy( sTemp,qry.field("NOWDATE").asString() );
		strcpy( sStatDate,sTemp);
		sTemp[6]='\0';
		iResult=atoi(sTemp);
		return iResult;
	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.getErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
	
}

int StatBill::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatBill::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	
	class BssOrgMgr orgmgr;
	MEM_STAFF_CLASS	mem_staff("COMM.STAFF");
	
	
	//基本域
    switch ( iFieldID){
    	case	FA_BILL_BSS_ORG_LEVELID100:
    		{
    			_TBL_STAFF_STR_	*pStaffStr=mem_staff[pRead->Get_STAFF_ID()];
    			int iOrgID=-1;
    			int iOrgLevelID=-1;
    			
    			if (pStaffStr!=NULL)
    				iOrgID=pStaffStr->BSS_ORG_ID;
    				
    			for (;iOrgID>=0;){
    				   					
    				iOrgLevelID=orgmgr.getLevel(iOrgID);
    				
    				if (iOrgLevelID==100)
    					break;
    				iOrgID=orgmgr.getParentID(iOrgID);
    			}
    			
    			if (iOrgLevelID==100)
    				pTFieldValue->AutoSetValue(iOrgID);
    			else 
    				pTFieldValue->SetDefaultValue();
    			       				            
    		}
    		break;
    	case	FA_BILL_BSS_ORG_LEVELID200:
    		{
    			_TBL_STAFF_STR_	*pStaffStr=mem_staff[pRead->Get_STAFF_ID()];
    			int iOrgID=-1;
    			int iOrgLevelID=-1;
    			
    			if (pStaffStr!=NULL)
    				iOrgID=pStaffStr->BSS_ORG_ID;
    				
    			for (;iOrgID>=0;){
    				   					
    				iOrgLevelID=orgmgr.getLevel(iOrgID);
    				
    				if (iOrgLevelID==200)
    					break;
    				iOrgID=orgmgr.getParentID(iOrgID);
    			}
    			
    			if (iOrgLevelID==200)
    				pTFieldValue->AutoSetValue(iOrgID);
    			else 
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	case	FA_BILL_BSS_ORG_LEVELID300:
    		{
    			_TBL_STAFF_STR_	*pStaffStr=mem_staff[pRead->Get_STAFF_ID()];
    			int iOrgID=-1;
    			int iOrgLevelID=-1;
    			
    			if (pStaffStr!=NULL)
    				iOrgID=pStaffStr->BSS_ORG_ID;
    				
    			for (;iOrgID>=0;){
    				   					
    				iOrgLevelID=orgmgr.getLevel(iOrgID);
    				
    				if (iOrgLevelID==300)
    					break;
    				iOrgID=orgmgr.getParentID(iOrgID);
    			}
    			
    			if (iOrgLevelID==300)
    				pTFieldValue->AutoSetValue(iOrgID);
    			else 
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	case	FA_BILL_BSS_ORG_LEVELID400:
    		{
    			_TBL_STAFF_STR_	*pStaffStr=mem_staff[pRead->Get_STAFF_ID()];
    			int iOrgID=-1;
    			int iOrgLevelID=-1;
    			
    			if (pStaffStr!=NULL)
    				iOrgID=pStaffStr->BSS_ORG_ID;
    				
    			for (;iOrgID>=0;){
    				   					
    				iOrgLevelID=orgmgr.getLevel(iOrgID);
    				
    				if (iOrgLevelID==400)
    					break;
    				iOrgID=orgmgr.getParentID(iOrgID);
    			}
    			
    			if (iOrgLevelID==400)
    				pTFieldValue->AutoSetValue(iOrgID);
    			else 
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	case	FA_BILL_BSS_ORG_LEVELID500:
    		{
    			_TBL_STAFF_STR_	*pStaffStr=mem_staff[pRead->Get_STAFF_ID()];
    			int iOrgID=-1;
    			int iOrgLevelID=-1;
    			
    			if (pStaffStr!=NULL)
    				iOrgID=pStaffStr->BSS_ORG_ID;
    				
    			for (;iOrgID>=0;){
    				   					
    				iOrgLevelID=orgmgr.getLevel(iOrgID);
    				
    				if (iOrgLevelID==500)
    					break;
    				iOrgID=orgmgr.getParentID(iOrgID);
    			}
    			
    			if (iOrgLevelID==500)
    				pTFieldValue->AutoSetValue(iOrgID);
    			else 
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	case	FA_BILL_STAFF_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_STAFF_ID());
    		break;
    	case	FA_BILL_PAYED_METHOD:
    		pTFieldValue->AutoSetValue(pRead->Get_PAYMENT_METHOD());
    		break;
    	case 	FA_BILL_BILLING_CYCLE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_CYCLE_ID());
    		break;
    	case	FA_BILL_STATE_DATE:
    		strncpy(sTemp,pRead->Get_STATE_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case	FA_BILL_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_STATE());
    		break;
    	case	FA_BILL_STAT_DATE:
    		pTFieldValue->AutoSetValue(sStatDate);
    		break;
    	case 	FA_BILL_BALANCE:
    		pTFieldValue->AutoSetValue(pRead->Get_BALANCE());
    		break;
    	case	FA_BILL_CNT:
    		pTFieldValue->AutoSetValue(1);
    		break; 		
    	default	:
    		return -1;
    }
    return 0;
}


