// ActiveAllocateBalanceBiz.h 
#ifndef __ACTIVE_ALLOCATE_BALANCE_BIZ_H_INCLUDED_
#define __ACTIVE_ALLOCATE_BALANCE_BIZ_H_INCLUDED_

#include "dcc_ra.h"

#include "abmpaycmd.h"
#include "PaymentInfo.h"
#include "DccOperation.h"
#include "DepositInfo.h"

class ABMException;
class AllocateBalanceSql;
class PaymentInfo;
class DccOperation;
class DepositInfo;

extern bool g_toPlatformFlag;

/*
 * 支付帐户余额划拨类 - 主动划出
 *
 * 中心ABM接收省ABM的CCA后，从TT中查找自服务平台请求的CCR，判断
 * 1.省到全国(省ABM被动划拨)   - 中心ABM划拨充值
 * 2.全国到省(中心ABM主动划拨) - 中心ABM余额划出
 *
 * 2011.5.27
 **/
class ActiveAllocateBalanceBiz
{
	public:
		ActiveAllocateBalanceBiz();
		~ActiveAllocateBalanceBiz();	
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pCCA); // 处理接收的CCA，主动发CCA
			
		int deductBalanceIn(ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo);
		int deductBalanceOut(ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo);
	private:
		
		AllocateBalanceSql *m_poSql;
			
		AllocateBalanceCCA *m_poUnpack; 		// 解包-省ABM返回的CCA包，省到全国
		ActiveAllocateBalanceCCA *m_poActiveUnpack;	// 解包-省ABM返回的CCA包，全国到省
		
		PlatformDeductBalanceCCA *m_poPack;	// 打包-返回给自服务平台的CCA包
			
		PaymentInfo *m_paymentInfo;		
		
		DccOperation *m_dccOperation;
		
		DepositInfo *m_depositInfo;
};

#endif /*__ACTIVE_ALLOCATE_BALANCE_BIZ_H_INCLUDED_*/