/*VER: 8*/ 
#ifndef USERINFOREADER_H_HEADER_INCLUDED_BDB745F9
#define USERINFOREADER_H_HEADER_INCLUDED_BDB745F9

#include "../../etc/LocalDefine.h"

#include "CommonMacro.h"
#include "UserInfo.h"
#include "StdEvent.h"
#include "BillingCycle.h"
#include "BillingCycleMgr.h"

#include <vector>
#include "AcctItemMgr.h"
#include "BillingCycleMgr.h"
#include "OrgMgr.h"
#include "StateConvertMgr.h"
#include "Environment.h"
#include "NetworkMgr.h"
#include "OfferMgr.h"
#include "SHMFileInfo.h"
class PricingCombine;
using namespace std;

class BillingCycleMgr;
class Serv;


//#define USERINFO_LIMIT "USERINFO_MEM_LIMIT"
#ifdef SHM_FILE_USERINFO
//##ModelId=424A47C303AB
class UserInfoReader : public SHMFileInfo
{
  public:
    //##ModelId=42525763028B
    UserInfoReader();
	UserInfoReader(int iMode);

  protected:
    //##ModelId=424A47DC01D0
    static BillingCycleMgr * m_poBillingCycleMgr;

    //##ModelId=424A47DC01EE
    static AcctItemMgr * m_poAcctItemMgr;

    //##ModelId=424A47DC0202
    static OrgMgr * m_poOrgMgr;

    static StateConvertMgr * m_poStateConvertMgr;
    
    static NetworkPriorityMgr * m_poNetworkMgr;
    
    static OfferMgr * m_poOfferMgr;
    
    #ifdef USERINFO_OFFSET_ALLOWED
    static int m_offsetSecs;

    static int m_beginOffsetSecs;

    void loadOffsetSecs();
    #endif

    long execOperation(int iOperation,StdEvent * m_poEvent,int iAttrID = 0);
};
#else
//##ModelId=424A47C303AB
class UserInfoReader : public UserInfoBase
{
  public:
    //##ModelId=42525763028B
    UserInfoReader();
	UserInfoReader(int iMode);

  protected:
    //##ModelId=424A47DC01D0
    static BillingCycleMgr * m_poBillingCycleMgr;

    //##ModelId=424A47DC01EE
    static AcctItemMgr * m_poAcctItemMgr;

    //##ModelId=424A47DC0202
    static OrgMgr * m_poOrgMgr;

    static StateConvertMgr * m_poStateConvertMgr;
    
    static NetworkPriorityMgr * m_poNetworkMgr;
    
    static OfferMgr * m_poOfferMgr;
    
    #ifdef USERINFO_OFFSET_ALLOWED
    static int m_offsetSecs;

    static int m_beginOffsetSecs;

    void loadOffsetSecs();
    #endif

    long execOperation(int iOperation,StdEvent * m_poEvent,int iAttrID = 0);
};
#endif
//##ModelId=4257416200BE
//##Documentation
//## 20050409新增属性类，
//## 描述serv，servproduct，cust的属性对象，
//## 需求为周期性费用生成时，要获取某附属产品实例
//## 的所有属性，包括这些属性的生效时间，失效时间，
//## 所以新增加此类。
class UserInfoAttr
{
  public:
    //##ModelId=42578E4F0129
    int &getAttrID();

    //##ModelId=42578E570383
    char *getAttrValue();

    //##ModelId=42578E60015F
    char *getEffDate();

    //##ModelId=42578E6603B7
    char *getExpDate();

  private:
    //##ModelId=4257439E0369
    //##Documentation
    //## 属性标识
    int m_iAttrID;

    //##ModelId=425743B102C6
    char m_sAttrValue[32];

    //##ModelId=425743ED02C2
    char m_sEffDate[16];

    //##ModelId=425743F503B4
    char m_sExpDate[16];

};

//##ModelId=42414E1D038F
class ServProduct : public UserInfoReader
{
	friend class ServProductIteration;
	friend class Serv;
	friend class UserInfoInterface;
  public:
    //##ModelId=42414E7A0176
    long getServProductID();

    //##ModelId=42414E81022B
    long getServID();

    //##ModelId=42414E86026E
    int getProductID();

    bool getEff(char *sDate);

    int getUsingDay(char *sBeginDate, char *sEndDate, Serv * pServ = 0);

    //##ModelId=42414E8C0226
    char *getEffDate();

    //##ModelId=42414E9202F7
    char *getExpDate();

    //##ModelId=42414E99010D
    char * getAttr(int attrid, char *sDate = 0);

    //##ModelId=42578D9C03AC
    int getAttrs(vector<UserInfoAttr> &ret);

    //##ModelId=4271EA030243
    //##Documentation
    //## 返回ServProductInfo的指针，
    //## 这个地方考虑来考虑去，
    //## 暂时先加上这个方法
    ServProductInfo *getServProductInfo();

  private:
    //##ModelId=4248A99203A3
    ServProductInfo *m_poServProductInfo;

    //##ModelId=4248A9AD006D
    ServInfo *m_poServInfo;

};

//##ModelId=42414EE6038E
class ServProductIteration : public UserInfoReader
{
	friend class Serv;
  public:
    //##ModelId=42414F1C02D8
    bool next(ServProduct &servProduct);
  private:
    //##ModelId=4248AEBB0190
    unsigned int m_iCurOffset;


    //##ModelId=4248AF2200F8
    bool m_bAll;
    //##ModelId=4248B13C0110
    char m_sTime[16];


};
//商品实例查询对象
class OfferInsQO : public UserInfoReader
{
public:
    //商品实例
    ProdOfferIns* m_poOfferIns;
    //商品实例明细
    OfferDetailIns* m_poDetailIns;
    //优先级别
    int m_iPriority;
    //商品类别
    int m_iOfferType;
    //是否有累积量
    bool m_bAccued;
    
    char m_sTime[16];
    //获得商品实例的所有属性值
    int getOfferInsAttr(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate = 0);
    //获得商品实例的所有属性值（带生失效时间，给界面展示用）
    int getOfferInsAttrAndDate(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate = 0);

    //获得商品实例的某个属性值
    int getOfferInsAttrByAttrID(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], int iAttrID, char * sDate = 0,StdEvent * poEvent =0);
	//只获取商品实例某个属性值的个数
    int getOfferInsAttrCntByAttrID(int iAttrID,char* sDate =0,StdEvent * poEvent =0);
    //获得商品实例的某个属性值(@TODO这个返回类型应该为const)
    char *getOfferAttr(int iAttrID, char *sDate = 0,StdEvent * poEvent =0);
    //商品实例的某个属性值是否为iAttrVal
    bool isOfferAttrValue(int iAttrID, char* iAttrVal, char *sDate = 0,StdEvent * poEvent =0);
    //商品实例是否有某个成员
    bool isOfferMember(int iMemberID, char *sDate = 0);
    //获得商品实例的所有成员
    int getOfferInsMember(OfferDetailIns ret[MAX_OFFER_MEMBERS], 
                                                            char *sDate = 0);
    OfferDetailIns * getOneMemberIns(int iMemberID, char *sDate = 0);
    int getOfferInsMember(OfferDetailIns * ret[MAX_OFFER_MEMBERS],
                            int iMemberID, char * sDate = 0);
    //判断用户，账户是否属于该商品的iMemberID成员
    bool isOfferInsMember(long lServID, long lAcctID, int iMemberID, char *sDate = 0);
		//add by zhaoziwei
		//获得商品实例的协议期信息
		bool getOfferInsAgreement(ProdOfferInsAgreement &ret,char * sDate = 0);
		//end

private:
    bool __isOfferMember(long lProdOfferInsID, int iMemberID, char *sDate = 0);
    
    int __getOfferInsMember(OfferDetailIns ret[MAX_OFFER_MEMBERS], long lProdOfferInsID, char *sDate = 0);
    
    bool __isOfferInsMember(long lProdOfferInsID, long lServID, long lAcctID, int iMemberID, char *sDate);
};
//商品实例访问接口
class OfferInsIteration : public UserInfoReader
{
    friend class UserInfoInterface;
public:
    bool next(ProdOfferIns &offerins);
private:
    unsigned int m_iCurOffset;
};
//##ModelId=4232A6400001
//##Documentation
//## 主产品实例类。
//## 该类对应一个主产品实例
class Serv : public UserInfoReader
{
    friend class ServIteration;
    friend class UserInfoInterface;

  public:

	//##ModelId=4232A90601B0
    //##Documentation
    //## 获取主产品ID
    long getServID();

    //##ModelId=4232AE380131
    //##Documentation
    //## 该主产品实例是否归属于某商品实例
    bool belongOfferInstance(long instanceid, char *sdate = 0);
    
	char const * getServTypeEffDate(char *sDate = 0);

        //某段时间内是否是该产品
    //pBeginDate < ExpDate && pEndDate > pEffDate
    bool hasProduct(int nProductID,char* pBeginDate,char* pEndDate=0);

    char const * getAccNBR(int iType, char *stime);

    //##ModelId=4232AE690131
    //##Documentation
    //## 是否归属于某商品
    bool belongOffer(int offerid, char *sdate = 0);

    //##ModelId=4232AEBF0370
    //##Documentation
    //## 是否有某附属产品
    //## 如果有的话，返回到sp中
    bool hasServProduct(int productid, ServProduct & sp, char *sdate) ;
    
    //指定的时间是否在该附属产品的生失效时间范围内
    bool hasServProduct(int nServProductID,char* pBeginDate,char* pEndDate=0);

	//开通附属产品的时间是否是指定的时间范围内
    bool hasServProductEx(int nServProductID,char* pBeginDate,char* pEndDate=0);

    //##ModelId=4232B01003D8
    //##Documentation
    //## 获取附属产品开通时间
    char const *getProductEffDate(int productid, char *sdate = 0);

    //根据serv_product_id获取附属产品开通时间
    char const *getServProductEffDate(long iServProductID, char *sdate = 0);

    //##ModelId=4232B430006F
    //##Documentation
    //## 返回运维机构标识
    int getOrgID(
        //##Documentation
        //## 时间，如果为空，则根据 查找主产品实例的时间条件 来查找
        char *sDate = 0);

    //##ModelId=4232B69901F3
    //##Documentation
    //## 获取某账目类型的付费账号
    long getAcctID(int acctitemtype);
    long getAcctID(int acctitemtype,char * sDate);
	long getAcctID(int acctitemtype,StdEvent * poEvent);    
    void getAcctID(vector<long> & vAcctID);
    void getAcctID(vector<long> & vAcctID,char * sDate);
	void getAcctID(vector<long> & vAcctID,StdEvent * poEvent);

    //##ModelId=4232B6FC02F0
    //##Documentation
    //## 获取该实例的帐务周期类型
    int getBillingCycleType();

    //##ModelId=4232B70B032E
    //##Documentation
    //## 返回某时间所处的帐务周期
    BillingCycle const * getBillingCycle(char *time = 0, int iCycleType = -1);

    //##ModelId=423505310263
    //##Documentation
    //## 获取该实例，某时间可能适用的定价计划，优先级从高到低排
    int getPricingPlans(
        //##Documentation
        ExPricingPlan ret[MAX_PRICINGPLANS], char * sDate = 0);

    //##ModelId=423506410214
    //##Documentation
    //## 获取基本的定价计划（产品的），没有返回0
    int getBasePricingPlan(char *sDate = 0);

    //##ModelId=423506930302
    //##Documentation
    //## 返回CUSTID
    long getCustID();

    //##ModelId=42355F1103A7
    //##Documentation
    //## 返回cust类型
    char * getCustType();

    //##ModelId=423677D80135
    //##Documentation
    //## 获取该主产品所属客户的所有主产品的id
    int getCustServ(
        //##Documentation
        //## 输出结果到vector
        vector<long> &ret);

    int getProductID(char *sDate = 0);


    //##ModelId=42414DD400F5
    int getOffers(int  offers[MAX_OFFERS], char *sdate = 0);

    //##ModelId=42414DF202E3
    int getOfferInstances(long offerinstances[MAX_OFFERINSTANCES], char *sdate = 0);

    //##ModelId=42414E330142
    bool getServProductIteration(ServProductIteration &servProductIteration, 
        //##Documentation
        //## 某时间，
        //## 如果不指定，认为是当前serv的时间
        //## 某时间生效的附属产品
        char *sdate = 0,
        //##Documentation
        //## 是否所有的servproduct，
        //## 如果为true，获取这个serv下所有的servproduct（不管是否生效），此时第二个参数忽略
        bool isall = false);

    bool getServProduct(long lServProductID, ServProduct & servProduct);

    //##ModelId=4242BA380063
    bool belongOrg(int orgid);

    //##ModelId=42437460021F
    //##Documentation
    //## 获取某时间，该主产品实例的状态
    char *getState(char *sDate = 0);
    
    char *getPreState();

    //获得用户最新的状态
    char *getLatestState();

	//获得用户最新的状态的生效时间减1秒
    char *getLatestStateDate();

    //##某时刻的计费状态
    char *getBillingState(char *sDate = 0);

    //##某一天是否在用
    bool getUsing(Date d);	

    //##获取[sBeginDate, sEndDate]的使用天数
    int  getUsingDay(char *sBeginDate, char *sEndDate);

    //##ModelId=4248BC6B013E
    bool belongServGroup(long groupid, char *sdate = 0);

    //##ModelId=4248BC7600EA
    bool belongCustGroup(long groupid);
    //##ModelId=425A2BD10371
    char *getRentDate();

    //##ModelId=425A2BD902E6
    //##ModelId=425A2BD902E6
    char *getCreateDate();

    //PricingCombine *getCombine(int iEventType, char *sDate = 0);
    //##ModelId=425A2BEE0264
    char *getNscDate();

    //##ModelId=425A2BFD0374
    char *getStateDate(char * sDate = 0);
    //##ModelId=426F002B014E
    //##Documentation
    //## 返回ServInfo的指针，
    //## 这个地方考虑来考虑去，
    //## 暂时先加上这个方法
    ServInfo *getServInfo();
    //##ModelId=4271EA3D0051
    //##Documentation
    //## 返回CustInfo的指针，
    //## 这个地方考虑来考虑去，
    //## 暂时先加上这个方法
    CustInfo *getCustInfo();

    char * getAttr(int attrid, char *sDate = 0);
	char * getAttrNew(int attrid, char * sDate, char * m_sValue, int iSize);
    int getAttrs(vector<UserInfoAttr> &ret);


	bool isAttrValue(int attrid, char *sValue, char *sDate=0);
	bool isAttrValueMax(int attrid, char *sValue, char *sDate=0);

    int getAttrNumber(int attrid, char *sDate=0);

    void setTime(char *sDate);
    
    int getFreeType();
    //取用户所有的商品,包括基本商品
    int getOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER],char *sDate=0);
    //取用户的基本商品
    int getBaseOfferIns(OfferInsQO * ret, char *sDate = 0);
    //判断用户是否有某个商品
    bool hasOffer (int iOfferID, char *sDate = 0);
    //返回基本商品标识,找不到返回-1
    int getBaseOfferID(char *sDate = 0);
    int getOfferInsQO(OfferInsQO& ret,long lOfferInstID); 
    // 在指定的时间账期内，取用户在本账期所有生效的商品，包括基本商品
    int getOfferInsBillingCycle(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], char *sDate);
    
		bool isServAttrChanged(const char *sBeginTime,const char *sEndTime);
		
		bool isServStateAttrChanged(const char *sBeginTime,const char *sEndTime);
		
		bool isServTypeChanged(const char *sBeginTime,const char *sEndTime);
		
		bool isServProductChanged(const char *sBeginTime,const char *sEndTime);
		
		bool isServProductAttrChanged(const char *sBeginTime,const char *sEndTime);
		
		bool isProdOffer_Ins_Attr_Detail_Changed(const char *sBeginTime,const char *sEndTime);
		
		bool isServChanged(const char *sBeginTime,const char *sEndTime);
    
  private:
    //##ModelId=4247F5370312
    char m_sTime[16];

    //##ModelId=425A2B880167
    ServInfo *m_poServInfo;

    //##ModelId=425A2B88016E
    CustInfo *m_poCustInfo;

    //##ModelId=425A2B880178
    ServProductInfo *m_poServProductInfo;

    //##ModelId=425A2B88018C
    ServAcctInfo *m_poServAcctInfo;

};

//##ModelId=42414EDA00FC
//##Documentation
//## 目前先提供全局的遍列，默认get的是当前时间的serv
class ServIteration : public UserInfoReader
{
  friend class UserInfoInterface;

  public:
    //##ModelId=42414F0F010C
    bool next(Serv &serv);
    bool nextByOrg(Serv &serv);

    /*  当用 nextByOrg的时候， 可以使用如下接口：   */
    bool belongCurServLocation(char *sTime);

  private:
    //##ModelId=424A00FD01A9
    unsigned int m_iCurOffset;

    unsigned int m_iLastOffset;


    //##ModelId=424A01060134
    char m_sTime[16];

};

class CustIteration : public UserInfoReader
{
  public:
	friend class UserInfoInterface;

  public:
	bool next(long & lCustID);
	
  private:
	unsigned int m_iCurOffset;

};

class ServGroupIteration : public UserInfoReader
{
  public:
	friend class UserInfoInterface;

  public:
	bool next(long & lGroupID);

  private:
	unsigned int m_iCurOffset;
};

//##ModelId=41D259580067
//##Documentation
//## 用户资料读取接口
//## 目前，哪块东西放在哪个共享内存上先写死，后面可以考虑升级到用可配置
class UserInfoInterface : public UserInfoReader
{
  public:
    //##ModelId=42353FFC014D
    //##Documentation
    //## 获取是否可用
    operator bool();

    static bool getAcctBelongOrg(long lAcctID, int iOrgID);
    static bool getCustBelongOrg(long lCustID, int iOrgID);
    static bool getServGroupBelongOrg(int iServGroupID, int iOrgID);

    //##ModelId=4232A42F02B3
    //##Documentation
    //## 根据业务号码，找出主产品实例相关客户资料。（现在不考虑某时刻，一个业务号码对应多个主产品实例的情况
    static bool getServ(
        //##Documentation
        //## 输出结果到的对象
        Serv &ret, 
        //##Documentation
        //## 业务号码
        char *accnbr, 
        //##Documentation
        //## 时间
        //## yyyymmddhhmiss
        char stime[16], 
        int pNetworkID = 0,
        bool isPhy=false) ;

    ////封装：先把号码和区号拼起来找，找不到的话直接使用号码找
    static bool getServ_A(
        //## 输出结果到的对象
        Serv &ret, 
        //业务号码的区号
        char *area_code,        
        //## 业务号码
        char *accnbr, 
        //## 时间 yyyymmddhhmiss
        char stime[16], 
        int pNetworkID = 0,
        bool isPhy=false) ;

	////封装：对于对端号码不使用网络标识，先根据号码来找，然后把号码和区号拼起来找，以后的所有对端查找全部用这个方法
    static bool getServ_Z(
        //## 输出结果到的对象
        Serv &ret,
        //业务号码的区号
        char *area_code,       
        //## 业务号码
        char *accnbr,
        //## 时间 yyyymmddhhmiss
        char stime[16],
        bool isPhy=false);

	//根据业务号码找出一机双号的另一个用户
	static bool getServ(
        //输出结果到的对象
        Serv &ret, 
        //业务号码
        char *accnbr, 
		//serv_Id
		long lServID,
        //时间 yyyymmddhhmiss
        char stime[16],
        bool isPhy=false);

    static char const * convertAccNBR(char *accnbr, int iSrcType, int iDestType, char *stime);

    static bool getServProduct(long lServProductID, ServProduct & servProduct);

    //##ModelId=423677970042
    //##Documentation
    //## 根据SERVID，找出主产品实例相关客户资料。
    static bool getServ(
        //##Documentation
        //## 输出结果到的对象
        Serv &ret, 
        //##Documentation
        //## 业务号码
        long servid, 
        //##Documentation
        //## 时间
        //## yyyymmddhhmiss
        char stime[16]) ;
    
    
    //## 根据号码/号码类型 查找用户资料
    static bool getServ (
        //## 输出结果到的对象
        Serv &ret,
        //## 号码
        char *accnbr,
        //## 号码类型
        int  iType,
        //## 时间
        char *stime
    );

	//## 根据号码/号码类型、网络标识 查找用户资料
    static bool getServN (
        //## 输出结果到的对象
        Serv &ret,
        //## 号码
        char *accnbr,
        //## 号码类型
        int  iType,
        //## 时间
        char *stime,
		//标络标识
		int pNetworkID = 0
    );
    
    static int getCustServ(long lCustID, vector<long> &vServ);
    static int getGroupServ(long lGroupID, vector<long> &vServ, char *sDate);
    static int getCustAcct(long lCustID, vector<long> &vAcct);

    static int getAcctCreditAmount(long lAcctID);
    static int getAcctCreditGrade(long lAcctID);

    //##ModelId=4234E8030031
    //##Documentation
    //## 检查那些静态变量有没有被初始化，如果没有被初始化，进行初始化（连接共享内存）。
    UserInfoInterface();
	UserInfoInterface(int iMode);

    //##ModelId=42414DB202A5
    static bool getServIteration(ServIteration &servIteration);

    static bool getServIterByOrg(ServIteration &servIter, int iOrgID);

    static bool getCustIteration(CustIteration &custIteration);

    static bool getServGroupIteration(ServGroupIteration &servGroupIteration);
    
    static int getServOrg(long lServID, char * sDate);
   // int getCustomMadeServ(vector<long> &ret, int iObjectID, unsigned int iCustPlanOffset, int iAcctItem=0); 

    long getCustomParamValue (int iParamID, unsigned int iCustPlanOffset);

    int getCustomPricingPlans(ExPricingPlan ret[MAX_PRICINGPLANS],int iObjectType,long lObjectID,char *sDate);

    long getCustomObjectID(int iPlanObjectID, unsigned int iCustPlanOffset);

	bool isBindServObject(long lServID, unsigned int iCustPlanOffset);

    char const *getCustomPlanEffDate(unsigned int iCustPlanOffset);

    CustPricingObjectInfo const * getCustomObject(int iPlanObjectID, unsigned int iCustPlanOffset);

    int getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, char * sDate=0);
	int getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, StdEvent * poEvent);

    CustPricingTariffInfo * getCustTariff(unsigned int iCustPlanOffset, char *sCalledNBR);
    
    //cdk增加商品实例的查询
    //获得帐户的商品实例
    int getAcctOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lAcctID, char *sDate);
    //获得客户的商品实例
    int getCustOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lCustID, char *sDate);
    //根据商品实例标识返回自身和自身所属的商品实例
    int getAllOfferInsByID(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lProdOfferInsID, char *sDate);
    int getOfferInsByID(OfferInsQO& ret,long lProdOfferInstID,char* sDate); 
	//xueqt 根据商品实例标识，返回商品实例, 
    ProdOfferIns * getOfferIns(long lOfferInstID);

    //返回商品实例游标
    bool getOfferInsIteration(OfferInsIteration &offerinsiter);
    //判断商品实例属地信息
    bool isOfferInsBelongOrg(long lOfferInstID, int iOrgID, char * sDate);
    //##ModelId=4232B69901F3
    //##Documentation
    //## 获取某账目类型的付费账号
    long getAcctID(long lServID, int acctitemtype);
    long getAcctID(long lServID, int acctitemtype, char * sDate);
	long getAcctID(long lServID, int acctitemtype, StdEvent * poEvent);
		//返回NP表中信息
	//add by jx 2009-10-26
	static bool getNpInfo(NpInfo &oNpInfo,char *sAccNbr,char *sDate = 0);
	//清理保存在内存中的serv信息
	static bool clearServ(Serv &cur);



#ifdef GROUP_CONTROL
	bool checkAccNbrOfferType(char * pAccNbr,char * pStartDate,int iOfferType);
	bool getAccNbrOfferType(char* pAccNbr,char* pStartDate,AccNbrOfferType& oAccNbrOfferType);
#endif

#ifdef GROUP_CONTROL
	bool getGroupInfoByID(GroupInfo &groupInfo,int iGroupID,char *sDate=0);
	int getGroupMemberByAccNbr(GroupMember ret[GROUP_NUM],char * pAccNbr,char *sDate=0);
	int getNumberGroupByAccNbr(NumberGroup ret[GROUP_NUM],char * pAccNbr,char *sDate=0);
	int getGroupResult(GroupInfoResult ret[GROUP_NUM],char * pCAccNbr,char * pGAccNbr,char * sDate=0);
#endif

#ifdef	IMSI_MDN_RELATION
	bool getImsiMdnRelationByImsi(ImsiMdnRelation &oRelation, char *sImsi,long iType, const char *sDate=0);
	void showImsiMdnRelationByImsi(char *sImsi, const char *sDate=0);
	void showAllImsiMdnRelation();
#endif
#ifdef	IMMEDIATE_VALID
	int getRollbackInstByServ(long lServID,OfferInsQO * pOfferBuf, int iOfferNum,
		long ret[MAX_OFFER_INSTANCE_NUMBER],const char *sDate=0);
	bool updateRollbackInstByServ(long lServID,long lInstID);
#endif	
};


#endif /* USERINFOREADER_H_HEADER_INCLUDED_BDB745F9 */
