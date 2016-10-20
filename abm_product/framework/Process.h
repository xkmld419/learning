// Copyright (c) 2011
// All rights reserved.

#ifndef PROCESS_H_HEADER_INCLUDED_BDCA7789
#define PROCESS_H_HEADER_INCLUDED_BDCA7789
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <vector>


using namespace std;

#include "Date.h"

#include "MBC.h"

#include "CommandCom.h"
#include "ThreeLogGroup.h"
#include "mntapi.h"
#include "WorkFlow.h" 
#include "BaseProcess.h"

#define PROC_RELOAD_RDY    1
#define PROC_RELOAD_RUN    2
#define PROC_RELOAD_OK     4
#define PROC_RELOAD_FAIL   8
#define PROC_RELOAD_VIRTUAL_PARAM   16
#define PROC_RELOAD_HEAD_REGION     32  
#define PROC_RELOAD_MOBILE_REGION   64  
#define PROC_RELOAD_ZONE            128 
#define PROC_RELOAD_ORGEX           256 
#define PROC_RELOAD_TONE_EVENT_RULE 512


//进程标记可以使用的5个long型值
const int MNT_SAVE_L_INCEPT_BLOCK = 1;
const int MNT_SAVE_L_INCEPT_REC = 2;
const int MNT_SAVE_L_3 = 3;
const int MNT_SAVE_L_4 = 4;
const int MNT_SAVE_L_RELOAD=5;  ////重载参数信号


void AbortProcess(int isignal);


/*	发送强制提交事件	*/
#define SEND_FORCE_COMMIT_EVENT() \
	{ \
		StdEvent oCommitEvent; \
		memset(&oCommitEvent,0,sizeof(StdEvent)); \
		oCommitEvent.m_iEventTypeID = FORCE_COMMIT_EVENT_TYPE; \
		m_poSender->send (&oCommitEvent); \
	}

/*	发送文件结束信号	*/
#define SEND_FILE_END_EVENT(iFileID,iFromProID) \
	{ \
		Date d; \
		StdEvent oFileEndEvent; \
		memset(&oFileEndEvent,0,sizeof(StdEvent)); \
		oFileEndEvent.m_iEventTypeID = FILE_END_EVENT_TYPE; \
		oFileEndEvent.m_iFileID = iFileID; \
		oFileEndEvent.m_iProcessID=iFromProID;\
		strcpy (oFileEndEvent.m_sStartDate, d.toString ()); \
		m_poSender->send (&oFileEndEvent); \
	}

class EventReceiver;
class EventSender;

//StdEvent g_oStdEvent;  已放入.cpp 中

//##ModelId=41E1DCF40120
//##Documentation
//## 进程的基类，后台计费实时流程里面的进程直接从此类继承
class Process
    : public BaseProcess
{
public:
	//##ModelId=4313CB2501DB
	//##Documentation
	//## 日志接口
	int log(int log_level, char *format,  ...);

	

	//##ModelId=4313CB2501E3
	//##Documentation
	//## 初始化
	Process();

	//##ModelId=4313CB2501E4
	//##Documentation
	//## 释放本对象申请的内存
	virtual ~Process();

    //##ModelId=424BB1EA0113
    //##Documentation
    //## 数据库访问的用户名
    char m_sDBUser[24];


    //##ModelId=424BB1EA0145
    //##Documentation
    //## 访问数据库的密码
    char m_sDBPwd[24];

  
    //##ModelId=424BB1EA0177
    //##Documentation
    //## 访问数据库的连接串
    char m_sDBStr[32];
    //##ModelId=42252E6401E9
    static int m_iProcID;
    //##ModelId=42252E7000C4
    static int m_iFlowID;
    //##ModelId=42252E770024
    static int m_iAppID;
    //##ModelId=424BEED002DD
   // static BillConditionMgr *m_poConditionMgr;
    //##ModelId=424BEED50077
   // static BillOperationMgr *m_poOperationMgr;
    //##ModelId=4270B32E0304
    static char m_sTIBS_HOME[80];
    //此进程加锁地址
    static char sLockPath[100];

	static ThreeLogGroup *m_pLogObj;
	static CommandCom *m_poCmdCom;
	static void redirectDB(char * sIniHeader);
    static void redirectDB(char const * sIniHeader);
    static void getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr);
   // static long CheckDoReload();
	static CommandCom* m_pCommandCom;    
    static int m_iFromProcID;


protected:
    virtual void GetLogFileName(string& strFileName);

    int run_onlyone(const char *filename);
    //##ModelId=42252E8300AD
    char m_sStartTime[16];

    //##ModelId=42252D5F018A
    EventReceiver * m_poReceiver;

    //##ModelId=42252D9A031F
    EventSender * m_poSender;
private:
	int  GetProcessID();
  public:
#ifdef _STAT_FILE_LOG    
	static char m_sLogFilePath[200];
#endif	
};





#endif /* PROCESS_H_HEADER_INCLUDED_BDCA7786 */

