#include "parserDccMsg.h"



static MessageQueue *g_poRcvMQ = NULL;         //接收消息队列
static MessageQueue *g_poSndMQ = NULL;         //发送消息队列 
static MessageQueue *g_poSndPeerMQ = NULL;     //发送消息队列,发往sock服务器
static MessageQueue *g_poSndCCAMQ  = NULL;     //业务程序主动发送CCR后收到的回包，发往这个消息队列

static ABMCCR g_oSndBuf;                  //发送缓存区，发往业务处理进程
static StruMqMsg   g_sRecvBuf;            //接收缓存区，接收从socket发送过来的消息
static StruMqMsg   g_sPeerBuf;            //发送缓存区，不发往后面的业务处理进程，直接发往socket时用到
//static struDccHead goDccHead;           //dcc消息的公共头部信息
extern NodeConfStru g_oNodeConf;          //业务节点的一些固定配置
//测试添加入库
char m_sSessionIdFD[128];
char m_sOriginHostFD[128];
char m_sDstRealmFD[128];
//ACE_UINT32 U32Var;
ACE_UINT32 m_iresultCodeFD;
ACE_UINT32 m_iSresultCodeFD;
//测试添加入库

int ParserMsgMgr::init(ABMException &oExp)
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
    //发往sock服务器的消息队列
    if (struConf.m_iSndPeerKey <= 0 || struConf.m_iSndCcaKey <=0) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init 解析程序需要配置mqsnd_peer和mqsend_cca.");
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
    //主动请求包对应的回包发往的消息队列
    if ((g_poSndCCAMQ=new MessageQueue(struConf.m_iSndCcaKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init g_poSndPeerMQ malloc failed!");
        return -1;
    }
	if (g_poSndCCAMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT1(oExp, "ParserMsgMgr::init g_poSndPeerMQ->open 消息队列连接失败!", struConf.m_iSndKey);
        return -1;
    }
    
    m_poDcc = new DccMsgParser(struConf.m_sDictFile);
    if (NULL == m_poDcc) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poDcc malloc failed!");
        return -1;
    }
    //20110604
    m_poSql = new DccSql;
    if (NULL == m_poSql) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poSql malloc failed!");
        return -1;
    }
    if (m_poSql->init(oExp) != 0) {
        ADD_EXCEPT0(oExp, "ParserMsgMgr::init m_poSql init failed!");
        return -1;
    }
     //20110604
    return 0;
}

int ParserMsgMgr::run()
{
    int iRecvLen, iRet, iCmdId, iSize, iFlag;
    string sTemp,sProductName;
	char m_sOriginName[30];
	char m_sOriginNameHSS[30]="HSS2@001.ChinaTelecom.com";
    MessageQueue *pMqTmp;
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
       //测试添加入库
        m_iSresultCodeFD=2222;
		m_poDcc->getAvpValue("Session-Id", sTemp);
        snprintf(m_sSessionIdFD, sizeof(m_sSessionIdFD), sTemp.c_str());
		
	    m_poDcc->getAvpValue("Origin-Host", sTemp);
	    snprintf(m_sOriginHostFD, sizeof(m_sOriginHostFD), sTemp.c_str());
		m_poDcc->getAvpValue("Destination-Realm", sTemp);
	    snprintf(m_sDstRealmFD, sizeof(m_sDstRealmFD), sTemp.c_str());
		
		m_poDcc->getAvpValue("Result-Code", m_iresultCodeFD);
		
	    m_poDcc->getAvpValue("Service-Information.Service-Result-Code", m_iSresultCodeFD);
		long m_lSresultCodeFD =m_iSresultCodeFD;
		if((strcmp(m_sOriginHostFD,"ABM@001.ChinaTelecom.com")!= 0 )&&(strcmp(m_sOriginHostFD,"SR1@001.ChinaTelecom.com")!= 0 )){
		//if(strcmp(m_sOriginHostFD,"ABM@001.ChinaTelecom.com")!= 0 ){
		if ((m_iresultCodeFD==2001)&&(m_iSresultCodeFD==0)){
		   pQryCursor->setSQL("insert into ABM_RESULT "
            "(SESSION_ID,ORIGIN_HOST,RESULT_DATE,RESULT_CODE,SERVICE_RESULT_CODE) "
            "VALUES (:p0,:p1,sysdate,0,:p2)");
        pQryCursor->setParameter("p0", m_sSessionIdFD);
        pQryCursor->setParameter("p1", m_sOriginHostFD);
		pQryCursor->setParameter("p2", m_lSresultCodeFD);
        pQryCursor->execute();
        pQryCursor->close();
        pQryCursor->commit();
		}
		else{
		   pQryCursor->setSQL("insert into ABM_RESULT "
            "(SESSION_ID,ORIGIN_HOST,RESULT_DATE,RESULT_CODE,SERVICE_RESULT_CODE) "
            "VALUES (:p0,:p1,sysdate,1,:p2)");
        pQryCursor->setParameter("p0", m_sSessionIdFD);
        pQryCursor->setParameter("p1", m_sOriginHostFD);
		pQryCursor->setParameter("p2", m_lSresultCodeFD);
        pQryCursor->execute();
        pQryCursor->close();
        pQryCursor->commit();
		}
		}
		if (((m_iresultCodeFD!=2001)&&(m_iresultCodeFD!=0))||((m_iSresultCodeFD==2222)&&(strcmp(m_sDstRealmFD,"001.ChinaTelecom.com")!= 0 )&&
		    ((strcmp(m_sOriginHostFD,"ABM@001.ChinaTelecom.com")!= 0 )&&(strcmp(m_sOriginHostFD,"SR1@001.ChinaTelecom.com")!= 0)))){
		    	
		continue;
		}
		//测试添加入库
		
		
		g_sPeerBuf.m_lMsgType = g_sRecvBuf.m_lMsgType;
		g_oSndBuf.m_lType =g_sRecvBuf.m_lMsgType;
		 
		if ( (iRet = m_poDcc->getDiameterCommandCodeInfo()) == COMMAND_CODE_CE ) {    //能力交换
		    if (m_poDcc->getDiameterHeader().flags.r) {  //请求消息
		        iRet = packCEA(m_poDcc, &g_sPeerBuf);
		        if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "打包CEA消息出错.");
		            continue;
		        }        
		    } else {                                     //应答消息
			    m_poDcc->getAvpValue("Origin-Host", sProductName);
				snprintf(m_sOriginName, sizeof(m_sOriginName), sProductName.c_str());
				if(strcmp(m_sOriginName,m_sOriginNameHSS)!=0){
		            __DEBUG_LOG0_(0, "收到能力交换应答包,将发送能力激活包.");
		            iRet = packNetCtrl(m_poDcc, &g_sPeerBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "打包激活请求出错.");
		                continue;
		            }
					if (g_poSndPeerMQ->Send((void *)&g_sPeerBuf, iRet + sizeof(long)) < 0) {
			            __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			         return -1;	
		            }
				}
		    }
			/***
		    if (g_poSndPeerMQ->Send((void *)&g_sPeerBuf, iRet + sizeof(long)) < 0) {
			    __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			    return -1;	
		    }
			***/
		} else if (iRet == COMMAND_CODE_DW) {                                         //设备监控
		    if (m_poDcc->getDiameterHeader().flags.r) {   //请求消息
		        iRet = packDWA(m_poDcc, &g_sPeerBuf);
		        if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "打包DWA消息出错.");
		            continue;
		        }
		    
		        if (g_poSndPeerMQ->Send((void *)&g_sPeerBuf, iRet + sizeof(long)) < 0) {
			        __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			        return -1;	
		        }
		    } else {                                      //应答消息
		        __DEBUG_LOG0_(0, "收到设备监控应答包，忽略.");
		    }
		} else if (iRet == COMMAND_CODE_DP) {                                         //拆除对端
		    if (m_poDcc->getDiameterHeader().flags.r) {  //请求消息
		        iRet = packDPA(m_poDcc, &g_sPeerBuf);
		        if (iRet < 0 ) {
		            __DEBUG_LOG0_(0, "返回DPA消息出错.");
		            continue;
		        }
		    
		        if (g_poSndPeerMQ->Send((void *)&g_sPeerBuf, iRet + sizeof(long)) < 0) {
			        __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			        return -1;	
		        }
		    } else {                                              //应答消息
		        __DEBUG_LOG0_(0, "收到拆除对端应答包，忽略.");
		    }
		} else if (iRet == COMMAND_CODE_CC) {                                         //业务消息
		    //获取请求消息的公共部分，并获得消息类型
		    iFlag = (m_poDcc->getDiameterHeader().flags.r)?1:0;
		    g_oSndBuf.clear();
		    iCmdId = getDccHead(m_poDcc, &g_oSndBuf, iFlag, m_poSql->m_poQrySession);
		    if (iCmdId < 0 ) {
		        __DEBUG_LOG0_(0, "获取dcc消息头部信息出错.");
		        continue;
		    }
		    
		    switch (iCmdId)
		    {
		        case ABMCMD_QRY_SRV_INFOR:          //3.4.1.1 资料查询并开户
		            iRet = parserInfoQryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserInfoQryR: 组包错误.");
		                continue;
		            }
		            break;
					
				case ABMCMD_REVERSE_PAY:          //3.4.2.4  支付冲正
		            iRet = ReversePaySvcR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "ReversePaySvcR: 组包错误.");
		                continue;
		            }
		            break;
		        case ABMPAYCMD_INFOR:               //3.4.2.1 支付
		            iRet = parserChargeR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserChargeR：组包错误.");
		                continue;
		            }
		            break;
				case ABMCMD_QRY_SRV_PSWD:          //3.4.2.5 二次认证的密码下发
					iRet = parserRandPswdR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserRandPswdR：组包错误.");
		                continue;
		            }
		            break;
		        case ABMCMD_ALLOCATE_BALANCE:      //3.4.5.1 余额被动划拨
		           //lijianchen 20110530
				    if (iFlag == 1) {
		                iRet = AllocateBalanceSvcR(m_poDcc, &g_oSndBuf);
		            } else {
		                iRet = parserDeductBalA(m_poDcc, &g_oSndBuf);
		            }
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "AllocateBalanceSvcR. 组包错误.");
		                continue;
		            }
		            break;
				case ABMCMD_REVERSE_DEDUCT_BALANCE:   //3.4.5.5 
			//lijianchen 20110530
				    if (iFlag == 1) {
		                iRet = ReqReverseDeductSvcR(m_poDcc, &g_oSndBuf);
		            } else {
		                iRet = parserRvsChargeA(m_poDcc, &g_oSndBuf);
		            }
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "ReqReverseDeductSvcR. 组包错误.");
		                continue;
		            }
		            break;
		            	
				case ABMCMD_QRY_BIL_INFOR:            //3.4.6.16 电子账单查询
		            iRet = RechargeBillQueryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "RechargeBillQueryR：组包错误.");
		                continue;
		            }
		            break;
					
				case ABMCMD_QRY_PAY_INFOR:            //3.4.6.15 电子账单查询
		            iRet = QueryPayLogByAccoutR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "QueryPayLogByAccoutR：组包错误.");
		                continue;
		            }
		            break;
					
				case ABMCMD_QRY_REC_INFOR:            //3.4.6.14 
		            iRet = RechargeRecQueryR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "RechargeRecQueryR：组包错误.");
		                continue;
		            }
		            break;
		            
		        case ABMCMD_QRY_MICROPAY_INFO:        //3.4.6.4  余额划拨查询  收到外围系统的请求命令
		            iRet = parserQryBalDrawR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawR：组包错误.");
		                continue;
		            }
		            break;
		            
		        case ABMCMD_QRY_MICROPAYCCA_INFO:      //3.4.6.4  余额划拨查询  外围系统对abm中心主动请求CCR的回包
		             iRet = parserQryBalDrawA(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
				case ABMCMD_QRY_BAL_INFOR:      //3.4.6.13,  3.4.5.3,  3.4.5.4 余额查询
		       //lijianchen  20220530
			        if (iFlag == 1) {
		                iRet = QueryBalanceR(m_poDcc, &g_oSndBuf);
		            } else {
		                iRet = parserQryBalanceA(m_poDcc, &g_oSndBuf);
		            }
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
				case ABMDEPOSITCMD_INFOR:      //3.4.4.1 开户充值 VC规范2.1  张成
		             iRet = DepositServCondOneR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
				case ABM_REVERSEEPOSIT_CMD:      //3.4.4.3 充值冲正 VC规范2.3  张成
		             iRet = ReverseDepositR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
					
				case ABMCMD_PLATFORM_DEDUCT_BALANCE:      ////3.4.6.5  全国中心余额划出  zhuyong
		             iRet = ReqPlatformSvcR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
					
				case ABMCMD_ACTIVE_ALLOCATE_BALANCE:      //3.4.5.2 余额划拨  收全国中心CCAzhuyong
		             iRet = ReqBalanceSvcA(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;
		        case ABMCMD_QRY_SRV_SYN:      //3.4.1.2 资料同步
			        if (iFlag == 1) {
		                iRet = QryServSynCondR(m_poDcc, &g_oSndBuf);
		            } else {
		                iRet = QryServSynCondAT(m_poDcc, &g_oSndBuf);
		            }
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break; 
				case ABMCMD_PASSWD_IDENTIFY:      //3.4.1.3 密码鉴权 ABM发给HSS
		             iRet = PasswdIdentifyCondAT(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "parserQryBalDrawA：组包错误.");
		                continue;
		            }
		            break;	
				case ABMCMD_LOGIN_PASSWD:      //3.4.6.1 登陆鉴权 自服平台发给ABM
		             iRet = LoginPasswdCCRR(m_poDcc, &g_oSndBuf);
		            if (iRet < 0 ) {
		                __DEBUG_LOG0_(0, "LoginPasswdCCRR：组包错误.");
		                continue;
		            }
		            break;	
		        default:
		            __DEBUG_LOG1_(0, "未知的命令请求[%d].", iCmdId);
		            continue;
		            break;
            }
#ifdef _DEBUG_DCC_
            cout<<"parserDccMsg send packet:"<<endl;
            cout<<"command_id--"<<g_oSndBuf.m_iCmdID<<endl;
            cout<<"size--"<<g_oSndBuf.m_iSize<<endl;
            //cout<<"first number--"<<g_oSndBuf.m_iFirNum<<endl;
            //cout<<"second number--"<<g_oSndBuf.m_iSecNum<<endl;
            //cout<<"flags--"<<g_oSndBuf.m_iFlag<<endl;
#endif
            //根据不同不同消息类型放到不同的消息队列
            /*
            if (iFlag == 1)            //判断是CCR还是CCA
                pMqTmp = g_poSndMQ;    //请求包发往正常的消息队列
            else
                pMqTmp = g_poSndCCAMQ; //主动请求的回包发往这个特殊的消息队列
            */
            
            //根据不同不同消息类型放到不同的消息队列
            switch (iCmdId)
            {
                case ABMCMD_QRY_MICROPAY_INFO:       //3.4.6.4  余额划拨查询  收到外围系统的请求命令
                case ABMCMD_QRY_MICROPAYCCA_INFO:    //3.4.6.4  余额划拨查询  外围系统对abm中心主动请求CCR的回包
                    pMqTmp = g_poSndMQ; 
                    break;
                default:
                    pMqTmp = g_poSndMQ;
                    break;
            }
            
		    if (pMqTmp->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
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
	__FREE_PTR(g_poSndCCAMQ);
	
	return 0;
}
