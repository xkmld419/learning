/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "Environment.h"
#include "Process.h"
#include "Log.h"
#include "MBC.h"

#include "Socket.h"

Socket gSocket;

const char *LOGDIR = "/home/bill/TIBS_HOME/log";
const char *LOGNAME = "/home/bill/TIBS_HOME/log/fee.log";

FILE *fp = NULL;
static int giCnt = 0;
char sIP[16];

struct STRPROCESS
{
    unsigned char sProcess[21];
    int  iLen;
};

struct STRPROCESS *gpProcess = NULL;

class AppLog: public Process
{
	public:
		int run();
	private:
		void InitProcess();
};

void AppLog::InitProcess()
{
        int i=0;
        char * sSql = "Select lpad(strlen,5,'0')||process send_buff, \
            length(lpad(strlen,5,'0')||process) buff_len From ( \
            Select '|'||a.process_id||'|0|' process, length('|'||a.process_id||'|0|')+5 strlen \
            From wf_process a,wf_application b where a.app_id=b.app_id and b.app_type=0 Order By billflow_id, process_id)";

	DEFINE_QUERY(qry);

        qry.close();
        qry.setSQL(sSql);
        qry.open();

        while (qry.next()) {
            giCnt++;
        }

        if (giCnt>0) {
            gpProcess = (STRPROCESS *) malloc (giCnt*sizeof(STRPROCESS));
            qry.close();
            qry.setSQL(sSql);
            qry.open();
            while( qry.next()){
              strcpy((char *)(gpProcess[i].sProcess),qry.field("send_buff").asString());
              gpProcess[i].iLen = qry.field("buff_len").asInteger();

              i++;
            }
            qry.close();
        }
}

int AppLog::run()
{

    unsigned char sBuff[1024]; 
    char sCommand[20];
    int iLen, iSocketFd, i;
    DIR *dirp;

    if (g_argc<2){
        printf("\tUsage: applog ip_address\n");
        return 0;
    }
    
    strcpy (sIP, g_argv[1]);
        
    InitProcess();

        
    //删除日志文件
    sprintf(sCommand, "rm %s", LOGNAME);
    system(sCommand);

    dirp = opendir(LOGDIR);
    if (dirp == NULL) {
        mkdir (LOGDIR, 0777);
        chmod (LOGDIR, 0777);
    }

    fp = fopen(LOGNAME,"w");

    if (fp == NULL) {
        printf("logfile can't open!\n");
        exit (0);
    }

    printf ("Connect to %s:1214\n", sIP);

    iSocketFd = gSocket.connect(sIP, 1214);

    if (iSocketFd > 0)
        printf ("Connect ok\n");
    else {
        printf ("Connect failed\n");
        exit (1);
    }    
    
    for (i=0; i<giCnt; i++)
    {
        gSocket.write(gpProcess[i].sProcess, gpProcess[i].iLen);

        gSocket.read (sBuff, 6);
        iLen = atoi ((char *)sBuff)-6;

        gSocket.read (sBuff, iLen);
        sBuff[iLen] = 0;

        fprintf(fp, "%s\n", sBuff);

        printf ("Recv:[%s]\n", sBuff);
    }

    fclose(fp);

    sprintf(sCommand, "chmod 777 %s", LOGNAME);
    system(sCommand);
    return 0;
}
DEFINE_MAIN (AppLog);
