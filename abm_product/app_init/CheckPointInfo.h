#ifndef CHECKPOINT_INFO_H_HEADER_INCLUDED_BDD_1222
#define CHECKPOINT_INFO_H_HEADER_INCLUDED_BDD_1222

#include "Log.h"
#include "CSemaphore.h"

#define _CHECKPOINT		"CHECKPOINT"
#include "CommandCom.h"
#include "SysParamDefine.h"
class CheckPointInfo
{
	  public:
	  		  CheckPointInfo();
			  ~CheckPointInfo();
	  public: //HSS函数
	  	      char *getChkPointBatchPath(char *batch);
			  char *getNowBatch();
			  bool createBatchID();
	  public: //2.8使用函数
			  // 赋值到sRootPath,获取根目录信息,根目录下根据程序名.各自建立子目录.将落地的文件存放在这个目录下
			  bool getCheckPointPath(char *sRootPath);
			  //获取批次号
			  char *getCharBatchID();
			  //检查路径是否存在,不存在就创建
			  int  CheckFilePath (char * filepath);	
			  void getBatch(); 
			  // 查看批次基本信息
              void showBatchBaseInfo(char *sBatch);  	  
				
			  //#ifdef DEF_SCHEDULE
			  //bool P(bool Real=false);//只以是否发送出去的结果为准
			  //bool V();//只以是否发送出去的结果为准
			  //#else
			  CSemaphore *lock;
			  int m_iLockKey;
			  bool P(bool Real=false);//只以是否发送出去的结果为准
			  bool V();//只以是否发送出去的结果为准
			  //#endif
				
			  int waitTimer(int &iTimer,int iMaxTimeNum); 
			  //批次-时间-落地数据简介(FileMgr)-desc
			  bool recordBatchInfo(char * sBatch, char* sTime, char *sDataType,char*desc=0,char *tablename=0); 
			  bool getNeedState();
	  private:
	          char m_sBatchID[32];
			  char m_sPath[2048];
              long m_lBatchID;
			  CommandCom *m_poCommandCom;
			  //char m_sBathPath[1024];
			  //string strCheckPath; 
};
#endif
