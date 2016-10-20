/*VER: 1*/ 
#include "UsageTypeMgr.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include "Exception.h"
#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

HashList<UsageTypeMgr::stUsageType*>* UsageTypeMgr::m_poUsageTypeSerial = 0;
bool UsageTypeMgr::m_bUploaded = false;
UsageTypeMgr::stUsageType *UsageTypeMgr::m_poEventTypeList = 0;
UsageTypeMgr::UsageTypeMgr(void)
{
#ifdef PRIVATE_MEMORY
    if(!UsageTypeMgr::m_bUploaded)
    {
        load();
        UsageTypeMgr::m_bUploaded = true;
    }
#endif
}

UsageTypeMgr::~UsageTypeMgr(void)
{
#ifdef PRIVATE_MEMORY
    unload();
#endif
}

void UsageTypeMgr::load()
{
    if( UsageTypeMgr::m_poEventTypeList!=0)
        delete[] UsageTypeMgr::m_poEventTypeList;
    UsageTypeMgr::m_poEventTypeList = 0;

    TOCIQuery qry (Environment::getDBConn ());
	int nCnt, i;


	qry.setSQL ("SELECT COUNT(*) FROM B_USAGE_TYPE");
	qry.open (); qry.next ();
	nCnt = qry.field(0).asInteger () + 16;	
    qry.close();
    UsageTypeMgr::m_poEventTypeList = new UsageTypeMgr::stUsageType[nCnt];
    memset(UsageTypeMgr::m_poEventTypeList,0,sizeof(UsageTypeMgr::stUsageType)*nCnt);
    if(!UsageTypeMgr::m_poEventTypeList)
        THROW(MBC_EventTypeMgr+1);

    UsageTypeMgr::m_poUsageTypeSerial = new HashList<stUsageType*>(nCnt);
    if(!UsageTypeMgr::m_poUsageTypeSerial)
        THROW(MBC_EventTypeMgr+1);

    qry.setSQL(" SELECT USAGE_TYPE_ID,NVL(CAL_BILLING_UNIT,-1) FROM  B_USAGE_TYPE ");
    qry.open();
    i= 0;
    while(qry.next())
    {
        UsageTypeMgr::m_poEventTypeList[i].iUsageTypeID = qry.field(0).asInteger();
        UsageTypeMgr::m_poEventTypeList[i].bNeedCal = qry.field(1).asInteger();
        UsageTypeMgr::m_poUsageTypeSerial->add(UsageTypeMgr::m_poEventTypeList[i].iUsageTypeID,&UsageTypeMgr::m_poEventTypeList[i]);
        i++;
    }
    qry.close();
}

void UsageTypeMgr::unload()
{
    if( UsageTypeMgr::m_poEventTypeList!=0)
        delete[] UsageTypeMgr::m_poEventTypeList;
    UsageTypeMgr::m_poEventTypeList = 0;
    UsageTypeMgr::m_poUsageTypeSerial->clearAll();
    UsageTypeMgr::m_poUsageTypeSerial->destroy();
}

#ifdef PRIVATE_MEMORY
bool UsageTypeMgr::needCal(int nUsageTypeID)
{
    UsageTypeMgr::stUsageType* pUsageType;
    if(!UsageTypeMgr::m_poUsageTypeSerial->get(nUsageTypeID,&pUsageType))
        return false;
    return pUsageType->bNeedCal>0 ?true:false;
}
#else
bool UsageTypeMgr::needCal(int nUsageTypeID)
{
    UsageTypeMgr::stUsageType *m_poUsageType = (UsageTypeMgr::stUsageType*)(*G_PPARAMINFO->m_poUsageTypeInfoData);
	 unsigned int iUsageType = 0;
     if(!G_PPARAMINFO->m_poUsageTypeIndex->get(nUsageTypeID,&iUsageType))
         return false;
     return m_poUsageType[iUsageType].bNeedCal>0 ?true:false;
}
#endif
