#include "dccpublic.h"

using namespace std;
#define ABM_HOME "ABMAPP_DEPLOY"
#define _DCC_SUCCESS_ 2001

NodeConfStru g_oNodeConf;   //业务节点的一些固定配置
static unsigned int g_iHHTimes;

//以16进制显示缓存区内容
void disp_buf(char *buf, int size)
{
  int i;
  printf("buf [%d] start:\n", size);
  for( i = 0 ; i < size; i ++ )
  {
  	if( i%16 == 0 )
  		printf("\n");
  	printf("%02X ", buf[i]&0xff);
  }
  printf("\nbuf end\n");
  fflush(0);
}

unsigned int getH2H()
{
    int iVar = 0;
    static time_t now_time = time(0);
    iVar = (int)now_time + _SEED_NUM_*(g_iHHTimes++);
    
    return iVar;
}

unsigned int getE2E()
{
    int iVar = 0;
    time_t nowtime = time(0);
    srand((unsigned int)nowtime);
    
    iVar = (rand() & 0x000fffff) | (((int)nowtime) << 20);
    
    return iVar;
}
    
    
/*****************************************************
 *
 *            几个非业务应答打包
 *
 *****************************************************/ 
int packCEA(DccMsgParser *poDcc, StruMqMsg *pSendMsg)
{
    int int32 = 2001, i, iVenderID;
    diameter_address_t m_dHostIPAddressX;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    struct in_addr addr;
    string sTmp, sProductName;
    
    //取出相关的avp,有的话
    poDcc->getAvpValue("Vendor-Id", iVenderID);
    poDcc->getAvpValue("Product-Name", sProductName);
    
    //打包
    poDcc->setDiameterHeader(COMMAND_CODE_CE, false,0,3,4);
    poDcc->releaseMsgAcl();
    
    poDcc->setAvpValue("Result-Code",int32);
    sTmp.assign("ABMCETER@001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Host",sTmp);
    sTmp.assign("001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Realm",sTmp);
    
	m_dHostIPAddressX.type =1;
        
    if(gethostname(sHostName, sizeof(sHostName)) !=-1)
    {
        struct hostent* phost = gethostbyname(sHostName);
        for(i=0;phost!= NULL&&phost->h_addr_list[i]!=NULL;i++)
        {
            memcpy(&addr, phost->h_addr_list[i], sizeof(struct in_addr));
        }
        strcpy(LocalIp, inet_ntoa(addr));
    }
    int buf[4] = {0};
    sscanf(LocalIp, "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
    char tmp[4] = {0};
    tmp[0] = buf[0];
    tmp[1] = buf[1];
    tmp[2] = buf[2];
    tmp[3] = buf[3];

    m_dHostIPAddressX.value.assign(tmp,4);
    poDcc->setAvpValue("Host-IP-Address",m_dHostIPAddressX);
    poDcc->setAvpValue("Vendor-Id",iVenderID);
    poDcc->setAvpValue("Product-Name",sProductName);
    
    i = poDcc->parseAppToRaw(pSendMsg->m_sMsg, MSG_SIZE);
    if (i <= 0) {
        __DEBUG_LOG0_(0, "打包失败! ");
        return -1;
    }
    
    return i;
}

int packDWA(DccMsgParser *poDcc, StruMqMsg *pSendMsg)
{
    int int32 = 2001, i;
    string sHost, sRealm;
    
    //取出相关的avp,有的话
    poDcc->getAvpValue("Origin-Host", sHost);
    poDcc->getAvpValue("Origin-Realm", sRealm);
    
    //打包
    poDcc->setDiameterHeader(COMMAND_CODE_DW, false,0,3,4);
    poDcc->releaseMsgAcl();
    
    poDcc->setAvpValue("Result-Code",int32);
    //sTmp.assign("ABMCETER@001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Host",sHost);
    //sTmp.assign("001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Realm",sRealm);
    
    i = poDcc->parseAppToRaw(pSendMsg->m_sMsg, MSG_SIZE);
    if (i <= 0) {
        __DEBUG_LOG0_(0, "打包失败! ");
        return -1;
    }
    
    return i;
}

int packDPA(DccMsgParser *poDcc, StruMqMsg *pSendMsg)
{
    int int32 = 2001, i;
    string sHost, sRealm;
    
    //取出相关的avp,有的话
    poDcc->getAvpValue("Origin-Host", sHost);
    poDcc->getAvpValue("Origin-Realm", sRealm);
    
    //打包
    poDcc->setDiameterHeader(COMMAND_CODE_DP, false,0,3,4);
    poDcc->releaseMsgAcl();
    
    poDcc->setAvpValue("Result-Code",int32);
    //sTmp.assign("ABMCETER@001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Host",sHost);
    //sTmp.assign("001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Realm",sRealm);
    
    i = poDcc->parseAppToRaw(pSendMsg->m_sMsg, MSG_SIZE);
    if (i <= 0) {
        __DEBUG_LOG0_(0, "打包失败! ");
        return -1;
    }
    
    return i;
}

int packNetCtrl(DccMsgParser *poDcc, StruMqMsg *pSendMsg)
{
    string sTmp;
    int iVar;
    ACE_UINT32 U32Var;
    diameter_address_t m_dHostIPAddressX;
    int iLen;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    struct in_addr addr;
    pSendMsg->m_lMsgType = 1;
	
	//session-id
	char sTmpName[110];
	time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
	snprintf(sTmpName,sizeof(sTmpName),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",iSeq);
	cout<<"sTmpName"<<sTmpName;
	//session-id
	
	
    poDcc->setDiameterHeader(COMMAND_CODE_CC, true, 4, 3, 4);
    poDcc->releaseMsgAcl();

    //sTmp.assign("ABM@001.ChinaTelecom.com;1306402306;0004");
	sTmp.assign(sTmpName);
    poDcc->setAvpValue("Session-Id", sTmp);
    
    sTmp.assign("ABM@001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Host", sTmp);
    
    sTmp.assign("001.ChinaTelecom.com");
    poDcc->setAvpValue("Origin-Realm", sTmp);
    
    sTmp.assign("SR2@001.ChinaTelecom.com");
    poDcc->setAvpValue("Destination-Host", sTmp);
    
    sTmp.assign("001.ChinaTelecom.com");
    poDcc->setAvpValue("Destination-Realm", sTmp);
    
    iVar = 4;
    poDcc->setAvpValue("Auth-Application-Id", iVar);

    iVar = 111;
    poDcc->setAvpValue("Requested-Action", iVar);
    iVar = 4;
    poDcc->setAvpValue("CC-Request-Type", iVar);
    
    iVar = 0;
    poDcc->setAvpValue("CC-Request-Number", iVar);
    
    sTmp.assign("Active.Net-Ctrl@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Context-Id", sTmp);
    
    U32Var = time(0);
    poDcc->setAvpValue("Event-Timestamp", U32Var);
    /*****
    sTmp.assign("201105251021481234567800-00010-000024005999-v1.02.03-010001-Recharge.ABM@001;");
    poDcc->setAvpValue("Service-Flow-Id", sTmp);
    ****/
    
    sTmp.assign("ABM@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Host", sTmp);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Type", iVar);
    
    m_dHostIPAddressX.type =1;   
    if(gethostname(sHostName, sizeof(sHostName)) !=-1)
    {
        struct hostent* phost = gethostbyname(sHostName);
        for(int i=0;phost!= NULL&&phost->h_addr_list[i]!=NULL;i++)
        {
            memcpy(&addr, phost->h_addr_list[i], sizeof(struct in_addr));
        }
        strcpy(LocalIp, inet_ntoa(addr));
    }
    int buf[4] = {0};
    sscanf(LocalIp, "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
    char tmp[4] = {0};
    tmp[0] = buf[0];
    tmp[1] = buf[1];
    tmp[2] = buf[2];
    tmp[3] = buf[3];

    m_dHostIPAddressX.value.assign(tmp,4);
    
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-IP", m_dHostIPAddressX);
    
    iVar = 3868;
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Port", iVar);
    
    iVar = 12;
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.Network-Latency", iVar);
	
    /****
    sTmp.assign("Query.Balance@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[0]", sTmp);
    
    sTmp.assign("ReverseChargeFromABM.Micropayment@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[1]", sTmp);

    sTmp.assign("DeductFromABM.Micropayment@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[2]", sTmp);
	sTmp.assign("VCRecharge.Balance@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[3]", sTmp);
	sTmp.assign("VCRefund.Balance@001.ChinaTelecom.com");
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[4]", sTmp);
    ****/
	
	//net-ctrl   config
	ABMException oExp;
	ReadCfg *m_poConfS = NULL; 
	char * const pLable = "DCC_ACTIVE_CONF";
    if (m_poConfS == NULL) { 
        if ((m_poConfS=new ReadCfg) == NULL) {
            ADD_EXCEPT0(oExp, "ABMConf::getConf m_poConfS malloc failed!");
            return -1;
        }
    }
    char *p = getenv(ABM_HOME);
	char sValue[128];
    char path[1024];
    char sTmpC[1024] = {0};
    strncpy(sTmpC, p, sizeof(sTmpC)-2);
    if (sTmpC[strlen(sTmpC)-1] != '/') {
        sTmpC[strlen(sTmpC)] = '/';
    }
    snprintf(path, sizeof(path), "%sconfig/abm_app_cfg.ini", sTmpC);
    if (m_poConfS->read(oExp, path, pLable) != 0) {
        ADD_EXCEPT2(oExp, "AccuConf::getConf 读取配置文件%s中的%s出错", path, pLable);
        return -1;
    }
    
    int iLines = m_poConfS->getRows();
	int i = 0;
	int j = 0;
	memset(sValue, 0x00, sizeof(sValue));
    for(i;i<iLines;i++)
	{
	 j = i+1;
	 if (!m_poConfS->getValue(oExp, sValue, "active-comm", j)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现origin-host选项,读取失败!", pLable);
            return -1;
        }
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[%d]", i);
    sTmp.assign(sValue);
    poDcc->setAvpValue(sTmpName, sTmp);
	}
	//net-ctrl   config
	
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Priority", iVar);
    
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Update-Action", iVar);
    
    iLen = poDcc->parseAppToRaw(pSendMsg->m_sMsg, MSG_SIZE);
    if (iLen <= 0) {
        __DEBUG_LOG0_(0, "打包失败! ");
        return -1;
    }
    
    return iLen;
}
    
/*****************************************************
 *
 *            DCC消息公共部分的解包打包
 *
 *****************************************************/  
 
//根据service-context-id(请求包)或service-context-id(应答包)
//的第一个域判断业务类型，返回业务员类型
int switchCmd(string &sCmd, int iReqFlag)
{
    int iCmd = -1;
    int iPos;
    
#ifdef _DEBUG_DCC_
    if (iReqFlag == 1)
        __DEBUG_LOG1_(0, "service-context-id:%s", sCmd.c_str());
    else
        __DEBUG_LOG1_(0, "service-flow-id:%s", sCmd.c_str());
#endif

    //对于应答包，cCmd对应的是Service-Flow-Id中的内容，取出最后一个track单元
    //关于Service-Flow-Id中的内容，请参照规范11.33中的介绍
	//20110604
    /*
    if (iReqFlag == 0) {
        if(sCmd[sCmd.size() - 1] != ';') {
            __DEBUG_LOG0_(0, "Service-Flow-Id中最后一位不是';',无法识别命令.");
            return -1;
        }
        
        iPos = sCmd.rfind(';', sCmd.size()-2);
        if (iPos == string::npos) {
            iPos = sCmd.rfind('-', sCmd.size()-2);
            if (iPos == string::npos) {
                __DEBUG_LOG0_(0, "Service-Flow-Id中格式有误,未发现';'或'-'.");
                return -1;
            }
        }
        sCmd = sCmd.substr(iPos+1, sCmd.size()-iPos-2);
    }
    */  
	//20110604  
    //取业务类型，如Recharge.Balance@000中的Recharge
    iPos = sCmd.find('.', 0);
    if (iPos != string::npos) {
        sCmd = sCmd.substr(0, iPos);
    }
    
    if (strcmp(sCmd.c_str(), "ServInfoQuery") == 0) {            //3.4.1.1资料查询并开户
        iCmd = ABMCMD_QRY_SRV_INFOR;
    } else if (strcmp(sCmd.c_str(), "Charge") == 0) {            //3.4.2.1支付
        iCmd = ABMPAYCMD_INFOR;
    } else if (strcmp(sCmd.c_str(), "Random-Password") == 0) {   //3.4.2.5 支付二次认证的密码下发
    	iCmd = ABMCMD_QRY_SRV_PSWD;
    }else if(strcmp(sCmd.c_str(), "DeductFromABM") == 0){        //3.4.5.1
	    iCmd = ABMCMD_ALLOCATE_BALANCE;
	} else if(strcmp(sCmd.c_str(), "RechargeBillQuery") == 0){   //3.4.6.16
	    iCmd = ABMCMD_QRY_BIL_INFOR;
	} else if(strcmp(sCmd.c_str(), "QueryPayLogByAccout") == 0){ //3.4.6.15
	    iCmd = ABMCMD_QRY_PAY_INFOR;
	} else if(strcmp(sCmd.c_str(), "RechargeRecQuery") == 0){    //3.4.6.14
	    iCmd = ABMCMD_QRY_REC_INFOR;
	} else if(strcmp(sCmd.c_str(), "ReverseChargeFromABM") == 0){//3.4.5.5
	    iCmd = ABMCMD_REVERSE_DEDUCT_BALANCE;
	} else if (strcmp(sCmd.c_str(), "Balance_Query") == 0) {     //3.4.6.4  余额划拨查询
	    if (iReqFlag == 1) 
	        iCmd = ABMCMD_QRY_MICROPAY_INFO;       //全国中心abm收到的查询请求
	    else 
	        iCmd == ABMCMD_QRY_MICROPAYCCA_INFO;   //全国中心abm主动发送请求的回包
    }else if(strcmp(sCmd.c_str(), "Query") == 0){//3.4.5.3，3.4.5.4，3.4.6.13
	    iCmd = ABMCMD_QRY_BAL_INFOR;
	}else if(strcmp(sCmd.c_str(), "VCRecharge") == 0){//3.4.4.1 开户充值 VC规范2.1  张成
	    iCmd = ABMDEPOSITCMD_INFOR;
	}else if(strcmp(sCmd.c_str(), "VCRefund") == 0){//3.4.4.3 开户充值 VC规范2.3  张成
	    iCmd = ABM_REVERSEEPOSIT_CMD;
	}
	else if(strcmp(sCmd.c_str(), "Balance_Trans") == 0){//3.4.6.5  余额划拨  zhuyong
	    iCmd = ABMCMD_PLATFORM_DEDUCT_BALANCE;
	}
	else if(strcmp(sCmd.c_str(), "RechargeFromABM000") == 0){//3.4.5.2 全国中心余额划出  zhuyong
	    iCmd = ABMCMD_ACTIVE_ALLOCATE_BALANCE;
	}
	else if(strcmp(sCmd.c_str(), "Reverse_Charge") == 0){//3.4.2.4  支付冲正 
	    iCmd = ABMCMD_REVERSE_PAY;
	}
	else if(strcmp(sCmd.c_str(), "InfoSynchroFromABM000") == 0){//3.4.1.2  资料同步  ABM到HSS
	    iCmd = ABMCMD_QRY_SRV_SYN;
	}
	else if(strcmp(sCmd.c_str(), "InfoSynchroFromHSS000") == 0){//3.4.1.2  资料同步  HSS到ABM
	    iCmd = ABMCMD_QRY_SRV_SYN;
	}
	else if(strcmp(sCmd.c_str(), "PaymentPasswdAuthen") == 0){//3.4.1.3  密码鉴权  ABM到HSS
	    iCmd = ABMCMD_PASSWD_IDENTIFY;
	}
	else if(strcmp(sCmd.c_str(), "InfoSynchroFromHSS000") == 0){//3.4.6.1  登陆鉴权  自服平台到ABM
	    iCmd = ABMCMD_LOGIN_PASSWD;
	}
	else {                                                     //没有配置，未知命令
        iCmd = 0;
    }
    
    return iCmd;
} 
//获取dcc请求包的公共信息，存入结构体，成功返回请求消息的业务类型（正数）
int getDccHead(DccMsgParser *poDcc, ABMCCR *pCCR, int iReqFlag, TimesTenCMD *poTTCmd)
{
    string sTemp;
    int iCmdId, iPos;
    struDccHead struhead;
    char sSvcCtxID[72];
    
    if (poDcc == NULL || pCCR == NULL) {
	    __DEBUG_LOG0_(0, "传入的指针有空指针.");
	    return -1;
	}
	
	struhead.m_iHH = poDcc->getDiameterHeader().hh;
	struhead.m_iEE = poDcc->getDiameterHeader().ee;
    
    poDcc->getAvpValue("Session-Id", sTemp);
    snprintf(struhead.m_sSessionId, sizeof(struhead.m_sSessionId), sTemp.c_str());
    
    poDcc->getAvpValue("Origin-Host", sTemp);
    snprintf(struhead.m_sOrignHost, sizeof(struhead.m_sOrignHost), sTemp.c_str());
    
    poDcc->getAvpValue("Origin-Realm", sTemp);
    snprintf(struhead.m_sOrignRealm, sizeof(struhead.m_sOrignRealm), sTemp.c_str());
    
    poDcc->getAvpValue("Auth-Application-Id", struhead.m_iAuthAppId);
    
    poDcc->getAvpValue("Service-Flow-Id", sTemp);
    snprintf(struhead.m_sSrvFlowId, sizeof(struhead.m_sSrvFlowId), sTemp.c_str());
    
    poDcc->getAvpValue("OutPlatformDescription", sTemp);
    snprintf(struhead.m_sOutPlatform, sizeof(struhead.m_sOutPlatform), sTemp.c_str());
    
    poDcc->getAvpValue("CC-Request-Type", struhead.m_iReqType);
    
    poDcc->getAvpValue("CC-Request-Number", struhead.m_iReqNumber);
    
    if (iReqFlag == 1) {   //CCR
        poDcc->getAvpValue("Destination-Realm", sTemp);
        snprintf(struhead.m_sDestRealm, sizeof(struhead.m_sDestRealm), sTemp.c_str());
        
        poDcc->getAvpValue("Service-Context-Id", sTemp);
        snprintf(struhead.m_sSrvTextId, sizeof(struhead.m_sSrvTextId), sTemp.c_str());
        
        struhead.m_iResultCode = _DCC_SUCCESS_;
    } else {               //CCA
        poDcc->getAvpValue("Result-Code", struhead.m_iResultCode);
        memset(struhead.m_sDestRealm, 0, sizeof(struhead.m_sDestRealm));
        memset(struhead.m_sSrvTextId, 0, sizeof(struhead.m_sSrvTextId));
        //为传入switchCmd的参数赋值
		//20110604
        //sTemp.assign(struhead.m_sSrvFlowId);
        if (poTTCmd == NULL) {
            __DEBUG_LOG0_(0, "pointor poTTCmd is NULL"); 
            return -1;
        }
        try {
            poTTCmd->setParam(1, struhead.m_sSessionId);
            poTTCmd->Execute();
            if (poTTCmd->FetchNext() == 0) {
                poTTCmd->getColumn(1, sSvcCtxID);
                sTemp.assign(sSvcCtxID);
            } else {
                poTTCmd->Close();
                __DEBUG_LOG1_(0, "can't fine a record for session-id:%s.",  struhead.m_sSessionId);
                return -1;
            }
            poTTCmd->Close();
        } catch (TTStatus &st) {
            poTTCmd->Close();
            __DEBUG_LOG1_(0, "tt operator error:%s.",  st.err_msg);
            return -1;
        }   
	//20110604
    }
    
    //对于请求包sTmp取Service-Context-Id的内容，应答包取Service-Flow-Id的内容
    if ((iCmdId = switchCmd(sTemp, iReqFlag)) <= 0) {
        __DEBUG_LOG2_(0, "parameter:<%s>,request-flag=%d can't find a fit command.", 
                                                                        sTemp.c_str(), iReqFlag);
        return iCmdId;
    }
    
#ifdef _DEBUG_DCC_
    struhead.print();
#endif

    //复制到CCR结构的公共头部
    memcpy((void *)pCCR->m_sDccBuf, (void *)&struhead, sizeof(struDccHead));
    
    //消息的命令码
    pCCR->m_iCmdID = iCmdId;

    return iCmdId;
}

//打包dcc应答包的公共信息，部分信息取自公共头部结构体，成功返回0，失败-1
int setDccHead(DccMsgParser *poDcc, ABMCCA *pCCA, int iReqFlag,char *sessCCR,char *srealmCCR)
{
	string sTemp;
	struDccHead *pDccHead;
	
	if (poDcc == NULL || pCCA == NULL) {
	    __DEBUG_LOG0_(0, "setCCAHead:传入的指针有空指针");
	    return -1;
	}
	
	pDccHead = (struDccHead *)pCCA->m_sDccBuf;
#ifdef _DEBUG_DCC_
    cout<<"setCCAHead：传入的公共头部信息"<<endl;
    pDccHead->print();
#endif	
	
	if (iReqFlag == 0 or pDccHead->m_sSrvTextId[0] == '\0')
	    poDcc->setDiameterHeader(272, false, 4, pDccHead->m_iHH, pDccHead->m_iEE);
	else
	    poDcc->setDiameterHeader(272, true, 4, pDccHead->m_iHH, pDccHead->m_iEE);
    poDcc->releaseMsgAcl();
	
	if (pDccHead->m_sSessionId[0] == '\0') {
	    __DEBUG_LOG0_(0, "Session-Id不可为空.");
	    return -1;
	}
	sTemp.assign(pDccHead->m_sSessionId);
	poDcc->setAvpValue("Session-Id", sTemp);
	//poDcc->setAvpValue("Result-Code", pDccHead->m_iResultCode);
	strcpy(sessCCR, pDccHead->m_sSessionId); 
	
	sTemp.assign(pDccHead->m_sSrvFlowId);
	poDcc->setAvpValue("Service-Flow-Id", sTemp);
	
	sTemp.assign(pDccHead->m_sOutPlatform);
	poDcc->setAvpValue("OutPlatformDescription", sTemp);
	
	//sTemp.assign(pDccHead->m_sOrignHost);
	sTemp.assign(g_oNodeConf.m_sOriginHost);
	poDcc->setAvpValue("Origin-Host", sTemp);
	
	//sTemp.assign(pDccHead->m_sOrignRealm);
	sTemp.assign(g_oNodeConf.m_sOriginRealm);
	poDcc->setAvpValue("Origin-Realm", sTemp);
	strcpy(sessCCR, pDccHead->m_sSessionId);
	
	poDcc->setAvpValue("Auth-Application-Id", pDccHead->m_iAuthAppId);
	
	poDcc->setAvpValue("CC-Request-Type", pDccHead->m_iReqType);
	
	poDcc->setAvpValue("CC-Request-Number", pDccHead->m_iReqNumber);
	
	if (iReqFlag == 0 or pDccHead->m_sSrvTextId[0] == '\0') {            //应答包  pDccHead->m_sSrvTextId是为了判断主动CCA
	    poDcc->setAvpValue("Result-Code", pDccHead->m_iResultCode);
	} else {                        //请求包
	    if (pDccHead->m_sOrignRealm[0] == '\0' || pDccHead->m_sSrvTextId[0] == '\0') {
	        __DEBUG_LOG0_(0, "Destination-Realm或Service-Context-Id没有设置");
	        return -1;
	    }
#ifdef _DEBUG_DCC_
    __DEBUG_LOG1_(0, "Destination-Realm:%s", pDccHead->m_sOrignRealm);
    __DEBUG_LOG1_(0, "Service-Context-Id:%s", pDccHead->m_sSrvTextId);
#endif		    
	    sTemp.assign(pDccHead->m_sDestRealm);
	    poDcc->setAvpValue("Destination-Realm", sTemp);
	    strcpy(srealmCCR, pDccHead->m_sDestRealm);
	    sTemp.assign(pDccHead->m_sSrvTextId);
	    poDcc->setAvpValue("Service-Context-Id", sTemp);
	}
	
	return 0;
}

/*****************************************************
 *
 *                  3.4.1.1 资料查询并开户
 *
 *****************************************************/
//解析dcc的资料查询请求，并压入内部结构
int parserInfoQryR(DccMsgParser *poDcc, ABMCCR *poSendBuf)
{
	string sTemp;
	int iTemp;
	QryServInforCond oQryCond;
	
	if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "parserInfoQryR:传入的指针有空指针");
	    return -1;
	}
	
	QryServInforCCR *pIn = (QryServInforCCR *)poSendBuf;
	memset(&oQryCond, 0, sizeof(oQryCond));	
	
	poDcc->getAvpValue("Service-Information.Request-Id", sTemp);
	snprintf(oQryCond.m_sReqID, sizeof(oQryCond.m_sReqID), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.Destination-Type", iTemp);
	oQryCond.m_hDsnType = iTemp;
	
	poDcc->getAvpValue("Service-Information.Destination-Account", sTemp);
	snprintf(oQryCond.m_sDsnNbr, sizeof(oQryCond.m_sDsnNbr), sTemp.c_str());
    
    poDcc->getAvpValue("Service-Information.Destination-Attr", iTemp);
    oQryCond.m_hDsnAttr = iTemp;
    
    poDcc->getAvpValue("Service-Information.Service_Platform_ID", sTemp);
    snprintf(oQryCond.m_sServPlatID, sizeof(oQryCond.m_sServPlatID), sTemp.c_str());
    
    poDcc->getAvpValue("Service-Information.QueryType", iTemp);
    oQryCond.m_uiQryType = iTemp;
    
    if (!pIn->addServInfor(oQryCond)) {
        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	    return -1;
	}
    
    return 0;
}

//根据内部机构体，打包资料查询的应答消息
int packInfoQryA(DccMsgParser *poDcc, ABMCCA *poSendBuf)
{
    if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "packInfoQryA:传入的指针有空指针.");
	    return -1;
	}
    
    QryServInforCCA *pIn = (QryServInforCCA *)poSendBuf;
    vector<ResponsStruct *> v1;
    vector<QryServInforData *> v2;
    string sTemp;
    char sAvpName[256], sN[16];
    ui64 U64Var;
    
    pIn->getRespons(v1);
    if (v1.size() == 0) return -1;
    pIn->getServData(v1[0], v2);
    
    sTemp.assign(v1[0]->m_sResID);    
    poDcc->setAvpValue("Service-Information.Response-Id", sTemp);
    
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_iRltCode);
    
    for (int i=0; i<v2.size(); i++) {
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].ServId", i);
        U64Var = v2[i]->m_lServID;
        poDcc->setAvpValue(sAvpName, U64Var);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].LocalServId", i);
        U64Var = v2[i]->m_lLocServID;
        poDcc->setAvpValue(sAvpName, U64Var);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].AcctId", i);
        U64Var = v2[i]->m_lAcctID;
        poDcc->setAvpValue(sAvpName, U64Var);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].ProductId", i);
        poDcc->setAvpValue(sAvpName, v2[i]->m_uiProdID);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].HomeAreaCode", i);
        poDcc->setAvpValue(sAvpName, v2[i]->m_uiAreaCode);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].Random-AccNbr", i);
        sTemp.assign(v2[i]->m_sAccNbr);
        poDcc->setAvpValue(sAvpName, sTemp);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].StateDate", i);
        sTemp.assign(v2[i]->m_sStateDate);
        poDcc->setAvpValue(sAvpName, sTemp);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].ServState", i);
        snprintf(sN, sizeof(sN), "%d", v2[i]->m_hServState);
        sTemp.assign(sN);
		//sTemp.assign(v2[i]->m_hServState);
        poDcc->setAvpValue(sAvpName, sTemp);
        
        snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Data-Information[%d].PaymentFlag", i);
        snprintf(sN, sizeof(sN), "%d", v2[i]->m_hPayFlag);
        sTemp.assign(sN);
		//sTemp.assign(v2[i]->m_hPayFlag);
        poDcc->setAvpValue(sAvpName, sTemp);
    }
    
    return 0;
}


/*****************************************************
 *
 *                  3.4.2.1 支付
 *
 *****************************************************/
//解包
int parserChargeR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    AbmPayCond oCond;
    string sTmp;
	ui64 iTmp64;
    ACE_UINT32 U32Var;
    time_t nowtime;
    struct tm *ptm;
    AbmPaymentCCR *pIn = (AbmPaymentCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Action", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_iOperateAction);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOperateSeq, sizeof(oCond.m_sOperateSeq), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].SP-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSpID, sizeof(oCond.m_sSpID), sTmp.c_str());

		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].SP-NAME", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSpName, sizeof(oCond.m_sSpName), sTmp.c_str());

	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sServicePlatformID, sizeof(oCond.m_sServicePlatformID), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Staff-ID", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_iStaffID);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Micropay-Type", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sMicropayType, sizeof(oCond.m_sMicropayType), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDestinationAccount, sizeof(oCond.m_sDestinationAccount), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    snprintf(oCond.m_sDestinationtype, sizeof(oCond.m_sDestinationtype), "%d", iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestinationAttr = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Pay-Password", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sPayPassword, sizeof(oCond.m_sPayPassword), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Random-Password", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sRandomPassword, sizeof(oCond.m_sRandomPassword), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Pay-Date", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
		
	    oCond.m_iPayDate=U32Var; 
		/***
	    U32Var -= SECS_70_YEARS;
	    nowtime = (time_t)U32Var;
	    ptm = localtime(&nowtime);
	    snprintf(oCond.m_sPayDate, sizeof(oCond.m_sPayDate), "%04d%02d%02d%02d%02d%02d",
	                                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                                        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        
        snprintf(oCond.m_sPayDate, sizeof(oCond.m_sPayDate), "%d", U32Var);
		****/
        
        snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Order-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOrderID, sizeof(oCond.m_sOrderID), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Description", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDescription, sizeof(oCond.m_sDescription), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].PayAmount", i);
		poDcc->getAvpValue(sTmpName, iTmp64);
	    oCond.m_lPayAmount = iTmp64;
	    //poDcc->getAvpValue(sTmpName, oCond.m_lPayAmount);
	    
	    if (!pIn->addPayInfo(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int packChargeA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResponsPayOne *> v1;
    vector<ResponsPayTwo *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	AbmPaymentCCA *pIn = (AbmPaymentCCA *)poCCA;
	iRet = pIn->getRespons(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getPayData(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	//dcc打包
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_iRltCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
    sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	for (int i=0; i<v2.size(); i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
	    sTmp.assign(v2[i]->m_sOperateSeq);
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Req-Serial", i);
	    sTmp.assign(v2[i]->m_sReqSerial);
	    poDcc->setAvpValue(sTmpName, sTmp);
	}
	
	return 0;
}
    
/*****************************************************
 *
 *              3.4.2.5 支付二次认证的密码下发
 *
 *****************************************************/
//解包
int parserRandPswdR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
	PasswordResponsStruct oCond1;
	QryPasswordCond       oCond2;
	int iNums, iRet;
	string sTmp;
	ACE_UINT32 U32Var;
	char sTmpName[128];
	
	if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserRandPswdR:传入的指针有空指针.");
	    return -1;
	}
	
	QryPasswordCCR *pIn = (QryPasswordCCR *)poCCR;
	
	//添加第一部分结构体
	memset((void *)&oCond1, 0, sizeof(oCond1));	
	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	snprintf(oCond1.m_ReqID, sizeof(oCond1.m_ReqID), sTmp.c_str());
	poDcc->getAvpValue("Service-Information.Request-Time", U32Var);
	oCond1.m_ReqTime = U32Var;
	if (!pIn->addpasswdRespons(oCond1)) {
		__DEBUG_LOG0_(0, "parserRandPswdR: addpasswdRespons error.");
		return -1;
	}
	
	//添加第二部分结构体
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
		memset((void *)&oCond2, 0, sizeof(oCond2));
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account",i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_sDestAct, sizeof(oCond2.m_sDestAct), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type",i);
		poDcc->getAvpValue(sTmpName, oCond2.m_DestType);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr",i);
		poDcc->getAvpValue(sTmpName, oCond2.m_DestAttr);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID",i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_SevPlatID, sizeof(oCond2.m_SevPlatID), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Random-AccNbr",i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_RanAccNbr, sizeof(oCond2.m_RanAccNbr), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Order-ID",i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_OrdID, sizeof(oCond2.m_OrdID), sTmp.c_str());
		
		if (!pIn->addServPassWd(oCond2)) {
			__DEBUG_LOG0_(0, "parserRandPswdR: addServPassWd error.");
			return -1;
		}	
	}
	
	return 0;
}

//打包
int packRandPaswA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
	vector<QryPasswordData *> v1;
	int iRet;
	ACE_UINT32 U32Var;
	string sTmp;
	
	if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packRandPaswA:传入的指针有空指针.");
	    return -1;
	}
	
	QryPasswordCCA *pIn = (QryPasswordCCA *)poCCA;
	iRet = pIn->getServPassWdData(v1);
	if (iRet <= 0) {
		__DEBUG_LOG0_(0, "packRandPaswA: getServPassWdData缺少业务处理结果码等信息");
	    return -1;
	}
	
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_SevResCode);
	sTmp.assign(v1[0]->ResID);
	poDcc->setAvpValue("Service-Information.Response-Id", sTmp);
	U32Var = v1[0]->ResTime;
	poDcc->setAvpValue("Service-Information.Response-Time", U32Var);
	
	return 0;
}


/*****************************************************
 *
 *    3.4.4.1 开户充值 VC规范2.1  张成
 *    开户充值CCR    
 *
 *****************************************************/
//解包
int DepositServCondOneR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    DepositServCondOne oCond;
	DepositServCondTwo oCondT;
    string sTmp;
    ACE_UINT32 U32Var;
    time_t nowtime;
    struct tm *ptm;
    DepositInfoCCR *pIn = (DepositInfoCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	poDcc->getAvpValue("Service-Information.VC-Information.Req-Serial", sTmp);
	snprintf(oCond.m_sReqSerial, sizeof(oCond.m_sReqSerial), sTmp.c_str());
	
	//poDcc->getAvpValue("Service-Information.VC-Information.Balance-Type-VC", sTmp);
	//snprintf(oCond.m_sBalanceType, sizeof(oCond.m_sBalanceType), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Balance-Type", iTmp);
	oCond.m_iBalanceType = iTmp;
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Balance-Unit", U32Var);
	oCond.m_iDestinationBalanceUnit = U32Var;
	
	poDcc->getAvpValue("Service-Information.VC-Information.Voucher-Value", U32Var);
	oCond.m_iVoucherValue = U32Var;
	
	poDcc->getAvpValue("Service-Information.VC-Information.Prolong-Days", U32Var);
	oCond.m_iProlongDays = U32Var;
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Type-Vc", sTmp);
	snprintf(oCond.m_sDestinationType, sizeof(oCond.m_sDestinationType), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Account", sTmp);
	snprintf(oCond.m_sDestinationAccount, sizeof(oCond.m_sDestinationAccount), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Attr-VC", iTmp);
	oCond.m_iDestinationAttr = iTmp;
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Attr-Detail", U32Var);
	oCond.m_lDestinationAttrDetail = U32Var;
	
	poDcc->getAvpValue("Service-Information.VC-Information.AreaCode", sTmp);
	snprintf(oCond.m_sAreaCode, sizeof(oCond.m_sAreaCode), sTmp.c_str());

	if (!pIn->addDepositOneCCR(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addAllocateBalanceSvc error.");
	        return -1;
	    }
	iNums = poDcc->getGroupedAvpNums("Service-Information.VC-Information.Bonus-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Bonus-Unit", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
	    oCondT.m_iBonusUnit = U32Var;
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Bonus-Amount", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
	    oCondT.m_iBonusAmount = U32Var;
	    
	    //snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Balance-Type-VC", i);
	    //poDcc->getAvpValue(sTmpName, sTmp);
	    //snprintf(oCond.m_sBalanceType, sizeof(oCond.m_sBalanceType), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Balance-Type", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
	    oCondT.m_iBalanceType = U32Var;
		
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Bonus-Start-Time", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
	    oCondT.m_iBonusStartTime = U32Var;
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Bonus-Info[%d].Bonus-Expiration-Time", i);
	    poDcc->getAvpValue(sTmpName, U32Var);
	    oCondT.m_iBonusExpirationTime = U32Var;
	    
	    if (!pIn->addDepositTwoCCR(oCondT)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addDepositTwoCCR error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int DepositServCondOneA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResponsDepositOne *> v1;
    vector<ResponsDepositTwo *> v2;
    int iRet;
	ACE_UINT32 U32Var;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	DepositInfoCCA *pIn = (DepositInfoCCA *)poCCA;
	iRet = pIn->getRespDepositOneCCA(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespDepositOneCCA缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getRespDepositTwoCCA(v1[0], v2);
	if (iRet < 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespDepositTwoCCA 业务处理具体信息解析失败");
	    return -1;
	}
	
	//dcc打包
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Req-Serial", 0);
	sTmp.assign(v1[0]->m_sReqSerial);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	poDcc->setAvpValue("Service-Information.VC-Information.Result", v1[0]->m_iRltCode);
	
	for (int i=0; i<v2.size(); i++) {
	
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Destination-Balance", i);
        poDcc->setAvpValue(sTmpName, v2[i]->m_iDestinationBalance);
	   // poDcc->setAvpValue("Service-Information.VC-Information.Balance-Info[%d].Destination-Balance", v2[i]->m_iDestinationBalance);
		
		//snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Balance-Type-VC", i);
	    //sTmp.assign(v2[i]->m_sBalanceType);
	    //poDcc->setAvpValue(sTmpName, sTmp);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Balance-Type", i);
	    U32Var = v2[i]->m_iBalanceType;
	    poDcc->setAvpValue(sTmpName, U32Var);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Destination-Balance-Unit", i);
		U32Var = v2[i]->m_iDestinationBalanceUnit;
        poDcc->setAvpValue(sTmpName, U32Var); 
		//poDcc->setAvpValue("Service-Information.VC-Information.Balance-Info[%d].Destination-Balance-Unit", v2[i]->m_iDestinationBalanceUnit);
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Destination-Effective-Time", i);
		U32Var = v2[i]->m_iDestinationEffectiveTime;
        poDcc->setAvpValue(sTmpName, U32Var); 
		//poDcc->setAvpValue("Service-Information.VC-Information.Balance-Info[%d].Destination-Effective-Time", v2[i]->m_iDestinationEffectiveTime);
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.VC-Information.Balance-Info[%d].Destination-Expiration-Time", i);
		U32Var = v2[i]->m_iDestinationExpirationTime;
        poDcc->setAvpValue(sTmpName, U32Var); 
		//poDcc->setAvpValue("Service-Information.VC-Information.Balance-Info[%d].Destination-Expiration-Time", v2[i]->m_iDestinationExpirationTime);
	}
	
	return 0;
}


/*****************************************************
 *
 *             3.4.5.1 余额被动划拨
 *
 *****************************************************/
//解包
int AllocateBalanceSvcR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
	ui64 iTmp64;
    char sTmpName[128];
    //AbmPayCond oCond;
    R_AllocateBalanceSvc oCond;
    R_RequestDebitAcctItem oRCond;
    string sTmp;
	time_t nowtime;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    AllocateBalanceCCR *pIn = (AllocateBalanceCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	  }
	
	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Destination-Id", sTmp);
	snprintf(oCond.m_servNbr, sizeof(oCond.m_servNbr), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Destination-Id-Type", iTmp);
	oCond.m_nbrType = iTmp;
	
	poDcc->getAvpValue("Service-Information.Destination-Attr", iTmp);
	oCond.m_servAttr = iTmp;
	
	poDcc->getAvpValue("Service-Information.AreaCode", sTmp);
	snprintf(oCond.m_areaCode, sizeof(oCond.m_areaCode), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Staff-ID", iTmp);
	oCond.m_staffId = iTmp;
	
	poDcc->getAvpValue("Service-Information.Deduct-Date", U32Var);
	oCond.m_iDeductDate = U32Var;
	/**
	iTmp64 -= SECS_70_YEARS;
	nowtime = (time_t)iTmp64;
	ptm = localtime(&nowtime);
	snprintf(oCond.m_deductDate, sizeof(oCond.m_deductDate), "%04d%02d%02d%02d%02d%02d",
	                                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                                        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    ***/
	
	poDcc->getAvpValue("Service-Information.ABM-Id", sTmp);
	snprintf(oCond.m_abmId, sizeof(oCond.m_abmId), sTmp.c_str());
	
	if (!pIn->addAllocateBalanceSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addAllocateBalanceSvc error.");
	        return -1;
	    }
	iNums = poDcc->getGroupedAvpNums("Service-Information.Request-Debit-Acct-Item");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oRCond, 0, sizeof(oRCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Billing-Cycle-ID", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_billingCycle = iTmp;
	    //snprintf(oRCond.m_billingCycle, sizeof(oRCond.m_billingCycle), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Item-Type-Id", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oRCond.m_acctItemTypeId, sizeof(oRCond.m_acctItemTypeId), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Balance-Unit", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_balanceUnit = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Deduct-Amount", i);
	    poDcc->getAvpValue(sTmpName, iTmp64);
	    oRCond.m_deductAmount = iTmp64;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Charge-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_chargeType = iTmp;
	    
	    if (!pIn->addRequestDebitAcctItem(oRCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addRequestDebitAcctItem error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int AllocateBalanceSvcA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<A_AllocateBalanceSvc *> v1;
    vector<A_ResponseDebitAcctItem *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	AllocateBalanceCCA *pIn = (AllocateBalanceCCA *)poCCA;
	iRet = pIn->getResBalanceSvcData(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getAcctBalanceData(v1[0], v2);
	/*****
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getAcctBalanceData 业务处理具体信息解析失败");
	    return -1;
	}
	********/
	
	//dcc打包
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Response-Id", 0);
	sTmp.assign(v1[0]->m_responseId);
    poDcc->setAvpValue(sTmpName, sTmp);
    
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_svcResultCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
	
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Accout-Id", 0);
    ui64 var = v1[0]->m_lAcctId;
    poDcc->setAvpValue(sTmpName, var);
 
    //poDcc->setAvpValue("Service-Information.Accout-Id", v1[0]->m_lAcctId);
    
    poDcc->setAvpValue("Service-Information.Balance-Unit", v1[0]->m_balanceUnit);
	
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Accout-Id", 0);
    var = v1[0]->m_lAcctId;
    poDcc->setAvpValue(sTmpName, var);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Unit", 0);
    var = v1[0]->m_balanceUnit;
    poDcc->setAvpValue(sTmpName, var);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Deduct-Amount", 0);
    var = v1[0]->m_deductAmount;
    poDcc->setAvpValue(sTmpName, var);
    //poDcc->setAvpValue("Service-Information.Deduct-Amount", v1[0]->m_deductAmount);
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Available", 0);
    var = v1[0]->m_acctBalance;
    poDcc->setAvpValue(sTmpName, var);
    //poDcc->setAvpValue("Service-Information.Balance-Amount ", v1[0]->m_acctBalance);
	
	for (int i=0; i<v2.size(); i++) {
		 poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[%d].Billing-Cycle-ID", v2[i]->m_billingCycle);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Item-Type-Id", i);
		 sTmp.assign(v2[i]->m_acctItemTypeId);
		 poDcc->setAvpValue(sTmpName, sTmp);
		 
		// poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Id", v2[i]->m_acctBalanceId);
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Id", i);
		 var = v2[i]->m_acctBalanceId;
        poDcc->setAvpValue(sTmpName, var);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Deduct-Amount", i);
		 var = v2[i]->m_deductAmount;
        poDcc->setAvpValue(sTmpName, var);
		
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Amount", i);
		var = v2[i]->m_acctBalanceAmount;
        poDcc->setAvpValue(sTmpName, var);
		// poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance–deduct-Amount", v2[i]->m_deductAmount);
		// poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Amount", v2[i]->m_acctBalanceAmount);
		 
	}
	
	return 0;
}

//lijianchen 20110530
//3.4.5.1 --全国中心abm主动发出
int packDeductBalR(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    int iNums;
    int iTmp;
    string sTmp;
    ACE_UINT32 U32Var;
    struct tm tmVar;
    time_t nowtime;
    char sTmpName[128];
    ui64 iTmp64;
    vector<R_AllocateBalanceSvc *> v1;
    vector<R_RequestDebitAcctItem *> v2;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packDeductBalR:传入的指针有空指针.");
	    return -1;
	}
	
	AllocateBalanceCCR *pIn = (AllocateBalanceCCR *)poCCR;
	
	iNums = pIn->getR_AllocateBalanceSvc(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packDeductBalR: getR_AllocateBalanceSvc缺少相关的请求信息");
	    return -1;
	}
	
	sTmp.assign(v1[0]->m_requestId);
	poDcc->setAvpValue("Service-Information.Request-Id", sTmp);
	
	sTmp.assign(v1[0]->m_servNbr);
	poDcc->setAvpValue("Service-Information.Destination-Id", sTmp);
	
	iTmp = v1[0]->m_nbrType;
	poDcc->setAvpValue("Service-Information.Destination-Id-Type", iTmp);
	
	iTmp = v1[0]->m_servAttr;
	poDcc->setAvpValue("Service-Information.Destination-Attr", iTmp);
	
	sTmp.assign(v1[0]->m_areaCode);
	poDcc->setAvpValue("Service-Information.AreaCode", sTmp);
	
	U32Var = v1[0]->m_staffId;
	poDcc->setAvpValue("Service-Information.Staff-ID", U32Var);
	/***
	sscanf(v1[0]->m_deductDate, "%04d%02d%02d%02d%02d%02d", 
	                     &tmVar.tm_year, &tmVar.tm_mon, &tmVar.tm_mday,
	                     &tmVar.tm_hour, &tmVar.tm_min, &tmVar.tm_sec);
    tmVar.tm_year -= 1900;
    tmVar.tm_mon -= 1;
    nowtime = mktime(&tmVar);
    U32Var = nowtime + SECS_70_YEARS;
	***/
	U32Var = v1[0]->m_iDeductDate;
	poDcc->setAvpValue("Service-Information.Deduct-Date", U32Var);
	
	sTmp.assign(v1[0]->m_abmId);
	poDcc->setAvpValue("Service-Information.ABM-Id", sTmp);
	
	iNums = pIn->getR_RequestDebitAcctItem(v1[0], v2);
	for (int i=0; i<iNums; i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Billing-Cycle-ID", i);
	    iTmp = v2[i]->m_billingCycle;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Item-Type-Id", i);
	    sTmp.assign(v2[i]->m_acctItemTypeId);
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Balance-Unit", i);
	    U32Var = v2[i]->m_balanceUnit;
	    poDcc->setAvpValue(sTmpName, U32Var);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Deduct-Amount", i);
	    iTmp64 = v2[i]->m_deductAmount;
	    poDcc->setAvpValue(sTmpName, iTmp64);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Charge-Type", i);
	    iTmp = v2[i]->m_chargeType;
	    poDcc->setAvpValue(sTmpName, iTmp);
	}
	
	return 0;
}
int parserDeductBalA(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    A_AllocateBalanceSvc oCond1;
    A_ResponseDebitAcctItem oCond2;
    ui64 var;
    int iRet, iNums;
    string sTmp;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserDeductBalA:传入的指针有空指针.");
	    return -1;
	}
	
	AllocateBalanceCCA *pIn = (AllocateBalanceCCA *)poCCA;
	
	memset((void *)&oCond1, 0, sizeof(oCond1));
	
    poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
    snprintf(oCond1.m_responseId, sizeof(oCond1.m_responseId), sTmp.c_str());
    
    poDcc->getAvpValue("Service-Information.Service-Result-Code", oCond1.m_svcResultCode);
	
    poDcc->getAvpValue("Service-Information.Accout-Id", var);
    oCond1.m_lAcctId = var;
    
    poDcc->getAvpValue("Service-Information.Balance-Unit", oCond1.m_balanceUnit);

    poDcc->getAvpValue("Service-Information.Deduct-Amount", var);
    oCond1.m_deductAmount = var;
	poDcc->getAvpValue("Service-Information.Balance-Available", var);
    oCond1.m_acctBalance = var;
    
    if (!pIn->addResBalanceSvcData(oCond1)) {
        __DEBUG_LOG0_(0, "parserDeductBalA: addResBalanceSvcData error.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Request-Debit-Acct-Item");
	for (int i=0; i<iNums; i++) {
	    memset((void *)&oCond2, 0, sizeof(oCond2));
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Billing-Cycle-ID", i);
	    poDcc->getAvpValue(sTmpName, oCond2.m_billingCycle);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Item-Type-Id", i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_acctItemTypeId, sizeof(oCond2.m_acctItemTypeId), sTmp.c_str());
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Id", i); 
		poDcc->getAvpValue(sTmpName, oCond2.m_acctBalanceId);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Deduct-Amount", i);
        poDcc->getAvpValue(sTmpName, var);
        oCond2.m_deductAmount = var;
        
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Debit-Acct-Item[%d].Acct-Balance-Amount", i);
        poDcc->getAvpValue(sTmpName, var);
        oCond2.m_acctBalanceAmount  = var;
        
        if (!pIn->addResDebitAcctData(oCond2)) {
            __DEBUG_LOG0_(0, "parserDeductBalA: addResDebitAcctData error.");
	        return -1;
        }
    }
    
    return 0;
}
//lijianchen 20110530

/*****************************************************
 *
 *                3.4.5.5  余额划拨冲正
 *
 *****************************************************/
 //解包
int ReqReverseDeductSvcR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    //AbmPayCond oCond;
    ReqReverseDeductSvc oCond;
    string sTmp;
	time_t nowtime;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    ReverseDeductBalanceCCR *pIn = (ReverseDeductBalanceCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
		    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Id");
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sRequestId, sizeof(oCond.m_sRequestId), sTmp.c_str());
		
	    poDcc->getAvpValue("Service-Information.Request-Time", iTmp);
		oCond.m_iRequestTime = iTmp; 
		/***
	    iTmp -= SECS_70_YEARS;
	    nowtime = (time_t)iTmp;
	    ptm = localtime(&nowtime);
	    snprintf(oCond.m_sRequestTime, sizeof(oCond.m_sRequestTime), "%04d%02d%02d%02d%02d%02d",
	                                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                                        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		*****/
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Old-Request-Id");
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOldRequestId, sizeof(oCond.m_sOldRequestId), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.ABM-Id");
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sABMId, sizeof(oCond.m_sABMId), sTmp.c_str());
		
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Action-Type");
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iActionType = iTmp;
	    
	    if (!pIn->addReqReverseDeductSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addReqReverseDeductSvc error.");
	        return -1;
	    }
	    
    return 0;   
}

//打包
int ReqReverseDeductSvcA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResReverseDeductData *> v1;
    //vector<ResRechargeBillQuery *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	ReverseDeductBalanceCCA *pIn = (ReverseDeductBalanceCCA *)poCCA;
	iRet = pIn->getResReverseDeductData(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	/***
	iRet = pIn->getBil(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getBil 业务处理具体信息解析失败");
	    return -1;
	}
	***/
	
	//dcc打包
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Response-Id", 0);
	sTmp.assign(v1[0]->m_sResponseId);
    poDcc->setAvpValue(sTmpName, sTmp);

    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Service-Result-Code");
	poDcc->setAvpValue(sTmpName, v1[0]->m_iServiceRltCode);

    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Response-Time");
	poDcc->setAvpValue(sTmpName, v1[0]->m_iResponseTime);
	
/*****	
	poDcc->setAvpValue("Service-Information.Operate-Seq", v1[0]->m_uiRltCode);
	
	for (int i=0; i<v2.size(); i++) {
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].PayAmount", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_lPayMount);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Balance", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_iBalance);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Last-Balance", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_iLastBalance);	 
	}
****/
	return 0;
}

//lijianchen 20110530
//3.4.5.5--全国中心abm主动发出
int packRvsChargeR(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    int iNums;
    struct tm tmVar;
    time_t nowtime;
    ACE_UINT32 U32Var;
    string sTmp;
    int iTmp;
    vector<ReqReverseDeductSvc *> v1;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packRvsChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	ReverseDeductBalanceCCR *pIn = (ReverseDeductBalanceCCR *)poCCR;
	
	iNums = pIn->getReqReverseDeductSvc(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packRvsChargeR: getReqReverseDeductSvc缺少相关的请求信息");
	    return -1;
	}
	    
	sTmp.assign(v1[0]->m_sRequestId);
	poDcc->setAvpValue("Service-Information.Request-Id", sTmp);
	
    U32Var = v1[0]->m_iRequestTime;
	poDcc->setAvpValue("Service-Information.Request-Time", U32Var);
	
	sTmp.assign(v1[0]->m_sOldRequestId);
	poDcc->setAvpValue("Service-Information.Old-Request-Id", sTmp);
	
	sTmp.assign(v1[0]->m_sABMId);
	poDcc->setAvpValue("Service-Information.ABM-Id", sTmp);	
	
	iTmp = v1[0]->m_iActionType;
	poDcc->setAvpValue("Service-Information.Action-Type", iTmp);
	
    return 0;  
}

int parserRvsChargeA(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    ResReverseDeductData oCond1;
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserRvsChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	ReverseDeductBalanceCCA *pIn = (ReverseDeductBalanceCCA *)poCCA;
	
	memset((void *)&oCond1, 0, sizeof(oCond1));
	
    poDcc->getAvpValue("Service-Information.Response-Id", sTmp);
    snprintf(oCond1.m_sResponseId, sizeof(oCond1.m_sResponseId), sTmp.c_str());

	poDcc->getAvpValue("Service-Information.Service-Result-Code", oCond1.m_iServiceRltCode);
		 
    poDcc->getAvpValue("Service-Information.Response-Time", oCond1.m_iResponseTime);
    //snprintf(oCond1.m_sResponseTime, sizeof(oCond1.m_sResponseTime), sTmp.c_str());
    
    if (!pIn->addResReverseDeductData(oCond1)) {
        __DEBUG_LOG0_(0, "parserRvsChargeA: addResReverseDeductData error.");
	    return -1;
	}
	
	return 0;
	
}
//lijianchen 20110530

/*****************************************************
 *
 *                  3.4.6.16 电子帐单查询
 *
 *****************************************************/
//解包
int RechargeBillQueryR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    //AbmPayCond oCond;
    RechargeBillQuery oCond;
    string sTmp;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    HssRechargeBillQueryCCR *pIn = (HssRechargeBillQueryCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOperSeq, sizeof(oCond.m_sOperSeq), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDestAcct, sizeof(oCond.m_sDestAcct), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestType = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestAttr = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Billing-Cycle-ID", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
		oCond.m_uiBilCycID = iTmp;
	    
	    if (!pIn->addBil(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addBil error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int RechargeBillQueryA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResBilCode *> v1;
    vector<ResRechargeBillQuery *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
	ui64 iTmp64;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	HssRechargeBillQueryCCA *pIn = (HssRechargeBillQueryCCA *)poCCA;
	iRet = pIn->getRespons(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getBil(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getBil 业务处理具体信息解析失败");
	    return -1;
	}
	
	//dcc打包
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_uiRltCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Operate-Seq", 0);
	sTmp.assign(v1[0]->m_sSeq);
    poDcc->setAvpValue(sTmpName, sTmp);
  
	
	for (int i=0; i<v2.size(); i++) {
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].PayAmount", i);
		 iTmp64 = v2[i]->m_lPayMount;
		 poDcc->setAvpValue(sTmpName, iTmp64);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Balance", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_iBalance);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Last-Balance", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_iLastBalance);	 
	}
	
	return 0;
}


/*****************************************************
 *
 *                  3.4.6.15 交易记录查询
 *
 *****************************************************/
//解包
int QueryPayLogByAccoutR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    //AbmPayCond oCond;
    QueryPayLogByAccout oCond;
    string sTmp;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    HssQueryPayLogByAccoutCCR *pIn = (HssQueryPayLogByAccoutCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Action", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_uiOperAct = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOperSeq, sizeof(oCond.m_sOperSeq), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].SP-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSPID, sizeof(oCond.m_sSPID), sTmp.c_str());

		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].SP-NAME", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSPName, sizeof(oCond.m_sSPName), sTmp.c_str());

		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSrvPlatID, sizeof(oCond.m_sSrvPlatID), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDestAcct, sizeof(oCond.m_sDestAcct), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestType = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestAttr = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Query-Start-Time", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_uiStartTime);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Query-Expiration-Time", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_uiExpTime);
	    
	    if (!pIn->addPay(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int QueryPayLogByAccoutA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResPayCode *> v1;
    vector<ResQueryPayLogByAccout *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	HssQueryPayLogByAccoutCCA *pIn = (HssQueryPayLogByAccoutCCA *)poCCA;
	iRet = pIn->getRespons(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getPay(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
    //dcc打包
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_uiRltCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Operate-Seq", 0);
	sTmp.assign(v1[0]->m_sSeq);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	for (int i=0; i<v2.size(); i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Order-ID", i);
		 sTmp.assign(v2[i]->m_sOderID);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Description", i);
		 sTmp.assign(v2[i]->m_sDesc);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].SP-ID", i);
		 sTmp.assign(v2[i]->m_sSPID);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Service-Platform-ID", i);
		 sTmp.assign(v2[i]->m_sSrvPlatID);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		 //snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Pay_Date", i);
		 //poDcc->setAvpValue(sTmpName, v2[i]->m_uiPayDate);
		 
		 snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Pay-Date", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_uiPayDate);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Status", i);
		sTmp.assign(v2[i]->m_sStatus);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].Micropay-Type", i);
		sTmp.assign(v2[i]->m_sMicropayType);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Order-Info[%d].PayAmount", i);
		 poDcc->setAvpValue(sTmpName, v2[i]->m_lPayAmount);

	 
	}
	
	return 0;
}


/*****************************************************
 *
 *                 3.4.6.14 充退记录查询
 *
 *****************************************************/
//解包
int RechargeRecQueryR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    //AbmPayCond oCond;
    RechargeRecQuery oCond;
    string sTmp;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    HssRechargeRecQueryCCR *pIn = (HssRechargeRecQueryCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sOperSeq, sizeof(oCond.m_sOperSeq), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDestAcct, sizeof(oCond.m_sDestAcct), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestType = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestAttr = iTmp;
	    
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sSrvPlatID, sizeof(oCond.m_sSrvPlatID), sTmp.c_str());
		
		
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Query-Start-Time", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_uiStartTime);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Query-Expiration-Time", i);
	    poDcc->getAvpValue(sTmpName, oCond.m_uiExpTime);
	    
	    if (!pIn->addRec(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int RechargeRecQueryA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResRecCode *> v1;
    vector<ResRechargeRecQuery *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	HssRechargeRecQueryCCA *pIn = (HssRechargeRecQueryCCA *)poCCA;
	iRet = pIn->getRespons(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getRec(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	
	//dcc打包
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_uiRltCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.Operate-Seq", 0);
	sTmp.assign(v1[0]->m_sSeq);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	for (int i=0; i<v2.size(); i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Result", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiResult);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Operate-Action", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiOperAct);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].RechargeTime", i);
		 sTmp.assign(v2[i]->m_sRechargeTime);
	     poDcc->setAvpValue(sTmpName, sTmp);
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Destination-Account", i);
		 sTmp.assign(v2[i]->m_sDestAcct);
	     poDcc->setAvpValue(sTmpName, sTmp);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Destination-Attr", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_iDestAttr);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Service-Platform-ID", i);
		 sTmp.assign(v2[i]->m_sSrvPlatID);
	     poDcc->setAvpValue(sTmpName, sTmp);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].Charge-Type", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiChargeType);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info.FillRecord-Info[%d].RechargeAmount", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiRechargeAmount);

	 
	}
	
	return 0;
}


/*****************************************************
 *
 *                3.4.6.4 余额划拨查询
 *
 *****************************************************/
 //--- abm中心收到外围请求
int parserQryBalDrawR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    QueryAllPay oCond1;
    string sTmp;
    ACE_UINT32 U32Var;
    int iRet;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserQryBalDrawR:传入的指针有空指针.");
	    return -1;
	}
	
	HssPaymentQueryCCR *pIn = (HssPaymentQueryCCR *)poCCR;
	
	memset((void *)&oCond1, 0, sizeof(oCond1));
	
	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	snprintf(oCond1.m_sReqID, sizeof(oCond1.m_sReqID), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Request-Time", U32Var);
	oCond1.m_uiReqTime = U32Var;
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Destination-Account", sTmp);
	snprintf(oCond1.m_sDestAcct, sizeof(oCond1.m_sDestAcct), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Destination-Type", oCond1.m_iDestType);
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Destination-Attr", oCond1.m_iDestAttr);
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Service-Platform-ID", sTmp);
	snprintf(oCond1.m_sServPlatID, sizeof(oCond1.m_sServPlatID), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Query-Flag", oCond1.m_iQueryFlg);
	
	poDcc->getAvpValue("Service-Information.Balance-Information.Query-Item-Type", oCond1.m_iQueryItemType);
	
	if (!pIn->addReq(oCond1)) {
	    __DEBUG_LOG0_(0, "parserQryBalDrawR: addReq error.");
	    return -1;
	}
	
	return 0;
}

int packQryBalDrawA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<MicroPayRlt *> v1;
    vector<MicroPaySrv *> v2;
    int iRet;
    ACE_UINT64 U64Var;
    string sTmp;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packQryBalDrawA:传入的指针有空指针.");
	    return -1;
	}
	
	HssResMicroPayCCA *pIn = (HssResMicroPayCCA *)poCCA;
	
	iRet = pIn->getRlt(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packQryBalDrawA: getRlt缺少业务处理结果码等信息");
	    return -1;
	}
	poDcc->setAvpValue("Service-Information.Service_Result_Code", v1[0]->m_uiRltCode);
	
	sTmp.assign(v1[0]->m_sResID);
	poDcc->setAvpValue("Service-Information.Response-Id", sTmp);
	
	poDcc->setAvpValue("Service-Information.Response_Time", v1[0]->m_uiResTime);
	
	iRet = pIn->getSrv(v1[0], v2);
	for (int i=0; i<iRet; i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].Balance-Source", i);
        poDcc->setAvpValue(sTmpName, v2[i]->m_iBalSrc);
        
        snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].UnitType-Id", i);
        poDcc->setAvpValue(sTmpName, v2[i]->m_iUnitTypID);
        
        snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].Balance-Amount", i);
        U64Var = v2[i]->m_lBalAmount;
        poDcc->setAvpValue(sTmpName, U64Var);
    }
        
	return 0;
}
/*****************************************************
 *
 *                3.4.5.3 余额查询
 *
 *****************************************************/

//--- abm中心主动发起
int parserQryBalDrawA(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    MicroPayRlt oCond1;
    MicroPaySrv oCond2;
    int iNums;
    string sTmp;
    ACE_UINT64 U64Var;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserQryBalDrawA:传入的指针有空指针.");
	    return -1;
	}
	
	HssResMicroPayCCA *pIn = (HssResMicroPayCCA *)poCCA;
	
	memset((void *)&oCond1, 0, sizeof(oCond1));
	
	poDcc->getAvpValue("Service-Information.Service_Result_Code", oCond1.m_uiRltCode);
	
	poDcc->getAvpValue("Service-Information.Response-Id", sTmp);
	snprintf(oCond1.m_sResID, sizeof(oCond1.m_sResID), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Response_Time", oCond1.m_uiResTime);
	
	if (!pIn->addRlt(oCond1)) {
	    __DEBUG_LOG0_(0, "parserQryBalDrawA: addRlt error.");
	    return -1;
	}
	
	//解包第二部分
	iNums = poDcc->getGroupedAvpNums("Service_Information.Balance-Information.Balance-Item-Detail");
	for (int i=0; i<iNums; i++) {
	    memset((void *)&oCond2, 0, sizeof(oCond2));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].Balance-Source", i);
        poDcc->getAvpValue(sTmpName, oCond2.m_iBalSrc);
        
        snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].UnitType-Id", i);
        poDcc->setAvpValue(sTmpName, oCond2.m_iUnitTypID);
        
        snprintf(sTmpName, sizeof(sTmpName), "Service_Information.Balance-Information.",
	                                  "Balance-Item-Detail[%d].Balance-Amount", i);
        poDcc->setAvpValue(sTmpName, U64Var);
        oCond2.m_lBalAmount = U64Var;
        
        if (!pIn->addSrv(oCond2)) {
            __DEBUG_LOG0_(0, "parserQryBalDrawA: addSrv error.");
	        return -1;
        }
    }
	    
	return 0;
}

int packQryBalDrawR(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    vector<QueryAllPay *> v1;
    int iNums;
    string sTmp;
    ACE_UINT32 U32Var;
    
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packQryBalDrawR:传入的指针有空指针.");
	    return -1;
	}
	
	HssPaymentQueryCCR *pIn = (HssPaymentQueryCCR *)poCCR;
	iNums = pIn->getReq(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packQryBalDrawR: getReq缺少相关的请求信息");
	    return -1;
	}
	
	sTmp.assign(v1[0]->m_sReqID);
	poDcc->setAvpValue("Service-Information.Request-Id", sTmp);
	
	U32Var = v1[0]->m_uiReqTime;
	poDcc->setAvpValue("Service-Information.Request-Time", U32Var);
	
	sTmp.assign(v1[0]->m_sDestAcct);
	poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Account", sTmp);
	
	poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Type", v1[0]->m_iDestType);
	
	poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Attr", v1[0]->m_iDestAttr);
	
	sTmp.assign(v1[0]->m_sServPlatID);
	poDcc->setAvpValue("Service-Information.Balance-Information.Service-Platform-ID", sTmp);
	
	poDcc->setAvpValue("Service-Information.Balance-Information.Query-Flag", v1[0]->m_iQueryFlg);
	
	poDcc->setAvpValue("Service-Information.Balance-Information.Query-Item-Type", v1[0]->m_iQueryItemType);
	
	return 0;
}



/*****************************************************
 *
 *   3.4.6.13, 3.4.5.4 余额查询
 *
 *****************************************************/
//解包
int QueryBalanceR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    //AbmPayCond oCond;
    QueryBalance oCond;
    string sTmp;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    HssQueryBalanceCCR *pIn = (HssQueryBalanceCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Balance-Information");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond, 0, sizeof(oCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Id", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sDestID, sizeof(oCond.m_sDestID), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Id-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestIDType = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iDestAttr = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].AreaCode", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond.m_sAreaCode, sizeof(oCond.m_sAreaCode), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Query-Flag", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iQueryFlg = iTmp;
	    
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Query-Item-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oCond.m_iQueryItemTyp = iTmp;
	    
	    if (!pIn->addReqBal(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int QueryBalanceA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResBalanceCode *> v1;
    vector<ResQueryBalance *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	HssQueryBalanceCCA *pIn = (HssQueryBalanceCCA *)poCCA;
	iRet = pIn->getResPons(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	iRet = pIn->getResBal(v1[0], v2);
	/***
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getPayData 业务处理具体信息解析失败");
	    return -1;
	}
	***/
	
	//dcc打包
	/***
	sTmp.assign(v1[0]->m_uiRltCode);
	poDcc->setAvpValue("Service-Information.Service-Result-Code", sTmp);
	***/
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_uiRltCode);
	
	ui64 var = v1[0]->m_lTotalBalAvail;
	poDcc->setAvpValue("Service-Information.Total-Balance-Available", var);
	
	sTmp.assign(v1[0]->m_sParaFieldResult);
	poDcc->setAvpValue("Service-Information.Para-Field-Result", sTmp);
	
	for (int i=0; i<v2.size(); i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-ItemType-Detail", i);
	    sTmp.assign(v2[i]->m_sBalItmDetail);
		poDcc->setAvpValue(sTmpName, sTmp);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].UnitType-Id", i);
		poDcc->setAvpValue(sTmpName, v2[i]->m_iUnitTypID);

		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Amount", i);
		var = v2[i]->m_lBalAmount;
        poDcc->setAvpValue(sTmpName, var);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Available", i);
		var = v2[i]->m_lBalAvailAble;
        poDcc->setAvpValue(sTmpName, var);
		 
		//snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Owe-Used", i);
		//var = v2[i]->m_lBalanceOweUsed;
        //poDcc->setAvpValue(sTmpName, var);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Used", i);
		var = v2[i]->m_lBalUsed;
        poDcc->setAvpValue(sTmpName, var);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Reserved", i);
		var = v2[i]->m_lBalReserved;
        poDcc->setAvpValue(sTmpName, var);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Eff-Date", i);
		//晓哲
		//ui32 var32 = 2023232;
        //poDcc->setAvpValue(sTmpName, var32);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiEffDate);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Exp-Date", i);
		//var32 = 2023232;
        //poDcc->setAvpValue(sTmpName, var32);
		poDcc->setAvpValue(sTmpName, v2[i]->m_uiExpDate);
	 
	}
	
	return 0;
}

//lijianchen 20110530
//3.4.6.13,  3.4.5.3,  3.4.5.4 余额查询 --- abm中心主动发起
int packQryBalanceR(DccMsgParser *poDcc, ABMCCA *poCCR,char *serNUMCCR)
{
    int iNums;
    vector<QueryBalance *> v1;
    string sTmp;
    int iTmp;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packQryBalanceR:传入的指针有空指针.");
	    return -1;
	}
	
	HssQueryBalanceCCR *pIn = (HssQueryBalanceCCR *)poCCR;
	iNums = pIn->getReqBal(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packQryBalanceR: getReqBal缺少相关的请求信息");
	    return -1;
	}
	
	for (int i=0; i<iNums; i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Id", i);
	    sTmp.assign(v1[i]->m_sDestID);
	    poDcc->setAvpValue(sTmpName, sTmp);
		strcpy(serNUMCCR, v1[i]->m_sDestID); 
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Id-Type", i);
	    iTmp = v1[i]->m_iDestIDType;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Destination-Attr", i);
	    iTmp = v1[i]->m_iDestAttr;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].AreaCode", i);
	    sTmp.assign(v1[i]->m_sAreaCode);
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Query-Flag", i);
	    iTmp = v1[i]->m_iQueryFlg;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information[%d].Query-Item-Type", i);
	    iTmp = v1[i]->m_iQueryItemTyp;
	    poDcc->setAvpValue(sTmpName, iTmp);
	}
	
	return 0;
}
int parserQryBalanceA(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    int iRet, iTmp, iNums;
    string sTmp;
    ui64 var;
	ACE_UINT32 U32Var;
    char sTmpName[128];
    ResBalanceCode oCond1;
    ResQueryBalance oCond2;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserQryBalanceA:传入的指针有空指针.");
	    return -1;
	}
	
	HssQueryBalanceCCA *pIn = (HssQueryBalanceCCA *)poCCA;
	memset((void *)&oCond1, 0, sizeof(oCond1));
	poDcc->getAvpValue("Service-Information.Service-Result-Code", U32Var);
	oCond1.m_uiRltCode = U32Var;
	poDcc->getAvpValue("Service-Information.Total-Balance-Available", var);
	oCond1.m_lTotalBalAvail = var;
	/**
	poDcc->getAvpValue("Service-Information.Service-Result-Code", oCond1.m_uiRltCode);
	poDcc->getAvpValue("Service-Information.Total-Balance-Available", oCond1.m_lTotalBalAvail);
	**/
	if (!pIn->addRespons(oCond1)) {
	    __DEBUG_LOG0_(0, "parserQryBalanceA: addRespons error.");
	    return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Balance-Information.Balance-Item-Detail");
	for (int i=0; i<iNums; i++) {
	    memset((void *)&oCond2, 0, sizeof(oCond2));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-ItemType-Detail", i);
		poDcc->getAvpValue(sTmpName, sTmp);
		snprintf(oCond2.m_sBalItmDetail, sizeof(oCond2.m_sBalItmDetail), sTmp.c_str());
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].UnitType-Id", i);
		poDcc->getAvpValue(sTmpName, oCond2.m_iUnitTypID);

		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Amount", i);
        poDcc->getAvpValue(sTmpName, var);
		oCond2.m_lBalAmount = var;
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Available", i);
        poDcc->getAvpValue(sTmpName, var);
        oCond2.m_lBalAvailAble = var;
		 
		//snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Owe-Used", i);
        //poDcc->getAvpValue(sTmpName, var);
        //oCond2.m_lBalanceOweUsed = var;
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Used", i);
        poDcc->getAvpValue(sTmpName, var);
		oCond2.m_lBalUsed = var;
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Balance-Reserved", i);
        poDcc->getAvpValue(sTmpName, var);
        oCond2.m_lBalReserved = var;
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Eff-Date", i);
		poDcc->getAvpValue(sTmpName, U32Var);
		oCond2.m_uiEffDate = U32Var;
		//poDcc->getAvpValue(sTmpName, oCond2.m_uiEffDate);
		 
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Balance-Information.Balance-Item-Detail[%d].Exp-Date", i);
		poDcc->getAvpValue(sTmpName, U32Var);
		oCond2.m_uiExpDate =U32Var;
		//poDcc->getAvpValue(sTmpName, oCond2.m_uiExpDate);
	
	    if (!pIn->addResBal(oCond2)) {
	        __DEBUG_LOG0_(0, "parserQryBalanceA: addResBal error.");
	        return -1;
        }
    }
    
	return 0;
}


//lijianchen 20110530


/**********************
*
*        3.4.6.5  余额划拨
*
***********************/

//解包
int ReqPlatformSvcR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
	ui64 U64Var;
    char sTmpName[128];
    //AbmPayCond oCond;
    ReqPlatformSvc oCond;
    ReqPlatformPayInfo oRCond;
    string sTmp;
	time_t nowtime;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    PlatformDeductBalanceCCR *pIn = (PlatformDeductBalanceCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	  }
	
	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	snprintf(oCond.m_requestId, sizeof(oCond.m_requestId), sTmp.c_str());
	
	
	poDcc->getAvpValue("Service-Information.Request-Time", U32Var);
	oCond.m_iRequestTime = U32Var;
	/**
	iTmp64 -= SECS_70_YEARS;
	nowtime = (time_t)iTmp64;
	ptm = localtime(&nowtime);
	snprintf(oCond.m_requestTime, sizeof(oCond.m_requestTime), "%04d%02d%02d%02d%02d%02d",
	                                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                                        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	***/
	
	if (!pIn->addReqPlatformSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addAllocateBalanceSvc error.");
	        return -1;
	    }
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oRCond, 0, sizeof(oRCond));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oRCond.m_sDestinationAccount, sizeof(oRCond.m_sDestinationAccount), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_iDestinationType = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_iDestinationAttr = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oRCond.m_sSvcPlatformId, sizeof(oRCond.m_sSvcPlatformId), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Pay-Password", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oRCond.m_sPayPassword, sizeof(oRCond.m_sPayPassword), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Balance-TransFlag", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_iBalanceTransFlag = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].UnitType-Id", i);
	    poDcc->getAvpValue(sTmpName, iTmp);
	    oRCond.m_iUnitTypeId = iTmp;
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Deduct-Amount", i);
	    poDcc->getAvpValue(sTmpName, U64Var);
	    oRCond.m_lDeductAmount = U64Var;
	    
	    
	    
	    if (!pIn->addReqPlatformPayInfo(oRCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addRequestDebitAcctItem error.");
	        return -1;
	    }
	}
	    
    return 0;   
}


//打包
int ReqPlatformSvcA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<ResPlatformSvc *> v1;
    vector<ResPlatformAcctBalance *> v2;
    //vector<A_ResPlatformAcctBalance *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	PlatformDeductBalanceCCA *pIn = (PlatformDeductBalanceCCA *)poCCA;
	iRet = pIn->getResPlatformSvc(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	iRet = pIn->getResPlatformAcctBalance(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getAcctBalanceData 业务处理具体信息解析失败");
	    return -1;
	}
	
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_svcResultCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
	sTmp.assign(v1[0]->m_sParaFieldResult);
    poDcc->setAvpValue(sTmpName, sTmp);
    
   	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Response-Id", 0);
	sTmp.assign(v1[0]->m_responseId);
    poDcc->setAvpValue(sTmpName, sTmp);
    
    poDcc->setAvpValue("Service-Information.Response-Time", v1[0]->m_iResponseTime);
	
    poDcc->setAvpValue("Service-Information.Micropayment-Info.UnitType-Id", v1[0]->m_iUnitTypeId);
	poDcc->setAvpValue("Service-Information.Micropayment-Info.Deduct-Amount", v1[0]->m_lDeductAmount);
	
	for (int i=0; i<v2.size(); i++) {
		poDcc->setAvpValue("Service-Information.Micropayment-Info.Balance-Item-Detail[%d].Balance-Source", v2[i]->m_iBalanceSource);
		poDcc->setAvpValue("Service-Information.Micropayment-Info.Balance-Item-Detail[%d].Balance-Amount", v2[i]->m_lBalanceAmount);
		 
	}
	
	return 0;
}




/*************
3.4.5.2 全国中心余额划出
************/



int ReqBalanceSvcA(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    int iNums, iTmp;
    ui64 iTmp64;
    char sTmpName[128];
    //AbmPayCond oCond;
    ResBalanceSvc oCond;
    string sTmp;
	time_t nowtime;
    ACE_UINT32 U32Var;
    struct tm *ptm;
    
    ActiveAllocateBalanceCCA *pIn = (ActiveAllocateBalanceCCA *)poCCA;
    memset((void *)&oCond, 0, sizeof(oCond));
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	
	  poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	  snprintf(oCond.m_responseId, sizeof(oCond.m_responseId), sTmp.c_str());
	
	  poDcc->getAvpValue("Service-Information.Service-Result-Code", iTmp);
	  oCond.m_svcResultCode = iTmp;
	  
	  poDcc->getAvpValue("Service-Information.Para-Field-Result", sTmp);
	  snprintf(oCond.m_sParaFieldResult, sizeof(oCond.m_sParaFieldResult), sTmp.c_str());
	  
	  poDcc->getAvpValue("Service-Information.Request-Time", U32Var);
	  oCond.m_iResponseTime = U32Var;
	  /****
	    iTmp -= SECS_70_YEARS;
	    nowtime = (time_t)iTmp;
	    ptm = localtime(&nowtime);
	    snprintf(oCond.m_responseTime, sizeof(oCond.m_responseTime), "%04d%02d%02d%02d%02d%02d",
	                                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                                        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	  ****/
	  poDcc->getAvpValue("Service-Information.Balance-Unit", iTmp);
	  oCond.m_balanceUnit = iTmp;
	  
	  poDcc->getAvpValue("Service-Information.Balance-Amount", iTmp64);
	  oCond.m_balanceAmount = iTmp64;
	  
	  poDcc->getAvpValue("Service-Information.Balance-Type", iTmp);
	  oCond.m_blanceType = iTmp;
	    
	    if (!pIn->addResBalanceSvc(oCond)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addBil error.");
	        return -1;
	    }	    
    return 0;   
}

//打包
int ReqBalanceSvcR(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    vector<ReqBalanceSvc *> v1;
    //vector<ResRechargeBillQuery *> v2;
    int iRet;
	time_t nowtime;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	ActiveAllocateBalanceCCR *pIn = (ActiveAllocateBalanceCCR *)poCCR;
	iRet = pIn->getReqBalanceSvc(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	/***
	iRet = pIn->getBil(v1[0], v2);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getBil 业务处理具体信息解析失败");
	    return -1;
	}
	***/
	
	//dcc打包
    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Request-Id", 0);
	sTmp.assign(v1[0]->m_requestId);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Destination-Id", 0);
	sTmp.assign(v1[0]->m_destinationId);
    poDcc->setAvpValue(sTmpName, sTmp);
  
    poDcc->setAvpValue("Service-Information.Destination-Id-Type", v1[0]->m_destinationIdType);
	
	poDcc->setAvpValue("Service-Information.Destination-Attr", v1[0]->m_destinationAttr);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.AreaCode", 0);
	sTmp.assign(v1[0]->m_areaCode);
    poDcc->setAvpValue(sTmpName, sTmp);
	
	poDcc->setAvpValue("Service-Information.Response-Time", v1[0]->m_iRequestTime);
	
	poDcc->setAvpValue("Service-Information.Balance-Unit", v1[0]->m_balanceUnit);
	ui64 U64Var;
	U64Var = v1[0]->m_rechargeAmount;
	poDcc->setAvpValue("Service-Information.Recharge-Amount", U64Var);
	
	poDcc->setAvpValue("Service-Information.Balance-Type", v1[0]->m_blanceType);
	
	return 0;
}


/*****************************************************
 *
 *                  3.4.2.4  支付冲正
 *
 *****************************************************/
//解析dcc的资料查询请求，并压入内部结构
int ReversePaySvcR(DccMsgParser *poDcc, ABMCCR *poSendBuf)
{
	string sTemp;
	int iTemp;
	ReversePaySvc oQryCond;
	
	if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "parserInfoQryR:传入的指针有空指针");
	    return -1;
	}
	
	ReversePayCCR *pIn = (ReversePayCCR *)poSendBuf;
	memset(&oQryCond, 0, sizeof(oQryCond));	
	
	poDcc->getAvpValue("Service-Information.Micropayment-Info.Operate-Action", iTemp);
	oQryCond.lOperationAction = iTemp;
	
	poDcc->getAvpValue("Service-Information.Micropayment-Info.Operate-Seq", sTemp);
	snprintf(oQryCond.sOperationSeq, sizeof(oQryCond.sOperationSeq), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.Micropayment-Info.Req-Serial", sTemp);
	snprintf(oQryCond.sReqSerial, sizeof(oQryCond.sReqSerial), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.Micropayment-Info.SP-ID", sTemp);
	snprintf(oQryCond.sSpId, sizeof(oQryCond.sSpId), sTemp.c_str());

	poDcc->getAvpValue("Service-Information.Micropayment-Info.SP-NAME", sTemp);
	snprintf(oQryCond.sSpName, sizeof(oQryCond.sSpName), sTemp.c_str());

	poDcc->getAvpValue("Service-Information.Micropayment-Info.Service-Platform-ID", sTemp);
	snprintf(oQryCond.sServicePlatformId, sizeof(oQryCond.sServicePlatformId), sTemp.c_str());
    
    poDcc->getAvpValue("Service-Information.Micropayment-Info.Staff-ID", iTemp);
    oQryCond.iStaffId = iTemp;
    
    if (!pIn->addReversePaySvc(oQryCond)) {
        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	    return -1;
	}
    
    return 0;
}

//根据内部机构体，打包资料查询的应答消息
int ReversePaySvcA(DccMsgParser *poDcc, ABMCCA *poSendBuf)
{
    if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "packInfoQryA:传入的指针有空指针.");
	    return -1;
	}
    
    ReversePayCCA *pIn = (ReversePayCCA *)poSendBuf;
    vector<ReversePayRes1 *> v1;
    vector<ReversePayRes2 *> v2;
    string sTemp;
    char sAvpName[256], sN[16];
    ACE_UINT64 U64Var;
    
    pIn->getReversePayCCA1(v1);
    if (v1.size() == 0) return -1;
    pIn->getReversePayCCA2(v1[0], v2);
    
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->lServiceRltCode);
    sTemp.assign(v1[0]->sParaFieldResult);    
    poDcc->setAvpValue("Service-Information.Para-Field-Result", sTemp);
    
    for (int i=0; i<v2.size(); i++) {
    	snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Micropayment-Info[%d].Operate-Seq", i);
        sTemp.assign(v2[i]->sOperateSeq);
        poDcc->setAvpValue(sAvpName, sTemp);
      snprintf(sAvpName, sizeof(sAvpName), "Service-Information.Micropayment-Info[%d].Req-Serial", i);
        sTemp.assign(v2[i]->sReqSerial);
        poDcc->setAvpValue(sAvpName, sTemp);
    }
    
    return 0;
}

/*****************************************************
 *
 *                 3.4.4.3 & 2.3  VC冲正
 *
 *****************************************************/
//解析dcc的资料查询请求，并压入内部结构
int ReverseDepositR(DccMsgParser *poDcc, ABMCCR *poSendBuf)
{
	string sTemp;
	int iTemp;
	ReverseDeposit oQryCond;
	
	if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "parserInfoQryR:传入的指针有空指针");
	    return -1;
	}
	
	ReverseDepositInfoCCR *pIn = (ReverseDepositInfoCCR *)poSendBuf;
	memset(&oQryCond, 0, sizeof(oQryCond));	
	
	poDcc->getAvpValue("Service-Information.VC-Information.Req-Serial", sTemp);
	snprintf(oQryCond.m_sReqSerial, sizeof(oQryCond.m_sReqSerial), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Res-Serial", sTemp);
	snprintf(oQryCond.m_sPaymentID, sizeof(oQryCond.m_sPaymentID), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Type-Vc", sTemp);
	snprintf(oQryCond.m_sDestinationType, sizeof(oQryCond.m_sDestinationType), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Account", sTemp);
	snprintf(oQryCond.m_sDestinationAccount, sizeof(oQryCond.m_sDestinationAccount), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.VC-Information.Destination-Attr-VC", iTemp);
  oQryCond.m_iDestinationAttr = iTemp;
	
    
    if (!pIn->addReverseDepositInfo(oQryCond)) {
        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	    return -1;
	}
    
    return 0;
}

//根据内部机构体，打包资料查询的应答消息
int ReverseDepositA(DccMsgParser *poDcc, ABMCCA *poSendBuf)
{
    if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "packInfoQryA:传入的指针有空指针.");
	    return -1;
	}
    
    ReverseDepositInfoCCA *pIn = (ReverseDepositInfoCCA *)poSendBuf;
    vector<ResponsReverseDeposit *> v1;
    //vector<ReversePayRes2 *> v2;
    string sTemp;
    char sAvpName[256], sN[16];
    ACE_UINT64 U64Var;
    
    pIn->getRespReverseDepositCCA(v1);
    if (v1.size() == 0) return -1;
    //pIn->getServData(v1[0], v2);
    
    poDcc->setAvpValue("Service-Information.VC-Information.Result", v1[0]->m_iRltCode);
    
    sTemp.assign(v1[0]->m_sReqSerial);
    poDcc->setAvpValue("Service-Information.VC-Information.Req-Serial", sTemp);
    
    return 0;
}


/*****************************************************
 *
 *                  3.4.1.2 资料同步  HSS发CCR
 *
 *****************************************************/
//解析dcc的资料查询请求，并压入内部结构
int QryServSynCondR(DccMsgParser *poDcc, ABMCCR *poSendBuf)
{
	string sTemp;
	int iTemp;
	QryServSynCond oQryCond;
	ui64 iTmp64;
	if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "QryServSynCondR:传入的指针有空指针");
	    return -1;
	}
	
	QryServSynCCR *pIn = (QryServSynCCR *)poSendBuf;
	memset(&oQryCond, 0, sizeof(oQryCond));	
	
	poDcc->getAvpValue("Service-Information.Request-Id", sTemp);
	snprintf(oQryCond.m_sReqID, sizeof(oQryCond.m_sReqID), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.Destination-Type", iTemp);
	oQryCond.m_hDsnType = iTemp;
	
	poDcc->getAvpValue("Service-Information.Destination-Account", sTemp);
	snprintf(oQryCond.m_sDsnNbr, sizeof(oQryCond.m_sDsnNbr), sTemp.c_str());
	
	poDcc->getAvpValue("Service-Information.Destination-Attr", iTemp);
	oQryCond.m_hDsnAttr = iTemp;
	
	poDcc->getAvpValue("Service-Information.Service-Platform-ID", sTemp);
  snprintf(oQryCond.m_sServPlatID, sizeof(oQryCond.m_sServPlatID), sTemp.c_str());
  
  poDcc->getAvpValue("Service-Information.ServId", iTmp64);
  oQryCond.m_lServID = iTmp64;
  
  poDcc->getAvpValue("Service-Information.AcctId", iTmp64);
  oQryCond.m_lAcctID = iTmp64;
  
  poDcc->getAvpValue("Service-Information.Action-Type-Id", sTemp);
  snprintf(oQryCond.m_sActType, sizeof(oQryCond.m_sActType), sTemp.c_str());
  
  poDcc->getAvpValue("Service-Information.Destination-Account-Pin", sTemp);
	snprintf(oQryCond.m_sAccPin, sizeof(oQryCond.m_sAccPin), sTemp.c_str());
  
	
    
    if (!pIn->addServSyn(oQryCond)) {
        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	    return -1;
	}
    
    return 0;
}

//根据内部机构体，打包资料查询的应答消息
int QryServSynCondA(DccMsgParser *poDcc, ABMCCA *poSendBuf)
{
    if (poDcc == NULL || poSendBuf == NULL) {
	    __DEBUG_LOG0_(0, "packInfoQryA:传入的指针有空指针.");
	    return -1;
	}
    
    QryServSynCCA *pIn = (QryServSynCCA *)poSendBuf;
    vector<QryServSynData *> v1;
    //vector<ReversePayRes2 *> v2;
    string sTemp;
    char sAvpName[256], sN[16];
    ACE_UINT64 U64Var;
    
    pIn->getServSyn(v1);
    if (v1.size() == 0) return -1;
    //pIn->getServData(v1[0], v2);
    
    sTemp.assign(v1[0]->m_sResID);
    poDcc->setAvpValue("Service-Information.Response-Id", sTemp);
	
    poDcc->setAvpValue("Service-Information.ServId", v1[0]->m_lServId);

	
    poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_iRltCode);
    
    sTemp.assign(v1[0]->m_sParaRes);
    poDcc->setAvpValue("Service-Information.Para-Field-Result", sTemp);
    
    return 0;
}


/*****************************************************
 *
 *                  3.4.1.2 资料同步  ABM发CCR
 *
 *****************************************************/
int QryServSynCondRT(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    int iNums;
    vector<QryServSynCond *> v1;
    string sTmp;
    int iTmp;
	ui64 iTmp64;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packQryBalanceR:传入的指针有空指针.");
	    return -1;
	}
	
	QryServSynCCR *pIn = (QryServSynCCR *)poCCR;
	iNums = pIn->getServSyn(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packQryBalanceR: getReqBal缺少相关的请求信息");
	    return -1;
	}
	
	for (int i=0; i<iNums; i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Request-Id", i);
	    sTmp.assign(v1[i]->m_sReqID);
	    poDcc->setAvpValue(sTmpName, sTmp);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Type", i);
	    iTmp = v1[i]->m_hDsnType;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Account", i);
	    sTmp = v1[i]->m_sDsnNbr;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Attr", i);
	    iTmp = v1[i]->m_hDsnAttr;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Service-Platform-ID", i);
	     sTmp = v1[i]->m_sServPlatID;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].ServId", i);
	    iTmp64 = v1[i]->m_lServID;
	    poDcc->setAvpValue(sTmpName, iTmp64);
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].AcctId", i);
	    iTmp64 = v1[i]->m_lAcctID;
	    poDcc->setAvpValue(sTmpName, iTmp64);
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Action-Type-Id", i);
	     sTmp = v1[i]->m_sActType;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Account-Pin", i);
	     sTmp = v1[i]->m_sAccPin;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	}
	
	return 0;
}
//parserQryBalanceA
int QryServSynCondAT(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    int iRet, iTmp, iNums;
    string sTmp;
    ui64 var;
    char sTmpName[128];
    QryServSynData oQryCond;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserQryBalanceA:传入的指针有空指针.");
	    return -1;
	}
	
	QryServSynCCA *pIn = (QryServSynCCA *)poCCA;
	memset((void *)&oQryCond, 0, sizeof(oQryCond));

	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
  snprintf(oQryCond.m_sResID, sizeof(oQryCond.m_sResID), sTmp.c_str());
  
  	poDcc->getAvpValue("Service-Information.ServId", var);
    oQryCond.m_lServId = var;
  
	poDcc->getAvpValue("Service-Information.Service-Result-Code", oQryCond.m_iRltCode);
	poDcc->getAvpValue("Service-Information.Para-Field-Result", sTmp);
  snprintf(oQryCond.m_sParaRes, sizeof(oQryCond.m_sParaRes), sTmp.c_str());
	
	
	    if (!pIn->addServSyn(oQryCond)) {
	        __DEBUG_LOG0_(0, "parserQryBalanceA: addResBal error.");
	        return -1;
        }
	return 0;
}
    

/*****************************************************
 *
 *       头文件中 3.4.6.1 密码HSS端鉴权   3.4.1.3  向HSS发送CCR
 *
 *****************************************************/
 //QryServSynCondRT
int PasswdIdentifyCondRT(DccMsgParser *poDcc, ABMCCA *poCCR)
{
    int iNums;
    vector<PasswdIdentifyCond *> v1;
    string sTmp;
    int iTmp;
    char sTmpName[128];
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "packQryBalanceR:传入的指针有空指针.");
	    return -1;
	}
	
	PasswdIdentifyCCR *pIn = (PasswdIdentifyCCR *)poCCR;
	iNums = pIn->getPasswdIdentify(v1);
	if (iNums <= 0) {
	    __DEBUG_LOG0_(0, "packQryBalanceR: getReqBal缺少相关的请求信息");
	    return -1;
	}
	
	for (int i=0; i<iNums; i++) {
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Request-Id", i);
	    sTmp.assign(v1[i]->m_sReqID);
	    poDcc->setAvpValue(sTmpName, sTmp);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Type", i);
	    iTmp = v1[i]->m_hDsnType;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Account", i);
	    sTmp = v1[i]->m_sDsnNbr;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Attr", i);
	    iTmp = v1[i]->m_hDsnAttr;
	    poDcc->setAvpValue(sTmpName, iTmp);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Service-Platform-ID", i);
	     sTmp = v1[i]->m_sServPlatID;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information[%d].Destination-Account-Pin", i);
	     sTmp = v1[i]->m_sLoginPasswd;
	    poDcc->setAvpValue(sTmpName, sTmp);
	    
	}
	
	return 0;
}
//parserQryBalanceA
int PasswdIdentifyCondAT(DccMsgParser *poDcc, ABMCCR *poCCA)
{
    int iRet, iTmp, iNums;
    string sTmp;
    ui64 var;
    char sTmpName[128];
    PasswdIdentifyData oQryCond;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "parserQryBalanceA:传入的指针有空指针.");
	    return -1;
	}
	
	PasswdIdentifyCCA *pIn = (PasswdIdentifyCCA *)poCCA;
	memset((void *)&oQryCond, 0, sizeof(oQryCond));

	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
  snprintf(oQryCond.m_sResID, sizeof(oQryCond.m_sResID), sTmp.c_str());
  
	poDcc->getAvpValue("Service-Information.Service-Result-Code", oQryCond.m_hRltCode);
	poDcc->getAvpValue("Service-Information.Para-Field-Result", sTmp);
  snprintf(oQryCond.m_sParaRes, sizeof(oQryCond.m_sParaRes), sTmp.c_str());
	
	
	    if (!pIn->addPasswdIdentify(oQryCond)) {
	        __DEBUG_LOG0_(0, "parserQryBalanceA: addResBal error.");
	        return -1;
        }
	return 0;
}
    


/*****************************************************
 *
 *       头文件中 3.4.6.1 登陆密码鉴权   3.4.6.1
 *
 *****************************************************/
int LoginPasswdCCRR(DccMsgParser *poDcc, ABMCCR *poCCR)
{
    int iNums, iTmp;
    char sTmpName[128];
    AbmPayCond oCond;
    string sTmp;
	  ui64 iTmp64;
    ACE_UINT32 U32Var;
    time_t nowtime;
    struct tm *ptm;
    LoginPasswdStruct oCond1;
	  LoginPasswdCond   oCond2;
	  
    LoginPasswdCCR *pIn = (LoginPasswdCCR *)poCCR;
    
    if (poDcc == NULL || poCCR == NULL) {
        __DEBUG_LOG0_(0, "parserChargeR:传入的指针有空指针.");
	    return -1;
	}
	//添加第一部分结构体
	memset((void *)&oCond1, 0, sizeof(oCond1));	
	poDcc->getAvpValue("Service-Information.Request-Id", sTmp);
	snprintf(oCond1.m_sReqID, sizeof(oCond1.m_sReqID), sTmp.c_str());
	
	poDcc->getAvpValue("Service-Information.Request-Time", U32Var);
	oCond1.m_iReqTime = U32Var;
	
	if (!pIn->addLoginPasswdRespons(oCond1)) {
		__DEBUG_LOG0_(0, "parserRandPswdR: addpasswdRespons error.");
		return -1;
	}
	
	iNums = poDcc->getGroupedAvpNums("Service-Information.Micropayment-Info");
	for (int i=0; i<iNums; i++) {
	    
	    memset(&oCond2, 0, sizeof(oCond2));
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond2.m_sDestAccount, sizeof(oCond2.m_sDestAccount), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
	    poDcc->getAvpValue(sTmpName, oCond2.m_hDestType);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
	    poDcc->getAvpValue(sTmpName, oCond2.m_hDestAttr);
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond2.m_sServPlatID, sizeof(oCond2.m_sServPlatID), sTmp.c_str());
	    
	    snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Pay-Password", i);
	    poDcc->getAvpValue(sTmpName, sTmp);
	    snprintf(oCond2.m_sLoginPasswd, sizeof(oCond2.m_sLoginPasswd), sTmp.c_str());
	    
	    if (!pIn->addLoginPasswd(oCond2)) {
	        __DEBUG_LOG0_(0, "parserChargeR: addPayInfo error.");
	        return -1;
	    }
	}
	    
    return 0;   
}

//打包
int LoginPasswdCCRA(DccMsgParser *poDcc, ABMCCA *poCCA)
{
    vector<LoginPasswdData *> v1;
    //vector<ResponsPayTwo *> v2;
    int iRet;
    char sTmpName[128];
    string sTmp;
    
    if (poDcc == NULL || poCCA == NULL) {
        __DEBUG_LOG0_(0, "packChargeA:传入的指针有空指针.");
	    return -1;
	}
	
	//取内部包结构
	LoginPasswdCCA *pIn = (LoginPasswdCCA *)poCCA;
	iRet = pIn->getLoginPasswdData(v1);
	if (iRet <= 0) {
	    __DEBUG_LOG0_(0, "packChargeA: getRespons缺少业务处理结果码等信息");
	    return -1;
	}
	
	//dcc打包
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Response-Id", 0);
    sTmp.assign(v1[0]->m_sResID);
    poDcc->setAvpValue(sTmpName, sTmp);
    
	poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_hRltCode);
	
	snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Para-Field-Result", 0);
    sTmp.assign(v1[0]->m_sParaRes);
    poDcc->setAvpValue(sTmpName, sTmp);
poDcc->setAvpValue("Service-Information.Service-Result-Code", v1[0]->m_iResTime);
	
	return 0;
}


