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

#define ABMCMD_ALLOCATE_BALANCE  6684801   //余额被动划拨

#define ABMCMD_ACTIVE_ALLOCATE_BALANCE 6684803 // 主动划拨

#define ABMCMD_REVERSE_DEDUCT_BALANCE 6684804 // 划拨冲正

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
		/*
		bool addRespons(ResponsStruct &data)
		{
			return __add2(data);
		}
		*/
		bool addResBalanceSvcData(A_AllocateBalanceSvc &data)
		{
			return __add2(data);
		}
	
		bool addResDebitAcctData(A_ResponseDebitAcctItem &data)
		{
			return __add3(data);
		}
	
		// 解包
		/*
		int getRespons(vector<ResponsStruct*> &vec)
		{
			return __getValue2(vec);
		}*/
		
		int getResBalanceSvcData(vector<A_AllocateBalanceSvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getAcctBalanceData(A_AllocateBalanceSvc *pPtr, vector<A_ResponseDebitAcctItem *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};



/*============================余额主动划出============================*/
// CCR
struct ReqBalanceSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_destinationId[UTF8_MAX];	// 被充值用户号码
	int m_destinationIdType;	// 号码类型
	int m_destinationAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char m_areaCode[UTF8_MAX];	// 电话区号
	char m_requestTime[32];		// 请求时间
	ui32 m_balanceUnit;		// 余额单位
	long m_rechargeAmount;		// 充值金额
	int m_blanceType;		// 余额类型
};

// CCA
struct ResBalanceSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_responseTime[32];	// 响应时间
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


/*===========================余额划拨冲正===============================*/
// CCR
struct ReqReverseDeductSvc
{
	char m_sRequestId[UTF8_MAX];		// 请求流水
	char m_sRequestTime[32];		// 请求时间
	char m_sOldRequestId[UTF8_MAX];		// 被冲正的划拨流水号
	char m_sABMId[UTF8_MAX];		// 发起冲正ABM标识
	int m_iActionType;			// 冲正操作类型,0:划入冲正,1:划出冲正
	int m_iActionTypeE;
};

// CCA
struct ResReverseDeductData
{
	char m_sResponseId[UTF8_MAX];		// 请求流水
	ui32 m_iServiceRltCode;			// 业务级的结果代码
	char m_sResponseTime[32];		// 响应时间
	int m_iActionTypeE;
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



#endif /*__DCC_RA_H_INCLUDED_*/
