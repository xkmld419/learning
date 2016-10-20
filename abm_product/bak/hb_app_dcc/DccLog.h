#ifndef _DCCLOG_H
#define _DCCLOG_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include "Date.h"

class DccLog
{
public:
	DccLog();
	static void setFileName(char *sPath, char *sPrefixName, int iProcID);
	static void log(char const *format, ...);      // 需判断当前时间与日志创建时间是否为同一天
	static void close();

private:
    static char m_sLogFileName[256];
	static int  m_nFd;
	static Date m_dtDate;   // 日志创建时间
};

#endif

