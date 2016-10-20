#ifndef __DCC_RA_H_INCLUDED_
#define __DCC_RA_H_INCLUDED_

#include "abmcmd.h"
#include "abmobject.h"

/*
#define UTF8_MAX  32
#define TEXT_MAX  256
*/
typedef unsigned int ui32;

typedef unsigned long ui64;
/*
#define ABMCMD_ALLOCATE_BALANCE  6684801   //余额被动划拨

#define ABMCMD_REVERSE_PAY  6684802   //支付冲正

#define ABMCMD_ACTIVE_ALLOCATE_BALANCE 6684803 // 主动划拨

#define ABMCMD_REVERSE_DEDUCT_BALANCE 6684804 // 划拨冲正

#define ABMCMD_PLATFORM_DEDUCT_BALANCE 6684805 // 自服务平台请求余额划拨

#define ABMCMD_PLATFORM_DEDUCT_BALANCE_BIZ 6684806 // 

*/
/*
struct ResponsStruct
{
	char m_sResID[UTF8_MAX];
	int m_iRltCode;
	int m_iAlign;
};
*/

/*=============================余额被动划出======================*/
// 用户帐户信息
struct ServAcctInfo
{
	long m_servId;			// 用户标识
	long m_acctId;			// 帐户标识
	char m_servNbr[UTF8_MAX];	// 用户号码
	long m_nbrType;		// 号码类型	
};

// 余额划拨规则配置
struct TransRule
{
	long lAcctId;
	long lDayTransFreq;	// 每天频度
	long lTransLimit;	// 每次限额
	long lMonthTransLimit;	// 每月限额
				// 自动划拨标识
				// 自动划拨阀值
				// 自动划拨金额
	char sType[UTF8_MAX];		// 限额类型，0：当天，1：当月
		 	
};

/*
// 余额划拨CCR信息 - 服务信息
struct R_AllocateBalanceSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_servNbr[UTF8_MAX];	// 用户号码
	short m_nbrType;		// 号码类型
	short m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char m_areaCode[UTF8_MAX];	// 电话区号
	ui32 m_staffId;			// 操作员ID
	char m_deductDate[32];		// 扣费时间
	int m_iDeductDate;
	int m_iTest;// 补齐
	char m_abmId[UTF8_MAX];		// 发起扣费ABM标识
};

// 余额划拨CCR信息 - 请求扣费的账目明细信息
struct R_RequestDebitAcctItem
{
	//char m_billingCycle[UTF8_MAX];	// 帐务周期YYYYMM	
	ui32 m_billingCycle;			// 帐务周期YYYYMM	
	char m_acctItemTypeId[UTF8_MAX];	// 账目类型标识	
	ui32 m_balanceUnit;			// 余额单位-分
	long m_deductAmount;			// 划拨金额
	int m_chargeType;			// 必须一次性全部扣完	
};

// 余额划拨CCA信息 - 服务信息
struct A_AllocateBalanceSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	long m_lAcctId;			// 扣费帐务标识
	ui32 m_balanceUnit;		// 余额单位-分	
	long m_deductAmount;		// 成功划拨金额
	long m_acctBalance;		// 帐户剩余金额
};

// 余额划拨CCA信息 - 响应扣费的账目明细信息
struct A_ResponseDebitAcctItem
{
	//char m_billingCycle[UTF8_MAX];	// 帐务周期YYYYMM	
	ui32 m_billingCycle;		// 帐务周期YYYYMM	
	char m_acctItemTypeId[UTF8_MAX];// 帐目类型标识	
	ui64 m_acctBalanceId;		// 扣费账本标识
	long m_deductAmount;		// 帐本划拨金额
	long m_acctBalanceAmount;	// 帐本剩余金额	
};

class AllocateBalanceCCR : public ABMTpl<__DFT_T1_,R_AllocateBalanceSvc, R_RequestDebitAcctItem, ABMCMD_ALLOCATE_BALANCE>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addAllocateBalanceSvc(R_AllocateBalanceSvc &data)
		{
			return __add2(data);
		}
		
		bool addRequestDebitAcctItem(R_RequestDebitAcctItem &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getR_AllocateBalanceSvc(vector<R_AllocateBalanceSvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getR_RequestDebitAcctItem(R_AllocateBalanceSvc *pSvc,vector<R_RequestDebitAcctItem *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class AllocateBalanceCCA : public ABMTpl<__DFT_T1_, A_AllocateBalanceSvc, A_ResponseDebitAcctItem, ABMCMD_ALLOCATE_BALANCE>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包

		bool addResBalanceSvcData(A_AllocateBalanceSvc &data)
		{
			return __add2(data);
		}
	
		bool addResDebitAcctData(A_ResponseDebitAcctItem &data)
		{
			return __add3(data);
		}
	
		// 解包

		
		int getResBalanceSvcData(vector<A_AllocateBalanceSvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getAcctBalanceData(A_AllocateBalanceSvc *pPtr, vector<A_ResponseDebitAcctItem *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};

*/

/*============================余额主动划出============================*/
/*
// CCR
struct ReqBalanceSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_destinationId[UTF8_MAX];	// 被充值用户号码
	int m_destinationIdType;	// 号码类型
	int m_destinationAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char m_areaCode[UTF8_MAX];	// 电话区号
	char m_requestTime[32];		// 请求时间
	int m_iRequestTime;		
	ui32 m_balanceUnit;		// 余额单位
	long m_rechargeAmount;		// 充值金额
	int m_blanceType;		// 余额类型
	ui32 m_test;// 补齐
};

// CCA
struct ResBalanceSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; // 错误信息
	char m_responseTime[32];	// 响应时间
	int m_iResponseTime;
	ui32 m_balanceUnit;		// 余额单位
	long m_balanceAmount;		// 划拨后省ABM帐户余额
	int m_blanceType;		// 余额类型
};

class ActiveAllocateBalanceCCR : public ABMTpl<__DFT_T1_,ReqBalanceSvc,__DFT_T3_, ABMCMD_ACTIVE_ALLOCATE_BALANCE>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addReqBalanceSvc(ReqBalanceSvc &data)
		{
			return __add2(data);
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getReqBalanceSvc(vector<ReqBalanceSvc *> &vec)
		{
			return __getValue2(vec);
		}
};

class ActiveAllocateBalanceCCA : public ABMTpl<__DFT_T1_,ResBalanceSvc,__DFT_T3_, ABMCMD_ACTIVE_ALLOCATE_BALANCE>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addResBalanceSvc(ResBalanceSvc &data)
		{
			return __add2(data);
		}
		
		int getResBalanceSvc(vector<ResBalanceSvc *> &vec)
		{
			return __getValue2(vec);
		}

};
*/
/*===========================自服务平台请求余额划拨==================================*/
// 自服务平台发起CCR
// CCR 服务信息
/*
struct ReqPlatformSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[64];		// 请求时间
	int m_iRequestTime;
	int m_test;// 补齐
};

// 支付类信息组
struct ReqPlatformPayInfo
{
	char m_sDestinationAccount[UTF8_MAX];	// 用户号码
	int m_iDestinationType;			// 标识类型
	int m_iDestinationAttr;			// 用户属性
	char m_sSvcPlatformId[UTF8_MAX];	// 业务平台标识
	char m_sPayPassword[UTF8_MAX];		// 支付密码
	int m_iBalanceTransFlag;		// 余额划拨类型
	int m_iUnitTypeId;			// 余额的单位类型
	long m_lDeductAmount;			// 需划拨金额
	
};

// CCA 服务信息
struct ResPlatformSvc
{
	ui32 m_svcResultCode;			// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX];	// 错误信息
	char m_responseId[UTF8_MAX];		// 响应流水
	char m_responseTime[UTF8_MAX];		// 响应时间	
	int m_iResponseTime;
	//int iTest;// 凑数
	
	int m_iUnitTypeId;			// 余额的单位类型
	long m_lDeductAmount;			// 成功划拨的金额
	int m_iTest;// 补齐
	
};

// 支付类信息
struct ResPlatformPayInfo
{
	int m_iUnitTypeId;			// 余额的单位类型
	long m_lDeductAmount;			// 成功划拨的金额
	int m_iTest;// 补齐
};

// 余额帐本明细
struct ResPlatformAcctBalance
{
	int m_iBalanceSource;			// 余额帐本来源
	long m_lBalanceAmount;			// 余额帐本当前余额	
	int m_iTest;//补齐
};

// CCR操作类
class PlatformDeductBalanceCCR : public ABMTpl<__DFT_T1_,ReqPlatformSvc,ReqPlatformPayInfo, ABMCMD_PLATFORM_DEDUCT_BALANCE>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包-第一层服务信息
		bool addReqPlatformSvc(ReqPlatformSvc &data)
		{
			return __add2(data);
		}
	
		// 打包-第二层支付信息
		bool addReqPlatformPayInfo(ReqPlatformPayInfo &data)
		{
			return __add3(data);	
		}
	
	
		// 解包-CCR包获取服务请求信息
		int getReqPlatformSvc(vector<ReqPlatformSvc *> &vec)
		{
			return __getValue2(vec);
		}
		
		// 解包-CCR包第二层支付信息
		int getReqPlatformPayInfo(ReqPlatformSvc *pSvc,vector<ReqPlatformPayInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
};
*/

// CCA操作类
/*
class PlatformDeductBalanceCCA : public ABMTpl<ResPlatformSvc,ResPlatformPayInfo,ResPlatformAcctBalance, ABMCMD_PLATFORM_DEDUCT_BALANCE>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addResPlatformSvc(ResPlatformSvc &data)
		{
			return __add1(data);
		}
		

		bool addResPlatformPayInfo(ResPlatformPayInfo &data)
		{
			return __add2(data);
		}
	
		bool addResPlatformAcctBalance(ResPlatformAcctBalance &data)
		{
			return __add3(data);
		}
	
		// 解包
		int getResPlatformSvc(vector<ResPlatformSvc *> &vec)
		{
			return __getValue1(vec);
		}
		

		
		//int getResPlatformPayInfo(ResPlatformSvc *pSvc,vector<ResPlatformPayInfo *> &vec)
		//{
		//	return __getValue3(pSvc,vec);
		//}
		
		int getResPlatformPayInfo(vector<ResPlatformPayInfo *> &vec)
		{
			return __getValue2(vec);
		}
		
		int getResPlatformAcctBalance(ResPlatformPayInfo *pPtr,vector<ResPlatformAcctBalance *> &vec)
		{
			return __getValue3(pPtr,vec);
		}
		
		
	
};
*/
/*
class PlatformDeductBalanceCCA : public ABMTpl<__DFT_T1_,ResPlatformSvc,ResPlatformAcctBalance, ABMCMD_PLATFORM_DEDUCT_BALANCE>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addResPlatformSvc(ResPlatformSvc &data)
		{
			return __add2(data);
		}
			
		bool addResPlatformAcctBalance(ResPlatformAcctBalance &data)
		{
			return __add3(data);
		}
	
		// 解包
		int getResPlatformSvc(vector<ResPlatformSvc *> &vec)
		{
			return __getValue2(vec);
		}
		
		int getResPlatformAcctBalance(ResPlatformSvc *pPtr,vector<ResPlatformAcctBalance *> &vec)
		{
			return __getValue3(pPtr,vec);
		}
};
*/


/*===========================余额划拨冲正===============================*/
/*
// CCR
struct ReqReverseDeductSvc
{
	char m_sRequestId[UTF8_MAX];		// 请求流水
	char m_sRequestTime[32];		// 请求时间
	char m_sOldRequestId[UTF8_MAX];		// 被冲正的划拨流水号
	char m_sABMId[UTF8_MAX];		// 发起冲正ABM标识
	int m_iActionType;			// 冲正操作类型,0:划入冲正,1:划出冲正
};

// CCA
struct ResReverseDeductData
{
	char m_sResponseId[UTF8_MAX];		// 请求流水
	ui32 m_iServiceRltCode;			// 业务级的结果代码
	char m_sResponseTime[32];		// 响应时间
	int m_iTest;
};

class ReverseDeductBalanceCCR : public ABMTpl<__DFT_T1_,ReqReverseDeductSvc,__DFT_T3_, ABMCMD_REVERSE_DEDUCT_BALANCE>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		// 打包
		bool addReqReverseDeductSvc(ReqReverseDeductSvc &data)
		{
			return __add2(data);
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getReqReverseDeductSvc(vector<ReqReverseDeductSvc *> &vec)
		{
			return __getValue2(vec);
		}
};

class ReverseDeductBalanceCCA : public ABMTpl<__DFT_T1_,ResReverseDeductData,__DFT_T3_, ABMCMD_REVERSE_DEDUCT_BALANCE>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addResReverseDeductData(ResReverseDeductData &data)
		{
			return __add2(data);
		}
		
		int getResReverseDeductData(vector<ResReverseDeductData *> &vec)
		{
			return __getValue2(vec);
		}

};
*/

/*===========================支付冲正===================================*/
/********************************************
 * 支付冲正接口定义
 *
 ****************************************** */
/*
struct ReversePaySvc
{
	long lOperationAction;			// 操作的类型,9 - 扣费取消
	char sOperationSeq[UTF8_MAX];		// 操作流水
	char sReqSerial[UTF8_MAX];		// 需要取消扣费的原支付操作流水号
	char sSpId[UTF8_MAX];			// 商家ID
	char sServicePlatformId[UTF8_MAX];	// 平台ID
	ui32 iStaffId;				// 操作员ID
	ui32 iTest;// 补齐
};

struct ReversePayRes1
{
	long lServiceRltCode;		// 业务级的结果代码
	char sParaFieldResult[UTF8_MAX];// 错误信息
};

struct ReversePayRes2
{
	char sOperateSeq[UTF8_MAX];	// 冲正请求流水
	char sReqSerial[UTF8_MAX];	// 计费系统的冲正流水号
	char sParaFieldResult[UTF8_MAX];// 错误信息
};

class ReversePayCCR : public ABMTpl<ReversePaySvc,__DFT_T2_, __DFT_T3_, ABMCMD_REVERSE_PAY>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addReversePaySvc(ReversePaySvc &data)
		{
			return __add1(data);
		}
		
		// 解包
		// CCR包获取服务请求信息
		int getReversePaySvc(vector<ReversePaySvc *> &vec)
		{
			return __getValue1(vec);
		}
};

class ReversePayCCA : public ABMTpl<__DFT_T1_, ReversePayRes1, ReversePayRes2, ABMCMD_REVERSE_PAY>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addReversePayCCA1(ReversePayRes1 &data)
		{
			return __add2(data);
		}
	
		bool addReversePayCCA2(ReversePayRes2 &data)
		{
			return __add3(data);
		}
	
		// 解包	
		int getReversePayCCA1(vector<ReversePayRes1*> &vec)
		{
			return __getValue2(vec);
		}

		int getReversePayCCA2(ReversePayRes1 *pPtr, vector<ReversePayRes2 *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};
*/


/*
// 余额支出记录结构定义
struct BalancePayoutInfo
{
	long m_lOperPayoutId; 	// 支出操作流水
	long m_lAcctBalanceId;	// 余额帐本标识
	long m_lBillingCycleId; // 帐务周期标识
	long m_lOptId;		// 业务流水
	char m_sOperType[3];	// 操作类型
	char m_sOperDate[32];	// 操作时间
	long m_lAmount;		// 金额
	long m_lBalance;	// 操作后金额
	long m_lPrnCount;	// 打印次数
	char m_sState[3];	// 状态
	char m_sStateDate[32];	// 状态日期
	long m_lStaffId;	// 操作员工号	
}
*/

#endif /*__DCC_RA_H_INCLUDED_*/
