#include "testxmlmsg.h"

#define MSG_SIZE 4096

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
    if ((g_poSndMQ=new MessageQueue(10007)) == NULL) {
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

/***
int PackXMLMsg::run()
{
    int iRet;
    int iMsgLen;	
    while (!RECEIVE_STOP()) {
      
        //g_sSndBuf.m_lMsgType = g_oRcvBuf.m_lType;
		g_sSndBuf.m_lMsgType = 1;
        
        //添加xml声明
        TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "gbk", "" ); 
        m_poXmlDoc->LinkEndChild(decl);
		
		int icmd;
		cout<<"输入需要处理的业务"<<endl;
		cout<<"退出  0"<<endl;
		cout<<"余额划拨 Balance_Trans  1"<<endl;
		cout<<"余额查询 Query  2"<<endl;
		cout<<"余额划拨查询 Balance_Query  3"<<endl;
		cout<<"电子账单查询 RechargeBillQuery  4"<<endl;
		cout<<"交易记录查询 QueryPayLogByAccout  5"<<endl;
		cout<<"冲退记录查询 RechargeRecQuery  6"<<endl;
		cout<<"支付 Charge  7"<<endl;
		cout<<"支付能力状态查询 PayStatus_Query 8"<<endl;
		cout<<"余额划拨规则查询 TransRules_Query 9"<<endl;
		cout<<"余额划拨规则设置 TransRules_Reset 10"<<endl;
		cout<<"支付&登陆密码修改 Password_Reset 11"<<endl;
		cout<<"支付能力状态变更 PayStatus_Reset 12"<<endl;

		cin>>icmd;
        
        switch (icmd)
        {
            case 1:   //余额划拨
                iRet = packTransBalA();
                break;
				
            case 2:  //余额查询
                iRet = packQueryA();
				break;
			
			case 3:  //余额划拨查询
			    iRet = packQryBalA();
				break;

			case 4: //电子账单查询
			    iRet = packQryRechargeBillA();
				break;
			case 5: //交易记录查询
			    iRet = packQryPayLogA();
				break;
			case 6: //冲退记录查询
			    iRet = packRechargeRecA();
				break;
			case 7: 	    //3.4.6.2 支付
				iRet = packChargeA();
				break;
				
			case 8: 	    // 支付能力状态查询
				iRet = packQryPayStatusA();
				break;
				
			case 9: 	    // 支付能力状态查询
				iRet = packQryTransRuleA();
				break;
			case 10: 	    // 余额划拨规则设置
				iRet = packResetTransRuleA();
				break;
			case 11: 	    // 余额划拨规则设置
				iRet = packResetPassWdA();
				break;
			case 12: 	    // 余额划拨规则设置
				iRet = packResetPayStatusA();
				break;

			case 0:
			    goto __OPEN;
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
		
    }
    
__OPEN:
    __DEBUG_LOG0_(0, "程序收到安全退出信号, 安全退出");
    
    return 0;
}
***/

//20110819xkm

int PackXMLMsg::run()
{
    int iRet;
    int iMsgLen;
	char sTmp[256];
	char sTmpNUM[256];
	string sTmpT;
	while(1)
   {
	ABMException oExp;
	ReadCfg *m_poConfS = NULL; 
	char * const pLable = "TEST_CONF";
    if (m_poConfS == NULL) { 
        if ((m_poConfS=new ReadCfg) == NULL) {
            ADD_EXCEPT0(oExp, "ABMConf::getConf m_poConfS malloc failed!");
            return -1;
        }
    }
    //char *p = getenv(ABM_HOME);
	char sValue[128];
    char path[1024];

 //   char sTmpC[1024] = {0};
 //   strncpy(sTmpC, p, sizeof(sTmpC)-2);
 //   if (sTmpC[strlen(sTmpC)-1] != '/') {
 //       sTmpC[strlen(sTmpC)] = '/';
 //   }

    snprintf(path, sizeof(path), "/ABM/app/deploy/config/abm_Balance_number.ini");
    if (m_poConfS->read(oExp, path, pLable) != 0) {
        ADD_EXCEPT2(oExp, "AccuConf::getConf 读取配置文件%s中的%s出错", path, pLable);
        return -1;
    }
    int TransFlag =2;
	for (TransFlag;TransFlag>0;TransFlag--){
    int iLines = m_poConfS->getRows();
	int i = 0;
	int j = 0;
	memset(sValue, 0x00, sizeof(sValue));
	
    for(i;i<iLines;i++)
	{
	 j = i+1;
	 cout<<"处理的数据条数"<<j<<endl;
	 if (!m_poConfS->getValue(oExp, sValue, "number", j)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现number选项,读取失败!", pLable);
            return -1;
        }
	
    //sTmpT.assign(sValue);
	//strncpy(sTmpNUM,(LPCTSTR)sTmpT,sizeof(sTmpNUM));  
	int iVar;

	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Balance_Trans")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	

	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sValue)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=123456;
	pElem = new TiXmlElement("Pay_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Balance_TransFlag");
	snprintf(sTmp, sizeof(sTmp), "%d", TransFlag);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=0;
	pElem = new TiXmlElement("UnitType_Id");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Deduct_Amount");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);

    //if (iRet != 0) {
    //        m_poXmlDoc->Clear();
    //        __DEBUG_LOG1_(0, "消息打包过程出错，命令号：%d. ", g_oRcvBuf.m_iCmdID);
    //       continue;
    //    }
   

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
        //cout<<"CCA消息发送前原始消息包：\n"<<g_sSndBuf.m_sMsg<<endl;
#endif
          
        if (g_poSndMQ->Send((void *)&g_sSndBuf, iMsgLen+sizeof(long)) < 0) {
	        __DEBUG_LOG1_(0, "消息队列发送失败, 程序退出! errno = %d", errno);
			return -1;	
		} 
		sleep(1);
	}
	sleep(10);
	}
	sleep(500);
	
   }
}


//3.4.6.5 余额划拨
int PackXMLMsg::packTransBalA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Balance_Trans")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Pay_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入划出类型 1 被动划拨(划拨充值)  2 主动划拨(中心余额划出)"<<endl;
    cin>>iVar;
	
	pElem = new TiXmlElement("Balance_TransFlag");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("UnitType_Id");
	snprintf(sTmp, sizeof(sTmp), "%d", 0);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入划拨金额"<<endl;
	cin>>iVar;
	pElem = new TiXmlElement("Deduct_Amount");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	return 0;
}


//3.4.6.13 余额查询
int PackXMLMsg::packQueryA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Query")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Balance_Information");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Id");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_Id_Type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入区号"<<endl;
    cin>>iVar;
	pElem = new TiXmlElement("AreaCode");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);

	pElem = new TiXmlElement("Query_Flag");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Query_Item_Type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	
	return 0;
}


//3.4.6.4 余额划拨查询
int PackXMLMsg::packQryBalA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Balance_Query")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Balance_Information");
	root->LinkEndChild(pPayInfoB);
	
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);

	pElem = new TiXmlElement("Query_Flag");
	snprintf(sTmp, sizeof(sTmp), "%d", 0);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Query_Item_Type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	return 0;
}


//3.4.6.16 电子账单查询
int PackXMLMsg::packQryRechargeBillA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("RechargeBillQuery")); 
	root->LinkEndChild(pRltCode);
	
	/**
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	***/
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	pElem = new TiXmlElement("Operate_Seq");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
    iVar = 20110601;
	pElem = new TiXmlElement("Billing_Cycle_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);	
	
	return 0;
}

//3.4.6.15 交易记录查询
int PackXMLMsg::packQryPayLogA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("QueryPayLogByAccout")); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	TiXmlElement * pElem = new TiXmlElement("Operate_Action");
	snprintf(sTmp, sizeof(sTmp), "%d", 13);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	pElem = new TiXmlElement("Operate_Seq");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("SP_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 23456);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 23456);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Query_Start_Time");
	snprintf(sTmp, sizeof(sTmp), "%d", 20110701);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
    iVar = 20110711;
	pElem = new TiXmlElement("Query_Expiration_Time");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);	
	
	return 0;
}

//3.4.6.14 冲退记录查询
int PackXMLMsg::packRechargeRecA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("RechargeRecQuery")); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
		time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","002201100707164230",tTime);
	TiXmlElement * pElem = new TiXmlElement("Operate_Seq");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
		
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Query_Start_Time");
	snprintf(sTmp, sizeof(sTmp), "%d", 20110701);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
    iVar = 20110711;
	pElem = new TiXmlElement("Query_Expiration_Time");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);	
	
	return 0;
}


 //3.4.6.2 支付
int PackXMLMsg::packChargeA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Charge")); 
	root->LinkEndChild(pRltCode);
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	TiXmlElement * pElem = new TiXmlElement("Operate_Action");
	snprintf(sTmp, sizeof(sTmp), "%d", 8);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
		time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","889901100707164230",tTime);
	pElem = new TiXmlElement("Operate_Seq");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("SP_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Staff_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 888);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Micropay_Type");
	snprintf(sTmp, sizeof(sTmp), "%d", 0);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
		
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Pay_Password");
	snprintf(sTmp, sizeof(sTmp), "%s", "123456");
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Random_Password");
	snprintf(sTmp, sizeof(sTmp), "%s", "123456");
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Pay_Date");
	snprintf(sTmp, sizeof(sTmp), "%d", 20110712);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Order_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", 98765);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Description");
	snprintf(sTmp, sizeof(sTmp), "%s", "支付平台");
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);	
	
	cout<<"支付金额"<<endl;
	cin>>iVar;
	pElem = new TiXmlElement("PayAmount");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);	
	
	return 0;
}


// 支付能力状态查询
int PackXMLMsg::packQryPayStatusA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("PayStatus_Query")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	return 0;
}



// 余额划拨规则查询
int PackXMLMsg::packQryTransRuleA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("TransRules_Query")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	return 0;
}



// 余额划拨规则设置
int PackXMLMsg::packResetTransRuleA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	cout<<"余额划拨规则设置 TransRules_Reset 10"<<endl;
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("TransRules_Reset")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=123456;
	pElem = new TiXmlElement("Pay_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=3;
	pElem = new TiXmlElement("Date_Trans_Freq");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=5;
	pElem = new TiXmlElement("Trans_Limit");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=7;
	pElem = new TiXmlElement("Month_Trans_Limit");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=0;
	pElem = new TiXmlElement("Auto_Trans_Flag");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=3;
	pElem = new TiXmlElement("Auto_Balance_limit");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=3;
	pElem = new TiXmlElement("Auto_Amount_limit");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	
	return 0;
}



// 8 支付&登陆密码修改
int PackXMLMsg::packResetPassWdA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("Password_Reset")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入旧密码"<<endl;
    cin>>iVar;
	//iVar=123456;
	pElem = new TiXmlElement("Old_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入新密码"<<endl;
    cin>>iVar;
	pElem = new TiXmlElement("New_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	cout<<"请输入01-修改密码,02-新建密码"<<endl;
    cin>>sTmp;
	pElem = new TiXmlElement("Action_id");
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
		
	return 0;
}



//10.支付能力状态变更
int PackXMLMsg::packResetPayStatusA()
{
    int iRet;
	int iVar;
    char sTmp[256];
	
	TiXmlElement * root = new TiXmlElement("Service_Information");
	m_poXmlDoc->LinkEndChild(root);
	
	TiXmlElement * pRltCode = new TiXmlElement("Service_Context_Id");
	pRltCode->LinkEndChild( new TiXmlText("PayStatus_Reset")); 
	root->LinkEndChild(pRltCode);
	
	time_t tTime;
    tTime = time(NULL);
    snprintf(sTmp,sizeof(sTmp),"%s%010d\0","898901100707164230",tTime);
	pRltCode = new TiXmlElement("Request_Id");
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	pRltCode = new TiXmlElement("Request_Time");
	snprintf(sTmp, sizeof(sTmp), "%10d", 2248988800);
	pRltCode->LinkEndChild( new TiXmlText(sTmp)); 
	root->LinkEndChild(pRltCode);
	
	
	TiXmlElement * pPayInfoB = new TiXmlElement("Micropayment_Info");
	root->LinkEndChild(pPayInfoB);
	
	cout<<"请输入用户号码"<<endl;
    cin>>sTmp;
	TiXmlElement * pElem = new TiXmlElement("Destination_Account");
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	pElem = new TiXmlElement("Destination_type");
	snprintf(sTmp, sizeof(sTmp), "%d", 1);
	pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	   
	pElem = new TiXmlElement("Destination_Attr");
	snprintf(sTmp, sizeof(sTmp), "%d", 2);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=1;
	pElem = new TiXmlElement("Service_Platform_ID");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=12;
	pElem = new TiXmlElement("Pay_Status");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
	iVar=123456;
	pElem = new TiXmlElement("Pay_Password");
	snprintf(sTmp, sizeof(sTmp), "%d", iVar);
    pElem->LinkEndChild( new TiXmlText(sTmp)); 
	pPayInfoB->LinkEndChild(pElem);
	
		
	return 0;
}





