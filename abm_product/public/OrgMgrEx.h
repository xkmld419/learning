/*VER: 2*/ 
#ifndef _ORG_MGR_EX_H_
#define _ORG_MGR_EX_H_

#include "Log.h"
#include "TOCIQuery.h"
#include "KeyTree.h"
#include "HashList.h"
#include <map>
#ifdef _DEBUG_
#define DEBUG_LOG(a)    Log::log(0,a)
#define DEBUG_LOG1(a,b)  Log::log(0,a,b)
#else
#define DEBUG_LOG(a)    
#define DEBUG_LOG1(a,b)  
#endif



#define BILL_ORG    1
#define STAT_ORG    2
#define OTHER_ORG   10
using namespace std;
class OrgEx
{
public:
    OrgEx()
    {
        m_lVector = 0;
        m_poParent = 0;
        m_iOrgID=-2;
        m_iParentID=-2;
        m_iLevel =-2;
        memset(sAreaCode,0,sizeof(sAreaCode));
    };
 private:
    friend class OrgMgrEx;
#ifndef PRIVATE_MEMORY
        friend class ParamInfoMgr;
		friend class SeekSHMTool;
#endif

    //##ModelId=42732DF10108
    int m_iOrgID;

    //##ModelId=42732DF60051
    int m_iParentID;

    //##ModelId=42732DFA03E6
    int m_iLevel;

    //##ModelId=42732E420169
    OrgEx *m_poParent;

    long m_lVector;

    char sAreaCode[10];
    
#ifndef PRIVATE_MEMORY
        unsigned int m_iParentOffset;
#endif

};

class OrgMgrEx 
{
public:
    static OrgMgrEx* GetInstance(void);
    static bool reload();
    
    void ExitInstance(void);
    ~OrgMgrEx();
public:


    //##ModelId=4248F8440211
    bool getBelong(int iChildOrg, int iParentOrg,int iOrgType=BILL_ORG);

    //##ModelId=4248F844024D
    //##ModelId=42732CFB01F5
    int getParentID(int iOrgID,int iOrgType=BILL_ORG);

    //added by ap//返回对应Level的ParentID
    int getLevelParentID(int iOrgID, int iLevel,int iOrgType=BILL_ORG);

    //##ModelId=42732D1800AC
    int getLevel(int iOrgID,int iOrgType=BILL_ORG);

    bool getAreaCode(int iOrgID,char* pAreaCode,int nSize,int iLevelID=0,int iOrgType=BILL_ORG);

protected:
    //##ModelId=4248F8730331
    HashList<OrgEx *> *m_poOrgExIndex;
    //##ModelId=42732EB50024
    bool m_bUploadedEx;

    void load(char* pOrgTable);

    //##ModelId=42732E62022D
    OrgEx *m_poOrgExList;

private:
    //##ModelId=4248F84401D5
    void unload();
    OrgMgrEx(void);
    bool Init(void);
    static OrgMgrEx* m_gOrgMgrEx;
    map<int,OrgMgrEx*> m_mapType2OrgEx;
    map<int,OrgMgrEx*>::const_iterator m_ItEnd;
    OrgMgrEx* m_pOrgMgrEx_Bill;
};

#endif

