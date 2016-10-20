/*VER: 1*/ 
#ifndef _USAGETYPEMGR_H_
#define _USAGETYPEMGR_H_

#include "CommonMacro.h"
#include "KeyTree.h"
#include "HashList.h"

class UsageTypeMgr
{
public:
    UsageTypeMgr(void);
    ~UsageTypeMgr(void);


public:
    //装载 b_usage_type 表
    void load();

    //卸载B_USAGE_TYPE　表
    void unload();

    //是否需要计算费率计费单元
    bool needCal(int nUsageTypeID);

private:

private:
    static bool m_bUploaded;    //标记是否已经装载

public:
    typedef struct _USAGE_TYPE_
    {
        int iUsageTypeID;
        int bNeedCal;
		char m_sUsageTypeCode[50];
    }stUsageType;
private:
    static UsageTypeMgr::stUsageType *m_poEventTypeList;
    static HashList<UsageTypeMgr::stUsageType*>* m_poUsageTypeSerial;  //UsageTypeID索引
};

#endif
