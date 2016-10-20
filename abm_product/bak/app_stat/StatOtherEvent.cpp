/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatOtherEvent.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#define _FILE_NAME_ "StatOtherEvent.cpp"

extern TOCIDatabase gpDBLink;

StatOtherEvent::StatOtherEvent():TStatClass(OID_OTHER_EVENT)
{
    pRead = NULL;
}

StatOtherEvent::~StatOtherEvent()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatOtherEvent::GetMaxPoint(char * sTableName,POINT_TYPE Point)
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
							
POINT_TYPE StatOtherEvent::GetMinPoint(char * sTableName,POINT_TYPE Point)
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
							
void StatOtherEvent::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_OTHER_EVENT_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," EVENT_ID>=%llu AND EVENT_ID<=%llu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," EVENT_ID>=%lu AND EVENT_ID<=%lu ORDER BY EVENT_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatOtherEvent::GetRecordFromTable(POINT_TYPE &BreakPoint)		
{
	char sTemp[200];
	
	if ( !pRead){
        sprintf(sTemp,"[%s:%d] GetRecordFromTable 游标还没打开!",_FILE_NAME_,__LINE__);
		throw TException(sTemp);
    }
    
    if ( ! pRead->Next()){
        return false;
    }
  	
    BreakPoint=pRead->Get_EVENT_ID();

/*
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
		}

	}
*/
    return true;
}

void StatOtherEvent::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatOtherEvent::GetCycle()
{
	char sTemp[7];
	static int iResult;
	Date sCurDate;
	strncpy(sTemp,sCurDate.toString("YYYYMMDD"),6);
	sTemp[6]='\0';
	iResult=atoi(sTemp);
	return iResult;
/*
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
		sprintf(sTemp,"[%s:%d] billing_id=[%d]找不到!",_FILE_NAME_,__LINE__,(int )pRead->Get_BILLING_CYCLE_ID());
		throw TException(sTemp);
	}
	strncpy(sTemp,pBillingCycle->getEndDate(),6);
	sTemp[6]='\0';
	iResult=atoi(sTemp);
	return iResult;
*/
}

int StatOtherEvent::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatOtherEvent::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200],sEndTemp[200];
	int		iValue;
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case 	FA_OTHER_BILLFLOW_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILL_FLOW_ID() );
    	case 	FA_OTHER_BILLING_ORG_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_BILLING_ORG_ID() );
    		break;
    	case	FA_OTHER_STAT_DAY:
    		strncpy(sTemp,pRead->Get_START_DATE(),8);
				sTemp[8]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_OTHER_STAT_HOUR:
    		strncpy(sTemp,pRead->Get_START_DATE()+8,2);
				sTemp[2]='\0';
				pTFieldValue->AutoSetValue(sTemp);
    		break;
    	case	FA_OTHER_SWITCH_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SWITCH_ID() );
    		break;
    	case	FA_OTHER_TRUNK_IN:
    		pTFieldValue->AutoSetValue(pRead->Get_TRUNK_IN() );
    		break;
    	case	FA_OTHER_TRUNK_OUT:
    		pTFieldValue->AutoSetValue(pRead->Get_TRUNK_OUT() );
    		break;
    	case	FA_OTHER_FILE_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_FILE_ID() );
    		break;
    	case	FA_OTHER_EVENT_STATE:
    		pTFieldValue->AutoSetValue(pRead->Get_EVENT_STATE() );
    		break;
    	case	FA_OTHER_ERROR_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_ERROR_ID() );
    		break;
    	case	FA_OTHER_CYCLE_BEGIN_DAY:
    		strncpy(sTemp,pRead->Get_CYCLE_BEGIN_DATE(),8);
				sTemp[8]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_OTHER_CYCLE_END_DAY:
    		strncpy(sTemp,pRead->Get_CYCLE_END_DATE(),8);
				sTemp[8]='\0';
				pTFieldValue->AutoSetValue(sTemp);    		
    		break;
    	case	FA_OTHER_PAGE:
    		if (pTFieldValue->ValType==VT_INT)
				pTFieldValue->SetValue(1);
			else if ( pTFieldValue->ValType==VT_STRING)
				throw TException("PAGE:不能为string型，请修改统计配置!");
			break;
    	case	FA_OTHER_DURATION:
    		if (pTFieldValue->ValType==VT_INT)
				pTFieldValue->SetValue(pRead->Get_DURATION());
			else if ( pTFieldValue->ValType==VT_STRING)
				throw TException("DURATION:不能为string型，请修改统计配置!");
			break;
    	case 	FA_OTHER_BILL_DURATION:
    		if (pTFieldValue->ValType==VT_INT)
				pTFieldValue->SetValue( ( pRead->Get_DURATION() + 59)/60*60);
			else if ( pTFieldValue->ValType==VT_STRING)
				throw TException("BILL_DURATION:不能为string型，请修改统计配置!");
			break;
			case	FA_OTHER_SEND_BYTES:
				if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(  pRead->Get_SEND_BYTES());
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("SEND_BYTES:不能为string型，请修改统计配置!");
				break;
			case	FA_OTHER_RECV_BYTES:
				if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(  pRead->Get_RECV_BYTES());
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("RECV_BYTES:不能为string型，请修改统计配置!");
				break;
    	case	FA_OTHER_ORG_CHARGE1:
    		if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(pRead->Get_ORG_CHARGE1());
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("ORG_CHARGE1:不能为string型，请修改统计配置!");
				break;
    	case	FA_OTHER_CYCLE_DAY:
    		strncpy(sEndTemp,pRead->Get_CYCLE_END_DATE(),8);
    		sEndTemp[8]=0;
    		strncpy(sTemp,pRead->Get_CYCLE_BEGIN_DATE(),8);
    		sTemp[8]=0;
    		iValue=atoi(sEndTemp)-atoi(sTemp);
    		if (pTFieldValue->ValType==VT_INT)
					pTFieldValue->SetValue(iValue);
				else if ( pTFieldValue->ValType==VT_STRING)
					throw TException("CYCLE_DAY:不能为string型，请修改统计配置!");
				break;
    	default	:
    		return -1;
    }
    return 0;
}


