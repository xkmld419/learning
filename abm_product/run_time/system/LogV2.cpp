#include "LogV2.h"
#include "Date.h"
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef THREE_LOG_GROUP
#include "ThreeLogGroup.h"
ThreeLogGroup *g_po3Log = NULL;
#else
LogV2 *g_po3Log = NULL;
#endif

bool init3Log()
{
    if (g_po3Log == NULL) {
        #ifdef THREE_LOG_GROUP
        if ((g_po3Log=new ThreeLogGroup) == NULL) {
        #else
        if ((g_po3Log=new LogV2) == NULL) {
        #endif
            return false;
        }                        
    }
    return true;    
}

int LogV2::m_iLogLevel = 0;

char LogV2::m_sLogName[256] = {0};
    
static FILE *g_poFp = NULL;

LogV2::LogV2()
{
    g_poFp = NULL;
}

LogV2::~LogV2()
{
    if (g_poFp != NULL)
        fclose(g_poFp);	
}

void LogV2::setFileName(const char *sName)
{
	char *p = getenv("ABM_LOG_LEVEL");
	if (p != NULL)
		m_iLogLevel = atoi(p);
	strncpy(m_sLogName, sName, sizeof(m_sLogName)-1);
	g_poFp = fopen(m_sLogName, "a+");	
	if (!g_poFp) {
	    perror("fopen");
	    throw;
	}    
}

void LogV2::log(const char *sFile, const int iLine, int iLogLevel, char const * sFormt, ...)
{
    if (g_poFp == NULL)
        return;
	char sTemp[8192];
	char sText[8192];

	if (iLogLevel > m_iLogLevel) 
		return;

	Date dt;
	snprintf(sTemp, sizeof(sTemp), "[%s][LogLevel_%d]: %s at %s:%d\n", dt.toString("yyyy-mm-dd hh:mi:ss"), \
		iLogLevel, sFormt, sFile, iLine);

	va_list ap;
	va_start(ap, sFormt);
	vsnprintf(sText, sizeof(sText), sTemp, ap);
	va_end(ap);
	
	fwrite(sText, strlen(sText), 1, g_poFp);
    fflush(g_poFp);
	return;
}

void LogV2::logExcep(ABMException &oExp)
{
    if (g_poFp != NULL) {
        oExp.printErr(NULL, g_poFp);
        fflush(g_poFp);
    }    
	return;	
}


