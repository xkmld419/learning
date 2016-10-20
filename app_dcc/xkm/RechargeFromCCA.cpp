/************
3.4.6.5
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
#include "dccpublic.h"


#define MAXSIZE 1024

using namespace std;

int          iVar;
long         lVar;
//ACE_UINT32   U32Var;
ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;



void packChargeR(DccMsgParser *poDcc)
{
    
    sVar.assign("response-20110602");
    poDcc->setAvpValue("Service-Information.Request-Id", sVar);

    iVar = 0;
    poDcc->setAvpValue("Service-Information.Service-Result-Code", iVar);

    iVar = 18900998;
    poDcc->setAvpValue("Service-Information.Response-Time", iVar);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Unit",iVar);

    U64Var = 10;
    poDcc->setAvpValue("Service-Information.Balance-Amount", U64Var);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Type", iVar);
	
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
	MessageQueue oMq(10005);
	oMq.open(oExp, true, true);
    

    poOcpMsgParser = new DccMsgParser(sDictFile);
    if (poOcpMsgParser == NULL) {
        cout<<"new error"<<endl;
        exit(-1);
    }
    poOcpMsgParser->setDiameterHeader(272,false,4,3,4);
    poOcpMsgParser->releaseMsgAcl();
    
    // set_avp_value
    //============================================================
    
    sVar.assign("ABM2@001.ChinaTelecom.com;1307261794;0059");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("HB@025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);

    iVar=0;
    poOcpMsgParser->setAvpValue("CC-Request-Number",iVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("CC-Request-Type",iVar);
    iVar = 2001;
    poOcpMsgParser->setAvpValue("Result-Code", iVar);
    
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
