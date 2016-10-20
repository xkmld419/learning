#ifndef _ONLINEUPDATE_H_
#define _ONLINEUPDATE_H_

#include <string>
//#include <strings.h>
#include <map>
using namespace std;

class onlineUpdate
{
		public:
			onlineUpdate();
			~onlineUpdate();
			int GetUpdatePro(char* filepath,char* process);
			int GetUpdateFile(char* filepath,char* UpFile);
			int DisProcessId(int aAppid[1000],int count,int RunProID[1000],int &RunCount);
	    map <int,string> mapProcessID;
	    int StopProcess(int RunProcess[1024],int count,int iTimeOut);
	    int RunProcess(int RunProcess[1024],int count,int iTimeOut);
	    int GetProName(int aAppid[1000],int count,string sProName[1000],int &ProNaCount);
	    void log(int log_level, char const *format, ...);
	 private:
	 	  double get_time();
	  	
		
};

#endif
