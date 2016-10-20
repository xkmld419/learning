#ifndef	_GLOBAL_TOOLS_H_
#define	_GLOBAL_TOOLS_H_
#include <iostream>
#include "Date.h"
#include <string>
#define	_CONVERT_UPPER 1
#define _CONVERT_LOWER 2

using namespace std;
class GlobalTools
{
public:
    static bool CheckDateString(const char* sDate);
    static char* upperLowerConvert( char* srcStr , const int flag );
    
    //下面是一堆的检查工具
    //检查进程是否停止
    static bool CheckProcessIsStop(pid_t pid);
    //通过脚本检查进程是否存在
    static bool CheckProcessIsStop(const char* pAppName);
    static int CheckProcessNum(const char* sAppName);

    //通过脚本杀掉进程
    static bool KillAppByShell(const char* pAppName);
    static int	StartApp(const char*pAppName,char* argv[32],const char* pDir=0);
    static int	StartShell(const char*pAppName,char* argv[32],const char* pDir=0);

};

//产品化的一些工具

namespace StandardDevelopMent
{
    class  StandardTools
    {
    public:
        static void GetStdConfigFile(string& strFileName);
        static void GetConfigPath(string& strFileName);
    };
};




#endif

