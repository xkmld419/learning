/*VER: 1*/ 
#include "BaseStationMgr.h"

#define _JUST_DBLINK_
#include "Environment.h"

KeyTree<int>* BaseStationMgr::m_poBaseStation = 0;
bool BaseStationMgr::m_bInitFlag = false;
KeyTree<int>* BaseStationMgr::m_poStationTree = 0;

//## init BaseStation info
void BaseStationMgr::init ()
{
    DEFINE_QUERY (qry);
    
    m_poBaseStation = new KeyTree<int>;
    m_poStationTree = new KeyTree<int>;
    
    qry.setSQL ("select gate_way||'|'||nvl(paging_area,0)||'|'||base_station skey, "
        "  station_area_id  from B_BASE_STATION  "
    );
    
    qry.open ();
    while (qry.next ()) {
        m_poBaseStation->add (qry.field("skey").asString(),
            qry.field("station_area_id").asInteger());
    }
    qry.close ();

    qry.setSQL ("select station_area_id, nvl(parent_area_id, -2) p_area_id "
        " from b_base_station_area " );
    
    qry.open ();
    int iTmpStation=0;
    int iStation=0;
    while (qry.next ()) {
        iStation=qry.field("station_area_id").asInteger();
        if(m_poStationTree->get(iStation, &iTmpStation )==false ){
            m_poStationTree->add(iStation, qry.field("p_area_id").asInteger());
        }
    }
    qry.close ();
    
    m_bInitFlag = true;
}

//## 根据 网关/寻呼区(pa)/基站 获取 基站信息
bool BaseStationMgr::getBaseStationInfo (const char sGateWay[],
    const int iPageArea, const char sRP[], int *piRPArea)
{
    if (!m_bInitFlag) init ();
    
    char sKey[100];
    
    sprintf (sKey, "%s|%d|%s", sGateWay, iPageArea, sRP);
    return m_poBaseStation->get (sKey, piRPArea);
}

//## 根据 网关/寻呼区(pa)/基站 获取 基站信息 v2
bool BaseStationMgr::getBaseStationInfo_v2 (const char sGateWay[],
    const int iPageArea, const char sRP[], int *piRPArea)
{
    if (!m_bInitFlag) init ();
    
    char sKey[100];
    
    //const char * saGW[2]={sGateWay, NULL};
    const char * saGW[2] = {sGateWay, ""};
    const int iaPA[2]={iPageArea,0};
    //const char * saRP[2]={sRP, NULL};
    const char * saRP[2] = {sRP, ""};
    
    for(int itemp=0; itemp<2; itemp++){
        for(int jtemp=0; jtemp<2; jtemp++){
            for(int ktemp=0; ktemp<2; ktemp++){
                sprintf (sKey, "%s|%d|%s", saGW[itemp], iaPA[jtemp], saRP[ktemp]);
                if (m_poBaseStation->get (sKey, piRPArea) )   return true;
            }
        }
    }
    return false;    
}


//判断是否属于
bool BaseStationMgr::getBelong(int iChildStation, int iParentStation)
{
    if (!m_bInitFlag) init ();

    int iTmpStation=0;
    if (!m_poStationTree->get (iChildStation, &iTmpStation) ) return false;

    if (iChildStation == iParentStation ) return true;

    while (iTmpStation > 0) {
        if(iTmpStation == iParentStation) return true;
        if(m_poStationTree->get(iTmpStation, &iTmpStation) == false)
            break;
    }

    return false;
}


