/************
3.4.2.5
*************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/timeb.h>
#include "DccMsgParser.h"
#include "ABMException.h"
#include "MessageQueue.h"
#include "../dccpublic.h"


#define MAXSIZE 1024

using namespace std;

int          iVar;
long         lVar;
ACE_UINT32   U32Var;
ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;



void packChargeR(DccMsgParser *poDcc)
{
    sVar.assign("charge-20110513000001");
    poDcc->setAvpValue("Service-Information.Request-Id", sVar);
	iVar = 120232322;
    poDcc->setAvpValue("Service-Information.Request-Time", iVar);
    
    sVar.assign("18901235423");
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Destination-Account", sVar);
	iVar = 1;
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Destination-type", iVar);
	iVar = 3;
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Destination-Attr", iVar);
	sVar.assign("china-123");
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Service-Platform-ID", sVar);
	sVar.assign("china-123");
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Random-AccNbr", sVar);
	sVar.assign("china-123");
    poDcc->setAvpValue("Service-Information.Micropayment-Info[0].Order-ID", sVar);
	
	
    return;
}

int main(int argc,char* argv[])
{
    
    char sDictFile[256];
    char *p;
    int iResult;
    std::string sResult;
    DccMsgParser *poOcpMsgParser;
    ABMException oExp;
    
	sprintf(sDictFile, "/ABM/app/deploy/config/dictionary.xml");
	MessageQueue oMq(10002);
	oMq.open(oExp, true, true);
    

    poOcpMsgParser = new DccMsgParser(sDictFile);
    if (poOcpMsgParser == NULL) {
        cout<<"new error"<<endl;
        exit(-1);
    }
    poOcpMsgParser->setDiameterHeader(272,true,4,3,4);
    poOcpMsgParser->releaseMsgAcl();
    
    // set_avp_value
    //============================================================
    
    sVar.assign("session-abcdefg");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("SCP001.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("abm-center");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("www.lianchuang.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("Random-Password.Micropayment@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Context-Id",sVar);
    sVar.assign("srv_flow_id");
    poOcpMsgParser->setAvpValue("Service-Flow-Id",sVar);
    sVar.assign("platform-desc");
    poOcpMsgParser->setAvpValue("OutPlatformDescription", sVar);
    iVar=0;
    poOcpMsgParser->setAvpValue("CC-Request-Number",iVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("CC-Request-Type",iVar);
    iVar = 113;
    poOcpMsgParser->setAvpValue("Requested-Action", iVar);
    
    packChargeR(poOcpMsgParser);
    
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

    delete poOcpMsgParser;
    exit(0);
    
}
