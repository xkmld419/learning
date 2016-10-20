#ifndef __ABMCMD_H_INCLUDED_
#define __ABMCMD_H_INCLUDED_

#include "abmobject.h"

#define UTF8_MAX  64
#define UTF8_MAX_32  32
#define TEXT_MAX  256
typedef unsigned int ui32;
typedef unsigned long ui64;
typedef unsigned long time_s;

#define ABMCMD_PAY_ABILITY_QRY  31          //3.4.6.9 支付能力状态查询
#define ABMCMD_BAL_TRANS_QRY  32            //3.4.6.6 划拨规则查询
#define ABMCMD_TRANS_RULES_RESET  33        //3.4.6.7 划拨规则设置
#define ABMCMD_PASSWORD_RESET  34           //3.4.6.8 支付密码修改
#define ABMCMD_PAYSTATUS_RESET  35          //3.4.6.10 支付状态变更
#define ABMCMD_PAYRULE_QRY  36              //3.4.6.11 支付规则查询
#define ABMCMD_PAYRULE_RESET 37 			//3.4.6.12 支付规则变更
#define ABMCMD_BINDINGACCNBR_QRY 38			//3.4.6.3 支付规则变更


#define ABMCMD_QRY_SRV_INFOR  1               //3.4.1.1 查询资料并开户
#define ABMCMD_QRY_SRV_PSWD   8               //3.4.2.5 二次认证密码下发
#define ABMPAYCMD_INFOR       9               //3.4.2.1 支付功能
#define ABMCMD_QRY_SRV_SYN  18   //资料同步
#define ABMCMD_LOGIN_PASSWD 20	//登陆密码鉴权
#define ABMCMD_PASSWD_IDENTIFY 21 //HSS端密码鉴权
#define ABMCMD_PASSWD_CHANGE_CCR 22 //登陆密码修改主动发CCR
#define ABMCMD_PASSWD_CHANGE_CCA 23 //登陆密码修改接收CCA
#define ABMCMD_ALLOCATE_BALANCE  6684801	//3.4.5.1 余额被动划拨
#define ABMCMD_REVERSE_PAY  6684802		//3.4.2.4 支付冲正
#define ABMCMD_ACTIVE_ALLOCATE_BALANCE 6684803	//3.4.5.2 主动划拨
#define ABMCMD_REVERSE_DEDUCT_BALANCE 6684804	//3.4.5.5 划拨冲正
#define ABMCMD_PLATFORM_DEDUCT_BALANCE 6684805	//3.4.6.5 自服务平台请求余额划拨
#define ABMCMD_PLATFORM_DEDUCT_BALANCE_BIZ 6684806 // 
#define ABMCMD_PLATFORM_QUEUE 66848999
#define ABMDEPOSITCMD_INFOR  10    //deposit
#define ABM_REVERSEEPOSIT_CMD  11   //reverse deposit
const int  ABMCMD_QRY_BAL_INFOR = 13;  //余额查询
const int  ABMCMD_QRY_REC_INFOR = 14;  //充退记录查询
const int  ABMCMD_QRY_PAY_INFOR = 15;  //交易记录查询
const int  ABMCMD_QRY_BIL_INFOR = 16;  //电子账单查询
const int  ABMCMD_QRY_MICROPAY_INFO = 4;//余额划拨查询
const int  ABMCMD_QRY_MICROPAYCCA_INFO = 44; //用于余额划拨查询省ABM的请求消息

/*********************************************************************
 *
 *               3.4.1.1 资料查询并开户
 *
 *********************************************************************/
//*3.4.1.1 资料查询并开户
struct ResponsStruct
{
    char m_sResID[UTF8_MAX];
    int m_iRltCode;
    int m_iAlign;    
};
struct QryServInforCond
{
    char  m_sReqID[UTF8_MAX];
    char  m_sServPlatID[UTF8_MAX];
    char  m_sDsnNbr[UTF8_MAX];
    short m_hDsnType;
    short m_hDsnAttr;
    ui32  m_uiQryType;    
};

class QryServInforCCR : public ABMTpl<QryServInforCond, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_SRV_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addServInfor(QryServInforCond &data) {
        return __add1(data);
    }
    
    int getServInfor(vector<QryServInforCond *> &vec) {
        return __getValue1(vec);    
    }
};
 
struct QryServInforData
{
    long  m_lServID;
    long  m_lLocServID;
    long  m_lAcctID;
    ui32  m_uiProdID;
    ui32  m_uiAreaCode;
    char  m_sAccNbr[32];
    char  m_sStateDate[32];
    short m_hServState;
    short m_hPayFlag;        
};

class QryServInforCCA : public ABMTpl<__DFT_T1_, ResponsStruct, QryServInforData, ABMCMD_QRY_SRV_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addRespons(ResponsStruct &data) {
        return __add2(data);    
    }   
    
    bool addServData(QryServInforData &data) {
        return __add3(data);    
    }
    
    int getRespons(vector<ResponsStruct*> &vec) {
        return __getValue2(vec);     
    }
    
    int getServData(ResponsStruct *pPtr, vector<QryServInforData*> &vec) {
        return __getValue3(pPtr, vec);    
    }
};  



/*********************************************************************
 *
 *                3.4.6.13 余额查询
 *
 *********************************************************************/
//3.4.6.13 余额查询
struct QueryBalance
{
	char m_sReqID[UTF8_MAX];
	char m_sDestID[UTF8_MAX];
	int  m_iDestIDType;
	int  m_iDestAttr;
	char m_sAreaCode[UTF8_MAX];
	int  m_iQueryFlg;
	int  m_iQueryItemTyp;
};

class HssQueryBalanceCCR : public ABMTpl<QueryBalance, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_BAL_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }

	bool addReqBal(QueryBalance &data)
	{
		return __add1(data);
	}

	int getReqBal(vector<QueryBalance *> &data)
	{
		return __getValue1(data);
	}
	
};

//余额查询返回结果
struct ResBalanceCode
{
	char m_sResID[UTF8_MAX];
	ui32 m_uiRltCode;
	long m_lTotalBalAvail;
        char m_sParaFieldResult[UTF8_MAX];
};
struct ResQueryBalance
{
	char m_sBalItmDetail[52];
	int  m_iUnitTypID;
	long m_lBalAmount;
	long m_lBalAvailAble;
	//long m_lBalanceOweUsed;
	long m_lBalUsed;
	long m_lBalReserved;
	ui32 m_uiEffDate;
	ui32 m_uiExpDate;
};

class HssQueryBalanceCCA : public ABMTpl<__DFT_T1_, ResBalanceCode, ResQueryBalance, ABMCMD_QRY_BAL_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }
	
	bool addRespons(ResBalanceCode &data)
	{
        return __add2(data);    
    }
	
	bool addResBal(ResQueryBalance &data)
	{
		return __add3(data);
	}

	int getResPons(vector<ResBalanceCode *> &data)
	{
		return __getValue2(data);
	}

	long getResBal(ResBalanceCode * addr, vector<ResQueryBalance *> &data)
	{
		return __getValue3(addr, data);
	}
};


/*********************************************************************
 *
 *                 3.4.6.14 充退记录查询
 *
 *********************************************************************/
//3.4.6.14 充退记录查询
struct RechargeRecQuery
{
	char m_sOperSeq[UTF8_MAX];
	char m_sDestAcct[UTF8_MAX];
	int  m_iDestType;
	int  m_iDestAttr;
	char m_sSrvPlatID[UTF8_MAX];
	ui32 m_uiStartTime;
	ui32 m_uiExpTime;
};

class HssRechargeRecQueryCCR : public ABMTpl<RechargeRecQuery, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_REC_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }

	bool addRec(RechargeRecQuery &data)
	{
		return __add1(data);
	}

	int getRec(vector <RechargeRecQuery *> &data)
	{
		return __getValue1(data);
	}
};

//充退记录查询返回结果
struct ResRecCode
{
	char m_sSeq[UTF8_MAX];
	ui32 m_uiRltCode;
        int  m_iAlign;
        char m_sParaFieldResult[UTF8_MAX];
};
struct ResRechargeRecQuery
{
	ui32 m_uiResult;
	ui32 m_uiOperAct;
	char m_sRechargeTime[UTF8_MAX];
	char m_sDestAcct[UTF8_MAX];
	int  m_iDestAttr;
        int  m_iAlign;
	char m_sSrvPlatID[UTF8_MAX];
	ui32 m_uiChargeType;
	ui32 m_uiRechargeAmount;
};

class HssRechargeRecQueryCCA : public ABMTpl<__DFT_T1_, ResRecCode, ResRechargeRecQuery, ABMCMD_QRY_REC_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }

	bool addRespons(ResRecCode &data)
	{
        return __add2(data);
    }

	bool addRec(ResRechargeRecQuery &data)
	{
		return __add3(data);
	}

	int getRespons(vector<ResRecCode *> &data)
	{
		return __getValue2(data);
	}

	long getRec(ResRecCode * addr, vector<ResRechargeRecQuery *> &data)
	{
		return __getValue3(addr, data);
	}
};


/*********************************************************************
 *
 *                 3.4.6.15 交易记录查询
 *
 *********************************************************************/
//3.4.6.15 交易记录查询
struct QueryPayLogByAccout
{
	ui32 m_uiOperAct;
	char m_sOperSeq[UTF8_MAX];
	char m_sSPID[UTF8_MAX];
	char m_sSPName[UTF8_MAX];
	char m_sSrvPlatID[UTF8_MAX];
	char m_sDestAcct[UTF8_MAX];
	int  m_iDestType;
	int  m_iDestAttr;
	ui32 m_uiStartTime;
	ui32 m_uiExpTime;
};

class HssQueryPayLogByAccoutCCR : public ABMTpl<QueryPayLogByAccout, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_PAY_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }

	bool addPay(QueryPayLogByAccout &data)
	{
		return __add1(data);
	}

	int getPay(vector<QueryPayLogByAccout *> &data)
	{
		return __getValue1(data);
	}
};

//交易记录查询返回结果
struct ResPayCode
{
	char m_sSeq[UTF8_MAX];
        char m_sParaFieldResult[UTF8_MAX];
	ui32 m_uiRltCode;
        int  m_iAlign;
};
struct ResQueryPayLogByAccout
{
	char m_sOderID[UTF8_MAX];
	char m_sDesc[52];
	ui32 m_uiPayDate;
	char m_sSPID[UTF8_MAX];
	char m_sSrvPlatID[UTF8_MAX];
	char m_sStatus[UTF8_MAX];
	char m_sMicropayType[UTF8_MAX];
	long m_lPayAmount;
};

class HssQueryPayLogByAccoutCCA : public ABMTpl<__DFT_T1_, ResPayCode, ResQueryPayLogByAccout, ABMCMD_QRY_PAY_INFOR>
{
public:
	void clear() {
        return ABMObject::clear();    
    }

	bool addRespons(ResPayCode &data)
	{
		return __add2(data);    
	}

	bool addPay(ResQueryPayLogByAccout &data)
	{
		return __add3(data);
	}

	int getRespons(vector<ResPayCode *> &data)
	{
		return __getValue2(data);
	}

	long getPay(ResPayCode * addr, vector<ResQueryPayLogByAccout *> &data)
	{
		return __getValue3(addr, data);
	}
};


/*********************************************************************
 *
 *                 3.4.6.16 电子账单查询
 *
 *********************************************************************/
//3.4.6.16 电子账单查询
struct RechargeBillQuery
{
    char m_sOperSeq[UTF8_MAX];
    char m_sDestAcct[UTF8_MAX];
    int  m_iDestType;
    int  m_iDestAttr;
    ui32 m_uiBilCycID;
};

class HssRechargeBillQueryCCR : public ABMTpl<RechargeBillQuery, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_BIL_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();    
    }

    bool addBil(RechargeBillQuery &data)
    {
        return __add1(data);
    }

    int getBill(vector<RechargeBillQuery *> &data)
    {
        return __getValue1(data);
    }
};

//电子账单查询返回结果
struct ResBilCode
{
    char m_sSeq[UTF8_MAX];
    ui32 m_uiRltCode;
    int  m_iAlign;
    char m_sParaFieldResult[UTF8_MAX];
};
struct ResRechargeBillQuery
{
    long m_lPayMount;
    int  m_iBalance;
    int  m_iLastBalance;
};

class HssRechargeBillQueryCCA : public ABMTpl<__DFT_T1_, ResBilCode, ResRechargeBillQuery, ABMCMD_QRY_BIL_INFOR>
{
public:
    void clear() {
        return ABMObject::clear();    
    }
	
    bool addRespons(ResBilCode &data)
    {
        return __add2(data);    
    }

    bool addBil(ResRechargeBillQuery &data)
    {
        return __add3(data);
    }

    int getRespons(vector<ResBilCode *> &data)
    {
        return __getValue2(data);
    }

    long getBil(ResBilCode* addr, vector<ResRechargeBillQuery *> &data)
    {
        return __getValue3(addr, data);
    }
};

//3.4.6.4余额划拨查询
struct QueryAllPay
{
public:
    char m_sReqID[UTF8_MAX];
    ui32 m_uiReqTime;
    ui32 m_uialign; //字节对齐
    char m_sDestAcct[UTF8_MAX];
    int  m_iDestType;
    int  m_iDestAttr;
    char m_sServPlatID[UTF8_MAX];
    int  m_iQueryFlg;
    int  m_iQueryItemType;
};
class HssPaymentQueryCCR : public ABMTpl<QueryAllPay, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_MICROPAY_INFO>
{
public:
    void clear() {
        return ABMObject::clear();    
    }
	
    bool addReq(QueryAllPay &data)
    {
        return __add1(data);    
    }

    int  getReq(vector <QueryAllPay *> &data)
    {
        return __getValue1(data);
    }
};

//余额划拨返回消息
struct MicroPayRlt
{
    ui32 m_uiRltCode;
    ui32 m_uiResTime;
    char m_sResID[UTF8_MAX];
    char m_sParaFieldResult[UTF8_MAX];
};

struct MicroPaySrv
{
    int m_iBalSrc;
    int m_iUnitTypID;
    long m_lBalAmount;
    char m_sRequestId[UTF8_MAX];
};

class HssResMicroPayCCA : public ABMTpl<__DFT_T1_,MicroPayRlt,MicroPaySrv,ABMCMD_QRY_MICROPAY_INFO>	
{
public:
    void clear() {
        return ABMObject::clear();    
    }

    bool addRlt(MicroPayRlt &data)
    {
        return __add2(data);
    }
	
    bool addSrv(MicroPaySrv &data)
    {
        return __add3(data);
    }

    int getRlt(vector<MicroPayRlt *> &data)
    {
        return __getValue2(data);
    }
    int getSrv(MicroPayRlt *addr, vector<MicroPaySrv *> &data)
    {
        return __getValue3(addr,data);
    }
};



/*********************************************************************
 *
 *                 3.4.2.1支付相关的结构定义
 *
 *********************************************************************/
//支付CCR
typedef struct _Abm_Pay_Cond
{
    char  m_sOperatype[UTF8_MAX_32];//业务操作类型
    ui32  m_iOperateAction;//操作的类型,8–扣费
    char  m_sOperateSeq[UTF8_MAX_32];//支付操作流水号
    char  m_sSpID[UTF8_MAX_32];//商家ID
    char  m_sSpName[UTF8_MAX_32];//商家ID
    char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
    ui32  m_iStaffID;//操作员ID
    char  m_sMicropayType[UTF8_MAX_32];//支付类型	0电信内帐户、1借记卡、2信用卡
    char  m_sDestinationAccount[UTF8_MAX_32];//用户标识
    char  m_sDestinationtype[UTF8_MAX_32];//标识类型,1：通行证帐号,2：业务平台
    int m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
    char  m_sPayPassword[UTF8_MAX_32];//支付密码
    char  m_sRandomPassword[UTF8_MAX_32];//二次验证随机密码
    char  m_sPayDate[UTF8_MAX_32];//订单时间
    unsigned int   m_iPayDate;//订单时间
    char  m_sOrderID[UTF8_MAX_32];//订单ID
    char  m_sDescription[UTF8_MAX_32];//订单简要描述
    long  m_lPayAmount;//支付金额    
    int   m_itest; 
}AbmPayCond;

//支付CCA
typedef struct _ResponsPayOne
{
    int m_iRltCode;//业务级的结果代码
    int m_iRltCodeE;
    char m_sParaFieldResult[UTF8_MAX_32];//avp code
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
 
/*******************************************************************
 *
 *             3.4.2.5 二次认证密码下发
 *
 *******************************************************************/
struct PasswordResponsStruct
{
    char m_ReqID[UTF8_MAX_32];
    ui32 m_ReqTime;
};

//*3.4.2.5 二次认证密码下发
struct QryPasswordCond
{
    char m_sDestAct[UTF8_MAX_32];
    int m_DestType;
    int m_DestAttr;
    char m_SevPlatID[UTF8_MAX_32];
    char m_RanAccNbr[UTF8_MAX_32];
    char m_OrdID[UTF8_MAX_32];
};

struct QryPasswordData
{
        ui32 m_SevResCode;
        char ResID[UTF8_MAX_32];
        ui32 ResTime;
};

class QryPasswordCCR : public ABMTpl<__DFT_T1_, PasswordResponsStruct, QryPasswordCond, ABMCMD_QRY_SRV_PSWD>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addpasswdRespons(PasswordResponsStruct &data) {
    return __add2(data);
        }
        
    bool addServPassWd(QryPasswordCond &data) {
      return __add3(data);    
   }
  
   int getpasswdRespons(vector<PasswordResponsStruct*> &vec) {
      return __getValue2(vec);     
   }

   int getServPassWd(PasswordResponsStruct *pPtr, vector<QryPasswordCond*> &vec) {
      return __getValue3(pPtr, vec);    
   }
};

class QryPasswordCCA :public ABMTpl<QryPasswordData, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_SRV_PSWD>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addServPassWdData(QryPasswordData &data) {
        return __add1(data);    
    }
    
    int getServPassWdData(vector<QryPasswordData *> &vec) {
        return __getValue1(vec);    
    }
}; 

/*******************************************************************
 *
 *             3.4.1.2 资料同步
 *
 *******************************************************************/
//资料同步发送用CCR
struct QryServSynCond
{
	char m_sReqID[UTF8_MAX];			//请求流水
	int m_hDsnType;							//账号值类型
	char m_sDsnNbr[UTF8_MAX];			//账号值
	int m_hDsnAttr;								//用户属性
	char m_sServPlatID[UTF8_MAX];	//业务平台标识
	long m_lServID;								//全国中心用户标识
	long m_lAcctID;								//全国中心账户标识
	char m_sActType[UTF8_MAX];		//服务具体动作
	char m_sAccPin[UTF8_MAX];						//用户登录密码
};

class QryServSynCCR: public ABMTpl<QryServSynCond, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_SRV_SYN>
{
	public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addServSyn(QryServSynCond &data) {
        return __add1(data);
    }
    
    int getServSyn(vector<QryServSynCond *> &vec) {
        return __getValue1(vec);    
    }
};

//资料同步接收用CCA
struct QryServSynData
{
	char m_sResID[UTF8_MAX];			//响应流水
	int  m_iRltCode;									//业务级结果代码
	ui64 m_lServId;					// 省内ServId,省HSS回复时填写
	char m_sParaRes[UTF8_MAX];			//错误信息
	int  m_iTest;//对齐
};

class QryServSynCCA:public ABMTpl<QryServSynData, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_SRV_SYN>
{
	public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addServSyn(QryServSynData &data) {
        return __add1(data);
    }
    
    int getServSyn(vector<QryServSynData *> &vec) {
        return __getValue1(vec);    
    }
};


//开户充值CCR
typedef struct _Deposit_Serv_Cond_One
{
    char  m_sReqSerial[UTF8_MAX_32];//充值请求流水号
    //char  m_sBalanceType[UTF8_MAX_32];//余额类型
    ui32  m_iBalanceType;//余额类型
    ui32  m_iBalanceTypeE;//对齐
    ui32  m_iDestinationBalanceUnit;//余额单位：0－分，1－条
    ui32  m_iVoucherValue;//充值卡面额
    ui32  m_iProlongDays;//充值卡延长有效期(天数)
    char  m_sDestinationType[UTF8_MAX_32];//号码类型（5BA：账户；5BB：客户；5BC：用户）
    char  m_sDestinationAccount[UTF8_MAX_32];//	被充值用户的标识
    int   m_iDestinationAttr;//被充值用户的属性
    ui32  m_iBonusStartTime;//优惠有效期起始时间，yyyymmdd
    ui32  m_iBonusExpirationTime;//优惠有效期终结时间，yyyymmdd
    long  m_lDestinationAttrDetail;
    char  m_sAreaCode[UTF8_MAX];


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
    int  m_iRltCodeE;//	充值结果0：成功；非０：失败错误代码
    char  m_sReqSerial[UTF8_MAX_32];//充值请求流水号
}ResponsDepositOne;
//开户充值CCA
typedef struct _ResponsDepositTwo
{
    int  m_iDestinationBalance;//账本的余额
    //char  m_sBalanceType[UTF8_MAX_32];//余额类型
    ui32  m_iBalanceType;//余额类型
    ui32  m_iBalanceTypE;//余额类型
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

/*******************************************************************
 *
 *             3.4.6.1 登陆密码鉴权
 *
 *******************************************************************/
 struct LoginPasswdStruct
 {
 	char m_sReqID[UTF8_MAX];			//请求流水
    char m_requestTime[64];     // 请求时间
 	ui32 m_iReqTime;
	ui32 m_itest;
 };
 
 struct LoginPasswdCond
 {
 	char m_sDestAccount[UTF8_MAX];	//用户号码
 	int m_hDestType;	//标识类型
 	int m_hDestAttr;	//用户属性
 	char m_sServPlatID[UTF8_MAX];	//业务平台标识
 	char m_sLoginPasswd[UTF8_MAX];//登陆密码
 };
 
 struct LoginPasswdData
 {
 	char m_sResID[UTF8_MAX];	//响应流水
 	int m_hRltCode;	//业务级结果代码
 	char m_sParaRes[UTF8_MAX];			//错误信息
 	ui32 m_iResTime;	//应答时间
 };
 
 class LoginPasswdCCR : public ABMTpl<__DFT_T1_, LoginPasswdStruct, LoginPasswdCond, ABMCMD_LOGIN_PASSWD>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addLoginPasswdRespons(LoginPasswdStruct &data) {
    return __add2(data);
        }
        
    bool addLoginPasswd(LoginPasswdCond &data) {
      return __add3(data);    
   }
  
   int getLoginPasswdRespons(vector<LoginPasswdStruct*> &vec) {
      return __getValue2(vec);     
   }

   int getLoginPasswd(LoginPasswdStruct *pPtr, vector<LoginPasswdCond*> &vec) {
      return __getValue3(pPtr, vec);    
   }
};

class LoginPasswdCCA :public ABMTpl<LoginPasswdData, __DFT_T2_, __DFT_T3_, ABMCMD_LOGIN_PASSWD>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addLoginPasswdData(LoginPasswdData &data) {
        return __add1(data);    
    }
    
    int getLoginPasswdData(vector<LoginPasswdData *> &vec) {
        return __getValue1(vec);    
    }
};

/*******************************************************************
 *
 *             3.4.6.1 密码HSS端鉴权
 *
 *******************************************************************/
 struct PasswdIdentifyCond
 {
 	char m_sReqID[UTF8_MAX];			//请求流水
 	int m_hDsnType;	//账号值类型
 	char m_sDsnNbr[UTF8_MAX];			//查询值
 	int m_hDsnAttr;	//用户属性
 	char m_sServPlatID[UTF8_MAX];	//业务平台标识
 	char m_sLoginPasswd[UTF8_MAX];//登陆密码
 };
 
 struct PasswdIdentifyData
 {
 	char m_sResID[UTF8_MAX];			//响应流水
 	int m_hRltCode;	//业务级结果代码
 	char m_sParaRes[UTF8_MAX];			//错误信息
 };
 
 class PasswdIdentifyCCR: public ABMTpl<PasswdIdentifyCond, __DFT_T2_, __DFT_T3_, ABMCMD_PASSWD_IDENTIFY>
{
	public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addPasswdIdentify(PasswdIdentifyCond &data) {
        return __add1(data);
    }
    
    int getPasswdIdentify(vector<PasswdIdentifyCond *> &vec) {
        return __getValue1(vec);    
    }
};

class PasswdIdentifyCCA:public ABMTpl<PasswdIdentifyData, __DFT_T2_, __DFT_T3_, ABMCMD_PASSWD_IDENTIFY>
{
	public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addPasswdIdentify(PasswdIdentifyData &data) {
        return __add1(data);
    }
    
    int getPasswdIdentify(vector<PasswdIdentifyData *> &vec) {
        return __getValue1(vec);    
    }
};

/*******************************************************************
 *
 *             3.4.5.1 余额被动划拨
 *
 *******************************************************************/
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
	ui32 m_iTest;
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
	int m_iTest;
};

// 余额划拨CCA信息 - 服务信息
struct A_AllocateBalanceSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
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
	ui32 m_test;
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


/*******************************************************************
 *
 *             3.4.5.2 全国中心余额划出 （主动划出）
 *
 *******************************************************************/
// CCR
struct ReqBalanceSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_destinationId[UTF8_MAX];	// 被充值用户号码
	int m_destinationIdType;	// 号码类型
	int m_destinationAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char m_areaCode[UTF8_MAX];	// 电话区号
	char m_requestTime[32];		// 请求时间
	unsigned int m_iRequestTime;		
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
	unsigned int m_iResponseTime;
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

/*******************************************************************
 *
 *                    3.4.5.5 余额划拨冲正
 *
 *******************************************************************/
// CCR
struct ReqReverseDeductSvc
{
	char m_sRequestId[UTF8_MAX];		// 请求流水
	char m_sRequestTime[32];		// 请求时间
        unsigned int m_iRequestTime;	        	// 请求时间
	char m_sOldRequestId[UTF8_MAX];		// 被冲正的划拨流水号
	char m_sABMId[UTF8_MAX];		// 发起冲正ABM标识
	int m_iActionType;			// 冲正操作类型,0:划入冲正,1:划出冲正
//	int m_iTest;
};

// CCA
struct ResReverseDeductData
{
	char m_sResponseId[UTF8_MAX];		// 请求流水
	ui32 m_iServiceRltCode;			// 业务级的结果代码
        char sParaFieldResult[UTF8_MAX]; 
        ui32 m_iTest;
	char m_sResponseTime[32];		// 响应时间
	unsigned int  m_iResponseTime;		// 响应时间
	int m_iTest2;
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

/*******************************************************************
 *
 *                    3.4.6.5 余额划拨(自服务平台发起划拨请求)
 *
 *******************************************************************/
// CCR 服务信息
struct ReqPlatformSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[64];		// 请求时间
	unsigned int m_iRequestTime;
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
	unsigned int m_iResponseTime;
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

// CCA操作类
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

/*******************************************************************
 *
 *                    3.4.2.4 支付冲正
 *
 *******************************************************************/
struct ReversePaySvc
{
	long lOperationAction;			// 操作的类型,9 - 扣费取消
	char sOperationSeq[UTF8_MAX];		// 操作流水
	char sReqSerial[UTF8_MAX];		// 需要取消扣费的原支付操作流水号
	char sSpId[UTF8_MAX];			// 商家ID
	char sSpName[UTF8_MAX];			// 商家名称
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

/*******************************************************************
 *
 *             3.4.1.4.2密码修改
 *
 *******************************************************************/
 
struct PasswdChangeStruct
{
	char m_sReqID[UTF8_MAX];			//请求流水
 	char m_requestTime[64];     //请求时间
 	ui32 m_iReqTime;
	ui32 test;
};	

struct PasswdChangeCond
{
	char m_sDestAccount[UTF8_MAX];	//用户号码
 	int m_hDestType;	//标识类型
 	int m_hDestAttr;	//用户属性
 	char m_sServPlatID[UTF8_MAX];	//业务平台标识
 	char m_sOldPasswd[UTF8_MAX];//当前密码密码
 	char m_sNewPasswd[UTF8_MAX];	//重置后密码
};

struct PasswdChangeData
{
 	char m_sResID[UTF8_MAX];	//响应流水
 	int m_hRltCode;	//业务级结果代码
 	char m_sParaRes[UTF8_MAX];			//错误信息
 	//char m_responseTime[64];	//应答时间
 	ui32 m_iResTime;	//应答时间
};

class PasswdChangeCCR : public ABMTpl<__DFT_T1_, PasswdChangeStruct, PasswdChangeCond, ABMCMD_PASSWD_CHANGE_CCR>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addPasswdChangeRespons(PasswdChangeStruct &data) {
    return __add2(data);
        }
        
    bool addPasswdChange(PasswdChangeCond &data) {
      return __add3(data);    
   }
  
   int getPasswdChangeRespons(vector<PasswdChangeStruct*> &vec) {
      return __getValue2(vec);     
   }

   int getPasswdChange(PasswdChangeStruct *pPtr, vector<PasswdChangeCond*> &vec) {
      return __getValue3(pPtr, vec);    
   }
};

class PasswdChangeCCA :public ABMTpl<PasswdChangeData, __DFT_T2_, __DFT_T3_, ABMCMD_PASSWD_CHANGE_CCR>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addPasswdChangeData(PasswdChangeData &data) {
        return __add1(data);    
    }
    
    int getPasswdChangeData(vector<PasswdChangeData *> &vec) {
        return __getValue1(vec);    
    }
};


/*******************************************************************
 *
 *             3.4.6.9 支付能力状态查询
 *
 *******************************************************************/
// 支付能力状态查询CCR信息 - 服务信息
struct R_PayAbilityQuerySvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 支付能力状态查询CCR信息 - 支付类信息组
struct R_MicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
};

// 支付能力状态查询CCA信息 - 服务信息
struct A_PayAbilityQuerySvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数

};

// 支付能力状态查询CCA信息 -支付类信息组 
struct A_MicropaymentInfo
{
	char m_PayStatus[UTF8_MAX];//支付能力状态
};

class PayAbilityQueryCCR : public ABMTpl<__DFT_T1_,R_PayAbilityQuerySvc, R_MicropaymentInfo, ABMCMD_PAY_ABILITY_QRY>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addPayAbilityQuerySvc(R_PayAbilityQuerySvc &data)
		{
			return __add2(data);
		}
		
		bool addMicropaymentInfo(R_MicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getR_PayAbilityQuerySvc(vector<R_PayAbilityQuerySvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getR_MicropaymentInfo(R_PayAbilityQuerySvc *pSvc,vector<R_MicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class PayAbilityQueryCCA : public ABMTpl<__DFT_T1_, A_PayAbilityQuerySvc, A_MicropaymentInfo, ABMCMD_PAY_ABILITY_QRY>
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
		bool addResPayAbilityQuerySvc(A_PayAbilityQuerySvc &data)
		{
			return __add2(data);
		}
	
		bool addResMicropaymentInfo(A_MicropaymentInfo &data)
		{
			return __add3(data);
		}
	
		// 解包
		/*
		int getRespons(vector<ResponsStruct*> &vec)
		{
			return __getValue2(vec);
		}*/
		
		int getResPayAbilityQuerySvc(vector<A_PayAbilityQuerySvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getResMicropaymentInfo(A_PayAbilityQuerySvc *pPtr, vector<A_MicropaymentInfo *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};

/*******************************************************************
 *
 *             3.4.6.6 余额划拨规则查询
 *
 *******************************************************************/
// 余额划拨规则查询CCR信息 - 服务信息
struct R_BalanceTransSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 余额划拨规则查询CCR信息 - 支付类信息组
struct R_BTMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
};

// 余额划拨规则查询CCA信息 - 服务信息
struct A_BalanceTransSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数

};

// 余额划拨规则查询CCA信息 -支付类信息组 
struct A_BTMicropaymentInfo
{
	int m_dateTransFreq; //每天划拨频度（单位次）
	int m_transLimit;    //每次金额限额（单位分）
	int m_monthTransLimit; //每月金额限额（单位分）
	int m_autotransflag;  //自动触发标志（0-不自动划拨；1-自动）
	int m_autobalancelimit; //自动划拨阀值（单位分）
	int m_autoAmountlimit;  //自动划拨金额（单位分）
};

class BalanceTransCCR : public ABMTpl<__DFT_T1_,R_BalanceTransSvc, R_BTMicropaymentInfo, ABMCMD_BAL_TRANS_QRY>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addBalanceTransSvc(R_BalanceTransSvc &data)
		{
			return __add2(data);
		}
		
		bool addBTMicropaymentInfo(R_BTMicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getBalanceTransSvc(vector<R_BalanceTransSvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getBTMicropaymentInfo(R_BalanceTransSvc *pSvc,vector<R_BTMicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class BalanceTransCCA : public ABMTpl<__DFT_T1_, A_BalanceTransSvc, A_BTMicropaymentInfo, ABMCMD_BAL_TRANS_QRY>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addResBalanceTransSvc(A_BalanceTransSvc &data)
		{
			return __add2(data);
		}
	
		bool addResBTMicropaymentInfo(A_BTMicropaymentInfo &data)
		{
			return __add3(data);
		}
	
		// 解包
		
		int getResBalanceTransSvc(vector<A_BalanceTransSvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getResBTMicropaymentInfo(A_BalanceTransSvc *pPtr, vector<A_BTMicropaymentInfo *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};


/*******************************************************************
 *
 *             3.4.6.7 余额划拨规则设置
 *
 *******************************************************************/
// 余额划拨规则设置CCR信息 - 服务信息
struct R_BalanceTransResetSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 余额划拨规则设置CCR信息 - 支付类信息组
struct R_TRMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
	char m_payPassword[UTF8_MAX];//支付密码
	
	int m_dateTransFreq; //每天划拨频度（单位次）
	int m_transLimit;    //每次金额限额（单位分）
	int m_monthTransLimit; //每月金额限额（单位分）
	int m_autotransflag;  //自动触发标志（0-不自动划拨；1-自动）
	int m_autobalancelimit; //自动划拨阀值（单位分）
	int m_autoAmountlimit;  //自动划拨金额（单位分）
};

// 余额划拨规则设置CCA信息 - 服务信息
struct A_BalanceTransResetSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数

};

// 余额划拨规则设置CCA信息 -支付类信息组 
struct A_TRMicropaymentInfo
{
	int m_dateTransFreq; //每天划拨频度（单位次）
	int m_transLimit;    //每次金额限额（单位分）
	int m_monthTransLimit; //每月金额限额（单位分）
	int m_autotransflag;  //自动触发标志（0-不自动划拨；1-自动）
	int m_autobalancelimit; //自动划拨阀值（单位分）
	int m_autoAmountlimit;  //自动划拨金额（单位分）
};

class BalanceTransResetCCR : public ABMTpl<__DFT_T1_,R_BalanceTransResetSvc, R_TRMicropaymentInfo, ABMCMD_TRANS_RULES_RESET>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addBalanceTransResetSvc(R_BalanceTransResetSvc &data)
		{
			return __add2(data);
		}
		
		bool addTRMicropaymentInfo(R_TRMicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getBalanceTransResetSvc(vector<R_BalanceTransResetSvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getTRMicropaymentInfo(R_BalanceTransResetSvc *pSvc,vector<R_TRMicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class BalanceTransResetCCA : public ABMTpl<__DFT_T1_, A_BalanceTransResetSvc, A_TRMicropaymentInfo, ABMCMD_TRANS_RULES_RESET>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addBalanceTransResetSvc(A_BalanceTransResetSvc &data)
		{
			return __add2(data);
		}
	
		bool addResTRMicropaymentInfo(A_TRMicropaymentInfo &data)
		{
			return __add3(data);
		}
	
		// 解包
		
		int getResBalanceTransResetSvc(vector<A_BalanceTransResetSvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getResRTMicropaymentInfo(A_BalanceTransResetSvc *pPtr, vector<A_TRMicropaymentInfo *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};




/*******************************************************************
 *
 *             3.4.6.8支付密码修改
 *
 *******************************************************************/
 
struct R_PasswordReset
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};	

struct R_PRMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
 	char m_oldPassword[UTF8_MAX];//当前密码密码
 	char m_newPassword[UTF8_MAX];	//重置后密码
 	char m_actionId[UTF8_MAX];	//重置后密码
};

struct A_PasswordReset
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数
};

class PasswordResetCCR : public ABMTpl<__DFT_T1_, R_PasswordReset, R_PRMicropaymentInfo, ABMCMD_PASSWORD_RESET>
{
public:
    void clear() 
    {
        return ABMObject::clear();
    }
    
    bool addR_PasswordReset(R_PasswordReset &data) 
    {
    	return __add2(data);
    }
        
    bool addR_PRMicropaymentInfo(R_PRMicropaymentInfo &data) 
    {
    	return __add3(data);    
   	}
  
   int getR_PasswordReset(vector<R_PasswordReset*> &vec) 
   {
      	return __getValue2(vec);     
   }

   int getR_PRMicropaymentInfo(R_PasswordReset *pPtr, vector<R_PRMicropaymentInfo*> &vec) 
   {
      	return __getValue3(pPtr, vec);    
   }
};

class PasswordResetCCA :public ABMTpl<A_PasswordReset, __DFT_T2_, __DFT_T3_, ABMCMD_PASSWORD_RESET>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addA_PasswordReset(A_PasswordReset &data) {
        return __add1(data);    
    }
    
    int getA_PasswordReset(vector<A_PasswordReset *> &vec) {
        return __getValue1(vec);    
    }
};



/*******************************************************************
 *
 *             3.4.6.10支付状态变更
 *
 *******************************************************************/
 
struct R_PayStatusReset
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};	

struct R_PSRMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
 	char m_payStatus[UTF8_MAX];//支付状态
 	char m_payPassword[UTF8_MAX];	//支付密码

};

struct A_PayStatusReset
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数
	char m_payStatus[UTF8_MAX];//支付状态
};

class PayStatusResetCCR : public ABMTpl<__DFT_T1_, R_PayStatusReset, R_PSRMicropaymentInfo, ABMCMD_PAYSTATUS_RESET>
{
public:
    void clear() 
    {
        return ABMObject::clear();
    }
    
    bool addR_PayStatusReset(R_PayStatusReset &data) 
    {
    	return __add2(data);
    }
        
    bool addR_PSRMicropaymentInfo(R_PSRMicropaymentInfo &data) 
    {
    	return __add3(data);    
   	}
  
   int getR_PayStatusReset(vector<R_PayStatusReset*> &vec) 
   {
      	return __getValue2(vec);     
   }

   int getR_PSRMicropaymentInfo(R_PayStatusReset *pPtr, vector<R_PSRMicropaymentInfo*> &vec) 
   {
      	return __getValue3(pPtr, vec);    
   }
};

class PayStatusResetCCA :public ABMTpl<A_PayStatusReset, __DFT_T2_, __DFT_T3_, ABMCMD_PAYSTATUS_RESET>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addA_PayStatusReset(A_PayStatusReset &data) {
        return __add1(data);    
    }
    
    int getA_PayStatusReset(vector<A_PayStatusReset *> &vec) {
        return __getValue1(vec);    
    }
};



/*******************************************************************
 *
 *             3.4.6.11 支付规则查询
 *
 *******************************************************************/
// 支付规则查询CCR信息 - 服务信息
struct R_PayRulesQuerySvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 支付规则查询CCR信息 - 支付类信息组
struct R_PRQMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
};

// 支付规则查询CCA信息 - 服务信息
struct A_PayRulesQuerySvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数

};

// 支付规则查询CCA信息 -支付类信息组 
struct A_PRQMicropaymentInfo
{
	int m_datePayFreq;//每天支付频度（单位次）
	int m_payLimit;//每次支付限额（单位分）
	int m_monthPayLimit;//每月支付限额（单位分）
	int m_microPayLimit;//小额支付阀值（单位分）位分）
};

class PayRulesQueryCCR : public ABMTpl<__DFT_T1_,R_PayRulesQuerySvc, R_PRQMicropaymentInfo, ABMCMD_PAYRULE_QRY>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addR_PayRulesQuerySvc(R_PayRulesQuerySvc &data)
		{
			return __add2(data);
		}
		
		bool addR_PRQMicropaymentInfo(R_PRQMicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getR_PayRulesQuerySvc(vector<R_PayRulesQuerySvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getR_PRQMicropaymentInfo(R_PayRulesQuerySvc *pSvc,vector<R_PRQMicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class PayRulesQueryCCA : public ABMTpl<__DFT_T1_, A_PayRulesQuerySvc, A_PRQMicropaymentInfo, ABMCMD_PAYRULE_QRY>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addA_PayRulesQuerySvc(A_PayRulesQuerySvc &data)
		{
			return __add2(data);
		}
	
		bool addA_PRQMicropaymentInfo(A_PRQMicropaymentInfo &data)
		{
			return __add3(data);
		}
	
		// 解包
		
		int getA_PayRulesQuerySvc(vector<A_PayRulesQuerySvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getA_PRQMicropaymentInfo(A_PayRulesQuerySvc *pPtr, vector<A_PRQMicropaymentInfo *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};



/*******************************************************************
 *
 *             3.4.6.12 支付规则变更
 *
 *******************************************************************/
// 支付规则变更CCR信息 - 服务信息
struct R_PayRulesResetSvc
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 支付规则变更CCR信息 - 支付类信息组
struct R_PRRMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
	
	char m_payPassword[UTF8_MAX];//支付密码
	
	int m_datePayFreq;//每天支付频度（单位次）
	int m_payLimit;//每次支付限额（单位分）
	int m_monthPayLimit;//每月支付限额（单位分）
	int m_microPayLimit;//小额支付阀值（单位分）位分）
};

// 支付规则变更CCA信息 - 服务信息
struct A_PayRulesResetSvc
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数

};

// 支付规则变更CCA信息 -支付类信息组 
struct A_PRRMicropaymentInfo
{
	int m_datePayFreq;//每天支付频度（单位次）
	int m_payLimit;//每次支付限额（单位分）
	int m_monthPayLimit;//每月支付限额（单位分）
	int m_microPayLimit;//小额支付阀值（单位分）位分）
};

class PayRulesResetCCR : public ABMTpl<__DFT_T1_,R_PayRulesResetSvc, R_PRRMicropaymentInfo, ABMCMD_PAYRULE_RESET>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addR_PayRulesResetSvc(R_PayRulesResetSvc &data)
		{
			return __add2(data);
		}
		
		bool addR_PRRMicropaymentInfo(R_PRRMicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getR_PayRulesResetSvc(vector<R_PayRulesResetSvc *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getR_PRRMicropaymentInfo(R_PayRulesResetSvc *pSvc,vector<R_PRRMicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class PayRulesResetCCA : public ABMTpl<__DFT_T1_, A_PayRulesResetSvc, A_PRRMicropaymentInfo, ABMCMD_PAYRULE_RESET>
{
	public:
		void clear() {
			return ABMObject::clear();
		}
	
		// 打包
		bool addA_PayRulesResetSvc(A_PayRulesResetSvc &data)
		{
			return __add2(data);
		}
	
		bool addA_PRRMicropaymentInfo(A_PRRMicropaymentInfo &data)
		{
			return __add3(data);
		}
	
		// 解包
		
		int getA_PayRulesResetSvc(vector<A_PayRulesResetSvc*> &vec)
		{
			return __getValue2(vec);
		}

		int getA_PRRMicropaymentInfo(A_PayRulesResetSvc *pPtr, vector<A_PRRMicropaymentInfo *> &vec)
		{
			return __getValue3(pPtr, vec);
		}
};


/*******************************************************************
 *
 *             3.4.6.3 绑定号码查询
 *
 *******************************************************************/
// 支付规则查询CCR信息 - 服务信息
struct R_BindingAccNbrQuery
{
	char m_requestId[UTF8_MAX];	// 请求流水
	char m_requestTime[32];		// 请求时间
};

// 支付规则查询CCR信息 - 支付类信息组
struct R_BDAMicropaymentInfo
{
	char m_servNbr[UTF8_MAX];	// 用户号码
	int m_nbrType;		// 号码类型
	int m_servAttr;		// 用户属性，号码类型为用户号码类型时，需填写
	char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
};

// 支付规则查询CCA信息 - 服务信息
struct A_BindingAccNbrQuery
{
	char m_responseId[UTF8_MAX];	// 响应流水
	ui32 m_svcResultCode;		// 业务级的结果代码
	char m_sParaFieldResult[UTF8_MAX]; //AVP ERROR CODE 
	char m_responseTime[32];	// 响应时间
	ui32 m_test;		// 补齐位数
	
	char m_randomAccNbr[32]; //绑定号码
};

class BindingAccNbrQueryCCR : public ABMTpl<__DFT_T1_,R_BindingAccNbrQuery, R_BDAMicropaymentInfo, ABMCMD_BINDINGACCNBR_QRY>
{
	public:
		void clear()
		{
			return ABMObject::clear();
		}
		
		// 打包
		bool addR_BindingAccNbrQuery(R_BindingAccNbrQuery &data)
		{
			return __add2(data);
		}
		
		bool addR_BDAMicropaymentInfo(R_BDAMicropaymentInfo &data)
		{
			return __add3(data);	
		}
	
		// 解包
		// CCR包获取服务请求信息
		int getR_BindingAccNbrQuery(vector<R_BindingAccNbrQuery *> &vec)
		{
			return __getValue2(vec);
		}
		// CCR包获取请求帐目明细信息
		int getR_BDAMicropaymentInfo(R_BindingAccNbrQuery *pSvc,vector<R_BDAMicropaymentInfo *> &vec)
		{
			return __getValue3(pSvc,vec);	
		}
	
};

class BindingAccNbrQueryCCA :public ABMTpl<A_BindingAccNbrQuery, __DFT_T2_, __DFT_T3_, ABMCMD_BINDINGACCNBR_QRY>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addA_BindingAccNbrQuery(A_BindingAccNbrQuery &data) {
        return __add1(data);    
    }
    
    int getA_BindingAccNbrQuery(vector<A_BindingAccNbrQuery *> &vec) {
        return __getValue1(vec);    
    }
};


#endif/*__ABMCMD_H_INCLUDED_*/
