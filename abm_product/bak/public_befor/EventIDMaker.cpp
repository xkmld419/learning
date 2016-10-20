/*VER: 1*/ 
#include "EventIDMaker.h"
#include "Environment.h"

long EventIDMaker::getNextEventID()
{
        if (!m_lFlag)
                getNextEventSeq();
        else
                m_lCurEventID++;

        m_lFlag++;
        m_lFlag = m_lFlag%EVENT_ID_INCREMENT;

        return m_lCurEventID;
}

long EventIDMaker::getNextEventSeq()
{
        DEFINE_QUERY (qry);

        qry.setSQL("select seq_event_id.nextval event_id from dual");
        qry.open();

        if (!qry.next() ) THROW(EVENT_ID_INCREMENT);

        m_lCurEventID = qry.field(0).asLong();

        qry.close();

        return m_lCurEventID;
}

long EventIDMaker::m_lFlag = 0;
long EventIDMaker::m_lCurEventID = 0;

