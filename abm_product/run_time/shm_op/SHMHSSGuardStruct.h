/*
*   链接计费2.8版本的GUARD_MAIN 内存结构体
*/

#ifndef __HSS_APP_GUARD_H_INCLUDED_
#define __HSS_APP_GUARD_H_INCLUDED_

#include <sys/sem.h>
#include <sys/time.h>

#define STD_EVENT_SIZE 4096

const int MAX_CMAMSG_CNT  = 32;
const int MAX_APPINFO__LEN = 256;   //单条应用进程消息的最大长度
const int MAX_CMAMSG_LEN  = 1024; //与monitor交互的最大缓冲

const int MAX_OUTMSG_LEN  = 1024;  //与client交互的最大缓冲
const int INFO_Q_SIZE     = 100;   //应用程序输出消息缓冲条数
const int ORGID_STR_LEN = 256;

#ifndef SAVE_BUFF_LEN
#define SAVE_BUFF_LEN         128
#endif

#ifndef MAX_APP_NUM
//#define MAX_APP_NUM 1024  //应用进程的最大个数,可在 "LocalDefine.h" 中本地化定义
#endif


#ifndef LockSafe
#define LockSafe(x,y)   { struct sembuf LOCKSAFE={ (y),-1,SEM_UNDO };   semop((x),&LOCKSAFE,1); }
#endif

#ifndef UnlockSafe
#define UnlockSafe(x,y) { struct sembuf UNLOCKSF={ (y), 1,SEM_UNDO };   semop((x),&UNLOCKSF,1); }
#endif

#ifndef APP_REG_REGISTER
#define APP_REG_REGISTER      3000
#endif

#ifndef APP_REG_UNREGISTER
#define APP_REG_UNREGISTER    3001
#endif

#ifndef APP_REG_ACCEPT
#define APP_REG_ACCEPT        3002
#endif

#ifndef APP_REG_REJECT
#define APP_REG_REJECT        3003
#endif

#ifndef APP_LOG_DEAD
#define APP_LOG_DEAD          3004
#endif


//-TERMINAL MONITOR

#ifndef MAX_MSG_LEN
#define MAX_MSG_LEN 10
#define MAX_MSG_NUM 10
#endif

#ifndef MAX_APP_PARAM_LEN
#define MAX_APP_PARAM_LEN 256     // 应用进程最大携带参数长度
#endif

#ifndef ST_RUNNING
#define ST_INIT        1
#define ST_RUNNING     2
#define ST_WAIT_BOOT   3
#endif

namespace __HSS {
    
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

    

 private:

    bool  bNewMsg;      //是否需要输出进程实例信息 

    bool  bNewState;    //状态是否有改变过

    

};



//## 存放应用进程共享信息()

class TAppSaveBuff {

 friend class TSHMCMA; 

 public:

    

 private:

    int  m_iProcessID;

    bool m_bExistEvent;

    int  m_iEventState; //## 表达事件当前的处理状态, 具体的值由应用程序自己定义. 初始值0

    char m_sBuf[STD_EVENT_SIZE];

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

    TAppInfo       AppInfo[1024];

                                    // 主监控对应每个后台应用进程,远程监控只对应远程应用进程)

        

    //远程监控登录、消息转发共享缓冲区

    int            m_iClientLock;   // 远程监控Client端请求主监控守护服务时锁定

    TClientReqInfo ClientReqInfo;   // 远程监控 与 主监控信息交互接口缓冲

    

    //## 存放每个应用进程最后处理的事件

    TAppSaveBuff   AppSaveBuff[1024];

    //------------------------------------------------------------------------------
};

typedef struct TMsgInfo {

    int iNewFlag;

    long lTime;

    int iMsgType;

    char sMsg[MAX_MSG_LEN];

}TMsgInfo;



typedef struct TProcessInfo 

{

    int  iProcessID;

    int  iAppID;

    int  iBillFlowID;

    int  iSysPID;

//  int  iDBUserID;

    int  iMqID; //消息队列ID

    char sName[96];

    char sExecFile[256];

    char sExecPath[256];

    char sParam[MAX_APP_PARAM_LEN];

    long lLoginTime;

    int  iState;

    long iAllTickets;

    long iNormalTickets;

    long iBlackTickets;

    long iErrTickets;

    long iOtherTickets;

    long iCurMsgPos;

    long iCurErrPos;

    TMsgInfo MsgItem[MAX_MSG_NUM];

    TMsgInfo ErrItem[MAX_MSG_NUM];

    int  iFreeFlag;

    

    time_t tSaveTime;

    long   lSaveTickets;

    int iBlockFlag; //积压标识

    int iTreatRate;//话单处理效率

    int iIORate; //IO频率

    int iCPU;

    int iMEM;

    long lAllIOCnt;//总IO次数

    long lLatestIOCnt;//保留上次IO次数

    long lLatestProcCnt;//保留上次的话单数  

    time_t tLatestCalcTime;//上次计算的时间

    char sFlowName[30];//流程的名字也载上去

    char sMsgInfo[100];//存放信息点



    /*add by xn for hbstat

    StatInfo[0]是当天的统计，StatInfo[1]是自启动以来的统计，

    StatInfo[2]是自当前小时整点以来累计的统计*/

  //  TStatInfo StatInfo[3];

    /*add by xn for schedule*/

    int iVpID;	//虚处理器ID，默认-1则不参与schedule管理

    int iTaskNum; //xn 本队列任务

    int iNextTaskNum; //xn下游队列任务最大值

    int iNextVpID;//下游vp 



	int iPetriState;

	int iLastState;/////进程的上次的状态

       

    /*add by xn for prep abort*/

    int iFileID;        //正在处理的文件号

    int iSendTickets;   //该文件已经发向下游的话单数

} TProcessInfo;



typedef struct THeadInfo {

    int iSysPID;

    int iProcNum;

    int iRefreshFlag;

    //add by jx for 2.8 test

    int iCpuUsed; //CPU使用率

    int iMemUsed;//内存使用率



    char bootTime[16]; //add by xn 20100719 for hbstat



    TProcessInfo ProcInfo;

}THeadInfo;

};

#endif/*__HSS_APP_GUARD_H_INCLUDED_*/