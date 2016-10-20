/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatARefundLog.h"
#include "BillingCycleMgr.h"
#include "TBL_STAFF_CLASS.h"
#include "BssOrgMgr.h"
#define _FILE_NAME_ "StatARefundLog.cpp"
#include "Date.h"
extern TOCIDatabase gpDBLink;

StatARefundLog::StatARefundLog():TStatClass(OID_AREFUNDLOG)
{
    pRead = NULL;
}

StatARefundLog::~StatARefundLog()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatARefundLog::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(REFUND_LOG_ID) MAXPOINT from %s where REFUND_LOG_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(REFUND_LOG_ID) MAXPOINT from %s where REFUND_LOG_ID<=%lu",sTableName,Point);
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
							
POINT_TYPE StatARefundLog::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(REFUND_LOG_ID) MINPOINT from %s where REFUND_LOG_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(REFUND_LOG_ID) MINPOINT from %s where REFUND_LOG_ID>=%lu",sTableName,Point);
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
							
void StatARefundLog::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_A_REFUND_LOG_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," REFUND_LOG_ID>=%llu AND REFUND_LOG_ID<=%llu ORDER BY REFUND_LOG_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," REFUND_LOG_ID>=%lu AND REFUND_LOG_ID<=%lu ORDER BY REFUND_LOG_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatARefundLog::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	static Date date;
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",_FILE_NAME_,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_REFUND_LOG_ID();
    
    
    
    if (interface.getServ(serv,pRead->Get_REFUND_LOG_ID(),date.toString()) ){
    	pServ=&serv;
    	pservinfo=serv.getServInfo();
    	pcustinfo=serv.getCustInfo();
	}
	else{
		pServ=NULL;
		pservinfo=NULL;
		pcustinfo=NULL;
	}
	
	return true;
}

void StatARefundLog::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatARefundLog::GetCycle()
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
		iResult=atoi(sTemp);
		return iResult;
	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.getErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
	
}

int StatARefundLog::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatARefundLog::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	static  Date	date;
	char sSql[256];

	class BssOrgMgr orgmgr;
	MEM_STAFF_CLASS	mem_staff("STAFF");
	
	
	//基本域
    switch ( iFieldID){
    	case	FA_REFUNDLOG_BSS_ORG_LEVELID100:
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
    	case	FA_REFUNDLOG_BSS_ORG_LEVELID200:
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
    	case	FA_REFUNDLOG_BSS_ORG_LEVELID300:
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
    	case	FA_REFUNDLOG_BSS_ORG_LEVELID400:
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
    	case	FA_REFUNDLOG_BSS_ORG_LEVELID500:
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
    	case	FA_REFUNDLOG_STAFF_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_STAFF_ID());
    		break;
    	case	FA_REFUNDLOG_PAYED_METHOD:
    		{
    			TOCIQuery qry(&gpDBLink);
				sprintf(sSql,"select /*+rule+*/ a.payment_method PAYMENT_METHOD from bill a, balance_payout b where a.bill_id=b.bill_id and b.oper_payout_id=:OPER_PAYOUT_ID");
				qry.close();
					qry.setSQL(sSql);
					qry.setParameter("OPER_PAYOUT_ID",pRead->Get_OPER_PAYOUT_ID());
					qry.open();	
					long lValue;
					for(;qry.next();){
						lValue = qry.field("PAYMENT_METHOD").asInteger();
					}
					pTFieldValue->AutoSetValue( lValue );
    		}
    		break;
    	case	FA_REFUNDLOG_BALANCE_TYPE_ID:
    		{
    			TOCIQuery qry(&gpDBLink);
				sprintf(sSql,"select /*+rule+*/ a.balance_type_id BALANCE_TYPE_ID from acct_balance a, balance_payout b where a.acct_balance_id=b.acct_balance_id and b.oper_payout_id=:OPER_PAYOUT_ID");
				qry.close();
					qry.setSQL(sSql);
					qry.setParameter("OPER_PAYOUT_ID",pRead->Get_OPER_PAYOUT_ID());
					qry.open();	
					long lValue;
					for(;qry.next();){
						lValue = qry.field("BALANCE_TYPE_ID").asInteger();
					}
					pTFieldValue->AutoSetValue( lValue );
    		}
    		break;
    	
    	case	FA_REFUNDLOG_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_STATE());
    		break;
    	case	FA_REFUNDLOG_STAT_DATE:
    		{
    			char sTemp[7];
    			strncpy(sTemp,pRead->Get_CREATED_DATE(),6);
    			sTemp[6]='\0';
    			pTFieldValue->AutoSetValue(sTemp);
    		}
    		break;
    	case	FA_REFUNDLOG_BALANCE:
    		pTFieldValue->AutoSetValue(pRead->Get_CHARGE());
    		break;
    	case	FA_REFUNDLOG_BILL_CNT:
    		pTFieldValue->AutoSetValue(1L);
    		break;
    	default	:
    		return -1;
    }
    return 0;
}


