/*VER: 4*/ 
#include "UserInfo.h"
#include "stdarg.h"
#undef DEBUG

void dprintf(const char *sfmt, ...)
{
#ifdef DEBUG
	va_list ap;
	va_start(ap, sfmt);
	vprintf(sfmt, ap);
	va_end(ap);
#endif
}


//##静态变量

//##ModelId=42475E120131
bool UserInfoBase::m_bAttached = false;

//##ModelId=4247582700E2
USERINFO_SHM_DATA_TYPE<CustInfo> *UserInfoBase::m_poCustData = 0;

USERINFO_SHM_DATA_TYPE<CustPricingTariffInfo> *UserInfoBase::m_poCustTariffData = 0;
USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poCustTariffIndex = 0;

//##ModelId=424758460317
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poCustIndex = 0;

//##ModelId=42492E2300B3
CustInfo *UserInfoBase::m_poCust = 0;

//##ModelId=424758A301F9
////SHMData<CustAttrInfo> *UserInfoBase::m_poCustAttrData = 0;

USERINFO_SHM_DATA_TYPE<AcctInfo> * UserInfoBase::m_poAcctData = 0;

USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poAcctIndex = 0;

CustAttrInfo * UserInfoBase::m_poCustAttr = 0;

//##ModelId=424758860201
USERINFO_SHM_DATA_TYPE<CustGroupInfo> *UserInfoBase::m_poCustGroupData = 0;

//##ModelId=42492E4902FD
CustGroupInfo *UserInfoBase::m_poCustGroup = 0;

//##ModelId=4247586C039E
USERINFO_SHM_DATA_TYPE<CustPricingPlanInfo> *UserInfoBase::m_poCustPricingPlanData = 0;

USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poCustPricingPlanIndex = 0;

//##ModelId=424945490231
CustPricingPlanInfo *UserInfoBase::m_poCustPlan = 0;

//##ModelId=424758CC0252
USERINFO_SHM_DATA_TYPE<ProductInfo> *UserInfoBase::m_poProductData = 0;

//##ModelId=424758FB021D
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poProductIndex = 0;

//##ModelId=424948060351
ProductInfo *UserInfoBase::m_poProduct = 0;

//##ModelId=424758E1022A
USERINFO_SHM_DATA_TYPE<OfferInfo> *UserInfoBase::m_poOfferData = 0;

//##ModelId=42475909028C
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poOfferIndex = 0;

//##ModelId=4247593001D3
USERINFO_SHM_DATA_TYPE<ServInfo> *UserInfoBase::m_poServData = 0;

USERINFO_SHM_DATA_TYPE<ServTypeInfo> * UserInfoBase::m_poServTypeData = 0;

//##ModelId=4247593E0350
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poServIndex = 0;

//##ModelId=42492E400083
ServInfo *UserInfoBase::m_poServ = 0;

//##ModelId=424759500248
USERINFO_SHM_DATA_TYPE<ServLocationInfo> *UserInfoBase::m_poServLocationData = 0;

USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poServLocationIndex = 0;

//##ModelId=42492E570018
ServLocationInfo *UserInfoBase::m_poServLocation = 0;

//##ModelId=4247597A0360
USERINFO_SHM_DATA_TYPE<ServStateInfo> *UserInfoBase::m_poServStateData = 0;

//##ModelId=42492E70030D
ServStateInfo *UserInfoBase::m_poServState = 0;

//##ModelId=424759C600D5
USERINFO_SHM_DATA_TYPE<ServAttrInfo> *UserInfoBase::m_poServAttrData = 0;

ServAttrInfo *UserInfoBase::m_poServAttr = 0;

//##ModelId=4247598E0142
USERINFO_SHM_DATA_TYPE<ServIdentInfo> *UserInfoBase::m_poServIdentData = 0;

//##ModelId=424759FA01B6
USERINFO_SHM_STR_INDEX_TYPE *UserInfoBase::m_poServIdentIndex = 0;

//##ModelId=42492E7F003D
ServIdentInfo *UserInfoBase::m_poServIdent = 0;

//##ModelId=42475D610136
USERINFO_SHM_DATA_TYPE<ServGroupInfo> *UserInfoBase::m_poServGroupData = 0;

USERINFO_SHM_DATA_TYPE<ServGroupBaseInfo> * UserInfoBase::m_poServGroupBaseData = 0;

USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poServGroupBaseIndex = 0;

//##ModelId=42492ED60151
ServGroupInfo *UserInfoBase::m_poServGroup = 0;

//##ModelId=42475A1300B7
USERINFO_SHM_DATA_TYPE<ServProductInfo> *UserInfoBase::m_poServProductData = 0;

//##ModelId=42475A330212
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poServProductIndex = 0;

//##ModelId=42492E9A0317
ServProductInfo *UserInfoBase::m_poServProduct = 0;

//##ModelId=42475A4A0139
USERINFO_SHM_DATA_TYPE<ServProductAttrInfo> *UserInfoBase::m_poServProductAttrData = 0;

ServProductAttrInfo * UserInfoBase::m_poServProductAttr = 0;

//##ModelId=42475A6B0028
USERINFO_SHM_DATA_TYPE<ServAcctInfo> *UserInfoBase::m_poServAcctData = 0;

//##serv acct indexed by acctid
USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poServAcctIndex = 0;

//##ModelId=42492EA902C9
ServAcctInfo *UserInfoBase::m_poServAcct = 0;

//##ModelId=42475A9101DB
USERINFO_SHM_DATA_TYPE<OfferInstanceInfo> *UserInfoBase::m_poOfferInstanceData = 0;

//##ModelId=424BAF5B0150
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poOfferInstanceIndex = 0;

//##ModelId=42492EBD028B
OfferInstanceInfo *UserInfoBase::m_poOfferInstance = 0;

//##ModelId=42475D3902A1
USERINFO_SHM_DATA_TYPE<ServPricingPlanInfo> *UserInfoBase::m_poServPricingPlanData = 0;


USERINFO_SHM_DATA_TYPE<CustPricingParamInfo> * UserInfoBase::m_poCustPricingParamData = 0;

USERINFO_SHM_DATA_TYPE<CustPricingObjectInfo> * UserInfoBase::m_poCustPricingObjectData = 0;
#ifdef GROUP_CONTROL
USERINFO_SHM_DATA_TYPE<AccNbrOfferType>* UserInfoBase::m_poAccNbrData=0;
USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poAccNbrIndex=0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoBase::m_poAccNbrItemIndex=0;

AccNbrOfferType* UserInfoBase::m_poAccNbr=0;
#endif
    
AcctInfo * UserInfoBase::m_poAcct = 0;
    
//商品实例数据和索引
USERINFO_SHM_DATA_TYPE<ProdOfferIns> * UserInfoBase::m_poProdOfferInsData = 0;
ProdOfferIns * UserInfoBase::m_poProdOfferIns = 0;
USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poProdOfferInsIndex = 0;
USERINFO_SHM_DATA_TYPE<OfferDetailIns> * UserInfoBase::m_poOfferDetailInsData = 0;
OfferDetailIns * UserInfoBase::m_poOfferDetailIns = 0;
USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poOfferDetailInsOfferInsIndex = 0;
USERINFO_SHM_DATA_TYPE<ProdOfferInsAttr> * UserInfoBase::m_poProdOfferInsAttrData = 0;
ProdOfferInsAttr * UserInfoBase::m_poProdOfferInsAttr = 0;
//add by zhaoziwei
USERINFO_SHM_DATA_TYPE<ProdOfferInsAgreement> * UserInfoBase::m_poProdOfferInsAgreementData = 0;
ProdOfferInsAgreement * UserInfoBase::m_poProdOfferInsAgreement = 0;
//end

#ifdef GROUP_CONTROL
	GroupInfo * UserInfoBase::m_poGroupInfo = 0;
	USERINFO_SHM_DATA_TYPE<GroupInfo> * UserInfoBase::m_poGroupInfoData = 0;
	USERINFO_SHM_INT_INDEX_TYPE * UserInfoBase::m_poGroupInfoIndex = 0;
	GroupMember * UserInfoBase::m_poGroupMember = 0;
	USERINFO_SHM_DATA_TYPE<GroupMember> * UserInfoBase::m_poGroupMemberData = 0;
	USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poGroupMemberIndex = 0;
	NumberGroup * UserInfoBase::m_poNumberGroup = 0;
	USERINFO_SHM_DATA_TYPE<NumberGroup> * UserInfoBase::m_poNumberGroupData = 0;
	USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poNumberGroupIndex = 0;
#endif

#ifdef IMSI_MDN_RELATION
	ImsiMdnRelation * UserInfoBase::m_poImsiMdnRelation = 0;
	USERINFO_SHM_DATA_TYPE<ImsiMdnRelation> * UserInfoBase::m_poImsiMdnRelationData = 0;
	USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poImsiMdnRelationIndex = 0;
#endif

USERINFO_SHM_DATA_TYPE<NpInfo> * UserInfoBase::m_poNpData= 0;
USERINFO_SHM_STR_INDEX_TYPE * UserInfoBase::m_poNpIndex = 0;
NpInfo * UserInfoBase::m_poNp=0;
	

	
//##基类函数

//##ModelId=42452759033D
void UserInfoBase::loadSHMInfo()
{
}

UserInfoBase::UserInfoBase(pathFullName oFileName)
{
	if (m_bAttached)
		return;
    
	m_bAttached = true;	
	//##try to attach

	//##客户信息数据
	USERINFO_ATTACH_FILEDATA(m_poCustData, CustInfo, oFileName.sCustName);

	//##客户信息索引
	USERINFO_ATTACH_INT_INDEX(m_poCustIndex ,CUSTFILE_INFO_IDX);

    //##产品数据
    USERINFO_ATTACH_FILEDATA(m_poProductData, ProductInfo, oFileName.sProductName);

    //##产品索引
    USERINFO_ATTACH_INT_INDEX(m_poProductIndex, PRODFILE_INFO_IDX);

    //##主产品实例数据
    USERINFO_ATTACH_FILEDATA(m_poServData, ServInfo, oFileName.sServName);

    USERINFO_ATTACH_FILEDATA(m_poServTypeData, ServTypeInfo, oFileName.sServTypeName);

    //##主产品实例索引
    USERINFO_ATTACH_INT_INDEX(m_poServIndex, SERVFILE_INFO_IDX);

    //##主产品实例属性区
    USERINFO_ATTACH_FILEDATA(m_poServAttrData, ServAttrInfo, oFileName.sServAttrName);

    //##主产品实例地域属性数据
    USERINFO_ATTACH_FILEDATA(m_poServLocationData, ServLocationInfo, oFileName.sServLocName);

    USERINFO_ATTACH_INT_INDEX(m_poServLocationIndex, SERVLOCFILE_INFO_IDX);

    //##主产品实例状态数据
    USERINFO_ATTACH_FILEDATA(m_poServStateData, ServStateInfo, oFileName.sServStateName);

    //##主产品实例标识数据
    USERINFO_ATTACH_FILEDATA(m_poServIdentData, ServIdentInfo, oFileName.sServIdenName);

    //##主产品实例标识索引
    USERINFO_ATTACH_STRING_INDEX(m_poServIdentIndex, SERVIDENTFILE_INFO_IDX);

    //##附属产品实例数据
    USERINFO_ATTACH_FILEDATA(m_poServProductData, ServProductInfo, oFileName.sServProdName);

    //##附属产品实例索引
    USERINFO_ATTACH_INT_INDEX(m_poServProductIndex, SERVPRODFILE_INFO_IDX);

    //##附属产品实例属性数据
    USERINFO_ATTACH_FILEDATA(m_poServProductAttrData, ServProductAttrInfo, oFileName.sServProdAttrName);

    //##帐务定制关系数据
    USERINFO_ATTACH_FILEDATA(m_poServAcctData, ServAcctInfo, oFileName.sServAcctName);

    USERINFO_ATTACH_INT_INDEX (m_poServAcctIndex, SERVACCTFILE_ACCT_IDX);

    USERINFO_ATTACH_FILEDATA(m_poAcctData, AcctInfo, oFileName.sAcctName);

    USERINFO_ATTACH_INT_INDEX (m_poAcctIndex, ACCTFILE_INFO_IDX);
    USERINFO_ATTACH_FILEDATA(m_poProdOfferInsData, ProdOfferIns, oFileName.sOfferInsName);
    USERINFO_ATTACH_INT_INDEX(m_poProdOfferInsIndex, PRODOFFERINSFILE_IDX);
    USERINFO_ATTACH_FILEDATA(m_poOfferDetailInsData, OfferDetailIns, oFileName.sOfferInsDetailName);
    USERINFO_ATTACH_INT_INDEX(m_poOfferDetailInsOfferInsIndex, OFFERDETAILINS_IDX);
    USERINFO_ATTACH_FILEDATA(m_poProdOfferInsAttrData, ProdOfferInsAttr, oFileName.sOfferInsAttrName);
    USERINFO_ATTACH_FILEDATA(m_poProdOfferInsAgreementData, ProdOfferInsAgreement, oFileName.sOfferInsAgrName);

#ifdef GROUP_CONTROL
	USERINFO_ATTACH_FILEDATA(m_poGroupInfoData, GroupInfo, oFileName.sGroupInfoName);
	USERINFO_ATTACH_INT_INDEX(m_poGroupInfoIndex, GROUPFILE_INFO_IDX);
	USERINFO_ATTACH_FILEDATA(m_poGroupMemberData, GroupMember, oFileName.sGroupMemberName);
	USERINFO_ATTACH_STRING_INDEX(m_poGroupMemberIndex, GROUPFILE_MEMBER_IDX);
	USERINFO_ATTACH_FILEDATA(m_poNumberGroupData, NumberGroup, oFileName.sNumberGroupName);
    USERINFO_ATTACH_STRING_INDEX(m_poNumberGroupIndex, NUMBERFILE_GROUP_IDX);
#endif
#ifdef	GROUP_CONTROL
	USERINFO_ATTACH_FILEDATA(m_poAccNbrData,AccNbrOfferType, oFileName.sACCNbrOfferTypeName);
	USERINFO_ATTACH_STRING_INDEX(m_poAccNbrIndex, ACCNBROFFERTYPEFILE_IDX);
	USERINFO_ATTACH_INT_INDEX(m_poAccNbrItemIndex, ACCNBROFFERTYPEITEMFILE_IDX);
#endif
#ifdef IMSI_MDN_RELATION
	USERINFO_ATTACH_FILEDATA(m_poImsiMdnRelationData, ImsiMdnRelation, oFileName.sImsiMdnRelatName);
	USERINFO_ATTACH_STRING_INDEX(m_poImsiMdnRelationIndex, IMSIMDNRELATFILE_INFO_IDX);
#endif

	//开发测试 add by xn 2009.10.22
	USERINFO_ATTACH_FILEDATA(m_poNpData, NpInfo, oFileName.sNpName);
	USERINFO_ATTACH_STRING_INDEX(m_poNpIndex, NPINFOFILE_IDX);
	//结束
    //m_bAttached = true;//TODO only for testing...
	if (m_bAttached)
		bindData ();
}

//##ModelId=4247679B031B
UserInfoBase::UserInfoBase()
{
/*	
#undef ATTACH_DATA
#undef ATTACH_INT_INDEX
#undef ATTACH_STRING_INDEX 

	#define ATTACH_DATA(X, Y, Z) \
		X = new SHMData<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			dprintf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			dprintf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			dprintf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}
*/		
	if (m_bAttached)
		return;

	m_bAttached = true;	
	//##try to attach

	//##客户信息数据
	USERINFO_ATTACH_DATA(m_poCustData, CustInfo, CUST_INFO_DATA);

	//##客户信息索引
	USERINFO_ATTACH_INT_INDEX(m_poCustIndex ,CUST_INFO_INDEX);

	//##客户属性数据
//    ATTACH_DATA(m_poCustAttrData, CustAttrInfo, CUSTATTR_INFO_DATA);

	//##客户定价计划数据
//	ATTACH_DATA(m_poCustPricingPlanData, CustPricingPlanInfo, CUSTPRICINGPLAN_INFO_DATA);

//	ATTACH_INT_INDEX(m_poCustPricingPlanIndex, CUSTPRICINGPLAN_INFO_INDEX);

//  ATTACH_DATA(m_poCustTariffData, CustPricingTariffInfo, CUSTPRICINGTARIFF_INFO_DATA);

//  ATTACH_STRING_INDEX(m_poCustTariffIndex, CUSTPRICINGTARIFF_INFO_INDEX);


	//##客户群数据
//	ATTACH_DATA(m_poCustGroupData, CustGroupInfo, CUSTGROUP_INFO_DATA);

	//##产品数据
	USERINFO_ATTACH_DATA(m_poProductData, ProductInfo, PRODUCT_INFO_DATA);

	//##产品索引
	USERINFO_ATTACH_INT_INDEX(m_poProductIndex, PRODUCT_INFO_INDEX);

	//##商品数据
//	ATTACH_DATA(m_poOfferData, OfferInfo, OFFER_INFO_DATA);

	//##商品索引
//	ATTACH_INT_INDEX(m_poOfferIndex, OFFER_INFO_INDEX);

	//##主产品实例数据
	USERINFO_ATTACH_DATA(m_poServData, ServInfo, SERV_INFO_DATA);

	USERINFO_ATTACH_DATA(m_poServTypeData, ServTypeInfo, SERVTYPE_INFO_DATA);

	//##主产品实例索引
	USERINFO_ATTACH_INT_INDEX(m_poServIndex, SERV_INFO_INDEX);

	//##主产品实例属性区
	USERINFO_ATTACH_DATA(m_poServAttrData, ServAttrInfo, SERVATTR_INFO_DATA);

	//##主产品实例地域属性数据
	USERINFO_ATTACH_DATA(m_poServLocationData, ServLocationInfo, SERVLOCATION_INFO_DATA);

    USERINFO_ATTACH_INT_INDEX(m_poServLocationIndex, SERVLOCATION_INFO_INDEX);

	//##主产品实例状态数据
	USERINFO_ATTACH_DATA(m_poServStateData, ServStateInfo, SERVSTATE_INFO_DATA);

	//##主产品实例标识数据
	USERINFO_ATTACH_DATA(m_poServIdentData, ServIdentInfo, SERVIDENT_INFO_DATA);

	//##主产品实例标识索引
	USERINFO_ATTACH_STRING_INDEX(m_poServIdentIndex, SERVIDENT_INFO_INDEX);

	//##附属产品实例数据
	USERINFO_ATTACH_DATA(m_poServProductData, ServProductInfo, SERVPRODUCT_INFO_DATA);

	//##附属产品实例索引
	USERINFO_ATTACH_INT_INDEX(m_poServProductIndex, SERVPRODUCT_INFO_INDEX);

	//##附属产品实例属性数据
	USERINFO_ATTACH_DATA(m_poServProductAttrData, ServProductAttrInfo, SERVPRODUCTATTR_INFO_DATA);

	//##帐务定制关系数据
	USERINFO_ATTACH_DATA(m_poServAcctData, ServAcctInfo, SERVACCT_INFO_DATA);

	USERINFO_ATTACH_INT_INDEX (m_poServAcctIndex, SERVACCT_ACCT_INDEX);

	//##商品实例关系数据
//	ATTACH_DATA(m_poOfferInstanceData, OfferInstanceInfo, OFFERINSTANCE_INFO_DATA);

	//##商品实例关系索引
//	ATTACH_INT_INDEX(m_poOfferInstanceIndex, OFFERINSTANCE_INFO_INDEX);

//	ATTACH_DATA(m_poServPricingPlanData, ServPricingPlanInfo, SERVPRICINGPLAN_INFO_DATA);

	//##主产品实例群数据
//	ATTACH_DATA(m_poServGroupData, ServGroupInfo, SERVGROUP_INFO_DATA);

//	ATTACH_DATA(m_poServGroupBaseData, ServGroupBaseInfo, SERVGROUPBASE_INFO_DATA);

//	ATTACH_INT_INDEX (m_poServGroupBaseIndex, SERVGROUPBASE_INFO_INDEX);

//	ATTACH_DATA(m_poCustPricingParamData, CustPricingParamInfo, CUSTPRICINGPARAM_INFO_DATA);

//	ATTACH_DATA(m_poCustPricingObjectData, CustPricingObjectInfo, CUSTPRICINGOBJECT_INFO_DATA);

    USERINFO_ATTACH_DATA(m_poAcctData, AcctInfo, ACCT_INFO_DATA);

    USERINFO_ATTACH_INT_INDEX (m_poAcctIndex, ACCT_INFO_INDEX);
    //商品实例数据
    USERINFO_ATTACH_DATA(m_poProdOfferInsData, ProdOfferIns, PRODOFFERINS_DATA);
    USERINFO_ATTACH_INT_INDEX(m_poProdOfferInsIndex, PRODOFFERINS_INDEX);
    USERINFO_ATTACH_DATA(m_poOfferDetailInsData, OfferDetailIns, OFFERDETAILINS_DATA);
    USERINFO_ATTACH_INT_INDEX(m_poOfferDetailInsOfferInsIndex, OFFERDETAILINS_INSIDOFFER_INDEX);
    USERINFO_ATTACH_DATA(m_poProdOfferInsAttrData, ProdOfferInsAttr, PRODOFFERINSATTR_DATA);
    //add by zhaoziwei
    USERINFO_ATTACH_DATA(m_poProdOfferInsAgreementData, ProdOfferInsAgreement, PRODOFFERINSAGREEMENT_DATA);
    //end

#ifdef GROUP_CONTROL
	USERINFO_ATTACH_DATA(m_poGroupInfoData, GroupInfo, GROUP_INFO_DATA);
	USERINFO_ATTACH_INT_INDEX(m_poGroupInfoIndex, GROUP_INFO_INDEX);
	USERINFO_ATTACH_DATA(m_poGroupMemberData, GroupMember, GROUP_MEMBER_DATA);
	USERINFO_ATTACH_STRING_INDEX(m_poGroupMemberIndex, GROUP_MEMBER_INDEX);
	USERINFO_ATTACH_DATA(m_poNumberGroupData, NumberGroup, NUMBER_GROUP_DATA);
    USERINFO_ATTACH_STRING_INDEX(m_poNumberGroupIndex, NUMBER_GROUP_INDEX);
#endif
#ifdef	GROUP_CONTROL
	USERINFO_ATTACH_DATA(m_poAccNbrData,AccNbrOfferType,ACCNBROFFERTYPE_DATA);
	USERINFO_ATTACH_STRING_INDEX(m_poAccNbrIndex,ACCNBROFFERTYPE_INDEX);
	USERINFO_ATTACH_INT_INDEX(m_poAccNbrItemIndex,SHM_ACCNBROFFERTYPE_ITEM_INDEX);
#endif
#ifdef IMSI_MDN_RELATION
	USERINFO_ATTACH_DATA(m_poImsiMdnRelationData, ImsiMdnRelation, IMSI_MDN_RELATION_DATA);
	USERINFO_ATTACH_STRING_INDEX(m_poImsiMdnRelationIndex, IMSI_MDN_RELATION_INDEX);
#endif

	//开发测试 add by xn 2009.10.22
	USERINFO_ATTACH_DATA(m_poNpData, NpInfo, NP_INFO_DATA);
	USERINFO_ATTACH_STRING_INDEX(m_poNpIndex, NP_INFO_INDEX);
	//结束
    //m_bAttached = true;//TODO only for testing...
	if (m_bAttached)
		bindData ();
}

void UserInfoBase::closeAll()
{
    #define CLOSE(X) \
		if(X) \
			X->close();
    
    CLOSE(m_poCustData);
	CLOSE(m_poCustIndex);
	CLOSE(m_poProductData);
	CLOSE(m_poProductIndex);
	CLOSE(m_poServData);
	CLOSE(m_poServIndex);
	CLOSE(m_poServAttrData);
	CLOSE(m_poServTypeData);
	CLOSE(m_poServLocationData);
    CLOSE(m_poServLocationIndex);
	CLOSE(m_poServStateData);
	CLOSE(m_poServIdentData);
	CLOSE(m_poServIdentIndex);
	CLOSE(m_poServProductData);
	CLOSE(m_poServProductIndex);
	CLOSE(m_poServProductAttrData);
	CLOSE(m_poServAcctData);
	CLOSE(m_poServAcctIndex);
	CLOSE(m_poAcctData);
    CLOSE(m_poAcctIndex);
    CLOSE(m_poProdOfferInsData);
    CLOSE(m_poProdOfferInsIndex);
    CLOSE(m_poOfferDetailInsData);
    CLOSE(m_poOfferDetailInsOfferInsIndex);
    CLOSE(m_poProdOfferInsAttrData);
    CLOSE(m_poProdOfferInsAgreementData);
    
#ifdef GROUP_CONTROL
	CLOSE(m_poGroupInfoData);
	CLOSE(m_poGroupInfoIndex);
	CLOSE(m_poGroupMemberData);
	CLOSE(m_poGroupMemberIndex);
	CLOSE(m_poNumberGroupData);
	CLOSE(m_poNumberGroupIndex);
#endif
#ifdef GROUP_CONTROL
    CLOSE(m_poAccNbrData);
	CLOSE(m_poAccNbrIndex);
	CLOSE(m_poAccNbrItemIndex);
#endif
#ifdef IMSI_MDN_RELATION
	CLOSE(m_poImsiMdnRelationData);
	CLOSE(m_poImsiMdnRelationIndex);
#endif
    CLOSE(m_poNpData);
    CLOSE(m_poNpIndex);
}

//##ModelId=4247790103D4
void UserInfoBase::freeAll()
{
	#define FREE(X) \
		if (X) { \
			delete X; \
			X = 0; \
		}

	FREE(m_poCustData);

	FREE(m_poCustIndex);

//	FREE(m_poCustAttrData);

//	FREE(m_poCustPricingPlanData);

//	FREE(m_poCustPricingPlanIndex);

//  FREE(m_poCustTariffData);

//  FREE(m_poCustTariffIndex);

//	FREE(m_poCustGroupData);

	FREE(m_poProductData);

	FREE(m_poProductIndex);

//	FREE(m_poOfferData);

//	FREE(m_poOfferIndex);

	FREE(m_poServData);

	FREE(m_poServIndex);

	FREE(m_poServAttrData);

	FREE(m_poServTypeData);

	FREE(m_poServLocationData);

    FREE(m_poServLocationIndex);

	FREE(m_poServStateData);

	FREE(m_poServIdentData);

	FREE(m_poServIdentIndex);

	FREE(m_poServProductData);

	FREE(m_poServProductIndex);

	FREE(m_poServProductAttrData);

	FREE(m_poServAcctData);

	FREE(m_poServAcctIndex);

//	FREE(m_poOfferInstanceData);

//	FREE(m_poOfferInstanceIndex);

//	FREE(m_poServPricingPlanData);

//	FREE(m_poServGroupData);

//	FREE(m_poServGroupBaseData);

//	FREE(m_poServGroupBaseIndex);

//	FREE(m_poCustPricingParamData);

//	FREE(m_poCustPricingObjectData);

	FREE(m_poAcctData);

    FREE(m_poAcctIndex);
    
    //商品实例数据
    FREE(m_poProdOfferInsData);
    FREE(m_poProdOfferInsIndex);
    FREE(m_poOfferDetailInsData);
    FREE(m_poOfferDetailInsOfferInsIndex);
    FREE(m_poProdOfferInsAttrData);
    //add by zhaoziwei
    FREE(m_poProdOfferInsAgreementData);
    //end
    
#ifdef GROUP_CONTROL
	FREE(m_poGroupInfoData);
	FREE(m_poGroupInfoIndex);
	FREE(m_poGroupMemberData);
	FREE(m_poGroupMemberIndex);
	FREE(m_poNumberGroupData);
	FREE(m_poNumberGroupIndex);
#endif
#ifdef GROUP_CONTROL
    FREE(m_poAccNbrData);
	FREE(m_poAccNbrIndex);
	FREE(m_poAccNbrItemIndex);
#endif
#ifdef IMSI_MDN_RELATION
	FREE(m_poImsiMdnRelationData);
	FREE(m_poImsiMdnRelationIndex);
#endif
       //开发测试 add by xn 2009.10.22
  FREE(m_poNpData);
  FREE(m_poNpIndex);
  //结束  
}

//##ModelId=424930FE0349
void UserInfoBase::bindData()
{
	m_poCust = (CustInfo *)(*m_poCustData);
	//---m_poCustAttr = (CustAttrInfo *)(*m_poCustAttrData);
	m_poServIdent = (ServIdentInfo *)(*m_poServIdentData);
	m_poServLocation = (ServLocationInfo *)(*m_poServLocationData);
	m_poServ = (ServInfo *)(*m_poServData);
//	m_poServGroup = (ServGroupInfo *) (*m_poServGroupData);
//	m_poCustGroup = (CustGroupInfo *)(*m_poCustGroupData);
	m_poServState = (ServStateInfo *)(*m_poServStateData);
	m_poServAttr = (ServAttrInfo *)(*m_poServAttrData);
	m_poServProduct = (ServProductInfo *)(*m_poServProductData);
	m_poServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
	m_poServAcct = (ServAcctInfo *)(*m_poServAcctData);
	m_poAcct = (AcctInfo *)(*m_poAcctData);
//	m_poOfferInstance = (OfferInstanceInfo *)(*m_poOfferInstanceData);
//	m_poCustPlan = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);
	m_poProduct = (ProductInfo *)(*m_poProductData);
	//商品实例数据
	m_poProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);
	m_poOfferDetailIns = (OfferDetailIns *)(*m_poOfferDetailInsData);
	m_poProdOfferInsAttr = (ProdOfferInsAttr *)(*m_poProdOfferInsAttrData);
	//add by zhaoziwei
	m_poProdOfferInsAgreement = (ProdOfferInsAgreement *)(*m_poProdOfferInsAgreementData);
	//end

#ifdef GROUP_CONTROL
	m_poGroupInfo   = (GroupInfo *)(*m_poGroupInfoData);
	m_poGroupMember = (GroupMember *)(*m_poGroupMemberData);
	m_poNumberGroup = (NumberGroup *)(*m_poNumberGroupData);
#endif
#ifdef GROUP_CONTROL
	m_poAccNbr = (AccNbrOfferType*)(*m_poAccNbrData);
#endif
#ifdef IMSI_MDN_RELATION
	m_poImsiMdnRelation = (ImsiMdnRelation *)(*m_poImsiMdnRelationData);
#endif

	//开发测试 add by xn 2009.10.22
	m_poNp = (NpInfo*) (*m_poNpData);
	//结束
}


//##ModelId=424A148D0363
UserInfoBase::operator bool()
{
	return m_bAttached;
}

