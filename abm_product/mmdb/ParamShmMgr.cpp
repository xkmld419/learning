#ifndef PRIVATE_MEMORY
#include "ParamShmMgr.h"
#include "IpcKey.h"
#include "Log.h"
#include "MBC.h"
#include "fee_shm.h"


ParamShmMgr::ParamShmMgr()
{
    //初始化入口共享内存的key和信号量
    m_pParamAccessPort = NULL;
    m_pShmStateLockSem = NULL;
    m_lShmStateKey = IpcKeyMgr::getIpcKey(-1, "SHM_PARAMDATA_STATE");
    m_lShmStateLockKey = IpcKeyMgr::getIpcKey(-1, "LOCK_PARAMDATA_STATE");
    if(m_lShmStateKey > 0L && m_lShmStateLockKey > 0L)
    {
        m_pShmStateLockSem = new CSemaphore();
        if(NULL == m_pShmStateLockSem)
            THROW(MBC_Semaphore);                   //创建对象失败 请检查是否有足够的内存
    }
    else
    {
        Log::log(0, "[错误]:共享内存配置错误!请正确配置共享内存的KEY");
        THROW(MBC_ParamShmMgr+1);                   //参数访问入口的共享内存配置错误
    }

    //初始化参数信息块
    m_pParamDataBuf[PARAM_SHM_INDEX_A] = new ParamInfoMgr(PARAM_SHM_INDEX_A);
    m_pParamDataBuf[PARAM_SHM_INDEX_B] = new ParamInfoMgr(PARAM_SHM_INDEX_B);

    //根据参数信息块的编号初始化对应的参数块内的参数共享内存的key
    m_pParamDataBuf[PARAM_SHM_INDEX_A]->bindKey(PARAM_SHM_INDEX_A);
    m_pParamDataBuf[PARAM_SHM_INDEX_B]->bindKey(PARAM_SHM_INDEX_B);

    //根据参数信息块的编号连接对应的参数块内的参数共享内存
    m_pParamDataBuf[PARAM_SHM_INDEX_A]->attachAll();
    m_pParamDataBuf[PARAM_SHM_INDEX_B]->attachAll();


}

ParamShmMgr::~ParamShmMgr()
{
    //分离入口共享内存
    detachShmByData(m_pParamAccessPort);
    
    //删除入口共享内存的信号量
    if(m_pShmStateLockSem != NULL)
    {
        delete m_pShmStateLockSem;
        m_pShmStateLockSem = NULL;
    }
}


void ParamShmMgr::unloadShmByFlag(int iFlag)
{
    if(iFlag == PARAM_SHM_INDEX_A){
		if(m_pParamDataBuf[PARAM_SHM_INDEX_A])
		{
			m_pParamDataBuf[PARAM_SHM_INDEX_A]->empty();
        	m_pParamDataBuf[PARAM_SHM_INDEX_A]->unload();
        	delete m_pParamDataBuf[PARAM_SHM_INDEX_A];
        	m_pParamDataBuf[PARAM_SHM_INDEX_A] = 0;
		}
    } else {
		if(m_pParamDataBuf[PARAM_SHM_INDEX_B])
		{
			m_pParamDataBuf[PARAM_SHM_INDEX_B]->empty();
        	m_pParamDataBuf[PARAM_SHM_INDEX_B]->unload();
        	delete m_pParamDataBuf[PARAM_SHM_INDEX_B];
        	m_pParamDataBuf[PARAM_SHM_INDEX_B] = 0;
		}
    }
}
void ParamShmMgr::loadShm()
{
    //非强制方式卸载所有的共享内存,如果有任何进程已经连接在这3块共享内存上则异常退出
    unloadAllShm(false);
    
    //首先创建数据访问的共享内存
    allocDataAccessShm();
    m_pShmStateLockSem->P();
		Log::log(0,"正在上载A区的内存，请稍候...");
    //调用load()函数导入参数
	try{
    if(!m_pParamDataBuf[PARAM_SHM_INDEX_A]->load())
	{
		Log::log(0,"加载共享内存参数失败,清检查数据库配置数据!");
		return;
	}
	}catch(...)
	{
		Log::log(0,"加载共享内存失败");
		return;
	}

    m_pParamAccessPort->m_iShmIdx = PARAM_SHM_INDEX_A;
	m_pParamAccessPort->m_iFlashSeqIdx = 0;
	m_pParamAccessPort->cState = 'R';
		Log::log(0,"正在上载B区的内存，请稍候...");
    if(!m_pParamDataBuf[PARAM_SHM_INDEX_B]->load())
	{
		Log::log(0,"加载共享内存参数失败,清检查数据库配置数据!");
		return;
	}

    m_pShmStateLockSem->V();
    detachShmByData(m_pParamAccessPort); 
    
}


void ParamShmMgr::flashShm()
{
    int iSpareShmID;
    int iWorkShmID;

    //如果连接入口共享内存失败,说明入口共享内存可能没有创建
    //调用导入参数到内存
    if(linkDataAccessShm() == false)
    {
        Log::log(0,"连接入库参数访问共享内存失败! 尝试由程序来创建.."); 
        loadShm();    //调用开辟第1个共享内存使用
        return;
    }

    m_pShmStateLockSem->P();

    iWorkShmID = m_pParamAccessPort->m_iShmIdx;
    if(iWorkShmID == PARAM_SHM_INDEX_A)
        iSpareShmID = PARAM_SHM_INDEX_B;
    else
        iSpareShmID = PARAM_SHM_INDEX_A;

    //调用load()函数导入参数(导入的是当前切换前空闲块)
	try{
    if(!m_pParamDataBuf[iSpareShmID]->load())
	{
		Log::log(0,"切换共享内存失败,清检查数据库配置数据,当前工作内存指向[%d]",iWorkShmID);
		return;
	}
	}catch(...)
	{
		Log::log(0,"切换共享内存失败,当前工作内存指向[%d]",iWorkShmID);
		return;
	}
    m_pParamAccessPort->m_iShmIdx = iSpareShmID;   //切换
	m_pParamAccessPort->m_iFlashSeqIdx++;
	Log::log(0,"切换共享内存从[%d]到[%d]",iWorkShmID,iSpareShmID);
	Log::log(0,"刷新次数[%d]",m_pParamAccessPort->m_iFlashSeqIdx);
    Log::log(0,"等待10秒......程序将清空空闲共享内存的数据."); 
    sleep(10);  //这里等待30秒是为了工作进程已经取得了共享内存的数据在使用,等待使用结束后清除数据

    //调用unload()函数卸载参数(卸载的是当前切换后空闲块)
    //只删除数据,并不删除共享内存,仍保持共享内存的存在
    //m_pParamDataBuf[iWorkShmID]->empty();


    m_pShmStateLockSem->V();
    detachShmByData(m_pParamAccessPort);

}


//提供给外部调用,调用删除所有2块共享内存并释放内存资源
void ParamShmMgr::unloadAllShm(bool bForce)
{
    //非强制方式卸载访问入口共享内存,如果存在其他进程在使用该共享内存则抛出异常
    unloadShmByKey(m_lShmStateKey, bForce);

    //非强制方式卸载参数共享内存,如果存在其他进程在使用该共享内存则抛出异常
    m_pParamDataBuf[PARAM_SHM_INDEX_A]->unload();
    m_pParamDataBuf[PARAM_SHM_INDEX_B]->unload();

}

void ParamShmMgr::unloadShmByKey(const long lShmKey, bool bForce)
{
    int iNattach = 0;  //连接在共享内存上的进程数

    if(bForce == true)
    {
        deleteShmByKey(lShmKey);
    }
    else
    {
        //获取该共享内存的连接进程个数
        iNattach = GetShmNattch(lShmKey);
        if(iNattach == 0)
        {
            deleteShmByKey(lShmKey);
        }
        else if(iNattach > 1)
        {
            Log::log(0,"其他进程正在使用,无法删除共享内存[%ld]", lShmKey);
            THROW(MBC_ParamShmMgr+6);
        }
        else
        {
            Log::log(0,"获取共享内存的连接进程数失败, 共享内存KEY[%ld].", lShmKey);
            //THROW(MBC_ParamShmMgr+7);
        }
    }
}



bool ParamShmMgr::linkDataAccessShm()
{
    int  iErrCode;
    char sSemName[64];

    iErrCode = sizeof(struct ParamAccessPort);
    m_pParamAccessPort = (struct ParamAccessPort*)GetShm(m_lShmStateKey, &iErrCode, 0);

    if(1 == iErrCode)
    {
        detachShmByData(m_pParamAccessPort);
        Log::log(0, "连接共享内存失败, 共享内存[SHM_PARAMDATA_STATE]");
        return false;
    }
    else
    {
        sprintf(sSemName, "%d", (int)m_lShmStateLockKey);
        m_pShmStateLockSem->getSem(sSemName, 1, 1);

        return true;
    }
}


void ParamShmMgr::allocDataAccessShm()
{
    int  iErrCode = 0;
    char sSemName[64];

    m_pParamAccessPort = (struct ParamAccessPort*)AllocShm(m_lShmStateKey, 
                         sizeof(struct ParamAccessPort), &iErrCode);

    if(iErrCode == 1)
    {
        Log::log(0, "分配共享内存失败, 共享内存[SHM_PARAMDATA_STATE]");
        THROW(MBC_ParamShmMgr+4);  //分配共享内存失败  请检查是否有足够的内存
    }
    //初始化共享内存中的数据结构体
    memset(m_pParamAccessPort, 0, sizeof(struct ParamAccessPort));  

    sprintf(sSemName, "%d", (int)m_lShmStateLockKey);
    m_pShmStateLockSem->getSem(sSemName, 1, 1);

}



void ParamShmMgr::detachShmByData(void *pData)
{
    if(pData)
    {
        DetachShm(pData);
    }
    Log::log(0,"分离参数的共享内存.");
}



void ParamShmMgr::deleteShmByKey(const long lShmKey)
{
    FreeShm(lShmKey);

    Log::log(0,"释放参数的共享内存.");
}

void ParamShmMgr::getStat()
{
    int iWorkShmID;

    //如果连接入口共享内存失败,说明入口共享内存可能没有创建
    //调用导入参数到内存
    if(linkDataAccessShm() == false)
    {
        Log::log(0,"连接入库参数访问共享内存失败! 请使用程序创建..");
        return;
    }

    m_pShmStateLockSem->P();

    iWorkShmID = m_pParamAccessPort->m_iShmIdx;

    m_pParamDataBuf[iWorkShmID]->getState();

    m_pShmStateLockSem->V();
    detachShmByData(m_pParamAccessPort);
    
}

void ParamShmMgr::setState(char cState)
{
	if(linkDataAccessShm() == false)
    {
        Log::log(0,"连接入库参数访问共享内存失败! 请使用程序创建..");
        return;
    }
    m_pShmStateLockSem->P();
	Log::log(0,"设置状态为[%c]",cState);
	switch(cState)
    {
        case 'R'://run正常运行
        {
            m_pParamAccessPort->cState = cState;
        }
        break;
        case 'S'://stop暂停
        {
            m_pParamAccessPort->cState = cState;
        }
        break;
        case 'W'://error
        {
            m_pParamAccessPort->cState = cState;
        }
        break;
        default:
        {
            Log::log(0,"state[%c]不支持!只支持运行态R 暂停态S 异常态W",cState);
        }
        break;
    }
	m_pShmStateLockSem->V();
    detachShmByData(m_pParamAccessPort);
}

int ParamShmMgr::showCtlInfo()
{
	if(linkDataAccessShm() == false)
    {
        Log::log(0,"连接入库参数访问共享内存失败! 请使用程序创建..");
        return 1;
    }
	printf("-----------总控信息-----------\n");
	printf("数据指向内存块[%d]\n",m_pParamAccessPort->m_iShmIdx);
	printf("内存块状态[%c]\n",m_pParamAccessPort->cState);
	printf("数据刷新次数[%d]\n",m_pParamAccessPort->m_iFlashSeqIdx);
	printf("扩展值[%ld]\n",m_pParamAccessPort->m_lExVal);
	if(m_pParamDataBuf[PARAM_SHM_INDEX_A])
		printf("共享内存块[%d]进程连接数[%ld]\n",PARAM_SHM_INDEX_A,m_pParamDataBuf[PARAM_SHM_INDEX_A]->getConnnum());
	if(m_pParamDataBuf[PARAM_SHM_INDEX_B])
		printf("共享内存块[%d]进程连接数[%ld]\n",PARAM_SHM_INDEX_B,m_pParamDataBuf[PARAM_SHM_INDEX_B]->getConnnum());
	printf("-----------数据区基本信息------------------\n");
	return 0;
}

#endif
