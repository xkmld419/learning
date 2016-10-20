/*VER: 1*/ 
//不需要以来monitor等的 main
#include "ProcessEx.h"
#include "Log.h"
#include <algorithm>
#include <functional>

ProcessEx *GetProcess ();

extern int g_argcex;
extern char ** g_argvex;
extern ProcessEx *g_pProc;

#include<sys/types.h>
#include <sys/stat.h>
#define MAXFILE 65535

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
	setsid();
	/*
	chdir("/");
	umask(0);
	*/
	for(i=0;i<MAXFILE;i++)
		close(i);
}
int main (int argc, char **argv)
{
	for(int i=1;i<argc;i++)	{
		string strargv= argv[i];
		transform(strargv.begin(),strargv.end(),strargv.begin(),::tolower);	
		if(strargv.compare("-service")==0 ||(strargv.compare("service")==0)){
			StartAsService();
		}
	}
	char sString[4096]={0};
	try {

		//# 全局命令行参数, 子进程如果要使用, 通过extern的方式传递
		g_argcex = argc;
		g_argvex = argv;

		//# 获取子类地址, GetProcess 是子类的全局函数
		//# 子类中定义宏: DEFINE_MAIN(class_name) 即可,由该宏去提供GetProcess函数
		g_pProc = GetProcess ();
		//g_pProc->Init(argv[0]);

		//TODO ::init没有日志文件名
		Log::log (0,"[Message]: 当前进程 PID = %d", getpid());

		//# run() 为虚函数, 动态执行子类的run()方法
		g_pProc->run();

		delete g_pProc;

		Pprintf(0,1,0,"应用程序正常退出！");
		return 0;

	}

	//# OCI Exception
	catch (TOCIException &e) {
		cout<<e.getErrMsg()<<e.getErrSrc()<<endl;
		if (g_pProc) {
			delete g_pProc;
			g_pProc = 0;
		}

		snprintf (sString,4095, "收到TOCIException退出\n[TOCIEXCEPTION]: TOCIException catched\n"
			"  *Err: %s\n"
			"  *SQL: %s", e.getErrMsg(), e.getErrSrc());

		Log::log (0, sString);

		DB_LINK->rollback ();
		//Pprintf (9011, 1, 0, sString);      

	}

	//# TException
	catch (TException &e) {
		if (g_pProc) {
			delete g_pProc;
			g_pProc = 0;
		}
		snprintf (sString,4095, "收到TException退出\n[EXCEPTION]: %s", e.GetErrMsg());
		Log::log (0, sString);

		DB_LINK->rollback ();
		Pprintf (9012, 1, 0, sString);
		AbortProcessEx(-2);
	}

	//# Exception
	catch (Exception &e) {
		if (g_pProc) {
			delete g_pProc;
			g_pProc = 0;
		}
		snprintf (sString,4095, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
		Log::log (0, sString);

		if (DB_CONNECTED) {
			DB_LINK->rollback ();
		}

		Pprintf (9013, 1, 0, sString);
		AbortProcessEx(-3);
	}

	//# Other Exception
	catch (...) {
		if (g_pProc) {
			delete g_pProc;
			g_pProc = 0;
		}
		snprintf (sString,4095, "收到其他Exception退出");
		Log::log (0, sString);

		DB_LINK->rollback ();
		Pprintf (9014, 1, 0, sString);
		AbortProcessEx(-4);
	}

}

