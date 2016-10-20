/************
backOUT
*************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <sys/timeb.h>
#include "DccMsgParser.h"
#include "ABMException.h"
#include "MessageQueue.h"
#include "../dccpublic.h"


#define MAXSIZE 1024

using namespace std;
//static StruMqMsg   g_sPeerBuf; 
int          iVar;
long         lVar;
//ACE_UINT32   U32Var;
//ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;


int main(int argc,char* argv[])
{
    
    char sDictFile[256];
    char *p;
    int iResult;
    std::string sResult;
    DccMsgParser *poOcpMsgParser;
    DccMsgParser *poOcpMsgParserDPR;
    ABMException oExp;
    ACE_UINT32 U32Var;
    diameter_address_t m_dHostIPAddressX;
    int iLen;
    char sHostName[1024] = {0};
    char LocalIp[64] = {0};
    struct in_addr addr;
    struMsg.m_lMsgType = 1;
    
	  sprintf(sDictFile, "/ABM/app/deploy/config/dictionary.xml");
	  MessageQueue oMq(10001);
	  oMq.open(oExp, true, true);
    

    poOcpMsgParser = new DccMsgParser(sDictFile);
    if (poOcpMsgParser == NULL) {
        cout<<"new error"<<endl;
        exit(-1);
    }
    poOcpMsgParser->setDiameterHeader(272,true,4,3,4);
    poOcpMsgParser->releaseMsgAcl();
    
    // CCRÈ¥¼¤»îÇëÇó
    //============================================================
    sVar.assign("ABM2@001.ChinaTelecom.com;1306302304;90713");
    poOcpMsgParser->setAvpValue("Session-Id", sVar);
    
    sVar.assign("ABM2@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host", sVar);
    
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm", sVar);
    
    sVar.assign("SR2@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host", sVar);
    
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm", sVar);
    
    iVar = 4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id", iVar);

    iVar = 111;
    poOcpMsgParser->setAvpValue("Requested-Action", iVar);
    iVar = 4;
    poOcpMsgParser->setAvpValue("CC-Request-Type", iVar);
    
    iVar = 0;
    poOcpMsgParser->setAvpValue("CC-Request-Number", iVar);
    
    sVar.assign("De-Active.Net-Ctrl@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Context-Id", sVar);
    
    iVar = time(0);
    poOcpMsgParser->setAvpValue("Event-Timestamp", iVar);
    /*****
    sVar.assign("201105251021481234567800-00010-000024005999-v1.02.03-010001-Recharge.ABM2@001;");
    poOcpMsgParser->setAvpValue("Service-Flow-Id", sVar);
    ****/
    
    sVar.assign("ABM2@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Host", sVar);
    
    iVar = 2;
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Type", iVar);
    
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
    
     poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-IP", m_dHostIPAddressX);
    
    iVar = 3868;
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Port", iVar);
    
    iVar = 12;
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.Network-Latency", iVar);
    
    sVar.assign("Query.Balance@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[0]", sVar);
    sVar.assign("ReverseChargeFromABM.Micropayment@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[1]", sVar);

    sVar.assign("DeductFromABM.Micropayment@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[2]", sVar);
sVar.assign("VCRecharge.Balance@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[3]", sVar);
sVar.assign("VCRefund.Balance@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[3]", sVar);


    iVar = 2;
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Priority", iVar);
    
    iVar = 0;
    poOcpMsgParser->setAvpValue("Service-Information.Net-Ctrl-Information.Service-Context-Information.Update-Action", iVar);
    
    
    sResult.clear();
    poOcpMsgParser->getAllAvp();
    iResult=poOcpMsgParser->parseAppToRaw(sResult);
    if (iResult<=0){
    	printf("pack error.\n");
    	delete poOcpMsgParser;
    	exit(1);
    }
    
    
    memcpy(struMsg.m_sMsg, sResult.c_str(), sResult.size());
    disp_buf((char *)&struMsg, sResult.size()+sizeof(long));
    oMq.Send((void *)&struMsg, (sResult.size()+sizeof(long)));
    
    //oMq.Send((void *)sResult.c_str(), sResult.size());

    //delete poOcpMsgParser;
    //DPR
    /*
    poOcpMsgParserDPR = new DccMsgParser(sDictFile);
    if (poOcpMsgParserDPR == NULL) {
        cout<<"new error"<<endl;
        exit(-1);
    }
    */
    sleep(10);
    poOcpMsgParser->setDiameterHeader(282,true,0,3,4);
    poOcpMsgParser->releaseMsgAcl();
    sVar.assign("ABM2@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host", sVar);
    
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm", sVar);
    
    //sVar.assign("out");
    int iiii= 2;
    poOcpMsgParser->setAvpValue("Disconnect-Cause", iiii);
    
    sResult.clear();
    poOcpMsgParser->getAllAvp();
    iResult=poOcpMsgParser->parseAppToRaw(sResult);
    if (iResult<=0){
    	printf("pack error.\n");
    	delete poOcpMsgParser;
    	exit(1);
    }
       memcpy(struMsg.m_sMsg, sResult.c_str(), sResult.size());
    disp_buf((char *)&struMsg, sResult.size()+sizeof(long));
    oMq.Send((void *)&struMsg, (sResult.size()+sizeof(long)));
    
	delete poOcpMsgParser;
	//delete poOcpMsgParserDPR;
    exit(0);
    
}
