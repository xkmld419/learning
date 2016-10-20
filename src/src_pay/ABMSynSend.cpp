#include "ReadAppConf.h"
#include "MessageQueue.h"
#include "QryServSynSend.h"
#include "ABMSynSend.h"
#include "LogV2.h"
#include "abmcmd.h"
#include "PublicConndb.h"

static QryServSynSend *g_poSynSend = NULL;						//资料同步短信下发处理类

/*
*ABMSYNSEND初始化函数
*/
ABMException oExp6;
int ABMSynSend::init(ABMException &oExp)
{
		if (g_iProcID == -1) {
		ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
		return -1;	
}

    if ((g_poSynSend=new QryServSynSend) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poSyn malloc failed!");
        return -1;
    } 
    if (g_poSynSend->init(oExp) != 0) {
        ADD_EXCEPT0(oExp, "ABM累积量业务处理类初始化失败!");
        return -1;
    }
    	if (m_pPublicConndb->init(oExp) != 0)
	{
		ADD_EXCEPT0(oExp, "ABMCheck::init m_pPublicConndb->init failed!");
		return -1;
	}
    
	return 0;	
}

/*
	资料同步短信下发业务处理（send端）
	函数功能：1.开户：新增用户生成CCR请求全国HSS同步后发送短信通知开户成功
*/

int ABMSynSend::run()
{
	int iRet = -1;
	while(!RECEIVE_STOP())
	{
		iRet = g_poSynSend->deal();
		
		// 提交
		if(iRet==0)
		{
        		__DEBUG_LOG0_(0, "m_poAllocateBalance->deal  ---0");
			m_pPublicConndb->Commit(oExp6);
		}
		else
		{
        		__DEBUG_LOG0_(0, "m_poAllocateBalance->deal  ---1");
			m_pPublicConndb->RollBack(oExp6);
		}
		sleep(60);
	}
	
	__DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
	
	return 0;
}

int ABMSynSend::destroy()
{
	//释放业务处理类

	#define __FREE_PTR(P) \
/*
       if (p != NULL) { \
            delete p; p = NULL; }
*/

    __FREE_PTR(g_poSynSend);
    //__FREE_PTR(g_poSndMQ);
    //__FREE_PTR(g_poRcvMQ);
	
	return 0;
}
