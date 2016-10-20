#ifndef _LIB_THREE_LOG_BASE_H_
#define _LIB_THREE_LOG_BASE_H_

#include "SHMData.h"
#include "ReadIni.h"
#include "SHMData_A.h"
#include "CSemaphore.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_A.h"
#include "MBC.h"

#include "Env.h"

using namespace std;
#define  GROUP_NAME_LENGTH        64
#define  LOG_DATA_SIZE           320                     //日志内容大小

#define  GROUP_LOG_NUMS     6                       //日志组个数
#define  GROUP_FILE_CNT     6                       //日志组成员文件个数

/*
#define SHM_LOG_GROUP_DATA          (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_DATA"))
#define SHM_LOG_GROUP_INDEX         (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INDEX"))
#define SHM_LOG_GROUP_LOCK          (IpcKeyMgr::getIpcKey(-1, "LOCK_LOGGROUP_DATA"))
#define SHM_LOG_GROUP_INDEX_LOCK    (IpcKeyMgr::getIpcKey(-1, "LOCK_LOGGROUP_INDEX_DATA"))

#define SHM_LOG_GROUP_INFO_DATA_1       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_1"))
#define SHM_LOG_GROUP_INFO_DATA_2       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_2"))
#define SHM_LOG_GROUP_INFO_DATA_3       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_3"))
#define SHM_LOG_GROUP_INFO_DATA_4       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_4"))
#define SHM_LOG_GROUP_INFO_DATA_5       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_5"))
#define SHM_LOG_GROUP_INFO_DATA_6       (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_DATA_6"))

#define SHM_LOG_GROUP_INFO_INDEX_1      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_1"))
#define SHM_LOG_GROUP_INFO_INDEX_2      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_2"))
#define SHM_LOG_GROUP_INFO_INDEX_3      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_3"))
#define SHM_LOG_GROUP_INFO_INDEX_4      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_4"))
#define SHM_LOG_GROUP_INFO_INDEX_5      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_5"))
#define SHM_LOG_GROUP_INFO_INDEX_6      (IpcKeyMgr::getIpcKey(-1, "SHM_LOGGROUP_INFO_INDEX_6"))

#define SHM_LOG_GROUP_INFO_LOCK		(IpcKeyMgr::getIpcKey(-1, "LOCK_LOGGROUP_INFO_DATA"))
*/
#define SHM_LOG_GROUP_COUNT 6


class LogFileData
{//文件管理类
 public:
    LogFileData():m_lFileSize(0)
    {
        memset(m_sFileName,0,sizeof(m_sFileName));
        memset(m_sGroupPath,0,sizeof(m_sGroupPath));
    }
 public:
    long m_lFileSize;               //文件大小:byte
    char m_sFileName[100];          //文件名
    char m_sGroupPath[500];         //文件路径
};

class LogInfoData
{//内存信息数据区
 public:
    LogInfoData():m_illCode(-1),m_illProId(-1),m_illAppId(-1),m_illLevelId(-1),m_illClassId(-1),
                    m_illTypeId(-1),m_iNextOffset(0),m_iThreadId(-1),m_iNetId(-1)
    {
        memset(m_sAreaCode,0,sizeof(m_sAreaCode));
        memset(m_sLogTime,0,sizeof(m_sLogTime));
        memset(m_sCodeName,0,sizeof(m_sCodeName));
        memset(m_sInfo,0,sizeof(m_sInfo));
        memset(m_sActionName,0,sizeof(m_sActionName));
		memset(m_sFlow,0,sizeof(m_sFlow));
    }
 public:
    int m_illCode;                  //日志编码
    char m_sLogTime[17+1];          //日志时间
    int m_illProId;                 //进程号
    int m_illEmployee;              //操作员工号 
    int m_illLevelId;               //日志等级标识
    int m_illClassId;               //日志类别标识
    int m_illTypeId ;               //日志分类标识
	int m_illAppId;                 //模块号    
    int m_illActionId;              //参数修改类型或SQL操作类型
    char m_sActionName[GROUP_NAME_LENGTH];//参数名或表名
    char m_sAreaCode[4+1];          //本地网标识
    char m_sCodeName[GROUP_NAME_LENGTH];  //编码名称
    int m_iThreadId;               //线程号
    int m_iNetId;					//对端网元标识
    char m_sFlow[8];				//流向eg. send  recv
    
    char m_sInfo[LOG_DATA_SIZE];         //日志内容

    unsigned int m_iNextOffset;     //游标
};

class ThreeLogGoupData
{
 public:
    ThreeLogGoupData():m_iGoupId(0),m_iSeq(0),m_iState(0)
        ,m_bPigeonhole(false),m_bCheckPoint(false),m_bPigeonholeState(0),
        m_lCurFileSize(0),m_lCurDirSize(0),m_iNext(0),m_iWritSeq(0),m_iMemSeq(0)
        ,m_lPredictFileSize(0),m_iFileCheckPercent(0),m_lFileSizeThreshold(0)
    {
    }
 public:
	int m_iGoupId;                      //日志组标识
	int m_iLevel ;                      //日志等级
	int m_iSeq;                         //文件数
	int m_iState;                       //当前状态
	int m_bPigeonholeState;             //归档状态
	int m_iMemSeq;                      //记录内存区序数
	int m_iWritSeq;                     //记录文件区序数
	bool m_bPigeonhole;                 //是否归档
	bool m_bCheckPoint;                 //是否checkpoint
	long m_lCurFileSize;                //当前文件大小:byte

	long m_lPredictFileSize;			//当前日志组实际可落地大小，需要预测,因为落地延迟，可能导致m_lCurFileSize不准确

	int m_iFileCheckPercent;           //当文件大小达到此百分比时候，切换日志组
	long m_lFileSizeThreshold;         //文件大小的阈值
		
	long m_lCurDirSize;                 //当前日志组大小:byte
	LogFileData m_File[GROUP_FILE_CNT]; //成员文件
	unsigned int m_iNext;               //下一个日志组
};

class ThreeLogGroupBase
{
public:
    ThreeLogGroupBase();
    ~ThreeLogGroupBase();

	long getLogGroupDataKey(int iGroupId);
	long getLogGroupIndexKey(int iGroupId);
	
	void initBase();

protected:
    static void bindData();
    static void bindGroupData(int GroupId);
    static void freeAll();
    static bool m_bAttached;

    static ThreeLogGoupData *m_poLogGroup;
    static SHMIntHashIndex_A *m_poLogGroupIndex;
    static SHMData<ThreeLogGoupData> *m_poLogGroupData;

    static SHMData_A<LogInfoData> *m_poInfoDataAll[GROUP_LOG_NUMS+1];
    static SHMIntHashIndex_A *m_poInfoIndexAll[GROUP_LOG_NUMS+1];

    static CSemaphore* m_poInfoDataLock;                       //全局数据区锁
    static CSemaphore* m_poDataLock;                           //日志组信息锁
    static CSemaphore* m_poIndexLock;                          //索引区锁
    static CSemaphore* m_poLogGroupMgrLock;					  //落地归档等锁

    static LogInfoData *m_poInfoGroup;                          //管理区
    static SHMData_A<LogInfoData> *m_poInfoData;                //全局数据区
    static SHMIntHashIndex_A *m_poInfoIndex;                    //全局索引区

    static bool getIpcCfg() ;
private :
	 static long SHM_LOG_GROUP_DATA   ;
   static long SHM_LOG_GROUP_INDEX        ;
   static long SHM_LOG_GROUP_LOCK         ;
   static long SHM_LOG_GROUP_INDEX_LOCK   ;
   static long SHM_LOG_GROUP_INFO_LOCK ;
   static long SHM_LOG_GROUP_MGR_LOCK ;
   static long SHM_LOG_GROUP_INFO_DATA_1  ;
   static long SHM_LOG_GROUP_INFO_DATA_2  ;
   static long SHM_LOG_GROUP_INFO_DATA_3  ;
   static long SHM_LOG_GROUP_INFO_DATA_4  ;
   static long SHM_LOG_GROUP_INFO_DATA_5  ;
   static long SHM_LOG_GROUP_INFO_DATA_6  ;
   static long SHM_LOG_GROUP_INFO_INDEX_1 ;
   static long SHM_LOG_GROUP_INFO_INDEX_2 ;
   static long SHM_LOG_GROUP_INFO_INDEX_3 ;
   static long SHM_LOG_GROUP_INFO_INDEX_4 ;
   static long SHM_LOG_GROUP_INFO_INDEX_5 ;
   static long SHM_LOG_GROUP_INFO_INDEX_6 ;     
   static bool SHM_KEY_IFLOAD;
};

#endif //_LIB_THREE_LOG_BASE_H_
