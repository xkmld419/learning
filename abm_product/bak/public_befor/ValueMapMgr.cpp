/*VER: 1*/ 
#include "ValueMapMgr.h"
#include "FormatMethod.h"
#include "Process.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

#include <stdlib.h>
#include <unistd.h>


bool ValueMapMgr::bInited = false;
ValueMapData * ValueMapMgr::m_poValueMapData = 0;
HashList<ValueMapData *> * ValueMapMgr::m_pHValueData = 0 ;

#ifdef PRIVATE_MEMORY
ValueMapMgr::ValueMapMgr()
{
  init();

}
#else
ValueMapMgr::ValueMapMgr()
{
}
#endif

void ValueMapMgr::init()
{
    bInited = true;
    int iCount = 0;
    
    TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from  b_value_map"
	  " where nvl(app_id,:vAppID)=:vAppID"
	  " and nvl(billflow_id,:vFlowID)=:vFlowID"
	  " and nvl(process_id,:vProcID)=:vProcID");
	qry.setParameter("vAppID" , Process::m_iAppID);
	qry.setParameter("vFlowID", Process::m_iFlowID);
	qry.setParameter("vProcID", Process::m_iProcID);
	qry.open();

	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
    
    if(iCount<=0)
        return;
    
    m_pHValueData = new HashList<ValueMapData *>((iCount>>1) + 16);
    
    m_poValueMapData = new ValueMapData[iCount+1];
    
    if(!m_poValueMapData ){
		 THROW(MBC_HashList+1);
	}
	memset(m_poValueMapData, 0, sizeof(ValueMapData) * (iCount+1) );
	
    qry.setSQL(" select org_value,dest_value,to_number(value_map_type) Mtype,"
        " nvl(to_char(EFF_DATE,'yyyymmddhh24miss'),'19000101000000') EFF,"
        " nvl(to_char(EXP_DATE,'yyyymmddhh24miss'),'30000101000000') EXP"
        " from b_value_map"
	    " where nvl(app_id,:vAppID)=:vAppID"
	    " and nvl(billflow_id,:vFlowID)=:vFlowID"
	    " and nvl(process_id,:vProcID)=:vProcID"
	    " order by EFF_DATE");

	qry.setParameter("vAppID" , Process::m_iAppID);
	qry.setParameter("vFlowID", Process::m_iFlowID);
	qry.setParameter("vProcID", Process::m_iProcID);
    qry.open();
    
    
    int i=0;
    char sTemp[100];
    int iTemp = 0;
	while(qry.next()){
	     
	     strncpy(m_poValueMapData[i].m_sOrgValue,qry.field(0).asString(),31);
	     strncpy(m_poValueMapData[i].m_sDestValue,qry.field(1).asString(),31); 
         m_poValueMapData[i].m_iMapType = qry.field(2).asInteger();
	     strncpy(m_poValueMapData[i].m_sEffDate,qry.field(3).asString(),16);
	     strncpy(m_poValueMapData[i].m_sExpDate,qry.field(4).asString(),16);
	     
	     sprintf(sTemp,"%d=%s",
	         m_poValueMapData[i].m_iMapType,m_poValueMapData[i].m_sOrgValue);

	     m_pHValueData->get(sTemp, &m_poValueMapData[i].pNext );
         m_pHValueData->add(sTemp, &m_poValueMapData[i]);
         
	     i++;
	     if(i>=iCount)
	        break;
	}
	
    
}

#ifdef PRIVATE_MEMORY
bool ValueMapMgr::mapValue(EventSection *  pEvent,  int iType, 
    char * sOrg, char * sRet, int iMaxLen)
{
    if(!bInited) 
        init();
      
    char sTemp[100];
    ValueMapData * pValueMapData;
    sprintf(sTemp, "%d=%s", iType, sOrg);
	if( m_pHValueData->get(sTemp, &pValueMapData) ){
		while(pValueMapData){
			if( strcmp(pEvent->m_sStartDate, pValueMapData->m_sEffDate)>=0 
			  &&strcmp(pEvent->m_sStartDate, pValueMapData->m_sExpDate)<0 ){
			    if(iMaxLen>0){
			        strncpy(sRet, pValueMapData->m_sDestValue, iMaxLen);
			    }else{
			        strncpy(sRet, pValueMapData->m_sDestValue, 
			            sizeof(pValueMapData->m_sDestValue));
			    }
			    return true;

			}
            
			pValueMapData = pValueMapData->pNext;

		}
	}

    return false;
    
}
#else
bool ValueMapMgr::mapValue(EventSection *  pEvent,  int iType, 
    char * sOrg, char * sRet, int iMaxLen)
{
    char sTemp[100];
    unsigned int iIdx = 0;
    ValueMapData * pValueMapData = 0;
    ValueMapData * pHead = G_PPARAMINFO->m_poValueMapList;
    
    memset(sTemp, '\0', 100);
    sprintf(sTemp, "%d_%s", iType, sOrg);
    
	if(G_PPARAMINFO->m_poValueMapIndex->get(sTemp, &iIdx))
    {
        pValueMapData = pHead + iIdx;
        while(pValueMapData != pHead)
        {
            if((pValueMapData->m_iAppID == Process::m_iAppID || pValueMapData->m_iAppID == -1)
                && (pValueMapData->m_iFlowID == Process::m_iFlowID || pValueMapData->m_iFlowID == -1)
                && (pValueMapData->m_iProcID == Process::m_iProcID || pValueMapData->m_iProcID == -1))
            {
                if(strcmp(pEvent->m_sStartDate, pValueMapData->m_sEffDate) >= 0 
			        &&strcmp(pEvent->m_sStartDate, pValueMapData->m_sExpDate) < 0)
                {
                    if(iMaxLen > 0)
                    {
                        strncpy(sRet, pValueMapData->m_sDestValue, iMaxLen);
                    }
                    else
                    {
			            strncpy(sRet, pValueMapData->m_sDestValue, sizeof(pValueMapData->m_sDestValue));
			        }
			        return true;
                }
            }
            pValueMapData = pHead + pValueMapData->m_iNextOffset;
        }
	}
    else
        return false;
    
}
#endif

