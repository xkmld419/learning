// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.


#include "Log.h"

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

void Log::log(int log_level, const char *format, ...)
{
        char *p, sTemp[256], sLine[500];
	int fd;

	/* LOG_LEVEL */
	if (m_iLogLevel == -1) {
		if ((p=getenv("LOG_LEVEL")) == NULL)
			m_iLogLevel = 0;
		else
			m_iLogLevel = atoi (p);
	}

        if (log_level > m_iLogLevel)
                return;

	if ((fd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) {
		printf ("[ERROR]: Log::log() open %s error\n", m_sLogFileName);
                return;
	}

        Date dt;
        sprintf (sTemp, "[%s]: %s\n", dt.toString((char *)"yyyy-mm-dd hh:mi:ss"), format);

        va_list ap;
        va_start(ap, format);
		vsnprintf (sLine, 500, sTemp, ap);
        va_end(ap);

	printf ("%s", sLine);
	write (fd, sLine, strlen (sLine));

	close (fd);
}

//##ModelId=4270E983020A
void Log::log(int log_level, char *format, ...)
{
        char *p, sTemp[256], sLine[500];
	int fd;

	/* LOG_LEVEL */
	if (m_iLogLevel == -1) {
		if ((p=getenv("LOG_LEVEL")) == NULL)
			m_iLogLevel = 0;
		else
			m_iLogLevel = atoi (p);
	}

        if (log_level > m_iLogLevel)
                return;

	if ((fd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) {
		printf ("[ERROR]: Log::log() open %s error\n", m_sLogFileName);
                return;
	}

        Date dt;
        sprintf (sTemp, "[%s]: %s\n", dt.toString((char *)"yyyy-mm-dd hh:mi:ss"), format);

        va_list ap;
        va_start(ap, format);
		vsnprintf (sLine, 500, sTemp, ap);
        va_end(ap);

	printf ("%s", sLine);
	write (fd, sLine, strlen (sLine));

	close (fd);
}

//##ModelId=4270E22C021C
int Log::m_iLogLevel = -1;

//##ModelId=4270E89302D7
char Log::m_sLogFileName[256];

//##ModelId=4270E89F00D6
FILE *Log::m_pLogFile = NULL;

