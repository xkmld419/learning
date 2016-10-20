#ifndef __DEPOSIT_INFOR_H_INCLUDED_
#define __DEPOSIT_INFOR_H_INCLUDED_

#include "abmpaycmd.h"
#include "PaymentInfoSql.h"
#include "AllocateBalanceSql.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
/*
*  充值开户操作类
*/

class DepositInfo
{

public:

    DepositInfo();
    ~DepositInfo();
    int deal(ABMCCR *pCCR, ABMCCA *pCCA);
    int _deal(DepositServCondOne *&pInOne,vector<DepositServCondTwo *>&vInTwo);
    
    // 划拨充值
    int _dealIn(DepositServCondOne *&pInOne,vector<DepositServCondTwo *>&vInTwo);
    //资料开户
    int CreateServAcctInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq);
    //资料存在,
    int UpdateAcctInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq);

private:

    PaymentInfoSql *m_poSql;
    
    DepositInfoCCR *m_poUnpack;
    DepositInfoCCA *m_poPack;

    AllocateBalanceSql  *pAllocateBalanceSql;
    DccOperation *m_dccOperation;
    
};

#endif/*__DEPOSIT_INFOR_H_INCLUDED_*/


