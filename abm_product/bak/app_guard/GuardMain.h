/*VER: 1*/ 
#ifndef __WF_GUARD_MAIN_H__
#define __WF_GUARD_MAIN_H__

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Guard.h"
#include "OrgMgr.h"

#include "TOCIQuery.h"

#include "ProcessInfo.h"

const int MAIN_CYCLE_DELAY = 10000;  //0.01 秒
const int OUT_MSG_DELAY    = 200000; //0.2 秒
const int MAX_OUT_MSG_CNT  = 10;


//////  以上为mnt整合进来的代码 ///////////////////////
////////////////////////////////////////////////////////

int  InitWorkFlowGuard();
void WorkFlowGuard();
void CalcTreatRateEx();//add by xn
void ResponseMntConnect();
void ResponseClientConnect();

void ResponseRecvAppInfo();
void ResponseSendCommand();
void ResponseChangeStatus();
void ResponseRefreshRunplan();
void ResponseGetCommand();
void ResponseAckCommand();
void ResponseSendRemoteMsg();

void OutputMsgIntoFrontBuff(); 
void CheckMonitorAlive();
void CheckClientAlive();
void ScanOrdinalPlan();
void ScanTimeRunPlan();

///////////////////////////////////////////////////////////////////
// ClassName: TMonitorFront
// Function : 前台流程调度连接, 消息管理
//-----------------------------------------------------------------
class TMonitorFront {
 public:
    TMonitorFront() { 
        init();
    }
    ~TMonitorFront() {};
    
    void init();
    
    // 压入后台应用进程消息到 m_sMsg[m_iMsgHead]; 满则覆盖最旧的消息;
    bool pushMsg(const char _sMsg[], bool bNeed=false);
    
    // 获取一条后台应用进程消息 from m_sMsg[m_iMsgTail];
    bool getMsg(char _rsMsg[]);
    
    // 检查前台连接是否还活着
    bool checkLinkAlive() {
        if (m_bInit) { m_bInit = 0; return true;}        
        Date o_tmpDate;
        o_tmpDate.addSec(-3600*48); //2天不活动
        if (o_tmpDate>m_oLastOutDate)
            return true;
        else return false;
    }
    
    int     m_iStatus;        // -1:未启用(空闲)  1:启用
    int     m_iAuthID;        // iAuthID 由主守护进程处理Monitor登录时维护;
      // 身份认证ID作用:[[ 当前台Monitor某个计费流程长时间未请求
      // <通过时间判断的方法,本结构中iLastReqTime来记录上一次输出进程消息的时间>,
      // 主守护进程则认为其异常退出,注销它 { iStatus = -1;MntReqInfo.m_iRegNum --; }
      // 身份认证ID是防止 某个前台流程的监控进程 由于异常原因长时间未发送出请求而
      // 被主守护进程注销后，又重新连接上，而此时，该iIndex已经分配给其他监控进程 
      // 而造成错乱的情况发生 ]]
    int     m_iBillFlowID;        // 计费流程ID
    int     m_iLogMode;            // 登录模式： LOG_AS_VIEW  LOG_AS_CONTROL
    int     m_iStaffID;
    char    m_sHostAddr[15];    //Monitor主机IP地址
    
    void    parseOrgIDStr(char *str);
    
    bool    belongOrg(int iOrgID); //iOrgID 是否属于当前连接的管理范围
    
 private:
    char    m_sMsg[MAX_CMAMSG_CNT][MAX_APPINFO__LEN*2];  // Formated
      //后台输出信息m_sMsg[x]: <iProcessID=***><cState='*'><sMsg=***1>...<sMsg=***n>
    int     m_iMsgHead;
    int     m_iMsgTail;
    
    char    m_sMsgNeed[MAX_CMAMSG_CNT][MAX_APPINFO__LEN*2];  //前台必需的消息
    int     m_iMsgNeedHead;
    int     m_iMsgNeedTail;
    
    int        m_aiOrgID[MAX_ORG_CNT];

    Date    m_oLastOutDate;     // 最后一次输出MNT信息的时间
    
    bool    m_bInit;
};


///////////////////////////////////////////////////////////////////
// ClassName: TClientInfo
// Function : 远程监控守护连接/指令 信息管理
//-----------------------------------------------------------------
struct TPlanCommand {
    TPlanCommand() { m_poNext = NULL ; }
    char m_sCommMsg[MAX_OUTMSG_LEN];
    TPlanCommand *m_poNext;
};

class TClientInfo {

 private:
 
    int  m_iMntCommStatus;   // 0-无远程MNT指令(执行)信息; 1-MNT指令被获取但未返回; 2-有MNT指令待获取
    char m_sMntCommand[MAX_OUTMSG_LEN]; // Formated MNT command.

    int  m_iPlanCommStatus;             // 0-无远程计划指令(执行)信息; 1-远程计划指令被获取但未返回
    char m_sPlanAckMsg[MAX_OUTMSG_LEN]; 
    TPlanCommand *m_poCommList;         // 计划指令列表 Plan command List.
    
    // get head record.
    bool getPlanCommand(char *_sCommand) 
    {
        TPlanCommand *pt = m_poCommList;
        if (!pt) return false;
        m_poCommList = pt->m_poNext;
        strcpy( _sCommand, pt->m_sCommMsg);
        delete pt;
        m_iPlanCommStatus = 1;
        return true;
    }
    
 public: 
    int  m_iHostID;    // InitValue -> -1 (show it is not used)
    char m_sSysUserName[32];
    
    Date m_oLastTouch;

    TClientInfo() { 
        init();
    }    
    void init() {
        m_iHostID = -1; 
        m_iMntCommStatus = 0;
        m_iPlanCommStatus = 0;
        m_poCommList = NULL;
        m_oLastTouch.getCurDate();    
    }
        
    // 压入MNT指令,已存在MNT指令或指令未结束则->fail;  success-1, fail-0
    bool pushMntCommand(const char *_sCommand) {
        if (m_iMntCommStatus == 0) {
            strcpy( m_sMntCommand, _sCommand);
            m_iMntCommStatus = 2;
            return true;
        }
        else return false;
    }
    
    // add to the tail.
    void pushPlanCommand(const char *_sCommand)
    {
        TPlanCommand **ppt;
        for (ppt = &m_poCommList; (*ppt); ppt = &((*ppt)->m_poNext) ) ;
        
        ppt = ppt;    
        (*ppt) = new TPlanCommand;
        strcpy( (*ppt)->m_sCommMsg, _sCommand);
        
        //cout<<ppt<<" "<<&m_poCommList<<endl; 
    }
        
    // 提取该远程连接的待执行指令( 若之前有指令被获取但未返回,不允许提取新的指令,return false )
    // 无指令 @return 0; 取得指令@return 1; 有未返回结果的指令(异常)@return 2;
    int getCommand(char *_sCommand)
    {
        m_oLastTouch.getCurDate();
        
        if ( m_iMntCommStatus == 1 || m_iPlanCommStatus == 1)  //有指令被获取但未返回
            return 2;
            
        if ( m_iMntCommStatus == 2 ) { //有MNT指令
            strcpy( _sCommand,m_sMntCommand);
            m_iMntCommStatus = 1;
            return 1;
        }
        
        if ( getPlanCommand(_sCommand) ) {//取到了一条计划指令
            m_iPlanCommStatus = 1;
            return 1;
        }
        
        //无指令:
        return 0;
    }
    
    //上一条被取走的指令执行完毕,返回给远程监控连接管理器. 
    //如果是MNT指令 @return 1; 若是计划指令 @return 2;
    //Err @return 0;
    int ackCommand() { 
        if (m_iMntCommStatus == 1) { m_iMntCommStatus = 0; return 1; }
        else if (m_iPlanCommStatus == 1) { m_iPlanCommStatus = 0; return 2; }
        else return 0; //err.
    }
    
    bool checkLinkAlive() {
        Date o_tmpDate;
        o_tmpDate.addSec(-30); //30秒死亡
        if (o_tmpDate>m_oLastTouch)
            return true;
        else return false;
    }
};


///////////////////////////////////////////////////////////////////
// ClassName: TTimeRunPlan
// Function : 定时执行计划管理 (对应 wf_time_plan,wf_run_time_plan)
//-----------------------------------------------------------------
class TRunPlan ;
class TTimeRunPlan {
 public:
    TTimeRunPlan()  { load(); }
    ~TTimeRunPlan() { unload(); }
    void load();
    void unload();
    void reload()   { unload(); load(); }
    
    //检查是否有到点(到达计划执行时间点)的进程,如果没有,返回0
    //如果有,返回 待执行的进程个数, 并置 TRunPlan.m_bShouldRun = true.
    int checkArrive();
    
    //检查 _pRunPlan 指向的时间计划格式是否正确
    // 正确->返回 true
    // 错误->返回 false, 并置表 wf_time_plan.check_err = 1
    bool checkFormatCorrect(TRunPlan *_pRunPlan);
        
    //从当前计划中查找第一条需要运行的计划(RunPlan.m_bShouldRun == true),
    //找到 返回 ProcessID,_rRunPlanID,_iStaffID,
    //     并置TRunPlan.m_bShouldRun = false  
    //否则返回 -1
    int  getShouldRunProcess(int &_rRunPlanID, int &_iStaffID);

 private:
 
    TRunPlan *m_poRunPlan;
    Date  m_oCurDate;
    
    void UpdateErrPlan(int _iPlanID);
};

class TRunPlan {
 public:
    TRunPlan() { m_bShouldRun = false; m_poNext = NULL; m_poLastCheckDate = NULL; }
    ~TRunPlan() {
        if (m_poNext) { 
            delete m_poNext; 
            delete m_poLastCheckDate;
            m_poNext = NULL;
            m_poLastCheckDate = NULL;
        }
        #ifdef WF_DEBUG
        cout<<"Del m_iProcessID:"<<m_iProcessID<<endl;
        printf(" Time: %s年%s月%s日%s时%s分; (星期%s)",m_sYear,m_sMonth,m_sDay,m_sHour,m_sMinute,m_sWeekDay);
        cout<<"\n m_bShouldRun:"<<m_bShouldRun;
        cout<<endl<<endl;
        #endif
    }

    int     m_iRunPlanID; //定时运行计划标识
    int     m_iPlanID;    //定时计划标识
    int        m_iProcessID;
    int     m_iStaffID;
    char    m_sYear[5];
    char    m_sMonth[3];
    char    m_sDay[3];
    char    m_sWeekDay[2];
    char    m_sHour[3];
    char    m_sMinute[3];
    bool    m_bShouldRun;
    Date    *m_poLastCheckDate;
    TRunPlan *m_poNext;    
};


///////////////////////////////////////////////////////////////////
// ClassName: TOrdinalPlan
// Function : 顺序执行计划管理 (对应 wf_nodeconnector,wf_process )
//-----------------------------------------------------------------
class TNodeConnector {
 public:    
     TNodeConnector() { m_poNextPrefix = NULL; m_poNextProcess = NULL; }
     ~TNodeConnector() {
         #ifdef WF_DEBUG
         cout<<"PrefixID = "<<m_iPrefixID<<"; ProcID = "<<m_iProcessID<<endl;
         #endif
         if (m_poNextPrefix)  { delete m_poNextPrefix;  m_poNextPrefix = NULL; }
         if (m_poNextProcess) { delete m_poNextProcess; m_poNextProcess = NULL; }
     }
     
    int    m_iProcessID;
    int    m_iPrefixID;    //前置进程ID
    char   m_cPrefixState; //前置进程的状态
    
    TNodeConnector  *m_poNextPrefix;  //相同 m_iProcessID 的下一个节点
    TNodeConnector  *m_poNextProcess; //下一个 m_iProcessID 头节点

    void setPrefixState(TRouteTable *pRtTab);
    
    //检查当前 m_iProcessID 是否应该被启动, 是,返回其在RouteTable中的指针.
    TServerAppInfo* checkShouldRun(TRouteTable *pRtTab);
    
 private:    
    bool checkPrefixEnd() { //检查当前 m_iProcessID 的所有前置进程是否处于正常结束状态
        if (m_cPrefixState!='E') return false;
        
        TNodeConnector *pt;
        for (pt=m_poNextPrefix; pt; pt=pt->m_poNextPrefix)
            if (pt->m_iPrefixID != 'E') break;
        if (!pt) return true;
        return false;
    }
};

class TOrdinalPlan { 
 public:
    TOrdinalPlan() { m_poPlan = NULL; load(); }
    ~TOrdinalPlan() { unload(); }

    TNodeConnector *m_poPlan;
    
    void load();
    void unload();
    void reload() { unload(); load(); }
    
    //获取 _iProcessID 对应的前置进程 链表头指针,找不到 return NULL
    TNodeConnector* getAutoProcInfo(int _iProcessID);
    
    void refreshNodeState(TRouteTable *pRtTab) ;
};


//#################################################################
#endif


