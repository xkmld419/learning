#include "MessageQueueH.h"
#include <iostream>
#include <fcntl.h>
#include "ABMException.h"
#include <sys/msg.h>

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
   


int main(int argc, char *argv[])
{
    int key = 66;
    char sName[128] = {0};
    char sPwd[128] = {0};
    char sDsn[128] = {0};
    long fd=0;
    long laudit_acct_item_id=0;
    int iaudit_flag=0;
    ABMException oExp;
    int g_iProcID=0;
    int g_iNodeID=0;
    int oc;
    char * paraOpt = "p:P:n:N";
    while ((oc=getopt(argc, argv, paraOpt)) != -1) {
        switch (oc) {
        case 'p':
        case 'P':
            g_iProcID = atoi(optarg);
            break;
        case 'n':
        case 'N':
            g_iNodeID = atoi(optarg);
            break;
        default:
            printf("程序发现未知参数 %c\n", oc);
            break;
        }
    }
    
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
   
    if(!ConnectMonitor(g_iProcID)){
       return -1;
    }
    ConnectTerminalMonitor(g_iProcID);
   
    MessageQueue *a_acctMsgRecv = new MessageQueue(60);    
   if (a_acctMsgRecv->open(oExp, true, true) != 0)  {
        cout<<"failed to open MessageQueue...\n";
        return -1;
    }
    /*
    long aaa=0;
    time ((time_t *)&aaa);
    
    while(true) {//第1次*/
        cout<<"1begin...\n";
        for(long tti=1;tti<=10000000;tti++){
        	cout<<aaa<<"第"<<tti<<"次...\n";
        	msg_sbuf.mdate[0]=tti;
        	a_acctMsgRecv->Send(&msg_sbuf, sizeof(msg_sbuf));
        	sleep(10);
        	
        }
        	
       // break;
    //}
    //long aaaa=a_acctMsgRecv->getSysID();
    //cout<<aaaa;
    //msgctl(aaaa,IPC_RMID,NULL);
    //a_acctMsgRecv->remove(oExp);
    DisConnectMonitor();
    DisconnectTerminalMonitor();
  
   
    
    
    return 0;
}
