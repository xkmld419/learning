#ifndef __ABM_SRV_SUPPORT_H_INCLUDED_
#define __ABM_SRV_SUPPORT_H_INCLUDED_

class ABMCCR;
class ABMCCA;
class ABMException;
class PaymentInfo;//支付类
class PublicConndb;//共享数据连接类
class AllocateBalance;//余额划拨类
class DepositInfo;//充值开户类
class ReverseDeductBalance;
class QryPasswordSend;//二次密码认证下发类
class QryServInfosyn;//销户、过户、换号资料同步
class HssQuery;
class QueryAll;
class ActiveAllocateBalance; // 主动划拨类-CCR
class ActiveAllocateBalanceBiz; // 主动划拨类-CCA
class ReverseDepositInfo;//充值冲正
class ReversePay; // 支付冲正类
class PasswdIdentify;	//密码鉴权
class PasswdIdentifyBiz; //密码鉴权
class PasswdChange;	//登陆密码修改
class PasswdChangeBiz;	//登陆密码修改
class ABMSynRcv;//开户接收CCA同步处理
class PayAbilityQuery;//支付能力状态查询
class BalanceTransQuery;//划拨规则查询
class BalanceTransReset;//划拨规则设置
class PasswordReset;//支付密码修改
class PayStatusReset;//支付状态变更
class PayRulesQuery;//支付状态查询
class PayRulesReset;//支付状态变更
class BindingAccNbrQuery;//支付状态变更

class ABMSvcSupt
{
	public:	ABMSvcSupt();	
		~ABMSvcSupt();
		int init(ABMException &oExp);	
		int deal(ABMCCR *pCCR, ABMCCA *pCCA);
	private:	
		PayAbilityQuery *m_poPayAbilityQuery;//支付能力状态查询	
		BalanceTransQuery *m_poBalanceTransQuery;//支付能力状态查询	
		BalanceTransReset *m_poBalanceTransReset;//支付能力状态查询	
		PasswordReset *m_poPasswordReset;//支付密码修改
		PayStatusReset *m_poPayStatusReset;//支付状态变更
		PayRulesQuery *m_poPayRulesQuery;//支付规则查询
		PayRulesReset *m_poPayRulesReset;//支付规则变更
		BindingAccNbrQuery *m_poBindingAccNbrQuery;//绑定号码查询
		
		PaymentInfo  *m_poPaymentInfo;//支付	
		AllocateBalance *m_poAllocateBalance;//余额划拨	
		PublicConndb *m_pPublicConndb;//公用数据库连接	
		DepositInfo  *m_poDepositInfo;//充值支付	
		ReverseDeductBalance *m_poReverseDeductBalance;	
		QryPasswordSend *m_poQrySrv;//二次密码认证下发	
		QryServInfosyn *m_poQryInfosyn;//销户、过户、换号资料同步        
		HssQuery     *m_poHssQuery;          
		QueryAll     *m_poQueryAll;        
		ActiveAllocateBalance *m_poActiveAllocateBalance; //主动划拨-CCR        
		ActiveAllocateBalanceBiz *m_poActiveAllocateBalanceBiz; //主动划拨-CCA	
		ReverseDepositInfo *m_pReverseDepositInfo;//充值冲正	
		ReversePay *m_poReversePay; // 支付冲正		
		PasswdIdentify *m_pIdentify;//密码鉴权发CCR	
		PasswdIdentifyBiz *m_pIdentifyBiz;//密码鉴权收CCA返CCA	
		PasswdChange *m_pChange;	//密码修改发CCR	
		PasswdChangeBiz *m_pChangeBiz;	//密码修改收CCA返CCA	
		ABMSynRcv *m_pSynRcv;//开户资料同步收CCA
};
#endif /*__ABM_SRV_SUPPORT_H_INCLUDED_*/
