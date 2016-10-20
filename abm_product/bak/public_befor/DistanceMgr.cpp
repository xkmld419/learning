#include "DistanceMgr.h"
#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

CDistanceMgr* CDistanceMgr::m_gDistanceMgr =0;

#ifdef PRIVATE_MEMORY
CDistanceMgr* CDistanceMgr::GetInstance(void)
{
    if(CDistanceMgr::m_gDistanceMgr)
        return CDistanceMgr::m_gDistanceMgr;

    CDistanceMgr::m_gDistanceMgr = new CDistanceMgr();
    if(CDistanceMgr::m_gDistanceMgr->Init())
        return CDistanceMgr::m_gDistanceMgr;
    delete CDistanceMgr::m_gDistanceMgr;
    CDistanceMgr::m_gDistanceMgr =0;
    return CDistanceMgr::m_gDistanceMgr;
}
#else
CDistanceMgr* CDistanceMgr::GetInstance(void)
{
    if(CDistanceMgr::m_gDistanceMgr)
        return CDistanceMgr::m_gDistanceMgr;

    CDistanceMgr::m_gDistanceMgr = new CDistanceMgr();
    return CDistanceMgr::m_gDistanceMgr;
}
#endif

#ifdef PRIVATE_MEMORY
bool CDistanceMgr::GetLongTypeByDistance(int iDistance,int& iLongTypeID)
{
    m_itPos = m_mapDistance2LongType.find(iDistance);
    if(m_itPos ==m_itEndLongTypeID)
        return false;
    iLongTypeID=(m_itPos->second);
    return true;
}
#else
bool CDistanceMgr::GetLongTypeByDistance(int iDistance,int& iLongTypeID)
{
    unsigned int iIdx = 0;
    DistanceType *pDistanceType = 0;
    if(!(G_PPARAMINFO->m_poDistanceTypeIndex->get(iDistance, &iIdx)))
        return false;
    else
    {
        pDistanceType = (DistanceType *)G_PPARAMINFO->m_poDistanceTypeList + iIdx;
        iLongTypeID = pDistanceType->m_iLongTypeID;
        return true;
    }
}
#endif

#ifdef PRIVATE_MEMORY
bool CDistanceMgr::GetRoamTypeByDistance(int iDistance,int& iRoamTypeID)
{
    m_itPos = m_mapDistance2RoamType.find(iDistance);
    if(m_itPos == m_itEndRoamID)
        return false;
    iRoamTypeID = (m_itPos->second);
    return true;
}
#else
bool CDistanceMgr::GetRoamTypeByDistance(int iDistance,int& iRoamTypeID)
{
    unsigned int iIdx = 0;
    DistanceType *pDistanceType = 0;
    if(!(G_PPARAMINFO->m_poDistanceTypeIndex->get(iDistance, &iIdx)))
        return false;
    else
    {
        pDistanceType = (DistanceType *)G_PPARAMINFO->m_poDistanceTypeList + iIdx;
        iRoamTypeID = pDistanceType->m_iRoamTypeID;
        return true;
    }
}
#endif

bool CDistanceMgr::LoadTable(const char* pTableName)
{
    if( 0 == pTableName || pTableName[0] == '\0')
        return false;

    DEFINE_QUERY(qry);
    char sSql[1024]={0};

    sprintf(sSql,"SELECT DISTANCE_TYPE_ID,LONG_TYPE_ID FROM %s "
        " where LONG_TYPE_ID IS NOT NULL ",pTableName);
    qry.setSQL(sSql);

    qry.open();
    int iDistance = 0;
    while(qry.next())
    {
        iDistance = qry.field(0).asInteger();
        m_mapDistance2LongType[iDistance] = qry.field(1).asInteger();
    }
    qry.commit();
    qry.close();

    sprintf(sSql," SELECT DISTANCE_TYPE_ID,ROAM_TYPE_ID FROM %s "
        " WHERE ROAM_TYPE_ID IS NOT NULL ",pTableName);
    qry.setSQL(sSql);
    qry.open();

    while(qry.next())
    {
        iDistance = qry.field(0).asInteger();
        m_mapDistance2RoamType[iDistance]=qry.field(1).asInteger();
    }
    qry.commit();
    qry.close();
    return true;
}


bool CDistanceMgr::Init(void)
{
    if(!LoadTable("B_DISTANCE_TYPE"))
        return false;
    
    m_itBeginLongTypeID = m_mapDistance2LongType.begin();
    m_itEndLongTypeID = m_mapDistance2LongType.end();

    m_itBeginRoamID = m_mapDistance2RoamType.begin();
    m_itEndRoamID = m_mapDistance2RoamType.end();
    return true;
}
CDistanceMgr::CDistanceMgr(void)
{
}

CDistanceMgr::~CDistanceMgr(void)
{
}

