
#include "CheckPoint.h"
#include "Log.h"
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

#define MAXFILE	65535

//使用服务模式启动
void StartAsService(void)
{
	//按照服务模式启动
	pid_t pid;
	int i;
	pid=fork();
	if(pid<0){
		printf("error in fork\n");
		exit(1); 
	}else if(pid>0) 
		exit(0); 
	if(setsid()<0)
	{
		printf("error in setsid\n"); 
  		exit(0); 
	} 
	chdir("/");
	umask(0);
	for(i=0;i<MAXFILE;i++)
		close(i);
}


int main (int argc, char **argv)
{
	Log::log (0,"[Message]: 当前进程 PID = %d", getpid());
	/*for(int i=1;i<argc;i++)
	{
		string strargv= argv[i];
		transform(strargv.begin(),strargv.end(),strargv.begin(),::tolower);	
		if(strargv.compare("-service")==0 ||(strargv.compare("service")==0)||strargv.compare("-s")==0 )
		{
			StartAsService();
		}
	}*/
	if(argc>1)
	{
		string strargv= argv[1];
		transform(strargv.begin(),strargv.end(),strargv.begin(),::tolower);	
		if(strargv.compare("-service")==0 || strargv.compare("service")==0)
		{
			StartAsService();
		}
	}
	Log::log(0,"准备启动");
	char sString[4096];
  	try {
        CheckPoint *p = new CheckPoint ();
		if(p)
		{
			//这个是内存数据落地的实际执行函数
			Log::log(0,"开始运行checkpoint程序!");
			p->run();
      	} else {
      		Log::log(0,"内存空间不够,请检查!");
      		return 0;
      	}
        printf("应用程序正常退出!\n");
        return 0;
		
   }
   
  //OCI Exception
  catch (TOCIException &e) 
  {
        cout<<e.getErrMsg()<<e.getErrSrc()<<endl;
        snprintf (sString,4095, "收到TOCIException退出\n[TOCIEXCEPTION]: TOCIException catched\n"
                "  *Err: %s\n"
                "  *SQL: %s", e.getErrMsg(), e.getErrSrc());
        Log::log (0, sString);  
  }
  //# TException
  catch (TException &e) {
        snprintf (sString,4095, "收到TException退出\n[EXCEPTION]: %s", e.GetErrMsg());
        Log::log (0, sString);
  }
  //# Exception
  catch (Exception &e) {
        snprintf (sString,4095, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
        Log::log (0, sString);
  }
  //# Other Exception
  catch (...) {
        snprintf (sString,4095, "收到其他Exception退出");
        Log::log (0, sString);
  }

}

