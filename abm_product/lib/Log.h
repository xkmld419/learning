/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef LOG_H_HEADER_INCLUDED_BD8F4FE0
#define LOG_H_HEADER_INCLUDED_BD8F4FE0
#ifndef _LOGIN_F_H_
#ifndef _MAINMENU_H_
#ifndef _PROCESS_F_H_
#ifndef _TICKET_F_H_

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Date.h"
#include "Exception.h"

#include "CommonMacro.h"

//using namespace std;

class LogFileData;
class LogInfoData;
class ThreeLogGoupData;
class ThreeLogGroupBase;
class ThreeLogGroupMgr; 
#define ERRCCNT 3
#define MONITOR 397
//##ModelId=4270E21703DE

#define ALARM_LOG_OFF
#define BALARMMAXSIZE 					52428800

class Log
{
  public:
    //##ModelId=4270E91103E7
    Log();

    //##ModelId=4270E9120027
    virtual ~Log();

    //##ModelId=4270E93D03CC
    static void setFileName(char *name);

	static void setAlarmName(char *name);

	static void setParamName(char *name);

	static void setLogHome(char *pPath);

    //##ModelId=4270E983020A
	static void logEx(int log_level, char const *format, ...);
	
    static void log(int log_level, char const *format, ...);

	static void pointlog(int log_level,char const *format, ...);

	static void alarmlog(int log_level,int ErrId,int err,int line,char const *file,char const *function,char const *format, ...);

	static void alarmlog28(int log_level,int Class_id,int ErrId,int err,int line,char const *file,char const *function,char const *format, ...);

	static void paramlog(char const *format, ...);

	static void LoggLog(char const *format, ...);

	static void bottomAlarmLog(char *pData);

	static bool CommitLogFile();

	static void init(int iModleId);
	static void setPrintFlag(bool flag = false);
  private:
	
	static bool CutLogFile();

	static bool CheckFile();

	static bool DeleteOldLog(char *pFile);

	static bool GetInitDirFile(char *pPath,bool bFromPro);

	static bool ErrFileWrite(char *pData,char *pfileName,int iSeq);
	static bool ErrFileWriteEx(char *pData,char *pfileName,int iSeq);
	static bool AlarmLogDb(int log_level,char* sErrid,int err,int line,char const *file,char const *function,char const *sForm);

  public:

	static int m_iInfoLevel;				//业务信息点等级

	static int m_iAlarmLevel;				//告警信息点等级

	static int m_iAlarmDbLevel;				//入库等级

	static int m_interval;					//日志信息的输出间隔

	static int m_iMaxLogFileSize;			//日志文件的最大资源 单位:M

	static int m_iMode;						//写日志文件的方式

	static int m_iModleId;				    //模块号

	static ThreeLogGroupMgr * m_pologg;		//日志组接口

  private:
    //##ModelId=4270E22C021C
    static int m_iLogLevel;

	static int m_iProcessId;

    //##ModelId=4270E89302D7
    static char m_sLogFileName[256];

	static char m_sInfoPointName[256];		

	static char m_sAlarmFileName[256];

	static char m_sParamFileName[256];

	static char m_sLog_HOME[80];

    //##ModelId=4270E89F00D6
    static FILE *m_pLogFile;

	static std::vector<std::string> m_vLogInfo;

	static std::ofstream *m_pofstream;

	static long m_lSeq;

	static bool m_bPrintf;				//是否打印出错误信息到界面
};
#define SETLOGPRINT Log::setPrintFlag(false)
#define RESETLOGPRINT Log::setPrintFlag(true)

//业务信息点可变参数宏 [注意]由于部分编译器不支持可变参数宏中可变参数为空的情况,所以在应用中如果无可变参请加上"%s" 
//如果无可变参加上"%s":			INFOPOINT(1,"%s","this is a test");
//如果有可变参则不需要加:		INFOPOINT(1,"this a test a=%d",1);

//信息点接口
#define INFOPOINT(x,y,args...) {if(Log::m_iInfoLevel!=-1)	Log::pointlog(x,y,args);}
//告警接口
#define ALARMLOG(x,y,z,args...) {if(Log::m_iAlarmLevel!=-1)	Log::alarmlog(x,y,errno, __LINE__, __FILE__,__FUNCTION__,z,args);}
//针对2.8的告警接口
#define ALARMLOG28(x,c,y,z,args...) {if(Log::m_iAlarmLevel!=-1)	Log::alarmlog28(x,c,y,errno, __LINE__, __FILE__,__FUNCTION__,z,args);}
//日志组接口

#define LOGG(x,y){ }
                             /*
				  if(!Log::m_pologg)\
					Log::m_pologg = new ThreeLogGroupMgr();\
				  Log::m_pologg->LogGroupWriteFile(x,y);\

				}*/

#endif
#endif
#endif
#endif
#endif /* LOG_H_HEADER_INCLUDED_BD8F4FE0 */
