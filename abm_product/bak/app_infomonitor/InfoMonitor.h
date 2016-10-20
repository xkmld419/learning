#ifndef __INFOMONITOR_
#define __INFOMONITOR_


#include <stdio.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include "CommandCom.h"
#include "ReadIni.h"
#include <map>
#include "MessageQueue.h"

#include "ThreeLogGroup.h"
#include "MBC_HSS.h"
using namespace std;
#define MAX_STRING_LEN 512

struct ProcessInfo
{
	char cAppName[100];
	char cAppParam[100];
	int  iAotoBoot;
	bool  bNeedShiftSysState;
};

class IpcInfo_
{
public:
	IpcInfo_(void):m_lIpcKey(0),m_iIpcType(0),m_imemory_shm_hwm_warming(0)
	{};
	~IpcInfo_(void){};
public:
	string m_strIpcName;
	long m_lIpcKey;
	int m_iIpcType;

	int m_imemory_shm_hwm_warming;
};

class TabSpac_
{
public:
	TabSpac_(void):m_iSize(0),m_iUsed(0),m_iUtili(0)
	{};
	~TabSpac_(void){};
public:
	string m_strName;
	string m_strType;
	string m_strExtent;
	int m_iSize;
	int m_iUsed;
	int m_iUtili;
};


class InfoMonitor
{
public:
	InfoMonitor(void);
	~InfoMonitor(void);
public:
	int m_iPid;
	int m_iSleep;
	//CommandCom m_oComm;
	//考虑0号进程不在的情况，直接读配置文件
	ReadIni m_oIni;
	CommandCom* m_pCmdCom;
public:
	bool init();
	int run();
	int refashParam(char *strFile);
  int refashSHMParam(char * sFilePath,vector<IpcInfo_> &vIpcInfo)	;
  int refashSHMParam()	;  
   bool PrepareMemPercent()  ;
   bool PrepareCpuPercent() ;
private:
	//参数变量
	int m_icpu_warning_value;
	int m_imem_warning_value;
	int m_iphysical_memory;
	int m_itable_space_warning_value;
	int m_idisk_space_warning_value;
	int m_iobject_shm_hwm_warming;
	int m_icheck_process_time;
	string m_strState;
	char m_sConfigName[512];
	int m_iCpuPercent ;
	int m_iMemPercent ;
	char m_sFreeMem[128];
	time_t m_tLastCheckTime;
	bool m_bFirstCheck;
	
	//消息队列
	int m_imqID;
	int m_imax_dcc_request_cnt;
private:
	int m_iAutoStartProc0;
	ThreeLogGroup m_oLog;
	//IpcInfo_ m_oIpcInfo;
	vector<IpcInfo_> m_vIpcInfo;
	vector<ProcessInfo> m_vProcessInfo;

public:
	//获取cpu，内存信息
	int getSysInfo();
	//获取表空间
	int getTableSpace();
	//获取磁盘空间
	int getDiskSpace();
	//获取0号进程状态
	int getProcessStat(const char* sAppName);
	//启动0号进程
	int runProcess(const char* sAppName,char* argv[]);
	//获取共享内存使用情况
	int getSHMInfo();
	
	void CheckResident();
	
	void getMessageQueue();
	
	void StartProcess();
};

#endif //__INFOMONITOR_
