#include "MessageQueue.h"
#include <iostream>
#include <fcntl.h>
#include "ABMException.h"
#include <sys/msg.h>
#include <signal.h>
#include <string.h>


#include "mntapi.h"

#include "ProcessInfo.h"
//#include "abmaccess.h"
//#include "TOCIQuery.h"
//#include "OracleConn.h"

//#include "ABMClntCfg.h"
//#include "ReadAuditCfg.h"


class A{
public :
	A(){};
	~A(){};
	void aaa(int aa){ a=aa;}
	int bb(){return a;}
private:
int a;
int b;

};
   
void alarm(int i){
    DisConnectMonitor();
    DisconnectTerminalMonitor();
    exit(0);
    }

int main()
{
    int key = 66;
    char sName[128] = {0};
    char sPwd[128] = {0};
    char sDsn[128] = {0};
    long fd=0;
    long laudit_acct_item_id=0;
    int iaudit_flag=0;
    ABMException oExp;
    
 
    
    
    A AA;
    AA.aaa(8);
    int bbb=AA.bb();
    A BB;
    
    bbb=BB.bb();
    
    BB.aaa(9);
    bbb=BB.bb();
    
    static char g_sSockColName[][16] = {
    "linkid",
	"proc",
	"locip",
	"locport",
	"remip",
	"remport",
	"recvmq",
	"sendmq",
	"option",
    "ftpkey"
};

    char row[5][32];
    char *f="*|60040|30000|6003031|0|180|";
    strncpy(row[0],f,8);
    row[0][8]='\0';
    
    strncpy(row[1],f,10);
    row[1][10]='\0';
    
    
    
    int bbbb=AA.bb();
    
    struct msgsbuf {
        long mtype;
        long mdate[1];
    }msg_sbuf;
    long aaa=0;
    
    if(!ConnectMonitor(99999)){
       return -1;
    }
    ConnectTerminalMonitor(99999);
    MessageQueue *a_acctMsgRecv = new MessageQueue(60);
    
        if (a_acctMsgRecv->open(oExp, true, true) != 0)  {
        cout<<"failed to open MessageQueue...\n";
        return -1;
    }
    //signal(SIGTERM,alarm);
    signal(SIGUSR1,alarm);
    /*
    long aaa=0;
    time ((time_t *)&aaa);
    
    while(true) {//第1次*/
        cout<<"1begin...\n";
        for(long tti=1;tti<=100000;tti++){
        	cout<<aaa<<"第"<<tti<<"次...\n";
        	//msg_sbuf.mdate[0]=tti;
        	//a_acctMsgRecv->Send(&msg_sbuf, sizeof(msg_sbuf));
        	
        	}
        	
       // break;
    //}
    //long aaaa=a_acctMsgRecv->getSysID();
    //cout<<aaaa;
    //msgctl(aaaa,IPC_RMID,NULL);
    //a_acctMsgRecv->remove(oExp);
    //DisConnectMonitor();
    //DisconnectTerminalMonitor();
  
   
    
    
    return 0;
}

/*
int main(){
        ABMException oExp;
        char sFileName[1024];
        char *penv = getenv("ABMAPP_DEPLOY");
        if (!penv) {
            ADD_EXCEPT0(oExp, "miss environment ABMAPP_DEPLOY, please add it to your.profile");
            return 0;
        }
        
        if (penv[strlen(penv)-1] != '/') {
            snprintf (sFileName, sizeof(sFileName)-1, "%s/config/abm_app_cfg.ini",penv);
        } else {
            snprintf (sFileName, sizeof(sFileName)-1, "%sconfig/abm_app_cfg.ini",penv);
        }
        char sValue[64];
        char ssssss[64];
        ReadCfg monitorCfg;
        ReadCfg infoCfg;
        memset(sValue, 0x00, sizeof(sValue));
        if (monitorCfg.read(oExp, sFileName, "RTEST" , '|') == -1) {
            ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"SYSTEM_MONITOR\", '|') failed",sFileName);
            return -1;     
        }
        memset(sValue, 0x00, sizeof(sValue));
        if (!monitorCfg.getValue(oExp, sValue, "proc", 1)) {
                ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed","proc", 1);
                return -1;     
        }
        strncpy(ssssss, sValue,sizeof(ssssss)-1);
      return 0;
}
*/