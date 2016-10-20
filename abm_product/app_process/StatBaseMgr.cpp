#include "StatFormBase.h"
#include "StatBaseMgr.h"

#include "OnlineTime.h"
//#include "ProcessRuning.h"
#include "MsgQueue.h"
#include "ProcessSes.h"
#include "LoggInfo.h"
#include "UserLogin.h"
#include "AbmHandlInfo.h"
//#include "AllStatInfo.h"
#include "SHMInfoShow.h"
//#include "SgwStatMem.h"

#include "ThreeLogGroup.h"

StatBaseMgr::StatBaseMgr()
{
	pDraw = NULL;
}

StatBaseMgr::~StatBaseMgr()
{
	if(pDraw != NULL)
		delete pDraw;
}
int StatBaseMgr::run()
{
	string sErrMsg;
	string sAgrv;
	try{
		if(argc<4||argc>4){
			PrintUsage();
			return -1;
		}
		pDraw = NULL;
		for(int i=1; i<g_argc; i++)
		{
			if(g_argv[i][0] != '-'){
				PrintUsage();
				return -1;
			}
			sAgrv = g_argv[i];
			if( sAgrv == "-a" )
			{
				PrintUsage();
				return 0;
			}
			else if(sAgrv == "-s")
			{
				pDraw = new OnlineTime();
			}
			else if(sAgrv == "-r")
			{
				//pDraw = new ProcessRuning();
			}
			else if(sAgrv == "-Q")
			{
				pDraw = new MsgQueue();
			}
			else if(sAgrv == "-ses")
			{
				pDraw = new ProcessSes();
			}
			else if(sAgrv == "-seg")
			{
				pDraw = new SHMInfoShow();
			}
			else if(sAgrv == "-L")
			{
				pDraw = new LoggInfo();
			}
			else if(sAgrv == "-u")
			{
				pDraw = new UserLogin();
			}
			/*
			else if(sAgrv == "-p")//当天
			{
				StatFormBase::SetShowFlag(ON_TODAY);
				pDraw = new AllStatInfo();
			}
			*/
			else if(sAgrv == "-P")//启动以来
			{
				pDraw = new AbmHandlInfo();
			}
			/**
			else if(sAgrv == "-o")//当前小时
			{
				StatFormBase::SetShowFlag(ON_HOUR);
				pDraw = new AllStatInfo();
			}
			*/
			else if(sAgrv == "-t")
			{
				i++;
				if(i<=g_argc)
					StatFormBase::SetInterval( atoi(g_argv[i]) );
			}
			else if(sAgrv == "-m")
			{
				/**
				i++;
				if(i<=g_argc)
				{
					sAgrv = g_argv[i];
					StatFormBase::SetMoudle( (char*)sAgrv.c_str() );
				}
				**/
			}
			else
			{
				printf("不支持的参数选项，请参考abmstat -a\n\n");
				return -1;
			}
		}
		if(!pDraw)
		{
			//printf("不支持的参数选项，请参考abmstat -h\n\n");
			PrintUsage();
			return -1;
		}
		//pDraw->StartWin();
		pDraw->Build();
		pDraw->EndWin();
	}
	catch(string ex){
		sErrMsg.append("string error:").append(ex);
	}
	catch(TOCIException &ex){
		sErrMsg.append("TOCIException error:").append(ex.getErrMsg()).append(ex.getErrSrc());
	}
	catch(TException &ex){
		sErrMsg.append("TException error:").append(ex.GetErrMsg());
	}
	catch(exception &ex){
		sErrMsg.append("exception error:").append(ex.what());
	}
	catch(...){
		sErrMsg.append("unkown error");
	}
	if(!sErrMsg.empty())
	{
		printf("发生错误:%s,程序退出!\n", sErrMsg.c_str());
		return -1;
	}
	return 0;
}


void StatBaseMgr::PrintUsage()
{
	printf("\nabmstat useage:\n\n" 
"		-a		列出所有选项 \n"
"		-s		列出当前系统的level0状态和SGW-ONLINE\n"
"				自上次启动以来运行了多长时间（格式：***日**时**分**秒）   \n"
"		-t		每隔秒后重复选项                                          \n"
"		-P		显示自启动以来累计的统计信息，内容至少包括：（请求数、    \n"
"				请求成功数、数据量、能力调用次数、调用成功数）            \n"
"		-Q		显示SGW系统消息队列的使用情况                             \n"
"		-u		显示今日凌晨到目前系统用户登录情况                        \n"
"		-ses		显示系统进程/线程情况，含义同abmprocess –a                \n"
"		-seg		显示当前内存的使用状况，含义同abmshm –stat                \n");
}


DEFINE_MAIN(StatBaseMgr);


