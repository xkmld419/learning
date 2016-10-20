/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef USERINFOMAP_H_HEADER_INCLUDED
#define USERINFOMAP_H_HEADER_INCLUDED

#include "UserInfo.h"
#include "SHMMapFile.h"

#define USERINFO_ATTACH_MAP_DATA(T, U, V, W) \
	printf("Userinfo file:%s begin!\n", V); \
	T = new SHMMapFile<U> (V, W); \
	if (!(T)) { \
		freeAll (); \
		THROW(MBC_UserInfo+1); \
	}else \
	    printf("Userinfo file:%s finish!\n", V); \
	if (!(T->exist())) { \
		m_bAttached = false; \
		printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
	}

#define USERINFO_ATTACH_MAP_DATA1(T, U, V) \
	T = new SHMMapFile<U> (V); \
	if (!(T)) { \
		freeAll (); \
		THROW(MBC_UserInfo+1); \
	} \
	if (!(T->exist())) { \
		m_bAttached = false; \
		printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
	}

#ifdef  USERINFO_REUSE_MODE
#define USERINFO_ATTACH_INT_IDX(X, Z, W, A) \
    X = new SHMIntHashIndex_A (Z); \
    if(!(X)){ \
        freeAll(); \
        THROW(MBC_UserInfo+1); \
    } \
    if(!(X->exist()) && A) { \
        m_bAttached = true; \
        X->create(W); \
    }else if((X->exist()) && A){ \
        m_bAttached = true; \
        X->remove(); \
        X->create(W); \
    }
    
#define USERINFO_ATTACH_STRING_IDX(X, Z, V, W, A) \
    X = new SHMStringTreeIndex_A (Z); \
    if(!(X)){ \
        freeAll(); \
        THROW(MBC_UserInfo+1); \
    } \
    if(!(X->exist()) && A){ \
        m_bAttached = true; \
        X->create(V, W); \
    }else if((X->exist()) && A){ \
        m_bAttached = true; \
        X->remove(); \
        X->create(V, W); \
    }
    
#else

#define USERINFO_ATTACH_INT_IDX(X, Z, W, A) \
	X = new SHMIntHashIndex (Z); \
	if (!(X)) { \
		freeAll (); \
		THROW(MBC_UserInfo+1); \
	} \
	if (!(X->exist()) && A) { \
		m_bAttached = true; \
		X->create(W); \
	}else if((X->exist()) && A){ \
        m_bAttached = true; \
        X->remove(); \
        X->create(W); \
    }

#define USERINFO_ATTACH_STRING_IDX(X, Z, V, W, A) \
	X = new SHMStringTreeIndex (Z); \
	if (!(X)) { \
		freeAll (); \
		THROW(MBC_UserInfo+1); \
	} \
	if (!(X->exist()) && A) { \
		m_bAttached = true; \
		X->create(V, W); \
	}else if((X->exist()) && A){ \
        m_bAttached = true; \
        X->remove(); \
        X->create(V, W); \
    }

#endif

#define USERINFO_COPY_IDX(A, B, C, D, E) \
	A = (char *)(B->getMemAddr()); \
	C = (char *)(D->getMemAddr()); \
	E = D->totalSize(); \
	memcpy(A, C, E);

#define USERINFO_FREE_MAP_DATA(X) \
	if (X) { \
		delete X; \
		X = 0; \
	}

/*struct pathFullName
{
	char sFilePath[128];
	
	char sCustName[128];  // CUST文件全路径
	char sProductName[128];  // PRODUCT文件全路径
	char sServName[128];  // SERV文件全路径
	char sServTypeName[128];  // SERVTYPE文件全路径
	char sServLocName[128];  // SERVLOCATION文件全路径
	char sServStateName[128];  // SERVSTATEATTR文件全路径
	char sServAttrName[128];  // SERVATTR文件全路径
    #ifdef IMSI_MDN_RELATION
	char sImsiMdnRelatName[128];  // IMSIMDNRELAT文件全路径
    #endif
	char sServIdenName[128];  // SERVIDEN文件全路径
	char sServProdName[128];  // SERVPROD文件全路径
	char sServProdAttrName[128];  // SERVPRODATTR文件全路径
	char sServAcctName[128];  // SERVACCT文件全路径
	char sAcctName[128];  // ACCT文件全路径
	#ifdef GROUP_CONTROL
	char sGroupInfoName[128];  // GROUPINFO文件全路径
	char sGroupMemberName[128];  // GROUPMEMBER文件全路径
	char sNumberGroupName[128];  // NUMBERGROUP文件全路径
	char sACCNbrOfferTypeName[128];  // ACCNBROFFERTYPE文件全路径
	#endif
	char sOfferInsName[128];	// OFFERINS文件全路径
	char sOfferInsAttrName[128];	// OFFERINSATTR文件全路径
	char sOfferInsDetailName[128];  // OFFERINSDETAIL文件全路径
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
#define NPINFOFILE_IDX              IpcKeyMgr::getIpcKey (-1,"SHM_NPINFOFILE_IDX")*/

const int ALL_DATA =             0;
const int ALL_INDEX =            1;

const int CUST_DATA =            2;
const int PROD_DATA =            3;
const int SERV_DATA =            4;
const int SERVTYPE_DATA =        5;
const int SERVLOC_DATA =         6;
const int SERVSTATE_DATA =       7;
const int SERVATTR_DATA =        8;
#ifdef IMSI_MDN_RELATION
const int IMSIMDN_DATA =         9;
const int SERVIDENT_DATA =      10;
const int SERVPROD_DATA =       11;
const int SERVPRODATTR_DATA =   12;
const int SERVACCT_DATA =       13;
const int ACCT_DATA =           14;

#ifdef GROUP_CONTROL
const int GROUP_DATA =          15;
const int GROUPMEMBER_DATA =    16;
const int NUMBERGROUP_DATA =    17;
const int ACCNBR_DATA =         18;
const int OFFERINS_DATA =       19;
const int OFFERINSATTR_DATA =   20;
const int OFFDETAILINS_DATA =   21;
const int OFFERINSAGGR_DATA =   22;
const int NP_DATA =             23;
#else
const int OFFERINS_DATA =       15;
const int OFFERINSATTR_DATA =   16;
const int OFFDETAILINS_DATA =   17;
const int OFFERINSAGGR_DATA =   18;
const int NP_DATA =             19;
#endif

#else

const int SERVIDENT_DATA =      9;
const int SERVPROD_DATA =       10;
const int SERVPRODATTR_DATA =   11;
const int SERVACCT_DATA =       12;
const int ACCT_DATA =           13;

#ifdef GROUP_CONTROL
const int GROUP_DATA =          14;
const int GROUPMEMBER_DATA =    15;
const int NUMBERGROUP_DATA =    16;
const int ACCNBR_DATA =         17;
const int OFFERINS_DATA =       18;
const int OFFERINSATTR_DATA =   19;
const int OFFDETAILINS_DATA =   20;
const int OFFERINSAGGR_DATA =   21;
const int NP_DATA =             22;
#else
const int OFFERINS_DATA =       14;
const int OFFERINSATTR_DATA =   15;
const int OFFDETAILINS_DATA =   16;
const int OFFERINSAGGR_DATA =   17;
const int NP_DATA =             18;
#endif

#endif

struct vectorData
{
    long lLen;
	int iOperType;
};

struct UserInfoManage
{
    pathFullName m_sFullName;
	UserInfoBase m_oUserInfoBase;
	int iOperMode[50];
};

class UserInfoMapBase
{
    friend class SHMFileCtl;
	
    public:
        UserInfoMapBase(pathFullName sFullName, UserInfoBase * poUserInfoBase = NULL, int iMode = 0);
		~UserInfoMapBase();
        operator bool();
        
        //long getSHMIdxSize(char * pSHM, bool bString = false);
        void setAllIndex();
		void setFileData(int iOperMode);
        
        //static int m_iThreadNum;
        UserInfoManage m_oUserInfoManage;
        
        void setAllData(int iMode, int * iOper);
        static bool sortFileLen(const vectorData & m1, const vectorData & m2);
        void assignData();
        
    protected:
        void bindData();
		void freeAll();
        
        static bool m_bAttached;
		
		//UserInfoBase *m_poUserInfoBase;
		
        static SHMMapFile<CustInfo> *m_poCustData;
        static SHMMapFile<ProductInfo> *m_poProductData;
        static SHMMapFile<ServInfo> *m_poServData;
        static SHMMapFile<ServTypeInfo> *m_poServTypeData;
        static SHMMapFile<ServLocationInfo> *m_poServLocationData;
        static SHMMapFile<ServStateInfo> *m_poServStateData;
        static SHMMapFile<ServAttrInfo> *m_poServAttrData;
        #ifdef IMSI_MDN_RELATION
        static SHMMapFile<ImsiMdnRelation> *m_poImsiMdnRelationData;
        #endif
        static SHMMapFile<ServIdentInfo> *m_poServIdentData;
        static SHMMapFile<ServProductInfo> *m_poServProductData;
        static SHMMapFile<ServProductAttrInfo> *m_poServProductAttrData;
        static SHMMapFile<ServAcctInfo> *m_poServAcctData;
        static SHMMapFile<AcctInfo> *m_poAcctData;
        #ifdef GROUP_CONTROL
        static SHMMapFile<GroupInfo> *m_poGroupInfoData;
        static SHMMapFile<GroupMember> *m_poGroupMemberData;
        static SHMMapFile<NumberGroup> *m_poNumberGroupData;
        static SHMMapFile<AccNbrOfferType> *m_poAccNbrData;
        #endif
        static SHMMapFile<ProdOfferIns> *m_poProdOfferInsData;
        static SHMMapFile<ProdOfferInsAttr> *m_poProdOfferInsAttrData;
        static SHMMapFile<OfferDetailIns> *m_poOfferDetailInsData;
		static SHMMapFile<ProdOfferInsAgreement> *m_poProdOfferInsAgreementData;
        static SHMMapFile<NpInfo> *m_poNpData;
        
        static CustInfo *m_poCust;
        static ProductInfo *m_poProduct;
        static ServInfo *m_poServ;
        //static ServTypeInfo *m_poServTypeMap;
        static ServLocationInfo *m_poServLocation;
        static ServStateInfo *m_poServState;
        static ServAttrInfo *m_poServAttr;
        #ifdef IMSI_MDN_RELATION
        static ImsiMdnRelation *m_poImsiMdnRelation;
        #endif
        static ServIdentInfo *m_poServIdent;
        static ServProductInfo *m_poServProduct;
        static ServProductAttrInfo *m_poServProductAttr;
        static ServAcctInfo *m_poServAcct;
        static AcctInfo *m_poAcct;
        #ifdef GROUP_CONTROL
        static GroupInfo *m_poGroupInfo;
        static GroupMember *m_poGroupMember;
        static NumberGroup *m_poNumberGroup;
        static AccNbrOfferType *m_poAccNbr;
        #endif
        static ProdOfferIns *m_poProdOfferIns;
        static ProdOfferInsAttr *m_poProdOfferInsAttr;
        static OfferDetailIns *m_poOfferDetailIns;
        static ProdOfferInsAgreement * m_poProdOfferInsAgreement;
        static NpInfo *m_poNp;
		
        static USERINFO_SHM_INT_INDEX_TYPE *m_poCustIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poProductIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poServIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poServLocationIndex;
        #ifdef IMSI_MDN_RELATION
        static USERINFO_SHM_STR_INDEX_TYPE *m_poImsiMdnRelationIndex;
        #endif
        static USERINFO_SHM_STR_INDEX_TYPE *m_poServIdentIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poServProductIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poServAcctIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poAcctIndex;
        #ifdef GROUP_CONTROL
        static USERINFO_SHM_INT_INDEX_TYPE *m_poGroupInfoIndex;
        static USERINFO_SHM_STR_INDEX_TYPE *m_poGroupMemberIndex;
        static USERINFO_SHM_STR_INDEX_TYPE *m_poNumberGroupIndex;
        
        static USERINFO_SHM_STR_INDEX_TYPE *m_poAccNbrIndex;
        static USERINFO_SHM_INT_INDEX_TYPE *m_poAccNbrItemIndex;
        #endif
        static USERINFO_SHM_INT_INDEX_TYPE * m_poProdOfferInsIndex;
        static USERINFO_SHM_INT_INDEX_TYPE * m_poOfferDetailInsOfferInsIndex;
		
		static USERINFO_SHM_STR_INDEX_TYPE *m_poNpIndex;
        
    private:
		//void createThread(/*int iOperMode*/);
        
        //void freeAll();
};
#endif
