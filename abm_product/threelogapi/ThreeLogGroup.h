#ifndef _LIB_THREE_LOG_H_
#define _LIB_THREE_LOG_H_

#include <map>
#include <math.h>
#include <dirent.h>

#include "SeqMgrExt.h"
#include "ThreeLogGroupBase.h"
#include "ThreeLogGroupDefine.h"
#include "HBECodeParse.h" 
#include "Log.h"
#include <vector>

#include "../../include/Env.h"

using namespace std;
//#include "CommandCom.h"

#define  CALLBACKPOINT      90                      //共享内存回收点
#define  FILEMIN            0                       //文件最小值
#define  FILEMAX            500                     //文件最大值


class ThreeLogGroup : public ThreeLogGroupBase
{
 public:
    ThreeLogGroup(bool bGreat = false);
    ~ThreeLogGroup();

    bool  init();
    bool  AddLogGroup(int GroupId);
    bool  AddLogGroupFile(int GroupId,char *pPath,bool bmml=false);

    void  LoggLog(int iLogType,char const *format, ...);
    void  bakGroupFile(int LogGroupId,char *path);
    void  CheckLogDir(char* Path);
    bool  CheckLogFile(int iGroupId);
    void  DisplayFileList(int GroupId,char *sDest = NULL);

    int   GetGroupNum();
    int   GetUseGroup();
    int   GetGroupState(int GroupId,char *sRetVal = NULL);
    int   GetGroupWriteMode();
    int   GetGroupFileSeq(int GroupId);
    int   GetGroupInfoUsed(int GroupId);
	int   GetGroupInfoTotal(int GroupId);

    bool  GetGroupFilePath(int GroupId);
    bool  GetGroupPigeonhole(int GroupId);
    bool  GetPigeonholeFileName(char *pFileName);
    bool  GetCurUsedInfoData(int GroupId);
    bool  GetCurUsedInfoIndex(int GroupId);
    bool  GetCurUsedInfoLock(int GroupId);
    bool  GetCurUseFileName(int GroupId,char *pFileName,char *pFilePath);
    bool  GetProNameByModule(int iParam, char * sReturn);
    
    long  GetGroupMaxSize();
    long  GetGroupMaxFileSize();
    
    char* GetGroupBakPath();

    double GetGroupSize(int GroupId);
    double GetGroupPercent(int GroupId);
    double GetGroupInfoPercent(int GroupId);

    bool  DisplayLogGroup(int GroupId,char * sDest = NULL);
    bool  ClearGroupFile(int LogGroupId,char *pFileName,bool bmml=false);
    bool  ChangeLogGroup(bool bmml,int GroupId=-1);

 public:
    bool  SetLogGroupPigeonhole(bool bParam);
    bool  SetGroupBakPath(char *path);
    bool  SetGroupDefaultPath(char *path);
    bool  SetGroupMaxFileSize(int size);
	bool  RefreshGroupMaxFileSize(int size);
    bool  SetGroupMaxSize(int size);
    bool  SetGroupWriteMode(int mode);
    bool  SetCheckPointTouch(int num);
    bool  SetLogLevel(int level);
	bool  SetFileCheckPercent(int iValue);
    
    void  SetGroupFilePath(int GroupId,int FileSeq,char *path);
    void  InitLogGroup();

    //数据展现接口
    bool CheckParam(int iLogTypeId,int iLogDisFlag,int iLogClassId,char *sParam,LogInfoData *pInfo);
    bool GetField(int iSeq,char *sBuf,char *sParam);
    bool GetInfoFromMem(vector<string> &v_list,int iLogTypeId,int iLogDisFlag,
                 int iLogClassId=0,char *sParam = NULL);
   bool GetInfoFromMem(vector<string> &v_list,
                         int iLogFlag,int iLogCond, char *sParam, char *sBeginTime = NULL, char *sEndTime = NULL);
    
    //清仓接口
    bool  CheckInfoCallBack();
    bool  CommitInfoFile();
    bool  FileDownInfo(int GroupId, bool bPrintFlag = true);
    bool  CallBackInfo(int GroupId);
    bool  Check75CheckPoint(bool &bCheck);
	void  UpdateLogFileArchiveSeq();

	bool  RefreshLogFilePath(int iGroupID,int iFileSeq,char *sNewPath);
    //void  CheckLogFile();

    //对外写数据接口
    void  log(int iCodeId,int iLogLevel,int iLogClassId,int iLogTypeId,
              int iAction,const char *pExpand,const char* format,...);
    void log(int iCodeId,int iLogLevel, int iLogTypeId,const char* format,...);

	//add by liyr
	void Product_log(int iCodeId,int iProcId,int iAppId,int iLogLevel,int iLogTypeId,
					int iNetId,const char *sFlow,const char *format,...);

	//iMode 1:是返给查询，信息多一些    2:是落地使用，有字段要求
	void combineLog(LogInfoData *p, char *sDest, int iMode = 1);

	void refreshArchivePath();
    //对外查询接口         
    bool  GetLoggInfo(vector<LogInfoData> &v_list,int iLogFlag,int iLogCond, char *sBTime,char *sETime=NULL);
    bool  GetAllLoggInfo(vector<LogInfoData> &v_list);

 private:
	bool  AddInfoDataInfo(int iCodeId,int iLogProcid,int iThreadId,int iLogAppid,int iLogLevel,
							 int iLogClassId,int iLogTypeId,int iAction,int iNetId,const char *sFlow,const char *pExpand,const char *pData,ThreeLogGoupData* p);
    int  getClassID(int iLevelID);
   
 private:
    int  m_iMaxGroupNum;                //日志组的块数
    int  m_iCurUseGroupId;              //当前正在是用的日志组标识
    long m_lMaxLogFileSize;             //日志文件的最大资源 单位:byte
    long m_lMaxLogDirSize;              //每个日志组的最大资源 单位:byte
    char m_sBakLogGroupPath[500];       //归档备份目录
    char m_sDefaultLogGroupPath[500];   //日志组成员文件没有指定目录情况下默认配置目录
    char m_sCheckPointPath[500];
    SeqMgrExt* m_poSeqExt;              //文件流水号
    map<string,string> m_mFilePathMap;  //文件路径
    int  m_iWriteMode;                  //写数据的方式(0-文件 1-共享内存)
    long m_lInfoDataSize;               //共享内存数据区大小
    int  m_iCheckPointTouch;            //checkpoint触发日志组切换点
 public:
    //CommandCom *m_poCmdCom;             //核心参数
    int m_iLogProid;
    int m_iLogAppid;
    int m_iLogLevel;
    Date m_dDT ;
    char m_ssAreaCode[4+1];
    
 private:
    HBErrorCode *m_pErrorCode;

};

#endif //_LIB_THREE_LOG_H_
