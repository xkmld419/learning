#include "SGWException.h"
#include "Date.h"

#define  LOG_FILE_MAX_SIZE  10*1024*1024
//#define  LOG_FILE_MAX_SIZE  100

void SGWException::printErr(char *sLineHead,int _iSign)
{
	if (m_oErrBuf.size() == 0) {
		sleep(2);
		return;
	}

	FILE *pFile = NULL;
	struct stat st;

    if (!sLineHead) sLineHead= "";

	if (_iSign==1) {
s1:
		pFile = stdout;
		// modify by huff 日志改成栈模式输出
		int ii = m_oErrBuf.size()-1;

		while (ii >=0 ) {
			fprintf (pFile, "[%s]: %s%s (line %d) %s\n", m_oErrBuf[ii].m_sTime,sLineHead,
				m_oErrBuf[ii].m_sFileName, m_oErrBuf[ii].m_iLineNo, m_oErrBuf[ii].m_sMsg);
			ii--;        
		}

	} else {
		if (strlen(m_sFileName) == 0){
			goto s1;
		} 

		int fd = 0;

		if ((fd=open (m_sFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) {
			printf ("[ERROR]: Log::log() open %s error %d\n", m_sFileName,errno);
			return;
		} else {
			fstat(fd, &st);

			if (st.st_size>LOG_FILE_MAX_SIZE) {
				ftruncate(fd,0);
			} 
		}
		pFile = fopen(m_sFileName,"a+");

		if (pFile == NULL) {
			printf ("[ERROR]: Log::log() fopen %s error %d\n", m_sFileName,errno);
			return;
		} else {

			int ii =  0;

			while (ii <m_oErrBuf.size()) {
				fprintf (pFile, "[%s]: %s%s (line %d) %s\n",m_oErrBuf[ii].m_sTime,sLineHead,
					m_oErrBuf[ii].m_sFileName, m_oErrBuf[ii].m_iLineNo, m_oErrBuf[ii].m_sMsg);
				fflush(pFile);
				//int fd = fileno(pFile);
				fstat(fd, &st);
				if (st.st_size>LOG_FILE_MAX_SIZE&&ii!=m_oErrBuf.size()-1) {
					ftruncate(fd,0);
				}
				ii++;        
			}
			fclose(pFile);
		}
		close(fd);
	}
    m_oErrBuf.clear ();
	sleep(2);
}

void SGWException::addErr(char const *sFile,int const iLine, char const *sFormat, ...) 
{
    char sBuf[4096] = {0};

    va_list ap;
    va_start(ap, sFormat);
    vsnprintf (sBuf, sizeof(sBuf)-1, sFormat, ap);
    va_end(ap);

    sBuf[4095] = 0;

    int l = strlen(sBuf);
    ExceptMsg msg;
	Date dt;

    for (int i=0; i<l; i+=(sizeof(msg.m_sMsg)-1)) {
        memset (&msg, 0, sizeof(msg));

        strncpy (msg.m_sFileName, sFile, sizeof(msg.m_sFileName)-1);
        msg.m_sFileName[sizeof(msg.m_sFileName)-1] = 0;
		strncpy(msg.m_sTime,dt.toString("yyyy-mm-dd hh:mi:ss"),sizeof(msg.m_sTime)-1);
        msg.m_iLineNo = iLine;
        strncpy (msg.m_sMsg, &(sBuf[i]), sizeof(msg.m_sMsg)-1);
        msg.m_sMsg[sizeof(msg.m_sMsg)-1] = 0;

        m_oErrBuf.push_back (msg);
    }
}

