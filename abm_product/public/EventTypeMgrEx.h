/*VER: 5*/ 
#ifndef _EVENT_TYPE_MGR_H_
#define _EVENT_TYPE_MGR_H_

#include "Log.h"
#include "TOCIQuery.h"
#include "KeyTree.h"
#include "HashList.h"
#include "OrgMgrEx.h"
#include "EdgeRoamMgr.h"
#include "FormatMethod.h"
#include "Head2OrgIDMgr.h"
#include "LongEventTypeRuleEx.h"
#include "OrgLongTypeMgr.h"

#include <list>

#define     MAX_SEGMENT_LENGTH     64

//对应B_NUMBER_TYPE
#define     NUMBER_TYPE_PN          "52Z"
#define     NUMBER_TYPE_IMSI        "52F"
    

using namespace std ;

class EventTypeMgrEx;
class EventTypeMgrEx
{
public:
    
    ~EventTypeMgrEx(void);

public:

    static EventTypeMgrEx* GetInstance(void);
    bool GetAreaCode(int iOrgID,char* pAreaCode,int nSize);

    bool GetOrgIDbyNbr(const char* pNbr,const char* pTime,
        int& iOrgID,const char* tszType="50Z",int* piPartyID =0,int* piPartnerID =0);

    //求取话单的长途和漫游类型
    bool fillLongTypeAndRoamID(EventSection* pEventSection,int iType=0);

    //填充主被叫和第三方的归属或者漫游区域地
    bool fillOrgID(EventSection* pEventSection);


    int GetEventTypeID(EventSection* pEventSection);

    //求取没有漫游的呼转话单的长途类型
    int GetLongTypeNoRoamFW(EventSection* pEventSection);
    //求取漫游呼转话单的长途类型
    int GetLongTypeRoamFW(EventSection* pEventSection);

        //从两点的距离返回长途类型
    int GetLongTypeByDistance(long lDistance);

    //两点的距离返回漫游类型
    int GetRoamTypeByDistance(long lDistance);

    //规整区号和ORG
    //ARG   >>which         int     0x001,主叫,0x010,被叫,3,0x100第三方号码
    //      >>va            int     1,归属地,2,漫游地
    bool fillAreaCode(EventSection* pEventSection, int which=0, int va=0);

    //求取主被叫或第三方号码的ORG_ID
    int GetOrgID(EventSection* pEventSection,int iType);
    /***山东特殊求取HomeType 和 ServType****/

    //山东特殊增加通过td_inter_fee_code 和 td_msisdn_seg 表求取HomeType和ServType
    bool GetSDSpecTypeB(EventSection* pEventSection,char* sType,int iSize,int iType=0);
    /***山东特殊求取HomeType 和 ServType****/
private:
    bool InitInstance(void);
    void ExitInstance(void);


    
    //通过两个ORG_ID判断
    inline int GetLongTypeByOrgID(int iOrgID1,int iOrgID2);
public:
    //通过两个ORG_ID判断漫游类型
    int GetRoamTypeByOrgID(int iOrgID1,int iOrgID2);
private:
    //计费号码没有漫游判断长途类型和事件类型
    inline bool Rule_BillingNoRoam(EventSection* pEventSection);

    //计费号码漫游
    inline bool Rule_BillingRoam(EventSection* pEventSection,int iType=0);

    //求取指定方漫游地或归属地的区号
    inline bool fillAreaCodeSpecify(EventSection* pEventSection,int which=0, int va=0);
private:
    EventTypeMgrEx(void);

    static EventTypeMgrEx* m_gpEventTypeMgrEx;

    Head2OrgIDMgr* m_pLocalHeadMgr;
    
    HeadEx* m_pLocalHead;

    //CHLRMgr* m_pHlrMgr;
    char m_sLongTypeNoModify[32];

    MobileRegionMgr* m_pRoamMgr;
    OrgMgrEx* m_pOrgMgrEx;

    LongEventTypeRuleMgrEx* m_pEventRuleMgr;

    int m_iBillingOrgID,m_iOtherOrgID,m_iBillingVisitOrgID,
        m_iOtherVisitOrgID;
    int m_iCallingCarrier,m_iCalledCarrier;
    /***山东特殊求取HomeType 和 ServType****/
    typedef struct _HEAD_SPEC_INFO_
    {
        char sHomeType[8];
        char sServType[8];
        char sAreaCode[MAX_BILLING_AREA_CODE_LEN];
        char sHead[11];
        char sEffDate[MAX_START_DATE_LEN];
        char sExpDate[MAX_START_DATE_LEN];
    }HeadSpecInfo;
    KeyTree<HeadSpecInfo*>* m_pHeadSpecInfo;  //国内
    KeyTree<HeadSpecInfo*>* m_pHeadSpecInfo_Inter ; //国际
    KeyTree<HeadSpecInfo*>* m_pHeadSpecInfo_national;
    /***山东特殊求取HomeType 和 ServType****/
};


#endif


