/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef WF_GUARD_H__
#define WF_GUARD_H__


#include "WfPublic.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include <StdEvent.h>
#include "MessageQueueH.h"
#include "ABMException.h"
#include "controls.h"
//#include "GetinfoMonitor.h"

#ifndef MAX_APP_NUM
#define MAX_APP_NUM 1024
#endif

const int HOSTNAME_MAX_LEN = 33;

void GetWorkFlowKey ();

//初始化,初始化信号量
int CommunicationInit(bool bInit = false);

//处理应用程序登录监控
void ResponseAppConnect();

//计算应用程序的处理效率
void CalcTreatRate();

//维护应用进程
void CheckApplication (int iHostID, char sUserName[]);

///////////////////////////////////////////////////////////////////
// ClassName: TMsgQueueMgr
// Function : 进程消息队列管理器
//-----------------------------------------------------------------
class TProcessQueue { 
 //表达每个进程的消息队列
 public:
    int m_iProcessID;
    int m_iBillFlowID;
    int m_iMqID;
    int m_iQueueSize;
    int m_iOutMqID; 
    TProcessQueue *m_poOutProcQueue;
    TProcessQueue *m_poNextProcess;
    
    TProcessQueue() { 
        m_poOutProcQueue=NULL; m_poNextProcess=NULL; 
        m_iProcessID=0; m_iBillFlowID=0; m_iMqID=0; m_iQueueSize=0;
    }
    ~TProcessQueue(){
        if (m_poOutProcQueue) delete m_poOutProcQueue;
        if (m_poNextProcess)  delete m_poNextProcess;
    }
};

class TMsgQueueMgr {
 public:
    TMsgQueueMgr() { load(); }
    ~TMsgQueueMgr() { unload(); }
    void reload() { unload(); load(); }
    bool createSelfQueue(int iProcessID);      //创建进程所拥有的队列
    bool deleteSelfQueue(int iProcessID);      //删除进程队列
    bool createAssociateQueue(int iProcessID); //创建进程关联的所有队列
    bool deleteAssociateQueue(int iProcessID); //删除进程关联的所有队列
    bool createBillFlowQueue(int iBillFlowID);  //创建流程所有消息队列
    bool deleteBillFlowQueue(int iBillFlowID);  //删除流程所有消息队列
    
    int getMsgNumber(int _iProcessID); //获取队列消息数目
    int getPercent(int _iProcessID);   //获取消息队列使用百分比
    
    //打印队列情况
    void printQueue(char sBuff[]);
    
 private:
    void load();
    void unload();
    
    TProcessQueue *searchProcQueue(int _iProcID);
    
    TProcessQueue *m_poQueueHead;
};


///////////////////////////////////////////////////////////////////
// ClassName: TServerHostInfo
// Function : 主守护/远程守护模块共用缓冲类定义 (wf_host)
//-----------------------------------------------------------------
struct TServerHostInfo {

    TServerHostInfo() {    m_poNext = NULL; }
    
    ~TServerHostInfo() {
        if (m_poNext) { delete m_poNext; m_poNext = NULL;}

    }
    
    int    m_iHostID;                   
    char   m_sHostAddr[16];                //主机 IP
    char   m_sHostName[HOSTNAME_MAX_LEN];  //主机 hostname

    TServerHostInfo *m_poNext;
};


///////////////////////////////////////////////////////////////////
// ClassName: TServerAppInfo
// Function : 应用进程执行计划管理 (m_wf_process)
//-----------------------------------------------------------------
class TServerAppInfo {

 friend class TRouteTable;

 public:

    TServerAppInfo() { m_poNext = NULL; m_bAutoBootWhenAbort=false;}
    
    ~TServerAppInfo() {
        if (m_poNext) { delete m_poNext; m_poNext = NULL; }
    }    
    int    m_iAppID;
    int    m_iProcessID;        //进程标识,m_wf_process.PROCESS_ID
    int    m_iBillFlowID;       //计费流程标识
    int    m_iHostID;           //主机标识
    int    m_iAutoRun;          //0:不自动执行 1:进程自动执行(当前置进程都运行完毕时)
    int    m_iAppType;          
               //0:后台程序 1:后台Unix shell脚本 2:存储过程 3:PL/SQL块 4:前台应用程序 
    char   m_sExecCommand[80];  //执行文件名及参数
    char   m_sExecPath[80];     //路径
    char   m_sSqlStatement[2000]; // m_iAppType == 3 时有效
    char   m_cState;            //进程状态 A-未启动; R-正在运行; E-正常结束; X-异常 

    char   m_sDBUserName[20];   //联接数据库的用户名
    char   m_sDBConnectStr[20]; //联接数据库的连接串
    
    int    m_iOrgID;            //进程所属的OrgID
    
    char   m_sSysUserName[32];  //后台Unix用户名
    
    bool   m_bAutoBootWhenAbort; //abort 的情况下是否自动启动
    int m_iMqId;  
 
    TServerAppInfo  *m_poNext;
    
 private:

    void setState(char _cState) { 
        m_cState = _cState; 
        updateState(); 
    }

    void updateState();
};


///////////////////////////////////////////////////////////////////
// ClassName: TRouteTable
// Function : 管理应用进程部署表格 (wf_host + m_wf_process)
//-----------------------------------------------------------------
class TRouteTable {
 public:
    TRouteTable()  { load(); }
    ~TRouteTable() { unload(); }
    void load();
    void unload();
    void reload()  { unload(); load(); }
    
    //## 根据iProcID获取进程对应TServerAppInfo地址, 找不到 return NULL
    TServerAppInfo* getServerAppInfo(int _iProcID);
    
    //## 根据hostname查找HostID, If can't found, return -1.
    int getHostIDByHostName(char *_hostname);

    TServerHostInfo * getHostInfoByHostID(int _iHostID);
    bool getHostNameByHostID(int _iHostID, char _sHostName[]);
    
    //## 根据 _iProcID 查找进程部署的主机ID, if can't found, return -1;
    int getHostIDByProcessID(int _iProcID, char *_sUserName);
    
    //## 启动进程 (0-失败, 1-成功)
    int startApp(int _iProcID,int _iProcessLogID = 0, int *pFailedReason = 0x0);
    
    //## 停止进程 (0-失败, 1-成功)
    int stopApp(int _iProcID);
    
    //## 强行改变 TServerAppInfo 的进程状态(0-失败, 1-成功)
    int changeState(int _iProcID,char _cState);

    //## 检查host_id是否存在
    bool touchHostID(int _iHostID) {
        TServerHostInfo *pHostInfo;
        for (pHostInfo = m_poHostInfo; pHostInfo; pHostInfo = pHostInfo->m_poNext ) {
            if (pHostInfo->m_iHostID == _iHostID )
                return true;
        }
        return false;
    }
    
    void createAssociMsgQueue(int _iProcessID) {
        m_poMsgQueue->createAssociateQueue(_iProcessID);
    }
    
    void killAllApp ();
    TServerAppInfo  *m_poAppInfo;  

 private:
     TServerHostInfo *m_poHostInfo;
   //  TServerAppInfo  *m_poAppInfo;
     TMsgQueueMgr    *m_poMsgQueue; //消息队列维护
    
    //## 执行编译出的可执行代码
     int startCommand(TServerAppInfo *);
    
     //## 执行PLSQL
    int startSQLCommand(TServerAppInfo *);
    
    //## 执行Unix Shell脚本
    int startShell(TServerAppInfo *);
    
};


//#################################################################
#endif

