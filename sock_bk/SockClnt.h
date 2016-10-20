/*
*   全国ABM中心测试版 多线程
*/
#ifndef __SOCKCLNT_H_INCLUDED_
#define __SOCKCLNT_H_INCLUDED_

#include "Application.h"
#include <map>

class ABMException;
class PthResource;

//索引信息
struct MapIndex 
{
    char m_sIP[16];
    int  m_iPort;
    
    bool operator < (MapIndex const& oKey) const {
        if (m_iPort < oKey.m_iPort)
            return true;
        else if (m_iPort > oKey.m_iPort)
            return false;
        else {
            if (strcmp(m_sIP, oKey.m_sIP) < 0)
                return true;
        }
        return false;            
    }        
};

//SOCKET客户端
class SockClnt : public Application
{

public:

    SockClnt();

    ~SockClnt();

    int init(ABMException &oExp);

    int run();

private:
    
    std::map<MapIndex, PthResource*> m_mLink;    
};

DEFINE_MAIN(SockClnt)

#endif/*__SOCKCLNT_H_INCLUDED_*/
