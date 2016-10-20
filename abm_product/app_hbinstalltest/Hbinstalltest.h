#ifndef _HBINSTALLTEST_H
#define _HBINSTALLTEST_H


#include <netinet/in.h>
#include <unistd.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Log.h"
#include "MBC.h"
#include "Environment.h"
#include "../app_guard/ProcessInfo.h"
#include "ReadIni.h"



//测试进程
#define GATHER_PROCESS 51
#define PREP_PROCESS 52
#define FORMAT_PROCESS 53
#define DUP_PROCESS 54
#define PRICING_PROCESS 55
#define WRITEFILEDB_PROCESS 56
#define FILEINSTORE_PROCESS 57

#define ALLPATH 1024

#define ST_INIT        1
#define ST_RUNNING     2
#define ST_WAIT_BOOT   3

class Hbinstalltest
{
	private:
		
	public:
		Hbinstalltest();
		~Hbinstalltest();
		
		int beginRunPro(int iProcessId);
		int Init();
		int getenviroment();
		bool writeTestList();
		bool createInfo();
		bool printTestinfo();
		int processState(int m_process);
		bool billflowCfg();
		void checkTableByInfo();
		
		char h_path[100];		//主机系统路径
		char h_user[20];			//主机用户
		char h_hostip[20];	//主机IP
		char h_hostname[100]; //主机名
		
		char GATHER_PART_ROOT[ALLPATH];
		char GATHER_PART_NODE[ALLPATH];
		char TEST_LIST[ALLPATH];
		char LIST_NAME[ALLPATH];
		char SQL_USERLOAD[ALLPATH];
		char SQL_BILLFLOWCFG[ALLPATH];
		char GATHER_PART_NODE2[ALLPATH];
		char DUPPATH[ALLPATH];
		
		char SET[ALLPATH];
		char BILLFLOWRUN[ALLPATH];
		char BILLFLOWDOWN[ALLPATH];
		char DBPATH[ALLPATH];
		
		int stopPro(int iProcessId);
		
		ReadIni	RD;	
		
		int getFullPath(const char * str1,const char * str2,const char * str3,char * pp,const char * scat,int pro=0);
		
};

#endif
