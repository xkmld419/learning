#ifndef __ABMCMD_H_INCLUDED_
#define __ABMCMD_H_INCLUDED_

#include "abmobject.h"
#include <iostream>

using std::cout;

#define UTF8_MAX  64
#define UTF8_MAX_32  32
#define TEXT_MAX  256
typedef unsigned int ui32;
typedef unsigned long time_s;

#define ABMCMD_QRY_SRV_INFOR  1   //3.4.1.1 查询资料并开户
#define ABMCMD_QRY_SRV_PSWD   8   //3.4.2.5 二次认证密码下发
#define ABMPAYCMD_INFOR       9   //3.4.2.1 支付功能
const int  ABMCMD_QRY_BAL_INFOR = 13;  //余额查询
const int  ABMCMD_QRY_REC_INFOR = 14;  //充退记录查询
const int  ABMCMD_QRY_PAY_INFOR = 15;  //交易记录查询
const int  ABMCMD_QRY_BIL_INFOR = 16;  //电子账单查询


//Dcc消息公共部分
/*********
struct struDccHead {
    char m_sSessionId[128];
    
    char m_sSrvTextId[64];   //通过这个字段区分业务消息类型
    char m_sOutPlatform[64];
    
    char m_sSrvFlowId[256];  //编码格式：sn-qqqwe-aaabbbcccddd-version-flowid-track
                             //          24   5     12         max<40>   6    <>;<>;.... 
    int  m_iAuthAppId; 
    int  m_iReqType;
    int  m_iReqNumber;
    int  m_iResultCode;
    
#ifdef _DEBUG_DCC_    
    void print()
    {
        cout<<endl<<"====================="<<endl;
        cout<<"public message of dcc head:"<<endl;
        cout<<"  <session-id>        "<<m_sSessionId<<endl;
        cout<<"  <service-context-id>"<<m_sSrvTextId<<endl;
        cout<<"  <outp-latform-id>   "<<m_sOutPlatform<<endl;
        cout<<"  <Service-Flow-Id>   "<<m_sSrvFlowId<<endl;
        cout<<"  <auth-app-id>="<<m_iAuthAppId<<",request-type="<<m_iReqType
            <<",request-number="<<m_iReqNumber<<",result-code="<<m_iResultCode<<endl;
        return;
    }
#endif

};

*************/
struct ResponsStruct
{
    char m_sResID[UTF8_MAX];
    int m_iRltCode;
    int m_iAlign;    
};

//*3.4.1.1 资料查询并开户
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

//3.4.6.13 余额查询
struct QueryBalance
{
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
	ui32 m_uiRltCode;
	long m_lTotalBalAvail;
};
struct ResQueryBalance
{
	char m_sBalItmDetail[52];
	int  m_iUnitTypID;
	long m_lBalAmount;
	long m_lBalAvailAble;
	long m_lBalanceOweUsed;
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
};
struct ResRechargeRecQuery
{
	ui32 m_uiResult;
	ui32 m_uiOperAct;
	char m_sRechargeTime[UTF8_MAX];
	char m_sDestAcct[UTF8_MAX];
	int  m_iDestAttr;
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

//3.4.6.15 交易记录查询
struct QueryPayLogByAccout
{
	ui32 m_uiOperAct;
	char m_sOperSeq[UTF8_MAX];
	char m_sSPID[UTF8_MAX];
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
	ui32 m_uiRltCode;
};
struct ResQueryPayLogByAccout
{
	char m_sOderID[UTF8_MAX];
	char m_sDesc[52];
	char m_sSPID[UTF8_MAX];
	char m_sSrvPlatID[UTF8_MAX];
	ui32 m_uiPayDate;
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
    char  m_sServicePlatformID[UTF8_MAX_32];//业务平台ID
    ui32  m_iStaffID;//操作员ID
    char  m_sMicropayType[UTF8_MAX_32];//支付类型	0电信内帐户、1借记卡、2信用卡
    char  m_sDestinationAccount[UTF8_MAX_32];//用户标识
    char  m_sDestinationtype[UTF8_MAX_32];//标识类型,1：通行证帐号,2：业务平台
    short m_iDestinationAttr;//用户属性(当用户号码为用户业务号码时填写)0-固话; 1-小灵通; 2-移动,3-ADSL,4-智能公话,5-互联星空 99-未知
    char  m_sPayPassword[UTF8_MAX_32];//支付密码
    char  m_sRandomPassword[UTF8_MAX_32];//二次验证随机密码
    char  m_sPayDate[UTF8_MAX_32];//订单时间
    char  m_sOrderID[UTF8_MAX_32];//订单ID
    char  m_sDescription[UTF8_MAX_32];//订单简要描述
    long  m_lPayAmount;//支付金额     
}AbmPayCond;

//支付CCA
typedef struct _ResponsPayOne
{
    int m_iRltCode;//业务级的结果代码
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
    time_s  m_ReqTime;
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
        time_s ResTime;
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

// 
#endif/*__ABMCMD_H_INCLUDED_*/
