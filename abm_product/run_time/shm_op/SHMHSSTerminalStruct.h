/*
*   链接TERMINAL MONITOR 内存
*/
#ifndef __SHMHSS_TERMINAL_MONITOR_H_INCLUDED_
#define __SHMHSS_TERMINAL_MONITOR_H_INCLUDED_

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


#endif/*__SHMHSS_TERMINAL_MONITOR_H_INCLUDED_*/