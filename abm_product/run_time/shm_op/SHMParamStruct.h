#ifndef __SHM_PARAM_STRUCT_H_INCLUDED_
#define __SHM_PARAM_STRUCT_H_INCLUDED_

#pragma pack(8)

#ifndef OFFSET_HEAD
#define OFFSET_HEAD(c,m) ((long )&((c *)0)->m)
#endif

#define TABLE_HCODE_INFO        1
#define TABLE_MIN_INFO          2
#define TABLE_LOCAL_INFO        3
#define TABLE_IMSI_INFO         4
#define TABLE_MENDB_INFO        5

//hss_hcode_info, local_head 共用此结构体
struct HCodeInfoStruct
{
    char m_sHead[48];
    char m_sTspID[8];
    char m_sZoneCode[8];
    char m_sEffDate[16];
    char m_sExpDate[16];
    unsigned int m_uiHeadID;
    unsigned int m_uiNextOffset;
    
    bool compare(HCodeInfoStruct &oRef) {
        if (strcmp(m_sHead, oRef.m_sHead) || \
            strcmp(m_sTspID, oRef.m_sTspID) || \
            strcmp(m_sZoneCode, oRef.m_sZoneCode) || \
            strcmp(m_sEffDate, oRef.m_sEffDate) || \
            strcmp(m_sExpDate, oRef.m_sExpDate) )
            return false;
        else
            return true;        
    }
    
    void assign(HCodeInfoStruct &oRef) {
        strcpy(m_sHead, oRef.m_sHead);
        strcpy(m_sTspID, oRef.m_sTspID);
        strcpy(m_sZoneCode, oRef.m_sZoneCode);
        strcpy(m_sEffDate, oRef.m_sEffDate);
        strcpy(m_sExpDate,oRef.m_sExpDate);
        m_uiHeadID = oRef.m_uiHeadID;    
    }
};

//MIN码结构体
struct MinInfoStruct
{
    char m_sBeginMin[16];
    char m_sEndMin[16];
    int  m_iUserFlag;
    int  m_iCountryID;
    char m_sHomeCarrCode[8];
    char m_sZoneCode[4];
    int  m_iUserType;
    char m_sEffDate[16];
    char m_sExpDate[16];
    unsigned int m_uiMinInfoID;
    unsigned int m_uiNextOffset;
    
    void assign(MinInfoStruct &oRef) {
        memcpy((void *)m_sBeginMin, (const void *)oRef.m_sBeginMin, OFFSET_HEAD(MinInfoStruct,m_uiNextOffset));   
    }
};

//IMSI 结构体
struct IMSIInfoStruct
{
    char m_sBeginIMSI[16];
    char m_sEndIMSI[16];
    int m_iIMSIType;
    char m_sSponsorCode[8];
    char m_sZoneCode[4];
    char m_sEffDate[16];
    char m_sExpDate[16];
    unsigned int m_uiIMSIID;
    unsigned int m_uiNextOffset;
    
    void assign(IMSIInfoStruct &oRef) {
        memcpy((void *)m_sBeginIMSI, (const void *)oRef.m_sBeginIMSI, OFFSET_HEAD(IMSIInfoStruct,m_uiNextOffset));    
    }
};

#endif/*__SHM_PARAM_STRUCT_H_INCLUDED_*/
