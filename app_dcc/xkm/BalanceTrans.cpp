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
    char sTmpName[110];
    time_t tTime;
    tTime = time(NULL);
    snprintf(sTmpName,sizeof(sTmpName),"%s%010d\0","00220110615164230",tTime);
    sVar.assign(sTmpName);
    
    //sVar.assign("002201106151033450057102249");
    poDcc->setAvpValue("Service-Information.Request-Id", sVar);

    iVar = 3423934560;
    poDcc->setAvpValue("Service-Information.Request-Time", iVar);

    cout<<"请输入用户号码"<<endl;
    cin>>sTmpName;
    sVar.assign(sTmpName);
    //sVar.assign("15372401996");   //****
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Destination-Account", sVar);

    iVar = 1;
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Destination-Type",iVar);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Destination-Attr", iVar);

    sVar.assign("1");
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Service-Platform-ID", sVar);

    sVar.assign("123456");
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Pay-Password", sVar);
 
    cout<<"请输入划出类型 1 被动划拨(划拨充值)  2 主动划拨(中心余额划出)"<<endl;
    cin>>iVar;
    //iVar = 1;   //****
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Balance-TransFlag", iVar);
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Micropayment-Info.UnitType-Id", iVar);
    
    cout<<"请输入划拨金额"<<endl;
    //U64Var = 18;  //***
     cin>>U64Var;
    poDcc->setAvpValue("Service-Information.Micropayment-Info.Deduct-Amount", U64Var);
	
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
    char sTmpName[110];
	time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
    snprintf(sTmpName,sizeof(sTmpName),"%s%010d%s%04d\0","ZFP@001.ChinaTelecom.com;",tTime,";",iSeq);
    sVar.assign(sTmpName);
    //sVar.assign("ZFP@001.ChinaTelecom.com;1306303334;90733"); //*****
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("ZFP@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);   
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);  
    sVar.assign("ABM@ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("Balance_Trans.Micropayment@001.ChinaTelecom.com");
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
