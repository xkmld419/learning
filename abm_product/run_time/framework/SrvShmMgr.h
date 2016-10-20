#ifndef _SRV_SHM_MGR_H_
#define _SRV_SHM_MGR_H_

#include "ABMException.h"
#include "PerfStat.h"
#include "SimpleSHM.h"
#include "SockSrvSHM.h"
#include "AccuTTStruct.h"

//于abm程序使用的共享内存，使用一块
//
struct SrvShmData {
    StatData m_oStatData;
    SockAppStruct m_oSockData;
    AccuTTStruct m_oAccuTTData;
};

class SrvShmMgr {
public:
    //由abminit调用，分配共享内存
    //-1 系统错误
    //-2 参数配置错误
    static int mallocMem(ABMException &oExp);
    static int removeMem(ABMException &oExp);

    //由socksrv, srvlog等调用，连接共享内存
    //-1 系统错误
    //-2 参数配置错误
    static int open(ABMException &oExp, bool bWrite=false, bool bRead=true);

    //由socksrv, srvlog等调用，获取到StatData的首指针
    //返回0代表没有open
    static StatData * getStatData() { return m_poStatData; };

    static SockAppStruct * getSockData() { return m_poSockData; };

    static AccuTTStruct* getTTData() { return m_poAccuTTData; };

private:
    static long getKey(ABMException &oExp);
    static int  openSHM(ABMException &oExp, bool iCreate, unsigned int iPermission);

private:
    static SimpleSHM * m_poSHM;
    static StatData * m_poStatData;
    static SockAppStruct * m_poSockData;
    static AccuTTStruct * m_poAccuTTData;
};

#endif

