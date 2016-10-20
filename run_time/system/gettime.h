#ifndef _GET_TIME_H
#define _GET_TIME_H
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include "sqltypes.h"
#define GMTDIFF 28800

extern void getSec(char *sOutTime);

int getSystemDate(char *sOutTime);

int getSystemDate(TIMESTAMP_STRUCT &tsSysDate);

int ttDateToStr(TIMESTAMP_STRUCT &cInTimestamp, char *sOutDate) ;

int strToTTDate(char *sInSystemDate,TIMESTAMP_STRUCT &cOutTimestampStruct);
#endif

