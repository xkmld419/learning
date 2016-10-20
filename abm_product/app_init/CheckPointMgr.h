#ifndef CHECKPOINT_MGR_H_HEADER_INCLUDED_BDD_1222
#define CHECKPOINT_MGR_H_HEADER_INCLUDED_BDD_1222

#include "Log.h"
#include <vector>
#include <string.h>
//#include "string.h"
#include "SysParamDefine.h"
#include "SHMLruData.h"
#include "CheckPointInfo.h"
#include "CmdMsg.h"

#define CHECKPOINT		"CHECKPOINT"
#define MAX_STD_INT_LEN  		4
//#define CHECKPOINTPATH		"/cbilling/app/DEV_TMP/src_jt/log/checkpoint"

class ThreeLogGroup;
class ThreeLogGroupMgr;
//class CommandCom;
//struct HighFeeData;

//add by hsir
class CheckPointMgr
{
    public:
            CheckPointMgr();
            ~CheckPointMgr();
			bool touchInfoDown();
            bool touchInfoDown(bool Show,bool RealTime=false);//checkpoint程序自己生成批次编号，落地所有checkpoint涉及共享内存
			char *getCharBatchID();
			
			bool touchInfoDown(int iAppID);//根据APPID落地相应共享内存
            bool touchInfoDown(char *sBatchID);//根据外界提供的批次编号落地所有checkpoint涉及共享内存
            
            // 上载对应批次编号的文件到内存
			// 参数说明:sBatch是批次标识.
			bool touchInfoLoad(char *sBatch);
			
			// 上载对应批次编号的文件到内存
			// 参数说明:sBatch是批次标识.
			bool touchInfoLoad(char *sBatch, int iAppID);
			
			// 赋值到sRootPath,获取根目录信息,根目录下根据程序名.各自建立子目录.将落地的文件存放在这个目录下
			bool getCheckPointPath(char *sRootPath);
			// 根据落地批次编号获取根目录路径
			bool getDatePath(char *sBatch);
			int  CheckFilePath (char * filepath);
	public: //
			long getLongBatchID();
			long getBatchID(); 	
    public: 
            // 查看批次基本信息
            void showBatchBaseInfo(char *sBatch);
			//判断文件是否存在
			bool FileExist(char *sFileName);   
			void getBatch();      
    private:
            //FileMgr *m_poFileMgr;
            ThreeLogGroupMgr *m_poLogGroup;
            char m_sBatchID[32]; //批次号
			char m_sPath[1024];
            long m_lBatchID;
			//CommandCom *m_poCommandCom;
			
			char sHFKey[128];
			char sHFKeyName[128];
			char sHFFileName[1024];			      
			//SHMLruData<HighFeeData> *m_poSHMHFData;	 
			void formatInt(int iNum,char *sStdInt);
			char sStdInt[MAX_STD_INT_LEN+1];
			ThreeLogGroup *m_poLog;
			CheckPointInfo *m_poCheckPointInfo;     		     
};

#endif
