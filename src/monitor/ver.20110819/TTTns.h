#ifndef __TT_TNS_H_INCLUDED_
#define __TT_TNS_H_INCLUDED_

#include "TimesTenAccess.h"

class TTTns
{

public:

    virtual ~TTTns() {};

    static TimesTenConn *m_poTTConn;

    static char m_sConnTns[256];
};

//TimesTenConn* TTTns::m_poTTConn = NULL;
//char TTTns::m_sConnTns[256] = "hss";

#endif/*__TT_TNS_H_INCLUDED_*/
