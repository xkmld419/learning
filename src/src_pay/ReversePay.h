#ifndef __REVERSE_PAY_H_INCLUDED_
#define __REVERSE_PAY_H_INCLUDED_

#include "dcc_ra.h"
#include "abmpaycmd.h"
#include "PaymentInfo.h"
#include "DccOperation.h"

class ABMException;
class PaymentInfo;
class DccOperation;

/*
 * Ö§¸¶³åÕý
 *
 * 2011.5.16
 */

class ReversePay
{
	public:
		ReversePay();
		~ReversePay();	
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pCCA);
		int _deal(ReversePaySvc *pSvcIn);
		
		int reversePay(ReversePaySvc &oRvsInfo);
	private:
			
		ReversePayCCR *m_poUnpack;
		ReversePayCCA *m_poPack;	
		
		PaymentInfo *m_paymentInfo;
		DccOperation *m_dccOperation;
};


#endif /*end __REVERSE_PAY_H_INCLUDED_ */
