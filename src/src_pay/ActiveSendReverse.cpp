#include "ActiveSendReverse.h"
#include "ReadAppConf.h"
#include "MessageQueue.h"
#include "ActiveSendReverseBiz.h"
#include "LogV2.h"
#include "abmcmd.h"

static MessageQueue *g_poSndMQ = NULL;         				//发送消息队列 
static ActiveSendReverseBiz *g_activeSendReverseBiz     = NULL;         //ABMCenter业务处理类
static ABMCCR g_oSndBuf;                 				//发送缓存

/*
 *	ActiveSendReverse处理程序的初始化函数    
 */
int ActiveSendReverse::init(ABMException &oExp)
{
	if (g_iProcID == -1) 
	{
		ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
		return -1;	
	}
	
	//读取配置文件
	ABMConfStruct appconf;
	if (ABMConf::getConf(g_iProcID, appconf, oExp) != 0) 
	{
		ADD_EXCEPT1(oExp, "ABMMgr::init 失败, 请检查ABMMgr -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
		return -1;		
	}

	//连接IPC资源 
	if ((g_poSndMQ = new MessageQueue(appconf.m_iSndKey)) == NULL) 
	{
        	ADD_EXCEPT0(oExp, "ABMMgr::init g_poSndMQ malloc failed!");
        	return -1;
    	}
    	
	if (g_poSndMQ->open(oExp, true, true) != 0) 
	{
        	ADD_EXCEPT1(oExp, "ABMMgr::init g_poSndMQ->open 消息队列连接失败!", appconf.m_iSndKey);
        	return -1;
    	}
    
	if ((g_activeSendReverseBiz = new ActiveSendReverseBiz) == NULL) 
	{
        	ADD_EXCEPT0(oExp, "ActiveSendReverse::init g_activeSendReverseBiz malloc failed!");
        	return -1;
    	} 
	if (g_activeSendReverseBiz->init(oExp) != 0) 
	{
        	ADD_EXCEPT0(oExp, "ABM主动发送冲正请求业务处理类初始化失败!");
        	return -1;
    	}
    
	return 0;	
}

/*
 *	业务处理
 *	函数功能:
 *	         扫描ACTIVE_CCR_LOG日志表，对于状态为00X的记录向省ABM发送冲正请求 
 */
int ActiveSendReverse::run()
{
	int iRet=-1;
		
	while (!RECEIVE_STOP()) 
	{
		g_oSndBuf.clear();
		g_oSndBuf.m_iSize = 0;
		g_activeSendReverseBiz->deal((ABMCCR*)&g_oSndBuf);
		
		//发送消息给省ABM
		//__DEBUG_LOG0_(0, "发送划拨冲正消息至省ABM的消息队列");
		if(g_oSndBuf.m_iSize == 0)
		{
			continue;	
		}
		else
		{
			if (g_poSndMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) 
			{
				__DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
				return -1;	
			}
			__DEBUG_LOG0_(0, "发送划拨冲正消息至省ABM的消息队列成功！");
		}
		
	}
	__DEBUG_LOG0_(0, "集团主动发送划拨冲正-程序收到安全退出信号, 安全退出");
	
	return 0;
}

int ActiveSendReverse::destroy()
{
	//释放业务处理类
	#define __FREE_PTR(p) \
	if (p != NULL) { \
		delete p; p = NULL; }
	
	__FREE_PTR(g_activeSendReverseBiz);
	__FREE_PTR(g_poSndMQ);
	
	return 0;
}

