#include "parserDccMsg.h"



static MessageQueue *g_poRcvMQ = NULL;         //接收消息队列
static MessageQueue *g_poSndMQ = NULL;         //发送消息队列 
static MessageQueue *g_poSndPeerMQ = NULL;     //发送消息队列,发往sock服务器

static ABMCCR g_oSndBuf;                  //发送缓存区，发往业务处理进程
static StruMqMsg   g_sRecvBuf;            //接收缓存区，接收从socket发送过来的消息
static StruMqMsg   g_sPeerBuf;            //发送缓存区，不发往后面的业务处理进程，直接发往socket时用到
//static struDccHead goDccHead;           //dcc消息的公共头部信息

int ParserMsgMgr::init(ABMException &oExp)
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
    //发往sock服务器的消息队列
    if (struConf.m_iSndPeerKey <= 0) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init 解析程序需要配置mqsnd_peer.");
        return -1;
    }
    if ((g_poSndPeerMQ=new MessageQueue(struConf.m_iSndPeerKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init g_poSndPeerMQ malloc failed!");
        return -1;
    }
	if (g_poSndPeerMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init g_poSndPeerMQ->open 消息队列连接失败!", struConf.m_iSndKey);
        return -1;
    }
    
    m_poDcc = new DccMsgParser(struConf.m_sDictFile);
    if (NULL == m_poDcc) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poDcc malloc failed!");
        return -1;
    }
    
    return 0;
}

int ParserMsgMgr::run()
{
    int iRecvLen, iRet, iSize;
    string sTemp;
    
    while (!RECEIVE_STOP()) {
        if ((iRecvLen=g_poRcvMQ->Receive((void *)&g_sRecvBuf, MSG_SIZE, 0, true)) < 0) {
			if (iRecvLen == -2) {
				continue;	
			}	
			__DEBUG_LOG1_(0, "应用程序从消息队列接收失败, 程序退出! errno=%d", errno);
			return -1;
		}
		
		iRecvLen -= sizeof(long);
		if (iRecvLen <= 0) {
		    __DEBUG_LOG1_(0, "接收的消息长度有误，消息长度：%d.", iRecvLen);
		    return -1;
		}
		
#ifdef _DEBUG_DCC_
        cout<<"接收到的原始消息包："<<endl;
        disp_buf((char *)&g_sRecvBuf, iRecvLen + sizeof(long));
#endif		
		
		//解包
		iRet = m_poDcc->parseRawToApp(g_sRecvBuf.m_sMsg, iRecvLen);
		if (iRet != 0) {
		    __DEBUG_LOG1_(0, "消息解包出错，错误编号:%d.", -iRet);
		    continue;
		}
		
#ifdef _DEBUG_DCC_
        cout<<"接收到的DCC消息包："<<endl;
        m_poDcc->getAllAvp();
#endif	
		
		if ( (iRet = m_poDcc->getDiameterCommandCodeInfo()) == COMMAND_CODE_CE ) {         //能力交换
		    /*  暂时不开发
		    iRet = packECA(m_poDcc, g_sPeerBuf, &iSize);
		    if (iRet < 0 ) {
		        __DEBUG_LOG0_(0, "返回CEA消息出错.");
		        continue;
		    }
		    
		    if (g_poSndPeerMQ->Send((void *)&g_sPeerBuf, iSize) < 0) {
			    __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			    return -1;	
		    }
		    */ 
		} else if (iRet == COMMAND_CODE_CC) {                                              //业务消息
		    //获取请求消息的公共部分，并获得消息类型
		    g_oSndBuf.clear();
		    iRet = getCCRHead(m_poDcc, &g_oSndBuf);
		    if (iRet < 0 ) {
		        __DEBUG_LOG0_(0, "获取dcc消息头部信息出错.");
		        continue;
		    }
		    
		    switch (iRet)
		    {
		        case ABMCMD_QRY_SRV_INFOR:       //3.4.1.1 资料查询并开户
		            iRet = parserInfoQryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserInfoQryR: 组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_QRY_SRV_INFOR;
		            break;
		        case ABMPAYCMD_INFOR:            //3.4.2.1 支付
		            iRet = parserChargeR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMPAYCMD_INFOR;
		            break;
					
				case ABMCMD_ALLOCATE_BALANCE:            //3.4.5.1 余额被动划拨
		            iRet = AllocateBalanceSvcR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_ALLOCATE_BALANCE;
		            break;
					
				case ABMCMD_QRY_BIL_INFOR:            //3.4.6.16 电子账单查询
		            iRet = RechargeBillQueryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_QRY_BIL_INFOR;
		            break;
					
				case ABMCMD_QRY_PAY_INFOR:            //3.4.6.15 电子账单查询
		            iRet = QueryPayLogByAccoutR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_QRY_PAY_INFOR;
		            break;
					
				case ABMCMD_REVERSE_DEDUCT_BALANCE:            //3.4.5.5 
		            iRet = ReqReverseDeductSvcR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_REVERSE_DEDUCT_BALANCE;
		            break;
					
				case ABMCMD_QRY_REC_INFOR:            //3.4.6.14 
		            iRet = RechargeRecQueryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            g_oSndBuf.m_iCmdID = ABMCMD_QRY_REC_INFOR;
		            break;
					
		        default:
		            __DEBUG_LOG1_(0, "未知的命令请求[%d].", iRet);
		            continue;
		            break;
            }
		    
            //返回消息给调用端
		    if (g_poSndMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
			    __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			    return -1;	
		    } 
		    
		} //业务消息
	}
	
	__DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
	
	return 0;
}

int ParserMsgMgr::destroy()
{
	//释放业务处理类
    #define __FREE_PTR(p) \
        if (p != NULL) { \
            delete p; p = NULL; }
    
    __FREE_PTR(g_poSndMQ);
    __FREE_PTR(g_poRcvMQ);
    __FREE_PTR(g_poSndPeerMQ);
	
	return 0;
}


