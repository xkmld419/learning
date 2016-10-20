#ifndef CHECKPOINT_H_HEADER_INCLUDED_BDD
#define CHECKPOINT_H_HEADER_INCLUDED_BDD

#include "Log.h"
#include <vector>


#include "SysParamInfo.h"
#include "SysParamDefine.h"
#include "CheckPointMgr.h"
//#include "CommandCom.h"

#define MAX_WAIT_LONG_TIME  1700   // 等待返回消息的时间,单位是0.1秒
#define MAX_WAIT_T  700

struct FileInfo
{
	char sline[192];
	char date[16];
	char batchid[16];
	int  pid;
	char cline;
};

class CheckPoint
{
    public:
            CheckPoint();
            ~CheckPoint();
            
			int run();
			bool touchInfoDown();
    public:          
            void getBatch();
			void chkrec();//落地信息数据记录在文件 
	private:
			CheckPointMgr* m_pCheckPoint;
	private:
			bool Link;
			bool getData();
            char sBatch[512];
			char sWaitTime[16];
			int iWaitTime;
};

#endif
