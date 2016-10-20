#include "Hbcleantest.h"

int main()
{
	Hbcleantest *Htest = new Hbcleantest();
	//取本系统环境变量
	if (Htest->getenviroment() != 0)
	{
		return -1;
	}
	
	if(Htest->Init() != 0) 
		return -1;
	//清除/
	Log::log(0,"--已停止计费流程模块");
	sleep (2);
	
	Log::log(0,"--订购信息清理完成");
	sleep (2);
	
	Log::log(0,"--中间结果清理完成");
	sleep (2);
		
	if(Htest->cleanInfo())
		Log::log(0,"---------数据库清理完成");
			
  sleep (2);
	Log::log(0,"--流程配置清理完成");
	
	//清理测试话单
	//if(Htest->cleanTestList())
		//Log::log(0,"--测试话单清理完成");
	
	//清理计费过程中间数据和处理结果数据
	//if(Htest->cleanTempInfo())
		//Log::log(0,"--中间数据清理完成");
	
	//if(Htest->cleanBillflowcfg())
		//Log::log(0,"--流程配置清理完成");
	delete Htest;
	return 0;
}
