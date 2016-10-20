#include "hbCheckPoint.h"
#include<sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <functional>

hbCheckPoint::hbCheckPoint()
{

	m_pProcessControler=0;
}

hbCheckPoint::~hbCheckPoint()
{
}


int hbCheckPoint::CreateChkPoint(void)
{
	//CheckPointMgr* pCheckPointMgr = new CheckPointMgr();
	//pCheckPointMgr->touchInfoDown();
	return 0;
}

bool hbCheckPoint::CheckCanDownShm(void)
{
		
	vector<int>vecPrepProcID;
	string strAppName="Prep";
	//进程ID要保留下来等下启动
	GetAllProc(strAppName,m_vecProcID);
	//30停止prep进程
	vecPrepProcID=m_vecProcID;
	if(!vecPrepProcID.empty()){
		StopAllProc(strAppName,30);
	}
	//再看一次
	vecPrepProcID.clear();
	GetAllProc(strAppName,vecPrepProcID);
	if(!vecPrepProcID.empty()){
		StopAllProc(strAppName,30);
	}
	vecPrepProcID.clear();
	GetAllProc(strAppName,vecPrepProcID);
	if(!vecPrepProcID.empty())
		return false;

	return CheckAllMsg(30);
	
}
//hss
bool hbCheckPoint::CheckCanDownShm(string strAppName,int time)
{
	vector<int>vecPrepProcID;
	//string strAppName="Prep";
	//进程ID要保留下来等下启动
	GetAllProc(strAppName,m_vecProcID);
	//30停止prep进程
	vecPrepProcID=m_vecProcID;
	if(!vecPrepProcID.empty()){
		StopAllProc(strAppName,time);
	}
	//再看一次
	vecPrepProcID.clear();
	GetAllProc(strAppName,vecPrepProcID);
	if(!vecPrepProcID.empty()){
		StopAllProc(strAppName,time);
	}
	vecPrepProcID.clear();
	GetAllProc(strAppName,vecPrepProcID);
	if(!vecPrepProcID.empty())
		return false;
	return CheckAllMsg(time);
}

bool hbCheckPoint::CheckAllMsg(int iSec)
{
	//看看消息队列有没有空
	string strAllProc;
	vector<int>vecPrepProcID;
	int iSecWait=iSec;
	GetAllProc(strAllProc,vecPrepProcID);
	int iMsgCnt=0;
	for(vector<int>::iterator it= vecPrepProcID.begin();
		it!=vecPrepProcID.end();it++){
			iMsgCnt=m_pProcessControler->getMsgNumber(*it);
			while(iMsgCnt>0&&iSecWait>0){
				sleep(1);
				--iSecWait;
				iMsgCnt=m_pProcessControler->getMsgNumber(*it);
			}
			if(iMsgCnt>0)
				return false;
			else
				iSecWait=iSec;
	}
	return true;
}

bool hbCheckPoint::GetAllProc(const string& strProgramName,vector<int>&vecProcID)
{
	vector<ManagedProcessInfo> vecProcess;
	m_pProcessControler->GetAllProc(vecProcess);
	vector<ManagedProcessInfo>::iterator it=vecProcess.begin();
	string strStopName=strProgramName;
	transform(strStopName.begin(),strStopName.end(),strStopName.begin(),::tolower);	
	for( ;it!=vecProcess.end();it++){
		string strAppName=it->strAppName;
		transform(strAppName.begin(),strAppName.end(),strAppName.begin(),::tolower);	
		if(strAppName==strStopName ||strStopName.empty()){
			vecProcID.push_back(it->iProcID);
		}
	}
	return true;
}
bool hbCheckPoint::StartAllProc(void)
{
	if(m_vecProcID.empty())
		return true;
	vector<int>::iterator it = m_vecProcID.begin();
	for( ;it!=m_vecProcID.end();it++){
		m_pProcessControler->startProcess(*it);
	}
	return true;
}

bool hbCheckPoint::StartProc(string strAppName)
{
	vector<int> m_vecAppProcID;
	//进程ID要保留下来等下启动
	GetAllProc(strAppName,m_vecAppProcID);
	vector<int>::iterator it = m_vecAppProcID.begin();
	for( ;it!=m_vecAppProcID.end();it++){
		m_pProcessControler->startProcess(*it);
	}
	return true;
}
bool hbCheckPoint::StopAllProc(const string& strProgramName,int iSec)
{
	vector<ManagedProcessInfo> vecProcess;
	m_pProcessControler->GetAllProc(vecProcess);
	vector<ManagedProcessInfo>::iterator it=vecProcess.begin();
	string strStopName=strProgramName;
	transform(strStopName.begin(),strStopName.end(),strStopName.begin(),::tolower);	
	bool bRet=true;
	for( ;it!=vecProcess.end();it++){
		string strAppName=it->strAppName;
		transform(strAppName.begin(),strAppName.end(),strAppName.begin(),::tolower);	
		if(strAppName==strStopName){
			bRet =bRet&&m_pProcessControler->stopProcess(it->iProcID,iSec);
		}
	}
	return bRet;
}

bool hbCheckPoint::Init(void)
{
	m_pProcessControler = HBProcessControl::GetInstance();
	ShmProcessControl* pShmProcessControl =new ShmProcessControl();
	pShmProcessControl->Init();
	m_pProcessControler->SwitchControl(pShmProcessControl);
	return true;
}

/*int main (int argc, char **argv)
{
	hbCheckPoint* pCheckPoint = new hbCheckPoint();
	pCheckPoint->Init();
	if(pCheckPoint->CheckCanDownShm()){
		pCheckPoint->CreateChkPoint();
		pCheckPoint->StartProc("Prep");
	}
	return 0;
}*/
