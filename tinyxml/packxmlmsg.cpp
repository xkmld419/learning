#include "packxmlmsg.h"

#define MSG_SIZE 70096
using namespace std;
struct StruMqMsg {
    long m_lMsgType;
    char m_sMsg[MSG_SIZE];
};

static MessageQueue *g_poRcvMQ = NULL;      //接收消息队列
static MessageQueue *g_poSndMQ = NULL;      //发送消息队列

static ABMCCA    g_oRcvBuf;                    //发送缓存区，发往业务处理进程
static StruMqMsg g_sSndBuf;                    //发送缓存区，发往socket

int PackXMLMsg::init(ABMException &oExp)
{
    char *p;
    
    if (g_iProcID == -1) {
		ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
		return -1;	
	}
	
	//读取配置文件
	XmlConfStru struConf;
	if (XmlConf::getXmlConf(g_iProcID, struConf, oExp) != 0) {
		ADD_EXCEPT1(oExp, "PackXMLMsg::init 失败, 请检查PackXMLMsg -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
		return -1;		
	}
	
	//消息队列初始化
	if ((g_poRcvMQ=new MessageQueue(struConf.m_iRcvKey)) == NULL) {
        ADD_EXCEPT0(oExp, "PackXMLMsg::init g_poRcvMQ malloc failed!");
        return -1;
    }
	if (g_poRcvMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "PackXMLMsg::init g_poRcvMQ->open 消息队列连接失败!");
        return -1;
    }
	if ((g_poSndMQ=new MessageQueue(struConf.m_iSndKey)) == NULL) {
        ADD_EXCEPT0(oExp, "PackXMLMsg::init g_poSndMQ malloc failed!");
        return -1;
    }
	if (g_poSndMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "PackXMLMsg::init g_poSndMQ->open 消息队列连接失败!");
        return -1;
    }
    
    //snprintf(m_sXmlFile, sizeof(m_sXmlFile), "%log/pack_%d.xml" ,getHomeDir(), getpid());
    if ((m_poXmlDoc = new TiXmlDocument()) == NULL) {
        ADD_EXCEPT0(oExp, "PackXMLMsg::init m_poXmlDoc malloc failed!");
        return -1;
    }
    
    return 0;
}

int PackXMLMsg::destroy()
{
	//释放业务处理类
    #define __FREE_PTR(p) \
        if (p != NULL) { \
            delete p; p = NULL; }
    
    __FREE_PTR(g_poSndMQ);
    __FREE_PTR(g_poRcvMQ);
	
	return 0;
}

int PackXMLMsg::run()
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
        
        //g_sSndBuf.m_lMsgType = g_oRcvBuf.m_lType;
		g_sSndBuf.m_lMsgType = 1;
        
        //添加xml声明
        TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "gbk", "" ); 
        m_poXmlDoc->LinkEndChild(decl);
        
		__DEBUG_LOG1_(0, "接受程序的 m_iCmdID=%d", g_oRcvBuf.m_iCmdID);
		
        switch (g_oRcvBuf.m_iCmdID)
        {
			case ABMCMD_LOGIN_PASSWD:	//3.4.6.1 登录鉴权
				iRet = packLoginA();
				break;
            case ABMPAYCMD_INFOR:  //3.4.6.2 支付
                iRet = packChargeA();
				break;
				
			case ABMCMD_BINDINGACCNBR_QRY:  //3.4.6.3 绑定手机号码查询
                iRet = packBindNbrA();
				break;
				
			case ABMCMD_QRY_MICROPAY_INFO:  //3.4.6.4 余额划拨查询
			    iRet = packQryBalA();
				break;
			
			case ABMCMD_PLATFORM_DEDUCT_BALANCE:   //ABMCMD_PLATFORM_QUEUE
			    iRet = packTransBalA(); //3.4.6.5 余额划拨
				break;
				
			case ABMCMD_BAL_TRANS_QRY:   
			    iRet = packQryTransRuleA(); //3.4.6.6 余额划拨规则查询
				break;
			
			case ABMCMD_TRANS_RULES_RESET:   
			    iRet = packResetTransRuleA(); //7 3.4.6.7 余额划拨规则设置
				break;
			
			case ABMCMD_PASSWORD_RESET:   
			    iRet = packResetPassWdA(); //3.4.6.8 支付密码修改
				break;
				
				
			case ABMCMD_PAY_ABILITY_QRY:   
			    iRet = packQryPayStatusA(); //3.4.6.9 支付能力状态查询
				break;
				
			case ABMCMD_PAYSTATUS_RESET:   
			    iRet = packResetPayStatusA(); //3.4.6.10 支付状态变更
				break;
				
			case ABMCMD_PAYRULE_QRY:   
			    iRet = packQryPayRulesA(); //3.4.6.11 支付规则查询
				break;

			case ABMCMD_PAYRULE_RESET:   
			    iRet = packResetPayRulesA(); //3.4.6.12 支付规则设置
				break;

			case ABMCMD_QRY_BAL_INFOR: //3.4.6.13 余额查询
			    iRet = packQueryA();
				break;
				
			case ABMCMD_QRY_REC_INFOR:   
			    iRet = packRechargeRecA(); //3.4.6.14 充退记录查询
				break;
				
			case ABMCMD_QRY_PAY_INFOR:   
			    iRet = packQryPayLogA(); //3.4.6.15 交易记录查询
				break;
				
			case ABMCMD_QRY_BIL_INFOR:   //ABMCMD_PLATFORM_QUEUE
			    iRet = packQryRechargeBillA(); //3.4.6.16 电子帐单查询
				break;
            default:
                break;
        }
        
        if (iRet != 0) {
            m_poXmlDoc->Clear();
            __DEBUG_LOG1_(0, "消息打包过程出错，命令号：%d. ", g_oRcvBuf.m_iCmdID);
            continue;
        }
        

		//输出到内存并清除xml中的内容
        TiXmlPrinter printer;
		printer.SetIndent( "" );
        printer.SetLineBreak( "" );
        m_poXmlDoc->Accept( &printer );
        snprintf(g_sSndBuf.m_sMsg+4, MSG_SIZE-4, printer.CStr());
        iMsgLen = printer.Size() + 4;
        //头4个字节包含的信息和dcc消息一样
        *(int *)g_sSndBuf.m_sMsg = htonl(iMsgLen);
        g_sSndBuf.m_sMsg[0] = 0x01;
        //
        m_poXmlDoc->Clear();
		
        
#ifdef _DEBUG_TI_XML_
        cout<<"CCA消息发送前原始消息包：\n"<<g_sSndBuf.m_sMsg<<endl;
#endif
          
        if (g_poSndMQ->Send((void *)&g_sSndBuf, iMsgLen+sizeof(long)) < 0) {
	        __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			return -1;	
		} 
		
		__DEBUG_LOG1_(0, "******************%d", 11111);
		
    }
    
    __DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
    
    return 0;
}
    
	
//3.4.6.1 登录鉴权
int PackXMLMsg::packLoginA()
{
    int iRet;
    char sTmp[256];
    vector<LoginPasswdData *> v1;
    LoginPasswdCCA *pUnPack = (LoginPasswdCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getLoginPasswdData(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_hRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_sResID)); 
	root->LinkEndChild(pRltCode);
	
	long2date(v1[0]->m_iResTime,sTmp);
	pRltCode = new TiXmlElement("Response_Time");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	__DEBUG_LOG1_(0, "m_hRltCode:[%d]",v1[0]->m_hRltCode);
	__DEBUG_LOG1_(0, "m_sResID:[%d]",v1[0]->m_sResID);
	__DEBUG_LOG1_(0, "m_iResTime:[%d]",v1[0]->m_iResTime);
	
	
	return 0;
}
	
//3.4.6.2 支付
int PackXMLMsg::packChargeA()
{
    int iRet;
    char sTmp[256];
    vector<ResponsPayOne *> v1;
    vector<ResponsPayTwo *> v2;
    AbmPaymentCCA *pUnPack = (AbmPaymentCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getRespons(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getPayData(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_iRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	__DEBUG_LOG1_(0, "m_iRltCode:[%d]",v1[0]->m_iRltCode);
	
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("Micropayment_Info");
	    root->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElem = new TiXmlElement("Operate_Seq");
	    pElem->LinkEndChild( new TiXmlText(v2[i]->m_sOperateSeq) );
	    pPayInfo->LinkEndChild(pElem);
	    
	    pElem = new TiXmlElement("Req_Serial");
	    pElem->LinkEndChild( new TiXmlText(v2[i]->m_sReqSerial) );
	    pPayInfo->LinkEndChild(pElem);
		
		__DEBUG_LOG1_(0, "m_sOperateSeq:[%s]",v2[i]->m_sOperateSeq);
		__DEBUG_LOG1_(0, "m_sReqSerial:[%s]",v2[i]->m_sReqSerial);
	}
	
	return 0;
}


//3.4.6.3 绑定手机号码查询
int PackXMLMsg::packBindNbrA()
{
    int iRet;
    char sTmp[256];
    vector<A_BindingAccNbrQuery *> v1;

    BindingAccNbrQueryCCA *pUnPack = (BindingAccNbrQueryCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getA_BindingAccNbrQuery(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Random_AccNbr");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_randomAccNbr)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);
	__DEBUG_LOG1_(0, "m_randomAccNbr:[%s]",v1[0]->m_randomAccNbr);
	
	return 0;
}


//3.4.6.4 余额划拨查询
int PackXMLMsg::packQryBalA()
{
    int iRet;
    char sTmp[256];
    vector<MicroPayRlt *> v1;
    vector<MicroPaySrv *> v2;
    HssResMicroPayCCA *pUnPack = (HssResMicroPayCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getRlt(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getSrv(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_sResID)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	long2date(v1[0]->m_uiResTime,sTmp);
	pRltCode = new TiXmlElement("Response_Time");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiResTime);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_sResID:[%s]",v1[0]->m_sResID);
    __DEBUG_LOG1_(0, "m_uiRltCode:[%d]",v1[0]->m_uiRltCode);
	__DEBUG_LOG1_(0, "m_uiResTime:[%d]",v1[0]->m_uiResTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Balance_Information");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("Balance_Item_Detail");
	    pPayInfoB->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElem = new TiXmlElement("Balance_Source");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iBalSrc);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
	    
	    pElem = new TiXmlElement("UnitType_Id");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iUnitTypID);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
	   
	    pElem = new TiXmlElement("Balance_Amount");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalAmount);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);


		__DEBUG_LOG1_(0, "m_iBalSrc:[%d]",v2[i]->m_iBalSrc);
		__DEBUG_LOG1_(0, "m_iUnitTypID:[%d]",v2[i]->m_iUnitTypID);
		__DEBUG_LOG1_(0, "m_lBalAmount:[%d]",v2[i]->m_lBalAmount);

	}
	
	return 0;
}


//3.4.6.5 余额划拨
int PackXMLMsg::packTransBalA()
{
    int iRet;
    char sTmp[256];
    vector<ResPlatformSvc *> v1;
    vector<ResPlatformAcctBalance *> v2;
    PlatformDeductBalanceCCA *pUnPack = (PlatformDeductBalanceCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getResPlatformSvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getResPlatformAcctBalance(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_responseTime);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_iResponseTime);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	TiXmlElement * pElem = new TiXmlElement("UnitType_Id");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_iUnitTypeId);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Balance_Amount");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_lDeductAmount);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);

	__DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);
	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
	__DEBUG_LOG1_(0, "m_iUnitTypeId:[%d]",v1[0]->m_iUnitTypeId);
	__DEBUG_LOG1_(0, "m_lDeductAmount:[%d]",v1[0]->m_lDeductAmount);
	
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("Balance_Item_Detail");
	    pPayInfoB->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElemT = new TiXmlElement("Balance_Source");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iBalanceSource);
	    pElemT->LinkEndChild( new TiXmlText(sTmp) );
	    pPayInfo->LinkEndChild(pElemT);
	    
	    pElemT = new TiXmlElement("Balance_Amount");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalanceAmount);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		__DEBUG_LOG1_(0, "m_iBalanceSource:[%d]",v2[i]->m_iBalanceSource);
		__DEBUG_LOG1_(0, "m_lBalanceAmount:[%d]",v2[i]->m_lBalanceAmount);
	}
	
	return 0;
}

//3.4.6.6 余额划拨规则查询
int PackXMLMsg::packQryTransRuleA()
{
    int iRet;
    char sTmp[256];
    vector<A_BalanceTransSvc *> v1;
    vector<A_BTMicropaymentInfo *> v2;
    BalanceTransCCA *pUnPack = (BalanceTransCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getResBalanceTransSvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getResBTMicropaymentInfo(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElem = new TiXmlElement("Date_Trans_Freq");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_dateTransFreq);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Trans_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_transLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Month_Trans_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_monthTransLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Trans_Flag");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autotransflag);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Balance_limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autobalancelimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Amount_limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autoAmountlimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);

		__DEBUG_LOG1_(0, "m_dateTransFreq:[%s]",v2[i]->m_dateTransFreq);
		__DEBUG_LOG1_(0, "m_transLimit:[%s]",v2[i]->m_transLimit);
		__DEBUG_LOG1_(0, "m_monthTransLimit:[%s]",v2[i]->m_monthTransLimit);
		__DEBUG_LOG1_(0, "m_autotransflag:[%s]",v2[i]->m_autotransflag);
		__DEBUG_LOG1_(0, "m_autobalancelimit:[%s]",v2[i]->m_autobalancelimit);
		__DEBUG_LOG1_(0, "m_autoAmountlimit:[%s]",v2[i]->m_autoAmountlimit);

	}
	
	return 0;
}


	//3.4.6.7 余额划拨规则设置
int PackXMLMsg::packResetTransRuleA()
{
    int iRet;
    char sTmp[256];
    vector<A_BalanceTransResetSvc *> v1;
    vector<A_TRMicropaymentInfo *> v2;
    BalanceTransResetCCA *pUnPack = (BalanceTransResetCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getResBalanceTransResetSvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getResRTMicropaymentInfo(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElem = new TiXmlElement("Date_Trans_Freq");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_dateTransFreq);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		
		pElem = new TiXmlElement("Trans_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_transLimit);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Month_Trans_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_monthTransLimit);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Trans_Flag");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autotransflag);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Balance_limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autobalancelimit);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Auto_Amount_limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_autoAmountlimit);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		__DEBUG_LOG1_(0, "m_dateTransFreq:[%d]",v2[i]->m_dateTransFreq);
		__DEBUG_LOG1_(0, "m_transLimit:[%d]",v2[i]->m_transLimit);
		__DEBUG_LOG1_(0, "m_monthTransLimit:[%d]",v2[i]->m_monthTransLimit);
		__DEBUG_LOG1_(0, "m_autotransflag:[%d]",v2[i]->m_autotransflag);
		__DEBUG_LOG1_(0, "m_autobalancelimit:[%d]",v2[i]->m_autobalancelimit);
		__DEBUG_LOG1_(0, "m_autoAmountlimit:[%d]",v2[i]->m_autoAmountlimit);
	}
	
	return 0;
}




//3.4.6.8 支付密码修改
int PackXMLMsg::packResetPassWdA()
{
    int iRet;
    char sTmp[256];
    vector<A_PasswordReset *> v1;

    PasswordResetCCA *pUnPack = (PasswordResetCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getA_PasswordReset(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);
	
	return 0;
}

	
	//3.4.6.9 支付能力状态查询
int PackXMLMsg::packQryPayStatusA()
{
    int iRet;
    char sTmp[256];
    vector<A_PayAbilityQuerySvc *> v1;
    vector<A_MicropaymentInfo *> v2;
    PayAbilityQueryCCA *pUnPack = (PayAbilityQueryCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getResPayAbilityQuerySvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getResMicropaymentInfo(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElem = new TiXmlElement("Pay_Status");
	    pElem->LinkEndChild( new TiXmlText(v2[i]->m_PayStatus)); 
	    pPayInfoB->LinkEndChild(pElem);
		__DEBUG_LOG1_(0, "m_PayStatus:[%s]",v2[i]->m_PayStatus);

	}
	
	return 0;
}

 //3.4.6.10 支付状态变更
int PackXMLMsg::packResetPayStatusA()
{
   int iRet;
    char sTmp[256];
    vector<A_PayStatusReset *> v1;

    PayStatusResetCCA *pUnPack = (PayStatusResetCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getA_PayStatusReset(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Pay_Status");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_payStatus)); 
	root->LinkEndChild(pRltCode);


	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);
	__DEBUG_LOG1_(0, "m_payStatus:[%s]",v1[0]->m_payStatus);
	
	return 0;
}

	//3.4.6.11 支付规则查询
int PackXMLMsg::packQryPayRulesA()
{
    int iRet;
    char sTmp[256];
    vector<A_PayRulesQuerySvc *> v1;
    vector<A_PRQMicropaymentInfo *> v2;
    PayRulesQueryCCA *pUnPack = (PayRulesQueryCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getA_PayRulesQuerySvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getA_PRQMicropaymentInfo(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);

	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElem = new TiXmlElement("Date_Pay_Freq");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_datePayFreq);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_payLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Month_Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_monthPayLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Micro_Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_microPayLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);	
	
		__DEBUG_LOG1_(0, "m_datePayFreq:[%d]",v2[i]->m_datePayFreq);
		__DEBUG_LOG1_(0, "m_payLimit:[%d]",v2[i]->m_payLimit);
		__DEBUG_LOG1_(0, "m_monthPayLimit:[%d]",v2[i]->m_monthPayLimit);
		__DEBUG_LOG1_(0, "m_microPayLimit:[%d]",v2[i]->m_microPayLimit);

	}
	
	return 0;
}


	//3.4.6.12 支付规则设置
int PackXMLMsg::packResetPayRulesA()
{
    int iRet;
    char sTmp[256];
    vector<A_PayRulesResetSvc *> v1;
    vector<A_PRRMicropaymentInfo *> v2;
    PayRulesResetCCA *pUnPack = (PayRulesResetCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getA_PayRulesResetSvc(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getA_PRRMicropaymentInfo(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sResID);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseId)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_svcResultCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_responseTime)); 
	root->LinkEndChild(pRltCode);

	__DEBUG_LOG1_(0, "m_responseId:[%s]",v1[0]->m_responseId);
    __DEBUG_LOG1_(0, "m_svcResultCode:[%d]",v1[0]->m_svcResultCode);
	__DEBUG_LOG1_(0, "m_responseTime:[%s]",v1[0]->m_responseTime);

	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElem = new TiXmlElement("Date_Pay_Freq");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_datePayFreq);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_payLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Month_Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_monthPayLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Micro_Pay_Limit");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_microPayLimit);
	    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElem);	
	
		__DEBUG_LOG1_(0, "m_datePayFreq:[%d]",v2[i]->m_datePayFreq);
		__DEBUG_LOG1_(0, "m_payLimit:[%d]",v2[i]->m_payLimit);
		__DEBUG_LOG1_(0, "m_monthPayLimit:[%d]",v2[i]->m_monthPayLimit);
		__DEBUG_LOG1_(0, "m_microPayLimit:[%d]",v2[i]->m_microPayLimit);

	}
	
	return 0;
}

//3.4.6.13 余额查询
int PackXMLMsg::packQueryA()
{
    int iRet;
    char sTmp[256];
    vector<ResBalanceCode *> v1;
    vector<ResQueryBalance *> v2;
    HssQueryBalanceCCA *pUnPack = (HssQueryBalanceCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getResPons(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getResBal(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_lTotalBalAvail);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_sResID)); 
	root->LinkEndChild(pRltCode);	
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Total_Balance_Available");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_lTotalBalAvail);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);	
	
	
	__DEBUG_LOG1_(0, "m_uiRltCode:[%d]",v1[0]->m_uiRltCode);
	__DEBUG_LOG1_(0, "m_lTotalBalAvail:[%d]",v1[0]->m_lTotalBalAvail);
	__DEBUG_LOG1_(0, "m_lTotalBalAvail:[%s]",v1[0]->m_sResID);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Balance_Information");
	root->LinkEndChild(pPayInfoB);
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("Balance_Item_Detail");
	    pPayInfoB->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElem = new TiXmlElement("Balance_ItemType_Detail");
	    pElem->LinkEndChild( new TiXmlText(v2[i]->m_sBalItmDetail) );
	    pPayInfo->LinkEndChild(pElem);
	    
	    pElem = new TiXmlElement("UnitType_Id");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iUnitTypID);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
	   
	    pElem = new TiXmlElement("Balance_Amount");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalAmount);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
	
		pElem = new TiXmlElement("Balance_Available");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalAvailAble);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Balance_Owe_Used");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalUsed);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Balance_Used");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalUsed);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
		
		pElem = new TiXmlElement("Balance_Reserved");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lBalReserved);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
		
		long2date(v2[i]->m_uiEffDate,sTmp);
		pElem = new TiXmlElement("Eff_Date");
	    //snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiEffDate);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
		
		long2date(v2[i]->m_uiExpDate,sTmp);
		pElem = new TiXmlElement("Exp_Date");
	    //snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiExpDate);
		pElem->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElem);
	
		__DEBUG_LOG1_(0, "m_sBalItmDetail:[%s]",v2[i]->m_sBalItmDetail);
		__DEBUG_LOG1_(0, "m_iUnitTypID:[%d]",v2[i]->m_iUnitTypID);
		__DEBUG_LOG1_(0, "m_lBalAmount:[%d]",v2[i]->m_lBalAmount);
		__DEBUG_LOG1_(0, "m_lBalAvailAble:[%d]",v2[i]->m_lBalAvailAble);
		__DEBUG_LOG1_(0, "m_lBalUsed:[%d]",v2[i]->m_lBalUsed);
		__DEBUG_LOG1_(0, "m_lBalReserved:[%d]",v2[i]->m_lBalReserved);
		__DEBUG_LOG1_(0, "m_uiEffDate:[%d]",v2[i]->m_uiEffDate);
		__DEBUG_LOG1_(0, "m_uiExpDate:[%d]",v2[i]->m_uiExpDate);
		

		
	}
	
	return 0;
}



//3.4.6.14 充退记录查询
int PackXMLMsg::packRechargeRecA()
{
    int iRet;
    char sTmp[256];
    vector<ResRecCode *> v1;
    vector<ResRechargeRecQuery *> v2;
    HssRechargeRecQueryCCA *pUnPack = (HssRechargeRecQueryCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getRespons(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getRec(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	TiXmlElement * pElem = new TiXmlElement("Operate_Seq");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sSeq);
	pElem->LinkEndChild( new TiXmlText(v1[0]->m_sSeq)); 
	pPayInfoB->LinkEndChild(pElem);
	
	__DEBUG_LOG1_(0, "m_sSeq:[%s]",v1[0]->m_sSeq);
	
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("FillRecord_Info");
	    pPayInfoB->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElemT = new TiXmlElement("Result");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiResult);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
	    
	    pElemT = new TiXmlElement("Operate_Action");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiOperAct);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("RechargeTime");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sRechargeTime);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sRechargeTime)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("Destination_Account");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sDestAcct);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sDestAcct)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("Destination_Attr");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iDestAttr);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("Service_Platform_ID");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sSrvPlatID);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sSrvPlatID)); 
	    pPayInfo->LinkEndChild(pElemT);	
		
		pElemT = new TiXmlElement("Charge_Type");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiChargeType);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);	

		pElemT = new TiXmlElement("RechargeAmount");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiRechargeAmount);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);	
		
		__DEBUG_LOG1_(0, "m_uiResult:[%d]",v2[i]->m_uiResult);
		__DEBUG_LOG1_(0, "m_uiOperAct:[%d]",v2[i]->m_uiOperAct);
		__DEBUG_LOG1_(0, "m_sRechargeTime:[%s]",v2[i]->m_sRechargeTime);
		__DEBUG_LOG1_(0, "m_sDestAcct:[%s]",v2[i]->m_sDestAcct);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%s]",v2[i]->m_iDestAttr);
		__DEBUG_LOG1_(0, "m_sSrvPlatID:[%s]",v2[i]->m_sSrvPlatID);
		__DEBUG_LOG1_(0, "m_uiChargeType:[%d]",v2[i]->m_uiChargeType);		
		__DEBUG_LOG1_(0, "m_uiRechargeAmount:[%d]",v2[i]->m_uiRechargeAmount);		
		
	}
	
	return 0;
}


//3.4.6.15 交易记录查询
int PackXMLMsg::packQryPayLogA()
{
    int iRet;
    char sTmp[256];
    vector<ResPayCode *> v1;
    vector<ResQueryPayLogByAccout *> v2;
    HssQueryPayLogByAccoutCCA *pUnPack = (HssQueryPayLogByAccoutCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getRespons(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getPay(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Operate_Seq");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sSeq);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_sSeq)); 
	root->LinkEndChild(pRltCode);
	
	__DEBUG_LOG1_(0, "m_uiRltCode:[%d]",v1[0]->m_uiRltCode);
	__DEBUG_LOG1_(0, "m_sSeq:[%s]",v1[0]->m_sSeq);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	for (int i=0; i<v2.size(); i++) {
	    TiXmlElement * pPayInfo = new TiXmlElement("Order_Info");
	    pPayInfoB->LinkEndChild(pPayInfo);
	    
	    TiXmlElement * pElemT = new TiXmlElement("Order_ID");
	    //snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sOderID);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sOderID)); 
	    pPayInfo->LinkEndChild(pElemT);
	    
	    pElemT = new TiXmlElement("Description");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sDesc);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sDesc)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("SP_ID");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sRechargeTime);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sSPID)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("Service_Platform_ID");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sDestAcct);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sSrvPlatID)); 
	    pPayInfo->LinkEndChild(pElemT);
		
		
		long2date(v2[i]->m_uiPayDate,sTmp);
		pElemT = new TiXmlElement("Pay_Date");
	    //snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiEffDate);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
		/**
		pElemT = new TiXmlElement("Pay_Date");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_uiPayDate);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);
		**/
		pElemT = new TiXmlElement("Status");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sSrvPlatID);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sStatus)); 
	    pPayInfo->LinkEndChild(pElemT);	
		
		pElemT = new TiXmlElement("Micropay_Type");
		//snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_sMicropayType);
		pElemT->LinkEndChild( new TiXmlText(v2[i]->m_sMicropayType)); 
	    pPayInfo->LinkEndChild(pElemT);	

		pElemT = new TiXmlElement("PayAmount");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lPayAmount);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfo->LinkEndChild(pElemT);	
		
		__DEBUG_LOG1_(0, "m_sOderID:[%s]",v2[i]->m_sOderID);
		__DEBUG_LOG1_(0, "m_sDesc:[%s]",v2[i]->m_sDesc);
		__DEBUG_LOG1_(0, "m_sSPID:[%s]",v2[i]->m_sSPID);
		__DEBUG_LOG1_(0, "m_sSrvPlatID:[%s]",v2[i]->m_sSrvPlatID);
		__DEBUG_LOG1_(0, "m_uiPayDate:[%d]",v2[i]->m_uiPayDate);
		__DEBUG_LOG1_(0, "m_sStatus:[%s]",v2[i]->m_sStatus);
		__DEBUG_LOG1_(0, "m_sMicropayType:[%s]",v2[i]->m_sMicropayType);
		__DEBUG_LOG1_(0, "m_lPayAmount:[%d]",v2[i]->m_lPayAmount);
		
	}
	
	return 0;
}

//3.4.6.16 电子帐单查询
int PackXMLMsg::packQryRechargeBillA()
{
    int iRet;
    char sTmp[256];
    vector<ResBilCode *> v1;
    vector<ResRechargeBillQuery *> v2;
    HssRechargeBillQueryCCA *pUnPack = (HssRechargeBillQueryCCA *)&g_oRcvBuf;
    
    iRet = pUnPack->getRespons(v1);
    if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pUnPack->getBil(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Result_Code");
	snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_uiRltCode);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Operate_Seq");
	//snprintf(sTmp, sizeof(sTmp), "%d", v1[0]->m_sSeq);
	pRltCode->LinkEndChild( new TiXmlText(v1[0]->m_sSeq)); 
	root->LinkEndChild(pRltCode);
	
	__DEBUG_LOG1_(0, "m_uiRltCode:[%d]",v1[0]->m_uiRltCode);
	__DEBUG_LOG1_(0, "m_sSeq:[%s]",v1[0]->m_sSeq);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	for (int i=0; i<v2.size(); i++) {	    
	    TiXmlElement * pElemT = new TiXmlElement("PayAmount");
	    snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_lPayMount);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElemT);
	    
	    pElemT = new TiXmlElement("Balance");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iBalance);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElemT);
		
		pElemT = new TiXmlElement("Last_Balance");
		snprintf(sTmp, sizeof(sTmp), "%d", v2[i]->m_iLastBalance);
		pElemT->LinkEndChild( new TiXmlText(sTmp)); 
	    pPayInfoB->LinkEndChild(pElemT);	

		__DEBUG_LOG1_(0, "m_lPayMount:[%d]",v2[i]->m_lPayMount);
		__DEBUG_LOG1_(0, "m_iBalance:[%d]",v2[i]->m_iBalance);
		__DEBUG_LOG1_(0, "m_iLastBalance:[%d]",v2[i]->m_iLastBalance);		
	}
	
	return 0;
}



