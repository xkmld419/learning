#include "ProcessControler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <signal.h>
#include <sys/shm.h>
#include "Process.h"
#include "encode.h"
#include <utime.h>
#include <dirent.h>
#define	MAXFILE	65535
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "TOCIQuery.h"
#include <algorithm>
using namespace std;

#include "mntapi.h"
vector<ManagedProcessInfo>* HBProcessControl::m_gpvecProcesss=0;
typedef bool(*EnumProcFunction)(void*);	
typedef bool(*EnumFileFunction)(const char* sFileName,bool bIsDir);
bool EnumProc(EnumProcFunction pFunc,void* pProcInfo);
bool GetConfigDir(char* sFilePath,int iSize)
{
	if(sFilePath==0)
		return false;
	char *p =  NULL;
	char sTIBS_HOME[512]={0};
	if ((p=getenv ("TIBS_HOME")) == NULL)
		sprintf (sTIBS_HOME, "/home/bill/TIBS_HOME/etc");
	else
		sprintf (sTIBS_HOME, "%s/%s", p,"etc");
	strncpy(sFilePath,sTIBS_HOME,
		strlen(sTIBS_HOME)>iSize?iSize:strlen(sTIBS_HOME));
	return true;	
}


bool ShmProcessControl::Init(void)
{
	return AttachShm();
}
bool ShmProcessControl::AttachShm(void)
{

	if(::AttachShm(0,(void**)&(MntAPI::m_poInfoHead)))
		return false;;
	MntAPI::m_pProcInfoHead = &(MntAPI::m_poInfoHead->ProcInfo);
	return true;
}
int ShmProcessControl::stopProcess(int iProcID,
								   int iWaitSecs,bool bCheckMQ)
{
	return MntAPI::stopProcess(iProcID,iWaitSecs,bCheckMQ);
}

int ShmProcessControl::getMsgNum(int iProcID)
{
	return MntAPI::getMsgNumber(iProcID);
}


int ShmProcessControl::startProcess(int iProcID)
{
	return MntAPI::startProcess(iProcID);
}

bool ShmProcessControl::FindSleepProcID(int& iProcID)
{
	if(MntAPI::m_pProcInfoHead==0||MntAPI::m_poInfoHead==0)
		return false;
	TProcessInfo* pCurProc = &(MntAPI::m_poInfoHead->ProcInfo);
	for (int i=0; i<MntAPI::m_poInfoHead->iProcNum; i++) {
		if(CheckIsStop(pCurProc)){
			iProcID= pCurProc->iProcessID;
			return false;
		}
	}
	return false;	
}
bool ShmProcessControl::GetAllProc(std::vector<ManagedProcessInfo>& vecProcess)
{
	if(HBProcessControl::m_gpvecProcesss==0)
		HBProcessControl::m_gpvecProcesss = new vector<ManagedProcessInfo>();
	HBProcessControl::m_gpvecProcesss->clear();
	if(MntAPI::m_pProcInfoHead==0||MntAPI::m_poInfoHead==0)
		return false;
	TProcessInfo* pCurProc = &(MntAPI::m_poInfoHead->ProcInfo);
	for (int i=0; i<MntAPI::m_poInfoHead->iProcNum; i++) {
		EnumProc(&(CheckIsActive),pCurProc);
		pCurProc++;
	}
	vecProcess = *HBProcessControl::m_gpvecProcesss;
	return true;
}

pthread_mutex_t HBProcessControl::g_mutexInit=PTHREAD_MUTEX_INITIALIZER;
HBProcessControl* HBProcessControl::m_gpProcessControl=0;

HBProcessControl* HBProcessControl::GetInstance()
{
	if(0==HBProcessControl::m_gpProcessControl)
	{
		pthread_mutex_lock(&g_mutexInit);	
		if(0==HBProcessControl::m_gpProcessControl){
			HBProcessControl* pTmp=0;
			pTmp = new HBProcessControl();
			if(!pTmp->Init())
			{
				delete pTmp;
				pTmp=0;
			}
			HBProcessControl::m_gpProcessControl = pTmp;
		}
		pthread_mutex_unlock(&g_mutexInit);
	}
	return HBProcessControl::m_gpProcessControl;
}

bool HBProcessControl::Init(void)
{
	m_pProcessCtl=0;	
	return true;
}


int HBProcessControl::stopProcess (int iProcID, int iWaitSecs, bool bCheckMQ)
{
	if(!m_pProcessCtl)
		return 0;
	return m_pProcessCtl->stopProcess(iProcID,iWaitSecs,bCheckMQ);
}

int HBProcessControl::getMsgNumber (int iProcID)
{
	if(!m_pProcessCtl)
		return 0;
	return m_pProcessCtl->getMsgNum(iProcID);
}
int HBProcessControl::startProcess (int iProcID)
{
	if(!m_pProcessCtl)
		return 0;
	return m_pProcessCtl->startProcess(iProcID);
}

bool EnumProc(EnumProcFunction pFunc,
								void* pProcInfo)
{
	//这个好像有问题，没啥用
	//todo:待整理
	if(0==pProcInfo)
		return false;
	return (*pFunc)(pProcInfo);
}

bool HBProcessControl::FindSleepProcID(int& iProcID)
{
	return m_pProcessCtl->FindSleepProcID(iProcID);
}

bool HBProcessControl::GetDefaultProc(vector<ManagedProcessInfo>& 
								  vecProcess)
{

	return true;
}


bool HBProcessControl::GetAllProc(vector<ManagedProcessInfo>&
								  vecProcess)
{
	this->m_pProcessCtl->GetAllProc(vecProcess);	

	return true;
}
bool CheckIsStop(void* pProcInfoTmp)
{
	if(0== pProcInfoTmp)
		return false;
	TProcessInfo* pProcInfo = (TProcessInfo*)pProcInfoTmp;
	if(0== pProcInfo)
		return false;
	if(HBProcessControl::m_gpvecProcesss ==0)
		HBProcessControl::m_gpvecProcesss = new vector<ManagedProcessInfo>();
	if(pProcInfo->iState==ST_SLEEP)
	{
		ManagedProcessInfo oProcInfo;
		oProcInfo.strAppName=pProcInfo->sExecFile;
		oProcInfo.strAppPath = pProcInfo->sExecPath;
		oProcInfo.strParam=pProcInfo->sParam;
		oProcInfo.iProcID = pProcInfo->iProcessID;
		HBProcessControl::m_gpvecProcesss->push_back(oProcInfo);
		return true;
	}
	return false;
}
bool CheckIsActive(void* pProcInfoTmp)
{
	if(0== pProcInfoTmp)
		return false;
	TProcessInfo* pProcInfo = (TProcessInfo*)pProcInfoTmp;
	if(0== pProcInfo)
		return false;
	if(HBProcessControl::m_gpvecProcesss ==0)
		HBProcessControl::m_gpvecProcesss = new vector<ManagedProcessInfo>();
	

	if(pProcInfo->iState==ST_INIT||pProcInfo->iState==ST_RUNNING||
		pProcInfo->iState==ST_WAIT_BOOT)
	{
		ManagedProcessInfo oProcInfo;
		oProcInfo.strAppName=pProcInfo->sExecFile;
		oProcInfo.strAppPath = pProcInfo->sExecPath;
		oProcInfo.strParam=pProcInfo->sParam;
		oProcInfo.iProcID = pProcInfo->iProcessID;
		HBProcessControl::m_gpvecProcesss->push_back(oProcInfo);
		return true;
	}
	return false;
}


