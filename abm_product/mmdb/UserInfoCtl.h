/*VER: 7*/ 
#ifndef USERINFOCTL_H_HEADER_INCLUDED_BDB8AD3D
#define USERINFOCTL_H_HEADER_INCLUDED_BDB8AD3D
#include "UserInfo.h"
#include "OfferMgr.h"
#include "CSemaphore.h"
#include "TOCIQuery.h"

//用户档案数量定义常量，取自b_param_define
//1、对应于param_segment字段
#define USERINFO_SEGMENT "USERINFO_UPLOAD"
#define USERINFO_LIMIT "USERINFO_MEM_LIMIT"

//2、对应于param_code字段
//2.1 SERV
#define USERINFO_NUM_SERV "SERV"
//2.2 SERV_ATTR
#define USERINFO_NUM_SERVATTR "SERVATTR"
#define USERINFO_NUM_SERVIDENT "SERVIDENT"
#define USERINFO_NUM_SERVSTATEATTR "SERVSTATEATTR"
#define USERINFO_NUM_SERVLOCATION "SERVLOCATION"
#define USERINFO_NUM_ASERVTYPECHANGE "ASERVTYPECHANGE"
#define USERINFO_NUM_SERVPRODUCT "SERVPRODUCT"
#define USERINFO_NUM_SERVPRODUCTATTR "SERVPRODUCTATTR"
#define USERINFO_NUM_PRODOFFERINS "PRODOFFERINS"
#define USERINFO_NUM_PRODOFFERINSDETAIL "PRODOFFERINSDETAIL"
#define USERINFO_NUM_PRODOFFERINSATTR "PRODOFFERINSATTR"
//add by zhaoziwei
#define USERINFO_NUM_PRODOFFERINSAGREEMENT "PRODOFFERINSAGREEMENT"
//end
#define USERINFO_NUM_CUST "CUST"
#define USERINFO_NUM_ACCT "ACCT"
#define USERINFO_NUM_SERVACCT "SERVACCT"

#ifdef GROUP_CONTROL
#define USERINFO_NUM_GROUPINFO "GROUPINFO"
#define USERINFO_NUM_GROUPMEMBER "GROUPMEMBER"
#define USERINFO_NUM_NUMBERGROUP "NUMBERGROUP"
#endif
#define	USERINFO_NUM_ACC_NBR_OFFER_TYPE	"ACCNBROFFERTYPE"

#ifdef	IMSI_MDN_RELATION
#define	USERINFO_NUM_IMSI_MDN	"IMSI_MDN_RELATION_NUM"
#endif

//开发测试 add by xn 2009.10.22
#define USERINFO_NUM_NP "NP"
//用户状态更新的同步控制锁
#define SERV_STATE_LOCK     (IpcKeyMgr::getIpcKey(-1,"LOCK_ServState"))
//资料加载日志记录控制锁
#define SERV_LOAD_INSERT     (IpcKeyMgr::getIpcKey(-1,"LOCK_LogInsert"))
#define SERV_LOAD_UPDATE     (IpcKeyMgr::getIpcKey(-1,"LOCK_LogUpdate"))

#define CAL_COUNT(count) \
	(\
	(int ) \
		( \
			(count)>1000000 ? \
			((count)*1.05) : \
			( \
				(count)>100000? \
				((count)*1.1): \
				(\
					(count)>10000? \
					((count)*1.15) : \
					( \
						(count)>1000? \
						10100 : \
						2000 \
					) \
				) \
			)\
		) \
	 ) 

//#define CAL_COUNT(count) count+50

//##ModelId=42391D0B0025
//##Documentation
//## 维护共享内存中的用户资料
class UserInfoAdmin : public UserInfoBase
{
  public:
	bool isSuccess;

  public:
    //##ModelId=42391DCD02AF
    void init(bool bForce);
    UserInfoAdmin( );
    UserInfoAdmin(bool bForce );
     /**************************************************
     *   创建所有的共享内存空间，但是不上载资料       *
     *                                                *
     **************************************************/
    void createAll();

#ifdef  USERINFO_REUSE_MODE
    void revokeAll();
    template < template<class> class dataT,class Y ,template<class> class getT1,class Z1,template<class> class getT2,class Z2,template<class> class getT3,class Z3,template<class> class getT4,class Z4,template<class> class getT5,class Z5 >
    unsigned int tidyUserInfo(dataT<Y> * pData, const char * sParamCode,
    SHMIntHashIndex_A * pIdx1, getT1<Z1> * pGet1, int iGetOffset1, int iChkOffset1, 
    SHMIntHashIndex_A * pIdx2, getT2<Z2> * pGet2, int iGetOffset2, int iChkOffset2,
    SHMIntHashIndex_A * pIdx3, getT3<Z3> * pGet3, int iGetOffset3, int iChkOffset3,
    SHMIntHashIndex_A * pIdx4=0, getT4<Z4> * pGet4=(SHMData_A<int> *)0, int iGetOffset4=-1, int iChkOffset4=-1,
    SHMIntHashIndex_A * pIdx5=0, getT5<Z5> * pGet5=(SHMData_A<int> *)0, int iGetOffset5=-1, int iChkOffset5=-1);
    //void revokeOwe();

    template <class dataT >
    void printClassInfo( dataT * pClass, char const * sName);
    void printInfo();
    
    /////以下3个函数用来测试 servLocation数据的加载和删除和导出。
    void testLoadAndRevoke();
    void exportServLocationInfo_test(char const * sTable);
    void loadServLocationInfo_test( int iMode);
    void exportServIdentInfo_test(char const * sTable);
    void loadServIdentInfo_test(int iMode);
    void testRevoke();
 	void revokeOwe();

#endif

    //##ModelId=42391ED4025D
    //##Documentation
    //## 加载用户资料（从全量表）
    void load();
	void loadTable(const int iMode);
	void loadTableDB(const int iMode);
	void loadTableMEM(const int iMode);
	void unloadTable(const int iMode);
	bool cleanOffset(const int iMode);
	int  run();
	int  runDB();
	int  runMEM();
	int  setRunInfo(const int iMode,char * msg);
	void loadPrefix();
	void bindDataEx(const int iMode);
	

    //##ModelId=42391ED40299
    //##Documentation
    //## 卸载内存中用户资料
    void unload();

    //##ModelId=4243D18002F6
    void update();
    //##ModelId=424BE38D0031
    void updateCustInfo(CustInfo &data, bool isinsert,bool bLog=false);

    void updateAcctInfo(AcctInfo &data, bool isinsert,bool bLog=false);
  
    //##ModelId=424BE3A30322
    void updateCustGroupInfo(CustGroupInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3AD00EC
    void updateCustPricingPlanInfo(CustPricingPlanInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3C40012
    void updateProductInfo(ProductInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3CE0388
    void updateOfferInfo(OfferInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3D7009C
    void updateServInfo(ServInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3E30035
    void updateServLocationInfo(ServLocationInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3EC0377
    void updateServStateInfo(ServStateInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE3F60032
    void updateServIdentInfo(ServIdentInfo &data, bool isinsert,bool bLog=false);

  
    //##ModelId=424BE40101E7
    void updateServGroupInfo(ServGroupInfo &data, bool isinsert,bool bLog=false);

    void updateServGroupBaseInfo(ServGroupBaseInfo &data, bool isinsert=true,bool bLog=false);

  
    //##ModelId=424BE41302B5
    void updateServProductInfo(ServProductInfo &data, bool isinsert,bool bLog=false);

    //##ModelId=424BE41C03BC
    void updateServAcctInfo(ServAcctInfo &data, bool isinsert,bool bLog=false);

    void updateServAttrInfo(ServAttrInfo &data, bool isinsert,bool bLog=false);
  
    //##ModelId=424BE4270047
    void updateOfferInstanceInfo(InstanceDetailUpdateInfo &data, bool isinsert,bool bLog=false);
    //##ModelId=425771CC008D
    void updateServProductAttrInfo(ServProductAttrInfo &data, bool isinsert,bool bLog=false);

    void updateCustPricingParamInfo(CustPricingParamInfo &data, bool isinsert = true,bool bLog=false);
    void updateCustPricingObjectInfo(CustPricingObjectInfo &data, bool isinsert = true,bool bLog=false);
    void updateCustTariffInfo(CustPricingTariffInfo &data, char *sCalledAreaCode, bool isinsert=true,bool bLog=false);

    void updateServTypeInfo(ServTypeInfo &data, bool isinsert = true,bool bLog=false);
    
    void updateProdOfferIns(ProdOfferIns &data, bool isinsert = true,bool bLog=false);
    
    void updateProdOfferInsDetail(long lOfferInsID, OfferDetailIns &data, bool isinsert = true,bool bLog=false);
    
    void updateProdOfferInsAttr(long lOfferInsID, ProdOfferInsAttr &data, bool isinsert = true,bool bLog=false);
    //add by zhaoziwei
    void updateProdOfferInsAgreement(long lOfferInsID, ProdOfferInsAgreement &data, bool isinsert = true,bool bLog=false);
		//end
    void updateServInfoState(long lServID, char * sState, char * sDate,bool bLog=false);

#ifdef GROUP_CONTROL
	void updateGroupMember(GroupMember &data,bool isInsert = true,bool bLog = false);
	void updateNumberGroup(NumberGroup &data,bool isInsert = true,bool bLog = false);
#endif
#ifdef GROUP_CONTROL
	void updateAccNbrOfferType(AccNbrOfferType& data,bool isinsert = true,bool bLog=false);
#endif
#ifdef IMSI_MDN_RELATION
	void updateImsiMdnRelation(ImsiMdnRelation &data, bool isInsert=true, bool bLog=false);
#endif
#ifdef IMMEDIATE_VALID
	void updateServInstRelation(ServInstRelation &data, bool isInsert=true, bool bLog=false);
#endif

  protected:
    //##ModelId=4247900203A0
    void loadCustInfo();

    void loadAcctInfo();

    //##ModelId=42479018038D
    void loadCustGroupInfo();

    //##ModelId=424790220157
    void loadCustPricingPlanInfo();

    //##ModelId=4247902A0195
    void loadProductInfo();

    //##ModelId=424790320006
    void loadOfferInfo();

    //##ModelId=4247903802B7
    void loadServInfo();

    void loadServTypeInfo();

    //##ModelId=4247903F025D
    void loadServLocationInfo();

    //##ModelId=424790460311
    void loadServStateInfo();

    //##ModelId=4247904D03BC
    void loadServIdentInfo();

    //##ModelId=4247905500D8
    void loadServGroupInfo();

    void loadServGroupBaseInfo();

    void loadServAttrInfo();

    //##ModelId=4247906101C6
    void loadServProductInfo();

    //##ModelId=4247906C009F
    void loadServAcctInfo();

    //##ModelId=424790740029
    void loadOfferInstanceInfo();

    //##ModelId=4247907B0318
    void loadServPricingPlanInfo();

    //##ModelId=4249068903D7
    void sortCustPlan(unsigned int u1, unsigned int u2);
    //##ModelId=425771C00357
    void loadServProductAttrInfo();
	void loadServProductAttrInfoDB();
	void loadServProductAttrInfoMEM();

    void loadCustPricingParamInfo();
    void loadCustPricingObjectInfo();
    void loadCustPricingTariffInfo();
    
    void loadProdOfferIns();
    
    void loadProdOfferInsDetail();
	void loadProdOfferInsDetailDB();
	void loadProdOfferInsDetailMEM();
    
    void loadProdOfferInsAttr();
    //add by zhaoziwei
    void loadProdOfferInsAgreement();
    //end

#ifdef GROUP_CONTROL
	void loadAccNbrOfferType();
#endif
	bool getBillingCycleOffsetTime(int iOffset,char * strDate,TOCIQuery &qry);
	bool getBillingCycleOffsetTime(int iOffset,char * strDate);

	int getSeq(TOCIQuery &qry);
	bool insertLog(int iSeq,char const * mem_table ,char const * db_table,TOCIQuery &qry);
	bool updateLog(int iSeq,TOCIQuery &qry);

#ifdef GROUP_CONTROL
	void loadGroupInfo();
	void loadGroupMember();
	void loadNumberGroup();
#endif

#ifdef IMSI_MDN_RELATION
	void loadImsiMdnRelation();
#endif
public:
	//开发测试 add by xn 2009.10.22
	void loadNpInfo();
	void updateNpInfo(NpInfo &data, bool isInsert, bool bLog);
	//long getNpSeqID(TOCIQuery &qry);

private:
    
    static OfferMgr * m_poOfferMgr;
    static CSemaphore * m_poServStateLock;
	static CSemaphore * m_poServLoadInsertLock;
	static CSemaphore * m_poServLoadUpdateLock;

	int  m_iRunMode;
	char * strMsg;
    bool m_bForce;
    
	static bool m_bSPOK;
	static bool m_bSPAOK;
	static bool m_bPOIOK;
	static bool m_bPOIDOK;
	static bool m_bLogClose;
    //2010-10-15
    static bool m_bFilterServ;
};



#endif /* USERINFOCTL_H_HEADER_INCLUDED_BDB8AD3D */
