/*VER: 1*/ 
#ifndef SEQ_MGR_H
#define SEQ_MGR_H

#include "Environment.h"

class SeqMgrExt
{
public:
    SeqMgrExt(char const *sSeqName, int iStep) {
        strcpy (m_sSeqName, sSeqName);
        m_iStep = iStep;
        m_iCursor = iStep;
        m_lBaseValue = 0;
    }

    long getNextVal() {
        if (m_iCursor == m_iStep) {
            DEFINE_QUERY (qry);
            char sSQL[512];

		    sprintf (sSQL, "select %s.nextval from dual", m_sSeqName);
		    qry.setSQL (sSQL); qry.open (); qry.next ();

            m_lBaseValue = qry.field (0).asLong ();

            qry.close ();

            m_iCursor = 0;
        }

        long bRet = m_lBaseValue + m_iCursor;
        m_iCursor++;

        return bRet;
    }

private:
    char m_sSeqName[48];
    int  m_iStep;
    int  m_iCursor;
    long m_lBaseValue;
  
};
#endif
