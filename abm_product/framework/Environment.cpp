/*VER: 5*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

// Modified by chenk 20051012 
// getDBConn中增加登陆选项配置，选项配置在当前目录的文件"DBConn.dat"中
// 文件格式：
// 用户名（默认bill）
// 密码（默认bill）
// 数据连接（默认tibs_bill）
#include "Environment.h"
#include <string.h>
#include <iostream>
#include <fstream> 
#include "Exception.h"
//#include "TransactionMgr.h"
//#include "ProdOfferAggr.h"
#include "SHMFileInfo.h"
#ifndef PRIVATE_MEMORY
//#include "ParamInfoInterface.h"
//#include "ParamInfoMgr.h"
#endif
#ifdef HAVE_ABM
#include "TempAccuMgr.h"
#endif
using namespace std;

EventSection * g_poEventSection = 0;

StdEventExt  * g_poStdEventExt = 0;



/*
int Environment::m_iCurMeasure = 0;
int Environment::m_iNewCycleTypeId = 0;
int Environment::m_iCurAttrID = 0;

PricingMeasure * Environment::m_poCurPricingMeasure = 0;
PricingCombine * Environment::m_poCurPricingCombine = 0;

StdEvent  * Environment::m_poStdEvent = 0;

//##ModelId=427DD6C400EE
UserInfoInterface *Environment::m_poUserInfoInterface = 0;
	
//网格租费、网格套餐费共享内存管理
GridRentDataMgr * Environment::m_poGridRentDataMgr = 0;
GridSetDataMgr * Environment::m_poGridSetDataMgr = 0;

AcctItemMgr * Environment::m_poAcctItemMgr = 0;

EventTypeMgr * Environment::m_poEventTypeMgr = 0;

//##ModelId=427DD6D403E0
Serv *Environment::m_poServ = 0;

OfferMgr *Environment::m_poOfferMgr = 0;

//##ModelId=427DD6E601B5
//EventSectionUnioner *Environment::m_poEventSectionUnioner = 0;

ParamInfoInterface * Environment::m_poParamInfoInterface = 0;
ParamInfoMgr * Environment::m_poParamSHM = 0;

CombDisctAssist * Environment::m_poCombDisctAssist = 0;
#ifdef HAVE_ABM
bool Environment::m_bAbm = false;
TempAccuMgr * Environment::m_poTempAccuMgr = 0;
#endif


//shm
//锁标识
bool Environment::SHMLock = false;//P-V处理
//P-V处理数据记录
int Environment::iLinkSHM = -1;
//刷新次数
int Environment::iFlashSeqIdx = 0;
//当前PROCESS_ID
int Environment::gpShmParamLink_ProcessID = 0;
//PID
int Environment::gpShmParamLink_SysPID = 0;

SHMFileInfo * Environment::m_pSHMFileInterface = 0;

//##ModelId=427DD69F0253
/*
void Environment::useUserInfo()
{
	 if(!m_poUserInfoInterface) {
                m_poUserInfoInterface = new UserInfoInterface ();
                if (!m_poUserInfoInterface) THROW(MBC_Environment+1);
        }

        if (!m_poServ) {
                m_poServ = new Serv ();
                if (!m_poServ) THROW(MBC_Environment+1);
				m_poUserInfoInterface->clearServ(*m_poServ);
        }

	if (!m_poAcctItemMgr) {
		m_poAcctItemMgr = new AcctItemMgr ();
                if (!m_poAcctItemMgr) THROW(MBC_Environment+1);
	}

	if (!m_poOrgMgr) {
		m_poOrgMgr = new OrgMgr ();
		if (!m_poOrgMgr) THROW (MBC_Environment+1);
	}

	if (!m_poEventTypeMgr) {
		m_poEventTypeMgr = new EventTypeMgr ();
                if (!m_poEventTypeMgr) THROW(MBC_Environment+1);
	}
	
	if(!m_ProdOfferAggr){
	    m_ProdOfferAggr = new ProdOfferDisctAggr ();
	    if (!m_ProdOfferAggr) THROW(MBC_Environment+1);
    }

    if (!m_poCycleAcctBuf) {
        m_poCycleAcctBuf = new vector<StdAcctItem> ();
    }
    if (!m_poEventAccuBuf) {
        m_poEventAccuBuf = new multimap<int, long> ();
    }
	if(!m_poOfferMgr){
		m_poOfferMgr = new OfferMgr();
		if (!m_poOfferMgr) THROW(MBC_Environment+1);
		m_poOfferMgr->loadNumberAttr();
		m_poOfferMgr->load();
	}

#ifndef PRIVATE_MEMORY
    if(!m_poParamInfoInterface)
    {
        m_poParamInfoInterface = new ParamInfoInterface();
        if(!m_poParamInfoInterface) THROW(MBC_Environment+1);
    }
#endif
}
void Environment::useParamShm()
{
	#ifndef PRIVATE_MEMORY
    if(!m_poParamInfoInterface)
    {
		char *p = getenv ("PROCESS_ID");
		if (!p) {
			gpShmParamLink_ProcessID = 0;  
		} else {
			gpShmParamLink_ProcessID = atoi(p);
		}
		gpShmParamLink_SysPID = getpid();
        m_poParamInfoInterface = new ParamInfoInterface();
        if(!m_poParamInfoInterface) 
			THROW(MBC_Environment+1);
		iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
		Log::log(0,"[param_perpare]PROCESS_ID[%d],PID[%d],FlashTimes[%d],using_block[%d],perpare params!",gpShmParamLink_ProcessID,gpShmParamLink_SysPID,iFlashSeqIdx,m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx);
    }
	#endif
	return;
}

void Environment::useUserInfo(int iMode)
{
	if(iMode == USER_INFO_BASE || iMode == USRE_INFO_NOITEMGROUP)
	{
		if(!m_poUserInfoInterface)
		{
			m_poUserInfoInterface = new UserInfoInterface(iMode);
			if (!m_poUserInfoInterface) THROW(MBC_Environment+1);
		}
		if (!m_poServ) 
		{
			m_poServ = new Serv ();
			if (!m_poServ) THROW(MBC_Environment+1);
			m_poUserInfoInterface->clearServ(*m_poServ);
		}
	}
#ifndef PRIVATE_MEMORY
    if(!m_poParamInfoInterface)
    {
        m_poParamInfoInterface = new ParamInfoInterface();
        if(!m_poParamInfoInterface) THROW(MBC_Environment+1);
    }
#endif
}

//获取数据管理指针
ParamInfoMgr *Environment::getSHM()
{
	#ifndef PRIVATE_MEMORY
	if(m_poParamSHM)
		return m_poParamSHM;
	if(!m_poParamInfoInterface)
	{
		char *p = getenv ("PROCESS_ID");
		if (!p) {
			gpShmParamLink_ProcessID = 0;  
		} else {
			gpShmParamLink_ProcessID = atoi(p);
		}
		gpShmParamLink_SysPID = getpid();
		
		m_poParamInfoInterface = new ParamInfoInterface();
		if(!m_poParamInfoInterface)
			THROW(MBC_Environment+1);
		iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
		SHMLock = false;//防止
		Log::log(0,"[param_link]PROCESS_ID[%d],PID[%d],FlashTimes[%d],using_block[%d],link params!",gpShmParamLink_ProcessID,gpShmParamLink_SysPID,iFlashSeqIdx,m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx);
		Date mDate;
	    long lDateNow = atol(mDate.toString());	
		m_poParamInfoInterface->updateInfo(gpShmParamLink_SysPID,lDateNow,iFlashSeqIdx,"[param_link]: first link SHM_PARAM",gpShmParamLink_ProcessID);
		return m_poParamInfoInterface->m_pParamDataBuf[m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx];
	}
	
	if(!SHMLock)
	{
	    if(m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx != iFlashSeqIdx)
	    {
			#ifdef  DEF_SHM_EXPAND
		    m_poParamInfoInterface->reBindData();//结算版本模式
			#endif
		    iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
		    Log::log(0,"[param_flash]PROCESS_ID[%d],PID[%d],FlashTimes[%d],using_block[%d],flash shm params!",gpShmParamLink_ProcessID,gpShmParamLink_SysPID,iFlashSeqIdx,m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx);
			Date mDate;
			long lDateNow = atol(mDate.toString()); 
			m_poParamInfoInterface->updateInfo(gpShmParamLink_SysPID,lDateNow,iFlashSeqIdx,"[param_flash] flash the shm link",gpShmParamLink_ProcessID);
		    return m_poParamInfoInterface->m_pParamDataBuf[m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx];
	    }else{
			return m_poParamInfoInterface->m_pParamDataBuf[m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx];	
	    }
	} else {
		if(SHMLock<0 || SHMLock>1)
			THROW(MBC_Environment+1);
	    return m_poParamInfoInterface->m_pParamDataBuf[iLinkSHM];
	}
	#else
	        return NULL;
	#endif
}

//连接数据管理指针
ParamInfoMgr *Environment::linkSHM()
{
	#ifndef PRIVATE_MEMORY
	if(!m_poParamInfoInterface)
	{		
		m_poParamInfoInterface = new ParamInfoInterface();
		if(!m_poParamInfoInterface)
			THROW(MBC_Environment+1);
		iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
	} 
	flashSHM();//先刷新一下保证程序自动释放原有连接 add by hsir
	return m_poParamInfoInterface->m_pParamDataBuf[m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx];
	#else
	return NULL;
	#endif
}

//释放对共享内存参数的连接
bool Environment::SHMDetach()
{
#ifndef PRIVATE_MEMORY
	 bool bRes = false;
	 if(m_poParamInfoInterface)
     {
	 	int iIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx;
		bool bRes0 = m_poParamInfoInterface->m_pParamDataBuf[0]->detachAll();
		bool bRes1 = m_poParamInfoInterface->m_pParamDataBuf[1]->detachAll();
		
		if(bRes0)
			Log::log(0,"Detach block_0 success!");
		if(bRes1)
			Log::log(0,"Detach block_1 success!");		    
        return true;
     } else {
        return false;
     }
#endif
	return false;
}

//加锁,先刷新再加锁
bool Environment::shm_P()
{
#ifndef PRIVATE_MEMORY
     if(m_poParamInfoInterface)
     {
	 	if(m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx != iFlashSeqIdx)
	    {
			#ifdef  DEF_SHM_EXPAND
		   	m_poParamInfoInterface->reBindData();
			#endif
		    iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
			Date mDate;
			long lDateNow = atol(mDate.toString()); 
			m_poParamInfoInterface->updateInfo(gpShmParamLink_SysPID,lDateNow,iFlashSeqIdx,"shm_P first link shm",gpShmParamLink_ProcessID);
	    }
        SHMLock = true;
        iLinkSHM = m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx;
        Log::log(0,"[param_lock]_P,lock_flash_block[%d],flash_times[%d]",iLinkSHM,iFlashSeqIdx);
     } else {
		char *p = getenv ("PROCESS_ID");
		if (!p) {
			gpShmParamLink_ProcessID = 0;  
		} else {
			gpShmParamLink_ProcessID = atoi(p);
		}
		gpShmParamLink_SysPID = getpid();
		Date mDate;
		long lDateNow = atol(mDate.toString());
	 	m_poParamInfoInterface = new ParamInfoInterface();
		if(!m_poParamInfoInterface)
			THROW(MBC_Environment+1);
		iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
		SHMLock = true;
		iLinkSHM = m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx;
		m_poParamInfoInterface->updateInfo(gpShmParamLink_SysPID,lDateNow,iFlashSeqIdx,"shm_P first link shm",gpShmParamLink_ProcessID);
		Log::log(0,"[param_lock]_P,first link shm,lock_flash_block[%d],flash_Times[%d]",iLinkSHM,iFlashSeqIdx);
	 }
#endif
	 return false;
}

//解锁
bool Environment::shm_V()
{
#ifndef PRIVATE_MEMORY
     if(m_poParamInfoInterface)
     {
        SHMLock = false;
        iLinkSHM = -1;
        Log::log(0,"[param_lock]_V");
     }
#endif
	return false;
}

//获取当前锁状态
bool Environment::getState()
{
	return SHMLock;
}

//刷新跟内存的连接
bool Environment::flashSHM()
{
#ifndef PRIVATE_MEMORY     
     bool bRes = false;
     if(m_poParamInfoInterface)
     {
        if(iFlashSeqIdx != m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx)
        {
			#ifdef  DEF_SHM_EXPAND
            m_poParamInfoInterface->reBindData();
			#endif
			iFlashSeqIdx = m_poParamInfoInterface->m_pParamAccessPort->m_iFlashSeqIdx;
			if(!gpShmParamLink_SysPID)
				gpShmParamLink_SysPID = getpid();
			Date mDate;
			long lDateNow = atol(mDate.toString());
			if(!gpShmParamLink_ProcessID)
			{
				char *p = getenv ("PROCESS_ID");
				if (!p) {
					gpShmParamLink_ProcessID = 0;  
				} else {
					gpShmParamLink_ProcessID = atoi(p);
				}
			}
			Log::log(0,"[flashSHM],flash link shm,flash_Times[%d]",iFlashSeqIdx);
			m_poParamInfoInterface->updateInfo(gpShmParamLink_SysPID,lDateNow,iFlashSeqIdx,"shm flash shm",gpShmParamLink_ProcessID);
            bRes = true;
        }
     }
     return bRes;
#endif
	return false;
}

//##ModelId=427DD6A703E6

void Environment::useEventSectionUnion()
{
	if (!m_poEventSectionUnioner) {
                m_poEventSectionUnioner = new EventSectionUnioner ();
                if (!m_poEventSectionUnioner) THROW(MBC_Environment+1);
        }
}



void Environment::refleshSHMFile()
{
    #ifdef SHM_FILE_USERINFO
    if(!m_pSHMFileInterface){
        m_pSHMFileInterface = new SHMFileInfo();
		if(!m_pSHMFileInterface)
			THROW(MBC_Environment+1);
    }
    
    m_pSHMFileInterface->refleshPointer();
    #endif
}

TransactionMgr * Environment::m_poTransMgr = 0;

OrgMgr * Environment::m_poOrgMgr = 0;

AggrMgr * Environment::m_poAggrMgr = 0;

//##ModelId=4285C05E0093
//ReferenceObjectMgr *Environment::m_poReferenceObjectMgr = 0;

OfferInsQO * Environment::m_poOfferInsQO = 0;
    
//OfferInsQO  * Environment::m_poCurOfferInsQO = 0;
    
ProdOfferDisctAggr * Environment::m_ProdOfferAggr = 0;

vector<StdAcctItem> * Environment::m_poCycleAcctBuf = 0;
multimap<int, long> * Environment::m_poEventAccuBuf = 0;
*/
