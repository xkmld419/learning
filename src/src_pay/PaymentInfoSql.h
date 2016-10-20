#ifndef __PAYMENT_INFO_SQL_H_INCLUDED_
#define __PAYMENT_INFO_SQL_H_INCLUDED_

#include "TTTns.h"
#include "abmpaycmd.h"
#include "LogV2.h"
#include "PublicConndb.h"

class ABMException;

class PaymentInfoSql : public TTTns
{

public:

	PaymentInfoSql();
	~PaymentInfoSql();
	//密码校验
	int ComparePasswd(AbmPayCond *&pAbmPayCond,PaymentInfoALLCCR &pPaymentInfoCCR,bool &bState);
	//查询序列
	int QueryPaySeq(StructPaymentSeqInfo &sStructPaymentSeqInfo,bool &bState);
	//记录业务表
	int InsertPaymentInfo(PaymentInfoALLCCR &pPaymentInfoCCR,StructPaymentSeqInfo  &sStructPaymentSeqInfo);
	//查询账本表
	int QueryAcctBalanceInfo(PaymentInfoALLCCR &pPaymentInfoCCR,vector<AcctBalanceInfo *>&vAcctBalanceInfo);

	//查询余额来源表 
	int QueryBalanceSourceInfo(long  &m_lAcctBalanceID,vector<BalanceSourceInfo *>&vBalanceSourceInfo);
	//记录支出表
	int InsertBalancePayout(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo,const string &m_sOperType);
	//更新账本表
	int updateAcctBalance(StructPaymentSeqInfo &sStructPaymentSeqInfo,AcctBalanceInfo *&pAcctBalanceInfo);
	//更新余额来源表
	int updateBalanceSource(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo);
	//更新来源支出关系表
	int InsertBalanceSourcePayoutRela(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo);
	// 查询支出表
	int qryPayoutInfo(long lOldRequestId,vector<BalancePayoutInfo *>&vBalancePayOut);
	// 查询余额来源信息
	int qryBalanceSource(long lOptId,BalanceSourceInfo *&pBalanceSourceInfo);
	// 更新余额支出记录状态
	int updateBalancePayout(long lOperPayoutId);
	int updateBalanceSourcePayoutRela(long lOperPayoutId);
	//查询序列
	int QueryServAcctSeq(ServAcctInfoSeq &oServAcctInfoSeq,bool &bState);
	//记录serv
	int InsertServInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne);
	//记录acct
	int InsertAcctInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne);
	//记录serv_acct
	int InsertServAcctInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne);
	//记录acct_balance
	int InsertAcctBalance(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne);
	//记录balance_source
	int InsertBalanceSource(DepositServCondOne *&pInOne,StructPaymentSeqInfo &sStructPaymentSeqInfo);
	//查询acct_balance
	int QueryAcctBalanceSimple(DepositServCondOne *&pInOne,AcctBalanceInfo *&pAcctBalanceInfo);
	//记录user_info_add
	int InsertUserInfoAdd(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne);
	
	//查询acct_balance
	int qryAcctBalance(long &lAcctBalanceId,AcctBalanceInfo *&pAcctBalanceInfo);
	// 记录余额来源 - 划拨
	int insertBalanceSource(BalanceSourceInfo &balanceSourceInfo);
	//生成用户支付密码
	int InsertAttUserInfo(ServAcctInfoSeq &oServAcctInfoSeq,bool &bState);
	//记录用户区号
	int InsertAppUserInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq);
	// 更改业务流水状态 - 划拨冲正
    	int updateOperation(long lOperPayoutId);
    	// 余额来源查询 - 划拨冲正
    	int qryBalanceSource(long lOptId,vector<BalanceSourceInfo *>&vBalanceSource);
    	// 更新余额帐本记录 - 划拨冲正更新
    	int updateAcctBalance(long &lAcctBalanceId,long &lPaymentAmount);
	//查询业务记录
	int QryPaymentInfo(long &lPaymentID,PaymentInfoALLCCR &pPaymentInfoCCR);
	int QueryOptID(char *m_sForeignID,long &lPaymentID);
    
    	int qryOptidByForeignId(char *sForeignId,long &lOptId);
    	
    	int qryOptInfoByForeignId(char *sForeignId,long &lOptId,long &lServId);
};

#endif/*__PAYMENT_INFO_SQL_H_INCLUDED_*/

