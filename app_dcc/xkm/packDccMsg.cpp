#include "packDccMsg.h"

static MessageQueue *g_poRcvMQ = NULL;         //接收消息队列
static MessageQueue *g_poSndMQ = NULL;         //发送消息队列 

static ABMCCA    g_oRcvBuf;                    //发送缓存区，发往业务处理进程
static StruMqMsg g_sSndBuf;                    //发送缓存区，发往socket
//static struDccHead goDccHead;                //dcc消息的公共头部信息


int PackMsgMgr::init(ABMException &oExp)
{
    char *p;
    
    if (g_iProcID == -1) {
        ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
        return -1;  
    }
    
    //读取配置文件
    DccConfStru struConf;
    if (ABMConf::getDccConf(g_iProcID, struConf, oExp) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init 失败, 请检查ParserMsgMgr -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
        return -1;      
    }
    
    //消息队列初始化
    if ((g_poRcvMQ=new MessageQueue(struConf.m_iRcvKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ABMMgr::init g_poRcvMQ malloc failed!");
        return -1;
    }
    if (g_poRcvMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init g_poRcvMQ->open 消息队列连接失败!", struConf.m_iRcvKey);
        return -1;
    }
    if ((g_poSndMQ=new MessageQueue(struConf.m_iSndKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init g_poSndMQ malloc failed!");
        return -1;
    }
    if (g_poSndMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init g_poSndMQ->open 消息队列连接失败!", struConf.m_iSndKey);
        return -1;
    }
    
    m_poDcc = new DccMsgParser(struConf.m_sDictFile);
    if (NULL == m_poDcc) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poDcc malloc failed!");
        return -1;
    }
    
    return 0;
}

int PackMsgMgr::run()
{
    int iRet;
    int iMsgLen;
    
    while (!RECEIVE_STOP()) {
        if ((iRet=g_poRcvMQ->Receive((void *)&g_oRcvBuf, sizeof(g_oRcvBuf), 0, true)) < 0) {
            if (iRet == -2) {
                continue;   
            }   
            __DEBUG_LOG1_(0, "应用程序从消息队列接收失败, 程序退出! errno=%d", errno);
            return -1;
        }
        
        if (setCCAHead(m_poDcc, &g_oRcvBuf) != 0) {
            __DEBUG_LOG0_(0, "打包消息公共部分出错失败! ");
            continue;
        }
        
        switch (g_oRcvBuf.m_iCmdID)
        {
            case ABMCMD_QRY_SRV_INFOR:                  //3.4.1.1 资料查询并开户
                iRet = packInfoQryA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
            case ABMPAYCMD_INFOR:                       //3.4.2.1 支付
                iRet = packChargeA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
				
			case ABMCMD_ALLOCATE_BALANCE:                       //3.4.5.1 余额被动划拨
                iRet = AllocateBalanceSvcA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
				
			case ABMCMD_QRY_BIL_INFOR:                       //3.4.6.16 电子账单查询
                iRet = RechargeBillQueryA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
			
			case ABMCMD_QRY_PAY_INFOR:                       //3.4.6.15
                iRet = QueryPayLogByAccoutA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
			
			case ABMCMD_REVERSE_DEDUCT_BALANCE:                       //3.4.5.5
                iRet = ReqReverseDeductSvcA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
				
			case ABMCMD_QRY_REC_INFOR:                       //3.4.6.14
                iRet = RechargeRecQueryA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packInfoQryA: DCC打包错误.");
		            continue;
		        }
                break;
				
				
            default:
                break;
        }
        
#ifdef _DEBUG_DCC_
        cout<<"CCA打包前avp："<<endl;
        m_poDcc->getAllAvp();
#endif        
        iMsgLen = m_poDcc->parseAppToRaw(g_sSndBuf.m_sMsg, MSG_SIZE);
        if (iMsgLen <= 0) {
            __DEBUG_LOG0_(0, "打包失败! ");
            continue;
        }
        
#ifdef _DEBUG_DCC_
        cout<<"CCA消息发送前原始消息包："<<endl;
        disp_buf((char *)&g_sSndBuf, iMsgLen + sizeof(long));
#endif          
        if (g_poSndMQ->Send((void *)&g_sSndBuf, iMsgLen+sizeof(long)) < 0) {
	        __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			return -1;	
		} 
    }
    
    return 0;
}

int PackMsgMgr::destroy()
{
    //释放业务处理类
    #define __FREE_PTR(p) \
        if (p != NULL) { \
            delete p; p = NULL; }
    
    __FREE_PTR(g_poSndMQ);
    __FREE_PTR(g_poRcvMQ);
    
    return 0;
}
