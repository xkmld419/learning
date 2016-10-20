// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef LOG_H_HEADER_INCLUDED_BD8F4FE0
#define LOG_H_HEADER_INCLUDED_BD8F4FE0

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//#include </usr/include/stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Date.h"
#include "Exception.h"

//#include "CommonMacro.h"

//##ModelId=4270E21703DE
class Log
{
  public:
    //##ModelId=4270E91103E7
    Log();

    //##ModelId=4270E9120027
    virtual ~Log();

    //##ModelId=4270E93D03CC
    static void setFileName(char *name);

    //##ModelId=4270E983020A
    static void log(int log_level, char *format, ...);
    static void log(int log_level, const char *format, ...);

  private:
    //##ModelId=4270E22C021C
    static int m_iLogLevel;

    //##ModelId=4270E89302D7
    static char m_sLogFileName[256];

    //##ModelId=4270E89F00D6
    static FILE *m_pLogFile;

};



#endif /* LOG_H_HEADER_INCLUDED_BD8F4FE0 */
