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
//ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;





void packChargeR(DccMsgParser *poDcc)
{
   char sTmpName[110];
   cout<<"请输入用户号码"<<endl;
    cin>>sTmpName;
    sVar.assign(sTmpName);

    //sVar.assign("charge-20110513000001");
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Id", sVar);
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Id-Type", iVar);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Attr", iVar);
    
    cout<<"请输入区号"<<endl;
    cin>>sTmpName;
    sVar.assign(sTmpName);
    poDcc->setAvpValue("Service-Information.Balance-Information.AreaCode", sVar);
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Balance-Information.Query-Flag", iVar);
    
    cout<<"查询类型 0:所有  1:可用  2:可划拨"<<endl;
    cin>>iVar;
    poDcc->setAvpValue("Service-Information.Balance-Information.Query-Item-Type", iVar);
	
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
	time_t tTime;
	tTime = time(NULL);
    int iSeq=1;
    snprintf(sTmp,sizeof(sTmp),"%s%010d%s%04d\0","ABM2@001.ChinaTelecom.com;",tTime,";",iSeq);
    sVar.assign(sTmp);

    //sVar.assign("ABM2@001.ChinaTelecom.com;1306302304;99999");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("ABM2@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);

    cout<<"请输入Destination-Host 示例HB@025.ChinaTelecom.com 输入025"<<endl;
    cin>>iVar;
    snprintf(sTmp,sizeof(sTmp),"%s%03d%s\0","HB@",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"estination-Host为:"<<sTmp<<endl;
    //sVar.assign("HB@025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);

    snprintf(sTmp,sizeof(sTmp),"%03d%s\0",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"Destination-Realm为:"<<sTmp<<endl;
    //sVar.assign("025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);

    snprintf(sTmp,sizeof(sTmp),"%s%03d%s\0","Query.Balance@",iVar,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"Service-Context-Id为:"<<sTmp<<endl;
    poOcpMsgParser->setAvpValue("Service-Context-Id",sVar);
   

    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    


    

    sVar.assign("srv_flow_id");
    poOcpMsgParser->setAvpValue("Service-Flow-Id",sVar);
    sVar.assign("");
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
