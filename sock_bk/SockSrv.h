/*
*   全国ABM中心测试版本  多线程 支持分发
*/
#ifndef __SOCKSRV_H_INCLUDE_
#define __SOCKSRV_H_INCLUDD_

#include "Application.h"
#include <vector>

class ABMException;
class PthResource;
class ListenSock;

class SockSrv : public Application
{

public:

    SockSrv();

    ~SockSrv();

    int init(ABMException &oExp);

    int run();

private:
    
    ListenSock *m_poLsn;
    
    std::vector <PthResource*> m_vRsc;
};

DEFINE_MAIN(SockSrv)

#endif/*__SOCKSRV_H_INCLUDE_*/

