/*VER: 2*/ 
#ifndef __PROCESS_INFO_H___
#define __PROCESS_INFO_H___
#include <time.h>
#include "../../etc/LocalDefine.h"
#include "CommonMacro.h"
#ifndef MAX_APP_NUM
#define MAX_APP_NUM 1024  //应用进程的最大个数,可在 "LocalDefine.h" 中本地化定义
#endif
#ifndef MAX_APP_PARAM_LEN
#define MAX_APP_PARAM_LEN 80
#endif


const int MAX_CMAMSG_CNT  = 32;
const int MAX_APPINFO__LEN = 256;   //单条应用进程消息的最大长度
const int MAX_CMAMSG_LEN  = 1024; //与monitor交互的最大缓冲

const int MAX_OUTMSG_LEN  = 1024;  //与client交互的最大缓冲
const int INFO_Q_SIZE     = 100;   //应用程序输出消息缓冲条数


/* ticket_type define */
#define TICKET_NORMAL           0
#define TICKET_ERROR            1
#define TICKET_BLACK            2
#define TICKET_OTHER            3

const char WF_START  = 'B'; //正在启动中 
const char WF_NORMAL = 'E'; //正常退出
const char WF_ABORT = 'X';  //异常终止
const char WF_STOP = 'K';   //前台调度界面发送停止命令
const char WF_RUNNING = 'R'; //正在运行
// 进程状态cState: A-未启动 B-启动中 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令



/* state define */
#define ST_SLEEP       0
#define ST_INIT        1
#define ST_RUNNING     2
#define ST_WAIT_BOOT   3
#define ST_WAIT_DOWN   4
#define ST_KILLED      5
#define ST_ABORT       6
#define ST_DESTROY     7
#define ST_IDLE        8


const int PETRI_ST_NORMAL=0;
const int PETRI_ST_OUT_ACCT_RDY=1;
const int PETRI_ST_OUT_ACCT_PRE=2;
const int PETRI_ST_OUT_ACCTING=3;


#define MAX_MSG_LEN 10
#define MAX_MSG_NUM 10

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
	//吞吐量 add by hsir
//	int iThruput;
	
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

#ifdef __cplusplus
extern "C" {
#endif

int CreateShm (long lSize, void **pRet);
int AttachShm (long lSize, void **pRet);
int DetachShm ();
int DestroyShm ();
int InitPV ();
int DestroyPV ();
int P() ;
int V();

#ifdef __cplusplus
};
#endif


#endif
