#ifndef _LOG_GROUP_DISPLAY_H_
#define _LOG_GROUP_DISPLAY_H_

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <curses.h>

#ifdef erase
#undef erase
#endif

#ifdef clear
#undef clear
#endif

#ifdef move
#undef move
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sys/shm.h>

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#endif

#ifdef DEF_HP
#include <strings.h>
#endif
using namespace std;

#define COLS_LENS 128
#define LINS_SIZE 27

struct Log_Menu
{
    int Index;          //索引
    int iNum;
    char ChName[100];  //中文名称 

    Log_Menu()
    {
        Index = 0;
        iNum = 0;
        memset(ChName,0,sizeof(ChName));
    }
};

class DisplayLogg
{
public:
    static void DisInit();
    static void Loggstartwin();
    static void LoggLoadGeneralInfo();
    static void LoggLoadEmptyMenu();
    static void LoggloadAll(vector<string> &m_vInfo);

    static void LoggProcessMain(int flag,char* sParam);
    static void LoggRebuildProblemProc();
    static void LoggRebuildProcess();
    static void LoggStartProblemProc();
    static void LoggInitProcessInfo(int flag,char *sParam);
    static void LoggTimeBreak_Process(int signo);
    static void LoggTimeBreak_ProblemProc(int signo);
    
private:
    static int INTERVAL;
    static int iLoggPageSize;
    static int Logg_ProblemProcSelectedRow;
    static int Logg_ProblemProcStartRow;
    static int Logg_Main2SelectedRow;
    static int Logg_Main2StartRow ;
    static int iDisParamCnt;
    static int NOW_USED;
    static Log_Menu* Logg_pMenuInfo;
    static Log_Menu* Logg_pMenuInfoAll;
    static vector<string> m_vConverInfo;

public:
    static vector<string> m_vAppInfo;
    static vector<string> m_vProInfo;
    static vector<string> m_vAlarmInfo;
    static vector<string> m_vParamInfo;
    static vector<string> m_vSystemInfo;
    static vector<string> m_vSysOperInfo;
    //Levle 
    static vector<string> m_vLevFatal;
    static vector<string> m_vLevError;
    static vector<string> m_vLevWarn;
    static vector<string> m_vLevInfo;
    static vector<string> m_vLevDebug;     
    
    //Class
    static vector<string> m_vClaError;
    static vector<string> m_vClaWarn;      
    static vector<string> m_vClaInfo; 
    
    //Type 
    static vector<string> m_vTypeBusi;
    static vector<string> m_vTypeParam;
    static vector<string> m_vTypeSystem ;
    static vector<string> m_vTypeStart;
    static vector<string> m_vTypeOracle;   

	//vCodeInfo
	static vector<string> m_vCodeInfo;
    
};

#endif //_LOG_GROUP_DISPLAY_H_
