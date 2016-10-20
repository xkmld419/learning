#include "ABMMgr.h"
#include "ReadAppConf.h"
#include "MessageQueue.h"
#include "ABMSvcSupport.h"
#include "LogV2.h"
#include "abmcmd.h"

static MessageQueue *g_poRcvMQ = NULL;         //接收消息队列
static MessageQueue *g_poSndMQ = NULL;         //发送消息队列 
static ABMSvcSupt *g_poSvc     = NULL;         //ABMCenter业务处理类
static ABMCCR g_oRcvBuf;                 //接收缓存
static ABMCCA g_oSndBuf;                 //发送缓存

// add 2011.7.6
static MessageQueue *g_poSndToPlatformMQ = NULL;         //发送消息队列-给自服务平台 
bool g_toPlatformFlag = false;

/*
*	ABM-CENTER 管理程序的初始化函数    
*/
int ABMMgr::init(ABMException &oExp)
{
	if (g_iProcID == -1) {
		ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
		return -1;	
	}
	
	//读取配置文件
    ABMConfStruct appconf;
	if (ABMConf::getConf(g_iProcID, appconf, oExp) != 0) {
		ADD_EXCEPT1(oExp, "ABMMgr::init 失败, 请检查ABMMgr -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
		return -1;		
	}

	//连接IPC资源 
	if ((g_poRcvMQ=new MessageQueue(appconf.m_iRcvKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poRcvMQ malloc failed!");
        return -1;
    }
	if (g_poRcvMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ABMMgr::init g_poRcvMQ->open 消息队列连接失败!", appconf.m_iRcvKey);
        return -1;
    }
	if ((g_poSndMQ=new MessageQueue(appconf.m_iSndKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poSndMQ malloc failed!");
        return -1;
    }
	if (g_poSndMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ABMMgr::init g_poSndMQ->open 消息队列连接失败!", appconf.m_iSndKey);
        return -1;
    }
    
    // add 2011.7.6 初始化发送自服务平台消息队列
    if ((g_poSndToPlatformMQ=new MessageQueue(appconf.m_iSndToPlatformKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poSndToPlatformMQ malloc failed!");
        return -1;
    }
    
    if (g_poSndToPlatformMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ABMMgr::init g_poSndToPlatformMQ->open 消息队列连接失败!", appconf.m_iSndToPlatformKey);
        return -1;
    }
    
    if ((g_poSvc=new ABMSvcSupt) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poSvc malloc failed!");
        return -1;
    } 
    if (g_poSvc->init(oExp) != 0) {
        ADD_EXCEPT0(oExp, "ABM累积量业务处理类初始化失败!");
        return -1;
    }
    
	return 0;	
}

/*
	累积量业务处理
	函数功能:
	         从接收队列取消息, 分发到具体业务类处理, 返回结果到发送队列 
*/
int ABMMgr::run()
{
	int iRet;
		
	while (!RECEIVE_STOP()) {

		if ((iRet=g_poRcvMQ->Receive((void *)&g_oRcvBuf, sizeof(g_oRcvBuf), 0, true)) < 0) {
			if (iRet == -2) {
				continue;	
			}	
			__DEBUG_LOG1_(0, "应用程序从消息队列接收失败, 程序退出! errno=%d", errno);
			return -1;
		}			
		
		g_poSvc->deal((ABMCCR*)&g_oRcvBuf, (ABMCCA*)&g_oSndBuf); 
		
		// add 2011.7.6 增加判断发送到哪个队列，1.省ABM 、 2.自服务平台
		struDccHead Head;
	
		memcpy((void *)&Head,(void *)(g_oSndBuf.m_sDccBuf),sizeof(Head));
		
		if(g_toPlatformFlag)
		{
			// 发送至自服务平台
			__DEBUG_LOG0_(0, "返回消息至自服务平台的消息队列");
			//返回消息给调用端
			if (g_poSndToPlatformMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
			__DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			g_toPlatformFlag = false;
			return -1;	
			}
			g_toPlatformFlag = false;
		}
		else
		{
			//返回消息给调用端
			__DEBUG_LOG0_(0, "返回消息至省ABM的消息队列");
			if (g_poSndMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
			__DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			return -1;	
			}
		}	
	}
	
	__DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
	
	return 0;
}

int ABMMgr::destroy()
{
	//释放业务处理类
    #define __FREE_PTR(p) \
        if (p != NULL) { \
            delete p; p = NULL; }

    __FREE_PTR(g_poSvc);
    __FREE_PTR(g_poSndMQ);
    __FREE_PTR(g_poRcvMQ);
	
	return 0;
}
