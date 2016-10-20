/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.


#include "Log.h"
#include "mntapi.h"
#include "ParamDefineMgr.h"
#include "Process.h"
#include "Environment.h"
#include "MBC.h"
#include "ThreeLogGroup.h"
using namespace std;
//##ModelId=4270E91103E7
Log::Log()
{
    char *p;

    /* LOG_LEVEL */
    if (m_iLogLevel == -1) {
        if ((p=getenv("LOG_LEVEL")) == NULL)
            m_iLogLevel = 0;
        else
            m_iLogLevel = atoi (p);
    }
}


//##ModelId=4270E9120027
Log::~Log()
{
}

//##ModelId=4270E93D03CC
void Log::setFileName(char *name)
{
    strncpy (m_sLogFileName, name, 256);
}

void Log::setAlarmName(char * name)
{
    strncpy(m_sAlarmFileName,name,256);
}

void Log::setParamName(char * name)
{
    strncpy(m_sParamFileName,name,256);   
}

void Log::setLogHome(char *pPath)
{
    strncpy(m_sLog_HOME,pPath,80);
}

void Log::init(int iModleId)
{
    Log::m_iModleId = iModleId;
}

//##ModelId=4270E983020A
void Log::log(int log_level, char const *format, ...)
{
    char *p, sTemp[10000], sLine[10000];
    int fd,cyccnt=ERRCCNT,seq=0;
    ssize_t  sRet;

    /* LOG_LEVEL */
    if (m_iLogLevel == -1) {
        if ((p=getenv("LOG_LEVEL")) == NULL)
            m_iLogLevel = 0;
        else
            m_iLogLevel = atoi (p);
    }

    if (log_level > m_iLogLevel)
         return;

    if(!strlen(m_sLogFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sLogFileName,"%s/Log.log",m_sLog_HOME);
    }
 
    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    //vsnprintf (sLine, 9999, sTemp, ap);
 vsnprintf (sLine,sizeof(sLine)-1, sTemp, ap);   
va_end(ap);
    //printf ("\n(PROCESS_ID:%d)\n",GetProcessID());
    printf ("%s", sLine);
    if((fd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        printf ("[ERROR]: Log::log() open %s error\n", m_sLogFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWrite(sLine,m_sLogFileName,seq))
                return;
            seq++;
        }
        return;
    }
    sRet = write (fd, sLine, strlen (sLine));
    close (fd);
    if(sRet<0)
    {
        if(errno==ENOSPC)
        {//磁盘空间不足
            ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWrite(sLine,m_sLogFileName,seq))
                return;
            seq++;
        }
    }
}

//##ModelId=4270E983020A
void Log::logEx(int log_level, char const *format, ...)
{
    char *p, sTemp[10000], sLine[10000];
    int fd,cyccnt=ERRCCNT,seq=0;
    ssize_t  sRet;

    /* LOG_LEVEL */
    if (m_iLogLevel == -1) {
        if ((p=getenv("LOG_LEVEL")) == NULL)
            m_iLogLevel = 0;
        else
            m_iLogLevel = atoi (p);
    }

    if (log_level > m_iLogLevel)
         return;

    if(!strlen(m_sLogFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sLogFileName,"%s/Log.log",m_sLog_HOME);
    }
 
    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);
    //printf ("\n(PROCESS_ID:%d)\n",GetProcessID());
    //printf ("%s", sLine);
    if((fd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        printf ("[ERROR]: Log::log() open %s error\n", m_sLogFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            //ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        //ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWriteEx(sLine,m_sLogFileName,seq))
                return;
            seq++;
        }
        return;
    }
    sRet = write (fd, sLine, strlen (sLine));
    close (fd);
    if(sRet<0)
    {
        if(errno==ENOSPC)
        {//磁盘空间不足
            //ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        //ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWriteEx(sLine,m_sLogFileName,seq))
                return;
            seq++;
        }
    }
}
void Log::pointlog(int log_level,char const * format,...)
{
    char *p, sTemp[10000], sLine[10000],sLvel[10];
    int fd,cyccnt=ERRCCNT,seq=0;
    ssize_t  sRet;
    bool bWrite = false;

    switch(log_level)
    {
        case 1:
            if(m_iInfoLevel==1 || m_iInfoLevel==2 || m_iInfoLevel==3)
            {
                sprintf(sLvel,"LV-1");
                bWrite = true;
            }
            break;
        case 2:
            if(m_iInfoLevel==2 || m_iInfoLevel==3)
            {
                sprintf(sLvel,"LV-2");
                bWrite = true;
            }
            break;
        case 3:
            if(m_iInfoLevel==3)
            {
                sprintf(sLvel,"LV-3");
                bWrite = true;
            }
            break;
        default:
            break;                        
    }

    if(bWrite==false)
        return ;
        
    if(!strlen(m_sLogFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sLogFileName,"%s/Log.log",m_sLog_HOME);
    }

    Date dt;
    sprintf(sTemp,"[%s]: [%s]%s\n",dt.toString("yyyy-mm-dd hh:mi:ss"),sLvel,format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);
    
    if(Log::m_iMode==0)
    {
        if((fd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) 
        {
            printf ("[ERROR]: Log::pointlog() open %s error\n", m_sLogFileName);
            if(errno==ENOSPC)
            {//磁盘空间不足
                ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
                return;
            }
            ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
            while(cyccnt>0)
            {//写文件失败，另起一个文件写
                cyccnt--;
                if(ErrFileWrite(sLine,m_sLogFileName,seq))
                    return;
                seq++;
            }
            return;
        }
//      printf ("%s", sLine);
        sRet = write (fd, sLine, strlen (sLine));
        close (fd);
        if(sRet<0)
        {//写文件失败，另起文件写
            if(errno==ENOSPC)
            {//磁盘空间不足
                ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
                return;
            }
            ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sLogFileName);
            while(cyccnt>0)
            {//写文件失败，另起一个文件写
                cyccnt--;
                if(ErrFileWrite(sLine,m_sLogFileName,seq))
                    return;
                seq++;
            }
        }
    }
    else if(Log::m_iMode==1)
    {//写私有
        string strLogInfo(sLine);
        m_vLogInfo.push_back(strLogInfo);        
    }
    else if(Log::m_iMode==2)
    {//写日志组
       // LOGG(OPERATIONLOG,sLine);
    }
}

void Log::alarmlog(int log_level,int ErrId,int err,int line,char const *file,char const *function,char const *format, ...)
{
    char *p, sTemp[10000], sLine[10000], sForm[10000],sErrid[20];
    int fd,cyccnt=ERRCCNT;
    bool bWrite = false,bAlarmDb = false;
    char *pt;
    ssize_t  sRet;
    errno=0;
#ifdef ALARM_LOG_OFF
    return;
#endif
    
    switch(log_level)
    {
        case 0:
            if(m_iAlarmLevel!=-1)
                bWrite = true;
            if(m_iAlarmDbLevel!=-1)
                bAlarmDb = true;
            break;
        case 1:
            if(m_iAlarmLevel==1 || m_iAlarmLevel==2 || m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==1 || m_iAlarmDbLevel==2 || m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        case 2:
            if(m_iAlarmLevel==2 || m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==2 || m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        case 3:
            if(m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        default:
            break;                        
    }

    if(bWrite==false)
        return ;
    
    if(!strlen(m_sAlarmFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sAlarmFileName,"%s/Log_alarm.log",m_sLog_HOME);
    }
    
    if(Log::m_iProcessId!=-1)
    {
        if((pt = getenv ("PROCESS_ID")) == NULL) 
            Log::m_iProcessId = -1;
        else
            Log::m_iProcessId = atoi (pt);
    }
    
    Date dt;
    sprintf(sErrid,"%d",ErrId);

    sprintf (sTemp, "[%s]: [ALARM][ALARM_TYPE:%d]--ProcId:[%d] ModuleId:[%d] File:[%s] LineNo:[%d] function:[%s] ErrId:[%s] ErrNo:[%d]",
                dt.toString("yyyy-mm-dd hh:mi:ss"),log_level,Log::m_iProcessId,m_iModleId,file,line,function,sErrid,err);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sForm, 9999, format, ap);
    va_end(ap);
    sprintf(sLine,"%s[Msg]%s\n",sTemp,sForm);
    if(m_iModleId != MONITOR)
        printf ("%s", sLine);

    //LOGG(OPERATIONLOG,sLine);
    if((fd=open (m_sAlarmFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {//告警不了咯
        printf ("[ERROR]: Log::alarmlog() open %s error\n", m_sAlarmFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            char altemp[200]={0};
            if(m_iModleId != MONITOR)
                printf("[ALARM]: Log::alarmlog() 磁盘空间不足!\n");
            sprintf(altemp,"[ALARM]: open %s error 磁盘空间不足!\n",m_sAlarmFileName);
           // m_pologg->LogGroupWriteFile(OPERATIONLOG,altemp);
            if(bAlarmDb)
                AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
        }
        return;
    }
    sRet = write (fd, sLine, strlen (sLine));
    close (fd);
    if(sRet<0)
    {//告警不了咯
        if(m_iModleId != MONITOR)
            printf ("[ERROR]: Log::alarmlog() write %s error\n", m_sAlarmFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            char altemp[200]={0};
            if(m_iModleId != MONITOR)
                printf("[ALARM]: Log::alarmlog() 磁盘空间不足!\n");
            sprintf(altemp,"[ALARM]: open %s error 磁盘空间不足!\n",m_sAlarmFileName);
            //m_pologg->LogGroupWriteFile(OPERATIONLOG,altemp);
            if(bAlarmDb)
                AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
        }
        return;
    }
    if(bAlarmDb)
        AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
}

void Log::alarmlog28(int log_level,int Class_id,int ErrId,int err,int line,char const *file,char const *function,char const *format, ...)
{
    char *p, sTemp[10000], sLine[10000], sForm[10000],sErrid[20];
    int fd,seq=0;
    bool bWrite = false,bAlarmDb = false;
    char *pt;
    int cyccnt=ERRCCNT;
    ssize_t  sRet;
    errno=0;   
    switch(log_level)
    {
        case 0:
            if(m_iAlarmLevel!=-1)
                bWrite = true;
            if(m_iAlarmDbLevel!=-1)
                bAlarmDb = true;
            break;
        case 1:
            if(m_iAlarmLevel==1 || m_iAlarmLevel==2 || m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==1 || m_iAlarmDbLevel==2 || m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        case 2:
            if(m_iAlarmLevel==2 || m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==2 || m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        case 3:
            if(m_iAlarmLevel==3)
                bWrite = true;
            if(m_iAlarmDbLevel==3)
                bAlarmDb = true;
            break;
        default:
            break;                        
    }

    if(bWrite==false)
        return ;
    
    if(!strlen(m_sAlarmFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sAlarmFileName,"%s/Log_alarm.log",m_sLog_HOME);
    }
    
    if(Log::m_iProcessId==-1)
    {
        if((pt = getenv ("PROCESS_ID")) == NULL) 
            Log::m_iProcessId = -1;
        else
            Log::m_iProcessId = atoi (pt);
    }
    
    Date dt;
//    sprintf(sErrid,"%03d%02d%04d%",m_iModleId,Class_id,ErrId);
    sprintf(sErrid,"%d",ErrId);

    sprintf (sTemp, "[%s]: [ALARM][ALARM_TYPE:%d]--ProcId:[%d] ModuleId:[%d] File:[%s] LineNo:[%d] function:[%s] ErrId:[%s] ErrNo:[%d]",
                dt.toString("yyyy-mm-dd hh:mi:ss"),log_level,Log::m_iProcessId,m_iModleId,file,line,function,sErrid,err);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sForm, 9999, format, ap);
    va_end(ap);
    sprintf(sLine,"%s[Msg]%s\n",sTemp,sForm);
    if(m_bPrintf)
        printf ("%s", sLine);
    if(Class_id==MBC_CLASS_Fatal||Class_id==MBC_CLASS_Memory||Class_id==MBC_CLASS_Database)
        bottomAlarmLog(sLine);
    else
       // LOGG(OPERATIONLOG,sLine);
    if((fd=open (m_sAlarmFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        if(m_bPrintf)
            printf ("[ERROR]: Log::alarmlog() open %s error\n", m_sAlarmFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            char altemp[200]={0};
			if(m_bPrintf)
            	printf("[ALARM]: Log::alarmlog() 磁盘空间不足!\n");
            sprintf(altemp,"[ALARM]:ErrId:[%s] open %s error 磁盘空间不足!\n",sErrid,m_sAlarmFileName);
           // m_pologg->LogGroupWriteFile(OPERATIONLOG,altemp);
            if(bAlarmDb)
                AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
            return;
        }
        ErrFileWrite(sLine,m_sAlarmFileName,0);
        if(bAlarmDb)
            AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
        return;
    }
    sRet = write (fd, sLine, strlen (sLine));
    if(sRet<0)
    {
        if(m_bPrintf)
            printf ("[ERROR]: Log::alarmlog() write %s error\n", m_sAlarmFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            char altemp[200]={0};
			if(m_bPrintf)
            	printf("[ALARM]: Log::alarmlog() 磁盘空间不足!\n");
            sprintf(altemp,"[ALARM]:ErrId:[%s] open %s error 磁盘空间不足!\n",sErrid,m_sAlarmFileName);
            //m_pologg->LogGroupWriteFile(OPERATIONLOG,altemp);
            if(bAlarmDb)
                AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
            return;
        }
        ErrFileWrite(sLine,m_sAlarmFileName,0);
        if(bAlarmDb)
            AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
        return;
    }
    close (fd);
    if(bAlarmDb)
        AlarmLogDb(log_level,sErrid,err,line,file,function,sForm);
}

void Log::paramlog(char const * format,...)
{
    char *p, sTemp[10000], sLine[10000];
    int fd,seq=0;
    int cyccnt=ERRCCNT;
    ssize_t  sRet;
    
    if(!strlen(m_sParamFileName))
    {
        if(strlen(m_sLog_HOME)==0)
        {
            char *p=NULL;
            if ((p=getenv ("BILLDIR")) == NULL)
		        sprintf (m_sLog_HOME, "/opt/opthb/log");
	        else 
		        sprintf (m_sLog_HOME, "%s/log", p);
        }
        sprintf(m_sParamFileName,"%s/Log_param.log",m_sLog_HOME);
    }

    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);
    printf ("%s", sLine);
    if((fd=open (m_sParamFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        printf ("[ERROR]: Log::paramlog() open %s error\n", m_sParamFileName);
        if(errno==ENOSPC)
        {//磁盘空间不足
            ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sParamFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWrite(sLine,m_sParamFileName,seq))
                return;
            seq++;
        }        
        return;
    }
    sRet = write (fd, sLine, strlen (sLine));
    if(sRet<0)
    {
        if(errno==ENOSPC)
        {//磁盘空间不足
            ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return;
        }
        ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sParamFileName);
        while(cyccnt>0)
        {//写文件失败，另起一个文件写
            cyccnt--;
            if(ErrFileWrite(sLine,m_sParamFileName,seq))
                return;
            seq++;
        }
    }
    close (fd);
}

void Log::LoggLog(char const * format,...)
{
    char *p, sTemp[10000], sLine[10000];
    int fd,seq=0;
    int cyccnt=ERRCCNT;
    char sLoggName[256];
    struct stat statbuf;
    
    memset(sLoggName,0,sizeof(sLoggName));
    if(strlen(m_sLog_HOME)==0)
    {
        char *p=NULL;
        if ((p=getenv ("BILLDIR")) == NULL)
	        sprintf (m_sLog_HOME, "/opt/opthb/log");
	    else 
	        sprintf (m_sLog_HOME, "%s/log", p);
    }
    sprintf(sLoggName,"%s/Log_g_alarm.log",m_sLog_HOME);

    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);
    if(lstat(sLoggName, &statbuf)<0)
        return;
 	if (statbuf.st_size >= BALARMMAXSIZE)
 	{
        if((fd=open (sLoggName, O_RDWR|O_CREAT|O_TRUNC, 0770)) < 0)
            return;
 	}
    else
    {
        if((fd=open (sLoggName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
            return;
    }
    write (fd, sLine, strlen (sLine));
    close (fd);
}

void Log::bottomAlarmLog(char *pData)
{
    char *p;
    int fd,seq=0;
    char sLoggName[256];
    struct stat statbuf;

    memset(sLoggName,0,sizeof(sLoggName));
    if(strlen(m_sLog_HOME)==0)
    {
        char *p=NULL;
        if ((p=getenv ("BILLDIR")) == NULL)
	        sprintf (m_sLog_HOME, "/opt/opthb/log");
	    else 
	        sprintf (m_sLog_HOME, "%s/log", p);
    }
    sprintf(sLoggName,"%s/Log_bottom_alarm.log",m_sLog_HOME);

    if(lstat(sLoggName, &statbuf)<0)
        return;
 	if (statbuf.st_size >= BALARMMAXSIZE)
 	{
        if((fd=open (sLoggName, O_RDWR|O_CREAT|O_TRUNC, 0770)) < 0)
            return;
 	}
    else
    {
        if((fd=open (sLoggName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
            return;
    }
    write (fd, pData, strlen (pData));
    close (fd);
}

bool Log::DeleteOldLog(char *pFile)
{
    char scmd[600]={0};

    if(strlen(pFile)==0)
        return false;

     fstream _file;
     _file.open(pFile,ios::in);
     if(_file)
     {
        sprintf(scmd,"rm %s",pFile);
        system(scmd);        
     }
    return true;
}
bool Log::CutLogFile()
{
    char *p=0,*pt=0;
    char tempHome[500]={0},scmd[600]={0};
    
    m_pofstream->close();	
	m_pofstream->clear();
    memset(m_sInfoPointName,0,sizeof(m_sInfoPointName));
    m_lSeq++;
    if((p=getenv ("BILLDIR")) == NULL)
        sprintf (tempHome, "/opt/opthb");
    else 
        sprintf (tempHome, "%s", p);
    if(Log::m_iProcessId == -1)
        sprintf(m_sInfoPointName, "%s/log/process_log_%ld.log",tempHome,m_lSeq);
    else
        sprintf(m_sInfoPointName, "%s/log/process_%d_%ld.log",tempHome,Log::m_iProcessId,m_lSeq);

    printf("拆分文件，当前序列:%ld",m_lSeq);
    return true;
}

bool Log::GetInitDirFile(char *pPath,bool bFromPro)
{
    FILE *fp;
    DIR *dirp=NULL;
    struct dirent *dp=NULL;
    struct stat statbuf;
    char sName[500]={0};
    long maxSeq=0;
    bool bDelFile=false;
	
    if(pPath[strlen(pPath)-1] != '/')
    {
        strcat(pPath, "/");
    }
    dirp = opendir(pPath);
    if(dirp==NULL){
        ALARMLOG28(0,MBC_CLASS_File,103,"打开目录出错: Path[%s]",pPath);
        return false;
    }
    
    for(dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
    {
        strncpy(sName, dp->d_name, 500);
        if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
            continue;
        char TempName[500]={0};
        snprintf(TempName,500,"%s%s/",pPath,sName);
        if(lstat(TempName, &statbuf)<0)
            continue;    
        if(S_ISDIR(statbuf.st_mode)==1)
        {//是目录
            continue;
        }
        char *m = strchr(sName, '_');
        if(!m)
            continue;
        char *n = sName;
        char temp[500]={0};
        strncpy(temp,n,m-n);
        if(strcmp(temp,"process"))
            continue;
        m++;
        n = m;
        m = strchr(n, '_');
        if(!m)
            continue;
        memset(temp,0,sizeof(temp));
        strncpy(temp,n,m-n);
        if(bFromPro)
        {
            int iProId = atoi(temp);
            if(Log::m_iProcessId!=iProId)
                continue;
        }
        else
        {
            if(strcmp(temp,"log"))
                continue;                
        }
        m++;
        n = m;
        m = strchr(n, '.');
        if(!m)
            continue;
        memset(temp,0,sizeof(temp));
        strncpy(temp,n,m-n);
        long seq = atol(temp);
        if(seq < maxSeq)
            continue;
        maxSeq = seq;
    }
    if(maxSeq==0)
    {//找不到，有可能被清空，重新建立新序列
        maxSeq = 1;
    }
    if(maxSeq>=1000)
    {//从新循环序列
        maxSeq = 1;
        bDelFile = true;
    }
    m_lSeq = maxSeq;
    memset(m_sInfoPointName,0,sizeof(m_sInfoPointName));
    if(bFromPro)
        sprintf(m_sInfoPointName, "%s/process_%d_%ld.log",pPath,Log::m_iProcessId,m_lSeq);
    else
        sprintf(m_sInfoPointName, "%s/process_log_%ld.log",pPath,m_lSeq);
    closedir(dirp);
    if(bDelFile)
        DeleteOldLog(m_sInfoPointName);
    return true;    
}

bool Log::CheckFile()
{
    char *p=0,*pt=0;
    char tempHome[500]={0},tempPath[500]={0};

    if(!m_pofstream)
        m_pofstream = new ofstream;
    if(strlen(m_sInfoPointName)==0)
    {//首次执行        
        if((p=getenv ("BILLDIR")) == NULL)
		    sprintf (tempHome, "/opt/opthb");
	    else 
		    sprintf (tempHome, "%s", p);
        if(Log::m_iProcessId!=-1)
        {
            if((pt = getenv ("PROCESS_ID")) == NULL) 
                Log::m_iProcessId = -1;
            else
                Log::m_iProcessId = atoi (pt);
        }
        sprintf(tempPath, "%s/log",tempHome);
        if(Log::m_iProcessId==-1)
            GetInitDirFile(tempPath,false);
        else
            GetInitDirFile(tempPath,true);
    }
    m_pofstream->open(m_sInfoPointName,ios::app);
    if(!m_pofstream->good())
    {
        Log::log(0,"信息点落地打开日志文件状态出错\n");
        if(errno==ENOSPC)
        {//磁盘空间不足
            ALARMLOG28(0,MBC_CLASS_Fatal,10,"%s","磁盘空间不足!");
            return false;
        }
        ALARMLOG28(0,MBC_CLASS_File,102,"写 %s 文件失败",m_sInfoPointName);
        if(!CutLogFile())
            return false;
    }
    return true;
}

bool Log::ErrFileWrite(char *pData,char *pfileName,int iSeq)
{
    int fd;
    ssize_t  sRet;
    
    sprintf(pfileName,"%s.temp",pfileName);
    if((fd=open (pfileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        printf ("[ERROR]: Log::log() open %s error\n", pfileName);
        return false;
    }
    sRet = write (fd, pData, strlen (pData));
    close (fd);
    if(sRet==-1)
        return false;
    printf ("启用备用文件 %s \n", pfileName);
    return true;
}

bool Log::ErrFileWriteEx(char *pData,char *pfileName,int iSeq)
{
    int fd;
    ssize_t  sRet;
    char errTemp[256],temp[256];
    memset(errTemp,0,sizeof(errTemp));
    memset(temp,0,sizeof(temp));
    
    char *m = strchr(pfileName, '.');
    if(!m)
        return false;
    strncpy(temp,pfileName,m-pfileName);
    memset(pfileName,0,sizeof(pfileName));
    if(iSeq==0)
        sprintf(pfileName,"%s_temp.log",temp);
    else
        sprintf(pfileName,"%s_%d.log",temp,iSeq);
    if((fd=open (pfileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
    {
        //printf ("[ERROR]: Log::log() open %s error\n", pfileName);
        return false;
    }
    sRet = write (fd, pData, strlen (pData));
    close (fd);
    if(sRet==-1)
        return false;
    //printf ("启用备用文件 %s \n", pfileName);
    return true;
}

bool Log::CommitLogFile()
{
    struct stat statbuf;
    bool bWrite = false;
    vector<string>::iterator iter;

    if(m_iMaxLogFileSize==0 || m_iMaxLogFileSize==-1)
    {//如果没有配置，默认值100
        m_iMaxLogFileSize = 100;
    }
    
    if(m_vLogInfo.size()==0)
        return true;

    if(!CheckFile())
        return false;
    
    for(iter=m_vLogInfo.begin();iter!=m_vLogInfo.end();++iter)
    {
        if(lstat(m_sInfoPointName,&statbuf)<0)
        {
            Log::log(0,"信息点落地日志文件状态出错\n");
            return false;             
        }
        if(statbuf.st_size >= m_iMaxLogFileSize * 1024 * 1024)
        {
            if(!CutLogFile())
            {
                Log::log(0,"信息点落地切换日志文件出错\n");
                return false;
            }
            if(!CheckFile())
                return false;
        }
        *m_pofstream<<*iter;
        if(bWrite==false)
            bWrite = true;
    }
    m_pofstream->close();	
    m_pofstream->clear();
    m_vLogInfo.clear();

    return true;    
}


bool Log::AlarmLogDb(int log_level,char* sErrid,int err,int line,char const *file,char const *function,char const *sForm)
{
    char sql[2000];
/*
    memset(sql,0,sizeof(sql));
	DEFINE_QUERY (qry);
    try
	{
        sprintf(sql,"insert into B_ALARM_LOG (LOG_TIME,PROCESS_ID,MODULE_ID,LOG_LEVEL,ERROR_ID,ERR_NO,FILE_NAME,FILE_LINE,LOG_FUNCTION,MSG) "
    				 "values(sysdate,%d,%d,%d,'%s',%d,'%s',%d,'%s','%s')",Log::m_iProcessId,m_iModleId,log_level,sErrid,err,file,line,function,sForm);
        qry.setSQL(sql);
        qry.execute();
        qry.commit();
        qry.close();
    }
    catch(TOCIException & e)
	{
    	printf("数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
	}*/   
    return true;
}

void Log::setPrintFlag(bool flag)
{
	 m_bPrintf = flag;
}
//##ModelId=4270E22C021C
int Log::m_iLogLevel = -1;

int Log::m_iInfoLevel = -1;

int Log::m_iAlarmLevel = -1;

int Log::m_iAlarmDbLevel = -1;

int Log::m_iProcessId = -1;

int Log::m_iMaxLogFileSize = -1;

int Log::m_interval = 0;

int Log::m_iMode = 0;

int Log::m_iModleId = 0;

//##ModelId=4270E89302D7
char Log::m_sLogFileName[256];

char Log::m_sInfoPointName[256];

char Log::m_sAlarmFileName[256];

char Log::m_sParamFileName[256];

char Log::m_sLog_HOME[80];

//##ModelId=4270E89F00D6
FILE *Log::m_pLogFile = NULL;

long Log::m_lSeq = 0;
bool Log::m_bPrintf = true;

vector<string> Log::m_vLogInfo;

ofstream * Log::m_pofstream = 0;

ThreeLogGroupMgr* Log::m_pologg = 0;

