#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "DateTime.h"

void sGetTime(time_t tStartDate,char sStartDate[])
{
	struct tm *now;
	char 	sDateTemp[15];
	now=localtime(&tStartDate);
	strftime(sDateTemp,15,"%Y%m%d%H%M%S",now);
	sDateTemp[14]='\0';
	strcpy(sStartDate,sDateTemp);
}

time_t tGetTime(char sAnswerTime[15])
{
    char sTempYear[5],sTempMon[3],sTempMDay[3],sTempHour[3],
         sTempMin[3],sTempSec[3];
    struct tm s;
    strncpy(sTempYear,sAnswerTime,4);sTempYear[4]=0;
    sTempMon[0]=sAnswerTime[4];
    sTempMon[1]=sAnswerTime[5];
    sTempMDay[0]=sAnswerTime[6]; sTempMon[2]=0;
    sTempMDay[1]=sAnswerTime[7]; sTempMDay[2]=0;
    sTempHour[0]=sAnswerTime[8];
    sTempHour[1]=sAnswerTime[9]; sTempHour[2]=0;

    sTempMin[0]=sAnswerTime[10];
    sTempMin[1]=sAnswerTime[11]; sTempMin[2]=0;

    sTempSec[0]=sAnswerTime[12];
    sTempSec[1]=sAnswerTime[13]; sTempSec[2]=0;

    s.tm_year=atoi(sTempYear)-1900;
    s.tm_mon=atoi(sTempMon)-1;
    s.tm_mday=atoi(sTempMDay);
    s.tm_hour=atoi(sTempHour);
    sTempMin[0]=sAnswerTime[10];
    sTempMin[1]=sAnswerTime[11]; sTempMin[2]=0;

    sTempSec[0]=sAnswerTime[12];
    sTempSec[1]=sAnswerTime[13]; sTempSec[2]=0;

    s.tm_year=atoi(sTempYear)-1900;
    s.tm_mon=atoi(sTempMon)-1;
    s.tm_mday=atoi(sTempMDay);
    s.tm_hour=atoi(sTempHour);
    s.tm_min=atoi(sTempMin);
    s.tm_sec=atoi(sTempSec);
    s.tm_isdst=0;
    return mktime(&s);
}

const char * get_longtime(const long lTime,char * pszTime)
{
	static char sz[18] = {0};
	static struct timeval tvTime;
	static struct tm* p_tm;
	if (pszTime == NULL)
		pszTime = sz;
	if (lTime <= 0)
	{
		gettimeofday(&tvTime,NULL);
		p_tm = localtime (&(tvTime.tv_sec));
		sprintf(pszTime,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,tvTime.tv_usec/1000L);
	}
	else
	{
		long lSec = lTime/1000;
		p_tm = localtime (&lSec);
		sprintf(pszTime,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,lTime % 1000L);
	}
	return pszTime;
}

long get_curtime_ms()
{
	static struct timeval tvTime;
	gettimeofday(&tvTime,NULL);
	return tvTime.tv_sec * 1000 + tvTime.tv_usec/1000;
}

// ´«ÈëºÁÃë
const char * get_longtime(const char * pszTime)
{
	static char sz[18] = {0};
	static struct timeval tvTime;
	static struct tm* p_tm;
	//gettimeofday(&tvTime,NULL);
	long lSSec = atol(pszTime);
	long lSec = lSSec/1000;
	p_tm = localtime (&lSec);
	sprintf(sz,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,lSSec % 1000L);
	return sz;
}

const char * get_curtime(int iType)
{
	static char sz[18] = {0};
	static struct timeval tvTime;
	static struct tm* p_tm;
	gettimeofday(&tvTime,NULL);
	p_tm = localtime (&(tvTime.tv_sec));
	if (iType == 1)
		sprintf(sz,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,tvTime.tv_usec/1000L);
	else
		sprintf(sz,"%04d%02d%02d%02d%02d%02d",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
	return sz;
}

const char * get_curtime()
{
	static char sz[18] = {0};
	static struct timeval tvTime;
	static struct tm* p_tm;
	gettimeofday(&tvTime,NULL);
	p_tm = localtime (&(tvTime.tv_sec));
	sprintf(sz,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,tvTime.tv_usec/1000L);
	return sz;
}

const char * get_curtime(char * pszTime,int iType)
{
	if (pszTime == NULL)
	{
		static char sz[18] = {0};
		static struct timeval tvTime;
		static struct tm* p_tm;
		gettimeofday(&tvTime,NULL);
		p_tm = localtime (&(tvTime.tv_sec));
		if (iType = 1)
			sprintf(sz,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,tvTime.tv_usec/1000L);
		else
			sprintf(sz,"%04d%02d%02d%02d%02d%02d",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
		return sz;
	}
	else
	{
		struct timeval tvTime;
		struct tm* p_tm;
		gettimeofday(&tvTime,NULL);
		p_tm = localtime (&(tvTime.tv_sec));
		if (iType = 1)
			sprintf(pszTime,"%04d%02d%02d%02d%02d%02d%03ld",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec,tvTime.tv_usec/1000L);
		else
			sprintf(pszTime,"%04d%02d%02d%02d%02d%02d",p_tm->tm_year + 1900,p_tm->tm_mon + 1,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
		return pszTime;
	}
}
