/*VER: 1*/ 
// Copyright (c) 2010,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef LOGDB_H_HEADER_INCLUDED
#define LOGDB_H_HEADER_INCLUDED

#include "LogDBBuf.h"
#include "Environment.h"

#define LOGDB_FILE_LOCK (IpcKeyMgr::getIpcKey(-1, "LOCK_LogDBFile"))

class LogDB
{
public:
    LogDB();
	virtual ~LogDB();
	
	static void logDB(int log_level, int log_type, char const *format, ...);
	static void init();
	static int logFileOper();
	static void writeLogDB();
	static void insertDBData(LogDBData & oLogInfo);
	static int getSysProc();
	static void freeSHMBuf();
	
private:
	static bool m_bInit;
	
	static int m_iLogLevel;
    static int m_iSysProcID;
	
	static char m_sInitPath[80];
	static char m_sLockPath[100];
	
    static CSemaphore * m_poLogDBFileLock;

	static LogDBBuf * m_pLogDBBuf;
	static TOCIQuery * m_pQry;
	
    static int m_iLogSeqID;
    static int m_iLogSeqIDMax;
    
	static int m_iLogDBSwitch;
};

#endif
