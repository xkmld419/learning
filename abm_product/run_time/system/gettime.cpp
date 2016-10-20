#include "gettime.h"
#include <string.h>
#define DATE_SIZE 15

void getSec(char *sOutTime)
{
	timeval tTimeval;
	tm  *Time;
	gettimeofday(&tTimeval,NULL);
	long lSecond = tTimeval.tv_sec+GMTDIFF;
    Time = gmtime(&lSecond);
    sprintf(sOutTime,"%04d%02d%02d%02d%02d%02d",
                    Time->tm_year+1900,
                    Time->tm_mon+1,
                    Time->tm_mday,
                    Time->tm_hour,
                    Time->tm_min,
                    Time->tm_sec);
}

int getSystemDate(char *sOutTime)
{
	memset(sOutTime,0x00,sizeof(sOutTime));
	timeval tTimeval;
	tm  *Time;
	gettimeofday(&tTimeval,NULL);
	long lSecond = tTimeval.tv_sec+GMTDIFF;
    Time = gmtime(&lSecond);
    sprintf(sOutTime,"%04d%02d%02d%02d%02d%02d",
                    Time->tm_year+1900,
                    Time->tm_mon+1,
                    Time->tm_mday,
                    Time->tm_hour,
                    Time->tm_min,
                    Time->tm_sec);
	return 1;
}
int getSystemDate(TIMESTAMP_STRUCT &tsSysDate)
{
	
	timeval tTimeval;
	tm  *Time;
	gettimeofday(&tTimeval,NULL);
	long lSecond = tTimeval.tv_sec+GMTDIFF;
    Time = gmtime(&lSecond);
    tsSysDate.year = Time->tm_year+1900 ;
    tsSysDate.month = Time->tm_mon+1 ;
    tsSysDate.day = Time->tm_mday;
    tsSysDate.hour = Time->tm_hour;
    tsSysDate.minute =Time->tm_min;
    tsSysDate.second =  Time->tm_sec;
    tsSysDate.fraction=0;
	return 1;
}

int strToTTDate(char *sInSystemDate,TIMESTAMP_STRUCT &cOutTimestampStruct)
{
    char buf[DATE_SIZE];
    
    strncpy(buf, sInSystemDate, DATE_SIZE);
    int size = strlen(buf);
     
    for(;size<DATE_SIZE-1;size++) {
        buf[size] = '0';
    }    
    
    cOutTimestampStruct.year = (buf[0] - '0') * 1000 + (buf[1] - '0') * 100 \
                                + (buf[2] - '0') * 10 + (buf[3] - '0') ;
    cOutTimestampStruct.month = (buf[4] - '0') * 10 + (buf[5] - '0') ;
    cOutTimestampStruct.day = (buf[6] - '0') * 10 + (buf[7] - '0') ;
    cOutTimestampStruct.hour = (buf[8] - '0') * 10 + (buf[9] - '0') ;
    cOutTimestampStruct.minute = (buf[10] - '0') * 10 + (buf[11] - '0') ;
    cOutTimestampStruct.second = (buf[12] - '0') * 10 + (buf[13] - '0') ;
    cOutTimestampStruct.fraction = 0;
        
    return 0;
}

int ttDateToStr(TIMESTAMP_STRUCT &cInTimestamp, char *sOutDate) 
{
    if(sOutDate == NULL)
        return -1;
    if(cInTimestamp.year==0)
        memset(sOutDate,0,sizeof(sOutDate));
    else
    sprintf(sOutDate, "%04d%02d%02d%02d%02d%02d", cInTimestamp.year, cInTimestamp.month,
        cInTimestamp.day, cInTimestamp.hour, cInTimestamp.minute, cInTimestamp.second);

    return 0;
}


