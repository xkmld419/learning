#ifndef __PAYMENT_INFOR_H_INCLUDED_
#define __PAYMENT_INFOR_H_INCLUDED_

#include "abmpaycmd.h"
#include "PaymentInfoSql.h"
#include "AllocateBalanceSql.h"
#include "DccOperation.h"
#include "DccOperationSql.h"


class ABMException;

/*
*   Ö§¸¶²Ù×÷Àà
*/

class PaymentInfo
{

public:

    PaymentInfo();
    ~PaymentInfo();
    //int init(ABMException &oExp);
    int deal(ABMCCR *pCCR, ABMCCA *pCCA);
    int _deal(vector<AbmPayCond *>&pIn);
    int RecodePaymentInfo(PaymentInfoALLCCR &pPaymentInfoCCR,PaymentInfoALLCCA &pPaymentInfoCCA);
    //int ReversePayoutInfo(long lOldRequestId,ReversePaySvc &oRvsInfo,vector<BalancePayoutInfo*> &vBalancePayOut);
    int ReversePayoutInfo(char *sOldRequestId,ReversePaySvc &oRvsInfo,vector<BalancePayoutInfo*> &vBalancePayOut);

    int DeductBalanceInfo(PaymentInfoALLCCR &pPaymentInfoCCR,PaymentInfoALLCCA &pPaymentInfoCCA);
    
    int ReverseDeductBalance(char* sOldRequestId,ReqReverseDeductSvc &oRvsInfo);

private:

    PaymentInfoSql *m_poSql;

    AbmPaymentCCR *m_poUnpack;

    AbmPaymentCCA *m_poPack;
    AllocateBalanceSql  *pAllocateBalanceSql;
    DccOperation *m_dccOperation;
    
};

#endif/*__PAYMENT_INFOR_H_INCLUDED_*/

