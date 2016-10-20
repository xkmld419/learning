#include "DccLog.h"

char DccLog::m_sLogFileName[256] = {0};
int  DccLog::m_nFd = -1;
Date DccLog::m_dtDate;

DccLog::DccLog()
{
}

void DccLog::setFileName(char *sPath, char *sPrefixName, int iProcID)
{
	Date dt;
	memset(m_sLogFileName, 0, sizeof(m_sLogFileName));
	if (strlen(sPath) > 0)
	{
		sprintf(m_sLogFileName, "%s", sPath);
		if (m_sLogFileName[strlen(m_sLogFileName)-1] != '/')
			strcat(m_sLogFileName, "/");
	}
	else
		sprintf(m_sLogFileName, "./");

	char sTemp[81] = {0};
	if (strlen(sPrefixName) > 0)
		sprintf(sTemp, "%s_%d_%s.log", sPrefixName, iProcID, dt.toString("yyyymmdd"));
	else
		sprintf(sTemp, "%d_%s.log", iProcID, dt.toString("yyyymmdd"));
	strcat(m_sLogFileName, sTemp);

	m_dtDate = dt;

	if (m_nFd != -1)
		close();
}

void DccLog::log(char const *format, ...)
{
    if (!strlen(m_sLogFileName))
        strcpy (m_sLogFileName,"./Log.log");
	else
	{
		Date dt;
		if (dt.diffStrictDay(m_dtDate) >= 1)     // 跨天，修改文件名
		{
			char *pos = strrchr(m_sLogFileName, '_');
			pos++;
			char *sDay = dt.toString("yyyymmdd");
			for (int i=0; i<8; ++i)
				pos[i] = sDay[i];
			close();
		}
	}

	if (m_nFd == -1)
	{
		if ((m_nFd=open (m_sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) 
		{
			printf ("[ERROR]: Log::log() open %s error\n", m_sLogFileName);
			return;
		}
	}

    Date dt;
	char sTemp[61441] = {0};
	char sLine[61441] = {0};
    sprintf (sTemp, "[%s]: \n%s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 61440, sTemp, ap);
    va_end(ap);
	
    write (m_nFd, sLine, strlen (sLine));	
}

void DccLog::close()
{
	::close(m_nFd);
	m_nFd = -1;
}

