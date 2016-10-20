#include "packDccMsg.h"

static MessageQueue *g_poRcvMQ = NULL;         //接收消息队列
static MessageQueue *g_poSndMQ = NULL;         //发送消息队列 

static ABMCCA    g_oRcvBuf;                    //发送缓存区，发往业务处理进程
static StruMqMsg g_sSndBuf;                    //发送缓存区，发往socket
//static struDccHead goDccHead;                //dcc消息的公共头部信息
extern NodeConfStru g_oNodeConf;               //业务节点的一些固定配置
int SockID;


//测试添加入库
char m_sSessionIdCCR[128];
char m_sservNUMCCR[128];
char m_sdstrealmCCR[128];
//测试添加入库

int PackMsgMgr::init(ABMException &oExp)
{
    char *p;
    
    if (g_iProcID == -1) {
        ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
        return -1;  
    }
    
    //读取配置文件
    DccConfStru struConf;
    if (DccConf::getDccConf(g_iProcID, struConf, oExp) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init 失败, 请检查ParserMsgMgr -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
        return -1;      
    }
    
    //业务节点相关的配置信息
	if (DccConf::getNodeConf(g_oNodeConf, oExp) != 0) {
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
    
	SockID=struConf.m_iSockID;
    m_poDcc = new DccMsgParser(struConf.m_sDictFile);
    if (NULL == m_poDcc) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poDcc malloc failed!");
        return -1;
    }
    
    return 0;
}

int PackMsgMgr::run()
{
    int iRet, iFlag;
    int iMsgLen;
	
     //测试添加入库
		string sORACLEName = "ABM";
        string sORACLEuser = "abm1";
        string sORACLEpwd = "abm1";
		  OracleConn *pOracleConn = NULL;
          TOCIQuery *pQryCursor = NULL;  
          pOracleConn = new OracleConn;        
          if (!pOracleConn->connectDb(sORACLEuser, sORACLEpwd, sORACLEName)) {
               printf("ORACLE CONNECT ERROR");
               return  - 1;
          }
          DEFINE_OCICMDPOINTBYCONN(pQryCursor, (*pOracleConn));
          if (pQryCursor == NULL) {
              printf("pQryCursor ERROR");
              return  - 1;
          }
		//测试添加入库  
    while (!RECEIVE_STOP()) {
        if ((iRet=g_poRcvMQ->Receive((void *)&g_oRcvBuf, sizeof(g_oRcvBuf), 0, true)) < 0) {
            if (iRet == -2) {
                continue;   
            }   
            __DEBUG_LOG1_(0, "应用程序从消息队列接收失败, 程序退出! errno=%d", errno);
            return -1;
        }
        //disp_buf((char *)&g_oRcvBuf, sizeof(g_oRcvBuf) + sizeof(long));
        //需要根据命令码来区分是请求包还是应答包
        //iFlag = 
		//lijianchen 20110530
        struDccHead *pHead = (struDccHead *)g_oRcvBuf.m_sDccBuf;
        if (strcmp(g_oNodeConf.m_sOriginHost, pHead->m_sOrignHost) == 0) {
            iFlag = 1;
            pHead->m_iHH = getH2H();
            pHead->m_iEE = getE2E();
        } else {
            iFlag = 0;
        }
        
		if (setDccHead(m_poDcc, &g_oRcvBuf,iFlag,m_sSessionIdCCR,m_sdstrealmCCR) != 0) {
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
				
			case ABMCMD_REVERSE_PAY:          //3.4.2.4  支付冲正
		            iRet = ReversePaySvcA(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "ReversePaySvcA: 组包错误.");
		                continue;
		            }
		            break;
					
            case ABMPAYCMD_INFOR:                       //3.4.2.1 支付
                iRet = packChargeA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packChargeA: DCC打包错误.");
		            continue;
		        }
                break;
            case ABMCMD_QRY_SRV_PSWD:                  //3.4.2.5 二次认证的密码下发
            	iRet = packRandPaswA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packRandPaswA: DCC打包错误.");
		            continue;
		        }
                break;
			case ABMCMD_ALLOCATE_BALANCE:                          //3.4.5.1 余额被动划拨
			   //lijianchen 20110530
			    if (iFlag == 0)
                    iRet = AllocateBalanceSvcA(m_poDcc, &g_oRcvBuf);
                else
                    iRet = packDeductBalR(m_poDcc, &g_oRcvBuf);
                
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "AllocateBalanceSvcA: DCC打包错误.");
		            continue;
		        }
                break;
                
            case ABMCMD_REVERSE_DEDUCT_BALANCE:                    //3.4.5.5
             //lijianchen 20110530
		        if (iFlag == 0)
                    iRet = ReqReverseDeductSvcA(m_poDcc, &g_oRcvBuf);
                else
                    iRet = packRvsChargeR(m_poDcc, &g_oRcvBuf);

                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "ReqReverseDeductSvcA: DCC打包错误.");
		            continue;
		        }
                break;
				
			case ABMCMD_QRY_BIL_INFOR:                            //3.4.6.16 电子账单查询
                iRet = RechargeBillQueryA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "RechargeBillQueryA: DCC打包错误.");
		            continue;
		        }
                break;
			
			case ABMCMD_QRY_PAY_INFOR:                            //3.4.6.15
                iRet = QueryPayLogByAccoutA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "QueryPayLogByAccoutA: DCC打包错误.");
		            continue;
		        }
                break;
			
			case ABMCMD_QRY_REC_INFOR:                            //3.4.6.14
                iRet = RechargeRecQueryA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "RechargeRecQueryA: DCC打包错误.");
		            continue;
		        }
                break;
                
            case ABMCMD_QRY_MICROPAY_INFO:                       //3.4.6.4  余额划拨查询   
                iRet = packQryBalDrawA(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packQryBalDrawA: DCC打包错误.");
		            continue;
		        }
                break;
                
            case ABMCMD_QRY_MICROPAYCCA_INFO:                    //3.4.6.4  余额划拨查询 中心abm主动发起
                iRet = packQryBalDrawR(m_poDcc, &g_oRcvBuf);
                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "packQryBalDrawR: DCC打包错误.");
		            continue;
		        }
                break;
			case ABMCMD_QRY_BAL_INFOR:      //3.4.6.13,  3.4.5.3,  3.4.5.4 余额查询
			 //lijianchen 20110530
			    if (iFlag == 0)
                    iRet = QueryBalanceA(m_poDcc, &g_oRcvBuf);
                else
                    iRet = packQryBalanceR(m_poDcc, &g_oRcvBuf,m_sservNUMCCR);

                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "QueryBalanceR: DCC打包错误.");
		            continue;
		        }
                break; 
				
			case ABMDEPOSITCMD_INFOR:      //3.4.4.1 开户充值 VC规范2.1  张成
		            iRet = DepositServCondOneA(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "DepositServCondOneA：组包错误.");
		                continue;
		            }
		            break;
					
			case ABM_REVERSEEPOSIT_CMD:      //3.4.4.3 充值冲正 VC规范2.3  张成
			        iRet = ReverseDepositA(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "DepositServCondOneA：组包错误.");
		                continue;
		            }
		            break;
			
			case ABMCMD_PLATFORM_DEDUCT_BALANCE:      ////3.4.6.5  全国中心余额划出  zhuyong
			/**
		            iRet = ReqPlatformSvcA(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
					**/
		            break;
					
			case ABMCMD_ACTIVE_ALLOCATE_BALANCE:      //3.4.5.2 余额划拨全国中心余额划出全国中心发送CCR  zhuyong
		            iRet = ReqBalanceSvcR(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
			case ABMCMD_QRY_SRV_SYN:      //3.4.1.2  资料同步
			    if (iFlag == 0)
                    iRet = QryServSynCondA(m_poDcc, &g_oRcvBuf);
                else
                    iRet = QryServSynCondRT(m_poDcc, &g_oRcvBuf);

                if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "QueryBalanceR: DCC打包错误.");
		            continue;
		        }
                break; 
			
			case ABMCMD_PASSWD_IDENTIFY:      //3.4.1.3 密码鉴权 ABM发给HSS
		            iRet = PasswdIdentifyCondRT(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
					
			case ABMCMD_LOGIN_PASSWD:      //3.4.6.1 登陆鉴权 自服平台发给ABM
		            iRet = LoginPasswdCCRA(m_poDcc, &g_oRcvBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
			
            default:
                __DEBUG_LOG1_(0, "未知的命令请求[%d].", g_oRcvBuf.m_iCmdID);
		        continue;
                break;
        }
        //g_sSndBuf.m_lMsgType = g_oRcvBuf.m_lType;
		/***
		if (strcmp(g_oNodeConf.m_sOriginRealm, pHead->m_sDestRealm) == 0)
		{
		   g_sSndBuf.m_lMsgType = 2;
		} else {
		g_sSndBuf.m_lMsgType = 1;
		}
		***/
		if(g_oRcvBuf.m_iCmdID == ABMCMD_QRY_BAL_INFOR){
		 pQryCursor->setSQL("insert into ABM_RESULT "
            "(SESSION_ID,RESULT_DATE,SERV_NUMBER,INFO_FLAG,DEST_REALM) "
            "VALUES (:p0,sysdate,:p1,'R',:p2)");
        pQryCursor->setParameter("p0", m_sSessionIdCCR);
        pQryCursor->setParameter("p1", m_sservNUMCCR);
        pQryCursor->setParameter("p2", m_sdstrealmCCR);
        pQryCursor->execute();
        pQryCursor->close();
        pQryCursor->commit();
		}
		g_sSndBuf.m_lMsgType = SockID;
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
