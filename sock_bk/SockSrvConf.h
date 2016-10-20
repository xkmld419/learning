#ifndef __SOCKSRV_CONF_H_INCLUDED_
#define __SOCKSRV_CONF_H_INCLUDED_

#include <vector>
using namespace std;

class ReadCfg;

//socket按命令分发的配置
struct SockDisStruct
{
    int m_iLinkID;
    int m_iCmdID;
    int m_iSndMQ;
};

//socket链路资源
struct SockSrvConf
{
    int  m_iLinkID;
    int  m_iProcID;
    char m_sLocIP[16];
    char m_sRemIP[16];
    int  m_iLocPort;
    int  m_iRemPort;
    int  m_iRcvMQ;
    int  m_iSndMQ;
    int  m_iOpt;
};

class ReadSockConf
{
    
public:
    
    static int getListenConf(int iProc, SockSrvConf &oConf);
    
    static int getAcceptConf(int iProc, char *pIP, int iPort, SockSrvConf &oConf);
    
    static int getConnectConf(int iProc, vector<SockSrvConf> &vConf);
    
    static ReadCfg *m_poRead;
};

#endif/*__SOCKSRV_CONF_H_INCLUDED_*/
