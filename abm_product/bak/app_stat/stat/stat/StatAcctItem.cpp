/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatAcctItem.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#define _FILE_NAME_ "StatAcctItem.cpp"
#include "Date.h"
#include "TBL_ACCT_ITEM_SOURCE_CLASS.h"
#include <vector>
extern TOCIDatabase gpDBLink;

StatAcctItem::StatAcctItem():TStatClass(OID_ACCT_ITEM)
{
    pRead = NULL;
	
}

StatAcctItem::~StatAcctItem()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatAcctItem::GetMaxPoint(char * sTableName,POINT_TYPE Point)
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
							
POINT_TYPE StatAcctItem::GetMinPoint(char * sTableName,POINT_TYPE Point)
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
							
void StatAcctItem::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_ACCT_ITEM_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," ACCT_ITEM_ID>=%llu AND ACCT_ITEM_ID<=%llu ORDER BY ACCT_ITEM_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," ACCT_ITEM_ID>=%lu AND ACCT_ITEM_ID<=%lu ORDER BY ACCT_ITEM_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatAcctItem::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",_FILE_NAME_,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_ACCT_ITEM_ID();
    
    
    
    if (interface.getServ(serv,pRead->Get_SERV_ID(),pRead->Get_CREATED_DATE()) ){
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

void StatAcctItem::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatAcctItem::GetCycle()
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

int StatAcctItem::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatAcctItem::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	MEM_ACCT_ITEM_SOURCE_CLASS		mem_acct_item_source("acct.acct_item_source");
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case	FA_A_ORG_LEVELID100:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_CREATED_DATE());
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
    	case	FA_A_ORG_LEVELID200:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_CREATED_DATE());
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
    	case	FA_A_ORG_LEVELID300:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_CREATED_DATE());
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
    	case	FA_A_ORG_LEVELID400:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_CREATED_DATE());
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
    	case	FA_A_ORG_LEVELID500:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(pRead->Get_CREATED_DATE());
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
    	case	FA_A_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_STATE());
    		break;
    	case 	FA_A_PARTNER_ID:
    		{
    			_TBL_ACCT_ITEM_SOURCE_STR_	* pAcctItemSource=mem_acct_item_source[pRead->Get_ITEM_SOURCE_ID()];
    			if (pAcctItemSource!=NULL)
    				pTFieldValue->AutoSetValue(pAcctItemSource->ITEM_SOURCE_TYPE);
    			else
    				pTFieldValue->SetDefaultValue();
    		}
    		break;
    	case 	FA_A_CUST_TYPE_ID:
    		if (pcustinfo)
    			pTFieldValue->AutoSetValue(pcustinfo->m_sCustType);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_A_ACCT_ITEM_TYPE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ACCT_ITEM_TYPE_ID());
    		break;
    	case 	FA_A_BILLING_CYCLE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_CYCLE_ID());
    		break;
    	case 	FA_A_PAYMENT_METHOD:
    		/*???*/
    		break;
    	case 	FA_A_STATE_DATE:
    		strncpy(sTemp,pRead->Get_STATE_DATE(),8);
    		sTemp[8]='\0';
    		pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case 	FA_A_SERV_STATE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_sState);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	
    	case	FA_A_BILLING_MODE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iBillingMode);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	
    	case	FA_A_OFFER_ID:
    		if (pServ){
    			static int  offers[MAX_OFFERS];
    			static int  iCnt=0;
    			static int  i=0;
    			if (i==iCnt){
    				iCnt=pServ->getOffers(offers,pRead->Get_CREATED_DATE());
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
    	
    	case 	FA_A_CHARGE:
    		pTFieldValue->AutoSetValue(pRead->Get_AMOUNT());
    		break;
    	
    	case	FA_A_SERV_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SERV_ID());
			break;
    	
    	case 	FA_A_ACCT_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ACCT_ID());
    		break;
    		
    	default	:
    		return -1;
    }
    return 0;
}


