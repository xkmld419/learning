/*
*   Version js at 2010-11-25
*/
#ifndef __READ_APP_CONF_H_INCLUDED_
#define __READ_APP_CONF_H_INCLUDED_

#include "ReadCfg.h"
#include "ABMException.h"

/*******累积量配置文件读取类************/
struct ABMConfStruct
{
    int m_iSndKey;     //发送消息队列KEY
    int m_iRcvKey;     //接收消息队列KEY
    int m_iProcID;     //进程ID
    
    int m_iSndToPlatformKey; // 发送自服务平台消息队列KEY
};

struct DccConfStru
{
    int m_iSndKey;          //发送消息队列KEY
    int m_iSndPeerKey;      //发往sock服务器的消息队列（仅解析程序会用）
    int m_iRcvKey;          //接收消息队列KEY
    int m_iProcID;          //进程ID
    char m_sDictFile[128];  //字典文件名称
};

class ABMConf 
{
public:

    static ReadCfg *m_poConf; 

    //根据传入的进程ID，读出相关累积量配置
    static int getConf(int proc, ABMConfStruct &conf, ABMException &oExp);
    
    //dcc消息相关进程的配置读取
    static int getDccConf(int proc, DccConfStru &conf, ABMException &oExp);

};


#endif

