/*VER: 5*/ 
#ifndef SHM_STAT_LOG_MGR
#define SHM_STAT_LOG_MGR

#include "SHMData_A.h"
//#include "SHMStringTreeIndex_A.h"
#include "SHMIntHashIndex_A.h"
#include "StdEvent.h"
#include "IpcKey.h"
#include "ParamDefineMgr.h"
#include "CSemaphore.h"

#include <vector>

using namespace std;

class SeqMgrExt;



#define SHM_STAT_LOG_DATA        (IpcKeyMgr::getIpcKeyEx(-1, "SHM_StatInfoLog"))
#define SHM_STAT_LOG_INDEX  (IpcKeyMgr::getIpcKeyEx(-1, "SHM_StatInfoLog_Index"))
#define SHM_STAT_LOG_LOCK     (IpcKeyMgr::getIpcKeyEx(-1,  "LOCK_StatInfoLog"))
#define SHM_STAT_LOG_INDEX_LOCK     (IpcKeyMgr::getIpcKeyEx(-1,  "LOCK_StatInfoLog_Index"))
#define SHM_STAT_LOG_COUNT \
                    (ParamDefineMgr::getLongParam("MEMORY_DB", "StatInfoLogCount"))
#define SHM_STAT_LOG_INDEX_COUNT \
                    (ParamDefineMgr::getLongParam("MEMORY_DB", "StatInfoLogIndexCount"))

#define  CDR_TYPE_NUMS   7
#define  NORMAL  0   //
#define  DUP     1   //重复事件
#define  INVALID 2   //无效事件
#define  PREP_ERR 3 //预处理错误
#define  PRIC_ERR 4 //批价错误
#define  FORM_ERR 5
#define  BlACK_ERR 6  //黑户

#define  LOG_TYPE_IN 1  //输入信息
#define  LOG_TYPE_OUT 2 //输出信息

#define  LOG_DELAY_TYPE 3 //延迟话单类型3种

#define MAX_STRING_INDEX_LEN 20
#define DR_ID 0 

struct LogInfo{
	int m_iType ; //
	int m_iNum; //话单数
	int m_lCharge ;
	long m_lDulation ; //
	long m_lAmount ;
};

struct SHMStatLogData {
    int m_iFileID;   //处理文件ID
    char m_sFileName[MAX_FILE_NAME_LEN];
    int m_iAppID ;    //处理程序ID
    int m_iDrID;     //容灾B标识ID 
    int m_iSourceID ; //数据来源省份ID
    int m_iModuleID ; //模块ID
    char m_sStartTime[14+1];  //文件开始处理时间
    char m_sEndTime[14+1];    //文件结束处理时间
    LogInfo m_sInLog ;  //文件输入信息
    LogInfo m_sALLInLog[CDR_TYPE_NUMS]  ; //文件各类输出信息    
    LogInfo m_sOutLog ; //文件输出信息
    LogInfo m_sALLOutLog[CDR_TYPE_NUMS]  ; //文件各类输出信息
    int m_iIsOver ;//是否收到文件结束信号    
};

class SHMStatLogBase {
public:
    SHMStatLogBase();

public:
    void bindData();

    static bool m_bAttached;
    static SHMStatLogData *m_poLogData;

    static SHMData_A<SHMStatLogData> *m_poSHMStatLogData;
    static SHMIntHashIndex_A *m_poFileAppIndex;
    void freeAll();
};


class SHMStatLogCtl : public SHMStatLogBase {
public:
    SHMStatLogCtl();
    void create();
    bool exist();
    void remove();
    void display();

public:
    //int  load(char const *sTable);
    //int  out(char const *sTable);
};


class SHMStatLogMgr : public SHMStatLogBase
{
  public:
    SHMStatLogMgr();    
    
  public:    
    void initRecord(SHMStatLogData * pData,int iFileID,int iProcessID,char *sFileName);
    void setOver(int iFileID,int iAppID);
    void addLogInfo(LogInfo * pLog,int iCnt,StdEvent *pEvt);
    bool addInEvent(int iFileID,int iProcessID,int iCnt,StdEvent *pEvt);
    void addInEvent(SHMStatLogData * pDate,int iEvtTypeID,int iCnt,StdEvent *pEvt);
    bool addOutEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt);
    void addOutEvent(SHMStatLogData * pDate,int iEvtTypeID,int iCnt,StdEvent *pEvt);
    int  getALLInfo(int iAppID,char *sStartTime, char *sEndTime,vector<SHMStatLogData> &vRetvec) ;
    int getSumInfo(int iFileID,int iAppID);
    void commitLog(int iFileID,int iAppID);
    void commitDB(SHMStatLogData *pDate);
    void insertLog(SHMStatLogData *pDate,LogInfo *pLog,int iLogType);
   void getFileTime(int iFileID, char *sFileTime);
   int  getSourceId(char *sFileName);
   
   //add by dealy
    void addDelayEvent(SHMStatLogData * pData,int iCnt,StdEvent *pEvt) ;
    int getDelayType(int iType,char *sFileTime,char *sEventTime) ; 
    
   void getStringIndex(int iFileID,int iAppID)   ;
   int getBusiclassBySwitch(int iSwitchID)     ;
   int getAppIDByProcess(int iProcessID)  ; 

   void insertMouldeCapabilityKpiLog(int iMouldeID, long lValue, const char* sKpiName);
   void commitRentLog(int iFileID,int iAppID);
   void commitDisctLog(int iFileID,int iAppID);
   void addDisctInEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt);
   void addRentInEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt);
   
   void sumDataInfo(SHMStatLogData * pDes,SHMStatLogData *pSor);
   int getSumInfo(int iAppID,int iBussiID,char *sStartTime, char *sEndTime,SHMStatLogData *pData) ;
   void setEndTime(char *sTime);
   void setStartTime(char *sTime);   
   void  sumLogInfo(LogInfo * pDes,LogInfo *pSor) ;
   void writeFile(SHMStatLogData *pData)   ;
   int revokeAll();
private:
    SeqMgrExt * m_poSeqExt;
    static CSemaphore * m_poDataLock;
    static CSemaphore * m_poIndexLock;

private:
	  long  m_lLongIndex;
	  char m_sProvCode[4+1];
	  int  m_iProvCode ;
};



#endif
