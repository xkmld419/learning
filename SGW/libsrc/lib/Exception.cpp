// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Exception.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Exception::Exception(int code, int err, int line, char const * file)
{
	m_iCode = code;
	m_iErrNo = err;
	m_iLineNo = line;
	memset (m_sFile, 0, sizeof(m_sFile));
	strncpy (m_sFile, file, sizeof(m_sFile)-1);
}
//##ModelId=421DCC2F01DE
Exception::Exception(int code, int err, int line, char *file)
{
	m_iCode = code;
	m_iErrNo = err;
	m_iLineNo = line;
	memset (m_sFile, 0, sizeof(m_sFile));
	strncpy (m_sFile, file, sizeof(m_sFile)-1);
}



//##ModelId=421DCC5E0308
char *Exception::descript()
{
	FILE *fp;
	char *p, sFileName[256], sTemp[1024], sCause[256], sAction[256];

	sCause[0] = 0;
	sAction[0] = 0;

	if ((p=getenv ("APP_HOME")) == NULL) 
		strcpy (sFileName, "/opt/optsr/SR_HOME/libsrc/lib/MBC.h");
	else
		sprintf (sFileName, "%s/libsrc/lib/MBC.h", p);

	if ((fp = fopen (sFileName, "r")) != NULL) {

		while (fgets(sTemp, 1024, fp) != NULL) {
			if (strncmp (sTemp, "//@@", 4) != 0)
				continue;

			p = strtok (sTemp, " \t\n");
			p = strtok (NULL, "\t\n");
			if (atoi (p) != m_iCode)
				continue;

			p = strtok (NULL, "\t\n");
			strncpy (sCause, p, sizeof(sCause)-1);
			p = strtok (NULL, "\t\n");
			strncpy (sAction, p, sizeof(sAction)-1);
			break;
		
		}
		fclose (fp);
	}

	sprintf (m_sBuf, "%s(%d) %d-%d\t  *Cause: %s\t  *Action: %s", 
			m_sFile, m_iLineNo, m_iCode, m_iErrNo, sCause, sAction);

	return m_sBuf;
}

SRException::SRException(int code, int err, int line, char const *file,char const *format, ...)
{
	m_iCode = code;
	m_iErrNo = err;
	m_iLineNo = line;
	memset (m_sFile, 0, sizeof(m_sFile));
	strncpy (m_sFile, file, sizeof(m_sFile)-1);	

  memset(m_sBuf,0,sizeof(m_sBuf));
  va_list ap;
  va_start(ap, format);
  vsnprintf (m_sBuf, 1023, format, ap);
  va_end(ap);	
    
    
}   
