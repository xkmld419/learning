#ifndef __Allocate_BALANCE_H_INCLUDED_
#define __Allocate_BALANCE_H_INCLUDED_

#include "dcc_ra.h"

#include "abmpaycmd.h"
#include "PaymentInfo.h"
#include "DccOperation.h"

class ABMException;
class AllocateBalanceSql;
class PaymentInfo;
class DccOperation;

/*
 * 支付帐户余额划拨操作类 - 被动划出
 *
 * 过户、销户时的余额被动划出
 * 
 * 2011.5.5 
 */
class AllocateBalance
{
	public:
		AllocateBalance();
		~AllocateBalance();	
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pCCA);
		int _deal(R_AllocateBalanceSvc *pSvcIn,vector<R_RequestDebitAcctItem *> g_vRRequestDebitAcctItem);
	private:
		
		AllocateBalanceSql *m_poSql;
			
		AllocateBalanceCCR *m_poUnpack;
		AllocateBalanceCCA *m_poPack;	
		
		PaymentInfo *m_paymentInfo;
		
		DccOperation *m_dccOperation;
		
};

#endif /*__Allocate_BALANCE_H_INCLUDED_*/