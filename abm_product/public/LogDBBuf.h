#ifndef LOGDBBUF_H_HEADER_INCLUDED
#define LOGDBBUF_H_HEADER_INCLUDED

#include <list>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CSemaphore.h"
#include "SimpleSHM.h"
#include "CommonMacro.h"
#include "MBC.h"
#include "UserInfo.h"

#define MAX_SHMBUF_NUM 1000
#define MAX_STRING_NUM 512

struct LogDBData
{
    int iProcID;
	int iFlowID;
	int iAppID;
	int iLogLevel;
	int iLogType;
	char sCreatedDate[MAX_CREATED_DATE_LEN];
	char sLogInfo[MAX_STRING_NUM];
};

struct LogDBSHMData
{
    LogDBData oLogDBData[MAX_SHMBUF_NUM];
	unsigned int iMaxNum;
	unsigned int iFullNum;
	unsigned int iWriteNum;
	unsigned int iReadNum;
};

class LogDBBuf
{
public:
    LogDBBuf();
	virtual ~LogDBBuf();
    
    bool writeCirBuf(LogDBData & oLogDBData);
    bool readCirBuf(LogDBData & oLogDBData);
    
    int inUseNum();
    int freeUseNum();
    int totleNum();
    bool isFull();
    bool isEmpty();
    
    void init();
	
    void unloadSHMBuf();
	
private:
    static USERINFO_SHM_DATA_TYPE<LogDBSHMData> *m_poLogDBData;
    
    static CSemaphore * m_poLogDBDataLock;
};
#endif
