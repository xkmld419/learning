/*VER: 1*/ 
#include <string.h>

#define SQLCA_NONE
#define ORACA_NONE
#include "StatServ.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#define _FILE_NAME_ "StatServ.cpp"
#include "Date.h"
extern TOCIDatabase gpDBLink;

StatServ::StatServ():TStatClass(OID_SERV)
{
    pRead = NULL;
}

StatServ::~StatServ()
{
    if ( pRead)
       delete pRead;
	pRead = NULL;
	pservinfo = NULL;
	pcustinfo = NULL;
	pServ = NULL;
}

POINT_TYPE StatServ::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select max(SERV_ID) MAXPOINT from %s where SERV_ID<=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select max(SERV_ID) MAXPOINT from %s where SERV_ID<=%lu",sTableName,Point);
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
							
POINT_TYPE StatServ::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	TOCIQuery qry(&gpDBLink);
	char sSql[200];
	POINT_TYPE 	Result;
#ifdef BIT32OS
	sprintf(sSql,"select min(SERV_ID) MINPOINT from %s where SERV_ID>=%llu",sTableName,Point);
#else /*BIT64OS*/
	sprintf(sSql,"select min(SERV_ID) MINPOINT from %s where SERV_ID>=%lu",sTableName,Point);
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
							
void StatServ::OpenCursor(char *sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	char 	sSql[200];
	
	if (pRead == NULL)
        pRead = new TBL_SERV_CLASS();
    
    pRead->SetTableName(sTableName);

#ifdef BIT32OS
    sprintf(sSql," SERV_ID>=%llu AND SERV_ID<=%llu ORDER BY SERV_ID ",MinPoint,MaxPoint);
#else  /* BIT64OS */
	sprintf(sSql," SERV_ID>=%lu AND SERV_ID<=%lu ORDER BY SERV_ID ",MinPoint,MaxPoint);
#endif
    
    pRead->AddCondition(sSql);
    pRead->Open();
    
}

	
bool StatServ::GetRecordFromTable(POINT_TYPE &BreakPoint)		
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
  	
    BreakPoint=pRead->Get_SERV_ID();
    
    
    
    if (interface.getServ(serv,pRead->Get_SERV_ID(),date.toString()) ){
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

void StatServ::CloseCursor()
{		
	if (pRead){
		delete pRead;
        pRead=NULL;
    }			
}

int  StatServ::GetCycle()
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

int StatServ::GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue)
{
    return -1;
}

int  StatServ::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[200];
	int		iValue;
	static  Date	date;
	class OrgMgr orgmgr;
	//基本域
    switch ( iFieldID){
    	case	FA_S_ORG_LEVELID100:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(date.toString());
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
    	case	FA_S_ORG_LEVELID200:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(date.toString());
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
    	case	FA_S_ORG_LEVELID300:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(date.toString());
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
    	case	FA_S_ORG_LEVELID400:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(date.toString());
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
    	case	FA_S_ORG_LEVELID500:
    		{
    			if (pServ!=NULL){
    				int iOrgID=pServ->getOrgID(date.toString());
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
    	case	FA_S_SERV_CREATE_DATE:
    		strncpy(sTemp,pservinfo->m_sCreateDate,8);
    		sTemp[8]='\0';
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(sTemp);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_S_STATE_DATE:
    		strncpy(sTemp,pservinfo->m_sStateDate,8);
    		sTemp[8]='\0';
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(sTemp);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case	FA_S_PRODUCT_ID:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iProductID);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case	FA_S_BILLING_MODE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_iBillingMode);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_S_CUST_TYPE_ID:
    		if (pcustinfo)
    			pTFieldValue->AutoSetValue(pcustinfo->m_sCustType);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case 	FA_S_STATE:
    		if (pservinfo)
    			pTFieldValue->AutoSetValue(pservinfo->m_sState);
    		else
    			pTFieldValue->SetDefaultValue();
    		break;
    	case	FA_S_OFFER_ID:
    		if (pServ){
    			static int  offers[MAX_OFFERS];
    			static int  iCnt=0;
    			static int  i=0;
    			if (i==iCnt){
    				iCnt=pServ->getOffers(offers,date.toString());
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
    	case	FA_S_AGENT_ID:/*?待定?*/
    		
    		break;
    	case	FA_S_SERV_CNT:
    		if (pTFieldValue->ValType==VT_INT)
				pTFieldValue->SetValue(1);
			else if ( pTFieldValue->ValType==VT_STRING)
				throw TException("SERV_CNT:不能为string型，请修改统计配置!");
			break;
    	case	FA_S_SERV_ID:
    		pTFieldValue->AutoSetValue(pRead->Get_SERV_ID());
    		break;
    	case 	FA_S_ACCT_ID:
    		
    	default	:
    		return -1;
    }
    return 0;
}


