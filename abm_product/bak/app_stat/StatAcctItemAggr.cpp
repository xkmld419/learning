/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatAcctItemAggr.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#include "Date.h"

extern TOCIDatabase gpDBLink;
char	sCurDate[9];
StatAcctItemAggr::StatAcctItemAggr():TStatClass(OID_ACCT_ITEM_AGGR)
{
    pRead = NULL;
	
}

StatAcctItemAggr::~StatAcctItemAggr()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatAcctItemAggr::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(ACCT_ITEM_ID) MAXPOINT from %s where ACCT_ITEM_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(ACCT_ITEM_ID) MAXPOINT from %s where ACCT_ITEM_ID<=%lu",sTableName,Point);
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
							
POINT_TYPE StatAcctItemAggr::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(ACCT_ITEM_ID) MINPOINT from %s where ACCT_ITEM_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(ACCT_ITEM_ID) MINPOINT from %s where ACCT_ITEM_ID>=%lu",sTableName,Point);
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
							
void StatAcctItemAggr::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_ACCT_ITEM_AGGR_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," ACCT_ITEM_ID>=%llu AND ACCT_ITEM_ID<=%llu ORDER BY ACCT_ITEM_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," ACCT_ITEM_ID>=%lu AND ACCT_ITEM_ID<=%lu ORDER BY ACCT_ITEM_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatAcctItemAggr::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	int	 iDate;
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",__FILE__,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_ACCT_ITEM_ID();
    
    iDate=GetCycle();
    
    sprintf(sTemp,"%d",iDate);
    strcat(sTemp,"20000000");
    if (interface.getServ(serv,pRead->Get_SERV_ID(),sTemp) ){
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

void StatAcctItemAggr::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}


int  StatAcctItemAggr::GetCycle()
{
	static 	int				iPreBillingCycleID=0;
	static  int				iResult;

	BillingCycleMgr		billing_cycle_mgr;
	BillingCycle	*	pBillingCycle;
	
	char			sTemp[7];
	
	if ( iPreBillingCycleID==pRead->Get_BILLING_CYCLE_ID() )
		return iResult;
	else 
		iPreBillingCycleID=	pRead->Get_BILLING_CYCLE_ID();
		
	pBillingCycle=billing_cycle_mgr.getBillingCycle(pRead->Get_BILLING_CYCLE_ID());
	
	if ( !pBillingCycle ){
		char sTemp[200];
		sprintf(sTemp,"[%s:%d] billing_id=[%d]找不到!",__FILE__,__LINE__,(int )pRead->Get_BILLING_CYCLE_ID());
		throw TException(sTemp);
	}
	strncpy(sTemp,pBillingCycle->getEndDate(),6);
	sTemp[6]='\0';
	iResult=atoi(sTemp);
	return iResult;
	
}
/*
int  StatAcctItemAggr::GetCycle()
{
	static  int	iResult=0;
	char	sTemp[9],sSql[256];
	
	if (iResult!=0)
		return iResult;
	
	sprintf(sSql,"SELECT TO_CHAR(SYSDATE,'YYYYMM') NOWDATE FROM DUAL");
	
	try{
		TOCIQuery qry(&gpDBLink);
		qry.close();
		qry.setSQL(sSql);
		qry.open();	
		qry.next();
		strcpy( sTemp,qry.field("NOWDATE").asString() );
		strcpy( sCurDate,sTemp);
		iResult=atoi(sTemp);
		return iResult;
	}
	catch(TOCIException &oe){
		cout<<"[ERROR]:"<<oe.getErrMsg()<<endl;
		cout<<"[SQL]:"<<oe.getErrSrc()<<endl;
        throw oe;
    }
	
}
*/
int StatAcctItemAggr::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatAcctItemAggr::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case	FA_AGGR_ORG_LEVELID100:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID();
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
    	case	FA_AGGR_ORG_LEVELID200:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID();
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
    	case	FA_AGGR_ORG_LEVELID300:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID();
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
    	case	FA_AGGR_ORG_LEVELID400:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID();
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
    	case	FA_AGGR_ORG_LEVELID500:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID();
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
    	/*
    	case	FA_AGGR_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_STATE());
    		break;
    	
    	case 	FA_AGGR_PARTNER_ID:
    		{
    			_TBL_ACCT_ITEM_SOURCE_STR_	* pAcctItemSource=mem_acct_item_source[pRead->Get_ITEM_SOURCE_ID()];
    			if (pAcctItemSource!=NULL)
    				pTFieldValue->AutoSetValue(pAcctItemSource->ITEM_SOURCE_TYPE);
    			else
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	*/
    	case 	FA_AGGR_CUST_TYPE_ID:
    		if (pcustinfo)
    			pTFieldValue->AutoSetValue(pcustinfo->m_sCustType);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_AGGR_ACCT_ITEM_TYPE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ACCT_ITEM_TYPE_ID());
    		break;
    	case 	FA_AGGR_BILLING_CYCLE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_CYCLE_ID());
    		break;
    	/*
    	case 	FA_AGGR_PAYMENT_METHOD:
    		
    		break;
    	*/
    	case 	FA_AGGR_STAT_DATE:
    		strncpy(sTemp,sCurDate,8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	
    	case 	FA_AGGR_SERV_STATE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_sState);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	
    	case	FA_AGGR_BILLING_MODE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iBillingMode);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	
    	case	FA_AGGR_OFFER_ID:
    		if (pServ){
    			static int  offers[MAX_OFFERS];
    			static int  iCnt=0;
    			static int  i=0;
    			if (i==iCnt){
    				iCnt=pServ->getOffers(offers);
    				i=0;
    				if (iCnt==0){
    					pTFieldValue->SetDefaultValue();
							break;
						}
    			}
    			pTFieldValue->AutoSetValue(offers[i++]);
    			if(i!=iCnt)
    				return 1;
    		}	
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	
    	case 	FA_AGGR_CHARGE:
    		pTFieldValue->AutoSetValue(pRead->Get_CHARGE());
    		break;
    	case	FA_AGGR_PAGE:
    		if (pTFieldValue->ValType==VT_INT)
				pTFieldValue->SetValue(1);
			else if ( pTFieldValue->ValType==VT_STRING)
				throw TException("PAGE:不能为string型，请修改统计配置!");
			break;
			case	FA_AGGR_FREE_TYPE_ID:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iFreeType);
			else
				pTFieldValue->SetDefaultValue(); 
    		break;
    	case	FA_AGGR_SERV_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SERV_ID());
			break;
    	
    	case 	FA_AGGR_ACCT_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ACCT_ID());
    		break;
    	case 	FA_AGGR_PRODUCT_ID:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iProductID);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	default	:
    		return -1;
    }
    return 0;
}


