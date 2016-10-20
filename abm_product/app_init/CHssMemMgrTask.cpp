#include "CHssMemMgrTask.h"
#include "SimpleSHM.h"
#include "Exception.h"

//##ModelId=4C0311EF02CB
CHbMemMgrTask::CHbMemMgrTask()
{
}


//##ModelId=4C0311EF02F9
CHbMemMgrTask::~CHbMemMgrTask()
{
}

//##ModelId=4C0311F603B9
bool CHbMemMgrTask::getTaskResult(CmdMsg* pMsg,long& lMsgID,variant& vtRet,int* piRetSize)
{
//Log::log(0,"莫要抢，莫要争，哥哥我在做,处理任务的类为 %s",typeid(this).name());
	if(0== pMsg)
		return false;

	long lCmd = 	pMsg->GetMsgCmd();
	string strUrl = pMsg->GetMsgSrc();
	if(strUrl.empty())
	{
		Log::log(0,"没有指定内存键值");
		vtRet =MBC_Server+1;
		return false;
	}
	
	variant vt;
	pMsg->GetMsgContent(vt);


	long lKey = atol(strUrl.c_str());
	if(MSG_MEM_CREATE == lCmd)
	{
		//创建共享内存
		long lSize= vt.asLong();
		return CreateShm(lKey,lSize);
	}else
	{
		if(MSG_MEM_DESTORY == lCmd)
		{
			//销毁内存
			return DestoryShm(lKey);
		}
	}
	return true;
}

//##ModelId=4C03121403D7
bool CHbMemMgrTask::Init()
{
	return CHbTaskBase::Init();
}

bool CHbMemMgrTask::DestoryShm(long lKey)
{
	SimpleSHM * pShmTmp = new SimpleSHM(lKey,0);
	try{
		pShmTmp->remove();
	}catch( Exception& e)
	{
		Log::log(0,"共享内存销毁失败");
		m_vtRet = MBC_Server+1;
		return false;
	}
	return true;
}
bool CHbMemMgrTask::CreateShm(long lKey,unsigned long size)
{
	//创建共享内存
	SimpleSHM * pShmTmp = new SimpleSHM(lKey,size);
	try
	{
		//打开共享内存
		pShmTmp->open(true);
		delete pShmTmp; pShmTmp=0;
	}catch( Exception& e)
	{
		Log::log(0,"共享内存创建失败");
		m_vtRet = MBC_Server+1;
		return false;
	}
	return true;
}
