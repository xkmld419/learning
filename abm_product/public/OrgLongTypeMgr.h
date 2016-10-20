#ifndef _ORG_LONG_TYPE_MGR_H_
#define _ORG_LONG_TYPE_MGR_H_

#include "Environment.h"
#include "HashList.h"
#include "OrgMgrEx.h"

#include <map>
#include <utility>

using namespace std;
class OrgLongType;
class OrgLongType
{
public:
    OrgLongType()
    {
        m_iOrg =0;
        m_iLongType=0;
        m_pNext = 0;
#ifndef PRIVATE_MEMORY
        m_iNextOffset = 0;
#endif
    };
public:
    int m_iOrg;
    int m_iLongType;
    OrgLongType* m_pNext;

#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
#endif
    
};

class OrgLongTypeMgr
{
public:
    OrgLongTypeMgr(void);
    ~OrgLongTypeMgr(void);


    static int GetLongTypeByOrg(int iOrg1,int iOrg2);

private:
    static void Load(void);

private:
    static OrgLongType* m_gpOrgLongType;
    static map<int,OrgLongType*>* m_gpHashOrg2LongTypeID;
    static bool m_bIsLoad;

};

#endif


