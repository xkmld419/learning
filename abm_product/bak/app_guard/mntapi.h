/*VER: 1*/ 
#ifndef _MNT_API_H_
#define _MNT_API_H_

#include <stdarg.h>


#include "WfPublic.h"
#include "ProcessInfo.h"

#include "CommonMacro.h"
#include "KeyTree.h"
//#include "EventTypeMgr.h"

const int WF_MAX_FILE_NAME_LEN = 256;

//TSHMCMA  *g_pShmTable = NULL;
//long g_lBillMntKey = 0; 
//long g_lWorkFlowKey = 0;
//long g_lLockWorkFlowKey = 0;
//long g_lLockApplicationKey = 0;
//long g_lLockClientKey = 0;

#ifdef __cplusplus
extern "C" {
#endif
    /* function declare */
    int ConnectTerminalMonitor (int g_iProcID);
    int DisconnectTerminalMonitor (int iState = ST_SLEEP);
    int AddTicketStat (int iType, int iNum=1);
    int SetProcState (int iState);
    int WriteMsg(int code, const char *fmt, ...);
    int WriteErr(int code, const char *fmt, ...);
    int GetBillFlowID ();
            
    /// workflow api begin.  
    bool ConnectMonitor(int g_iProcID);
    
    //WF_NORMAL--正常退出;                  WF_ABORT   --异常终止; 
    //WF_STOP  --前台调度界面发送停止命令;  WF_RUNNING --正在运行
    bool DisConnectMonitor(char cMode = WF_NORMAL);
    
    /*
    输出应用进程日志、告警信息到前台调度界面
        iAlertCode == 0时 表示非告警信息
            (AlertCode 在表BILL.WF_ALERT_DEFINE 中有解释,各模块增加告警代码时,
            需要在CommonMacro.h这个头文件和数据库表中增加相应的记录)
        bInsertDB: 是否需要入告警日志表(m_wf_alert_log)
    
    */
    bool Pprintf(int iAlertCode,bool bInsertDB,int iFileID,char const *fmt,...);    
    
    bool AddProcessCnt(int _iAddCnt=1); //增加当前应用进程数据处理量(调用一次加1)
        
    int GetProcessID(int g_iProcID);   //获取当前应用进程ID,对应m_wf_process.PROCESS_ID
    
    //## 保存事件到共享内存(覆盖旧的事件)
    bool SaveLastEvent(StdEvent *pEvt);
    
    //## 取走最后保存的事件(可以重复取, 无事件返回false)
    bool GetLastEvent(StdEvent *pEvt);
    
    //## 存取计费事件的处理状态(供应用程序调用)
    bool SetEventState (int _iState);
    bool GetEventState (int *piState);
    
    //## 存、取一段内存数据
    //   iLen 有效长度-字节数  SAVE_BUFF_LEN  
    //   (#define SAVE_BUFF_LEN  128    at WfPublic.h)
    bool SetSaveBuff (void *pt, int iLen);
    bool GetSaveBuff (void *pt, int iLen);
    
    //## 存取5个long型的值(iValueID = 1~5)
    bool SetSaveValue (int iValueID, long lValue);
    bool GetSaveValue (int iValueID, long &lValue);

    //## 存指定进程的用户积压标志(如果iProcessID为0, 则取当前进程ID)
    bool SetServBlockFlag (int iFlag, int iProcessID = 0);
    //## 取指定进程的用户积压标志(如果iProcessID为0, 则取当前进程ID), 获取不到返回-1
    int  GetServBlockFlag (int iProcessID = 0);
    
    //## 检查进程iProcessID 是否活着
    bool CheckProcAlive (int iProcessID);

#ifdef __cplusplus
};
#endif

class TStatLog;

class TStatLogMgr {
public:
    //统计模块输入数据
    static void addInEvent(int _iFileID, int _iEvtTypeID, int iCnt=1);
    static void addInEvent(int _iFileID, StdEvent *pEvt, int iCnt=1);
    
    //统计模块输出数据
    static void addOutEvent(int _iFileID, int _iEvtTypeID, int iCnt=1);
    static void addOutEvent(int _iFileID, StdEvent *pEvt, int iCnt=1);
    
    static void addRemainA (int _iFileID, int iCnt=1);
    static void addRemainB (int _iFileID, int iCnt=1);
    
    static void insertLog(int _iFileID);      //一个文件结束后提交
    static int getParentSecLevelEvtType(int _iEvtTypeID);

private:
    static void initSecLevelEventType();
    static TStatLog *searchLogByFileID(int iFileID);
    static TStatLog *m_poLog;
    static KeyTree<int>* m_pSecLevelEventType;
    //static EventTypeMgr* m_poEventTypeMgr;
};


class TStatLog {

friend class TStatLogMgr;
public:
    TStatLog(int _iFileID = 0) {
        reset();
        m_iFileID = _iFileID;
    }
    void addInEvent(int _iEvtTypeID, int iCnt=1);   //统计模块输入数据
    void addOutEvent(int _iEvtTypeID, int iCnt=1);  //统计模块输出数据
    void insertLog();      //一个文件结束后提交
    void setFileID (int _iFileID) { m_iFileID = _iFileID; }
    
private:
    void addInNormal(int iCnt=1);   //正确事件输入数 
    void addInBlack(int iCnt=1);    //黑户事件输入数 
    void addInDup(int iCnt=1);      //重复事件输入数 
    void addInPreErr(int iCnt=1);   //预处理错误事件输入数(预处理找不到参数,比如区号找不到)
    void addInPricErr(int iCnt =1); //批价错误事件输入数
    void addInUseless(int iCnt=1);  //无效事件输入数
    void addOutNormal(int iCnt=1);  //正确事件输出数
    void addOutBlack(int iCnt=1);   //黑户事件输出数
    void addOutDup(int iCnt=1);     //重复事件输出数 
    void addOutPreErr(int iCnt=1);  //预处理错误事件输出数 
    void addOutPricErr(int iCnt=1); //批价错误事件输出数
    void addOutUseless(int iCnt=1); //无效事件输出数
    void addFormatErr(int iCnt=1);  //预处理格式错误
    void addOutFormatErr(int iCnt=1);
    
    void addRemainA (int iCnt=1) {m_iRemainA += iCnt;}
    void addRemainB (int iCnt=1) {m_iRemainB += iCnt;}
    
private:
    int m_iFileID;   //处理文件ID
    char m_sFileName[WF_MAX_FILE_NAME_LEN]; //处理文件名
    int m_iInNormal; //正确事件输入数
    int m_iInBlack;  //黑户事件输入数
    int m_iInDup;    //重复事件输入数
    int m_iInPreErr;   //预处理错误事件输入数(预处理找不到参数)
    int m_iInPricErr;  //批价错误事件输入数
    int m_iInUseless;  //无效事件输入数
    int m_iOutNormal;  //正确事件输出数
    int m_iOutBlack;   //黑户事件输出数
    int m_iOutDup;     //重复事件输出数
    int m_iOutPreErr;  //预处理错误事件输出数
    int m_iOutPricErr; //批价错误事件输出数
    int m_iOutUseless; //无效事件输出数
    int m_iFormatErr;  //格式错误(指预处理读取的格式错误话单)
    int m_iOutFormatErr; //格式错误(指预处理读取的格式错误话单)
    int m_iRemainA;  //保留字段1 ()
    int m_iRemainB;  //保留字段2 ()
    
    TStatLog* m_poNext;
    
private:
    void reset();
};

///////////////////////////////////////////////////////////////////
// ClassName: TAlertLogMgr
// Function : 告警日志管理( m_wf_alert_log ), 提供应用进程日志输出接口
// comment  : Pprintf() 内部调用, 需要入库的消息直接Insert&&Commit
//-----------------------------------------------------------------
class TAlertLogMgr {
 public:
    int        m_iProcessID;
    int        m_iAppPID;
    int        m_iBillFlowID;
    int        m_iInforType;   //消息类型， 正常信息 or 出错信息等
    char       m_sRecordTime;  //消息产生的时间
    char       m_sInfor[MAX_APPINFO__LEN];
};


class MntAPI {
  public:
    static TProcessInfo *m_pProcInfoHead; 
    
    // 停进程 iProcID
    // bCheckMQ : 是否等待进程自身消息队列数据处理完成再停进程
    // iWaitSecs: 等待 iWaitSecs 秒后,仍然未能停止进程, 则返回. (为0时,一直等待)
    // 返回
    //   1 -- 成功
    //   0 -- 进程当前处于未启动状态 (如 sleep,abort等), 不需要执行停止操作
    //  -1 -- 消息队列数据超时未处理完
    //  -2 -- 已发送停止进程信号, 但是进程超时未退出
    //  -3 -- 进程当前状态异常 (如一直处于 正在启动<init>, 正在停止<downing>状态), 
    //        不允许发送停止进程信号
    //  -4 -- 进程ID不存在
    static int  stopProcess (int iProcID, int iWaitSecs=0, bool bCheckMQ=false);
    
    // 返回进程(iProcID)的消息队列中消息数目, 消息队列不存在返回-1
    static int  getMsgNumber (int iProcID);
    
    // 启动进程 iProcID
    // 返回
    //   1 -- 成功
    //   0 -- 当前进程已经处于运行状态, 不需要再次启动
    //  -1 -- 启动失败 (进程ID不存在) 
    //  -2 -- 当前进程状态异常(不能发送启动信号)
    //  -3 -- 发送启动信号后启动不成功
    static int  startProcess (int iProcID);
  
  private:
    static TProcessInfo * getProcessInfo (int iProcID);
    
};

#endif

