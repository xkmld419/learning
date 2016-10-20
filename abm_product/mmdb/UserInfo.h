/*VER: 9*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef USERINFO_H_HEADER_INCLUDED_BDBC7D5F
#define USERINFO_H_HEADER_INCLUDED_BDBC7D5F


/*	是否严格按照号码的生效时间和失效时间来找serv_id	*/
#define STRICT_SERV_IDENTIFICATION
//昆明这边先不严格要求
/*	如果不要求严格查找，就取消这个宏，重新编译程序	*/

#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMData_B.h"
#include "SHMStringTreeIndex.h"
#include "SHMStringTreeIndex_A.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_A.h"
#include "IpcKey.h"
#include "BillingCycleMgr.h"
#include "AcctItemMgr.h"
#include "OrgMgr.h"
#include "IpcKey.h"
//#include "SegmentMgr.h"


#ifdef  USERINFO_REUSE_MODE
///////打开 USERINFO_REUSE_MODE
    #define USERINFO_SHM_DATA_TYPE  SHMData_A
    #define USERINFO_SHM_INT_INDEX_TYPE  SHMIntHashIndex_A
    #define USERINFO_SHM_STR_INDEX_TYPE  SHMStringTreeIndex_A
	#define USERINFO_ATTACH_DATA(X, Y, Z) \
		X = new SHMData_A<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

    #define USERINFO_ATTACH_FILEDATA(X, Y, Z) \
		X = new SHMData_A<Y> (Z, false); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_FILEDATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex_A (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex_A (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}


#else
///////关闭 USERINFO_REUSE_MODE 
    #define USERINFO_SHM_DATA_TYPE  SHMData
    #define USERINFO_SHM_INT_INDEX_TYPE  SHMIntHashIndex
    #define USERINFO_SHM_STR_INDEX_TYPE  SHMStringTreeIndex
	#define USERINFO_ATTACH_DATA(X, Y, Z) \
		X = new SHMData<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

    #define USERINFO_ATTACH_FILEDATA(X, Y, Z) \
		X = new SHMData<Y> (Z, false); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_FILEDATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}
#endif

struct pathFullName
{
        char sFilePath[128];

        char sCustName[128];  // CUST???tè??·??
        char sProductName[128];  // PRODUCT???tè??·??
        char sServName[128];  // SERV???tè??·??
        char sServTypeName[128];  // SERVTYPE???tè??·??
        char sServLocName[128];  // SERVLOCATION???tè??·??
        char sServStateName[128];  // SERVSTATEATTR???tè??·??
        char sServAttrName[128];  // SERVATTR???tè??·??
    #ifdef IMSI_MDN_RELATION
        char sImsiMdnRelatName[128];  // IMSIMDNRELAT???tè??·??
    #endif
        char sServIdenName[128];  // SERVIDEN???tè??·??
        char sServProdName[128];  // SERVPROD???tè??·??
        char sServProdAttrName[128];  // SERVPRODATTR???tè??·??
        char sServAcctName[128];  // SERVACCT???tè??·??
        char sAcctName[128];  // ACCT???tè??·??
        #ifdef GROUP_CONTROL
        char sGroupInfoName[128];  // GROUPINFO???tè??·??
        char sGroupMemberName[128];  // GROUPMEMBER???tè??·??
        char sNumberGroupName[128];  // NUMBERGROUP???tè??·??
        char sACCNbrOfferTypeName[128];  // ACCNBROFFERTYPE???tè??·??
        #endif
        char sOfferInsName[128];        // OFFERINS???tè??·??
        char sOfferInsAttrName[128];    // OFFERINSATTR???tè??·??
        char sOfferInsDetailName[128];  // OFFERINSDETAIL???tè??·??
	char sOfferInsAgrName[128];  // 
	char sNpName[128];
};

#define CUSTFILE_INFO_IDX           IpcKeyMgr::getIpcKey (-1,"SHM_CUSTFILE_INFO_IDX")
#define PRODFILE_INFO_IDX           IpcKeyMgr::getIpcKey (-1,"SHM_PRODFILE_INFO_IDX")
#define SERVFILE_INFO_IDX           IpcKeyMgr::getIpcKey (-1,"SHM_SERVFILE_INFO_IDX")
#define SERVLOCFILE_INFO_IDX        IpcKeyMgr::getIpcKey (-1,"SHM_SERVLOCFILE_INFO_IDX")
#ifdef	IMSI_MDN_RELATION
#define IMSIMDNRELATFILE_INFO_IDX   IpcKeyMgr::getIpcKey (-1,"SHM_IMSIMDNRELATFILE_INFO_IDX")
#endif
#define SERVIDENTFILE_INFO_IDX      IpcKeyMgr::getIpcKey (-1,"SHM_SERVIDENTFILE_INFO_IDX")
#define SERVPRODFILE_INFO_IDX       IpcKeyMgr::getIpcKey (-1,"SHM_SERVPRODFILE_INFO_IDX")
#define SERVACCTFILE_ACCT_IDX       IpcKeyMgr::getIpcKey (-1,"SHM_SERVACCTFILE_ACCT_IDX")
#define ACCTFILE_INFO_IDX           IpcKeyMgr::getIpcKey (-1,"SHM_ACCTFILE_INFO_IDX")
#ifdef GROUP_CONTROL
#define GROUPFILE_INFO_IDX          IpcKeyMgr::getIpcKey (-1,"SHM_GROUPFILE_INFO_IDX")
#define GROUPFILE_MEMBER_IDX        IpcKeyMgr::getIpcKey (-1,"SHM_GROUPFILE_MEMBER_IDX")
#define NUMBERFILE_GROUP_IDX        IpcKeyMgr::getIpcKey (-1,"SHM_NUMBERFILE_GROUP_IDX")
#define ACCNBROFFERTYPEFILE_IDX     IpcKeyMgr::getIpcKey (-1,"SHM_ACCNBROFFERTYPEFILE_IDX")
#define	ACCNBROFFERTYPEITEMFILE_IDX IpcKeyMgr::getIpcKey (-1,"SHM_ACCNBROFFERTYPEITEMFILE_IDX")
#endif
#define PRODOFFERINSFILE_IDX        IpcKeyMgr::getIpcKey (-1,"SHM_PRODOFFERINSFILE_IDX")
#define OFFERDETAILINS_IDX          IpcKeyMgr::getIpcKey (-1,"SHM_OFFERDETAILINS_IDX")
#define NPINFOFILE_IDX              IpcKeyMgr::getIpcKey (-1,"SHM_NPINFOFILE_IDX")

#define CUST_INFO_DATA                          IpcKeyMgr::getIpcKey (-1,"SHM_CUST_INFO_DATA")
#define CUST_INFO_INDEX                         IpcKeyMgr::getIpcKey (-1,"SHM_CUST_INFO_INDEX")
#define CUSTATTR_INFO_DATA                      IpcKeyMgr::getIpcKey (-1,"SHM_CUSTATTR_INFO_DATA")


//#define CUSTGROUP_INFO_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_CUSTGROUP_INFO_DATA")
//#define CUSTPRICINGPLAN_INFO_DATA             IpcKeyMgr::getIpcKey (-1,"SHM_CUSTPRICINGPLAN_INFO_DATA")

//#define CUSTPRICINGPARAM_INFO_DATA		    0x418
//#define CUSTPRICINGOBJECT_INFO_DATA		    0x419
//#define CUSTPRICINGTARIFF_INFO_DATA           0x421
//#define CUSTPRICINGTARIFF_INFO_INDEX          0x422
//#define CUSTPRICINGPLAN_INFO_INDEX		    0x41a

#define PRODUCT_INFO_DATA                       IpcKeyMgr::getIpcKey (-1,"SHM_PRODUCT_INFO_DATA")
#define PRODUCT_INFO_INDEX                      IpcKeyMgr::getIpcKey (-1,"SHM_PRODUCT_INFO_INDEX")
//#define OFFER_INFO_DATA                       0X407
//#define OFFER_INFO_INDEX                      0X408

#define SERV_INFO_DATA                          IpcKeyMgr::getIpcKey (-1,"SHM_SERV_INFO_DATA")
#define SERV_INFO_INDEX                         IpcKeyMgr::getIpcKey (-1,"SHM_SERV_INFO_INDEX")
#define SERVLOCATION_INFO_DATA                  IpcKeyMgr::getIpcKey (-1,"SHM_SERVLOCATION_INFO_DATA")
#define SERVLOCATION_INFO_INDEX                 IpcKeyMgr::getIpcKey (-1,"SHM_SERVLOCATION_INFO_INDEX")
#define SERVSTATE_INFO_DATA                     IpcKeyMgr::getIpcKey (-1,"SHM_SERVSTATE_INFO_DATA")
#define SERVATTR_INFO_DATA                      IpcKeyMgr::getIpcKey (-1,"SHM_SERVATTR_INFO_DATA")
#define SERVIDENT_INFO_DATA                     IpcKeyMgr::getIpcKey (-1,"SHM_SERVIDENT_INFO_DATA")
#define SERVIDENT_INFO_INDEX                    IpcKeyMgr::getIpcKey (-1,"SHM_SERVIDENT_INFO_INDEX")

//#define SERVGROUP_INFO_DATA                   0X410

#define SERVPRODUCT_INFO_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_SERVPRODUCT_INFO_DATA")
#define SERVPRODUCT_INFO_INDEX                  IpcKeyMgr::getIpcKey (-1,"SHM_SERVPRODUCT_INFO_INDEX")
#define SERVPRODUCTATTR_INFO_DATA               IpcKeyMgr::getIpcKey (-1,"SHM_SERVPRODUCTATTR_INFO_DATA")

#define SERVACCT_INFO_DATA                      IpcKeyMgr::getIpcKey (-1,"SHM_SERVACCT_INFO_DATA")
#define SERVACCT_ACCT_INDEX			            IpcKeyMgr::getIpcKey (-1,"SHM_SERVACCT_ACCT_INDEX")

//#define OFFERINSTANCE_INFO_DATA               0X415
//#define OFFERINSTANCE_INFO_INDEX              0X416
//#define SERVPRICINGPLAN_INFO_DATA             0X417

#define ACCT_INFO_DATA                          IpcKeyMgr::getIpcKey (-1,"SHM_ACCT_INFO_DATA")
#define ACCT_INFO_INDEX                         IpcKeyMgr::getIpcKey (-1,"SHM_ACCT_INFO_INDEX")

//#define SERVGROUPBASE_INFO_DATA			    0X41E
//#define SERVGROUPBASE_INFO_INDEX		        0X41F

#define SERVTYPE_INFO_DATA			            IpcKeyMgr::getIpcKey (-1,"SHM_SERVTYPE_INFO_DATA")
//商品实例
#define PRODOFFERINS_DATA                       IpcKeyMgr::getIpcKey (-1,"SHM_PRODOFFERINS_DATA")
#define PRODOFFERINS_INDEX                      IpcKeyMgr::getIpcKey (-1,"SHM_PRODOFFERINS_INDEX")
#define OFFERDETAILINS_DATA                     IpcKeyMgr::getIpcKey (-1,"SHM_OFFERDETAILINS_DATA")
#define OFFERDETAILINS_INSIDOFFER_INDEX         IpcKeyMgr::getIpcKey (-1,"SHM_OFFERDETAILINS_INSIDOFFER_INDEX")
#define PRODOFFERINSATTR_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_PRODOFFERINSATTR_DATA")
//add by zhaoziwei
#define PRODOFFERINSAGREEMENT_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_PRODOFFERINSAGREEMENT_DATA")
//end

#ifdef GROUP_CONTROL
#define GROUP_INFO_DATA							IpcKeyMgr::getIpcKey (-1,"SHM_GROUP_INFO_DATA")
#define GROUP_INFO_INDEX						IpcKeyMgr::getIpcKey (-1,"SHM_GROUP_INFO_INDEX")
#define GROUP_MEMBER_DATA						IpcKeyMgr::getIpcKey (-1,"SHM_GROUP_MEMBER_DATA")
#define GROUP_MEMBER_INDEX						IpcKeyMgr::getIpcKey (-1,"SHM_GROUP_MEMBER_INDEX")
#define NUMBER_GROUP_DATA						IpcKeyMgr::getIpcKey (-1,"SHM_NUMBER_GROUP_DATA")
#define NUMBER_GROUP_INDEX						IpcKeyMgr::getIpcKey (-1,"SHM_NUMBER_GROUP_INDEX")
#endif
#ifdef GROUP_CONTROL
#define ACCNBROFFERTYPE_DATA					IpcKeyMgr::getIpcKey (-1,"SHM_ACCNBROFFERTYPE_DATA")
#define ACCNBROFFERTYPE_INDEX					IpcKeyMgr::getIpcKey (-1,"SHM_ACCNBROFFERTYPE_INDEX")
#define	SHM_ACCNBROFFERTYPE_ITEM_INDEX				IpcKeyMgr::getIpcKey (-1,"SHM_ACCNBROFFERTYPE_ITEM_INDEX")
#endif

#ifdef	IMSI_MDN_RELATION
#define IMSI_MDN_RELATION_INDEX					IpcKeyMgr::getIpcKey (-1,"SHM_IMSI_MDN_RELATION_INDEX")
#define IMSI_MDN_RELATION_DATA					IpcKeyMgr::getIpcKey (-1,"SHM_IMSI_MDN_RELATION_DATA")
#endif
//以下两行 开发测试 add by xn 2009.10.22
#define NP_INFO_DATA                          IpcKeyMgr::getIpcKey (-1,"SHM_NP_DATA")
#define NP_INFO_INDEX                         IpcKeyMgr::getIpcKey (-1,"SHM_NP_DATA_INDEX")

//##ModelId=4243D79B0218
//##Documentation
//## 客户群信息，在cust之后加载
class CustGroupInfo
{
public:
    //##ModelId=4243D7BB03D6
    //##Documentation
    //## 客户群标识
    long m_lGroupID;

    //##ModelId=4243D7C40158
    //##Documentation
    //## 客户标识
    long m_lCustID;

    //##ModelId=4243D7CA026F
    //##Documentation
    //## 同一个客户的下一个群信息
    unsigned int m_iNextOffset;

};

//##ModelId=41CA41290076
//##Documentation
//## 客户信息，要建立基于custid的索引，在serv之前加载
class CustInfo
{
    public:
    
    //##ModelId=41D3C2980246
    //##Documentation
    //## 客户标识
    long m_lCustID;

    //##ModelId=41D3C2BB00B6
    char m_sCustType[4];

    //##ModelId=41D3C2E700FF
    //int m_iCustLocation;

    //##ModelId=41D3C2ED031A
    ////char m_bIsVip; 

    //##ModelId=41D3C30C01AC
    //##Documentation
    //## 上级客户标识
    ////long m_lParentID;

    //##ModelId=41D3C31600C0
    //##Documentation
    //## 客户统一编码
   // char m_sCustCode[32];

    //##ModelId=41D3C32D02A4
    char m_sState[4];

    //##ModelId=41D3C33100B5
    //char m_sEffDate[16];

    //##ModelId=41D3C3400175
    //char m_sExpDate[16];

    //##ModelId=4243A96903CB
    //##Documentation
    //## 指向该客户的第一个定价信息
    unsigned int m_iCustPlanOffset;
    //##ModelId=4244159E02C1
    unsigned int m_iServOffset;


    //##ModelId=4247E6AB00BA
    unsigned int m_iGroupOffset;

    unsigned int m_iAcctOffset;
    //商品明细的索引
    unsigned int m_iOfferDetailOffset;
};

struct AcctItemOweData
{				
		long acctItemId;
	
		long iServID;	
	
		long iAcctID;	
		
		long iAmount;		
	
		int iBillingCnt;
		
		unsigned int iNextAcctOffset;
	
		unsigned int iNextServOffset;		
	
		char state[4];		
	
		char isValid;
		
		int iParTID ;
		
		char m_sExpDate[16];
};

class AcctCreditInfo
{
public:
	long m_lAcctID;
	long m_lServID;
	int m_iCreditGrade;
	int m_iCreditAmount;
	unsigned int m_iNextOffset;//帐户的下一个用户
};

//##ModelId=41CA62CF031B
//##Documentation
//## 账户信息，
//## 目前考虑不上载到共享内存，没有想到什么地方会获取这个信息
class AcctInfo
{
public:
    //##ModelId=41D3B6150041
    long m_lAcctID;

    //##ModelId=41D3C24A0009
    long m_lCustID;

    //##ModelId=41D3C1C800D4
    int m_iCreditGrade;

    //##ModelId=41D3C1D00325
    int m_iCreditAmount;

    //##ModelId=41D3C2750340
    char m_sState[4];

    //##ModelId=41D3C2790115
    //jchar m_sStateDate[16];

    unsigned int m_iNextOffset; //同一个cust下面的下一个acct
    //商品明细的索引
    unsigned int m_iOfferDetailOffset;

};

//##ModelId=41CA412F0396
//##Documentation
//## 主产品实例信息，主要信息对应SERV表，
//## 要建立基于servid的索引（考虑更新）
//## order by custid
class ServInfo
{
  public:
    //##ModelId=42303C5600E2
    //##Documentation
    //## 主产品标识
    long m_lServID;
    //##ModelId=41D27D9C0392
    //##Documentation
    //## 客户标识
    long m_lCustID;

    //##ModelId=42436B3C015C
    int m_iProductID;

    //##ModelId=42303BE3037C
    char m_sCreateDate[16];

    //##ModelId=42303C1A02A9
    //##Documentation
    //## 起租日期
    char m_sRentDate[16];

    //##ModelId=42303C260089
    char m_sNscDate[16];

    //##ModelId=4230F22803CA
    //##Documentation
    //## 状态
    char m_sState[4];

    //##ModelId=4230F2360077
    char m_sStateDate[16];

    //##ModelId=42303C300278
    //##Documentation
    //## 周期类别
	//short m_iCycleType;
    int m_iCycleType;

    //##ModelId=42436B860054
    char m_sAccNbr[32];

    //##ModelId=42436B720240
    int m_iBillingMode;

    //##ModelId=42436BD40179
    int m_iFreeType;

    //##ModelId=4230EC0F03D9
    //##Documentation
    //## 所属产品的偏移量
    //--unsigned int m_iProductOffset;

    //##ModelId=42355E8D01A9
    //##Documentation
    //## 同一客户的下个SERV
    unsigned int m_iCustNext;

    //##ModelId=42304127015D
    //##Documentation
    //## 商品实例指示
    unsigned int m_iOfferInstanceOffset;

    //##ModelId=42310AD0007D
    //##Documentation
    //## 支付关系指示
    unsigned int m_iServAcctOffset;

    //##ModelId=4243733B01BA
    //##Documentation
    //## 状态改变信息
    unsigned int m_iStateOffset;

    //##ModelId=4243808F024C
    //##Documentation
    //## 附属产品实例指示
    unsigned int m_iServProductOffset;

    //##ModelId=4244078801FD
    //##Documentation
    //## 指向ServLocationInfo里面
    //## 本servid的第一个记录
    unsigned int m_iLocationOffset;

    //##ModelId=42493026038F
    //--unsigned int m_iServPlanOffset;

    //##ModelId=4247BA1001CA
    unsigned int m_iGroupOffset;

    //##ModelId=4243A9B5033E
    //##Documentation
    //## 客户个性化定价信息
    //--unsigned int m_iCustPlanOffset;

    unsigned int m_iCustOffset;

    unsigned int m_iAttrOffset;

    unsigned int m_iServTypeOffset;

    unsigned int m_iServIdentOffset;
    
    //商品明细的索引
    unsigned int m_iOfferDetailOffset;
    //基本商品明细的偏移
    unsigned int m_iBaseOfferOffset;
};

/*
	用户产品的变更记录	
*/
class ServTypeInfo
{
  public:
	long m_lServID;
	int m_iProductID;

	char m_sEffDate[16];
	char m_sExpDate[16];

	int m_iPricingPlanID;

	unsigned int m_iNextOffset;
};

//##ModelId=42303812025F
//##Documentation
//## 主产品标识信息，对应主产品标识表
//## 该信息用来根据主业务号码获取标识
//## 业务号码－>主产品实例标识或者主产品实例
//## 在serv后面加载
class ServIdentInfo
{
public:

    //##ModelId=4230383E032A
    //##Documentation
    //## 主业务号码
    char m_sAccNbr[32];

    //##ModelId=423038BA02C4
    //##Documentation
    //## 主产品标识
    long m_lServID;

    //##ModelId=423038D70027
    //##Documentation
    //## 生效日期
    char m_sEffDate[16];

    //##ModelId=423038EB0026
    //##Documentation
    //## 失效日期
    char m_sExpDate[16];

    //##ModelId=42303908026C
    //##Documentation
    //## 对应的ServInfo的偏移量
    unsigned int m_iServOffset;

    //##ModelId=4238D88F00E4
    //##Documentation
    //## 相同业务号码的下一条记录，按照时间排序
    unsigned int m_iNextOffset;

    //##ModelId=424375570383
    //##Documentation
    //## 同一个主产品实例的下一个ServIdentInfo，现在不用，如果要根据servid获取主业务号码，这个字段就需要使用
    unsigned int m_iServNextOffset;

    int m_iType;
    //网络类型
    int m_iNetworkID;
    //网络类型匹配的优先级别
    int m_iNetworkPriority;

};

//##ModelId=42303A3A017C
//##Documentation
//## 附属产品实例信息，要建立基于servproductid的索引，order by servid，productid，effdate
//## 在serv后面加载，如果不需要获取附加属性，不需要建立索引。
class ServProductInfo
{
  public:
    //##ModelId=42303A4602E2
    //##Documentation
    //## 附属产品实例标识
    long m_lServProductID;
    //##ModelId=42303A5C0077
    //##Documentation
    //## 主产品标识
    long m_lServID;
    //##ModelId=42303A6201A2
    //##Documentation
    //## 产品标识
    int m_iProductID;
    //##ModelId=42303A6901F2
    //##Documentation
    //## 生效时间
    char m_sEffDate[16];
    //##ModelId=42303A7C034E
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];

    //##ModelId=42303A9001B2
    //##Documentation
    //## 某主产品标识的下一个附属产品
    unsigned int m_iNextOffset;

    //##ModelId=424382AC0037
    //##Documentation
    //## 对应的serv的偏移量，用于根据附属产品实例获取主产品实例的信息
    unsigned int m_iServOffset;

    //##ModelId=42577F790380
    unsigned int m_iAttrOffset;

};

//##ModelId=42303F7502EA
//##Documentation
//## 建立基于OfferInstanceID的索引,基本上是从商品明细实例表来的
class OfferInstanceInfo
{
public:
    //##ModelId=42303F9D019D
    //##Documentation
    //## 商品实例标识
    long m_lOfferInstanceID;

    //##ModelId=42303FA702E2
    //##Documentation
    //## 商品标识
    int m_iOfferID;

    //##ModelId=42303FBF016A
    //##Documentation
    //## 定价计划标识
    int m_iPricingPlan;

    //##ModelId=4230429500BA
    //##Documentation
    //## 生效时间
    char m_sEffDate[16];

    //##ModelId=4230429E033D
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];

    //##ModelId=4243BD060168
    //##Documentation
    //## 父实例ID
    ////long m_lChildInstanceID;

    //##ModelId=4243BAEC029B
    //##Documentation
    //## 服实例的偏移量
    unsigned int m_iParentOffset;
    //##ModelId=4247D69C03C6
    //##Documentation
    //## 同一个child，不同时间属于不同的parent，指向下一个
    unsigned int m_iNextOffset;
    //##ModelId=4247EF3C02DA
    char m_sState[4];
    //##ModelId=424B9D7B0269
    long m_lDetailID;

};

//##ModelId=4230EA47006C
//##Documentation
//## 用户地域属性
//## 不支持获取某个OrgID下面的所有SERV
//## 加载时，先加载ServInfo，ServLocationInfo按照ServID、sEffDate排序，这样，对这个表就不要建立索引。调用ServInfo的索引，获取ServInfo的位置，然后设置ServInfo的iLocationOffset。
//## 更新时，发现更新某ServID的信息后，通过ServInfo做索引，呵呵。
class ServLocationInfo
{
  public:

    //##ModelId=4230EA4F02EF
    //##Documentation
    //## 主产品标识，其实这里的数据结构并不需要，因为目前不支持获取某个OrgID下面的所有SERV
    long m_lServID;

    //##ModelId=4230EA55018F
    //##Documentation
    //## 运维机构的标识
    int m_iOrgID;

    //##ModelId=4230EA8C012A
    char m_sEffDate[16];

    //##ModelId=4230EA940262
    char m_sExpDate[16];

    //##ModelId=4230EAD30244
    //##Documentation
    //## 同一个主产品标识的地域属性的下一个，
    //## 按照生效时间排序
    unsigned int m_iNextOffset;

    //同一个org的下一个
    unsigned int m_iOrgNext;

};

//商品实例的结构
class ProdOfferIns
{
public:
    //商品实例ID
    long m_lProdOfferInsID;
    //商品实例明细的偏移
    unsigned int m_iOfferDetailInsOffset;
    //累计量的偏移
    unsigned int m_iAccumuOffset;
    //商品实例属性的偏移
    unsigned int m_iOfferAttrOffset;
    //add by zhaoziwei
    //商品实例协议期的偏移
    unsigned int m_iOfferAgreementOffset;
    //end
    //商品ID
    int m_iOfferID;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
};
//商品明细实例的结构,可以通过(用户,帐户,客户,商品实例ID==m_iInsType)查找到,因此需要单独建立商品ID的index
class OfferDetailIns
{
public:
    //商品实例明细ID
    long m_lOfferDetailInsID;
	//商品实例ID(20090413 wangs 为了并行粒度的细化）
    long m_lOfferInsID;
    //明细ID(可能为用户ID、帐户ID、客户ID、商品实例ID)
    long m_lInsID;
    //明细ID类型(1,2,3,4)
    int m_iInsType;
    //商品明细ID
    int m_iMemberID;
    //商品实例的偏移量
    unsigned int m_iParentOffset;
    //同一个实例下面的下一个明细的偏移量
    unsigned int m_iNextDetailOffset;
    //同一个明细ID的下一个明细的偏移量
    unsigned int m_iNextProdOfferOffset;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
};

class OfferDetailInsBase
{
public:
    //商品实例明细ID
    long m_lOfferDetailInsID;
	//商品实例ID
    long m_lOfferInsID;
    //明细ID(可能为用户ID、帐户ID、客户ID、商品实例ID)
    long m_lInsID;
    //明细ID类型(1,2,3,4)
    int m_iInsType;
    //商品明细ID
    int m_iMemberID;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
};

//商品属性实例
class ProdOfferInsAttr
{
public:
    //属性标识
    int m_iAttrID;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
    //属性值
    char m_sAttrValue[32];
    //序号
    long m_iAttrSeq;
    //同一个商品实例下面的下一个属性实例
    unsigned int m_iNextOfferInsAttrOffset;
};

//add by zhaoziwei
//商品实例协议期
class ProdOfferInsAgreement
{
public:
    //商品实例协议期ID
    long m_lAgreementInstID;
    //商品实例ID
    long m_lProdOfferInsID;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
    //同一个商品实例下面的下一个协议期实例
    unsigned int m_iNextOfferInsAgreementOffset;
};
//end

//##ModelId=4230EB89003D
class ProductInfo
{
public:
    //##ModelId=4230EB9103E2
    int m_iProductID;

    //##ModelId=4230EBA102A5
    int m_iProviderID;

    //##ModelId=4230EBD3036F
    int m_iPricingPlanID;
    //产品所属网络类型
    int m_iNetworkID;
    //网络类型匹配优先级
    int m_iNetworkPriority;
};

//##ModelId=4231071600AD
//##Documentation
//## 帐务定制关系
//## 在serv后面加载
//## order by servid
class ServAcctInfo
{

 public:
 	
 	//关系标识
	long m_lServAcctID;
    //##ModelId=42310A750195
    //##Documentation
    //## 账户标识
    long m_lAcctID;

    //##ModelId=42310AA000D8
    //##Documentation
    //## 主产品标识
    long m_lServID;

    //##ModelId=42310AA9038E
    //##Documentation
    //## 账目组
    int m_iAcctItemGroup;

    //##ModelId=423505E002FB
    //##Documentation
    //## 状态，分为活动、非活动
    //## 1为活动，0为非活动
    int m_iState;

    char m_sEffDate[16];

    char m_sExpDate[16];

    //##ModelId=42310ABB0037
    //##Documentation
    //## 同一主产品标识下一个定制关系
    unsigned int m_iNextOffset;

    //##同一个ACCT的下一个偏移量
    unsigned int m_iAcctNextOffset;
};


//##ModelId=42310B7502AB
//##Documentation
//## 客户个性化定价信息
//## 在cust后面加载
class CustPricingPlanInfo
{
public:
    //##ModelId=42310B920004
    long m_lCustID;

    //##ModelId=42310BA4008C
    int m_iPricingPlanID;

    //##ModelId=42310BAD0266
    int  m_iObjectType;

    //##ModelId=42310BBA0015
    long m_lObjectID;

    //##ModelId=42310BC802D2
    char m_sEffDate[16];

    //##ModelId=42310BD1002C
    char m_sExpDate[16];

    //##ModelId=42310BDE0180
    unsigned int m_iNextOffset;
    //##ModelId=424B592702E8
    //##Documentation
    //## 个性化定价信息标识，
    //## 主键
    long m_lCustPlanID;

    //##ModelId=42AE46330172
    unsigned int m_iParamOffset;


    //##ModelId=42AE463B030E
    unsigned int m_iObjectOffset;
};

//##ModelId=42437DDA020A
//##Documentation
//## 主产品实例附加属性，这个表目前先不上载，如果以后有要用到，再上载（上载也应该是按照某条件上载）
class ServAttrInfo
{
public:
    //##ModelId=42437E370362
    //##Documentation
    //## 主产品标识
    long m_lServID;

    //##ModelId=42437E4702CF
    //##Documentation
    //## 属性标识
    int m_iAttrID;

    //##ModelId=42437E5002AA
    //##Documentation
    //## 属性值
    char m_sValue[16];

    //##ModelId=42437E5800CA
    //##Documentation
    //## 生效日期
    char m_sEffDate[16];

    //##ModelId=42437E5E02F0
    //##Documentation
    //## 失效日期
    char m_sExpDate[16];

    //##ModelId=42437E6502FA
    //##Documentation
    //## 同一个主产品实例的下一个属性值信息，按照属性标识排序
    unsigned int m_iNextOffset;

};

//##ModelId=424378CB037A
//##Documentation
//## 主产品实例的状态信息
class ServStateInfo
{
public:
    //##ModelId=4243793301FD
    //##Documentation
    //## 主产品实例标识，这里并不需要用，因为不需要获取某一时间，某一状态下的所有servid
    long m_lServID;

    //##ModelId=4243793B005A
    //##Documentation
    //## 欠费类型
    int m_iOweType;

    //##ModelId=42437955029C
    //##Documentation
    //## 状态
    char m_sState[4];

    //##ModelId=4243796000FD
    //##Documentation
    //## 生效时间
    char m_sEffDate[16];

    //##ModelId=42437967025C
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];

    //##ModelId=4243798800C9
    //##Documentation
    //## 同一个SERVID对应的状态信息的下一个，按时间排序
    unsigned int m_iNextOffset;

};

//##ModelId=424382DD002E
//##Documentation
//## 附属产品实例的属性信息，目前这个表不上载，等需要再上载（上载也应该按照某种条件上载－－根据需求确定哪些要上载），在ServProductInfo后面加载
class ServProductAttrInfo
{
public:
    //##ModelId=42438321037F
    //##Documentation
    //## 附属产品实例标识，
    long m_lServProductID;

    //##ModelId=42438342012D
    //##Documentation
    //## 属性标识
    int m_iAttrID;

    //##ModelId=424383490106
    //##Documentation
    //## 属性值
    char m_sValue[16];

    //##ModelId=4243835603D5
    //##Documentation
    //## 生效日期
    char m_sEffDate[16];

    //##ModelId=4243835E0106
    //##Documentation
    //## 失效日期
    char m_sExpDate[16];

    //##ModelId=4243838303C6
    //##Documentation
    //## 同一个附属产品实例下的下一个属性，按照属性标识排序
    unsigned int m_iNextOffset;

};

//##ModelId=4243B93400AE
//##Documentation
//## 主产品定价计划信息，这个是根据客户个性化定价信息和商品定价计划、产品定价计划整理出来的,
//## 目前可先不计算
//## 加载完所有信息后，计算获取
class ServPricingPlanInfo
{
public:
    //##ModelId=4243B9D1032A
    //##Documentation
    //## 定价计划标识
    int m_iPricingPlanID;

    //##ModelId=4243BA000115
    //##Documentation
    //## 生效时间
    char m_sEffDate[16];

    //##ModelId=4243BA0C022B
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];

    //##ModelId=4243BA1803C3
    //##Documentation
    //## 下一个定价计划信息
    unsigned int m_iNextOffset;

};

//##ModelId=4243BDB903AA
//##Documentation
//## 商品信息，商品信息和产品信息，应该只会增加，不会修改，建立基于商品ID和产品ID的索引
class OfferInfo
{
public:
    //##ModelId=4243BE0E024E
    int m_iOfferID;

    //##ModelId=4243BE1E030F
    int m_iPricingPlan;

    ///int null;

};

//##ModelId=4243C95F01BE
//##Documentation
//## 用户群信息，在serv加载后加载，这样可以不建索引
class ServGroupInfo
{
public:
    //##ModelId=4243CA150197
    //##Documentation
    //## 用户群ID
    long m_lGroupID;

    //##ModelId=4243CA520145
    //##Documentation
    //## 主产品实例标识
    long m_lServID;

    //##ModelId=4243CA7203E0
    //##Documentation
    //## 生效日期
    char m_sEffDate[16];

    //##ModelId=4243CA7D038C
    //##Documentation
    //## 失效日期
    char m_sExpDate[16];

    //##ModelId=4243CA9200A7
    //##Documentation
    //## 同一个servid的下一个用户群信息
    unsigned int m_iNextOffset;

    unsigned int m_iGroupNext;

};

/*	add by xueqt 2005-08-18	*/
class ServGroupBaseInfo
{
public:
	long m_lGroupID;
	long m_lGroupServID;
	//unsigned int m_iCustPlanOffset;
	unsigned int m_iCustOffset;
	unsigned int m_iServOffset;

	// add by xueqt 20060512
	int m_iGroupType; // 1: Tradition;  2: Realtime
};

//##ModelId=4243D69B036E
//##Documentation
//## 客户属性信息（如果批价要用到客户的属性，建议全部采用纵表的格式，以节约空间）
class CustAttrInfo
{
public:
    //##ModelId=4243D6EA00DD
    //##Documentation
    //## 客户标识
    long m_lCustID;

    //##ModelId=4243D6F102E5
    //##Documentation
    //## 属性标识
    int m_iAttrID;

    //##ModelId=4243D6FF01A5
    //##Documentation
    //## 属性值
    char m_sValue[32];

    //##ModelId=4243D70701B1
    //##Documentation
    //## 生效日期
    char m_sEffDate[16];

    //##ModelId=4243D74C0354
    //##Documentation
    //## 失效日期
    char m_sExpDate[32];

    //##ModelId=4243D754012F
    //##Documentation
    //## 同一个cust的下一个属性值
    unsigned int m_iNextOffset;

};

//##ModelId=424B98C602AC
class InstanceDetailUpdateInfo
{
public:
    //##ModelId=424BA08200F0
    long m_lDetailID;

    //##ModelId=424B990B0387
    //##Documentation
    //## 商品实例标识
    long m_lOfferInstanceID;

    //##ModelId=424B990B03A6
    //##Documentation
    //## 商品标识
    int m_iOfferID;

    //##ModelId=424B990B03E2
    //##Documentation
    //## 生效时间
    char m_sEffDate[16];

    //##ModelId=424B990C0022
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];

    //##ModelId=424B990C0068
    //##Documentation
    //## 服实例的偏移量
    char  m_sObjectType[4];

    //##ModelId=424B990C0090
    //##Documentation
    //## 同一个child，不同时间属于不同的parent，指向下一个
    long m_lObjectID;

    //##ModelId=424B990C00B8
    char m_sState[4];

};

//add 20060710
class CustPricingTariffInfo
{
  public:
    long m_lCustPlanID;
    int  m_iUnit;
    int  m_iRate;
    int  m_iPrecision;
    
    unsigned int m_iLeft;
    unsigned int m_iRight;
    int  m_iBalance;
};

//##ModelId=42AE43D203C5
class CustPricingParamInfo
{
  public:
    //##ModelId=42AE43E502AA
    //##Documentation
    //## 客户定价信息标识
    long m_lCustPlanID;

    //##ModelId=42AE440F03D7
    int m_iParamID;

    //##ModelId=42AE441A0333
    long m_lValue;

    //##ModelId=42AE444000E8
    unsigned int m_iNextOffset;

};

//##ModelId=42AE45230077
class CustPricingObjectInfo
{
  public:
    //##ModelId=42AE457603B5
    long m_lCustPlanID;

    //##ModelId=42AE45AC0150
    int m_iPricingObjectID;

    //##ModelId=42AE45ED02F8
    int m_iObjectType;

    //##ModelId=42AE4605018A
    long m_lObjectID;

    //##ModelId=42AE46150223
    unsigned int m_iNextOffset;

};

#ifdef GROUP_CONTROL
class GroupInfo
{
public:
	int		m_iGroupID;
	char	m_sGroupName[101];
	char	m_sGroupDesc[251];
	int		m_iConditionID;
	int		m_iStrategyID;
	char	m_sEffDate[16];
	char	m_sExpDate[16];
	int		m_iPriority;
	int		m_iOrgID;

	unsigned int m_iNextOffset;//同一group_id的下一记录
};

class GroupMember
{
public:
	long	m_lMemberID;
	char	m_sAccNbr[33];
	int		m_iGroupID;
	char	m_sEffDate[16];
	char	m_sExpDate[16];

	unsigned int m_iNextOffset;//同一号码的下一记录
};

class NumberGroup
{
public:
	long	m_lRelationID;
	char	m_sAccNbr[33];
	int		m_iGroupID;
	char	m_sEffDate[16];
	char	m_sExpDate[16];
	long	m_lOfferInstID;

	unsigned int m_iNextOffset;//同一号码的下一记录
};
class AccNbrOfferType
{
public:
	long m_lItemID;
	char m_sAccNbr[32];		//号码
	 //## 生效时间
    char m_sEffDate[16];

    //##ModelId=424B990C0022
    //##Documentation
    //## 失效时间
    char m_sExpDate[16];
	int  m_iOfferType;
	unsigned int m_iNextOffset;
};

struct GroupInfoResult
{
	int		m_iGroupID;
	int		m_iConditionID;
	int		m_iStrategyID;
	int		m_iPriority;
	long	m_lOfferInstID;
};
#endif

#ifdef	IMSI_MDN_RELATION
struct ImsiMdnRelation{
	long	m_lRelationID;
	char	m_sImsi[MAX_IMSI_LEN];
	char	m_sAccNbr[MAX_CALLING_NBR_LEN];
	char	m_sAreaCode[MAX_BILLING_VISIT_AREA_CODE];
	char	m_sEffDate[16];	
	char	m_sExpDate[16];	
	int 	m_itype;

	unsigned int m_iNextOffset; //同一Imsi的下一记录
};
#endif

//开发测试 add by xn 2009.10.22
class NpInfo
{
	public:
	//	long m_lNpID;//主键 不要了
		//long m_lAgreementID;//和号码一起唯一指定一条记录
		char m_sAccNbr[33]; //NP号码
		int  m_iOrgNetwork;//原网络
		int  m_iNetwork;//现网络
		int  m_iNpState;//状态 5种
		char m_sEffDate[16];//生效时间
		char m_sExpDate[16];//失效时间
		unsigned int  m_iNext;//相同的号码链在一起
};
//##ModelId=42451EF10130
class UserInfoBase
{
//	friend class IvpnInfoAdmin;	//add by zhangyk	
  public:
    //##ModelId=4247679B031B
    UserInfoBase();
	UserInfoBase(pathFullName oFileName);
    //##ModelId=424A148D0363
    operator bool();

 // protected:
    //##ModelId=424930FE0349
    void bindData();

    //##ModelId=4247582700E2
    //##Documentation
    //## 客户信息数据区
    static USERINFO_SHM_DATA_TYPE<CustInfo> *m_poCustData;

    //##ModelId=424758460317
    //##Documentation
    //## 客户信息索引区，建立基于custid的索引
    static USERINFO_SHM_INT_INDEX_TYPE *m_poCustIndex;

    //##ModelId=4247586C039E
    //##Documentation
    //## 客户定价信息数据区
    static USERINFO_SHM_DATA_TYPE<CustPricingPlanInfo> *m_poCustPricingPlanData;

    static USERINFO_SHM_INT_INDEX_TYPE * m_poCustPricingPlanIndex;

    static USERINFO_SHM_DATA_TYPE<CustPricingTariffInfo> *m_poCustTariffData;
    static USERINFO_SHM_STR_INDEX_TYPE *m_poCustTariffIndex;

    //##ModelId=424758860201
    //##Documentation
    //## 客户群信息数据区
    static USERINFO_SHM_DATA_TYPE<CustGroupInfo> *m_poCustGroupData;

    //##ModelId=424758A301F9
    //##Documentation
    //## 客户属性数据区
    ////static SHMData<CustAttrInfo> *m_poCustAttrData;


    //##ModelId=424759C600D5
    //##Documentation
    //## 主产品实例附加属性区
    static USERINFO_SHM_DATA_TYPE<ServAttrInfo> *m_poServAttrData;

    //##ModelId=42475A4A0139
    //##Documentation
    //## 附属产品实例附加属性数据区
    static USERINFO_SHM_DATA_TYPE<ServProductAttrInfo> * m_poServProductAttrData;

    //##ModelId=42475E120131
    //##Documentation
    //## 是否成功连接共享内存
    static bool m_bAttached;

    //##ModelId=424759500248
    //##Documentation
    //## 主产品实例地域属性数据区
    static USERINFO_SHM_DATA_TYPE<ServLocationInfo> *m_poServLocationData;

    static USERINFO_SHM_INT_INDEX_TYPE * m_poServLocationIndex;


    //##ModelId=4247598E0142
    //##Documentation
    //## 主产品实例标识数据区
    static USERINFO_SHM_DATA_TYPE<ServIdentInfo> *m_poServIdentData;

  
    //##ModelId=42492E2300B3
    static CustInfo *m_poCust;

    //##ModelId=4247597A0360
    //##Documentation
    //## 主产品实例状态属性数据区
    static USERINFO_SHM_DATA_TYPE<ServStateInfo> *m_poServStateData;


    //##ModelId=42475A6B0028
    //##Documentation
    //## 帐务定制关系数据区
    static USERINFO_SHM_DATA_TYPE<ServAcctInfo> *m_poServAcctData;

  
    //##ModelId=42492E400083
    static ServInfo *m_poServ;

    //##ModelId=424758FB021D
    //##Documentation
    //## 产品信息索引区
    static USERINFO_SHM_INT_INDEX_TYPE *m_poProductIndex;


    //##ModelId=424945490231
    static CustPricingPlanInfo *m_poCustPlan;

    //##ModelId=424758E1022A
    //##Documentation
    //## 商品信息数据区
    static USERINFO_SHM_DATA_TYPE<OfferInfo> *m_poOfferData;


    //##ModelId=424759FA01B6
    //##Documentation
    //## 主产品标识索引区，基于业务号码的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poServIdentIndex;

  
    //##ModelId=42492E7F003D
    static ServIdentInfo *m_poServIdent;

  
    //##ModelId=424948060351
    static ProductInfo *m_poProduct;

    //##ModelId=42475A1300B7
    //##Documentation
    //## 附属产品实例数据区
    static USERINFO_SHM_DATA_TYPE<ServProductInfo> *m_poServProductData;


    //##ModelId=42492E4902FD
    static CustGroupInfo *m_poCustGroup;

  
    //##ModelId=42492E9A0317
    static ServProductInfo *m_poServProduct;
    //##ModelId=42475D3902A1
    //##Documentation
    //## 主产品实例定价计划信息区
    static USERINFO_SHM_DATA_TYPE<ServPricingPlanInfo> *m_poServPricingPlanData;


    //##ModelId=42492E570018
    static ServLocationInfo *m_poServLocation;

  
    //##ModelId=42492EA902C9
    static ServAcctInfo *m_poServAcct;

    //##ModelId=42475909028C
    //##Documentation
    //## 商品信息索引区
    static USERINFO_SHM_INT_INDEX_TYPE *m_poOfferIndex;


    //##ModelId=42492EBD028B
    static OfferInstanceInfo *m_poOfferInstance;

    //##ModelId=42475A330212
    //##Documentation
    //## 附属产品实例索引区，如果不考虑附属产品实例的附加属性，这个索引不要用
    static USERINFO_SHM_INT_INDEX_TYPE *m_poServProductIndex;

    //## ServAcct针对Acct的索引
    static USERINFO_SHM_INT_INDEX_TYPE * m_poServAcctIndex;


    //##ModelId=42492ED60151
    static ServGroupInfo *m_poServGroup;

    //##ModelId=424758CC0252
    //##Documentation
    //## 产品信息数据区
    static USERINFO_SHM_DATA_TYPE<ProductInfo> *m_poProductData;


    //##ModelId=42475A9101DB
    //##Documentation
    //## 产品实例明细信息
    static USERINFO_SHM_DATA_TYPE<OfferInstanceInfo> *m_poOfferInstanceData;

  
    //##ModelId=424BAF5B0150
    static USERINFO_SHM_INT_INDEX_TYPE *m_poOfferInstanceIndex;


    //##ModelId=42475D610136
    //##Documentation
    //## 用户群信息数据区
    static USERINFO_SHM_DATA_TYPE<ServGroupInfo> *m_poServGroupData;

    static USERINFO_SHM_DATA_TYPE<ServGroupBaseInfo> * m_poServGroupBaseData;
    static USERINFO_SHM_INT_INDEX_TYPE * m_poServGroupBaseIndex;

    //##ModelId=4247593001D3
    //##Documentation
    //## 主产品实例数据区
    static USERINFO_SHM_DATA_TYPE<ServInfo> *m_poServData;

    static USERINFO_SHM_DATA_TYPE<ServTypeInfo> * m_poServTypeData;

    static USERINFO_SHM_DATA_TYPE<CustPricingParamInfo> * m_poCustPricingParamData;

    static USERINFO_SHM_DATA_TYPE<CustPricingObjectInfo> * m_poCustPricingObjectData;

    static USERINFO_SHM_DATA_TYPE<AcctInfo> * m_poAcctData;
    
    static AcctInfo * m_poAcct;

    static USERINFO_SHM_INT_INDEX_TYPE * m_poAcctIndex;

    //##ModelId=4247593E0350
    //##Documentation
    //## 主产品实例索引区
    static USERINFO_SHM_INT_INDEX_TYPE *m_poServIndex;
    
    //商品实例数据和索引
    static USERINFO_SHM_DATA_TYPE<ProdOfferIns> * m_poProdOfferInsData;
    static ProdOfferIns * m_poProdOfferIns;
    static USERINFO_SHM_INT_INDEX_TYPE * m_poProdOfferInsIndex;
    static USERINFO_SHM_DATA_TYPE<OfferDetailIns> * m_poOfferDetailInsData;
    static OfferDetailIns * m_poOfferDetailIns;
    static USERINFO_SHM_INT_INDEX_TYPE * m_poOfferDetailInsOfferInsIndex;
    static USERINFO_SHM_DATA_TYPE<ProdOfferInsAttr> * m_poProdOfferInsAttrData;
    static ProdOfferInsAttr * m_poProdOfferInsAttr;
    //add by zhaoziwei
		static USERINFO_SHM_DATA_TYPE<ProdOfferInsAgreement> * m_poProdOfferInsAgreementData;
		static ProdOfferInsAgreement * m_poProdOfferInsAgreement;
		//end
		
    //##ModelId=42492E70030D
    static ServStateInfo *m_poServState;

    static ServAttrInfo * m_poServAttr;

    static ServProductAttrInfo * m_poServProductAttr;

    static CustAttrInfo * m_poCustAttr;

#ifdef GROUP_CONTROL
	static GroupInfo * m_poGroupInfo;
	static USERINFO_SHM_DATA_TYPE<GroupInfo> *m_poGroupInfoData;
	static USERINFO_SHM_INT_INDEX_TYPE *m_poGroupInfoIndex;
	static GroupMember * m_poGroupMember;
	static USERINFO_SHM_DATA_TYPE<GroupMember> *m_poGroupMemberData;
	static USERINFO_SHM_STR_INDEX_TYPE *m_poGroupMemberIndex;
	static NumberGroup * m_poNumberGroup;
	static USERINFO_SHM_DATA_TYPE<NumberGroup> *m_poNumberGroupData;
	static USERINFO_SHM_STR_INDEX_TYPE *m_poNumberGroupIndex;

	static USERINFO_SHM_DATA_TYPE<AccNbrOfferType>* m_poAccNbrData;
	static USERINFO_SHM_STR_INDEX_TYPE * m_poAccNbrIndex;
	static USERINFO_SHM_INT_INDEX_TYPE *m_poAccNbrItemIndex;
	static AccNbrOfferType* m_poAccNbr;
	
#endif

#ifdef IMSI_MDN_RELATION 
	static ImsiMdnRelation * m_poImsiMdnRelation;
	static USERINFO_SHM_DATA_TYPE<ImsiMdnRelation> * m_poImsiMdnRelationData;
	static USERINFO_SHM_STR_INDEX_TYPE* m_poImsiMdnRelationIndex;//imsi索引
#endif

//开发测试 add by xn 2009.10.22
	static USERINFO_SHM_DATA_TYPE<NpInfo> *m_poNpData;
	static NpInfo *m_poNp;
	static USERINFO_SHM_STR_INDEX_TYPE* m_poNpIndex;
//结束

 // private:
    //##ModelId=42452759033D
    //##Documentation
    //## 加载共享内存信息
    void loadSHMInfo();
    //##ModelId=4247790103D4
    void freeAll();
    
    void closeAll();

};

//为了客户个性化的定价参数和定价对象，
class ExPricingPlan
{
	public:
		int m_iPricingPlanID;
		unsigned int m_iCustPlanOffset;
};

#endif /* USERINFO_H_HEADER_INCLUDED_BDBC7D5F */
