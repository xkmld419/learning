#ifndef _APP_LOG_GROUP_H_
#define _APP_LOG_GROUP_H_

#include "MBC.h"
#include "LoggDis.h"
#include "ThreeLogGroupDefine.h"
#include "ThreeLogGroup.h"
#include "ThreeLogGroupMgr.h"
#include "CommandCom.h"
#include "Env.h"
#include "BaseProcess.h"
class LoggMgr
	: public BaseProcess
{
 public:

    LoggMgr(int argc, char **argv);
    ~LoggMgr();

    int run();
    int g_argc;
    char **g_argv;

 private:

    bool init();
    bool start();
    bool deal();
    bool prepare();

    bool AnalyzesParam(char *sParam,char *sReturn);
    bool CheckParamName();
    bool DisplayInfo(int iMode);
    
    void printUsage();

    static bool GetField(int iSeq,char *sBuf,char *sParam);
    static bool GreaterMark(const string & s1, const string & s2);
    
    bool CheckProcessId(int iProcessID);
    bool CheckAppId(int iAppID);

    int  m_iMode;
    int  m_iGroupId;
    
    char m_sPath[500];
    char m_sParam[100];
    char m_sFileName[500];
    char m_sParamFile[500];
    
    bool m_bPigeonhole;                     //设置归档
    bool m_bParamErr;
    
    ThreeLogGroup * m_poLogg;               //日志组实现
    ThreeLogGroupMgr * m_poLoggMgr;         //日志组管理
    ReadIni * m_RD;                         //读文件接口
   CommandCom *m_pCommandComm;             //核心参数接口
   
    int m_iNum;
    //CheckPointMgr *pPoint;
};

#endif // _APP_LOG_GROUP_H_
