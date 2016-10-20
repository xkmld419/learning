#ifndef __HSS_HSSQUERY_H_INCLUDE_
#define __HSS_HSSQUERY_H_INCLUDE_
#include "HssQuerySql.h"
#include "abmcmd.h"
#include "DccOperation.h"
#include "PublicConndb.h"

class HssQuery
{

public:

    HssQuery();
    
    ~HssQuery();
    
    int init(ABMException &oExp);
    
    int deal(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);

public:

    int dQueryBalance(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);
    int _dQueryBalance(QueryBalance *poQueryMsg);
    
    int dRechargeRecQuery(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);
    int _dRechargeRecQuery(RechargeRecQuery *poQueryMsg);
    
    int dQueryPayLogByAccout(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);
    int _dQueryPayLogByAccout(QueryPayLogByAccout *poQueryMsg);
    
    int dRechargeBillQuery(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);
    int _dRechargeBillQuery(RechargeBillQuery *poQueryMsg);
        
    void dTestQuery();
    
    unsigned int compute(char *str);
    
    int isNumberStr(char *str);

private:

    HssQuerySql *m_poSql;
    
    //余额查询
    ResBalanceCode m_oBalanceRes;
    HssQueryBalanceCCR  *m_poHBalCCR;
    HssQueryBalanceCCA  *m_poHBalCCA;
    
    //充退查询
    ResRecCode m_oRes;
    HssRechargeRecQueryCCR *m_poHRecCCR;
    HssRechargeRecQueryCCA *m_poHRecCCA;
    
    //交易记录查询
    ResPayCode m_oPayRes;
    HssQueryPayLogByAccoutCCR *m_poHPayCCR;
    HssQueryPayLogByAccoutCCA *m_poHPayCCA;
    
    //电子账单查询
    ResBilCode m_oBilRes;
    HssRechargeBillQueryCCR *m_poHBilCCR;
    HssRechargeBillQueryCCA *m_poHBilCCA;
    DccOperation m_oDccOperation;

    int m_iCount;

};

#endif /*end __HSS_HSSQUERY_H_INCLUDE_*/