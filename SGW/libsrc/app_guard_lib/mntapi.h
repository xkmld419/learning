/*VER: 3*/ 
#ifndef _MNT_API_H_
#define _MNT_API_H_

#include <stdarg.h>
#include <map>

#include "IpcKey.h"
#include "WfPublic.h"
#include "ProcessInfo.h"

#include "CommonMacro.h"
#include "KeyTree.h"
//#include "EventTypeMgr.h"

const int WF_MAX_FILE_NAME_LEN = 256;


#ifdef __cplusplus
extern "C" {
#endif

	// modify by huff proc_id deliver by param at 2011-07-15
    /* function declare */
    int ConnectTerminalMonitor (int iProcID=-1);
    int DisconnectTerminalMonitor (int iState = ST_SLEEP);
    int AddTicketStat (int iType, int iNum=1);
//	int AddThruput (int iNum);
    int SetProcState (int iState);
    int WriteMsg(int code, const char *fmt, ...);
    int WriteErr(int code, const char *fmt, ...);
    int GetBillFlowID ();
    void SetProcHead(THeadInfo*);    
    /// workflow api begin. 
    // modify by huff proc_id deliver by param at 2011-07-15 
    bool ConnectMonitor(int iProcID=-1);
    
    //WF_NORMAL--正常退出;                  WF_ABORT   --异常终止; 
    //WF_STOP  --前台调度界面发送停止命令;  WF_RUNNING --正在运行
    bool DisConnectMonitor(char cMode = WF_NORMAL);
    
    /*
    输出应用进程日志、告警信息到前台调度界面
        iAlertCode == 0时 表示非告警信息
            (AlertCode 在表BILL.WF_ALERT_DEFINE 中有解释,各模块增加告警代码时,
            需要在CommonMacro.h这个头文件和数据库表中增加相应的记录)
        bInsertDB: 是否需要入告警日志表(wf_alert_log)
    
    */

	void PutPetriRet(int iRet, char *sRetInfo);
	void ClearPetriRet();
    bool Pprintf(int iAlertCode,bool bInsertDB,int iFileID,char const *fmt,...);   
    
    /*20090625 shell和存过写日志无法调用Pprintf,因为非编译程序g_pAppInfo为空，
    为了不影响现有调用，增加另外写日志的函数*/
    bool Pprintf_ex(int iAlertCode,bool bInsertDB,int iFileID,TAppInfo *pAppInfo,char const *fmt,...); 
    
    bool AddProcessCnt(int _iAddCnt=1); //增加当前应用进程数据处理量(调用一次加1)
        
    int GetProcessID();   //获取当前应用进程ID,对应WF_PROCESS.PROCESS_ID
    
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


    bool SetSaveValueByProcID (int iValueID, long lValue, int iProcessID);
    bool GetSaveValueByProcID (int iValueID, long &lValue, int iProcessID);
    int AddSendTickets(int iFileID,int iTickets);
    int GetLastSendTickets(int iFileID);
    
#ifdef __cplusplus
};
#endif
/*
class TStatLog;

class TStatLogMgr {
public:
    //统计模块输入数据
    static void addInEvent(int _iFileID, int _iEvtTypeID, int iCnt=1);
    static void addInEvent(int _iFileID, StdEvent *pEvt, int iCnt=1);
    static void addInEventEX(int _iFileID, StdEvent *pEvt, int iCnt=1);
    
    static void addInEventEX(int _iFileID, int _iEventTypeID,int iFromProcessID,int iCnt=1);
	  static void addOutEventEX(int _iFileID, int _iEventTypeID, StdEvent *pEvt, int iFromProcessID,int iDesProcessID[],int iDesProIDCount,int iCnt=1);
    
    //统计模块输出数据
    static void addOutEvent(int _iFileID, int _iEvtTypeID, StdEvent *pEvt, int iCnt=1);
    static void addOutEvent(int _iFileID, StdEvent *pEvt, int iCnt=1);
    static void addOutEventEX(int _iFileID, int iFromProcessId, StdEvent *pEvt, int _iDesProcessID[],int _iDesProIDCount, int iCnt=1);
    
    static void addRemainAEX (int _iFileID, StdEvent *pEvt, int iCnt=1);
    static void addRemainBEX (int _iFileID, StdEvent *pEvt, int iCnt=1);
    
    static void addRemainA (int _iFileID, int iCnt=1);
    static void addRemainB (int _iFileID, int iCnt=1);
    static void addDistributeRemianA(int _iFileID, int iAppID,int iFromProcessId,StdEvent *pEvt, int _iDesProcessID[],int _iDesProIDCount, int iCnt);
    static void addDistributeRemianB(int _iFileID, int iFromProcessId,StdEvent *pEvt,int _iDesProIDCount, int iCnt);
    
    static void insertLog(int _iFileID);      //一个文件结束后提交
    static void insertLogEx(int _iFileID,int _iFromProcessID,int _iProcessIDBuf[],int _iProCount);      //一个文件结束后提交
    
    static void setForceFlag(bool bFlag);
    
    static void insertLogForce();//12.9for强制提交 由于进程都直接调用静态函数 析构函数无效 只能显性的强制提交
    ~TStatLogMgr();

private:
	static void addDistributeRemianB(int _iFileID, int iFromProcessID,int iDesProIDCount,int iCnt);
	static void addDistributeRemianA(int _iFileID, int iAppID, int iFromProcessID,int iDesProcessID[],int iDesProIDCount,int iCnt);
		
	static int getParentSecLevelEvtType(int _iEvtTypeID);
    static void initSecLevelEventType();
    static TStatLog *searchLogByFileID(int iFileID);
    static TStatLog *searchLogEX(string sMapKey);
    static void addDistributeRemianA(int _iFileID, int iCnt=1);
    static void addDistributeRemianB(int _iFileID, int iCnt=1);
    
private:
    
    static TStatLog *m_poLog;
    static KeyTree<int>* m_pSecLevelEventType;
    //static EventTypeMgr* m_poEventTypeMgr;
    
    static map<string,TStatLog *> m_mapStatLog;
	static map<string, TStatLog*>::iterator m_mapIter;
    static string m_strMapKey;
    static char m_sTMapKey[64]; 
    
    static bool m_bForceFlag;
private:
#ifdef _STAT_FILE_LOG    
	
	//static char m_sLogFilePath[200]; 放到process中初始化
	
	static char m_sLogOpenFilePath[300];
	
	static char m_sLogCloseFilePath[300];
	
	static char m_sLogCreateTime[16];//按分钟切割数据
	
	static char m_sLogFileName[100];
	
	static char m_sLogFullFileName[400];
	
	static FILE *m_pFile;
	
	static int m_iLogRecords;
	
	static bool writeStatLog(TStatLog* pStatLog);
	
	static int indbStatLog(TStatLog* pStatLog, int iLogRecords);
	
	static bool initLogFilePath();
	
	static int commitStatLog(bool bCommit = false);
	
	static int commitStatLogForce();
	
	static bool checkDelay();
#endif	
};


class TStatLog {

friend class TStatLogMgr;
public:
    TStatLog(int _iFileID = 0,int _iFromProcessID = 0,int _iDescProcessID = 0) {
        reset();
        m_iFileID = _iFileID;
        m_iDescProcessID = _iDescProcessID;//目标进程号
    	m_iFromProcessID = _iFromProcessID;//来源进程号
    }
    
    void addInEvent(int _iEvtTypeID, int iCnt=1);   //统计模块输入数据
    void addOutEvent(int _iEvtTypeID, StdEvent *pEvt, int iCnt=1);  //统计模块输出数据
    void insertLog();      //一个文件结束后提交
    void insertLogEx();      //一个文件结束后提交
    void setFileID (int _iFileID) { m_iFileID = _iFileID; }
    
private:
    void addInNormal(int iCnt=1);   //正确事件输入数 
    void addInBlack(int iCnt=1);    //黑户事件输入数 
    void addInDup(int iCnt=1);      //重复事件输入数 
    void addInPreErr(int iCnt=1);   //预处理错误事件输入数(预处理找不到参数,比如区号找不到)
    void addInPricErr(int iCnt =1); //批价错误事件输入数
    void addInUseless(int iCnt=1);  //无效事件输入数
    void addOutNormal(StdEvent *pEvt, int iCnt=1);  //正确事件输出数
    void addOutBlack(StdEvent *pEvt, int iCnt=1);   //黑户事件输出数
    void addOutDup(StdEvent *pEvt, int iCnt=1);     //重复事件输出数 
    void addOutPreErr(StdEvent *pEvt, int iCnt=1);  //预处理错误事件输出数 
    void addOutPricErr(StdEvent *pEvt, int iCnt=1); //批价错误事件输出数
    void addOutUseless(StdEvent *pEvt, int iCnt=1); //无效事件输出数
    void addFormatErr(int iCnt=1);  //预处理格式错误
    void addOutFormatErr(StdEvent *pEvt, int iCnt=1);
    
    void addRemainA (int iCnt=1) {m_iRemainA += iCnt;}
    void addRemainB (int iCnt=1) {m_iRemainB += iCnt;}
    
    void addDistributeRemianA(int iCnt=1);
    void addDistributeRemianB(int iCnt=1);
  
public://文件入库修改
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
    
    int m_iDistributeRemainA; //分发丢弃
    int m_iDistributeRemainB; //分发拆分
    
    int m_iDescProcessID;//目标进程号
    int m_iFromProcessID;//来源进程号

	int m_iTempProcessID;//临时变量
	
	long m_lNormalDuration; //正常话单的时长累积
	long m_lNormalAmount; //正常话单的流量累积
	long m_lNormalCharge; //正常话单的费用累积
	
	long m_lBlackDuration;
	long m_lBlackAmount;
	long m_lBlackCharge;
	
	long m_lDupDuration;
	long m_lDupAmount;
	long m_lDupCharge;
	
	long m_lPreErrDuration;
	long m_lPreErrAmount;
	long m_lPreErrCharge;
	
	long m_lPricErrDuration;
	long m_lPricErrAmount;
	long m_lPricErrCharge;
	
	long m_lUseLessDuration;
	long m_lUseLessAmount;
	long m_lUseLessCharge;
	
	long m_lFormatErrDuration;
	long m_lFormatErrAmount;
	long m_lFormatErrCharge;
    
#ifdef _STAT_FILE_LOG    
	int m_iProcLogID;
	int m_iProcessID;
	int m_iBillflowID;
	char m_sMsgDate[9];
	char m_sMsgTime[7];
#endif     
    TStatLog* m_poNext;
    
private:
    void reset();
};

///////////////////////////////////////////////////////////////////
// ClassName: TAlertLogMgr
// Function : 告警日志管理( wf_alert_log ), 提供应用进程日志输出接口
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
*/

class MntAPI {
  public:
    static TProcessInfo *m_pProcInfoHead;
    static THeadInfo * m_poInfoHead; 
    
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

