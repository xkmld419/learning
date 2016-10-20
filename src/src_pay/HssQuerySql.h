#ifndef _HSS_TT_QUERY_INCLUDE_
#define _HSS_TT_QUERY_INCLUDE_

#include "TTTns.h"
#include "TimesTenInterface.h"
#include "ABMException.h"
#include <string>

class HssQuerySql : public TTTns
{
public:
    HssQuerySql();
    
    ~HssQuerySql();
    
    int init(ABMException &oExp);

public:
    
    TimesTenCMD *m_poBal;  //余额查询
    TimesTenCMD *m_poBal2; //按serv_id查询
    TimesTenCMD *m_poRec;  //充退查询
    TimesTenCMD *m_poPay;  //交易记录查询
    TimesTenCMD *m_poBil;  //电子账单查询
    TimesTenCMD *m_poTest;
};

#endif /*end _HSS_TT_QUERY_INCLUDE_*/