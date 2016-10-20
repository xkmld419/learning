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
    poDcc->setAvpValue("Service-Information.VC-Information.Req-Serial", sVar);
    
    iVar =10;
    poDcc->setAvpValue("Service-Information.VC-Information.Balance-Type", iVar);
    iVar = 2;
    poDcc->setAvpValue("Service-Information.VC-Information.Destination-Balance-Unit", iVar);
    iVar = 2;
    poDcc->setAvpValue("Service-Information.VC-Information.Voucher-Value", iVar);
    iVar = 30;
    poDcc->setAvpValue("Service-Information.VC-Information.Prolong-Days", iVar);
    
    sVar.assign("5BC");
    poDcc->setAvpValue("Service-Information.VC-Information.Destination-Type-Vc", sVar);
    cout<<"帐号值"<<endl;
    cin>>sTmpName;
    sVar.assign(sTmpName);
    poDcc->setAvpValue("Service-Information.VC-Information.Destination-Account", sVar);
    cout<<"Destination-Attr"<<endl;
    cin>>iVar;
    poDcc->setAvpValue("Service-Information.VC-Information.Destination-Attr-VC", iVar);
    cout<<"Destination-Attr-Detail"<<endl;
    cin>>iVar;
    poDcc->setAvpValue("Service-Information.VC-Information.Destination-Attr-Detail", iVar);
    //sVar.assign("010");
    cout<<"区号"<<endl;
    cin>>sTmpName;
    sVar.assign(sTmpName);
    poDcc->setAvpValue("Service-Information.VC-Information.AreaCode", sVar);
    
   /***
    iVar = 0;
    poDcc->setAvpValue("Service-Information.VC-Information.Bonus-Info[0].Bonus-Unit", iVar);
    iVar = 3;
    poDcc->setAvpValue("Service-Information.VC-Information.Bonus-Info[0].Bonus-Amount", iVar);
    //sVar.assign("3");
    //poDcc->setAvpValue("Service-Information.VC-Information.Bonus-Info[0].Balance-Type-VC", sVar);
    
    iVar = 20110101;
    poDcc->setAvpValue("Service-Information.VC-Information.Bonus-Info[0].Bonus-Start-Time", iVar);
    
    iVar = 20110601;
    poDcc->setAvpValue("Service-Information.VC-Information.Bonus-Info[0].Bonus-Expiration-Time", iVar);
***/
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
cout<<"请输入Origin-Host 示例HB@025.ChinaTelecom.com 输入025"<<endl;
    cin>>iVar;
    char sTmpName[110];
	time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
    snprintf(sTmpName,sizeof(sTmpName),"%s%03d%s%010d%s%04d\0","HB@",iVar,".ChinaTelecom.com;",tTime,";",iSeq);
    sVar.assign(sTmpName);    

    //sVar.assign("ABM@001.ChinaTelecom.com;1306302304;90714");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    
    snprintf(sTmp,sizeof(sTmp),"%s%03d%s\0","HB@",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    
    //sVar.assign("VC@025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    snprintf(sTmp,sizeof(sTmp),"%03d%s\0",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    //sVar.assign("025.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("ABM@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("VCRecharge.Balance@001.ChinaTelecom.com");
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
