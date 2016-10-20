/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatRedoEvent.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"

extern TOCIDatabase gpDBLink;

StatRedoEvent::StatRedoEvent():TStatClass(OID_REDO_EVENT)
{
    pRead = NULL;
}

StatRedoEvent::~StatRedoEvent()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatRedoEvent::GetMaxPoint(char * sTableName,POINT_TYPE Point)
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
							
POINT_TYPE StatRedoEvent::GetMinPoint(char * sTableName,POINT_TYPE Point)
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
							
void StatRedoEvent::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_REDO_EVENT_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," EVENT_ID>=%llu AND EVENT_ID<=%llu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," EVENT_ID>=%lu AND EVENT_ID<=%lu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatRedoEvent::GetRecordFromTable(POINT_TYPE &BreakPoint)		
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
    
    pRead->Get_SERV_ID();
    
    if (interface.getServ(serv,pRead->Get_SERV_ID(),pRead->Get_START_DATE()) ){
    	pServ=&serv;
    	pservinfo=serv.getServInfo();
    	pcustinfo=serv.getCustInfo();
	}
	else{
		pServ=NULL;
		sprintf(sTemp," SERV_ID=%ld ",pRead->Get_SERV_ID() );
		TblServClass.AddCondition(sTemp);
		TblServClass.Open();
		if (!TblServClass.Next()){
			pservinfo=NULL;
			pcustinfo=NULL;
		}
		else{
			servinfo.m_lServID=TblServClass.Get_SERV_ID();
			servinfo.m_lCustID=TblServClass.Get_CUST_ID();
			servinfo.m_iProductID=TblServClass.Get_PRODUCT_ID();
			strcpy(servinfo.m_sCreateDate,TblServClass.Get_CREATE_DATE() );
			strcpy(servinfo.m_sRentDate,TblServClass.Get_RENT_DATE());
			strcpy(servinfo.m_sNscDate,TblServClass.Get_NSC_DATE());
			strcpy(servinfo.m_sState,TblServClass.Get_STATE());
			strcpy(servinfo.m_sStateDate,TblServClass.Get_STATE_DATE());
			servinfo.m_iCycleType=TblServClass.Get_CYCLE_TYPE_ID();
			strcpy(servinfo.m_sAccNbr,TblServClass.Get_ACC_NBR());
			servinfo.m_iBillingMode=TblServClass.Get_BILLING_MODE_ID();
			servinfo.m_iFreeType=TblServClass.Get_FREE_TYPE_ID();
			pservinfo=&servinfo;
		
			pcustinfo=NULL;
			/*
			sprintf(sTemp," CUST_ID=%ld ",servinfo.m_lCustID );
			TblCustClass.AddCondition(sTemp);
			TblCustClass.Open();
			if (!TblServClass.Next())
				pservinfo=NULL;
			else{
				custinfo.m_lCustID=TblCustClass.Get_SERV_ID();
				custinfo.m_lCustID=TblServClass.Get_CUST_ID();
				servinfo.m_iProductID=TblServClass.Get_PRODUCT_ID();
				strcpy(servinfo.m_sCreateDate,TblServClass.Get_CREATE_DATE() );
				strcpy(servinfo.m_sRentDate,TblServClass.Get_RENT_DATE());
				strcpy(servinfo.m_sNscDate,TblServClass.Get_NSC_DATE());
				strcpy(servinfo.m_sState,TblServClass.Get_STATE());
				strcpy(servinfo.m_sStateDate,TblServClass.Get_STATE_DATE());
				servinfo.m_iCycleType=TblServClass.Get_CYCLE_TYPE_ID();
				strcpy(servinfo.m_sAccNbr,TblServClass.Get_ACC_NBR());
				servinfo.m_iBillingMode=TblServClass.Get_BILLING_MODE_ID();
				servinfo.m_iFreeType=TblServClass.Get_FREE_TYPE_ID();
				pservinfo=&servinfo;
			}
			*/
		}
	}
	
	
    return true;
}

void StatRedoEvent::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatRedoEvent::GetCycle()
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

int StatRedoEvent::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatRedoEvent::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case	FA_REDO_BILL_FLOW_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILL_FLOW_ID());
    		break;
    	case 	FA_REDO_BILLING_CYCLE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_CYCLE_ID());
    		break;
    	case 	FA_REDO_CUST_TYPE_ID:
    		if (pcustinfo)
    			pTFieldValue->AutoSetValue(pcustinfo->m_sCustType);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_REDO_EVENT_TYPE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_EVENT_TYPE_ID() );
    		break;
    	case 	FA_REDO_BILLING_AREA_CODE:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_AREA_CODE() );
    		break;
    	case 	FA_REDO_BILLING_VISIT_AREA_CODE:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_VISIT_AREA_CODE() );
    		break;
    	case 	FA_REDO_CALLING_AREA_CODE:
    		pTFieldValue->AutoSetValue(pRead->Get_CALLING_AREA_CODE() );
    		break;
    	case	FA_REDO_CALLED_AREA_CODE:
    		pTFieldValue->AutoSetValue(pRead->Get_CALLED_AREA_CODE() );
    		break;
    	case 	FA_REDO_ACCOUNT_AREA_CODE:
    		pTFieldValue->AutoSetValue(pRead->Get_ACCOUNT_AREA_CODE() );
    		break;
    	case	FA_REDO_SERV_CREATE_DATE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_sCreateDate);
			else
				pTFieldValue->SetDefaultValue(); 
    		break;
    	
    	case	FA_REDO_SERV_STATE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_sState);
				else
					pTFieldValue->SetDefaultValue(); 
    		break;
    	case	FA_REDO_FREE_TYPE_ID:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iFreeType);
				else
					pTFieldValue->SetDefaultValue(); 
    		break;
    	case	FA_REDO_BILLING_MODE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iBillingMode);
				else
					pTFieldValue->SetDefaultValue(); 
    		break;
    	case	FA_REDO_START_HOUR:
				strncpy(sTemp,pRead->Get_START_DATE()+8,2);
				sTemp[2]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_REDO_START_DAY:
				strncpy(sTemp,pRead->Get_START_DATE(),8);
				sTemp[8]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_REDO_STAT_HOUR:
				strncpy(sTemp,pRead->Get_STATE_DATE()+8,2);
				sTemp[2]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_REDO_STAT_DAY:
				strncpy(sTemp,pRead->Get_STATE_DATE(),8);
				sTemp[8]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    		
    	case	FA_REDO_SWITCH_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SWITCH_ID() );
    		break;
    	
    	case	FA_REDO_ORG_LEVELID100:
    		{
    			int iOrgID=pRead->Get_BILLING_ORG_ID();
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
    		break;
    	case	FA_REDO_ORG_LEVELID200:
    		{
    			int iOrgID=pRead->Get_BILLING_ORG_ID();
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
    		break;
    	case	FA_REDO_ORG_LEVELID300:
    		{
    			int iOrgID=pRead->Get_BILLING_ORG_ID();
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
    		break;
    	case	FA_REDO_ORG_LEVELID400:
    		{
    			int iOrgID=pRead->Get_BILLING_ORG_ID();
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
    		break;
    	case	FA_REDO_ORG_LEVELID500:
    		{
    			int iOrgID=pRead->Get_BILLING_ORG_ID();
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
    		break;
    	    	
    	case 	FA_REDO_TRUNK_IN:
    		pTFieldValue->AutoSetValue(pRead->Get_TRUNK_IN());
    		break;
    	case 	FA_REDO_TRUNK_OUT:
    		pTFieldValue->AutoSetValue(pRead->Get_TRUNK_OUT());
    		break;
    		
    	case 	FA_REDO_PRODUCT_ID:
			if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iProductID);
			else
				pTFieldValue->SetDefaultValue();    		
    		break;
    	case 	FA_REDO_ERROR_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ERROR_ID() );
    		break;
    	
    	case	FA_REDO_PAGE:
    		if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(1);
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("PAGE:不能为string型，请修改统计配置!");
				break;
			
    	case	FA_REDO_DURATION:
    		if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(pRead->Get_DURATION());
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("DURATION:不能为string型，请修改统计配置!");
				break;
			
    	
    	default	:
    		return -1;
    }
    return 0;
}


