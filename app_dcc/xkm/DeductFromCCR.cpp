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
//ACE_UINT32   U32Var;
ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;



void packChargeR(DccMsgParser *poDcc)
{
   char sTmpName[110];
    time_t tTime;
    tTime = time(NULL);
    snprintf(sTmpName,sizeof(sTmpName),"%s%010d\0","00220110615164230",tTime);
    sVar.assign(sTmpName);

    poDcc->setAvpValue("Service-Information.Request-Id", sVar);

   cout<<"ÇëÊäÈëÓÃ»§ºÅÂë"<<endl;
    cin>>sTmpName;  
	sVar.assign(sTmpName);
    poDcc->setAvpValue("Service-Information.Destination-Id", sVar);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Destination-Id-Type", iVar);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Destination-Attr", iVar);
	  
	  sVar.assign("2-test");
    poDcc->setAvpValue("Service-Information.AreaCode", sVar);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Staff-ID", iVar);
    
    iVar = 2207920000;
    poDcc->setAvpValue("Service-Information.Deduct-Date", iVar);
    
    sVar.assign("2-id");
    poDcc->setAvpValue("Service-Information.ABM-Id", sVar);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[0].Billing-Cycle", iVar);
    sVar.assign("2");
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[0].Acct-Item-Type-Id", sVar);
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[0].Balance-Unit", iVar);
    
    U64Var = 20;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[0].Deduct-Amount", U64Var);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item[0].Charge-Type", iVar);
    return;
}

int main(int argc,char* argv[])
{
    char sTmp[170];
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
    
    time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
    snprintf(sTmp,sizeof(sTmp),"%s%010d%s%04d\0","ABM2@001.ChinaTelecom.com;",tTime,";",iSeq);
    sVar.assign(sTmp);
    poOcpMsgParser->setAvpValue("Session-Id",sVar);

    sVar.assign("HB@025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("DeductFromABM.Micropayment@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Service-Context-Id",sVar);
    iVar=0;
    poOcpMsgParser->setAvpValue("CC-Request-Number",iVar);
    iVar=1;
    poOcpMsgParser->setAvpValue("CC-Request-Type",iVar);
    
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
