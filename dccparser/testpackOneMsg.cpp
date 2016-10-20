#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/timeb.h>
#include "DccMsgParser.h"

#define MAXSIZE 1024

using namespace std;

int          iVar;
long         lVar;
ACE_UINT32   U32Var;
ACE_UINT64   U64Var;
std::string       sVar;
    

void disp_buf(char *buf, int size)
{
  int i;
  printf("buf [%d] start:\n", size);
  for( i = 0 ; i < size; i ++ )
  {
  	if( i%16 == 0 )
  		printf("\n");
  	printf("%02X ", buf[i]&0xff);
  }
  printf("\nbuf end\n");
}


void packQryUser(DccMsgParser *poDcc)
{
    sVar.assign("request-20110505150000");
    poDcc->setAvpValue("Service-Information.Request-Id", sVar);
    
    iVar = 1;
    poDcc->setAvpValue("Service-Information.Destination-Type", iVar);
    
    sVar.assign("18955558888");
    poDcc->setAvpValue("Service-Information.Destination-Account", sVar);
    
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Destination-Attr", iVar);
    
    sVar.assign("platform--02");
    poDcc->setAvpValue("Service-Information.Service_Platform_ID", sVar);
    
    U32Var = 2;
    poDcc->setAvpValue("Service-Information.QueryType", U32Var);
}

int main(int argc,char* argv[])
{
    
    char sDictFile[256];
    char *p;
    int iResult;
    std::string sResult;
    DccMsgParser *poOcpMsgParser;
    

    /*string sSessionID="SCP001.chinatelecom.com;3463088662;9999999999;00000001";
    string sOrgHost="SCP001.chinatelecom.com";
    string sOrgRealm="chinatelecom.com";
    string sDestRealm="www.lianchuang.com";
    string sServContext="voice@chinatelecom.com";*/
 
#ifdef TEST_EFFORT
   struct timeval start_timeval , end_timeval;
   long diff_time = 0;
#endif   
    /*
    if ((p=getenv ("ABMAPP_DEPLOY")) == NULL)
    {
      cout<<"Please set env value ABM_HOME. For example  export ABM_HOME=/home/abm/ABM_HOME"<<endl;
      return -1;
	}
	sprintf (sDictFile, "%s/config/dictionary.xml", p);
	*/
	sprintf(sDictFile, "/abmapp/hss/app/dccparser/telecom/dictionary.xml");
    

    poOcpMsgParser = new DccMsgParser(sDictFile);
    if (poOcpMsgParser == NULL) {
        cout<<"new error"<<endl;
        exit(-1);
    }
    poOcpMsgParser->setDiameterHeader(272,true,4,3,4);
    poOcpMsgParser->releaseMsgAcl();
    
    
#ifdef TEST_EFFORT
      gettimeofday(&start_timeval, NULL);
#endif

    // set_avp_value
    //============================================================
    sVar.assign("session");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("SCP001.chinatelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("www.abc.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);
    sVar.assign("ocs2");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);
    sVar.assign("www.lianchuang.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);
    iVar=4;
    poOcpMsgParser->setAvpValue("Auth-Application-Id",iVar);
    sVar.assign("voice@chinatelecom.com");
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
    
    packQryUser(poOcpMsgParser);
    
    //=============================================================
    //end set_avp_value

#ifdef TEST_EFFORT
        gettimeofday(&end_timeval, NULL);
        std::cout << "########:setAvpValue---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
  			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
        gettimeofday(&start_timeval, NULL);
#endif

    sResult.clear();
    iResult=poOcpMsgParser->parseAppToRaw(sResult);
    if (iResult<=0){
    	printf("pack error.\n");
    	delete poOcpMsgParser;
    	exit(1);
    }
    disp_buf((char *)sResult.c_str(),sResult.size());
    
#ifdef TEST_EFFORT
        gettimeofday(&end_timeval, NULL);
        std::cout << "########:parserApptoRaw---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
  			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
#endif
    

//    iResult = poOcpMsgParser->parseRawToApp((char *)sResult.c_str(), sResult.size());
//    if (iResult<0){
//      printf("parser error.\n");
//      delete poOcpMsgParser;
 //     exit(1);
//    }
//    poOcpMsgParser->getAllAvp(true);
    delete poOcpMsgParser;
    exit(0);
    
}
