#include "CHssProcessMgrTask.h"
#include "CmdMsg.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
extern char ** g_argvex;
using namespace std;
//##ModelId=4C06172C0394
CHbProcessMgrTask::CHbProcessMgrTask(){}
//##ModelId=4C06172C03E6
CHbProcessMgrTask::~CHbProcessMgrTask()
{
}

//##ModelId=4C0617C0012F
bool CHbProcessMgrTask::Init()
{	
	return true;
}

//##ModelId=4C0617B2025C
bool CHbProcessMgrTask::getTaskResult(CmdMsg *pMsg, long& lMsgID,variant& vtRet,int*  iRetSize){
	long lCmd = 	pMsg->GetMsgCmd();
	string strUrl = pMsg->GetMsgSrc();
	variant vt;
	pMsg->GetMsgContent(vt);
	int iVariantSize=pMsg->GetVariantSize(vt);
	//long lKey = atol(strUrl.c_str());
	if((MSG_PROCESS_STOP_SIG == lCmd)||
		(MSG_GAME_OVER==lCmd))
	{
		if(strUrl.empty())	
		{
			//没有指定进程号,CmdParser自己退出
			Log::log(0,"有命令要求我退出:老板ID :%d,自己ID:%d",pMsg->GetMsgDest(),				getpid());
			exit(0);
		}else
		{
			int iPid = atoi(strUrl.c_str());
			return killProcess(iPid);
		}
	}else
	{
		if(MSG_PROCESS_START_SIG == lCmd)
		{
			if(vt.isPtr()&&(iVariantSize== sizeof(ProcessExecInfo)))
			{
				ProcessExecInfo* pProcInfo=(ProcessExecInfo*)vt.asPtr();
				int iPid;
				bool bRet = startProcess(pProcInfo,iPid);
				vtRet.change_type(variant::vt_long);
				vtRet = iPid;
				return bRet;
				//	return startProcess(pProcInfo,0);
			}else
			{
				Log::log(0,"参数错误");
			}
		}
	}	return true;
}
//##ModelId=4C061A4602E0
bool CHbProcessMgrTask::killProcess( int iPid)
{
	return true;
}
//##ModelId=4C061A5A00FD
bool CHbProcessMgrTask::startProcess(ProcessExecInfo* pProcInfo,  int& iProcID)
{
	if(0 ==pProcInfo)
		return false;
	pid_t iPid ;
	sleep(20);
	switch(iPid=fork())
	{
		case 0:
		{
		sleep(1);
		string strProcess=pProcInfo->sProcessPath;
		if(pProcInfo->iProcType==1)
		{
			execlp("/bin/sh","sh","-c",pProcInfo->sArgv[1],NULL);
		}else
		{	
		strProcess.append(pProcInfo->sProcessName);
		if(execl(strProcess.c_str(),pProcInfo->sArgv[0],pProcInfo->sArgv[1],NULL)<0)
			switch(errno)
			{
				case ENOENT:
					Log::log(0,"COMMAND OR FILENAME NOT FOUND");
					break;
				case EACCES:
					Log::log(0,"YOU DO NOT HAVE RIGHT TO ACCESS");
					break;
				default:
					Log::log(0,"SOME ERROR HAPPENED IN EXEC\n");
	
			}
			exit(3);
		}
		}
		case -1:
		{
			return false;
		}
		default:
		{
			iProcID = iPid;
			pProcInfo->iPid=iPid;
			if((pProcInfo->iRunOnce==1)||(pProcInfo->iProcType==1))
				wait(0);
		}
	}
	return true;
}


