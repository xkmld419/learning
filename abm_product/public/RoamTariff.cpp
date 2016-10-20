#include "RoamTariff.h"
#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif
//#include "MBC_28.h"
#include "Date.h"

#include <stdio.h>


    #define DEL_SIN(x)      \
            if( x ){        \
                delete x;   \
                x = 0;      \
            }
    #define DEL_DUL(x)      \
            if( x ){        \
                delete []x; \
                x = 0;      \
            }

#ifdef PRIVATE_MEMORY
bool RoamTariffMgr::m_bUpLoaded = false;
HashList<int> * RoamTariffMgr::m_poCarrierIndex = 0;
CarrierInfo * RoamTariffMgr::m_poCarrierList = 0;
HashList<int > * RoamTariffMgr::m_poSectorIndex = 0;
HashList<int> * RoamTariffMgr::m_poTariffIndex = 0;
TariffInfo * RoamTariffMgr::m_poTariffList = 0;
HashList<int> * RoamTariffMgr::m_poCountryIndex = 0;
HashList<int> * RoamTariffMgr::m_poCountryCodeIndex = 0;
HashList<int> * RoamTariffMgr::m_poCountryAreaIndex = 0;
CountryInfo * RoamTariffMgr::m_poCountryList = 0;
HashList<int> * RoamTariffMgr::m_poSponsorIndex = 0;
SponsorInfo * RoamTariffMgr::m_poSponsorList = 0;
HashList<int> * RoamTariffMgr::m_poMemberIndex = 0;
SpecialGroupMember *RoamTariffMgr::m_poMemberList = 0;
#endif
RoamTariffMgr *RoamTariffMgr::m_poRoamTariffMgr = 0;
OrgMgrEx *RoamTariffMgr::m_poOrgMgrEx = 0;

RoamTariffMgr* RoamTariffMgr::getInstance(){
    if ( !m_poRoamTariffMgr ) {
        m_poRoamTariffMgr = new RoamTariffMgr();
    }
    return RoamTariffMgr::m_poRoamTariffMgr;
}

RoamTariffMgr::RoamTariffMgr( ){

    Environment::useParamShm();

    m_poOrgMgrEx = OrgMgrEx::GetInstance();

    if ( !m_poOrgMgrEx ) {
        Log::log(0, "初始化出错!");
        THROW(1);
    }
    #ifdef PRIVATE_MEMORY
    if (!m_bUpLoaded) {
        load();
    }
    #endif
}

RoamTariffMgr::~RoamTariffMgr(){

    DEL_SIN(m_poOrgMgrEx);
    DEL_SIN(m_poRoamTariffMgr);

    #ifdef PRIVATE_MEMORY
    DEL_SIN(m_poCarrierIndex);
    DEL_SIN(m_poTariffIndex);
    DEL_SIN(m_poSectorIndex);
    DEL_SIN(m_poCountryIndex);
    DEL_SIN(m_poCountryCodeIndex);
    DEL_SIN(m_poCountryAreaIndex);
    DEL_SIN(m_poSponsorIndex);
    DEL_SIN(m_poMemberIndex);

    DEL_DUL(m_poCarrierList);
    DEL_DUL(m_poTariffList);
    DEL_DUL(m_poCountryList);
    DEL_DUL(m_poSponsorList);
    DEL_DUL(m_poMemberList);
    #endif
}
#ifdef PRIVATE_MEMORY
void RoamTariffMgr::load(){

    //Log::log(0, "load carrier info...");
    loadCarrierInfo( );
    //Log::log(0, "load carrier info end...");

    //Log::log(0, "load tariff info...");
    loadTariffInfo( );
    //Log::log(0, "load tariff info end...");

    //Log::log(0, "load sector info...");
    loadSectorInfo( );
    //Log::log(0, "load sector info end...");

    //Log::log(0, "load country info...");
    loadCountryInfo( );
    //Log::log(0, "load country info end...");
    loadSponsorInfo();

    loadSpecialGroupMember();

    m_bUpLoaded = true;
}

void RoamTariffMgr::unload(){

    DEL_SIN(m_poCarrierIndex);
    DEL_SIN(m_poTariffIndex);
    DEL_SIN(m_poSectorIndex);
    DEL_SIN(m_poCountryIndex);
    DEL_SIN(m_poCountryCodeIndex);
    DEL_SIN(m_poCountryAreaIndex);
    DEL_SIN(m_poSponsorIndex);
    DEL_SIN(m_poMemberIndex);

    DEL_DUL(m_poCarrierList);
    DEL_DUL(m_poTariffList);
    DEL_DUL(m_poCountryList);
    DEL_DUL(m_poSponsorList);
    DEL_DUL(m_poMemberList);

    m_bUpLoaded = false;
}

void RoamTariffMgr::reload(){
    unload();
    load( );
}

void RoamTariffMgr::loadCarrierInfo(){
    DEFINE_QUERY(qry);

    int count = 0;
    qry.setSQL("SELECT COUNT(*) FROM b_carrier_sector ");
    qry.open();
    if ( qry.next() ) {
        count = qry.field(0).asInteger();
    }
    count = count | 1;
    qry.close();

    m_poCarrierList = new CarrierInfo[count];
    m_poCarrierIndex = new HashList<int >(count);
    if ( !m_poCarrierList || !m_poCarrierIndex) {
        Log::log(0, "申请内存空间失败!");
        THROW(1);
    }
    memset(m_poCarrierList, 0, sizeof(CarrierInfo) * count );

    qry.setSQL(" SELECT carrier_code, NVL(visited_country, ''), NVL(sponsor_code, 'CtoC'), "
               " NVL(imsi_type,'0'), sector_id, to_char(eff_date, 'yyyymmdd'), "
               " NVL(to_char(exp_date,'yyyymmdd'), '20500101')  "
               " FROM b_carrier_sector a  " );

    int i = 0;
    int *pTemp = NULL;
    int k = 0;

    char key[20]={0};
    qry.open();
    while (qry.next() ) {
        memset(key, 0, sizeof(key));
        strcpy( m_poCarrierList[i].m_sCarrierCode, qry.field(0).asString());
        strcpy( m_poCarrierList[i].m_sVisitedCountry, qry.field(1).asString() );
        strcpy( m_poCarrierList[i].m_sSponsorCode, qry.field(2).asString() );
        strcpy( m_poCarrierList[i].m_sImsiType, qry.field(3).asString());
        strcpy( m_poCarrierList[i].m_sSectorID, qry.field(4).asString()  );
        strcpy( m_poCarrierList[i].m_sEffDate, qry.field(5).asString() );
        strcpy( m_poCarrierList[i].m_sExpDate, qry.field(6).asString() );

        m_poCarrierList[i].m_iNext = -1;

        sprintf( key, "%s_%s", 
                 m_poCarrierList[i].m_sCarrierCode, 
                 m_poCarrierList[i].m_sSponsorCode);

        if ( m_poCarrierIndex->get(key, &k) ) {
            if ( strcmp(m_poCarrierList[i].m_sEffDate, 
                        m_poCarrierList[k].m_sEffDate) >= 0 ) {
                m_poCarrierList[i].m_iNext = k;
                m_poCarrierIndex->add(key, i);
            } else {
                pTemp = &(m_poCarrierList[k].m_iNext);
                while (*pTemp && (-1 != *pTemp)) {
                    if ( strcmp(m_poCarrierList[i].m_sEffDate, 
                                m_poCarrierList[*pTemp].m_sEffDate) >= 0 ) {
                        break;
                    }

                    pTemp = &(m_poCarrierList[*pTemp].m_iNext);
                }
                m_poCarrierList[i].m_iNext = *pTemp;
                *pTemp = i;
            }
        } else {
            m_poCarrierIndex->add( key , i );
        }

        if (++i >= count) {
            break;
        }
    }

    qry.close();

}

void RoamTariffMgr::loadTariffInfo( ){

    DEFINE_QUERY(qry);

    qry.setSQL("SELECT COUNT(*) FROM b_sector_tariff ");
    qry.open();
    int count = 0;
    if ( qry.next() ) {
        count = qry.field(0).asInteger();
    }
    count |= 1;
    qry.close();

    m_poTariffIndex = new HashList<int >(count);
    m_poTariffList = new TariffInfo[count + 1];

    if ( !m_poTariffIndex || !m_poTariffList ) {
        Log::log(0, "申请空间失败!");
        THROW(1);
    }
    memset(m_poTariffList, 0, sizeof(TariffInfo)*(count+1) );

    qry.setSQL("SELECT sector_no, NVL(special_group_id, 0), NVL(event_type_id, 0),"
               " NVL(value, 0), NVL(to_char(eff_date, 'yyyymmdd'), '20000101'), "
               " NVL(to_char(exp_date, 'yyyymmdd'),'20500101' )  "
               " FROM b_sector_tariff a "
               " ORDER BY sector_no, NVL(special_group_id, 0), "
               " NVL(to_char(eff_date, 'yyyymmdd'), '20000101') ");

    int i = 1;
    int k = 0;
    qry.open();
    bool bOrder = false;
    while (qry.next()) {
        m_poTariffList[i].m_iSectorNo = qry.field(0).asInteger();
        m_poTariffList[i].m_iSpecialGroupID = qry.field(1).asInteger();
        m_poTariffList[i].m_iEventTypeID = qry.field(2).asInteger();
        m_poTariffList[i].m_lValue = qry.field(3).asLong();
        strcpy(m_poTariffList[i].m_sEffDate, qry.field(4).asString() );
        strcpy(m_poTariffList[i].m_sExpDate, qry.field(5).asString() );

        bOrder = false;

        if ( m_poTariffIndex->get( m_poTariffList[i].m_iSectorNo, &k ) ) {
            if ( (!m_poTariffList[k].m_iSpecialGroupID &&
                  m_poTariffList[i].m_iSpecialGroupID) 
                 ||
                 (m_poTariffList[k].m_iSpecialGroupID &&
                  m_poTariffList[i].m_iSpecialGroupID &&
                  (strcmp(m_poTariffList[i].m_sEffDate, 
                          m_poTariffList[k].m_sEffDate) >= 0) )
                 ||
                 (!m_poTariffList[k].m_iSpecialGroupID &&
                  !m_poTariffList[i].m_iSpecialGroupID &&
                  (strcmp(m_poTariffList[i].m_sEffDate, 
                          m_poTariffList[k].m_sEffDate) >= 0))
               ) {
                m_poTariffList[i].m_iNext = k;
                m_poTariffIndex->add(m_poTariffList[i].m_iSectorNo, i);
            } else {
                unsigned int *pTemp = &(m_poTariffList[k].m_iNext);

                if ( *pTemp ) {
                    if ( !m_poTariffList[i].m_iSpecialGroupID && 
                         m_poTariffList[*pTemp].m_iSpecialGroupID) {
                        while (*pTemp ) {
                            if ( !m_poTariffList[*pTemp].m_iSpecialGroupID ) {
                                break;
                            }
                            pTemp = &(m_poTariffList[*pTemp].m_iNext);
                        }
                    } else if ( m_poTariffList[i].m_iSpecialGroupID ) {
                        while (*pTemp ) {
                            if ( !m_poTariffList[*pTemp].m_iSpecialGroupID ||
                                 (m_poTariffList[*pTemp].m_iSpecialGroupID &&
                                  (strcmp(m_poTariffList[i].m_sEffDate, 
                                          m_poTariffList[*pTemp].m_sEffDate) >= 0) ) ) {
                                break;
                            }
                            pTemp = &(m_poTariffList[*pTemp].m_iNext);
                        }

                        m_poTariffList[i].m_iNext = *pTemp;
                        *pTemp = i;
                        bOrder = true;
                    }
                }

                if (!bOrder) {
                    while ( *pTemp ) {
                        if ( strcmp(m_poTariffList[i].m_sEffDate, 
                                    m_poTariffList[*pTemp].m_sEffDate) >= 0 ) {
                            break;
                        }

                        pTemp = &(m_poTariffList[*pTemp].m_iNext);
                    }

                    m_poTariffList[i].m_iNext = *pTemp;
                    *pTemp = i;
                }
            }
        } else {
            m_poTariffIndex->add(m_poTariffList[i].m_iSectorNo, i);
        }

        if (++i >= count) {
            break;
        }
    }
    qry.close();
}

void RoamTariffMgr::loadSponsorInfo(){
    DEFINE_QUERY(qry);

    int count = 0;
    qry.setSQL("SELECT COUNT(*) FROM b_sponsor_carrier_info");
    qry.open();
    if (qry.next()) {
        count = qry.field(0).asInteger();
    }
    qry.close();
    count |= 1;

    m_poSponsorIndex = new HashList<int>(count);
    m_poSponsorList = new SponsorInfo[count];

    if ( !m_poSponsorIndex || !m_poSponsorList) {
        Log::log(0, "申请空间失败!" );
        THROW(1);
    }
    memset( m_poSponsorList, 0 ,sizeof(SponsorInfo) * count);

    qry.setSQL("SELECT sponsor_id, NVL(sponsor_code, 'CtoC'), NVL(default_sector_no,0) FROM "
               " b_sponsor_carrier_info ");
    int i = 0;
    qry.open();
    while ( qry.next() ) {
        strcpy( m_poSponsorList[i].m_sSponsorID, qry.field(0).asString());
        strcpy( m_poSponsorList[i].m_sSponsorCode, qry.field(1).asString());
        m_poSponsorList[i].m_iDefaultSectorNo = qry.field(2).asInteger();

        m_poSponsorIndex->add( m_poSponsorList[i].m_sSponsorID, i);

        if (++i >= count) {
            break;
        }
    }
    qry.close();
}

void RoamTariffMgr::loadSpecialGroupMember() {
    DEFINE_QUERY(qry);

    int count = 0;
    qry.setSQL("SELECT COUNT(*) FROM b_special_tariff_group_member");
    qry.open();
    if (qry.next()) {
        count = qry.field(0).asInteger();
    }
    qry.close();
    count |= 1;

    m_poMemberIndex = new HashList<int>(count);
    m_poMemberList = new SpecialGroupMember[count];

    if ( !m_poMemberIndex || !m_poMemberList) {
        Log::log(0, "申请空间失败!" );
        THROW(1);
    }
    memset( m_poMemberList, 0 ,sizeof(SpecialGroupMember) * count);

    qry.setSQL("SELECT group_id, NVL(group_member, '') FROM b_special_tariff_group_member ");
    int i = 0;
    int k = 0;
    qry.open();
    while ( qry.next() ) {
        m_poMemberList[i].m_iGroupID = qry.field(0).asInteger();
        strcpy( m_poMemberList[i].m_sMember, qry.field(1).asString());
        m_poMemberList[i].m_iNext = -1;

        if (m_poMemberIndex->get(m_poMemberList[i].m_iGroupID, &k) ) {
            m_poMemberList[i].m_iNext = k;
        }
        m_poMemberIndex->add( m_poMemberList[i].m_iGroupID, i);

        if (++i >= count) {
            break;
        }
    }
    qry.close();
}

void RoamTariffMgr::loadSectorInfo( ){
    DEFINE_QUERY(qry);

    int count = 0;
    qry.setSQL("SELECT COUNT(*) FROM b_tariff_sector");
    qry.open();
    if (qry.next()) {
        count = qry.field(0).asInteger();
    }
    qry.close();
    count |= 1;

    m_poSectorIndex = new HashList<int>(count);

    if ( !m_poSectorIndex ) {
        Log::log(0, "申请空间失败!" );
        THROW(1);
    }

    char key[20] = {0};

    qry.setSQL("SELECT sector_id, NVL(sponsor_code, 'CtoC'), sector_no FROM b_tariff_sector a ");
    qry.open();
    while (qry.next()) {
        memset(key, 0, sizeof(key) );
        sprintf(key, "%s_%s", qry.field(0).asString(), qry.field(1).asString() );
        m_poSectorIndex->add(key, qry.field(2).asInteger() );
    }
    qry.close();
}

void RoamTariffMgr::loadCountryInfo(){
    DEFINE_QUERY(qry);
    int count = 0;
    qry.setSQL("SELECT COUNT(*) FROM b_country_info ");
    qry.open();
    if ( qry.next() ) {
        count = qry.field(0).asInteger();
    }
    count |= 1;
    qry.close();

    /////m_poCountryIndex = new HashList<int>(count);
    m_poCountryCodeIndex = new HashList<int>(count);
    m_poCountryAreaIndex = new HashList<int>(count);
    m_poCountryList = new CountryInfo[count];

    if ( /*!m_poCountryIndex || */!m_poCountryCodeIndex || !m_poCountryList ) {
        Log::log(0, "申请空间失败!" );
        THROW(1);
    }
    memset(m_poCountryList, 0, sizeof(CountryInfo) * count);

    qry.setSQL("SELECT country_code, NVL(area_code,''), NVL(cn_name, ''), to_char(eff_date,'yyyymmdd'), "
               " NVL(to_char(exp_date,'yyyymmdd'), '20500101') "
               " FROM b_country_info ");
    qry.open();
    int i = 0;
    int *pTemp = NULL;
    int k = 0;
    while ( qry.next() ) {
        strcpy(m_poCountryList[i].m_sCountryCode ,qry.field(0).asString());
        strcpy(m_poCountryList[i].m_sAreaCode, qry.field(1).asString());
        strcpy(m_poCountryList[i].m_sCnName, qry.field(2).asString());
        strcpy(m_poCountryList[i].m_sEffDate, qry.field(3).asString());
        strcpy(m_poCountryList[i].m_sExpDate, qry.field(4).asString());

        m_poCountryList[i].m_iNext = -1;

        ////m_poCountryIndex->add(m_poCountryList[i].m_sCnName, i); 
        m_poCountryAreaIndex->add(m_poCountryList[i].m_sAreaCode, i);

        if ( m_poCountryCodeIndex->get(m_poCountryList[i].m_sCountryCode, &k) ) {
            if ( strcmp(m_poCountryList[i].m_sExpDate, 
                        m_poCountryList[k].m_sExpDate) >= 0 ) {
                m_poCountryList[i].m_iNext = k;
                m_poCountryCodeIndex->add(m_poCountryList[i].m_sCountryCode, i);
            } else {
                pTemp = &(m_poCountryList[k].m_iNext);
                while (*pTemp) {
                    if ( strcmp(m_poCountryList[i].m_sExpDate, 
                                m_poCountryList[*pTemp].m_sExpDate) >= 0 ) {
                        break;
                    }

                    pTemp = &(m_poCountryList[*pTemp].m_iNext);
                }
                m_poCountryList[i].m_iNext = *pTemp;
                *pTemp = i;
            }
        } else {
            m_poCountryCodeIndex->add(m_poCountryList[i].m_sCountryCode, i);
        }

        if ( ++i >= count) {
            break;
        }
    }
    qry.close();
}
#endif
SponsorInfo * RoamTariffMgr::getSponsorInfo(char *sSponsorID){
    if (!sSponsorID) {
        Log::log(0, "输入参数有误!");
        return 0;
    }

    #ifdef PRIVATE_MEMORY
    int k = 0;
    if ( m_poSponsorIndex->get(sSponsorID, &k) ) {
        return &m_poSponsorList[k];
    }
    #else 
    unsigned int k = 0;
    if ( G_PPARAMINFO->m_poSponsorIndex->get(sSponsorID, &k) ) {
        return &(G_PPARAMINFO->m_poSponsorList[k]);
    }
    #endif

    return 0;
}
bool RoamTariffMgr::isBelong(int iGroupID, char *sMember){
    if ( !sMember ) {
        Log::log(0, "输入参数有误!" );
        return false;
    }

    int k = 0;
    #ifdef PRIVATE_MEMORY
    if (m_poMemberIndex->get(iGroupID, &k) ) {
        while ( -1 != k ) {
            if ( !strcmp(m_poMemberList[k].m_sMember, sMember) ) {
                return true;
            }
            k = m_poMemberList[k].m_iNext;
        }
    #else
    unsigned int pos = 0;
    if (G_PPARAMINFO->m_poMemberIndex->get(iGroupID, &pos) ) {
        k = pos;
        while (-1 != k) {
            if ( !strcmp(G_PPARAMINFO->m_poMemberList[k].m_sMember, sMember) ) {
                return true;
            }
            k = G_PPARAMINFO->m_poMemberList[k].m_iNext;
        }
    #endif
    } else {
        Log::log(0, "[ERROR]特殊资费组[GroupID=%d]信息不存在!", iGroupID);
        return false;
    }

    return false;
}

CarrierInfo *RoamTariffMgr::getCarrierInfo(char *key, char *sDate){

    int k = 0;

    #ifdef PRIVATE_MEMORY
    if ( m_poCarrierIndex->get(key, &k) ) {
        if ( !sDate) {
            return &(m_poCarrierList[k]);
        } else {
            while (-1 != k) {
                if ( strncmp(sDate, m_poCarrierList[k].m_sEffDate, 8)>=0 &&
                     strncmp(sDate, m_poCarrierList[k].m_sExpDate, 8)<0 ) {
                    return &(m_poCarrierList[k]);
                }
                k = m_poCarrierList[k].m_iNext;
            }
        }
    }
    #else
    unsigned int pos = 0;
    if (G_PPARAMINFO->m_poRoamCarrierIndex->get(key, &pos) ) {
        if (!sDate) {
            return &(G_PPARAMINFO->m_poRoamCarrierList[pos]);
        } else {
            k = pos ;
            while (-1 != k) {
                if ( strncmp(sDate, G_PPARAMINFO->m_poRoamCarrierList[k].m_sEffDate, 8)>=0 &&
                     strncmp(sDate, G_PPARAMINFO->m_poRoamCarrierList[k].m_sExpDate, 8)<0 ) {
                    return &(G_PPARAMINFO->m_poRoamCarrierList[k]);
                }
                k = G_PPARAMINFO->m_poRoamCarrierList[k].m_iNext;
            }
        }
    }
    #endif

    return 0;
}

#ifdef PRIVATE_MEMORY
CountryInfo *RoamTariffMgr::getCountryInfo(char *sCnName){
    int k = 0;
    if ( m_poCountryIndex->get(sCnName, &k) ) {
        return &(m_poCountryList[k]);
    }

    return 0;
}
#endif
/*
CountryInfo *RoamTariffMgr::getCountryInfo(int iOrgID, char *sDate){
    int k = 0;

    if ( m_poCountryOrgIndex->get(iOrgID, &k) ) {
        if ( !sDate) {
            return &(m_poCountryList[k]);
        } else {
            int pos = k;
            while ( pos ) {
                if ( strcmp(sDate, m_poCountryList[pos].m_sEffDate)>=0 &&
                     strcmp(sDate, m_poCountryList[pos].m_sExpDate)<=0 ) {
                    return &(m_poCountryList[pos]);
                }
                pos = m_poCountryList[pos].m_iNext;
            }
        }
    }

    return 0;
} 
*/ 

bool RoamTariffMgr::getAreaCode(char *sCountryCode, char *sAreaCode, int iSize, char *sDate){
    if (!sCountryCode || !sAreaCode ) {
        Log::log(0, "输入参数有误!");
        return false;
    }

    int k = 0;
    #ifdef PRIVATE_MEMORY
    if ( m_poCountryCodeIndex->get(sCountryCode, &k) ) {
        if ( !sDate ) {
            strncpy(sAreaCode, m_poCountryList[k].m_sAreaCode, iSize);
            sAreaCode[iSize]=0;
            return true;
        } else {
            while (-1 != k ) {
                if ( strncmp(m_poCountryList[k].m_sEffDate, sDate, 8) <=0 &&
                     strncmp(m_poCountryList[k].m_sExpDate, sDate, 8) >0 ) {
                    strncpy(sAreaCode, m_poCountryList[k].m_sAreaCode, iSize);
                    sAreaCode[iSize]=0;
                    return true;
                }
                k = m_poCountryList[k].m_iNext;
            }
        }
    }
    #else
    unsigned int pos = 0;
    if ( G_PPARAMINFO->m_poCountryCodeIndex->get(sCountryCode, &pos) ) {
        if ( !sDate ) {
            strncpy(sAreaCode, G_PPARAMINFO->m_poCountryList[pos].m_sAreaCode, iSize);
            sAreaCode[iSize]=0;
            return true;
        } else {
            k = pos;
            while (-1 != k ) {
                if ( strncmp(G_PPARAMINFO->m_poCountryList[k].m_sEffDate, sDate, 8) <=0 &&
                     strncmp(G_PPARAMINFO->m_poCountryList[k].m_sExpDate, sDate, 8) >0 ) {
                    strncpy(sAreaCode, G_PPARAMINFO->m_poCountryList[k].m_sAreaCode, iSize);
                    sAreaCode[iSize]=0;
                    return true;
                }
                k = G_PPARAMINFO->m_poCountryList[k].m_iNext;
            }
        }
    }
    #endif

    return false;
}

CountryInfo * RoamTariffMgr::getCountryByAreaCode(char *sAreaCode, char *sDate){

    if ( !sAreaCode ) {
        Log::log(0, "输入参数有误!");
        return 0;
    }

    int k = 0;
    #ifdef PRIVATE_MEMORY
    if ( m_poCountryAreaIndex->get(sAreaCode, &k) ) {
        if ( !sDate ) {
            return &(m_poCountryList[k]);
        } else {
            while (-1 != k ) {
                if ( strncmp(m_poCountryList[k].m_sEffDate, sDate, 8) <=0 &&
                     strncmp(m_poCountryList[k].m_sExpDate, sDate, 8) >0 ) {
                    return &(m_poCountryList[k]);
                }
                k = m_poCountryList[k].m_iNext;
            }
        }
    }
    #else 
    unsigned int pos = 0;
    if ( G_PPARAMINFO->m_poCountryAreaIndex->get(sAreaCode, &pos) ) {
        if ( !sDate ) {
            return &(G_PPARAMINFO->m_poCountryList[pos]);
        } else {
            k = pos;
            while (-1 != k ) {
                if ( strncmp(G_PPARAMINFO->m_poCountryList[k].m_sEffDate, sDate, 8) <=0 &&
                     strncmp(G_PPARAMINFO->m_poCountryList[k].m_sExpDate, sDate, 8) >0 ) {
                    return &(G_PPARAMINFO->m_poCountryList[k]);
                }
                k = G_PPARAMINFO->m_poCountryList[k].m_iNext;
            }
        }
    }
    #endif

    return 0;
}

bool RoamTariffMgr::getSectorNo(char *key, int &iSectorNo){
    #ifdef PRIVATE_MEMORY
    if ( m_poSectorIndex->get(key, &iSectorNo) ) {
    #else
    if ( G_PPARAMINFO->m_poTariffSectorIndex->get(key, (unsigned int *) &iSectorNo) ) {
    #endif
        return true;
    } else {
        return false;
    }

    return false;
}


bool RoamTariffMgr::getValue(EventSection *pEventSection, char *sCarrierCode,
                             char * sAreaCode, long &lValue ) {
    int iSectorNo = 0;
    CarrierInfo *pCarrier = NULL;
    SponsorInfo *pSponsor = NULL;
    char sTemp[32] = {0};
    int iType = 0;

    if ( pEventSection->m_oEventExt.m_sIMSI[0] ) {
        snprintf(sTemp,6, "%s", pEventSection->m_oEventExt.m_sIMSI);
        if ( !(pSponsor= getSponsorInfo( sTemp)) ) {
            iType = CTOC_CARRIER;
            sprintf(sTemp, "%s_%s", sCarrierCode, "CtoC");
        } else {
            iType = CTOG_CARRIER;
            sprintf(sTemp, "%s_%s", sCarrierCode, pSponsor->m_sSponsorCode);
        }
    } else {
        Log::log(0, "[ERROR][EventID=%ld]无法判断被访运营商[%s]是否存在代理运营商!", 
                 pEventSection->m_lEventID ,sCarrierCode);
        lValue = 0;

        strcpy (pEventSection->m_sEventState, "ERR");
        // pEventSection->m_aiExtFieldID[9] = pEventSection->m_iEventTypeID;
        // pEventSection->m_iEventTypeID = RATE_PARAM_ERR_EVENT_TYPE;
        pEventSection->m_iErrorID = 4016;

        return false;
    }

    if ( !(pCarrier = getCarrierInfo(sTemp, pEventSection->m_sStartDate)) ) {
        if ( CTOG_CARRIER == iType && pSponsor && pSponsor->m_iDefaultSectorNo ) {
            iSectorNo = pSponsor->m_iDefaultSectorNo;
            Log::log(0, "[TIPS]使用代理运营商[%s]缺省资费区[SectorNo=%d]!",pSponsor->m_sSponsorCode, 
                     pSponsor->m_iDefaultSectorNo );
        } else {
            Log::log(0, "[ERROR][EventID=%ld]找不到被访运营商[ %s ]相关信息!", 
                     pEventSection->m_lEventID, sCarrierCode);
            lValue = 0;

            strcpy (pEventSection->m_sEventState, "ERR");
            // pEventSection->m_aiExtFieldID[9] = pEventSection->m_iEventTypeID;
            // pEventSection->m_iEventTypeID = RATE_PARAM_ERR_EVENT_TYPE;
            pEventSection->m_iErrorID = 4016;

            return false; 
        }
    } else {
        memset(sTemp, 0, sizeof(sTemp) );
        sprintf(sTemp, "%s_%s", pCarrier->m_sSectorID, pCarrier->m_sSponsorCode);
        if ( !getSectorNo(sTemp, iSectorNo) ) {
            if ( CTOG_CARRIER == iType && pSponsor && pSponsor->m_iDefaultSectorNo ) {
                iSectorNo = pSponsor->m_iDefaultSectorNo;
                Log::log(0, "[TIPS]使用代理运营商[%s]缺省资费区[%d]!",pSponsor->m_sSponsorCode, 
                         pSponsor->m_iDefaultSectorNo );
            } else {
                Log::log(0, "[ERROR][EventID=%ld]找不到资费区[ %s ]相关信息!", 
                         pEventSection->m_lEventID, pCarrier->m_sSectorID);
                lValue = 0;

                strcpy (pEventSection->m_sEventState, "ERR");
                // pEventSection->m_aiExtFieldID[9] = pEventSection->m_iEventTypeID;
                // pEventSection->m_iEventTypeID = RATE_PARAM_ERR_EVENT_TYPE;
                pEventSection->m_iErrorID = 4016;

                return false; 
            }
        }
    }
    int bRet = false;

    if ( !(bRet = getValue(iSectorNo,  pEventSection->m_iEventTypeID, sAreaCode,
                           pEventSection->m_sStartDate, lValue) )) {
        Log::log(0, "[ERROR][EventID=%ld]资费失败!",
                 pEventSection->m_lEventID);
        lValue = 0;

        strcpy (pEventSection->m_sEventState, "ERR");
        // pEventSection->m_aiExtFieldID[9] = pEventSection->m_iEventTypeID;
        // pEventSection->m_iEventTypeID = RATE_PARAM_ERR_EVENT_TYPE;
        pEventSection->m_iErrorID = 4016;
    }

    return bRet;
}


bool RoamTariffMgr::getValue(int iSectorID, int iEventTypeID,char * sAreaCode,
                             char *sDate, long &lValue){
    TariffInfo *p = NULL;
    int k = 0;

    #ifdef PRIVATE_MEMORY
    if ( !m_poTariffIndex->get((long)iSectorID, &k) ) {
        return false;
    }
    p = &m_poTariffList[k];
    #else
    unsigned int pos = 0;
    if ( !G_PPARAMINFO->m_poRoamTariffIndex->get((long)iSectorID, &pos) ) {
        return false;
    }
    k = pos;
    p = &(G_PPARAMINFO->m_poRoamTariffList[pos] ) ;
    #endif

    #ifdef PRIVATE_MEMORY
    for (;k;p = &(m_poTariffList[k]) ) {
    #else
    for (;k;p = &(G_PPARAMINFO->m_poRoamTariffList[k]) ) {
    #endif

        k = p->m_iNext;
        if ( p->m_iEventTypeID != iEventTypeID)
            continue;
        if ( p->m_iSpecialGroupID && !isBelong(p->m_iSpecialGroupID, sAreaCode) )
            continue;
        if (sDate && ( strncmp(p->m_sEffDate, sDate, 8) > 0 || 
                       strncmp(p->m_sExpDate, sDate, 8) <= 0) )
            continue;

        lValue = p->m_lValue;

        return true;
    }

    return false;
}

void RoamTariffMgr::printAll(){

    #ifdef PRIVATE_MEMORY

    #else
    unsigned int iCount = G_PPARAMINFO->m_poRoamTariffData->getCount();
    int i = 1;
    printf("SectorNo SpecialMember\tEventTypeID\tValue\tEffDate\tExpDate\tNextLoc\n" );
    while (i <= iCount) {
        printf("%10d%10d%10d%10ld%10s%10s%10d%10d\n",
               G_PPARAMINFO->m_poRoamTariffList[i].m_iSectorNo,
               G_PPARAMINFO->m_poRoamTariffList[i].m_iSpecialGroupID,
               G_PPARAMINFO->m_poRoamTariffList[i].m_iEventTypeID,
               G_PPARAMINFO->m_poRoamTariffList[i].m_lValue,
               G_PPARAMINFO->m_poRoamTariffList[i].m_sEffDate,
               G_PPARAMINFO->m_poRoamTariffList[i].m_sExpDate,
               G_PPARAMINFO->m_poRoamTariffList[i].m_iNext,
               i
              );
        ++i;
    }
    #endif

    return;
}


/*
bool RoamTariffMgr::checkSponsor(char *sSponsorCode, int &iType, int &iErrNo){
    if ( !sSponsorCode || !sSponsorCode[0] ) {
        iType = 1;   //CtoC
    } else {
        iErrNo = CTOG_SPONSOR_NOT_EXIST;
        if ( strcmp(sSponsorCode, "HKG08") && 
             strcmp(sSponsorCode, "NET04") ) {
            //  ALARMLOG28(0, MBC_CLASS_DiffStandCarrier, CTOG_SPONSOR_NOT_EXIST, 
            //             "CtoG代理运营商[ %s ]不存在！", sSponsorCode);
            Log::log(0, "代理意义上[ %s ]不存在!", sSponsorCode);
        }
        iType = 2;   //CtoG
    }
    return true;
}

bool RoamTariffMgr::checkCarrier(char *key, int iType, 
                                 int &iErrNo, char *sDate){
    CarrierInfo *pCarrier = NULL;
    char sTemp[10]={0};
    char sCarrierCode[8] = {0};

    parseKey(key, sCarrierCode, "_");

    if ( !sDate) {
        Date d;
        strcpy(sTemp, d.toString("yyyymmdd"));
    } else {
        strcpy(sTemp, sDate);
    }

    if ( pCarrier = getCarrierInfo(key) ) {
        if ( strcmp(sTemp, pCarrier->m_sEffDate) < 0 ) {
            if (1 == iType) {
                iErrNo = CTOC_CARRIER_NOT_EFF;
                // ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_CARRIER_NOT_EFF, 
                //            "被访运营商[ %s ]在时间[ %s ]尚未生效!", 
                //            sCarrierCode, sTemp);
            } else if ( 2 == iType) {
                iErrNo = CTOG_CARRIER_NOT_EFF;
                //  ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_CARRIER_NOT_EFF,
                //             "被访运营商[ %s ]在时间[ %s ]尚未生效!", 
                //             sCarrierCode, sTemp);
            }
            return false;
        } else if (strcmp( sTemp, pCarrier->m_sExpDate) > 0) {
            if (1 == iType) {
                iErrNo = CTOC_CARRIER_EXP;
                //  ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_CARRIER_EXP, 
                //             "被访运营商[ %s ]在时间[ %s ]已失效!", 
                //             sCarrierCode, sTemp);
            } else if ( 2 == iType) {
                iErrNo = CTOG_CARRIER_EXP;
                //  ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_CARRIER_EXP, 
                //             "被访运营商[ %s ]在时间[ %s ]已失效!", 
                //             sCarrierCode, sTemp);
            }
            return false;
        }
    } else {
        if (1 == iType) {
            iErrNo = CTOC_CARRIER_NOT_EXIST;
            // ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_CARRIER_NOT_EXIST, 
            //            "被访运营商[%s]不存在!", sCarrierCode);
        } else if ( 2 == iType) {
            iErrNo = CTOG_CARRIER_NOT_EXIST;
            //ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_CARRIER_NOT_EXIST, 
            //           "被访运营商[%s]不存在!", sCarrierCode);
        }
        return false;
    }

    return true;
}

bool RoamTariffMgr::checkCountry(char *key, int iType, int &iErrNo){
    char *pCountry = NULL;
    CarrierInfo *pCarrier = NULL;
    char sCarrierCode[8]={0};

    parseKey(key, sCarrierCode, "_");

    if ( pCarrier = getCarrierInfo(key) ) {
        char sTemp[100]={0};
        char *p=NULL, *pCountry;
        strcpy(sTemp, pCarrier->m_sVisitedCountry);
        p = sTemp;
        bool bAllExist = true;
        CountryInfo *pCountryInfo = NULL;

        while ( pCountry = strsep(&p, TOKEN) ) {
            if ( !(pCountryInfo = getCountryInfo(pCountry)) ) {
                bAllExist = false;
                if (1 == iType) {
                    iErrNo = CTOC_COUNTRY_NOT_EXIST;
                    //  ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_COUNTRY_NOT_EXIST,
                    //             "被访运营商[%s]所在国家[ %s ]不存在!", 
                    //             sCarrierCode, pCountry);
                } else if (2 == iType) {
                    iErrNo = CTOG_COUNTRY_NOT_EXIST;
                    // ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_COUNTRY_NOT_EXIST,
                    //            "被访运营商[%s]所在国家[ %s ]不存在!", 
                    //            sCarrierCode, pCountry);
                }

            }
        }//end while
        return bAllExist;
    } else {    //实际上不会走到这里
        if (1 == iType) {
            iErrNo = CTOC_CARRIER_NOT_EXIST;
            //ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_CARRIER_NOT_EXIST, 
            //           "被访运营商[%s]不存在!", sCarrierCode);
        } else if ( 2 == iType) {
            iErrNo = CTOG_CARRIER_NOT_EXIST;
            // ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_CARRIER_NOT_EXIST, 
            //            "被访运营商[%s]不存在!", sCarrierCode);
        }
        return false;
    } 

    return true;
}
 * @param  sDate: yyyymmdd 
 * @return iErrNo:1-不存在 2-已失效 3-未生效 4-重复
bool RoamTariffMgr::auditCountry(int iOrgID, int &iErrNo, char *sDate){
    CountryInfo *pCountryInfo = NULL;
    char sTemp[10]={0};

    if ( !(pCountryInfo = getCountryInfo(iOrgID) ) ) {
        iErrNo = 1;
        return false;
    }

    if ( !sDate ) {
        Date date;
        strcpy(sTemp, date.toString("yyyymmdd") );
    } else {
        strncpy(sTemp, sDate, sizeof(sTemp) );
    }

    if ( strcmp(sTemp, pCountryInfo->m_sExpDate) > 0) {
        iErrNo = 2;  
        return false;   
    } else if (strcmp(sTemp, pCountryInfo->m_sEffDate) < 0) {
        iErrNo = 3;   
        return false;  
    }
    unsigned int k = 0;
    k = pCountryInfo->m_iNext;
    while ( k ) {
        pCountryInfo = &(m_poCountryList[k]);
        if ( strcmp(sTemp, pCountryInfo->m_sExpDate) <= 0 &&
             strcmp(sTemp, pCountryInfo->m_sEffDate) >= 0 ) {
            iErrNo = 4;
            return false;
        }
        k = pCountryInfo->m_iNext;
    }

    return true;
}

bool RoamTariffMgr::checkSector(char *sSectorID, int iType, int &iErrNo){
    int iSectorNo = 0;

    if ( getSectorNo(sSectorID, iSectorNo) ) {
        return true;
    } else {
        if ( 1== iType) {
            iErrNo = CTOC_SECTOR_NOT_EXIST;
            //  ALARMLOG28(0, MBC_CLASS_Carrier,CTOC_SECTOR_NOT_EXIST, 
            //             "资费区[%s]不存在!", sSectorID);    
        } else if ( 2 == iType) {
            iErrNo = CTOG_SECTOR_NOT_EXIST;
            // ALARMLOG28(0, MBC_CLASS_DiffStandCarrier,CTOG_SECTOR_NOT_EXIST, 
            //            "资费区[%s]不存在!", sSectorID);    
        }
        return false;
    }

    return true;
}

*/

void RoamTariffMgr::parseKey(char *key, char *sCarrierCode, const char *substr){
    if (!key || !sCarrierCode) {
        Log::log(0, "参数错误!");
        return;
    }
    char *p = NULL;
    if ( p = strstr(key, substr) ) {
        strncpy(sCarrierCode, key, p - key);
    }
    return;
}

char *RoamTariffMgr::strsep(char **stringp, const char *delim){
    register char *s;
    register const char *spanp;
    register int c,c2, sc, sc2;
    char *tok;

    if ((s = *stringp) == NULL)
        return(NULL);

    for (tok = s;;) {
        c = *s++;
        if ( c > 128) {
            c2 = *s++;
        }
        spanp = delim;
        do {
            if ( (sc = *spanp++) == c ) {
                if (c < 129) {
                    if (c == 0)
                        s = NULL;
                    else
                        s[-1] = 0;
                    *stringp = s;
                    return(tok);
                } else {
                    if ( (sc = *spanp++) == c2 ) {
                        s[-2] = 0;

                        *stringp = s;
                        return(tok);
                    }
                }
            }
        } while (sc != 0);
    }
}





