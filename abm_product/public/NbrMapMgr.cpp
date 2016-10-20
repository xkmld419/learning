/*VER: 3*/ 
#include "NbrMapMgr.h"
#include "FormatMethod.h"
#include <stdlib.h>
#include <unistd.h>
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

bool NbrMapMgr::bInited = false;
NbrMapData * NbrMapMgr::m_poNbrMapData = 0;
HashList<NbrMapData *> * NbrMapMgr::m_pHNbrData[4] = {0} ;

#ifdef PRIVATE_MEMORY
NbrMapMgr::NbrMapMgr()
{
  init();

}
#else
NbrMapMgr::NbrMapMgr()
{
}
#endif

void NbrMapMgr::init()
{
    bInited = true;
    int iCount = 0;
    
    TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from b_nbr_map");
	qry.open();

	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
    
    m_pHNbrData[0] = new HashList<NbrMapData *>(200);
    m_pHNbrData[1] = new HashList<NbrMapData *>(200);
    m_pHNbrData[2] = new HashList<NbrMapData *>(200);
    m_pHNbrData[3] = new HashList<NbrMapData *>(200);
    
    m_poNbrMapData = new NbrMapData[iCount+1];
    
    if(!m_poNbrMapData 
      || !m_pHNbrData[0]|| !m_pHNbrData[1]|| !m_pHNbrData[2]|| !m_pHNbrData[3]){
		 THROW(MBC_HashList+1);
	}
	memset(m_poNbrMapData, 0, sizeof(NbrMapData)*iCount+1 );
	
    qry.setSQL(" select ACC_NBR,MAP_TYPE_ID,NEED_SAME_AREA,"
        " REF_VALUE,OUT_AREA_CODE,OUT_ACC_NBR,"
        " to_char(EFF_DATE,'yyyymmddhh24miss') EFF,"
        " to_char(EXP_DATE,'yyyymmddhh24miss') EXP from b_nbr_map");
    qry.open();
    
    
    int i=0;
    char sTemp[100];
    int iTemp = 0;
	while(qry.next()){
	     
	     strncpy(m_poNbrMapData[i].m_sAccNbr,qry.field(0).asString(),32);
	     m_poNbrMapData[i].m_iMapType = qry.field(1).asInteger();
	      
	     strncpy(m_poNbrMapData[i].m_sNeedSameArea,qry.field(2).asString(),16); 
	     strncpy(m_poNbrMapData[i].m_sRefValue,qry.field(3).asString(),32);
	     strncpy(m_poNbrMapData[i].m_sOutAreaCode,qry.field(4).asString(),16);
	     strncpy(m_poNbrMapData[i].m_sOutAccNbr,qry.field(5).asString(),32);
	     
	     strncpy(m_poNbrMapData[i].m_sEffDate,qry.field(6).asString(),16);
	     strncpy(m_poNbrMapData[i].m_sExpDate,qry.field(7).asString(),16);
	     
	     sprintf(sTemp,"%s=%s",m_poNbrMapData[i].m_sAccNbr,m_poNbrMapData[i].m_sRefValue);
	     iTemp = (m_poNbrMapData[i].m_iMapType/2);
	     if(iTemp>=4)
	         iTemp = 3;
	     m_pHNbrData[iTemp]->get(sTemp, &m_poNbrMapData[i].pNext );
	     m_pHNbrData[iTemp]->add(sTemp, &m_poNbrMapData[i] );
         
	     i++;
	     if(i>=iCount)
	        break;
	}
	
    
}


bool NbrMapMgr::mapNbr(EventSection *  pEventSection, int iType, char * sRef)
{
    if(!bInited) 
        init();
//  1，主叫映射，到主叫
//  2，被叫映射，到被叫
//  4，被叫映射到计费，
//  8，主叫映射然后专网映射，
    
    char sTemp[100];
    NbrMapData * pNbrMapData;
    
    if(iType & 0x01){
        if(pNbrMapData = getNbrData(0, pEventSection->m_sCallingNBR, sRef,
            pEventSection ) ){
            strcpy(pEventSection->m_sBillingNBR, pNbrMapData->m_sOutAccNbr);
            if(pNbrMapData->m_sOutAreaCode[0])
                strcpy(pEventSection->m_sBillingAreaCode, pNbrMapData->m_sOutAreaCode);
        }
    }

    if(iType & 0x02){
        if(pNbrMapData = getNbrData(1, pEventSection->m_sCalledNBR, sRef,
            pEventSection ) ){
            strcpy(pEventSection->m_sCalledNBR, pNbrMapData->m_sOutAccNbr);
            if(pNbrMapData->m_sOutAreaCode[0])
                strcpy(pEventSection->m_sCalledAreaCode, pNbrMapData->m_sOutAreaCode);

        }
    }    


    if(iType & 0x04){
        if(pNbrMapData = getNbrData(2, pEventSection->m_sCalledNBR, sRef,
            pEventSection ) ){
            strcpy(pEventSection->m_sBillingNBR, pNbrMapData->m_sOutAccNbr);
            if(pNbrMapData->m_sOutAreaCode[0])
                strcpy(pEventSection->m_sBillingAreaCode, pNbrMapData->m_sOutAreaCode);

        }
    }


    if(iType & 0x08){
        if(pNbrMapData = getNbrData(3, pEventSection->m_sCallingNBR, sRef,
			pEventSection ) ){
            strcpy(pEventSection->m_sBillingNBR, pNbrMapData->m_sOutAccNbr);
        if(pNbrMapData->m_sOutAreaCode[0])
            strcpy(pEventSection->m_sBillingAreaCode, pNbrMapData->m_sOutAreaCode);
        FormatMethod::formatPNSegAccNBR( pEventSection );
		}
    }

    return true;
    
}
 
#ifdef PRIVATE_MEMORY
NbrMapData * NbrMapMgr::getNbrData(int iType,char * sValue,char * sRef,EventSection * pEvent)
{
    NbrMapData * pNbrMapData;
    char sTemp[100];
	bool bNoRef = false;
	if(sRef && strcmp(sRef,"#")==0){
		bNoRef = true;
	}
	if(!bNoRef){
		sprintf(sTemp, "%s=%s", sValue, sRef);
		if( m_pHNbrData[iType]->get(sTemp, &pNbrMapData) ){
			while(pNbrMapData){
				if( strcmp(pEvent->m_sStartDate, pNbrMapData->m_sEffDate)>=0 
				  &&strcmp(pEvent->m_sStartDate, pNbrMapData->m_sExpDate)<0 ){
					if(pNbrMapData->m_sNeedSameArea[0]){
						if(pNbrMapData->m_sNeedSameArea[0]=='*'){
							if(strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0)
								return pNbrMapData;
						}else{
							if(iType == 2 || iType == 1){
								if(strcmp(pEvent->m_sCalledAreaCode, pNbrMapData->m_sNeedSameArea)==0)
									return pNbrMapData;
							}else{
								if(strcmp(pEvent->m_sCallingAreaCode,pNbrMapData->m_sNeedSameArea)==0)
									return pNbrMapData;
							}
							/*if( strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0
							  &&strcmp(pEvent->m_sCallingAreaCode, pNbrMapData->m_sNeedSameArea)==0)
								return pNbrMapData;*/
						}
	                    
					}else{
						return pNbrMapData;
					}
				}
	            
				pNbrMapData = pNbrMapData->pNext;

			}
		}
	}
    sprintf(sTemp, "%s=", sValue);
    if( m_pHNbrData[iType]->get(sTemp, &pNbrMapData) ){
        while(pNbrMapData){
            if( strcmp(pEvent->m_sStartDate, pNbrMapData->m_sEffDate)>=0 
              &&strcmp(pEvent->m_sStartDate, pNbrMapData->m_sExpDate)<0 ){
                if(pNbrMapData->m_sNeedSameArea[0]){
                    if(pNbrMapData->m_sNeedSameArea[0]=='*'){
                        if(strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0)
                            return pNbrMapData;
                    }else{
						if(iType == 2 || iType == 1){
							if(strcmp(pEvent->m_sCalledAreaCode, pNbrMapData->m_sNeedSameArea)==0)
								return pNbrMapData;
						}else{
							if(strcmp(pEvent->m_sCallingAreaCode,pNbrMapData->m_sNeedSameArea)==0)
								return pNbrMapData;
						}
                        /*if( strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0
                          &&strcmp(pEvent->m_sCallingAreaCode, pNbrMapData->m_sNeedSameArea)==0)
                            return pNbrMapData;*/
					}
                    
                }else{
                    return pNbrMapData;
                }
            }
            
            pNbrMapData = pNbrMapData->pNext;

        }
    }
    return 0;
}
#else
NbrMapData * NbrMapMgr::getNbrData(int iType,char * sValue,char * sRef,EventSection * pEvent)
{
    NbrMapData *pNbrMapData = 0;
    NbrMapData *pHead = 0;
    unsigned int iIdx = 0;
    char sTemp[100];
	memset(sTemp, '\0', 100);
    bool bExist = false;

    pHead = (NbrMapData *)G_PPARAMINFO->m_poNbrMapList;
    
	if(!(sRef && strcmp(sRef,"#") == 0))
    {
		sprintf(sTemp, "%s_%s", sValue, sRef);
        switch(iType)
        {
            case 1:
            {
                bExist = G_PPARAMINFO->m_poNbrMapType1Index->get(sTemp, &iIdx);
                break;
            }
            case 2:
            {
                bExist = G_PPARAMINFO->m_poNbrMapType2Index->get(sTemp, &iIdx);
                break;
            }
            case 3:
            {
                bExist = G_PPARAMINFO->m_poNbrMapType4Index->get(sTemp, &iIdx);
                break;
            }
            case 4:
            default:
            {
                bExist = G_PPARAMINFO->m_poNbrMapType8Index->get(sTemp, &iIdx);
                break;
            }
        }
        if(bExist)
        {
            pNbrMapData = pHead + iIdx;
            while(pNbrMapData != pHead)
            {
                if( strcmp(pEvent->m_sStartDate, pNbrMapData->m_sEffDate)>=0 
				    &&strcmp(pEvent->m_sStartDate, pNbrMapData->m_sExpDate)<0 )
                {
					if(pNbrMapData->m_sNeedSameArea[0])
                    {
						if(pNbrMapData->m_sNeedSameArea[0]=='*')
                        {
							if(strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0)
								return pNbrMapData;
						}
                        else
                        {
							if(iType == 2 || iType == 1)
                            {
								if(strcmp(pEvent->m_sCalledAreaCode, pNbrMapData->m_sNeedSameArea)==0)
									return pNbrMapData;
							}
                            else
                            {
								if(strcmp(pEvent->m_sCallingAreaCode,pNbrMapData->m_sNeedSameArea)==0)
									return pNbrMapData;
							}
						}
	                    
					}
                    else
                    {
						return pNbrMapData;
					}
				}

                pNbrMapData = pHead + pNbrMapData->m_iNextOffset;
            }
        }
	}

    sprintf(sTemp, "%s_", sValue);
    switch(iType)
    {
        case 1:
        {
            bExist = G_PPARAMINFO->m_poNbrMapType1Index->get(sTemp, &iIdx);
            break;
        }
        case 2:
        {
            bExist = G_PPARAMINFO->m_poNbrMapType2Index->get(sTemp, &iIdx);
            break;
        }
        case 3:
        {
            bExist = G_PPARAMINFO->m_poNbrMapType4Index->get(sTemp, &iIdx);
            break;
        }
        case 4:
        default:
        {
            bExist = G_PPARAMINFO->m_poNbrMapType8Index->get(sTemp, &iIdx);
            break;
        }
    }
    if(bExist)
    {
        pNbrMapData = pHead + iIdx;
        while(pNbrMapData != pHead)
        {
            if( strcmp(pEvent->m_sStartDate, pNbrMapData->m_sEffDate)>=0 
			    &&strcmp(pEvent->m_sStartDate, pNbrMapData->m_sExpDate)<0 )
            {
				if(pNbrMapData->m_sNeedSameArea[0])
                {
					if(pNbrMapData->m_sNeedSameArea[0]=='*')
                    {
						if(strcmp(pEvent->m_sCallingAreaCode, pEvent->m_sCalledAreaCode)==0)
							return pNbrMapData;
					}
                    else
                    {
						if(iType == 2 || iType == 1)
                        {
							if(strcmp(pEvent->m_sCalledAreaCode, pNbrMapData->m_sNeedSameArea)==0)
								return pNbrMapData;
						}
                        else
                        {
							if(strcmp(pEvent->m_sCallingAreaCode,pNbrMapData->m_sNeedSameArea)==0)
								return pNbrMapData;
						}
					}
                    
				}
                else
                {
					return pNbrMapData;
				}
			}

            pNbrMapData = pHead + pNbrMapData->m_iNextOffset;
        }
    }
    
    return 0;
}
#endif
