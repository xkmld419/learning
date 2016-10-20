#ifndef _HSS_TT_QUERYALL_INCLUDE_
#define _HSS_TT_QUERYALL_INCLUDE_

#include "TTTns.h"
#include "TimesTenInterface.h"
#include "ABMException.h"
#include <string>

class QueryAllSql :public TTTns
{
public:
    QueryAllSql();
    
    ~QueryAllSql();
    
    int init(ABMException &oExp);

    int GetSeq(long &t);

public:
    
    TimesTenCMD *m_poQueryAll;
    
    TimesTenCMD *m_poInsertDcc;
    
    TimesTenCMD *m_poInsertMsg;
    
    TimesTenCMD *m_poGetDcc;
    
    TimesTenCMD *m_poGetMsg;
    
    TimesTenCMD *m_poGetSeq;
};

#endif /*end _HSS_TT_QUERYALL_INCLUDE_*/
