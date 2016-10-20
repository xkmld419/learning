#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/timeb.h>
#include "DccMsgParser.h"
#include "ABMException.h"
#include "MessageQueue.h"
#include "dccpublic.h"
#include "dcc_ra.h"

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
	  iVar = 0;
    poDcc->setAvpValue("Service-Information.Service-Result-Code", iVar);
	  
	  U64Var=4094;
    poDcc->setAvpValue("Service-Information.Total-Balance-Available", U64Var);
    
    sVar.assign("VCÆÕÍ¨Óà¶î");
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Balance-ItemType-Detail", sVar);
    
    
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.UnitType-ID", iVar);
    
    U64Var=3810;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Balance-Amount", U64Var);
    
    U64Var=3810;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Balance-Available", U64Var);
    
    
    U64Var=0;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Balance-Used", U64Var);
    
    
    U64Var=0;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Balance-Reserved", U64Var);
    
    U32Var=123456;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Eff-Date", U32Var);
    
    U32Var=123456;
    poDcc->setAvpValue("Service-Information.Balance-Information.Balance-Item-Detail.Exp-Date", U32Var);
    
    
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
    
    sVar.assign("ABM2@001.ChinaTelecom.com;1306302304;90713");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("VC@025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("ABM2@001.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("001.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("Recharge.XEF@001.ChinaTelecom.com");
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
    //oMq.Send((void *)&struMsg, (sResult.size()+sizeof(long)));
    
    //oMq.Send((void *)sResult.c_str(), sResult.size());

    delete poOcpMsgParser;
    exit(0);
    
}
