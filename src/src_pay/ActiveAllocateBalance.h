#ifndef __ACTIVE_Allocate_BALANCE_H_INCLUDED_
#define __ACTIVE_Allocate_BALANCE_H_INCLUDED_

#include "dcc_ra.h"

#include "abmpaycmd.h"
#include "PaymentInfo.h"
#include "DccOperation.h"

class ABMException;
class AllocateBalanceSql;
class PaymentInfo;
class DccOperation;

/*
 * 支付帐户余额划拨类 - 主动划出
 *
 * 中心ABM接收自服务平台CCR后判断：
 * 1.省到全国(省ABM被动划拨)   - 中心ABM划拨充值
 * 2.全国到省(中心ABM主动划拨) - 中心ABM余额划出
 *
 * 2011.5.16
 **/
class ActiveAllocateBalance
{
	public:
		ActiveAllocateBalance();
		~ActiveAllocateBalance();	
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pCCA); // 仅处理收的CCR
		int _deal(ReqPlatformSvc *pSvcIn,vector<ReqPlatformPayInfo *> g_vReqPlatformPayInfo);
		
		int deductBalanceIn(ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *g_itr2);
		int deductBalanceOut(ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *g_itr2);
	private:
		
		AllocateBalanceSql *m_poSql;
			
		PlatformDeductBalanceCCR *m_poUnpack; // 解包-自服务平台的CCR包
		AllocateBalanceCCR *m_poPack;         // 打包-请求省ABM划拨的CCR包	
		ActiveAllocateBalanceCCR *m_poActivePack; // 打包-请求主动划出的CCR包
		
		PaymentInfo *m_paymentInfo;		
		
		DccOperation *m_dccOperation;
};
 

#endif /*__ACTIVE_Allocate_BALANCE_H_INCLUDED_*/