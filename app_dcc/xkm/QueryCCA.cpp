/************
3.4.5.5
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

    sVar.assign("002201106151642301310784562");
    poDcc->setAvpValue("Service-Information.Request-Id", sVar);
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Service-Result-Code", iVar);
    
    sVar.assign("OK");
    poDcc->setAvpValue("Service-Information.Para-Field-Result", sVar);
    
    
    U64Var = 7045807;
    poDcc->setAvpValue("Service-Information.Accout-Id", U64Var);
    
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Balance-Unit", iVar);
    U64Var = 34;
    poDcc->setAvpValue("Service-Information.Deduct-Amount", U64Var);
    U64Var = 12344;
    poDcc->setAvpValue("Service-Information.Balance-Available", U64Var);
    
    iVar = 20110601;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item.Billing-Cycle-ID", iVar);
    sVar.assign("1005");
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item.Acct-Item-Type-Id", sVar);
    
    U64Var = 17323305;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item.Acct-Balance-Id", U64Var);
    
    U64Var = 34;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item.Acct-Balance-Deduct-Amount", U64Var);
    U64Var = 12344;
    poDcc->setAvpValue("Service-Information.Request-Debit-Acct-Item.Acct-Balance-Amount", U64Var);
    

    return;
}


int main(int argc,char* argv[])
{
    char sTmp[170];
    char sTmp2[170];
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
    poOcpMsgParser->setDiameterHeader(272,false,4,3,4);
    poOcpMsgParser->releaseMsgAcl();
    
    cout<<"Session-Id: ABM2@001.ChinaTelecom.com;1310612688;130355"<<endl;
    snprintf(sTmp,sizeof(sTmp),"%s\0","ABM2@001.ChinaTelecom.com;1310612688;130355");
    cout<<sTmp<<endl;
    sVar.assign(sTmp);
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("HB@431.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("431.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);

	/***
    cout<<"请输入Destination-Host 示例HB@025.ChinaTelecom.com 输入025"<<endl;
    cin>>iVar;
    snprintf(sTmp,sizeof(sTmp),"%s%03d%s\0","HSS@",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"estination-Host为:"<<sTmp<<endl;
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);

    
    snprintf(sTmp,sizeof(sTmp),"%03d%s\0",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"Destination-Realm为:"<<sTmp<<endl;
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);

    snprintf(sTmp,sizeof(sTmp),"%s%03d%s\0","InfoSynchroFromHSS000.Micropayment@",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"Service-Context-Id为:"<<sTmp<<endl;
    poOcpMsgParser->setAvpValue("Service-Context-Id",sVar);

    ****/
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    
    
    sVar.assign("srv_flow_id");
    poOcpMsgParser->setAvpValue("Service-Flow-Id",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("OutPlatformDescription", sVar);
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
