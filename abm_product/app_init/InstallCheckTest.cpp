#include "InstallCheck.h"
#include "Log.h"


int main(int argc, char* argv[])
{
	InstallCheck *pCheck = new InstallCheck();
	if(pCheck)
	{
		int res = pCheck->checkAllParam();
		if(res != 0){
			Log::log(0,"有核心参数检验失败!");
			return 1;
		} else {
			Log::log(0,"核心参数检验成功!");
		}
		bool mem = true;
		if(argc>1 && strcmp(argv[1],"-ds")==0)
			mem = false;
		if(!pCheck->initprocess(mem)){
			Log::log(0,"有程序启动失败!");
			return 1;//非0失败
		}
	} else {
		Log::log(0,"内存不够,请检查!");
	}
	return 0;
}


