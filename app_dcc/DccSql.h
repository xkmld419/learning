#ifndef _DCC_SQL_H_
#define _DCC_SQL_H_

#include "TimesTenAccess.h"

class DccSql {

public:
    
    DccSql(): m_poTTConn(0), m_poQrySession(0) {}
    
    ~DccSql();
    
    int init(ABMException &oExp);
	int ccrInsert(ABMException &oExp);
    
    TimesTenCMD *m_poQrySession;
    
private:
    TimesTenConn *m_poTTConn;
    
    static char m_sConnTns[256];
};
    
    


#endif