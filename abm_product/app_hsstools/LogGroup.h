#ifndef _LOG_GROUP_H
#define _LOG_GROUP_H

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#include "Log.h"
#include "MBC.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include "ReadIni.h"
#include "Exception.h"
#include "Environment.h"
#include "ThreeLogGroup.h"
#include "hbcfg.h"
#include "CheckPointMgr.h"
//#include "HbAdminMgr.h"

#define GROUPCREAT   1               //创建共享内存
#define GROUPFREE    2               //释放共享内存
#define GROUPADD     3               //添加日志组成员文件
#define GROUPDELETE  4               //删除日志组成员文件
#define GROUPPRINTF  5               //显示日志组信息
#define GROUPQUIT    6               //切换日志组
#define GROUPSET     7               //设置归档
#define GROUPBAK     8               //手动备份文件
#define GROUPOUT     9               //输出文件信息
#define FILECDOWN    10              //checkpoint的导出
#define FILECLOAD    11              //checkpoint的加载
#define FILETEST     12              //测试命令
#define PIGEONHOLE   13              //手动归档
#define GROUPHELP    14              //命令帮助手册
#define GROUPMODULE  15              //按模块名称查询日志
#define GROUPPROCESS 16              //按进程名称查询日志
#define GROUPCHECKPT 17              //查询checkpoint日志
#define GROUPINFOLV  18              //按日志等级查询日志
#define GROUPCLASS   19              //按类别名称查询日志
#define GROUPSTAFFID 20              //按工号查询日志
#define GROUPTABNAME 21              //按表名查询日志
#define GROUPSQLTYPE 22              //按操作类别查询日志
#define GROUPPARAM   23              //按参数名查询日志

#define DISOPERAT    95              //界面展现-class-系统业务进程处理
#define DISPARAM     96              //界面展现-class-参数管理
#define DISSYS       97              //界面展现-class-系统管理
#define DISATTEMP    98              //界面展现-class-进程调度
#define DISACCESS    99              //界面展现-class-数据库操作
#define DISFORALL   100              //界面展现-class-全部

#define LIENUM       (ParamDefineMgr::getLongParam("HBLOGG","LIE_NUM"))

//界面显示
//typedef int  (* GETNUM)(void);//函数指针

struct Log_Menu
{
    int Index;          //索引
    int iNum;
//    GETNUM pFunc;       //获取数据的函数指针
    char	ChName[100];  //中文名称 
    
    Log_Menu()
    {
        Index = 0;
        iNum = 0;
        memset(ChName,0,sizeof(ChName));
    }
};

#define COLS_LENS 120

class DisplayLogg
{
public:
	static void DisInit();
	static void Loggstartwin();
	static void LoggLoadGeneralInfo();
	static void LoggLoadEmptyMenu();
    static void LoggStartProblemProc();
	static void LoggloadAll(vector<string> &m_vInfo);
	static void LoggInitAll_1();
	static void LoggStartProblemProcAll();
	static void LoggInitProcessInfo(int flag);
	static void LoggProcessMain_1();
    static int LoggProcessMain(int flag);

	static void LoggRebuildProblemProc();
	static void LoggRebuildProcessAll();
	static void LoggRebuildProcess();
	
	static void LoggTimeBreak_1(int signo);
	static void LoggTimeBreak_ProblemProc(int signo);
	static void LoggTimeBreak_Process(int signo);
	static void LoggTimeBreak_ProblemProcAll(int signo);

private:
	static int INTERVAL;
	static int iLoggPageSize;
	static int Logg_Main2SelectedRow;
	static int Logg_Main2StartRow ;
	static int Logg_Main3SelectedRow;
	static int Logg_Main3StartRow;
	static int iDisParamCnt;
	static int iDisParamCnt_ALL;
	static int Logg_ProblemProcSelectedRow;
	static int Logg_ProblemProcStartRow;
	static int NOW_USED;
	static Log_Menu* Logg_pMenuInfo;
	static Log_Menu* Logg_pMenuInfoAll;
	static int LogFlagAll;
};

class LogGroup : public DisplayLogg
{
public:
    LogGroup();
    ~LogGroup();

    bool init();
    int  Deal();
//    void ParamMain(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
    bool prepare(char g_argv[ARG_NUM][ARG_VAL_LEN],int g_argc);

private:
    void printUsage();

    bool IfCDisplayInfo();
    bool AnalyzesParam(char *sParam,char *sReturn);
    bool AnalyInfoByFile(int LogFlag,int StandId1,char *sStandId1,vector<string> &vDisplayInfo);
    bool AnalyAlarmInfoFromFile(char *sStandId1,vector<string> &vDisplayInfo);
    bool AnalyParamInfo(int LogFlag,char *sStandId1,vector<string> &vParamInfo,vector < string > & vDisInfo);
//	bool GetProcessIdByName(char *sParam,char *sReturn);
    bool GetAppIdByName(char *sParam,char *sReturn);
    bool GetModuleIdByName(char *sParam,char *sReturn);
    bool CheckParamModule();
    bool CheckModuleForMem();
    bool CheckParamProcess();
    bool CheckProcessForAlarm();
    int  CheckParamClass();
    bool CheckParamName();

    void DisplayLogInfo(char *pFile,char *pPath);
    void DisplayInfoByModuleDB();
    void DisplayInfoByProcessDB();
    void DisplayCheckPointDB();
    void DisplayStaffidDB();
    void DisplayInfoByTableNameDB();
    void DisplayInfoBySqlTypeDB();

    bool DisplayDataInfo(int idisNum,bool bspecify=false);
    bool DisplayForInterface(int LogFlag);

    int  DealForModule();
    int  DealForProcess();
    int  DealForParam();
    int  DealForLevel();
    int  DealForTableName();
    int  DealForSqlType();
    int  DealForClass();
    int  DealForCheckPoint();
	int  DealForStaffId();
	
    int  m_iMode;
    int  m_iDisMode;
    int  m_iGroupId;
    int  m_iLine;
    int  m_iModuleId;
    bool m_bPigeonhole;
    bool m_bQryFile;
    char m_sPath[500];
    char m_sFileName[500];
    char m_sBatch[100];
    char m_sParam[100];
    
    ThreeLogGroupMgr * m_poLogGroup;
    CheckPointMgr * m_poCheckPoint; //CheckPoint管理类
    CommandCom * m_poCmdCom;

public:
    static vector<string> m_vConverInfo;
    static vector<string> m_vAlarmInfo;
    static vector<string> m_vParamInfo;
    static vector<string> m_vSysInfo;
    static vector<string> m_vAttemperInfo;
    static vector<string> m_vAccessInfo;
    static vector<string> m_vDisplayInfo;
    static vector<string> m_vDmlInfo;

};

class LogGroupAdmin
{
public:
    LogGroupAdmin(){}
    ~LogGroupAdmin(){}

    void ParamMain(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
};

#endif
