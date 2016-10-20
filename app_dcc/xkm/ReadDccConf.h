/*
*   Version js at 2010-11-25
*/
#ifndef __READ_DCC_CONF_H_INCLUDED_
#define __READ_DCC_CONF_H_INCLUDED_

#include "ReadCfg.h"
#include "ABMException.h"

struct DccConfStru
{
    int m_iSndKey;          //发送消息队列KEY
    int m_iSndPeerKey;      //发往sock服务器的消息队列（仅解析程序会用）
    int m_iSndCcaKey;       //主动请求包对应的回包发往的消息队列
    int m_iRcvKey;          //接收消息队列KEY
    int m_iProcID;          //进程ID
    char m_sDictFile[128];  //字典文件名称
	int m_iSockID;
};

struct NodeConfStru
{
    char m_sOriginHost[64];
    char m_sOriginRealm[32];
};

class DccConf 
{
public:

    static ReadCfg *m_poConf; 
    
    //dcc消息相关进程的配置读取
    static int getDccConf(int proc, DccConfStru &conf, ABMException &oExp);
    
    //获取Origin-Host、Origin-Realm等配置
    static int getNodeConf(NodeConfStru &conf, ABMException &oExp);

};


#endif

