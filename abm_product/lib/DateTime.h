// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef DATETIME_H_HEADER_INCLUDED_BE09DCD0
#define DATETIME_H_HEADER_INCLUDED_BE09DCD0

extern void 	sGetTime(time_t tStartDate,char sStartDate[]);
    
extern time_t   tGetTime(char sAnswerTime[15]);

extern const char * get_longtime(const long lTime,char * pszTime = NULL);

extern long get_curtime_ms();

extern const char * get_curtime(int iType = 1);

extern const char * get_longtime(const char * pszTime);

extern const char * get_curtime(char * pszTime,int iType = 1);

#endif /* DATETIME_H_HEADER_INCLUDED_BE09DCD0 */




