#include "ABMException.h"
#include <stdarg.h>

void ABMException::printErr(char *sLineHead, FILE *pFile)
{
    if (!sLineHead) sLineHead= "";
    if (!pFile) pFile = stdout;

    for (int i=0; i<m_oErrBuf.size(); i++) {
        fprintf (pFile, "%s%s [line %d] %s\n", sLineHead, 
                m_oErrBuf[i].m_sFileName, m_oErrBuf[i].m_iLineNo, m_oErrBuf[i].m_sMsg);
        if (pFile != stdout)
            fprintf (stdout, "%s%s [line %d] %s\n", sLineHead, 
                m_oErrBuf[i].m_sFileName, m_oErrBuf[i].m_iLineNo, m_oErrBuf[i].m_sMsg);
    }

    m_oErrBuf.clear ();
}

void ABMException::addErr(char const *sFile, int const iLine, char const *sFormat, ...) 
{
    char sBuf[4096];

    va_list ap;
    va_start(ap, sFormat);
    vsnprintf (sBuf, sizeof(sBuf)-1, sFormat, ap);
    va_end(ap);

    sBuf[4095] = 0;

    int l = strlen(sBuf);
    ExceptMsg msg;
    for (int i=0; i<l; i+=(sizeof(msg.m_sMsg)-1)) {
        memset (&msg, 0, sizeof(msg));

        strncpy (msg.m_sFileName, sFile, sizeof(msg.m_sFileName)-1);
        msg.m_sFileName[sizeof(msg.m_sFileName)-1] = 0;

        msg.m_iLineNo = iLine;

        strncpy (msg.m_sMsg, &(sBuf[i]), sizeof(msg.m_sMsg)-1);
        msg.m_sMsg[sizeof(msg.m_sMsg)-1] = 0;

        m_oErrBuf.push_back (msg);
    }
}

