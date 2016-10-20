/*VER: 1*/
#include "SpInfoSeekMgr.h"
#include "FormatMethod.h"
#include "Process.h"
#include "TableMgr.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif
#include <stdlib.h>
#include <unistd.h>


bool SpInfoSeekMgr::bInited = false;
SpInfoData * SpInfoSeekMgr::m_poSpInfoData = 0;
HashList<SpInfoData *> * SpInfoSeekMgr::m_pHSpInfoData = 0 ;
char SpInfoSeekMgr::m_sLastDate[16]={0};
Date SpInfoSeekMgr::m_oDate;

SpInfoSeekMgr::SpInfoSeekMgr()
{

  init();

}

SpInfoSeekMgr::~SpInfoSeekMgr()
{
#ifdef PRIVATE_MEMORY
  unload();
#endif
}

void SpInfoSeekMgr::unload()
{
    if(m_poSpInfoData) delete [] m_poSpInfoData;
    if(m_pHSpInfoData) delete m_pHSpInfoData;
    bInited = false;
    m_poSpInfoData =0;
    m_pHSpInfoData =0;
}

void SpInfoSeekMgr::init()
{
    
    strncpy(m_sLastDate, m_oDate.toString() , sizeof(m_sLastDate) );
#ifdef PRIVATE_MEMORY    
    load();
	//bInited = true;
#else
	bInited = true;
#endif    
    
}
    
void SpInfoSeekMgr::load()
{
    
    bInited = true;
    int iCount = 0;
    
    TOCIQuery qry(Environment::getDBConn());
	qry.setSQL( " select count(*) from ismp_prod_info ");
	qry.open();

	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
    
    m_pHSpInfoData = new HashList<SpInfoData *>((iCount>>1) + 16);
    m_poSpInfoData = new SpInfoData[iCount+1];
    
    if(!m_poSpInfoData ){
		 THROW(MBC_HashList+1);
	}
	memset(m_poSpInfoData, 0, sizeof(SpInfoData) * (iCount+1) );

    if(0 == iCount)
        return;
	
	qry.setSQL(
        " select a.prd_offer_id,a.prd_id,a.policy_id,"
        "  (select nvl(max(b.feetype),-1) from ismp_bill_mode b "
        "   where b.policy_id = a.policy_id) feetype"
        " from ismp_prod_info a ");
    qry.open();
    
    int i=0;
    char sTemp[100];
    int iTemp = 0;
	while(qry.next()){
	     
	     strncpy(m_poSpInfoData[i].m_sPrdOfferID,qry.field(0).asString(),23);
	     strncpy(m_poSpInfoData[i].m_sPrdID,qry.field(1).asString(),23); 
	     strncpy(m_poSpInfoData[i].m_sPolicyID,qry.field(2).asString(),23); 
         m_poSpInfoData[i].m_iFeeType = qry.field(3).asInteger();

         m_pHSpInfoData->add(m_poSpInfoData[i].m_sPrdOfferID, &m_poSpInfoData[i]);
	     i++;
	     if(i>=iCount)
	        break;
	}
	qry.commit();
	qry.close();
	
	
    
}

#ifdef PRIVATE_MEMORY
int SpInfoSeekMgr::getSPInfo(EventSection *  pEvent,
    Value * i_pInNBR, ID * o_pServCode, ID * o_pFeeType)
{
    if(!bInited) 
        init();

    m_oDate.getCurDate();
    ////strlen("YYYYMMDDhh") = 10
    if(strncmp(m_oDate.toString(),  m_sLastDate, 10  ) >0){
        unload();
        init();
    }
    
    int iRet = 0;
    SpInfoData * pSpInfoData;
    Value vServCode;
    Value vFeeType;
    
    
    char sTemp[100]={0};
    char *pTemp = i_pInNBR->m_sValue;
    
    if(NUMBER == i_pInNBR->m_iType){
        sprintf(sTemp, "%ld", i_pInNBR->m_lValue);
        pTemp = sTemp;
    }
    
    vServCode.m_iType = STRING;
    vServCode.m_lValue = 0;
    
    vFeeType.m_iType = NUMBER;
    vFeeType.m_lValue = 0;
    
    
	if( m_pHSpInfoData->get( pTemp, &pSpInfoData) ){
        strncpy( vServCode.m_sValue, pSpInfoData->m_sPrdID,
            sizeof(vServCode.m_sValue) );
        vServCode.m_lValue = atol(vServCode.m_sValue);
        if(vServCode.m_sValue[0])
            iRet = 1;
        
        if(pSpInfoData->m_iFeeType != -1){    
            vFeeType.m_lValue = pSpInfoData->m_iFeeType;
            sprintf(vFeeType.m_sValue, "%ld", vFeeType.m_lValue);
            
            iRet = 2;
        }
	}
	
	IDParser::setValue(pEvent, o_pServCode, &vServCode);
	IDParser::setValue(pEvent, o_pFeeType, &vFeeType);    
	    

    return iRet;
    
}
#else
int SpInfoSeekMgr::getSPInfo(EventSection *  pEvent,
    Value * i_pInNBR, ID * o_pServCode, ID * o_pFeeType)
{
    //if(!bInited) 
    //    init();

    m_oDate.getCurDate();
    ////strlen("YYYYMMDDhh") = 10
    if(strncmp(m_oDate.toString(),  m_sLastDate, 10  ) >0){
        //unload(); //这个应该是切换操作，这里暂时无法做
        //init();
    }
    
    int iRet = 0;
    SpInfoData * pSpInfoData = 0;
    Value vServCode;
    Value vFeeType;
    
    
    char sTemp[100]={0};
    char *pTemp = i_pInNBR->m_sValue;
    
    if(NUMBER == i_pInNBR->m_iType){
        sprintf(sTemp, "%ld", i_pInNBR->m_lValue);
        pTemp = sTemp;
    }
    
    vServCode.m_iType = STRING;
    vServCode.m_lValue = 0;
    
    vFeeType.m_iType = NUMBER;
    vFeeType.m_lValue = 0;
    
    
	unsigned int i = 0;
	if( G_PPARAMINFO->m_pHSpInfoIndex->get( pTemp, &i) ){
		pSpInfoData = &(G_PPARAMINFO->m_poSpInfoDataList[i]);
        strncpy( vServCode.m_sValue, pSpInfoData->m_sPrdID,
            sizeof(vServCode.m_sValue) );
        vServCode.m_lValue = atol(vServCode.m_sValue);
        if(vServCode.m_sValue[0])
            iRet = 1;
        
        if(pSpInfoData->m_iFeeType != -1){   
            vFeeType.m_lValue = pSpInfoData->m_iFeeType;
            sprintf(vFeeType.m_sValue, "%ld", vFeeType.m_lValue);
            
            iRet = 2;
        }
	}
	
	IDParser::setValue(pEvent, o_pServCode, &vServCode);
	IDParser::setValue(pEvent, o_pFeeType, &vFeeType);    
	    

    return iRet;
    
}
#endif
 

