#ifndef __REVERSE_DEPOSIT_INFOR_H_INCLUDED_
#define __REVERSE_DEPOSIT_INFOR_H_INCLUDED_

#include "abmpaycmd.h"
#include "PaymentInfoSql.h"
#include "AllocateBalanceSql.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
/*
*  充值冲正操作类
*/

class ReverseDepositInfo
{

public:

    ReverseDepositInfo();
    ~ReverseDepositInfo();
    int deal(ABMCCR *pCCR, ABMCCA *pCCA);
    int _deal(ReverseDeposit *&pInOne);
    int DoReverseDeposit(ReverseDeposit *&pInOne,ServAcctInfo &oServAcct);

private:

    PaymentInfoSql *m_poSql;
    
    ReverseDepositInfoCCR *m_poUnpack;
    ReverseDepositInfoCCA *m_poPack;

    AllocateBalanceSql  *pAllocateBalanceSql;
    DccOperation *m_dccOperation;
    
};

#endif/*__REVERSE_DEPOSIT_INFOR_H_INCLUDED_*/



