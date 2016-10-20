#ifndef __HSS_QUERYALL_H_INCLUDE_
#define __HSS_QUERYALL_H_INCLUDE_
#include "QueryAllSql.h"
#include "abmcmd.h"

#include "abmpaycmd.h"
#include "DccOperation.h"
class DccOperation;

class QueryAll
{

public:

    QueryAll();
    
    ~QueryAll();
    
    int init(ABMException &oExp);
    
    int deal(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);

public:

    int dQueryAllCCR(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt);
	
    int _dQueryAll(QueryAllPay *poQueryMsg);

	int dQueryAllCCA(ABMCCR *,ABMCCA *);
    
    static int isNumStr(const char * str);

    unsigned int compute(char *);
    
    int _dQueryCenterABM(QueryAllPay *pIn);
	
private:
  
    struct struDccHead m_oCCRHead;

    MicroPayRlt m_oRlt;
    
    char m_sReqID[64];

    QueryAllSql *m_poSql;
	
    HssPaymentQueryCCR * m_poQueryAllCCR;

    HssResMicroPayCCA * m_poQueryAllCCA;

    HssQueryBalanceCCR * m_poQueryAllSndCCR;

    int m_iCount;
    
    DccOperation *m_dccOperation;
    
};

#endif /*end __HSS_QUERYALL_H_INCLUDE_*/
