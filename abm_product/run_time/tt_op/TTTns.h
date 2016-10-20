#ifndef __TT_TNS_H_INCLUDED_
#define __TT_TNS_H_INCLUDED_

#include "TimesTenAccess.h"

class TTTns
{

public:

    virtual ~TTTns() {};

    void commit() {
        return m_poTTConn->commit();
    }

    void rollback() {
        return m_poTTConn->rollback();
    }
    
    static TimesTenConn *m_poTTConn;

    static char m_sConnTns[256];
};


#endif/*__TT_TNS_H_INCLUDED_*/
