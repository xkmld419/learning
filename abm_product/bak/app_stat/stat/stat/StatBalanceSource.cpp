/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatBalanceSource.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#define _FILE_NAME_ "StatBalanceSource.cpp"
#include "Date.h"
extern TOCIDatabase gpDBLink;

#include "TBL_ACCT_BALANCE_CLASS.h"

static TBL_ACCT_BALANCE_CLASS	AcctBalance;
static int						iAcctBalanceFound;
static char						sStatDate[9];

StatBalanceSource::StatBalanceSource():TStatClass(OID_BALANCE_SOURCE)
{
    pRead = NULL;
}

StatBalanceSource::~StatBalanceSource()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatBalanceSource::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(OPER_INCOME_ID) MAXPOINT from %s where OPER_INCOME_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(OPER_INCOME_ID) MAXPOINT from %s where OPER_INCOME_ID<=%lu",sTableName,Point);
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
							
POINT_TYPE StatBalanceSource::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(OPER_INCOME_ID) MINPOINT from %s where OPER_INCOME_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(OPER_INCOME_ID) MINPOINT from %s where OPER_INCOME_ID>=%lu",sTableName,Point);
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
							
void StatBalanceSource::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_BALANCE_SOURCE_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," OPER_INCOME_ID>=%llu AND OPER_INCOME_ID<=%llu ORDER BY OPER_INCOME_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," OPER_INCOME_ID>=%lu AND OPER_INCOME_ID<=%lu ORDER BY OPER_INCOME_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatBalanceSource::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",_FILE_NAME_,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_OPER_INCOME_ID();
    
    sprintf(sTemp,"ACCT_BALANCE_ID=%ld",pRead->Get_ACCT_BALANCE_ID() );
    AcctBalance.AddCondition(sTemp);
    AcctBalance.SetTableName("ACCT.ACCT_BALANCE");
    AcctBalance.Open();
    
    if ( !AcctBalance.Next() ){
		iAcctBalanceFound=0;
		pServ=NULL;
		pservinfo=NULL;
		pcustinfo=NULL;
		return true;    
	}
    
    iAcctBalanceFound=1;
    
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
	
	return true;
}

void StatBalanceSource::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatBalanceSource::GetCycle()
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

int StatBalanceSource::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatBalanceSource::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case	FA_BS_ORG_LEVELID100:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_STATE_DATE());
    				int iOrgLevelID=-1;
    				
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
    			else{
    				pTFieldValue->SetDefaultValue();
    			}       				            
    		}
    		break;
    	case	FA_BS_ORG_LEVELID200:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_STATE_DATE());
    				int iOrgLevelID=-1;
    				
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
    			else{
    				pTFieldValue->SetDefaultValue();
    			}
    		}
    		break;
    	case	FA_BS_ORG_LEVELID300:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_STATE_DATE());
    				int iOrgLevelID=-1;
    				
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
    			else{
    				pTFieldValue->SetDefaultValue();
    			}
    		}
    		break;
    	case	FA_BS_ORG_LEVELID400:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_STATE_DATE());
    				int iOrgLevelID=-1;
    				
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
    			else{
    				pTFieldValue->SetDefaultValue();
    			}
    		}
    		break;
    	case	FA_BS_ORG_LEVELID500:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_STATE_DATE());
    				int iOrgLevelID=-1;
    				
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
    			else{
    				pTFieldValue->SetDefaultValue();
    			}
    		}
    		break;
    	case 	FA_BS_BALANCE_TYPE_ID:
    		if (iAcctBalanceFound)
    			pTFieldValue->AutoSetValue(AcctBalance.Get_BALANCE_TYPE_ID());
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case	FA_BS_BALANCE_LEVEL:
    		/* acct_id不能为空
				1。如果serv_id ,item_group_id 都为空，是帐户级
				2。如果serv_id 不为空 ,item_group_id 为空 是用户级
				3。如果serv_id ,item_group_id 都不为空 是帐目级 
			*/
			if (iAcctBalanceFound)
			{
    			long lServID=AcctBalance.Get_SERV_ID();
    			long lAcctItemTypeID= AcctBalance.Get_ITEM_GROUP_ID();
    			if (lServID==-1 && lAcctItemTypeID==-1)
    				pTFieldValue->AutoSetValue(1);
    			else if (lServID!=-1 && lAcctItemTypeID==-1)
    				pTFieldValue->AutoSetValue(2);
    			else if (lServID!=-1 && lAcctItemTypeID!=-1)
    				pTFieldValue->AutoSetValue(3);
    			else 
    				pTFieldValue->SetDefaultValue();
    		}
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_BS_STATE_DATE:
    		strncpy(sTemp,pRead->Get_STATE_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case	FA_BS_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_STATE());
    		break;
    	case	FA_BS_STAT_DATE:
    		pTFieldValue->AutoSetValue(sStatDate);
    		break;
    	case 	FA_BS_BALANCE:
    		pTFieldValue->AutoSetValue(pRead->Get_BALANCE());
    		break;
    	  		
    	default	:
    		return -1;
    }
    return 0;
}


