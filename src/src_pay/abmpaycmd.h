/*
 *用户在业务平台上使用全国中心ABM统一支付帐户进行支付，
 *业务平台发送支付请求给全国中心ABM，,接收到请求，到HSS中进行支付密码的鉴权(目前就在集团abm鉴权),
 *鉴权成功后，ABM完成扣费，并发送支付响应到业务平台
 *2011/05/08
 */
#ifndef __ABMPAYCMD_H_INCLUDED_
#define __ABMPAYCMD_H_INCLUDED_

#include "abmobject.h"
#include "abmcmd.h"
#include "errorcode_pay.h"

#define UTF8_MAX_32  32
#define TEXT_MAX_256  256
//typedef unsigned int ui32;
typedef unsigned long ui64;
//#define ABMPAYCMD_INFOR  9   //支付功能
//#define ABMDEPOSITCMD_INFOR  10   //充值开户功能
//#define ABM_REVERSEEPOSIT_CMD  11   //充值冲正

//付款内部使用
typedef struct _Struct_Payment_Info
{
	long m_lPaymentSeq;//扣费流水
	long m_lBalanceSourceSeq;//余额来源流水
	long m_lBalancePayoutSeq;//余额支出流水
	long m_lSourcePayoutSeq;//余额来源支出流水
	long m_lPaymentAmount;//每次付款金额
	long m_lRbkPaymentSeq;//原扣费流水
	long m_lAcctBalanceID;//账本表seq
}StructPaymentSeqInfo;
//余额账本信息表
typedef struct _Acct_Balance_Info
{
   long m_lAcctBalanceID;
   long m_lBalanceTypeID;
   char m_sEffDate[UTF8_MAX_32];
   char m_sExDate[UTF8_MAX_32];
   long  m_lBalance;
   long m_lCycleUpper;
   long m_lCycleLower;
   char m_sCycleUpperType[UTF8_MAX_32];
   char m_sCycleLowerType[UTF8_MAX_32];
   long m_lAcctID;
   long m_lServID;
   long m_lItemGroupID;
   char  m_sState[UTF8_MAX_32];
   char  m_sStateDate[UTF8_MAX_32];
   char  m_sBankAcct[UTF8_MAX_32];
   long m_lObjectTypeID;
   void clear()
   {
	   m_lAcctBalanceID=-1;
	   m_lBalanceTypeID=-1;
	   memset(m_sEffDate,0,sizeof(m_sEffDate));
	   memset(m_sExDate,0,sizeof(m_sExDate));
	   m_lBalance=0;
	   m_lCycleUpper=-1;
	   m_lCycleLower=-1;
	   memset(m_sCycleUpperType,0,sizeof(m_sCycleUpperType));
	   memset(m_sCycleLowerType,0,sizeof(m_sCycleLowerType));
	   m_lAcctID=-1;
	   m_lServID=-1;
	   m_lItemGroupID=-1;
	   memset(m_sState,0,sizeof(m_sState));
	   memset(m_sStateDate,0,sizeof(m_sStateDate));
	   memset(m_sBankAcct,0,sizeof(m_sBankAcct));
	   m_lObjectTypeID=-1;
   };
}AcctBalanceInfo;

//支付接口
typedef struct _Payment_info_All_CCR
{
    	char  m_sOperatype[UTF8_MAX_32];
	char  m_sOperateSeq[UTF8_MAX_32];//流水号
	ui32  m_iStaffID;//操作员ID
	char  m_sDestinationAccount[UTF8_MAX_32];//用户标识号码
    	char  m_sDestinationtype[UTF8_MAX_32];//标识类型,1：通行证帐号,2：业务平台
    	//short m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
    	int m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
	 long  m_lPayAmount;//支付金额     
	 long  m_lCharge_type;//是否一次扣完
	 ui32  m_iBalance_unit;//单位
	 char  m_sDeductDate[UTF8_MAX_32];//扣费时间
	 char  m_sAreaCode[UTF8_MAX_32];//区号
	 char  m_sAcctItemTypeID[UTF8_MAX_32];//账目类型
	 char  m_sSpID[UTF8_MAX_32];//商家ID
	 char  m_sSpName[UTF8_MAX_32];//商家名称
	 char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
	 char  m_sMicropayType[UTF8_MAX_32];//支付类型	0电信内帐户、1借记卡、2信用卡
	 char  m_sOrderID[UTF8_MAX_32];//订单ID
	 char  m_sDescription[UTF8_MAX_32];//订单简要描述
	 char  m_sCapabilityCode[UTF8_MAX_32];//能力编码
	 int   m_iDestinationIDtype;//用户号码类型ID
	 long  m_lServID;//用户标识
	 char  m_sOrderState[UTF8_MAX_32];//订单状态
	 long  m_lPaymentID;//opt_id
	 char  m_sRecType[UTF8_MAX_32];
	 char  m_sOptDate[UTF8_MAX_32];
	 char  m_sPayType[UTF8_MAX_32];
	
}PaymentInfoALLCCR;
typedef struct _Payment_info_All_CCA
{
	long m_lDeductAmount;// 帐本划拨金额	
	long m_lAcctBalanceAmount;// 帐本剩余金额	
	ui64 m_lAcctBalanceId;// 扣费账本标识	
	long m_lDeductAmountAll;// 成功划拨金额	
	long m_lAcctBalance;// 帐户剩余金额	
	ui32 m_iSvcResultCode;// 业务级的结果代码	
	long m_lAcctId;	// 扣费帐务标识型
	long m_lPaymentID;//扣费流水
	
}PaymentInfoALLCCA;
//余额来源
typedef  struct _Balance_Source_Info
{
  long  m_lOperIncomeID;
  long m_lAcctBalanceId ;
  char m_sOperType[UTF8_MAX_32]; 
  long m_iStaffID ;
  char m_sOperDate[UTF8_MAX_32];
  long m_lAmount;
  char m_sSourceType[UTF8_MAX_32];
  char m_sSourceDesc[UTF8_MAX_32];
  char m_sState[UTF8_MAX_32];
  char m_sStateDate[UTF8_MAX_32];
  long m_lAcctBalanceObjID;
  char m_sInvOffer[UTF8_MAX_32];
  char m_sAllowDraw[UTF8_MAX_32];
  char m_sCurrState[UTF8_MAX_32];
  char m_sCurrDate[UTF8_MAX_32];
  long m_lPaymentID;
  long m_lBalance;
  long m_lEffDateOffset;
  long m_lExpDateOffset;
  long m_lRemainAmount;
}BalanceSourceInfo;

// 余额支出记录结构定义
typedef struct _Balance_Payout_Info{	
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
}BalancePayoutInfo;


typedef struct _User_Self_Service
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
	
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	char m_payStatus[UTF8_MAX];//支付能力状态
	
	int m_dateTransFreq; //每天划拨频度（单位次）
	int m_transLimit;    //每次金额限额（单位分）
	int m_monthTransLimit; //每月金额限额（单位分）
	int  m_autotransflag;  //自动触发标志（0-不自动划拨；1-自动）
	int m_autobalancelimit; //自动划拨阀值（单位分）
	int m_autoAmountlimit;  //自动划拨金额（单位分）
	
	char m_oldPassword[32];//当前的密码
	char m_newPassword[32]; //重置后的密码
	char m_actionId[32];//01-修改密码,02-新建密码
	char m_userInfoId[32];		// 用户标识
	
	char m_payPassword[32];//支付密码
	
	char m_randomAccNbr[32]; //绑定号码
	
	int m_datePayFreq;//每天支付频度（单位次）
	int m_payLimit;//每次支付限额（单位分）
	int m_monthPayLimit;//每月支付限额（单位分）
	int m_microPayLimit;//小额支付阀值（单位分）
	
	int m_acctId;
	
	long m_lPaymentSeq;//业务流水
}UserSelfService;

/*
//支付CCR
typedef struct _Abm_Pay_Cond
{
    char  m_sOperatype[UTF8_MAX_32];//业务操作类型
    ui32  m_iOperateAction;//操作的类型,8–扣费
    char  m_sOperateSeq[UTF8_MAX_32];//支付操作流水号
    char  m_sSpID[UTF8_MAX_32];//商家ID
    char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
    ui32  m_iStaffID;//操作员ID
    char  m_sMicropayType[UTF8_MAX_32];//支付类型	0电信内帐户、1借记卡、2信用卡
    char  m_sDestinationAccount[UTF8_MAX_32];//用户标识
    char  m_sDestinationtype[UTF8_MAX_32];//标识类型,1：通行证帐号,2：业务平台
    //short m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
    int m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
    char  m_sPayPassword[UTF8_MAX_32];//支付密码
    char  m_sRandomPassword[UTF8_MAX_32];//二次验证随机密码
    char  m_sPayDate[UTF8_MAX_32];//订单时间
    unsigned int  m_iPayDate;//订单时间
    char  m_sOrderID[UTF8_MAX_32];//订单ID
    char  m_sDescription[UTF8_MAX_32];//订单简要描述
    long  m_lPayAmount;//支付金额     
}AbmPayCond;

//支付CCA
typedef struct _ResponsPayOne
{
    int m_iRltCode;//业务级的结果代码
    int m_iRltCodeE;//HP内存地址对齐问题，需要补齐8个字节
}ResponsPayOne;
//支付CCA
typedef struct _ResponsPayTwo
{
    char m_sOperateSeq[UTF8_MAX_32];//请求流水
    char m_sReqSerial[UTF8_MAX_32];//计费系统的扣费流水号
}ResponsPayTwo;

class AbmPaymentCCR : public ABMTpl<AbmPayCond, __DFT_T2_, __DFT_T3_, ABMPAYCMD_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addPayInfo(AbmPayCond &data) {
        return __add1(data);
    }
    
    int getPayInfo(vector<AbmPayCond *> &vec) {
        return __getValue1(vec);    
    }
};
 


class AbmPaymentCCA : public ABMTpl<__DFT_T1_, ResponsPayOne, ResponsPayTwo, ABMPAYCMD_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addRespons(ResponsPayOne &data) {
        return __add2(data);    
    }   
    
    bool addPayData(ResponsPayTwo &data) {
        return __add3(data);    
    }
    
    int getRespons(vector<ResponsPayOne*> &vec) {
        return __getValue2(vec);     
    }
    
    int getPayData(ResponsPayOne *pPtr, vector<ResponsPayTwo*> &vec) {
        return __getValue3(pPtr, vec);    
    }
};   
*/
//开户资料序列
typedef struct _Serv_Acct_Info_Seq
{
	long m_lServID;
	long m_lAcctID;
	long m_lServAcctID;
	long m_lAcctBalanceID;
	long m_lBalanceSourceID;
	long m_lPaymentID;
	long m_lCustID;
}ServAcctInfoSeq;
/*
//开户充值CCR
typedef struct _Deposit_Serv_Cond_One
{
    char  m_sReqSerial[UTF8_MAX_32];//充值请求流水号
    //char  m_sBalanceType[UTF8_MAX_32];//余额类型
    ui32  m_iBalanceType;//余额类型
    ui32  m_iBalanceTypE;//对齐
    ui32  m_iDestinationBalanceUnit;//余额单位：0－分，1－条
    ui32  m_iVoucherValue;//充值卡面额
    ui32  m_iProlongDays;//充值卡延长有效期(天数)
    char  m_sDestinationType[UTF8_MAX_32];//号码类型（5BA：账户；5BB：客户；5BC：用户）
    char  m_sDestinationAccount[UTF8_MAX_32];//	被充值用户的标识
    int   m_iDestinationAttr;//被充值用户的属性
    ui32  m_iBonusStartTime;//优惠有效期起始时间，yyyymmdd
    ui32  m_iBonusExpirationTime;//优惠有效期终结时间，yyyymmdd
    long  m_lDestinationAttrDetail;//被充值用户的属性明细
    char  m_sAreaCode[UTF8_MAX_32];//区号


    char m_sSourceType[UTF8_MAX_32];//余额来源类型
    char m_sSourceDesc[UTF8_MAX_32];//余额来源描述
    char m_sAllowDraw[UTF8_MAX_32];//是否允许结转
}DepositServCondOne;
//开户充值CCR
typedef struct _Deposit_Serv_Cond_Two
{
    ui32  m_iBonusUnit;//优惠额单位，0－分，1－条
    ui32  m_iBonusAmount;//赠与的优惠金额
    //char  m_sBalanceType[UTF8_MAX_32];//余额类型
    ui32  m_iBalanceType;//余额类型
    ui32  m_iBalanceTypeE;//对齐
    ui32  m_iBonusStartTime;//优惠有效期起始时间，yyyymmdd
    ui32  m_iBonusExpirationTime;//优惠有效期终结时间，yyyymmdd

}DepositServCondTwo;


//开户充值CCA
typedef struct _ResponsDepositOne
{
    int  m_iRltCode;//	充值结果0：成功；非０：失败错误代码
    int  m_iRltCodeE;//	8位对齐
    char  m_sReqSerial[UTF8_MAX_32];//充值请求流水号
}ResponsDepositOne;
//开户充值CCA
typedef struct _ResponsDepositTwo
{
    int  m_iDestinationBalance;//账本的余额
    char  m_sBalanceType[UTF8_MAX_32];//余额类型
    ui32  m_iDestinationBalanceUnit;//余额单位：0－分，1－条
    ui32  m_iDestinationEffectiveTime;//余额生效日期(YYYYMMDD)
    ui32  m_iDestinationExpirationTime;//账户的有效期yyyymmdd
}ResponsDepositTwo;

class DepositInfoCCR : public ABMTpl<__DFT_T1_, DepositServCondOne, DepositServCondTwo, ABMDEPOSITCMD_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addDepositOneCCR(DepositServCondOne &data) {
        return __add2(data);
    }

    bool addDepositTwoCCR(DepositServCondTwo  &data) {
        return __add3(data);
    }
    
    int getDepositOneCCR(vector<DepositServCondOne *> &vec) {
        return __getValue2(vec);    
    }
    int getDepositTwoCCR(DepositServCondOne *pDep, vector<DepositServCondTwo *> &vec) {
        return __getValue3(pDep,vec);    
    }
};
 


class DepositInfoCCA : public ABMTpl<__DFT_T1_, ResponsDepositOne, ResponsDepositTwo, ABMDEPOSITCMD_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addRespDepositOneCCA(ResponsDepositOne &data) {
        return __add2(data);    
    }   
    
    bool addRespDepositTwoCCA(ResponsDepositTwo &data) {
        return __add3(data);    
    }
    
    int getRespDepositOneCCA(vector<ResponsDepositOne*> &vec) {
        return __getValue2(vec);     
    }
    
    int getRespDepositTwoCCA(ResponsDepositOne *pPtr, vector<ResponsDepositTwo*> &vec) {
        return __getValue3(pPtr, vec);    
    }
};  

*/
/*
//充值冲正CCR
typedef struct _Reverse_Deposit_Info
{
    char  m_sReqSerial[UTF8_MAX_32];//冲正请求流水号
    char  m_sPaymentID[UTF8_MAX_32];//原充值请求流水号
    char  m_sDestinationType[UTF8_MAX_32];//号码类型（5BA：账户；5BB：客户；5BC：用户）
    char  m_sDestinationAccount[UTF8_MAX_32];//	被充值用户的标识
    int   m_iDestinationAttr;//被充值用户的属性
    int   m_iDestinationAttrE;//补齐8位

}ReverseDeposit;
//充值冲正CCA
typedef struct _Respons_Reverse_Deposit
{
    char m_sReqSerial[UTF8_MAX_32];//冲正请求流水号
    int  m_iRltCode;//结果0：成功；非０：失败错误代码
    int  m_iRltCodeE;//补齐8位	
}ResponsReverseDeposit;


class ReverseDepositInfoCCR : public ABMTpl<ReverseDeposit,  __DFT_T2_, __DFT_T3_, ABM_REVERSEEPOSIT_CMD>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addReverseDepositInfo(ReverseDeposit &data) {
        return __add1(data);
    }
    
    int getReverseDepositInfo(vector<ReverseDeposit *> &vec) {
        return __getValue1(vec);    
    }
};
 


class ReverseDepositInfoCCA : public ABMTpl<__DFT_T1_, ResponsReverseDeposit, __DFT_T3_, ABM_REVERSEEPOSIT_CMD>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addRespReverseDepositCCA(ResponsReverseDeposit &data) {
        return __add2(data);    
    }   
    
    
    int getRespReverseDepositCCA(vector<ResponsReverseDeposit*> &vec) {
        return __getValue2(vec);     
    }
    
}; 
*/
#endif/*__ABMDEPOSITCMD_H_INCLUDED_*/


