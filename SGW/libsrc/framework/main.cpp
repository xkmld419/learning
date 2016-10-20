#include "Process.h"
#include "Log.h"

Process *GetProcess();

extern int g_argc;
extern char ** g_argv;
extern Process *g_pProc;

int main(int argc, char **argv)
{
//	char sString[1024];

//	try
//	{

		//# 全局命令行参数, 子进程如果要使用, 通过extern的方式传递
		g_argc = argc;
		g_argv = argv;

		//# 获取子类地址, GetProcess 是子类的全局函数
		//# 子类中定义宏: DEFINE_MAIN(class_name) 即可,由该宏去提供GetProcess函数
		g_pProc = GetProcess();
		//# run() 为虚函数, 动态执行子类的run()方法
		g_pProc->run();

		delete g_pProc;

		NormalExitProcess();

	}
/*
	//# OCI Exception
	catch (TOCIException &e)
	{
		cout << e.getErrMsg() << e.getErrSrc() << endl;
		if (g_pProc)
		{
			delete g_pProc;
			g_pProc = 0;
		}

		Log::log(0, "收到TOCIException退出\n[TOCIEXCEPTION]: TOCIException catched\n"
			"  *Err: %s\n"
			"  *SQL: %s", e.getErrMsg(), e.getErrSrc());

		//		Log::log (0, sString);

		try
		{
			//Pprintf (0, 1, 0, sString);
		}
		catch (...)
		{
		}

		AbortProcess(-1); //异常退出,告知流程调度模块
	}

	//# TException
	catch (TException &e)
	{
		if (g_pProc)
		{
			delete g_pProc;
			g_pProc = 0;
		}
		Log::log(0, "收到Exception退出\n[EXCEPTION]: %s", e.GetErrMsg());
		//		Log::log (0, sString);

		try
		{
			//Pprintf (0, 1, 0, sString);
		}
		catch (...)
		{
		}

		AbortProcess(-2);
	}

	//# Exception
	catch (Exception &e)
	{
		if (g_pProc)
		{
			delete g_pProc;
			g_pProc = 0;
		}
		Log::log(0, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
		//		Log::log (0, sString);

		try
		{
			//Pprintf (0, 1, 0, sString);
		}
		catch (...)
		{
		}

		AbortProcess(-3);
	}
	//#TimesTen  Exception
	catch (TTStatus st)
	{
		cerr << "Error in  " << st << endl;
		Log::log(0, "%s", st.err_msg);
		AbortProcess(-4);
	}
	//# Other Exception
	catch (...)
	{
		if (g_pProc)
		{
			delete g_pProc;
			g_pProc = 0;
		}
		Log::log(0, "收到其他Exception退出");
		//		Log::log (0, sString);

		try
		{
			//Pprintf (0, 1, 0, sString);
		}
		catch (...)
		{
		}

		AbortProcess(-5);
	}

}*/

