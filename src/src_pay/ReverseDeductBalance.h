//ReverseDeductBalance.h
#ifndef __REVERSE_DEDUCT_BALANCE_H_INCLUDED_
#define __REVERSE_DEDUCT_BALANCE_H_INCLUDED_

#include "dcc_ra.h"

#include "abmpaycmd.h"
#include "PaymentInfo.h"
#include "DccOperation.h"

class ABMException;
class AllocateBalanceSql;
class PaymentInfo;
class DccOperation;

/*
 * 划拨冲正操作类
 *
 * 
 * 2011.5.17 
 */
class ReverseDeductBalance
{
	public:
		ReverseDeductBalance();
		~ReverseDeductBalance();	
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pCCA);
		int _deal(ReqReverseDeductSvc *pSvcIn);
		
		int reverseDeduct(ReqReverseDeductSvc &oRvsInfo);
	private:
		AllocateBalanceSql *m_poSql;	// 余额划拨数据库操作类
			
		ReverseDeductBalanceCCR *m_poUnpack;
		ReverseDeductBalanceCCA *m_poPack;	
		
		PaymentInfo *m_paymentInfo;
		DccOperation *m_dccOperation;
		
};

#endif /*__REVERSE_DEDUCT_BALANCE_H_INCLUDED_*/