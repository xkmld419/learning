
#ifndef ROAMTARIFF_H_INCLUDED_HEADER
#define ROAMTARIFF_H_INCLUDED_HEADER

#include "HashList.h"
#include "CommonMacro.h"
#include "EventSection.h"
#include "Log.h"
//#include "CommandCom.h"
#include "OrgMgrEx.h"

/**
 *  
 * @ 国际漫出资费 (错误码功能未完善,被注释) 
 * @date 2010-08-25 
 * @author wuan
 */

#define CTOC_CARRIER 1
#define CTOG_CARRIER 2


#define PEER_TYPE_LOCAL     1   //主叫本地
#define PEER_TYPE_CHINA     2   //主叫中国
#define PEER_TYPE_OTHER     3   //主叫其他国家地区

/*错误码For 2.8*/
#define CTOC_CARRIER_NOT_EXIST  311
#define CTOC_CARRIER_EXP        312
#define CTOC_CARRIER_NOT_EFF    313
#define CTOC_COUNTRY_NOT_EXIST  314
#define CTOC_SECTOR_NOT_EXIST   315

#define CTOG_CARRIER_NOT_EXIST  321
#define CTOG_CARRIER_EXP        322
#define CTOG_CARRIER_NOT_EFF    323
#define CTOG_COUNTRY_NOT_EXIST  324
#define CTOG_SPONSOR_NOT_EXIST  325
#define CTOG_SECTOR_NOT_EXIST   326

#define TOKEN       "、"



//资费信息
struct TariffInfo {
    //漫游所在资费区标识--index
    int m_iSectorNo;              
    //特殊资费组标识 
    int m_iSpecialGroupID; 
    //事件类型标识            
    int m_iEventTypeID;          
    //费率         
    long m_lValue;          

    char m_sEffDate[9];
    char m_sExpDate[9];

    //同一资费区域下的下一个资费               
    unsigned int m_iNext;               
};


//被访运营商与资费区关系
struct CarrierInfo {
    //被访运营商代码+代理运营商代码+ImsiType--index
    char m_sCarrierCode[8]; 
    //代理运营商代码
    char m_sSponsorCode[8];
    //
    char m_sImsiType[2];            
    //被访运营商所在国家
    char m_sVisitedCountry[500];         
    //资费区标识 
    char m_sSectorID[20];   
    //资费区名称 
    //char m_sSectorName[100]; 
    // 生效日期 
    char m_sEffDate[9];     
    //失效日期           
    char m_sExpDate[9];              

    //同一运营商的下一条记录
    int m_iNext;
};

//代理运营商信息
struct SponsorInfo {
    //代理运营商的IMSI标识
    char m_sSponsorID[8];
    //代理运营商代码
    char m_sSponsorCode[8];
    //缺省资费区
    int m_iDefaultSectorNo;
};

//国家信息
struct CountryInfo {
    //国家地区代码
    char m_sCountryCode[4];
    //区号
    char m_sAreaCode[10];
    //国家地区中文名称
    char m_sCnName[100];
    char m_sEffDate[9];
    char m_sExpDate[9];

    //同一个国家代码下一个国家
    int m_iNext;
};

//特殊资费组
struct SpecialGroupMember {
    int m_iGroupID;
    //成员(区号)
    char m_sMember[10];

    int m_iNext;
};



class RoamTariffMgr {
public:
    ~RoamTariffMgr();
    /**
    *   @param iBelongOrgID:规则所属Org
    */
    static RoamTariffMgr *getInstance();
    /**
     * @param 
     * in  
     *        pEvent         事件
     *        sCarrierCode   运营商代码
     *        sAreaCode      区号
     * out 
     *        lValue         费率
     * @return 成功true/失败false
     */
    bool getValue(EventSection *pEvent, char *sCarrierCode,
                  char *sAreaCode,  long &lValue);
    // /** 
    // * @param sDate: yyyymmdd,默认使用当前时间
    // * @return iErrNo:1-不存在 2-已失效 3-未生效 4-重复
    // */
    //bool auditCountry(int iOrgID, int &iErrNo, char *sDate=0);

    #ifdef PRIVATE_MEMORY
    static void load( );
    static void unload();
    static void reload( );
    #endif

    SponsorInfo * getSponsorInfo( char *sSponsorID);
    CarrierInfo * getCarrierInfo( char *sCarrierCode, char *sDate=0);
    #ifdef PRIVATE_MEMORY
    CountryInfo * getCountryInfo( char * sCnName);  //此方法不可用
    #endif
    //CountryInfo * getCountryInfo( int iOrgID, char *sDate=0);
    bool getSectorNo(char *sSector, int &iSectorNo);

    //bool checkSponsor(char *sSponsorCode, int &iType, int &iErrNo);
    //bool checkCarrier(char *sCarrierCode, int iType, int &iErrNo,
    //                  char *sDate=0);
    //bool checkCountry(char *sCarrierCode, int iType, int &iErrNo);
    //bool checkSector(char *sSectorID, int iType, int &iErrNo);

    bool getAreaCode(char *CountryCode, char *sAreaCode, int iSize, char *sDate=0);
    CountryInfo * getCountryByAreaCode(char *sAreaCode, char *sDate=0);
    bool isBelong(int iGroupID, char *sAreaCode);

protected:
    bool getValue( int iSectorID, int iEventTypeID, char *sAreaCode,
                   char *sDate, long &lValue) ;

    #ifdef PRIVATE_MEMORY
    static void loadCarrierInfo( );
    static void loadTariffInfo( );
    static void loadSectorInfo( );
    static void loadCountryInfo( );
    static void loadSpecialGroupMember();
    static void loadSponsorInfo();
    #endif
private:
    char *strsep(char **stringp, const char *delim);
    void parseKey(char *key, char *sCarrierCode, const char *substr);

private:

    RoamTariffMgr();

    #ifdef PRIVATE_MEMORY
    static HashList<int > *m_poTariffIndex;
    static TariffInfo *m_poTariffList;

    static HashList<int > *m_poCarrierIndex;
    static CarrierInfo *m_poCarrierList;

    static HashList<int > *m_poSectorIndex;

    static HashList<int > *m_poCountryIndex;////--
    static HashList<int > *m_poCountryCodeIndex;
    static HashList<int > *m_poCountryAreaIndex;
    static CountryInfo *m_poCountryList;

    static SponsorInfo *m_poSponsorList;
    static HashList<int > *m_poSponsorIndex;

    static SpecialGroupMember *m_poMemberList;
    static HashList<int> *m_poMemberIndex;

    static bool m_bUpLoaded;
    #endif

    static OrgMgrEx *m_poOrgMgrEx;
    static RoamTariffMgr *m_poRoamTariffMgr;
    
public:
    //For Debug Use
    void printAll();
};



#endif //#ifdef ROAMTARIFF_H_INCLUDED_HEADER

