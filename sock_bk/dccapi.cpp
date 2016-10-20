#include "dccapi.h"

//DccMsgParser *DccApi::m_poDcc = 0;

int DccApi::init()
{
    if (m_poDcc != NULL)
        return 0;
        
    char *p = getenv("ABMAPP_DEPLOY");
    if (p == NULL) {
        __DEBUG_LOG0_(0, "ABMAPP_DEPLOY 环境变量没有设置!");
        return -1;
    }
    char path[1024];
    char stmp[1024] = {0};
    strncpy(stmp, p, sizeof(stmp)-2);
    if (stmp[strlen(stmp)-1] != '/') {
        stmp[strlen(stmp)] = '/';
    }
    snprintf(path, sizeof(path), "%sconfig/dictionary.xml", stmp);
    
    m_poDcc = new DccMsgParser(path);
    if (m_poDcc == NULL) {
        __DEBUG_LOG0_(0, "分配内存出错!");
        return -1;
    }
    
    return 0;
}


int DccApi::cer2cea(char *sCer, int iLen, char *sCea)
{
    if (sCer == NULL || sCea == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空!");
        return -1;
    }
    
    int iRet, i;
    int int32=2001;
    int iVenderID;
    string sProductName;
    string  sHost;
    string  sRealm;
    diameter_address_t m_dHostIPAddressX;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    struct in_addr addr;
    
    iRet = m_poDcc->parseRawToApp(sCer, iLen);
    if (iRet != 0) {
	    __DEBUG_LOG1_(0, "消息解包出错，错误编号:%d.", -iRet);
		return -1;
    }
    
    m_poDcc->getAvpValue("Vendor-Id", iVenderID);
    m_poDcc->getAvpValue("Product-Name", sProductName);
    m_poDcc->getAvpValue("Origin-Host", sHost);
    m_poDcc->getAvpValue("Origin-Realm", sRealm);
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_CE, false,0,3,4);
    m_poDcc->releaseMsgAcl();
    
    m_poDcc->setAvpValue("Result-Code", int32);
    m_poDcc->setAvpValue("Origin-Host", sHost);
    m_poDcc->setAvpValue("Origin-Realm",sRealm);
    
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
    m_poDcc->setAvpValue("Host-IP-Address",m_dHostIPAddressX);
    m_poDcc->setAvpValue("Vendor-Id",iVenderID);
    m_poDcc->setAvpValue("Product-Name",sProductName);
    
    i = m_poDcc->parseAppToRaw(sCea, MSG_SIZE);
    if (i <= 0) {
        __DEBUG_LOG0_(0, "打包失败! ");
        return -1;
    }
    
    return i;
}


int DccApi::dwr2dwa(char *sDwr, int iLen, char *sDwa)
{
    string sHost, sRealm;
    int iRet;
    int int32 = 2001;
    
    if (sDwr == NULL || sDwa == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    m_poDcc->getAvpValue("Origin-Host", sHost);
    m_poDcc->getAvpValue("Origin-Realm", sRealm);
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_DW, true, 0, 3, 4);
    m_poDcc->releaseMsgAcl();
    
    m_poDcc->setAvpValue("Result-Code", int32);
    m_poDcc->setAvpValue("Origin-Realm", sRealm);
    m_poDcc->setAvpValue("Origin-Host", sHost);
    
    iRet = m_poDcc->parseAppToRaw(sDwa, MSG_SIZE);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}


int DccApi::dpr2dpa(char *sDpr, int iLen, char *sDpa)
{
    string sHost, sRealm;
    int iRet;
    int int32 = 2001;
    
    if (sDpr == NULL || sDpa == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    
    m_poDcc->getAvpValue("Origin-Host", sHost);
    m_poDcc->getAvpValue("Origin-Realm", sRealm);
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_DP, true, 0, 3, 4);
    m_poDcc->releaseMsgAcl();
    
    m_poDcc->setAvpValue("Result-Code", int32);
    m_poDcc->setAvpValue("Origin-Realm", sRealm);
    m_poDcc->setAvpValue("Origin-Host", sHost);
    
    iRet = m_poDcc->parseAppToRaw(sDpa, MSG_SIZE);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}


int DccApi::getCer(char *sBuf, int iMaxLen)
{
    string sTmp;
    struct in_addr addr;
	int iVar;
    diameter_address_t m_dHostIPAddressX;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    int iVenderID = 81000, iRet;
    
    if (sBuf == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_CE, true, 0, 3, 4);
    m_poDcc->releaseMsgAcl();
    
    sTmp.assign("001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Realm", sTmp);
    sTmp.assign("ABM@001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Host", sTmp);
	iVar = 4;
    m_poDcc->setAvpValue("Auth-Application-Id", iVar);
    
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
    m_poDcc->setAvpValue("Host-IP-Address",m_dHostIPAddressX);
    m_poDcc->setAvpValue("Vendor-Id",iVenderID);
    sTmp.assign("ChinaTelecom");
    m_poDcc->setAvpValue("Product-Name",sTmp);
    
    iRet = m_poDcc->parseAppToRaw(sBuf, iMaxLen);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}
 
   
int DccApi::getDwr(char *sBuf, int iMaxLen)
{
    string sTmp;
    int iRet;
    
    if (sBuf == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_DW, true, 0, 3, 4);
    m_poDcc->releaseMsgAcl();
    
    sTmp.assign("001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Realm", sTmp);
    sTmp.assign("ABM@001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Host", sTmp);
    
    iRet = m_poDcc->parseAppToRaw(sBuf, iMaxLen);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}


int DccApi::getDpr(char *sBuf, int iMaxLen)
{
    string sTmp;
    int iRet;
    ACE_UINT32   U32Var;
    
    if (sBuf == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    
    m_poDcc->setDiameterHeader(COMMAND_CODE_DP, true, 0, 3, 4);
    m_poDcc->releaseMsgAcl();
    
    sTmp.assign("001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Realm", sTmp);
    sTmp.assign("ABM@001.ChinaTelecom.com");
    m_poDcc->setAvpValue("Origin-Host", sTmp);
    U32Var = 2;                                 //1--Rebooting; 2--Busy;
                                                //3--Do not want to talk to you
    m_poDcc->setAvpValue("Disconnect-Cause", U32Var);
    
    iRet = m_poDcc->parseAppToRaw(sBuf, iMaxLen);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}