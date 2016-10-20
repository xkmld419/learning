/*VER: 3*/
#ifndef _EDGE_ROAM_MGR_H_
#define _EDGE_ROAM_MGR_H_
#include "KeyTree.h"
#include "HashList.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "Head2OrgIDMgr.h"
#include <assert.h>

#include <map>

using namespace std ;

#define  KEYTREE_ROAMRULE   KeyTree<EdgeRoamRuleEx* >

class MobileRegionMgr;
class EdgeRoamRuleEx;
class EdgeRoamRuleEx {
public:
    EdgeRoamRuleEx() 
    {
        m_nIsEdage = 0;
        m_iOrgID = 0;
        m_iOrgType =BILL_ORG;
        memset(m_szExpDate,0,sizeof(m_szExpDate));
        memset(m_szEffDate,0,sizeof(m_szEffDate));
        pNext = 0;
    };
public:
    int m_nIsEdage;
	int m_iOrgID;
    int m_iOrgType;
    char m_szExpDate[16];
    char m_szEffDate[16];
    EdgeRoamRuleEx* pNext;
#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
#endif
};

class MobileRegionMgr
{
public:
    MobileRegionMgr(void);
    ~MobileRegionMgr(void);

    static bool  GetOrgID(const char* sMsc,const char* sLac,const char* sCellID,
    		const char* szTime,
        int& iOrgID);

    //检查该交换机是否是指定覆盖类型到指定的org_id
    static bool  CheckCellEdgeType(const char* sMsc, const char* sLac,const char* sCellID,
        const char* szTime,
        int iOrgID,
        int iEdgeType=0);

    static bool GetOrgIDByType(const char* sMsc,const char* sLac,const char* sCellID,
        const char* szTime,
        int* pOrgID,int& iCnt,int iEdgeType,int iOrgType = BILL_ORG);

    //判断Msc是否存在

    static bool MscHasExists(const char* sMsc,char* szDate =0,char* szAreaCode = 0);

    static bool unLoadRule(void);
    static bool reLoadRule(void);

public:
    static bool m_bIsLoad;
private:
    static bool LoadRule(void);
    static KeyTree<EdgeRoamRuleEx* >* m_gpEdgeRoam ;  //边界类型
    static map<int,KEYTREE_ROAMRULE* >* m_gpmapEdgeRoamRule;

    static KeyTree<EdgeRoamRuleEx* >* m_gpmapMsc;

    static EdgeRoamRuleEx * m_gpEdgeRoamRuleEx;

};

#endif


