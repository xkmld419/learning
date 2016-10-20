/*VER: 1*/ 
#ifndef __BASE_DOMAIN__H__
#define __BASE_DOMAIN__H__

#include "CommonMacro.h"
#include "KeyTree.h"

class BaseStation {
  public:
    //网关信息
    char m_sGateWay[MAX_GATE_WAY_LEN];
    
    //寻呼区(PA)
    int  m_iPageArea; 
    
    //基站
    char m_sRP[MAX_RP_STR_LEN];
    
    //基站区域(根据网关信息、pa区、基站规整)
    int  m_iRPArea;
};

class BaseStationMgr {
  public:
    //## init BaseStation info
    static void init ();  
    
    //## 根据 网关/寻呼区(pa)/基站 获取 基站信息
    static bool getBaseStationInfo (const char sGateWay[], 
        const int iPageArea, const char sRP[], int *piRPArea);

    static bool getBaseStationInfo_v2 (const char sGateWay[], 
        const int iPageArea, const char sRP[], int *piRPArea);
                
    //判断是否属于
    static bool getBelong(int iChildStation, int iParentStation);    
  
  private:
    static KeyTree<int>* m_poBaseStation;
    static KeyTree<int>* m_poStationTree;
    
    static bool m_bInitFlag;
};

#endif



