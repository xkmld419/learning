#include "parserxmlmsg.h"

#define MSG_SIZE 4096
#define CCAMSGID  100  //自服务心跳
#define CCRMSGID  200  //HSS交互id
#define C_CCRMSGID  300 //省交互id
struct StruMqMsg {
    long m_lMsgType;
    char m_sMsg[MSG_SIZE];
};

static MessageQueue *g_poRcvMQ = NULL;      //接收消息队列
static MessageQueue *g_poSndMQ = NULL;      //发送消息队列
static MessageQueue *g_poSndPeerMQ = NULL;  //发送到sock的消息队列
static MessageQueue *g_poSndCCAMQ = NULL;  //发送到sock的心跳消息队列

static ABMCCR    g_oSndBuf;                 //发送缓存区，发往业务处理进程
static StruMqMsg g_oRcvBuf;                 //接收缓存区，接收从socket发送过来的消息
static StruMqMsg g_oPeerBuf;                //发送缓存区，不发往后面的业务处理进程，直接发往socket时用到

int MSGid = C_CCRMSGID;

int ParserXMLMsg::init(ABMException &oExp)
{
    char *p;
    
    if (g_iProcID == -1) {
		ADD_EXCEPT0(oExp, "启动参数中必须设置 -p 项!");
		return -1;	
	}
	
	//读取配置文件
	XmlConfStru struConf;
	if (XmlConf::getXmlConf(g_iProcID, struConf, oExp) != 0) {
		ADD_EXCEPT1(oExp, "ParserXMLMsg::init 失败, 请检查ParserXMLMsg -p %d 在abm_app_cfg.ini的配置!", g_iProcID);
		return -1;		
	}
	
	//消息队列初始化
	if ((g_poRcvMQ=new MessageQueue(struConf.m_iRcvKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poRcvMQ malloc failed!");
        return -1;
    }
	if (g_poRcvMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poRcvMQ->open 消息队列连接失败!");
        return -1;
    }
	if ((g_poSndMQ=new MessageQueue(struConf.m_iSndKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndMQ malloc failed!");
        return -1;
    }
	if (g_poSndMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndMQ->open 消息队列连接失败!");
        return -1;
    }
    //发往给省里面交互的消息队列
    if (struConf.m_iSndPeerKey <= 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init 解析程序需要配置mqsnd_peer.");
        return -1;
    }
    if ((g_poSndPeerMQ=new MessageQueue(struConf.m_iSndPeerKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndPeerMQ malloc failed!");
        return -1;
    }
	if (g_poSndPeerMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndPeerMQ->open 消息队列连接失败!");
        return -1;
    }
	
    //回自服务平台心跳包交互的消息队列
    if (struConf.m_iSndCCAKey <= 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init 解析程序需要配置mqsnd_peer.");
        return -1;
    }
    if ((g_poSndCCAMQ=new MessageQueue(struConf.m_iSndCCAKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndCCAMQ malloc failed!");
        return -1;
    }
	if (g_poSndCCAMQ->open(oExp, true, true) != 0) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init g_poSndCCAMQ->open 消息队列连接失败!");
        return -1;
    }
    
    snprintf(m_sXmlFile, sizeof(m_sXmlFile), "%log/parser_%d.xml" ,getHomeDir(), getpid());
    if ((m_poXmlDoc = new TiXmlDocument()) == NULL) {
        ADD_EXCEPT0(oExp, "ParserXMLMsg::init m_poXmlDoc malloc failed!");
        return -1;
    }
    
    return 0;
}


int ParserXMLMsg::selfDccHead( ABMCCR *pCCR)
{
    string sTemp;
    int iCmdId, iPos;
    struDccHead struhead;
    char sSvcCtxID[72];
    
	//char sTmpName[110];
	time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
    snprintf(struhead.m_sSessionId,sizeof(struhead.m_sSessionId),"%s%010d%s%04d\0","ZFP@001.ChinaTelecom.com;",tTime,";",iSeq);
    snprintf(struhead.m_sOrignHost,sizeof(struhead.m_sOrignHost),"%s%\0","ZFP");    
	snprintf(struhead.m_sOrignRealm,sizeof(struhead.m_sOrignRealm),"%s%\0","001.ChinaTelecom.com");
    struhead.m_iAuthAppId=4;
    snprintf(struhead.m_sSrvFlowId,sizeof(struhead.m_sSrvFlowId),"%s%\0","ZFPtest;");
    snprintf(struhead.m_sOutPlatform, sizeof(struhead.m_sOutPlatform),"%s%\0","ZFPOutPlatform;");
    struhead.m_iReqType = 4;
	struhead.m_iReqNumber = 4;
	snprintf(struhead.m_sDestRealm,sizeof(struhead.m_sDestRealm),"%s%\0","001.ChinaTelecom.com");
    
#ifdef _DEBUG_DCC_
    struhead.print();
#endif

    //复制到CCR结构的公共头部
    memcpy((void *)pCCR->m_sDccBuf, (void *)&struhead, sizeof(struDccHead));
    
    //消息的命令码

    return 0;
}

int ParserXMLMsg::destroy()
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

int ParserXMLMsg::run()
{
    int iRecvLen, iRet;
    
    while (!RECEIVE_STOP()) {
        //should clear XMLDocument before loop
        m_poXmlDoc->Clear();
        g_oSndBuf.clear();
        
        if ((iRecvLen=g_poRcvMQ->Receive((void *)&g_oRcvBuf, MSG_SIZE, 0, true)) < 0) {
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
		
		g_oSndBuf.m_lType = g_oRcvBuf.m_lMsgType;
		g_oPeerBuf.m_lMsgType = g_oRcvBuf.m_lMsgType;
		
#ifdef _DEBUG_TI_XML_
        __DEBUG_LOG1_(0, "接收到的请求包：\n%s\n", g_oRcvBuf.m_sMsg);
#endif	

        //parse xml message
        m_poXmlDoc->Parse(g_oRcvBuf.m_sMsg + 4);
		cout<<g_oRcvBuf.m_sMsg<<endl;
        if (m_poXmlDoc->Error()) {
            __DEBUG_LOG1_(0, "Parse error,error message:\n***%s\n", m_poXmlDoc->ErrorDesc());  
            continue;
        }
        
        iRet = toStruct();
        if (iRet < 0) {          // iRet<0 函数调用出错;iRet>0 接收包的内容不符合规范;
            __DEBUG_LOG0_(0, "toStruct error.");
            continue;
        }
		int iHead = selfDccHead(&g_oSndBuf);
		    if (iHead < 0 ) {
		        __DEBUG_LOG0_(0, "获取dcc消息头部信息出错.");
		        continue;
		    }

		if(MSGid == CCRMSGID){
			if (g_poSndMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
				__DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
				return -1;	
			}
		}

		if(MSGid == C_CCRMSGID)
		{
			if (g_poSndPeerMQ->Send((void *)&g_oSndBuf, g_oSndBuf.m_iSize) < 0) {
			__DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			return -1;	
			} 
		}
		MSGid = C_CCRMSGID;
	}
	
	__DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
	
	return 0;
}


int ParserXMLMsg::toStruct()
{
    TiXmlHandle hDocHandle(m_poXmlDoc);
    TiXmlHandle hSrvInfo(0);
    TiXmlElement* pElem;
    const char *pSrvCtxId;
    int iRet;
    
    hSrvInfo = hDocHandle.FirstChildElement("Service_Information");
    if (NULL == hSrvInfo.Element()) {
        __DEBUG_LOG0_(0, "node <Service_Information> not found.");
        return -1;
    }
	
    const char *pSrvReqId;
	pElem = hSrvInfo.FirstChildElement("Request_Id").Element();
    pSrvReqId = pElem->GetText();
	
    pElem = hSrvInfo.FirstChildElement("Service_Context_Id").Element();
    if (NULL == pElem) {
        __DEBUG_LOG0_(0, "node <Service-Context-Id> not found.");
        return -1;
    }
    pSrvCtxId = pElem->GetText();
    
    

#ifdef _DEBUG_TI_XML_
        __DEBUG_LOG1_(0, "Service_Context_Id：%s", pSrvCtxId);
#endif
    if (strcmp(pSrvCtxId, "breath") == 0)   //心跳包
	{
	   iRet = parseReady(hSrvInfo);  
	   MSGid = CCAMSGID;
	}
	else if (strcmp(pSrvCtxId, "Login") == 0)                //3.4.6.1 登录鉴权
    {          
        iRet = parseLoginR(hSrvInfo);     
    } 
    else if (strcmp(pSrvCtxId, "Charge") == 0)          //3.4.6.2 支付
    {  
        iRet = parseChargeR(hSrvInfo);   
    } 
    else if (strcmp(pSrvCtxId, "binding_AccNbr") == 0)  //3.4.6.3 绑定手机号码查询  
    {    
        iRet = parseBindNbrR(hSrvInfo);  
    } 
    else if (strcmp(pSrvCtxId, "Balance_Query") == 0)   //3.4.6.4 余额划拨查询
    {   
        iRet = parseQryBalR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "Balance_Trans") == 0)   //3.4.6.5 余额划拨
    {
        iRet = parseTransBalR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "TransRules_Query") == 0) //3.4.6.6 余额划拨规则查询
    {
        iRet = parseQryTransRuleR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "TransRules_Reset") == 0) //3.4.6.7 余额划拨规则设置
    {
        iRet = parseResetTransRuleR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "Password_Reset") == 0)  //3.4.6.8 支付密码修改
    {
        iRet = parseResetPassWdR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "PayStatus_Query") == 0)  //3.4.6.9 支付能力状态查询
    {
        iRet = parseQryPayStatusR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "PayStatus_Reset") == 0)  //3.4.6.10 支付状态变更
    {
        iRet = parseResetPayStatusR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "PayRules_Query") == 0)  //3.4.6.11 支付规则查询
    {
        iRet = parseQryPayRulesR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "PayRules_Reset") == 0)  //3.4.6.12 支付规则设置
    {
        iRet = parseResetPayRulesR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "Query") == 0)           //3.4.6.13 余额查询
    {
        iRet = parseQueryR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "RechargeRecQuery") == 0)//3.4.6.14 充退记录查询
    {
        iRet = parseRechargeRecR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "QueryPayLogByAccout") == 0)//3.4.6.15 交易记录查询
    {
        iRet = parseQryPayLogR(hSrvInfo);
    }
    else if (strcmp(pSrvCtxId, "RechargeBillQuery") == 0)  //3.4.6.16 电子帐单查询
    {
        iRet = parseQryRechargeBillR(hSrvInfo);
    }
    else 
    {
        //should pack a error-code for answer-packet before return
        __DEBUG_LOG1_(0, "Service-Context-Id：<%s>, is not valid.", pSrvCtxId);
        return 1;
    }
    
    return iRet;
}


//心跳
int ParserXMLMsg::parseReady(TiXmlHandle &SrvInfoHandle)
{
    ABMException oExp;
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(m_sReqID, sizeof(m_sReqID), pElem->GetText());
	
	static StruMqMsg g_sSndBufR;                    //发送缓存区，发往socket
	*g_sSndBufR.m_sMsg=' ';
	g_sSndBufR.m_lMsgType = 1;
     
	m_poXmlDoc->Clear();
    //添加xml声明
    TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "gbk", "" ); 
    m_poXmlDoc->LinkEndChild(decl);
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("context");
	pRltCode->LinkEndChild( new TiXmlText("breath") );
	root->LinkEndChild(pRltCode);           
    
    pRltCode = new TiXmlElement("Response_Id");
	pRltCode->LinkEndChild( new TiXmlText(m_sReqID) );
	root->LinkEndChild(pRltCode);	
    int iMsgLen;
	//输出到内存并清除xml中的内容
    TiXmlPrinter printer;
	printer.SetIndent( "" );
    printer.SetLineBreak( "" );
    m_poXmlDoc->Accept( &printer );
    snprintf(g_sSndBufR.m_sMsg+4, MSG_SIZE-4, printer.CStr());
    iMsgLen = printer.Size() + 4;
    //头4个字节包含的信息和dcc消息一样
    *(int *)g_sSndBufR.m_sMsg = htonl(iMsgLen);
    //g_sSndBufR.m_sMsg[0] = 0x01;
       
#ifdef _DEBUG_TI_XML_
    cout<<"CCA消息发送前原始消息包：\n"<<g_sSndBufR.m_sMsg<<endl;
#endif

/***
    MessageQueue *pMQS = new MessageQueue(struConf.m_iSndCCAKey); 
	if (pMQS->open(oExp, true, true) != 0) {
       cout<<"pMQS->open failed!"<<endl;
       return -1;
    }
		***/	
   // __DEBUG_LOG1_(0, "发送心跳包:[%d]" MSGid);		
    if (g_poSndCCAMQ->Send((void *)&g_sSndBufR, iMsgLen+sizeof(long)) < 0) {
	     __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
		return -1;	
	} 
	    m_poXmlDoc->Clear();

    return 0;
}
/***
//3.4.6.1 登录鉴权
int ParserXMLMsg::parseLoginR(TiXmlHandle &SrvInfoHandle)
{
    ABMException oExp;
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
	char sTmp[256];
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(m_sReqID, sizeof(m_sReqID), pElem->GetText());
	
	static StruMqMsg g_sSndBufR;                    //发送缓存区，发往socket
	*g_sSndBufR.m_sMsg=' ';
	g_sSndBufR.m_lMsgType = 1;
     
	m_poXmlDoc->Clear();
    //添加xml声明
    TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "gbk", "" ); 
    m_poXmlDoc->LinkEndChild(decl);
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Response_Id");
	pRltCode->LinkEndChild( new TiXmlText(m_sReqID) );
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_Result_Code");
	pRltCode->LinkEndChild( new TiXmlText("0") );
	root->LinkEndChild(pRltCode);

	pRltCode = new TiXmlElement("Service_Result");
	pRltCode->LinkEndChild( new TiXmlText("yyyy") );
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Service_id");
	pRltCode->LinkEndChild( new TiXmlText("xxxx") );
	root->LinkEndChild(pRltCode);

	pRltCode = new TiXmlElement("Response_Time");
	pRltCode->LinkEndChild( new TiXmlText("20111063032121") );
	root->LinkEndChild(pRltCode);
          
    int iMsgLen;
	//输出到内存并清除xml中的内容
    TiXmlPrinter printer;
	printer.SetIndent( "" );
    printer.SetLineBreak( "" );
    m_poXmlDoc->Accept( &printer );
    snprintf(g_sSndBufR.m_sMsg+4, MSG_SIZE-4, printer.CStr());
    iMsgLen = printer.Size() + 4;
    //头4个字节包含的信息和dcc消息一样
    *(int *)g_sSndBufR.m_sMsg = htonl(iMsgLen);
    //g_sSndBufR.m_sMsg[0] = 0x01;
       
#ifdef _DEBUG_TI_XML_
    cout<<"CCA消息发送前原始消息包：\n"<<g_sSndBufR.m_sMsg<<endl;
#endif
    MessageQueue *pMQS = new MessageQueue(10009); 
	if (pMQS->open(oExp, true, true) != 0) {
       cout<<"pMQS->open failed!"<<endl;
       return -1;
    }
		
    if (pMQS->Send((void *)&g_sSndBufR, iMsgLen+sizeof(long)) < 0) {
	     __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
		return -1;	
	} 
	    m_poXmlDoc->Clear();
    return 0;
}
***/


//3.4.6.1 登陆鉴权
int ParserXMLMsg::parseLoginR(TiXmlHandle &SrvInfoHandle)
{
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
    LoginPasswdStruct oCond;
	LoginPasswdCond oCondT;
    
    /////////////不同的命令，不同的设置///////////
    LoginPasswdCCR *pPack = (LoginPasswdCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_LOGIN_PASSWD;
    //////////////////////////////////////////////
    
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
	
    pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_sReqID, sizeof(oCond.m_sReqID), pElem->GetText());
    
    pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
	snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
    
	if (!pPack->addLoginPasswdRespons(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	
        memset((void *)&oCondT, 0, sizeof(oCondT));
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();    
        //遍历<Micropayment_Info>下的所有节点
		 struct LoginPasswdCond
 {
 	char m_sDestAccount[UTF8_MAX];	//用户号码
 	int m_hDestType;	//标识类型
 	int m_hDestAttr;	//用户属性
 	char m_sServPlatID[UTF8_MAX];	//业务平台标识
 	char m_sLoginPasswd[UTF8_MAX];//登陆密码
 };
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_sDestAccount, sizeof(oCondT.m_sDestAccount), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_hDestType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_hDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
                snprintf(oCondT.m_sServPlatID, sizeof(oCondT.m_sServPlatID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
               snprintf(oCondT.m_sLoginPasswd, sizeof(oCondT.m_sLoginPasswd), pTmpElem->GetText());
                continue;
            }
        }
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_sReqID:[%s]",oCond.m_sReqID);
		__DEBUG_LOG1_(0, "m_sDestAccount:[%s]",oCondT.m_sDestAccount);
		__DEBUG_LOG1_(0, "m_hDestType:[%d]",oCondT.m_hDestType);
		__DEBUG_LOG1_(0, "m_hDestAttr:[%d]",oCondT.m_hDestAttr);
		__DEBUG_LOG1_(0, "m_sServPlatID:[%s]",oCondT.m_sServPlatID);
		__DEBUG_LOG1_(0, "m_sLoginPasswd:[%s]",oCondT.m_sLoginPasswd);

        if (!pPack->addLoginPasswd(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
    MSGid = CCRMSGID;
    return 0;
}


//3.4.6.2 支付
int ParserXMLMsg::parseChargeR(TiXmlHandle &SrvInfoHandle)
{
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
    AbmPayCond oCond;
    
    /////////////不同的命令，不同的设置///////////
    AbmPaymentCCR *pPack = (AbmPaymentCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMPAYCMD_INFOR;
    //////////////////////////////////////////////
    
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCond, 0, sizeof(oCond));
        
        //遍历<Micropayment_Info>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Operate_Action") == 0){
                oCond.m_iOperateAction = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Operate_Seq") == 0){
                snprintf(oCond.m_sOperateSeq, sizeof(oCond.m_sOperateSeq), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "SP_ID") == 0){
                snprintf(oCond.m_sSpID, sizeof(oCond.m_sSpID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
                snprintf(oCond.m_sServicePlatformID, sizeof(oCond.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }

            if (strcmp(pTmpElem->Value(), "Staff_ID") == 0){
                oCond.m_iStaffID = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Micropay_Type") == 0){
                snprintf(oCond.m_sMicropayType, sizeof(oCond.m_sMicropayType), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCond.m_sDestinationAccount, sizeof(oCond.m_sDestinationAccount), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                snprintf(oCond.m_sDestinationtype, sizeof(oCond.m_sDestinationtype), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCond.m_iDestinationAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
                snprintf(oCond.m_sPayPassword, sizeof(oCond.m_sPayPassword), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Random_Password") == 0){
                snprintf(oCond.m_sRandomPassword, sizeof(oCond.m_sRandomPassword), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Pay_Date") == 0){
				snprintf(oCond.m_sPayDate, sizeof(oCond.m_sPayDate), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Order_ID") == 0){
                snprintf(oCond.m_sOrderID, sizeof(oCond.m_sOrderID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Description") == 0){
                snprintf(oCond.m_sDescription, sizeof(oCond.m_sDescription), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "PayAmount") == 0){
                sscanf(pTmpElem->GetText(), "%ld", &oCond.m_lPayAmount);
                continue;
            }
        }
        __DEBUG_LOG1_(0, "m_iOperateAction:[%d]",oCond.m_iOperateAction);
		__DEBUG_LOG1_(0, "m_sOperateSeq:[%s]",oCond.m_sOperateSeq);
		__DEBUG_LOG1_(0, "m_sSpID:[%s]",oCond.m_sSpID);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCond.m_sServicePlatformID);
		__DEBUG_LOG1_(0, "m_iStaffID:[%d]",oCond.m_iStaffID);
		__DEBUG_LOG1_(0, "m_sMicropayType:[%s]",oCond.m_sMicropayType);
		__DEBUG_LOG1_(0, "m_sDestinationAccount:[%s]",oCond.m_sDestinationAccount);
		__DEBUG_LOG1_(0, "m_sDestinationtype:[%s]",oCond.m_sDestinationtype);
		__DEBUG_LOG1_(0, "m_iDestinationAttr:[%d]",oCond.m_iDestinationAttr);
		__DEBUG_LOG1_(0, "m_sPayPassword:[%s]",oCond.m_sPayPassword);
		__DEBUG_LOG1_(0, "m_sRandomPassword:[%s]",oCond.m_sRandomPassword);
		__DEBUG_LOG1_(0, "m_sPayDate:[%s]",oCond.m_sPayDate);
		__DEBUG_LOG1_(0, "m_sOrderID:[%s]",oCond.m_sOrderID);
		__DEBUG_LOG1_(0, "m_sDescription:[%s]",oCond.m_sDescription);
		__DEBUG_LOG1_(0, "m_lPayAmount:[%d]",oCond.m_lPayAmount);
        if (!pPack->addPayInfo(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
    
    return 0;
}


//3.4.6.3 绑定手机号码查询  
int ParserXMLMsg::parseBindNbrR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_BindingAccNbrQuery oCond;
	R_BDAMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    BindingAccNbrQueryCCR *pPack = (BindingAccNbrQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_BINDINGACCNBR_QRY;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addR_BindingAccNbrQuery(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
        
        if (!pPack->addR_BDAMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}



//3.4.6.4 余额划拨查询
int ParserXMLMsg::parseQryBalR(TiXmlHandle &SrvInfoHandle)
{
    
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
    QueryAllPay oCond;
    
    /////////////不同的命令，不同的设置///////////
    HssPaymentQueryCCR *pPack = (HssPaymentQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_QRY_MICROPAY_INFO;
    //////////////////////////////////////////////
	memset((void *)&oCond, 0, sizeof(oCond));
	
    pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_sReqID, sizeof(oCond.m_sReqID), pElem->GetText());
    
    pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    oCond.m_uiReqTime = atoi(pElem->GetText());
    
    pElem = SrvInfoHandle.FirstChildElement("Balance_Information").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        //遍历<Micropayment_Info>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCond.m_sDestAcct, sizeof(oCond.m_sDestAcct), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCond.m_iDestType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCond.m_iDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
                snprintf(oCond.m_sServPlatID, sizeof(oCond.m_sServPlatID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Query_Flag") == 0){
                oCond.m_iQueryFlg = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Query_Item_Type") == 0){
                oCond.m_iQueryItemType = atoi(pTmpElem->GetText());
                continue;
            }
        }

        __DEBUG_LOG1_(0, "m_sReqID:[%s]",oCond.m_sReqID);
        __DEBUG_LOG1_(0, "m_uiReqTime:[%d]",oCond.m_uiReqTime);
		__DEBUG_LOG1_(0, "m_sDestAcct:[%s]",oCond.m_sDestAcct);
		__DEBUG_LOG1_(0, "m_iDestType:[%d]",oCond.m_iDestType);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%d]",oCond.m_iDestAttr);
		__DEBUG_LOG1_(0, "m_sServPlatID:[%s]",oCond.m_sServPlatID);
		__DEBUG_LOG1_(0, "m_iQueryFlg:[%d]",oCond.m_iQueryFlg);
		__DEBUG_LOG1_(0, "m_iQueryItemType:[%d]",oCond.m_iQueryItemType);
		
        if (!pPack->addReq(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}   

//3.4.6.4 余额划拨
int ParserXMLMsg::parseTransBalR(TiXmlHandle &SrvInfoHandle)
{
    
    TiXmlElement* pElem;
    TiXmlElement* pTmpElem;
    ReqPlatformSvc oCond;
    ReqPlatformPayInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PlatformDeductBalanceCCR *pPack = (PlatformDeductBalanceCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PLATFORM_DEDUCT_BALANCE;
    //////////////////////////////////////////////
    pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
    
    pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
    
    if (!pPack->addReqPlatformSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addAllocateBalanceSvc error.");
	        return -1;
	    }
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
        
        //遍历<Micropayment_Info>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_sDestinationAccount, sizeof(oCondT.m_sDestinationAccount), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_iDestinationType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_iDestinationAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
                snprintf(oCondT.m_sSvcPlatformId, sizeof(oCondT.m_sSvcPlatformId), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
                snprintf(oCondT.m_sPayPassword, sizeof(oCondT.m_sPayPassword), pTmpElem->GetText());
                continue;
            }
            
            if (strcmp(pTmpElem->Value(), "Balance_TransFlag") == 0){
                oCondT.m_iBalanceTransFlag = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "UnitType_Id") == 0){
                oCondT.m_iUnitTypeId = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Deduct_Amount") == 0){
                oCondT.m_lDeductAmount = atoi(pTmpElem->GetText());
                continue;
            }
        }
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_sDestinationAccount:[%s]",oCondT.m_sDestinationAccount);
		__DEBUG_LOG1_(0, "m_iDestinationType：:[%d]",oCondT.m_iDestinationType);
		__DEBUG_LOG1_(0, "m_iDestinationAttr：:[%d]",oCondT.m_iDestinationAttr);
		__DEBUG_LOG1_(0, "m_sSvcPlatformId：:[%s]",oCondT.m_sSvcPlatformId);
		__DEBUG_LOG1_(0, "m_sPayPassword:[%s]",oCondT.m_sPayPassword);
		__DEBUG_LOG1_(0, "m_iBalanceTransFlag:[%d]",oCondT.m_iBalanceTransFlag);
		__DEBUG_LOG1_(0, "m_iUnitTypeId:[%d]",oCondT.m_iUnitTypeId);
		__DEBUG_LOG1_(0, "m_lDeductAmount:[%d]",oCondT.m_lDeductAmount);
 
        if (!pPack->addReqPlatformPayInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}    
    MSGid = C_CCRMSGID;    
   
    return 0;
} 

//3.4.6.6 余额划拨规则查询
int ParserXMLMsg::parseQryTransRuleR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_BalanceTransSvc oCond;
	R_BTMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    BalanceTransCCR *pPack = (BalanceTransCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_BAL_TRANS_QRY;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addBalanceTransSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
        
        if (!pPack->addBTMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}


//3.4.6.7 余额划拨规则设置
int ParserXMLMsg::parseResetTransRuleR(TiXmlHandle &SrvInfoHandle)
{     
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_BalanceTransResetSvc oCond;
	R_TRMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    BalanceTransResetCCR *pPack = (BalanceTransResetCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_TRANS_RULES_RESET;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addBalanceTransResetSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
			    snprintf(oCondT.m_payPassword, sizeof(oCondT.m_payPassword), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Date_Trans_Freq") == 0){
			    oCondT.m_dateTransFreq = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Trans_Limit") == 0){
			    oCondT.m_transLimit = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Month_Trans_Limit") == 0){
			    oCondT.m_monthTransLimit = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Auto_Trans_Flag") == 0){
			    oCondT.m_autotransflag = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Auto_Balance_limit") == 0){
			    oCondT.m_autobalancelimit = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Auto_Amount_limit") == 0){
			    oCondT.m_autoAmountlimit = atoi(pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
		__DEBUG_LOG1_(0, "m_payPassword:[%s]",oCondT.m_payPassword);
		__DEBUG_LOG1_(0, "m_dateTransFreq:[%d]",oCondT.m_dateTransFreq);
		__DEBUG_LOG1_(0, "m_transLimit:[%d]",oCondT.m_transLimit);
		__DEBUG_LOG1_(0, "m_monthTransLimit:[%d]",oCondT.m_monthTransLimit);
		__DEBUG_LOG1_(0, "m_autotransflag:[%d]",oCondT.m_autotransflag);
		__DEBUG_LOG1_(0, "m_autobalancelimit:[%d]",oCondT.m_autobalancelimit);
		__DEBUG_LOG1_(0, "m_autoAmountlimit:[%d]",oCondT.m_autoAmountlimit);
        
        if (!pPack->addTRMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}


//3.4.6.8 支付密码修改
int ParserXMLMsg::parseResetPassWdR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_PasswordReset oCond;
	R_PRMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PasswordResetCCR *pPack = (PasswordResetCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PASSWORD_RESET;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addR_PasswordReset(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Old_Password") == 0){
			    snprintf(oCondT.m_oldPassword, sizeof(oCondT.m_oldPassword), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "New_Password") == 0){
			    snprintf(oCondT.m_newPassword, sizeof(oCondT.m_newPassword), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Action_id") == 0){
			    snprintf(oCondT.m_actionId, sizeof(oCondT.m_actionId), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
		__DEBUG_LOG1_(0, "m_oldPassword:[%s]",oCondT.m_oldPassword);
		__DEBUG_LOG1_(0, "m_newPassword:[%s]",oCondT.m_newPassword);
		__DEBUG_LOG1_(0, "m_actionId:[%s]",oCondT.m_actionId);
        
        if (!pPack->addR_PRMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}
//3.4.6.9 支付能力状态查询
int ParserXMLMsg::parseQryPayStatusR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_PayAbilityQuerySvc oCond;
	R_MicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PayAbilityQueryCCR *pPack = (PayAbilityQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PAY_ABILITY_QRY;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addPayAbilityQuerySvc(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
        
        if (!pPack->addMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}


//3.4.6.10 支付状态变更
int ParserXMLMsg::parseResetPayStatusR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_PayStatusReset oCond;
	R_PSRMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PayStatusResetCCR *pPack = (PayStatusResetCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PAYSTATUS_RESET;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addR_PayStatusReset(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Pay_Status") == 0){
			    snprintf(oCondT.m_payStatus, sizeof(oCondT.m_payStatus), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
			    snprintf(oCondT.m_payPassword, sizeof(oCondT.m_payPassword), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
		__DEBUG_LOG1_(0, "m_payStatus:[%s]",oCondT.m_payStatus);
		__DEBUG_LOG1_(0, "m_payPassword:[%s]",oCondT.m_payPassword);
        
        if (!pPack->addR_PSRMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}


//3.4.6.11 支付规则查询
int ParserXMLMsg::parseQryPayRulesR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_PayRulesQuerySvc oCond;
	R_PRQMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PayRulesQueryCCR *pPack = (PayRulesQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PAYRULE_QRY;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addR_PayRulesQuerySvc(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
        
        if (!pPack->addR_PRQMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}

//3.4.6.12 支付规则设置
int ParserXMLMsg::parseResetPayRulesR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    R_PayRulesResetSvc oCond;
	R_PRRMicropaymentInfo oCondT;
    /////////////不同的命令，不同的设置///////////
    PayRulesResetCCR *pPack = (PayRulesResetCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_PAYRULE_RESET;
	
	char m_sReqID[UTF8_MAX];
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), pElem->GetText());
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Time").Element();
    snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), pElem->GetText());
	
	if (!pPack->addR_PayRulesResetSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	}
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_servNbr, sizeof(oCondT.m_servNbr), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_nbrType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_servAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
			    snprintf(oCondT.m_sServicePlatformID, sizeof(oCondT.m_sServicePlatformID), pTmpElem->GetText());
                continue;
            }
			 if (strcmp(pTmpElem->Value(), "Pay_Password") == 0){
			    snprintf(oCondT.m_payPassword, sizeof(oCondT.m_payPassword), pTmpElem->GetText());
                continue;
            }
			
			if (strcmp(pTmpElem->Value(), "Date_Pay_Freq") == 0){
                oCondT.m_datePayFreq = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Pay_Limit") == 0){
                oCondT.m_payLimit = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Month_Pay_Limit") == 0){
                oCondT.m_monthPayLimit = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Micro_Pay_Limit") == 0){
                oCondT.m_microPayLimit = atoi(pTmpElem->GetText());
                continue;
            }
			
	
        }
		
		__DEBUG_LOG1_(0, "m_requestId:[%s]",oCond.m_requestId);
        __DEBUG_LOG1_(0, "m_requestTime:[%s]",oCond.m_requestTime);
		__DEBUG_LOG1_(0, "m_servNbr:[%s]",oCondT.m_servNbr);
		__DEBUG_LOG1_(0, "m_nbrType:[%d]",oCondT.m_nbrType);
		__DEBUG_LOG1_(0, "m_servAttr:[%d]",oCondT.m_servAttr);
		__DEBUG_LOG1_(0, "m_sServicePlatformID:[%s]",oCondT.m_sServicePlatformID);
		__DEBUG_LOG1_(0, "m_payPassword:[%s]",oCondT.m_payPassword);
		__DEBUG_LOG1_(0, "m_datePayFreq:[%d]",oCondT.m_datePayFreq);
		__DEBUG_LOG1_(0, "m_payLimit:[%d]",oCondT.m_payLimit);
		__DEBUG_LOG1_(0, "m_monthPayLimit:[%d]",oCondT.m_monthPayLimit);
		__DEBUG_LOG1_(0, "m_microPayLimit:[%d]",oCondT.m_microPayLimit);
		
        
        if (!pPack->addR_PRRMicropaymentInfo(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}


//3.4.6.13 余额查询
int ParserXMLMsg::parseQueryR(TiXmlHandle &SrvInfoHandle)
{ 
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    QueryBalance oCondT;
    /////////////不同的命令，不同的设置///////////
    HssQueryBalanceCCR *pPack = (HssQueryBalanceCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_QRY_BAL_INFOR;
	
	//char m_sReqID[UTF8_MAX];
	memset((void *)&oCondT, 0, sizeof(oCondT));
	
	pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    snprintf(oCondT.m_sReqID, sizeof(oCondT.m_sReqID), pElem->GetText());
 
    pElem = SrvInfoHandle.FirstChildElement("Balance_Information").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Destination_Id") == 0){
                snprintf(oCondT.m_sDestID, sizeof(oCondT.m_sDestID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Id_Type") == 0){
                oCondT.m_iDestIDType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_iDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "AreaCode") == 0){
                snprintf(oCondT.m_sAreaCode, sizeof(oCondT.m_sAreaCode), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Query_Flag") == 0){
               oCondT.m_iQueryFlg = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Query_Item_Type") == 0){
                oCondT.m_iQueryItemTyp = atoi(pTmpElem->GetText());
                continue;
            }
        }

	    __DEBUG_LOG1_(0, "m_sReqID:[%s]",oCondT.m_sReqID);
        __DEBUG_LOG1_(0, "m_sDestID:[%s]",oCondT.m_sDestID);
		__DEBUG_LOG1_(0, "m_iDestIDType:[%d]",oCondT.m_iDestIDType);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%d]",oCondT.m_iDestAttr);
		__DEBUG_LOG1_(0, "m_sAreaCode:[%s]",oCondT.m_sAreaCode);
		__DEBUG_LOG1_(0, "m_iQueryFlg:[%d]",oCondT.m_iQueryFlg);
		__DEBUG_LOG1_(0, "m_iQueryItemTyp:[%d]",oCondT.m_iQueryItemTyp);

        if (!pPack->addReqBal(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}

//3.4.6.14 充退记录查询
int ParserXMLMsg::parseRechargeRecR(TiXmlHandle &SrvInfoHandle)
{ 
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    RechargeRecQuery oCondT;
    /////////////不同的命令，不同的设置///////////
    HssRechargeRecQueryCCR *pPack = (HssRechargeRecQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_QRY_REC_INFOR;
	
	char m_sReqID[UTF8_MAX];
	
	//pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    //snprintf(m_sReqID, sizeof(m_sReqID), pElem->GetText());
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Operate_Seq") == 0){
                snprintf(oCondT.m_sOperSeq, sizeof(oCondT.m_sOperSeq), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_sDestAcct, sizeof(oCondT.m_sDestAcct), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_iDestType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_iDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
               snprintf(oCondT.m_sSrvPlatID, sizeof(oCondT.m_sSrvPlatID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Query_Start_Time") == 0){
                oCondT.m_uiStartTime = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Query_Expiration_Time") == 0){
                oCondT.m_uiExpTime = atoi(pTmpElem->GetText());
                continue;
            }
        }
		__DEBUG_LOG1_(0, "m_sOperSeq:[%s]",oCondT.m_sOperSeq);
        __DEBUG_LOG1_(0, "m_sDestAcct:[%s]",oCondT.m_sDestAcct);
		__DEBUG_LOG1_(0, "m_iDestType:[%d]",oCondT.m_iDestType);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%d]",oCondT.m_iDestAttr);
		__DEBUG_LOG1_(0, "m_sSrvPlatID:[%s]",oCondT.m_sSrvPlatID);
		__DEBUG_LOG1_(0, "m_uiStartTime:[%d]",oCondT.m_uiStartTime);
		__DEBUG_LOG1_(0, "m_uiExpTime:[%d]",oCondT.m_uiExpTime);
        
        if (!pPack->addRec(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }       
    
    return 0;
}

//3.4.6.15 交易记录查询
int ParserXMLMsg::parseQryPayLogR(TiXmlHandle &SrvInfoHandle)
{
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    QueryPayLogByAccout oCondT;
    /////////////不同的命令，不同的设置///////////
    HssQueryPayLogByAccoutCCR *pPack = (HssQueryPayLogByAccoutCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_QRY_PAY_INFOR;
	
	char m_sReqID[UTF8_MAX];
	
	//pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    //snprintf(m_sReqID, sizeof(m_sReqID), pElem->GetText());
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();        
    memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Operate_Action") == 0){
                oCondT.m_uiOperAct = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Operate_Seq") == 0){
                snprintf(oCondT.m_sOperSeq, sizeof(oCondT.m_sOperSeq), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "SP_ID") == 0){
                snprintf(oCondT.m_sSPID, sizeof(oCondT.m_sSPID), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_sDestAcct, sizeof(oCondT.m_sDestAcct), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
               oCondT.m_iDestType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_iDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Service_Platform_ID") == 0){
                snprintf(oCondT.m_sSrvPlatID, sizeof(oCondT.m_sSrvPlatID), pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Query_Start_Time") == 0){
                oCondT.m_uiStartTime = atoi(pTmpElem->GetText());
                continue;
            }
			if (strcmp(pTmpElem->Value(), "Query_Expiration_Time") == 0){
                oCondT.m_uiExpTime = atoi(pTmpElem->GetText());
                continue;
            }
        }
		__DEBUG_LOG1_(0, "m_sOperSeq:[%s]",oCondT.m_sOperSeq);
        __DEBUG_LOG1_(0, "m_sDestAcct:[%s]",oCondT.m_sDestAcct);
		__DEBUG_LOG1_(0, "m_sSPID:[%s]",oCondT.m_sSPID);
		__DEBUG_LOG1_(0, "m_iDestType:[%d]",oCondT.m_iDestType);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%d]",oCondT.m_iDestAttr);
		__DEBUG_LOG1_(0, "m_sSrvPlatID:[%s]",oCondT.m_sSrvPlatID);
		__DEBUG_LOG1_(0, "m_uiStartTime:[%d]",oCondT.m_uiStartTime);
		__DEBUG_LOG1_(0, "m_uiExpTime:[%d]",oCondT.m_uiExpTime);
        
        if (!pPack->addPay(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
    return 0;
}

//3.4.6.16 电子帐单查询
int ParserXMLMsg::parseQryRechargeBillR(TiXmlHandle &SrvInfoHandle)
{
      
    TiXmlElement* pElem;
	TiXmlElement* pElemT;
    TiXmlElement* pTmpElem;
    RechargeBillQuery oCondT;
    /////////////不同的命令，不同的设置///////////
    HssRechargeBillQueryCCR *pPack = (HssRechargeBillQueryCCR *)&g_oSndBuf;
    g_oSndBuf.m_iCmdID = ABMCMD_QRY_BIL_INFOR;
	
	char m_sReqID[UTF8_MAX];
	
	//pElem = SrvInfoHandle.FirstChildElement("Request_Id").Element();
    //snprintf(m_sReqID, sizeof(m_sReqID), pElem->GetText());
 
    pElem = SrvInfoHandle.FirstChildElement("Micropayment_Info").Element();
    //for (pElem; pElem; pElem=pElem->NextSiblingElement()) {
        
        memset((void *)&oCondT, 0, sizeof(oCondT));
    
        //遍历<Balance_Information>下的所有节点
        for (pTmpElem = pElem->FirstChildElement(); pTmpElem; pTmpElem=pTmpElem->NextSiblingElement()) {
            if (strcmp(pTmpElem->Value(), "Operate_Seq") == 0){
                snprintf(oCondT.m_sOperSeq, sizeof(oCondT.m_sOperSeq), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Account") == 0){
                snprintf(oCondT.m_sDestAcct, sizeof(oCondT.m_sDestAcct), pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_type") == 0){
                oCondT.m_iDestType = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Destination_Attr") == 0){
                oCondT.m_iDestAttr = atoi(pTmpElem->GetText());
                continue;
            }
            if (strcmp(pTmpElem->Value(), "Billing_Cycle_ID") == 0){
               oCondT.m_uiBilCycID = atoi(pTmpElem->GetText());
                continue;
            }
        }
		
		__DEBUG_LOG1_(0, "m_sOperSeq:[%s]",oCondT.m_sOperSeq);
        __DEBUG_LOG1_(0, "m_sDestAcct:[%s]",oCondT.m_sDestAcct);
		__DEBUG_LOG1_(0, "m_iDestType:[%d]",oCondT.m_iDestType);
		__DEBUG_LOG1_(0, "m_iDestAttr:[%d]",oCondT.m_iDestAttr);
		__DEBUG_LOG1_(0, "m_uiBilCycID:[%d]",oCondT.m_uiBilCycID);
        
        if (!pPack->addBil(oCondT)) {
	        __DEBUG_LOG0_(0, "parseChargeR: addPayInfo error.");
	        return -1;
	    }
	//}        
   
    return 0;
}

