/******************************************************************************
朱逢胜
2010.11.08
电信计费研发部
NMP信息点提取
*******************************************************************************/
#include <iostream>
#include <pthread.h>
#include "Process.h"
#include "interrupt.h"
#include "Environment.h"
#include "LogManage.h"
#include "MutexLock.h"
#include "TypeDef.h"
#ifndef CNMPINFO_H_
#define CNMPINFO_H_

typedef struct
{
    std::string s_sOid;
    std::string s_sSqlValue;
    std::string s_sContext;
    int s_iType;
    int s_iFrequency;
} T_oidInfo;


class CnmpInfo : public Process
{
public:
    CnmpInfo();
    ~CnmpInfo();
    virtual int run();
    int init();
protected:

    static void* thread_lowInfo(void*);
    static void* thread_highInfo(void*);
    static void* thread_trapInfo(void*);

private:
    bool prepare();

    CLogManage*  m_pLogManage;
    static TOCIQuery* m_pQry;
    static std::vector<T_oidInfo*> m_lowInfo;
    static std::vector<T_oidInfo*> m_highInfo;
    static std::vector<T_oidInfo*> m_trapInfo;

   static CmutexLock* m_pMutexLock;
   static std::string m_sCityCode;
};

#endif
