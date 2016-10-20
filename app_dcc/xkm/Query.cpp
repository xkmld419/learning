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
#include "ReadDccConf.h"
#include "OracleConn.h"


#define MAXSIZE 1024

using namespace std;

int          iVar;
long         lVar;
//ACE_UINT32   U32Var;
//ACE_UINT64   U64Var;
std::string  sVar;
StruMqMsg struMsg;
char sValue[128];
char sareaValue[128];
char sNumarea[20];
char sValueT[128];


void packChargeR(DccMsgParser *poDcc)
{
    sVar.assign(sValue);

    //sVar.assign("charge-20110513000001");
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Id", sVar);
    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Id-Type", iVar);

    iVar = 2;
    poDcc->setAvpValue("Service-Information.Balance-Information.Destination-Attr", iVar);
    
    sVar.assign(sNumarea);
    poDcc->setAvpValue("Service-Information.Balance-Information.AreaCode", sVar);
    iVar = 0;
    poDcc->setAvpValue("Service-Information.Balance-Information.Query-Flag", iVar);
    
    iVar = 1;
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
    
	
    string sORACLEName = "ABM";
    string sORACLEuser = "abm1";
    string sORACLEpwd = "abm1";
        
    long laID;
	  OracleConn *pOracleConn = NULL;
    TOCIQuery *pQryCursor = NULL;    //select
    pOracleConn = new OracleConn;        
    if (!pOracleConn->connectDb(sORACLEuser, sORACLEpwd, sORACLEName)) {
         printf("ORACLE CONNECT ERROR");
         return  - 1;
    }
    DEFINE_OCICMDPOINTBYCONN(pQryCursor, (*pOracleConn));
    if (pQryCursor == NULL) {
        printf("pQryCursor ERROR");
        return  - 1;
    }
    

    
	
    int iRet;
    int iMsgLen;
	  char sTmpNUM[256];
	  string sTmpT;
	  poOcpMsgParser = new DccMsgParser(sDictFile);
	  while(1)
   {
	     ABMException oExp;
	     ReadCfg *m_poConfS = NULL; 
	     char * const pLable = "TEST_CONF";
       if (m_poConfS == NULL) { 
           if ((m_poConfS=new ReadCfg) == NULL) {
               ADD_EXCEPT0(oExp, "ABMConf::getConf m_poConfS malloc failed!");
               return -1;
           }
       }
    //char *p = getenv(ABM_HOME);
	
       char path[1024];
       snprintf(path, sizeof(path), "/ABM/app/deploy/config/abm_test_number.ini");
       if (m_poConfS->read(oExp, path, pLable) != 0) {
          ADD_EXCEPT2(oExp, "AccuConf::getConf 读取配置文件%s中的%s出错", path, pLable);
          return -1;
       }
    
       int iLines = m_poConfS->getRows();
	     int i = 0;
	     int j = 0;
	     int a = 0;
	     int b=0;
	     memset(sValueT, 0x00, sizeof(sValueT));
	
	     pid_t pid;
       int status,ixi;
       if(fork()==0){
          printf("This is the child process .pid =%d\n",getpid());
          system("/ABM/app/deploy/xkmtemp/xkm.sh");
          exit(5);
       }else{
          sleep(1);
          printf("This is the parent process ,wait for child...\n");
          pid=wait(&status);
          ixi=WEXITSTATUS(status);
          printf("child’s pid =%d .exit status=%d\n",pid,ixi);

   for(i;i<iLines;i++)
	{
	 j = i+1;
	 cout<<"处理的数据条数"<<j<<endl;
	 if (!m_poConfS->getValue(oExp, sValueT, "number", j)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现number选项,读取失败!", pLable);
            return -1;
        }
   pQryCursor->close();
    pQryCursor->setSQL("select org_area from abm_h_code t where head=SUBSTR(:p0,1,7) ");
    pQryCursor->setParameter("p0", sValue);
    pQryCursor->open();
    if (pQryCursor->next()) {
    	  strcpy(sNumarea,pQryCursor->field(0).asString());
    }
    pQryCursor->close();
	if (!m_poConfS->getValue(oExp, sareaValue, "area", j)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现number选项,读取失败!", pLable);
            return -1;
        }
	
	while(sValueT[a]){
	a++;
	}

	if(a>11){
	a=a-11;
	b=0;
	while(sValueT[a]){
	sValue[b]=sValueT[a];
	a++;
	b++;
	}
	}
	else{
	a=0;
	while(sValueT[a]){
	sValue[a]=sValueT[a];
	a++;
	}
	}
    //sTmpT.assign(sValue);
	//strncpy(sTmpNUM,(LPCTSTR)sTmpT,sizeof(sTmpNUM));  
	int iVar;

    
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
    snprintf(sTmp,sizeof(sTmp),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",iSeq);
    sVar.assign(sTmp);

    //sVar.assign("ABM@001.ChinaTelecom.com;1306302304;99999");
    poOcpMsgParser->setAvpValue("Session-Id",sVar);
    sVar.assign("ABM@001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Host",sVar);
    sVar.assign("001.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Origin-Realm",sVar);


    snprintf(sTmp,sizeof(sTmp),"%s%s%s\0","HB@",sareaValue,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"estination-Host为:"<<sTmp<<endl;
    //sVar.assign("HB@025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Host",sVar);

    snprintf(sTmp,sizeof(sTmp),"%s%s\0",sareaValue,".ChinaTelecom.com");
    sVar.assign(sTmp);
    cout<<"Destination-Realm为:"<<sTmp<<endl;
    //sVar.assign("025.ChinaTelecom.com");
    poOcpMsgParser->setAvpValue("Destination-Realm",sVar);

    snprintf(sTmp,sizeof(sTmp),"%s%s%s\0","Query.Balance@",sareaValue,".ChinaTelecom.com");
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

    //delete poOcpMsgParser;
	sleep(1);
  }  
 }
	sleep(900);
	
   }
    exit(0);
    
}
