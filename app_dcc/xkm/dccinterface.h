#ifndef _DCC_INTERFACE_H_HEADER_
#define _DCC_INTERFACE_H_HEADER_
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "DccMsgParser.h"
#include "LogV2.h"

int getCER(char *sBuf, int iMaxLen)
{
    string sTmp;
    struct in_addr addr;
    diameter_address_t m_dHostIPAddressX;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    int iVenderID = 81000, iRet;
    
    if (sBuf == NULL) {
        __DEBUG_LOG0_(0, "传入指针为空.");
        return -1;
    }
    
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

    DccMsgParser oDccObj(path);
    
    oDccObj.setDiameterHeader(COMMAND_CODE_CE, true, 0, 3, 4);
    oDccObj.releaseMsgAcl();
    
    sTmp.assign("001.ChinaTelecom.com");
    oDccObj.setAvpValue("Origin-Realm", sTmp);
    sTmp.assign("ABM2@001.ChinaTelecom.com");
    oDccObj.setAvpValue("Origin-Host", sTmp);
    
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
    oDccObj.setAvpValue("Host-IP-Address",m_dHostIPAddressX);
    oDccObj.setAvpValue("Vendor-Id",iVenderID);
    sTmp.assign("ChinaTelecom");
    oDccObj.setAvpValue("Product-Name",sTmp);
    
    iRet = oDccObj.parseAppToRaw(sBuf, iMaxLen);
    if (iRet <= 0) {
        __DEBUG_LOG0_(0, "打包CER出错.");
        return -1;
    }
    
    return iRet;
}   
    
#endif 
        
        