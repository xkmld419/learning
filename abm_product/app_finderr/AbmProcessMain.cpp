#include <unistd.h>
#include <iostream>
#include "AbmProcess.h"

using namespace std;

bool PrintUsage();
int main(int argc,char ** argv)
{
	char opt;
	bool bRun=false,bStop=false;
	int iProcessId=0,iBillFlowID=0;
	//int bIsLogin;
	char sAppName[24];
	//bIsLogin=-1;
	memset(sAppName,0,sizeof(sAppName));
	ABMProcess *pABMP=new ABMProcess();	
	if(argc<4||argc>4)
	{
		PrintUsage();
		return -1;
	}
	
	if((strcmp(argv[1],"-o")==0)&&(strcmp(argv[2],"-p")==0))
	{
		iProcessId=atoi(argv[3]);
		if(pABMP->RunProcess(iProcessId)!=0)
		{
			cout<<"程序启动失败"<<endl;
		}
		return 0;
	}
	
	
	if((strcmp(argv[1],"-c")==0)&&(strcmp(argv[2],"-p")==0))
	{
		iProcessId=atoi(argv[3]);
		if(pABMP->StopProcess(iProcessId)!=0)
		{
			cout<<"程序停止失败"<<endl;
		}
		return 0;
	}
	
	
	if((strcmp(argv[1],"-a")==0)&&(strcmp(argv[2],"-p")==0))
	{
		iProcessId=atoi(argv[3]);
		if(pABMP->ShowProcess(iProcessId)!=0)
		{
			cout<<"程序信息查找失败"<<endl;
		}
		return 0;
	}
	
	
	if((strcmp(argv[1],"-a")==0)&&(strcmp(argv[2],"-m")==0))
	{
		strcpy(sAppName,argv[3]);
		if(pABMP->ShowProcessInfoByApp(sAppName)!=0)
		{
			cout<<"按模块查找信息失败"<<endl;
		}
		return 0;
	}
	
	
	PrintUsage();
	delete pABMP;
	pABMP=NULL;
	return 0;
		
}


bool PrintUsage()
{
		printf("\n *******************************************************************************************   \n") ;
		cout <<"usage: " <<"abmprocess"<<" [-a|-o|-c]| [-p|-m] \n";
		cout<<"abmprocess"<<" -a	显示信息包括进程ID、进程名称、进程/线程状态、CPU占用、内存占用、IO信息等"<<'\n';
		cout<<"abmprocess"<<" -o 启动进程"<<'\n';
		cout<<"abmprocess"<<" -c 停止进程"<<'\n';
		cout<<"abmprocess"<<" -m 后跟模块名称,按照模块显示进程信息"<<'\n';
		
		cout<<"abmprocess"<<" [-o|-c] -p 后跟进程ID，表明只指对此进程ID操作"<<'\n';
		cout<<"abmprocess"<<" [-a] [-p|-m]  后跟进程ID或者模块名称，显示进程详细信息"<<'\n';
		
		printf("\n *******************************************************************************************   \n") ;
		return true;
}
