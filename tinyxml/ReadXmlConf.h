/*
*   Version js at 2010-11-25
*/
#ifndef __READ_XML_CONF_H_INCLUDED_
#define __READ_XML_CONF_H_INCLUDED_

#include "ReadCfg.h"
#include "ABMException.h"

struct XmlConfStru
{
    int m_iSndKey;          //发送消息队列KEY
    int m_iSndPeerKey;      //发往sock服务器的消息队列（仅解析程序会用）
    int m_iRcvKey;          //接收消息队列KEY
	int m_iSndCCAKey;        //发送心跳包的消息队列
    int m_iProcID;          //进程ID
};

class XmlConf 
{
public:

    static ReadCfg *m_poConf; 
    
    //dcc消息相关进程的配置读取
    static int getXmlConf(int proc, XmlConfStru &conf, ABMException &oExp);

};


#endif

