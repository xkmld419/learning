#include "SockClnt.h"
#include "ABMException.h"
#include "LogV2.h"
#include "TcpSock.h"
#include "abmobject.h"
#include "SockSrvConf.h"
#include "MessageQueue.h"
#include "dccapi.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <vector>
using namespace std;

#define SELECT_MAX   5 
#define OUTTIME_MAX  3
int iDccDWR =1;

void disp_buf(char *buf, int size)
{
  int i;
  printf("buf [%d] start:\n", size);
  for( i = 0 ; i < size; i ++ )
  {
        if( i%16 == 0 )
                printf("\n");
        printf("%02X ", buf[i]&0xff);
  }
  printf("\nbuf end\n");
}

//线程资源
class PthResource
{
public:

    PthResource() {
        m_iRcvPthID = -1;
        m_iSndPthID = -1;
        m_poRcvBuf = NULL;
        m_poSndBuf = NULL;
        m_poRcvMQ = NULL;
        m_poSndMQ = NULL;
        m_poSock = NULL;
    }

    ~PthResource()
    {
        freeRsc();
    }

    int mallocSock() {
        if ((m_poSock=new ConnSock) == NULL) {
            __DEBUG_LOG0_(0, "PthResource::mallocSock m_poSock malloc failed!");
            return -1;
        }
        return 0;
    }

    int mallocRsc(ABMException &oExp, int iRcvKey, int iSndKey) {
        if ((m_poRcvMQ=new MessageQueue(iRcvKey)) == NULL) {
            __DEBUG_LOG0_(0, "PthResource::mallocRsc m_poRcvMQ malloc failed!");
            return -1;
        }
        if (m_poRcvMQ->open(oExp, true, true) != 0) {
            LogV2::logExcep(oExp);
            return -1;
        }
        if ((m_poSndMQ=new MessageQueue(iSndKey)) == NULL) {
            __DEBUG_LOG0_(0, "PthResource::mallocRsc m_poSndMQ malloc failed!");
            return -1;
        }
        if (m_poSndMQ->open(oExp, true, true) != 0) {
            LogV2::logExcep(oExp);
            return -1;
        }

        if ((m_poRcvBuf=new ABMCCR) == NULL) {
            __DEBUG_LOG0_(0, "PthResource::mallocRsc m_poRcvBuf malloc failed!");
            return -1;
        }

        if ((m_poSndBuf=new ABMCCA) == NULL) {
            __DEBUG_LOG0_(0, "PthResource::mallocRsc m_poSndBuf malloc failed!");
            return -1;
        }

        return 0;
    }

    void freeRsc()
    {
    #define __FREE_PTR(x) if (x != NULL) { \
                            delete x; x = NULL; }
        __FREE_PTR(m_poRcvBuf);
        __FREE_PTR(m_poSndBuf);
        __FREE_PTR(m_poRcvMQ);
        __FREE_PTR(m_poSndMQ);
        __FREE_PTR(m_poSock);
    }

    pthread_t m_iRcvPthID;       //接收线程
    pthread_t m_iSndPthID;       //发送线程
    ABMCCR *m_poRcvBuf;          //接收缓存
    ABMCCA *m_poSndBuf;          //发送缓存
    MessageQueue *m_poRcvMQ;     //接收消息队列
    MessageQueue *m_poSndMQ;     //发送消息队列
    ConnSock   *m_poSock;       //链路SOCKET
    SockSrvConf   m_oConf;       //配置信息
    int m_iState;                //资源状态
};

static pthread_attr_t g_oPthAttr = 0;   //线程属性
static vector<SockSrvConf> g_vSockConf;

//dcc api不支持多线程 先写成全局, 多线程会有BUG
static DccApi *g_poDcc = NULL;
static char g_sCerBuf[1024], g_sDwrBuf[1024];
static int g_iCerLen, g_iDwrLen;

static void *sndDCCPth(void *arg);   

static void *createSockPth(void *arg)
{
    int iRet, iTms;
    PthResource *pRsc = (PthResource *)arg;
    if (pRsc->m_poSock->connect(pRsc->m_oConf.m_sLocIP, \
                                pRsc->m_oConf.m_iLocPort, \
                                pRsc->m_oConf.m_sRemIP, \
                                pRsc->m_oConf.m_iRemPort) != 0) {
        __DEBUG_LOG4_(0, "::createSockPth localip=%s localport=%d,remoteip=%s remoteport=%d connect failed!", \
           pRsc->m_oConf.m_sLocIP, pRsc->m_oConf.m_iLocPort, pRsc->m_oConf.m_sRemIP, pRsc->m_oConf.m_iRemPort);
        exit(0);
    }
    
    //发送CER包
    if (pRsc->m_poSock->sendCer((const void *)g_sCerBuf, g_iCerLen) == -1) {
        __DEBUG_LOG1_(0, "::createSockPth socket=%d 发送CER包失败!", pRsc->m_poSock->getSock());
        exit(0);    
    }
    if ((iRet=pthread_create(&pRsc->m_iSndPthID, &g_oPthAttr, sndDCCPth, (void *)pRsc)) != 0) {
        __DEBUG_LOG1_(0, "::createSockPth pthread_create failed! iRet= %d", iRet);
        exit(0);
    }
    
    //从网络接收DCC包放入消息队列
    ABMCCA *pCCA = (ABMCCA *)pRsc->m_poRcvBuf;
    pCCA->m_lType = pRsc->m_oConf.m_iLinkID;
    while (true) {
        if ((iRet=pRsc->m_poSock->selectRD(SELECT_MAX)) == 0) {
            if (iTms++ >= OUTTIME_MAX) {
                __DEBUG_LOG0_(0, "createSockPth 心跳包服务端回复超时,socket重建");
                goto __Rebuilt;    
            }
            if(iDccDWR>30){
            if (pRsc->m_poSock->sendCer((const void *)g_sDwrBuf, g_iDwrLen) == -1) {
                __DEBUG_LOG0_(0, "createSockPth 发送心跳包失败, socket 重建");
                goto __Rebuilt;        
            } 
            iDccDWR=1;
          } 
           iDccDWR=iDccDWR+1;     
        }
        else if (iRet == -1) {
            __DEBUG_LOG0_(0, "createSockPth SELECT FAILED!");
            exit(0);
        }
        iTms = 0;
        if ((iRet=pRsc->m_poSock->recvDCC((void *)pCCA->m_sDccBuf, sizeof(ABMCCA)-8)) == -1) {
            __DEBUG_LOG1_(0, "::createSockPth socket=%d pRsc->m_poSock->recvDCC failed!", pRsc->m_poSock->getSock());
            goto __Rebuilt;
        }

        if (pRsc->m_poSndMQ->Send((void *)pCCA, iRet+8, true, false) <= 0) {
            __DEBUG_LOG3_(0, "createSockPth socket=%d 操作消息队列key=%d出错! errno=%d", \
                     pRsc->m_poSock->getSock(), pRsc->m_oConf.m_iSndMQ, errno);
            exit(0);
        }
        continue;
        
        __Rebuilt:
        if (pRsc->m_poSock->rebuiltSock() == -1) {
            __DEBUG_LOG0_(0, "createSockPth pRsc->m_poSock->rebuiltSock failed!");
            exit(0);    
        } 
        //发送CER包
        if (pRsc->m_poSock->sendCer((const void *)g_sCerBuf, g_iCerLen) == -1) {
            __DEBUG_LOG1_(0, "::createSockPth socket=%d 发送CER包失败!", pRsc->m_poSock->getSock());
            goto __Rebuilt;    
        }   
    }
    return NULL;
}

//从消息队列接收DCC包并发送
static void *sndDCCPth(void *arg)
{
    int iRet;
    PthResource *pRsc = (PthResource *)arg;
    ABMCCA *pCCA = pRsc->m_poSndBuf;
    int iType = pRsc->m_oConf.m_iLinkID;
    char sBuff[1024];

    while (true) {
        if ((iRet=pRsc->m_poRcvMQ->Receive((void *)pCCA, sizeof(ABMCCA), iType, true)) <= 0) {
            __DEBUG_LOG3_(0, "::sndDCCPth socket=%d 接收消息队列 key=%d 操作失败! errno=%d", \
                pRsc->m_poSock->getSock(), pRsc->m_oConf.m_iRcvMQ, errno);
            exit(0);
        }
        memcpy(sBuff, pCCA->m_sDccBuf, iRet -8);
        disp_buf(sBuff, iRet -8);
        if (pRsc->m_poSock->sendDCC((void *)pCCA->m_sDccBuf, iRet-8) == -1) {
            __DEBUG_LOG1_(0, "::sndDCCPth socket=%d 发送DCC包失败!", pRsc->m_poSock->getSock());
            exit(0);
        }
    }
    return NULL;
}

SockClnt::SockClnt()
{
    
}

SockClnt::~SockClnt()
{
    
}        

int SockClnt::init(ABMException &oExp)
{
    int iRet;
    if (g_iProcID == -1) {
        ADD_EXCEPT0(oExp, "SockClnt::init 启动命令 -p 参数没有设置!");
        return -1;
    }

    if ((iRet=pthread_attr_init(&g_oPthAttr)) != 0) {
        ADD_EXCEPT1(oExp, "SockClnt::init pthread_attr_init failed! iRet=%d", iRet);
        return -1;
    }
    if ((iRet=pthread_attr_setdetachstate(&g_oPthAttr, PTHREAD_CREATE_DETACHED)) !=0) {
        ADD_EXCEPT1(oExp, "SockClnt::init pthread_attr_setdetachstate failed! iRet=%d", iRet);
        return -1;
    }
    
    if ((g_poDcc=new DccApi) == NULL) {
        ADD_EXCEPT0(oExp, "SockClnt::init g_poDcc malloc failed!");
        return -1;
    }
    if (g_poDcc->init() != 0) {
        ADD_EXCEPT0(oExp, "SockClnt::init g_poDcc init failed!");
        return -1;
    }

    g_iCerLen = g_poDcc->getCer(g_sCerBuf, sizeof(g_sCerBuf));
    g_iDwrLen = g_poDcc->getDwr(g_sDwrBuf, sizeof(g_sDwrBuf));
    m_mLink.clear();

    return 0;
}

//主线程读取配置生成线程后, 等待SIGUP信号 唤醒
int SockClnt::run()
{
    int iRet;
    PthResource *pRsc;
    ABMException oExp;
    MapIndex oIndex;
    vector<SockSrvConf>::iterator itr;
    map<MapIndex, PthResource*>::iterator itrmap;

    __Refresh:
    g_vSockConf.clear();
    if (ReadSockConf::getConnectConf(g_iProcID, g_vSockConf) != 0) {
        __DEBUG_LOG0_(0, "SockClnt::run ReadSockConf::getConnectConf failed!");
        exit(0);
    }
    for (itr=g_vSockConf.begin(); itr!=g_vSockConf.end(); ++itr) {
        oIndex.m_iPort = itr->m_iLocPort;
        snprintf(oIndex.m_sIP, sizeof(oIndex.m_sIP), "%s", itr->m_sLocIP);
        itrmap = m_mLink.find(oIndex);
        if (itrmap != m_mLink.end())
            continue;
        //新增
        if ((pRsc=new PthResource) == NULL) {
            __DEBUG_LOG0_(0, "SockClnt::run pRsc malloc failed!");
            exit(0);
        }
        if (pRsc->mallocSock() != 0) {
            __DEBUG_LOG0_(0, "SockClnt::run pRsc->mallocSock failed!");
            exit(0);
        }
        if (pRsc->mallocRsc(oExp, itr->m_iRcvMQ, itr->m_iSndMQ) != 0) {
            __DEBUG_LOG0_(0, "SockClnt::run pRsc->mallocRsc failed!");
            exit(0);
        }
        //复制socket信息到pRsc
        memcpy((void *)&(pRsc->m_oConf), (const void *)&itr->m_iLinkID, sizeof(pRsc->m_oConf));
        if ((iRet=pthread_create(&pRsc->m_iRcvPthID, &g_oPthAttr, createSockPth, (void *)pRsc)) != 0) {
            __DEBUG_LOG1_(0, "SockClnt::run pthread_create failed! iRet= %d", iRet);
            exit(0);
        }
        m_mLink.insert(pair<MapIndex, PthResource*>(oIndex, pRsc));
    }
    //先做成睡眠60秒刷新　后续改成信号唤醒刷新
    sleep(60);
    goto  __Refresh;

    return 0;
}
