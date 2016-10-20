/*VER: 2*/ 
#ifndef __WF_PUBLIC_H___
#define __WF_PUBLIC_H___

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <memory.h>
#include <iostream>
//#include <atmi.h>
//#include <fml32.h>
//#include <userlog.h>
#include <unistd.h>
#include <time.h>

#include "Exception.h"
#include "Date.h"
#include "StdEvent.h"
#include "Environment.h"
#include "IpcKey.h"
#include "ProcessInfo.h"

#undef WF_FILE
#define WF_FILE __FILE__,__LINE__

using namespace std;

const int MEMBER_NULL = -99;
const int USLEEP_LONG = 10000; //睡眠时间 微秒 
const int MAX_SQL_LEN = 2048;
const int ORGID_STR_LEN = 256;
const int MAX_ORG_CNT = 100;
const int OUT_MSG_MIN_CNT = 3;

#define PERM  0660


#define WF_WAIT            0

#define MNT_LOG_DEAD       1000 //MNT前台未登录

#define MNT_REQ_REGISTER   1010
#define REG_ACCEPT         1011
#define REG_REJECT_MAXNUM  1012
#define MNT_REQ_UNREGISTER  1020
#define REG_UNREGISTERED    1021 //mnt log out
#define MNT_REQ_APPINFO     1030 //mnt req appinfo
#define REQ_APPINFO_FILLED  1031 //info got
#define REQ_SEND_COMMAND    1041 //mnt req exec command
#define REQ_COMMAND_SENDING 1042 //sending to rmtCma
#define REQ_COMMAND_SENT    1043 //command executed.
#define NO_PRIVILEGE        1044 //no privilege.
#define REG_APP_MAXNUM      1045 //后台应用进程达到最大限制
#define REMOTECMA_NOT_START 1046 //rmtCMA not start.
#define COMMAND_EXEC_ERR    1047

#define REQ_CHANGESTATUS      1050
#define REQ_CHANGESTATUS_OVER 1051
#define REQ_CHANGESTATUS_ERR  1052
#define REQ_STATUS_CHANGEING  1053
#define REQ_REFRESH_RUNPLAN  1060
#define REFRESHING_RUNPLAN   1061
#define RUNPLAN_REFRESHED    1062
#define REFRESHED_ERR        1063

#define CLIENT_LOG_DEAD      2010

#define REQ_CLIENT_LOGIN     2011
#define SERVER_ACCEPT        2012
#define MAX_CLIENT_NUM       2013
#define REQ_CLIENT_LOGOUT    2014
#define REQ_GET_COMMAND      2015
#define REQ_NO_COMMAND       2016
#define REQ_COMMAND_RECV     2017 //取得远程指令
#define REQ_ACK_COMMAND      2019
#define ACK_COMMAND_END      2020
#define REQ_SEND_REMOTEMSG   2021
#define REMOTEMSG_SENT       2022
#define SERVER_REJECT        2023

#define DEFAULT_BUFF_LEN  1024
#define MAX_PARSEBUFF_LEN 2048*4
#define MSG_MAXLEN        256  //TParseBuff.m_sMsg的单位长度
#define MSG_MAXCNT        100  //TParseBuff.m_sMsg最大个数

#define LOG_AS_VIEW       0
#define LOG_AS_CONTROL    1

#define APP_REG_REGISTER      3000
#define APP_REG_UNREGISTER    3001
#define APP_REG_ACCEPT        3002
#define APP_REG_REJECT        3003
#define APP_LOG_DEAD          3004

#define SAVE_BUFF_LEN         128

#ifndef WF_LOCKOP__
#define WF_LOCKOP__
#define LockSafe(x,y)   { struct sembuf LOCKSAFE={ (y),-1,SEM_UNDO };   semop((x),&LOCKSAFE,1); }
#define UnlockSafe(x,y) { struct sembuf UNLOCKSF={ (y), 1,SEM_UNDO };   semop((x),&UNLOCKSF,1); }
#endif

extern "C" {
//##去除字符串前后的空格符
void AllTrim(char sBuf[]);

//##去除定长字节前面的NULL值
void TrimHeadNull(char *src,int ilen);

void replace(char * sql,char *src,char *des);

}


class TWfLog {
 public:
    static void log(char const *fmt,...);
    static void log(int iErrCode,char const *fmt,...);
    static void log(char const *sFileName, int iLine, char const *fmt,...);
    static void writeLog(char *sMsg) ;
    static void writeLog2(char *sMsg) ;
    static void ignoreTuxBuildErr() {
        //cout<<h_tmenv<<h_atmi<<h_fml32<<h_userlog<<endl;
// cout<<h_tmenv<<h_atmi<<h_fml32<<endl;
    }
 private:    
    static Date m_oDate;
};

///////////////////////////////////////////////////////////////////
/*
  ClassName: TLogProcessMgr
  Function : 进程生命周期日志管理( log_process ), 由守护进程管理
  Comment  : 
      守护进程执行启动指令后,调用insertLogProcess生成记录, 
      后台应用进程 -- 连接后,根据process_id,寻找内存TAppInfo位置,更新db 
      sql语句      -- 执行指令后完成db中的记录. 
-----------------------------------------------------------------*/
class TLogProcessMgr {
 public:
 
    //守护进程处理后台应用进程注销登录时调用(_iAppPid == 0)
    //守护进程处理后台应用进程连接后调用    (_iAppPid != 0)
    static void updateLogProcess(int _iProcessLogID, char _cState, int _iAppPid = 0);
    
    //守护进程启动后台应用进程前调用(目前守护进程执行的指令全部放入,包括 停止,改变状态)
    static int insertLogProcess( int _iProcID, const char *sCommand,
        int iStaffID = -1,int iPlanID = -1 );
    
 private:
     static int getProcessLogID();
};


/**********************************************************
ClassName: TParseBuff
Function : 解析/格式化 内部定义格式的 进程间 通讯字符串
comment  : 格式化串的组成单位格式为 "<可识别域名=域值>"
  可识别的域名称有--
   iBillFlowID/iProcessID/iHostID/sHostAddr/iProcessCnt/iMsgType
   iStaffID/iPlanID/iLogMode/iLogID/iAuthID/iMainGuardIndex/iResult
   iOperation(指令: 1-start 2-stop 3-ChangeStatus 4-Refresh)
   cState(A-未启动 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令)
   sMsg
Restrict : 
 1.只有 "sMsg" 域重复出现在格式串中是合法的
 2."sMsg" 外的其他域重复出现时,解析器将用该域最后一次出现的值覆盖前面的值
 3.以 "i" 开头的可识别域名对应的域值必需是数值型的字符串,如2046(不加引号)
 4.以 "s" 开头的可识别域名对应的域值可以是任意字符串,但不能含:"<",">","="
 5.域名大小写敏感
 6."iProcessCnt" 是当前进程处理的事件量(无符号长整型),其他以"i"开头为整型
Demo     : <iBillFlowID=3><iProcessID=2046>...<sMsg=Str_1>...<sMsg=Str_N>
---------------------------------------------------------*/
class TParseBuff{
public:
    TParseBuff(); 
    TParseBuff(char *sInBuff); //sInBuff => m_Str.

    void reset(); //数值型的成员变量初始化为 MEMBER_NULL, 字符串为空
    bool parseBuff(char *sInBuff); //sInBuff => m_Str and parse it.

    int  getBillFlowID() { return  m_iBillFlowID; }
    int  getProcessID()  { return  m_iProcessID; }
    int  getHostID()     { return  m_iHostID; }
    int  getStaffID()    { return  m_iStaffID; }
    int  getPlanID()     { return  m_iPlanID; }
    int  getLogMode()    { return  m_iLogMode; } 
    int  getLogID()      { return  m_iLogID; } 
    int  getAuthID()     { return  m_iAuthID; }
    int  getOperation()  { return  m_iOperation; } 
    int  getResult()     { return m_iResult; }
    unsigned long getProcessCnt() { return m_lProcessCnt; }
    unsigned long getNormalCnt() { return m_lNormalCnt; }
    unsigned long getErrorCnt() { return m_lErrorCnt; }
    unsigned long getBlackCnt() { return m_lBlackCnt; }
    unsigned long getOtherCnt() { return m_lOtherCnt; }
    
    int  getMainGuardIndex()      { return m_iMainGuardIndex; }
    void getHostAddr(char *ip)    { if (ip) strcpy(ip,m_sHostAddr); }
    void getSysUserName(char *usr)    { if (usr) strcpy(usr,m_sSysUserName); }
    void getOrgIDStr(char *str)   { if (str) strcpy(str,m_sOrgIDStr); }
    char const *getMsg(int iOffSet = 0)  {
        if (iOffSet <= m_iMsgOff)
            return m_sMsg[iOffSet];
        else return "";        
    }
    char getState()  { return m_cState; }
    
    void setBillFlowID(int id)  { m_iBillFlowID = id;}
    void setProcessID(int id)   { m_iProcessID = id;}
    void setHostID(int id)      { m_iHostID = id;}
    void setHostAddr(char* str) {
        strncpy(m_sHostAddr,str,15); 
        m_sHostAddr[15] = 0;
        AllTrim(m_sHostAddr);
    }
    void setSysUserName(char* str) {
        strncpy(m_sSysUserName,str,31); 
        m_sSysUserName[31] = 0;
        AllTrim(m_sSysUserName);
    }
    void setOrgIDStr(char* str) {
        strncpy(m_sOrgIDStr,str,ORGID_STR_LEN);
        m_sOrgIDStr[ORGID_STR_LEN] = 0;
        AllTrim(m_sOrgIDStr);
    }
    void setLogID(int id)  { m_iLogID = id;}
    void setAuthID(int id) { m_iAuthID = id;}
    void setResult(int id) { m_iResult = id;}
    void setStaffID(int id) { m_iStaffID = id;}
    void setPlanID(int id) { m_iPlanID =  id;}
    void setOperation(int id) { m_iOperation = id;}
    void setMainGuardIndex(int id) { m_iMainGuardIndex = id;}
    void setProcessCnt(unsigned long id) {m_lProcessCnt = id;}
    void setNormalCnt(unsigned long id) {m_lNormalCnt = id;}
    void setErrorCnt(unsigned long id) {m_lErrorCnt = id;}
    void setBlackCnt(unsigned long id) {m_lBlackCnt = id;}
    void setOtherCnt(unsigned long id) {m_lOtherCnt = id;}
    
    void setState(char _cState) { m_cState = _cState; }
    
    void addMsg(const char *msg); //压入文本消息到m_sMsg
    bool getBuiltStr(char *sOutBuff); //获取m_Str
    char *getBuiltMsg(); //打包m_sMsg <sMsg=...>...<sMsg=...> 

    
private:
    bool parse(); //if (m_str) parse it.
    void formatBuff(); //将 m_sMsg 缓冲中的分隔符替换掉
    void parseResultSave(char *pAtt, char *pValue);
    bool build(); //将内部成员变量build成字符流 => m_Str
    
    unsigned long stringToUL (char *pValue);
    
    char m_Str[MAX_PARSEBUFF_LEN+1]; //存放封装好的字符串
    
    int  m_iBillFlowID;
    int  m_iProcessID;
    unsigned long m_lProcessCnt;
    unsigned long m_lNormalCnt;
    unsigned long m_lErrorCnt;
    unsigned long m_lBlackCnt;
    unsigned long m_lOtherCnt;
    int  m_iHostID;
    char m_sHostAddr[16];
    char m_sSysUserName[32];
    int  m_iStaffID;
    int  m_iPlanID;
    int  m_iLogMode;
    int  m_iLogID;
    int  m_iAuthID;
    int  m_iMainGuardIndex;
    int  m_iOperation; // 1-start 2-stop  3-ChangeStatus 4-Refresh
    char m_cState;     //CHANGE_APPSTATUS (A-未启动 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令))
    int  m_iResult;    //MONITOR_LOGIN
    char m_sOrgIDStr[ORGID_STR_LEN+1];
    char m_sMsg[MSG_MAXCNT][MSG_MAXLEN+1]; //应用文本信息
    int  m_iMsgOff;    //消息缓冲m_sMsg的下条消息存储偏移量
};


//############################################################
//  共享缓冲结构定义
class TMntReqInfo{
 public:
    int   m_iStatus; 
    int   m_iRegNum;                 // 登录的计费流程数
    int   m_iBillFlowID;             // iBillFlowID: 计费流程ID
    int   m_iLogMode;                // 登录模式： LOG_AS_VIEW  LOG_AS_CONTROL
    int   m_iStaffID;    
    char  m_sHostAddr[15];
    int   m_iIndex;                     // 前台Monitor监控进程ID
    int   m_igAuthID;                // 流程监控身份验证ID  初始化为0
    int   m_iAuthID;                 // 身份ID,信息交互时用
    int   m_iProcessID;
    int   m_iOperation;
    char  m_cState;
    char  m_sOrgIDStr[ORGID_STR_LEN+1];
    char  m_sMsgBuf[MAX_CMAMSG_LEN]; // 信息交互容器,用于主监控存放打包后的消息 ...
    TMntReqInfo() { m_iRegNum = 0; }
};

class  TClientReqInfo{
 public:
    int  m_iStatus;
    char m_cState;      // 进程状态
    int  m_iRegNum;     // the Client Number logged
    int  m_iHostID; 
    char m_sSysUserName[32];
    int  m_iIndex;      // Client-key
    int  m_iProcessID;
    int  m_iStaffID;
    int  m_iPlanID;
    int  m_iResult;
    int  m_iOperation;              //(1-start, 2-stop, 3-ChangeStatus, 4-Refresh)
    int  m_iMainGuardIndex;         //远程应用进程在主监控守护主机AppInfo中的下标
    char m_sMsgBuf[MAX_OUTMSG_LEN];
    TClientReqInfo() { m_iRegNum = 0; }
};

class TAppReqInfo{
 public:
    int       m_iStatus;    // APP_REG_REGISTER,APP_REG_UNREGISTER,APP_REG_ACCEPT,APP_REG_REJECT
    int       m_iProcessID; // WF_PROCESS.PROCESS_ID
    int       m_iAppPID;    // 后台应用进程PID
    int       m_iIndex;     // 应用 ConnectMonitor时指定其位置
    char      m_cState;
    int       m_iSaveBufIdx;
};


//## 描述每个进程运行实例
//## 守护进程在执行启动指令时,生成记录,并生成wf_log_process记录, 返回 process_log_id;
//## 
class TAppInfo {

 friend class TSHMCMA;

 public:
    int        m_iProcessLogID; // 对应 WF_LOG_PROCESS.process_log_id
    int        m_iProcessID;    // -1时,标示未使用过
    int        m_iMainGuardIndex; 
                         // 给远程监控主机使用，标识远程应用进程在 主监控守护主机AppInfo中的下标
    int        m_iBillFlowID;   // 计费流程ID
    char       m_cState; //应用进程状态(当进程状态改变时维护WF_LOG_PROCESS 表)
                         //A-未启动 B-启动中 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令
    int        m_iAppPID;
    int	 	m_iAppType;
    unsigned long m_lProcessCnt;
    unsigned long m_lNormalCnt;
    unsigned long m_lErrorCnt;
    unsigned long m_lBlackCnt;
    unsigned long m_lOtherCnt;
    
    time_t         m_tLatestCalcTime; //上次计算处理效率的时间
    unsigned long  m_lLatestProcCnt;  //上次计算时的处理到达量
    float          m_fTreatRate;      //最近一次计算出的处理速度
    
    int        m_iInfoHead;
    int        m_iInfoTail;
    char       m_asMsg[INFO_Q_SIZE][MAX_APPINFO__LEN]; // 应用进程的输出信息
 	int        m_iRet ;///增加返回给petri的值
 	char       m_sErrMsg[MAX_APPINFO__LEN]; ///增加返回给petri的错误信息串

    TAppInfo() { init(); m_iProcessID = -1;}

    void init() {
        m_iProcessID = -1; bNewMsg = false; bNewState = true;
        m_cState = 'A'; m_iProcessLogID = -1; m_iMainGuardIndex = -1; 
        m_lProcessCnt = 0; m_lNormalCnt=0; m_lErrorCnt=0; m_lBlackCnt =0; m_lOtherCnt=0;
        m_iInfoHead = 0; m_iInfoTail = 0;  
        m_iRet=0; memset(m_sErrMsg,0,sizeof(m_sErrMsg) );
    }

    bool putRetInfo(int iRet, char *sRetInfo);
    bool getRetInfo(int & iRet, char *sOutInfo);
	
    void pushInfo(char *_sMsg);
    bool getInfo( char *_sMsg); //取得最后一条消息时
    char getState() { bNewState = false; return m_cState; }
    bool checkNeedOutput();
    
    bool checkAlive(); //检查当前进程在后台是否真的活着(根据m_iAppPID)

    void setState( char _cState) { 
        m_cState = _cState; 
        bNewMsg = true;
        bNewState = true;
        TLogProcessMgr::updateLogProcess(m_iProcessLogID, _cState);
    }
    
    void forceOutput() { bNewMsg = true; }

//新增
    void getOtherProcessInfo();
    
 private:
    bool  bNewMsg;      //是否需要输出进程实例信息 
    bool  bNewState;    //状态是否有改变过
    
};

//## 存放应用进程共享信息()
class TAppSaveBuff {
 friend class TSHMCMA; 
 public:
    void init () 
    { 
        m_iProcessID = -1; 
        m_bExistEvent = false; 
        m_iEventState = 0;
        m_iServBlockFlag = 0;
        memset (m_lSaveValue, 0, sizeof(long)*5);
    }
    void saveEvent (StdEvent &Evt)    //## 保存事件 (进程框架中recieve 事件时已调用)
    {
        m_oEvent = Evt;
        m_bExistEvent = true;
        m_iEventState = 0;
    }
    void setEventState (int _iState)  //## 供应用程序在处理事件过程中保存事件的处理状态
    { 
        m_iEventState = _iState; 
    }
    bool getEvent (StdEvent &Evt) 
    {
        if (!m_bExistEvent)
            return false;
        Evt = m_oEvent;
        return true;
    }
    
    bool getEventState (int *piState) //## 事件不存在返回false; 存在返回true, *piState = 实际状态
    {
        if (!m_bExistEvent)
            return false;        
        *piState = m_iEventState;
        return true;
    }
    
    void setSaveBuff (void *pt, int iLen) {
        int iAdjustLen = iLen<SAVE_BUFF_LEN ? iLen:SAVE_BUFF_LEN;
        memcpy (m_sSaveBuf, pt, iAdjustLen);
    }
    void getSaveBuff (void *pt, int iLen) {
        int iAdjustLen = iLen<SAVE_BUFF_LEN ? iLen:SAVE_BUFF_LEN;
        memcpy (pt, m_sSaveBuf, iAdjustLen);
    }
    bool setSaveValue (int iValueID, long lValue) {
        if (iValueID<1 || iValueID>5) return false;
        m_lSaveValue[iValueID-1] = lValue;
        return true;
    }
    bool getSaveValue (int iValueID, long &lValue) {
        if (iValueID<1 || iValueID>5) return false;
        lValue = m_lSaveValue[iValueID-1];
        if (lValue == 0) return false;
        return true;
    }
    
    void setServBlockFlag (int iFlag)
    {
        m_iServBlockFlag = iFlag;
    }
    int  getServBlockFlag ()
    {
        return m_iServBlockFlag;
    }
    
    int  getProcessID() { return m_iProcessID; }
    void setProcessID(int id) { m_iProcessID = id; }
    
 private:
    int  m_iProcessID;
    bool m_bExistEvent;
    int  m_iEventState; //## 表达事件当前的处理状态, 具体的值由应用程序自己定义. 初始值0
    StdEvent m_oEvent;
    unsigned char m_sSaveBuf[SAVE_BUFF_LEN];
    long m_lSaveValue[5];
    int  m_iServBlockFlag;
};

class TSHMCMA {
 public:
    //前台监控界面登录、消息请求共享缓冲区
    int            m_iMonitorLock;  // 每次Monitor请求时锁定
    TMntReqInfo    MntReqInfo;      // 前台监控与主监控守护进程的信息交互接口缓冲    

    //应用程序登录、消息转发共享缓冲区 
    int            m_iAppLock;      // 应用程序登录时ConnectMonitor锁定的信号量
    TAppReqInfo    AppReqInfo;      // 应用程序请求登录时的临时登录信息
    TAppInfo       AppInfo[MAX_APP_NUM];
                                    // 主监控对应每个后台应用进程,远程监控只对应远程应用进程)
        
    //远程监控登录、消息转发共享缓冲区
    int            m_iClientLock;   // 远程监控Client端请求主监控守护服务时锁定
    TClientReqInfo ClientReqInfo;   // 远程监控 与 主监控信息交互接口缓冲
    
    //## 存放每个应用进程最后处理的事件
    TAppSaveBuff   AppSaveBuff[MAX_APP_NUM];
    //------------------------------------------------------------------------------
    
    TAppInfo*  getAppInfoByProcID(int _iProcID) ;
    
    TAppSaveBuff* getAppSaveBuffByProcID(int _iProcID);
    
    int changeState(int _iProcID, char _cState) ;
    
    //获取一个空闲的 TAppInfo, found:return 1; notFound:return 0; running:return -1;
    int getIdleAppInfo(int _iProcID, int &iIndex) ;
    
    void setMainGuardIndex(int _iProcID,int _iMainGuardIndex) ;
    
    void printShm() ;

};

#endif

