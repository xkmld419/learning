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

int packRandPswdR(DccMsgParser *poDcc)
{
	char sTmpName[128];
	sVar.assign("hello world");
	poDcc->setAvpValue("Service-Information.Request-Id", sVar);
	U32Var = time(0);
	poDcc->setAvpValue("Service-Information.Request-Time", U32Var);
	
	for (int i=0; i<1; i++) {
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Account", i);
		sVar.assign("18951765055");
		poDcc->setAvpValue(sTmpName, sVar);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Type", i);
		iVar = 2;
		poDcc->setAvpValue(sTmpName, iVar);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Destination-Attr", i);
		iVar = 2;
		poDcc->setAvpValue(sTmpName, iVar);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Service-Platform-ID", i);
		sVar.assign("telecom");
		poDcc->setAvpValue(sTmpName, sVar);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Random-AccNbr", i);
		sVar.assign("18951765055");
		poDcc->setAvpValue(sTmpName, sVar);
		
		snprintf(sTmpName, sizeof(sTmpName), "Service-Information.Micropayment-Info[%d].Order-ID", i);
		sVar.assign("123456");
		poDcc->setAvpValue(sTmpName, sVar);
	}
	
	return 0;
}

int main(int argc, char **argv)
{
  
    char sDictFile[256];
    char *p;
    int iResult;
    std::string sResult;
    DccMsgParser *poOcpMsgParser;
    ABMException oExp;
    
	sprintf(sDictFile, "/ABM/app/deploy/config/dictionary.xml");
	MessageQueue oMq(10005);
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
    
    sVar.assign("ABM2@001.ChinaTelecom.com;1306302304;90714");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("HB@025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("ABM2@001.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("001.chinatelecom.com");
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

    packRandPswdR(poOcpMsgParser);
    
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
