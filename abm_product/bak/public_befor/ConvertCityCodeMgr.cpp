#include "ConvertCityCodeMgr.h"
#include "Environment.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "ID.h"
#include "Value.h"
#include "IDParser.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

#ifdef PRIVATE_MEMORY
ConvertCityCodeMgr::ConvertCityCodeMgr()
{
    m_pCityCodeIndex = NULL;
    m_pAreaCodeIndex = NULL;
    m_pList = NULL;
    load();
    
}
#else
ConvertCityCodeMgr::ConvertCityCodeMgr()
{
    m_pCityCodeIndex = NULL;
    m_pAreaCodeIndex = NULL;
    m_pList = NULL;
}
#endif

#ifdef PRIVATE_MEMORY
ConvertCityCodeMgr::~ConvertCityCodeMgr()
{
    unLoad();
    
}
#else
ConvertCityCodeMgr::~ConvertCityCodeMgr()
{
}
#endif

void ConvertCityCodeMgr::unLoad()
{
    if(!m_pCityCodeIndex){
        m_pCityCodeIndex->clearAll();
        delete m_pCityCodeIndex;    
        m_pCityCodeIndex = NULL;
    }
    if(!m_pAreaCodeIndex){
        m_pAreaCodeIndex->clearAll();
        delete m_pAreaCodeIndex;    
        m_pAreaCodeIndex = NULL;
    }
    if(m_pList) delete [] m_pList;
    m_pList = NULL;    
}

void ConvertCityCodeMgr::load()
{
    static bool bInited = false;
    if(bInited){
        return ;
    }
    bInited = true;
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    qry.setSQL("select count(*)  from b_city_info");
    qry.open();
    if(qry.next()){
        iCount = qry.field(0).asInteger();
        m_pList = new CityInfo[ iCount+1 ];
        m_pAreaCodeIndex = new HashList<CityInfo *>(500);
        m_pCityCodeIndex = new HashList<CityInfo *>(500);
    }
    qry.close();

    qry.setSQL("select AREA_CODE,PROV_CODE,REGION_CODE from b_city_info");
    qry.open();
    int i = 0;
    while(qry.next()){
        strncpy( m_pList[i].m_sAreaCode, qry.field(0).asString(), 32);
        strncpy( m_pList[i].m_sProvCode, qry.field(1).asString(), 32);
        strncpy( m_pList[i].m_sRegionCode, qry.field(2).asString(), 32);
        m_pCityCodeIndex->add( m_pList[i].m_sRegionCode  ,&m_pList[i]);
        m_pAreaCodeIndex->add( m_pList[i].m_sAreaCode  ,&m_pList[i]);
        
        i++;
        if( i>= iCount)
            break;
    
    }
    qry.close();


}


////1，根据联通城市代码得到区号， 2，根据区号得到联通省代码 ，3，由区号得到联通城市代码
#ifdef PRIVATE_MEMORY
bool ConvertCityCodeMgr::convert(int iMode, char * i_sInCode, char * o_sOutCode )
{
    CityInfo * pCityInfo=NULL;
    if(!i_sInCode || !o_sOutCode)
        return false;
        
    if(iMode == 1){
        
        if( m_pCityCodeIndex->get(i_sInCode, &pCityInfo) ){
            strcpy( o_sOutCode,  pCityInfo->m_sAreaCode );
            return true;
        }
    
    
    }else if(iMode == 2){
        
        if( m_pAreaCodeIndex->get(i_sInCode, &pCityInfo) ){
            strcpy( o_sOutCode,  pCityInfo->m_sProvCode );
            return true;
        }    
    
    }else if(iMode == 3){
        ///由区号得到联通城市代码
        if( m_pAreaCodeIndex->get(i_sInCode, &pCityInfo) ){
            strcpy( o_sOutCode,  pCityInfo->m_sRegionCode);
            return true;
        }        
    }

    return false;

}
#else
bool ConvertCityCodeMgr::convert(int iMode, char * i_sInCode, char * o_sOutCode )
{
    unsigned int iIdx = 0;
    CityInfo * pCityInfo=NULL;

    if(!i_sInCode || !o_sOutCode)
        return false;

    if(iMode == 1)
    {
        if(!(G_PPARAMINFO->m_poCityInfoIndex->get(i_sInCode, &iIdx)))
            return false;
        else
        {
            pCityInfo = (CityInfo *)G_PPARAMINFO->m_poCityInfoList + iIdx;
            strcpy(o_sOutCode, pCityInfo->m_sAreaCode);
            return true;
        }
    }
    else if(iMode == 2)
    {
        if(!(G_PPARAMINFO->m_poCityAreaIndex->get(i_sInCode, &iIdx)))
            return false;
        else
        {
            pCityInfo = (CityInfo *)G_PPARAMINFO->m_poCityInfoList + iIdx;
            strcpy(o_sOutCode, pCityInfo->m_sProvCode);
            return true;
        }
    }
    else if(iMode == 3)
    {
        if(!(G_PPARAMINFO->m_poCityAreaIndex->get(i_sInCode, &iIdx)))
            return false;
        else
        {
            pCityInfo = (CityInfo *)G_PPARAMINFO->m_poCityInfoList + iIdx;
            strcpy(o_sOutCode, pCityInfo->m_sRegionCode);
            return true;
        }
    }
    else
        return false;

}
#endif


