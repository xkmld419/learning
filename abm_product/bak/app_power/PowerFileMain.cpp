#include "PowerFile.h"
#include "PowerXml.h"

int main()
{	
	PowerFile a;	

	Log::log(0,"开始处理文件，每15分钟扫描一次...");
	while (1){
		a.getFile();
		a.setUserInfo();
		a.MainProcess();	
		a.sendFile();
		Date d;
		//Log::log(0,"系统运行中，当前时间:%s",d.toString("yyyy-mm-dd hh:mi:ss"));
		sleep(15*1);//每15分钟处理一次
		//sleep(15*60);//每15分钟处理一次
		//exit(0);//-----------------------------------------
	}
	return 0;
}
