/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef USERINFOSHMFILE_H_HEADER_INCLUDED
#define USERINFOSHMFILE_H_HEADER_INCLUDED
#include <dirent.h>
#include "UserInfoMap.h"
#include "ReadIni.h"

const int EXTERNAL_INTERFACE_STATE = 0;
const int FILE_POINT_STATE = 1;
const int SHM_POINT_STATE = 2;
const int CHANGE_POINT_STATE = 3;

const int PROCE_NUM_MAX = 2000;

#define DETACHSHM(X) \
    if(X) \
        if (X->exist()){ \
            X->remove (); \
        }

struct SHMPointer
{
    USERINFO_SHM_DATA_TYPE<CustInfo> *m_poCustData;
    USERINFO_SHM_DATA_TYPE<ProductInfo> *m_poProductData;
    USERINFO_SHM_DATA_TYPE<ServInfo> *m_poServData;
	USERINFO_SHM_DATA_TYPE<ServTypeInfo> * m_poServTypeData;
	USERINFO_SHM_DATA_TYPE<ServLocationInfo> *m_poServLocationData;
	USERINFO_SHM_DATA_TYPE<ServStateInfo> *m_poServStateData;
	USERINFO_SHM_DATA_TYPE<ServAttrInfo> *m_poServAttrData;
	#ifdef IMSI_MDN_RELATION
	USERINFO_SHM_DATA_TYPE<ImsiMdnRelation> * m_poImsiMdnRelationData;
	#endif
	USERINFO_SHM_DATA_TYPE<ServIdentInfo> *m_poServIdentData;
	USERINFO_SHM_DATA_TYPE<ServProductInfo> *m_poServProductData;
	USERINFO_SHM_DATA_TYPE<ServProductAttrInfo> * m_poServProductAttrData;
	USERINFO_SHM_DATA_TYPE<ServAcctInfo> *m_poServAcctData;
	USERINFO_SHM_DATA_TYPE<AcctInfo> * m_poAcctData;
	#ifdef GROUP_CONTROL
	USERINFO_SHM_DATA_TYPE<GroupInfo> *m_poGroupInfoData;
	USERINFO_SHM_DATA_TYPE<GroupMember> *m_poGroupMemberData;
	USERINFO_SHM_DATA_TYPE<NumberGroup> *m_poNumberGroupData;
	USERINFO_SHM_DATA_TYPE<AccNbrOfferType>* m_poAccNbrData;
	#endif
	USERINFO_SHM_DATA_TYPE<ProdOfferIns> * m_poProdOfferInsData;
	USERINFO_SHM_DATA_TYPE<ProdOfferInsAttr> * m_poProdOfferInsAttrData;
	USERINFO_SHM_DATA_TYPE<OfferDetailIns> * m_poOfferDetailInsData;
	USERINFO_SHM_DATA_TYPE<ProdOfferInsAgreement> * m_poProdOfferInsAgreementData;
    
	USERINFO_SHM_DATA_TYPE<OfferInfo> *m_poOfferData;
    USERINFO_SHM_DATA_TYPE<ServGroupBaseInfo> * m_poServGroupBaseData;
	USERINFO_SHM_DATA_TYPE<CustPricingTariffInfo> *m_poCustTariffData;
	USERINFO_SHM_DATA_TYPE<CustPricingObjectInfo> *m_poCustPricingObjectData;
	USERINFO_SHM_DATA_TYPE<CustPricingParamInfo>  *m_poCustPricingParamData;
    USERINFO_SHM_DATA_TYPE<NpInfo>  *m_poNpData;
	
	//publicPointer oPublicPointer;
	CustInfo *m_poCust;
	USERINFO_SHM_INT_INDEX_TYPE *m_poCustIndex;
	ProductInfo *m_poProduct;
	USERINFO_SHM_INT_INDEX_TYPE *m_poProductIndex;
	ServInfo *m_poServ;
    USERINFO_SHM_INT_INDEX_TYPE *m_poServIndex;
    ServLocationInfo *m_poServLocation;
    ServStateInfo *m_poServState;
	ServAttrInfo * m_poServAttr;
	#ifdef IMSI_MDN_RELATION
	ImsiMdnRelation * m_poImsiMdnRelation;
	USERINFO_SHM_STR_INDEX_TYPE* m_poImsiMdnRelationIndex;
	#endif
    ServIdentInfo *m_poServIdent;
	USERINFO_SHM_STR_INDEX_TYPE *m_poServIdentIndex;
	ServProductInfo *m_poServProduct;
	USERINFO_SHM_INT_INDEX_TYPE *m_poServProductIndex;
	ServProductAttrInfo * m_poServProductAttr;
    ServAcctInfo *m_poServAcct;
	USERINFO_SHM_INT_INDEX_TYPE * m_poServAcctIndex;
	AcctInfo * m_poAcct;
	USERINFO_SHM_INT_INDEX_TYPE * m_poAcctIndex;
	#ifdef GROUP_CONTROL
	GroupInfo * m_poGroupInfo;
	USERINFO_SHM_INT_INDEX_TYPE *m_poGroupInfoIndex;
	GroupMember * m_poGroupMember;
	USERINFO_SHM_STR_INDEX_TYPE *m_poGroupMemberIndex;
	NumberGroup * m_poNumberGroup;
	USERINFO_SHM_STR_INDEX_TYPE *m_poNumberGroupIndex;
	AccNbrOfferType* m_poAccNbr;
	USERINFO_SHM_INT_INDEX_TYPE *m_poAccNbrItemIndex;
	#endif
	ProdOfferIns * m_poProdOfferIns;
	USERINFO_SHM_INT_INDEX_TYPE * m_poProdOfferInsIndex;
	ProdOfferInsAttr * m_poProdOfferInsAttr;
	OfferDetailIns * m_poOfferDetailIns;
	USERINFO_SHM_INT_INDEX_TYPE * m_poOfferDetailInsOfferInsIndex;
	ProdOfferInsAgreement * m_poProdOfferInsAgreement;
	
	OfferInstanceInfo *m_poOfferInstance;
	CustPricingPlanInfo *m_poCustPlan;
	ServGroupInfo *m_poServGroup;
	USERINFO_SHM_INT_INDEX_TYPE *m_poOfferIndex;
	USERINFO_SHM_INT_INDEX_TYPE *m_poServGroupBaseIndex;
	USERINFO_SHM_STR_INDEX_TYPE *m_poCustTariffIndex;
	
	NpInfo *m_poNp;
	USERINFO_SHM_STR_INDEX_TYPE *m_poNpIndex;
};

struct FilePointer
{
    SHMMapFile<CustInfo> *m_poCustData;
	SHMMapFile<ProductInfo> *m_poProductData;
	SHMMapFile<ServInfo> *m_poServData;
	SHMMapFile<ServTypeInfo> * m_poServTypeData;
	SHMMapFile<ServLocationInfo> *m_poServLocationData;
	SHMMapFile<ServStateInfo> *m_poServStateData;
	SHMMapFile<ServAttrInfo> *m_poServAttrData;
#ifdef IMSI_MDN_RELATION
	SHMMapFile<ImsiMdnRelation> * m_poImsiMdnRelationData;
#endif
	SHMMapFile<ServIdentInfo> *m_poServIdentData;
	SHMMapFile<ServProductInfo> *m_poServProductData;
	SHMMapFile<ServProductAttrInfo> * m_poServProductAttrData;
	SHMMapFile<ServAcctInfo> *m_poServAcctData;
	SHMMapFile<AcctInfo> * m_poAcctData;
#ifdef GROUP_CONTROL
	SHMMapFile<GroupInfo> *m_poGroupInfoData;
	SHMMapFile<GroupMember> *m_poGroupMemberData;
	SHMMapFile<NumberGroup> *m_poNumberGroupData;
	SHMMapFile<AccNbrOfferType>* m_poAccNbrData;
#endif
	SHMMapFile<ProdOfferIns> * m_poProdOfferInsData;
	SHMMapFile<ProdOfferInsAttr> * m_poProdOfferInsAttrData;
	SHMMapFile<OfferDetailIns> * m_poOfferDetailInsData;
	SHMMapFile<ProdOfferInsAgreement> * m_poProdOfferInsAgreementData;
    
	SHMMapFile<OfferInfo> *m_poOfferData;
    SHMMapFile<ServGroupBaseInfo> * m_poServGroupBaseData;
	SHMMapFile<CustPricingTariffInfo> *m_poCustTariffData;
	SHMMapFile<CustPricingObjectInfo> *m_poCustPricingObjectData;
	SHMMapFile<CustPricingParamInfo>  *m_poCustPricingParamData;
    SHMMapFile<NpInfo>  *m_poNpData;
	
	//publicPointer oPublicPointer;
	CustInfo *m_poCust;
	USERINFO_SHM_INT_INDEX_TYPE *m_poCustIndex;
	ProductInfo *m_poProduct;
	USERINFO_SHM_INT_INDEX_TYPE *m_poProductIndex;
	ServInfo *m_poServ;
    USERINFO_SHM_INT_INDEX_TYPE *m_poServIndex;
    ServLocationInfo *m_poServLocation;
    ServStateInfo *m_poServState;
	ServAttrInfo * m_poServAttr;
	#ifdef IMSI_MDN_RELATION
	ImsiMdnRelation * m_poImsiMdnRelation;
	USERINFO_SHM_STR_INDEX_TYPE* m_poImsiMdnRelationIndex;
	#endif
    ServIdentInfo *m_poServIdent;
	USERINFO_SHM_STR_INDEX_TYPE *m_poServIdentIndex;
	ServProductInfo *m_poServProduct;
	USERINFO_SHM_INT_INDEX_TYPE *m_poServProductIndex;
	ServProductAttrInfo * m_poServProductAttr;
    ServAcctInfo *m_poServAcct;
	USERINFO_SHM_INT_INDEX_TYPE * m_poServAcctIndex;
	AcctInfo * m_poAcct;
	USERINFO_SHM_INT_INDEX_TYPE * m_poAcctIndex;
	#ifdef GROUP_CONTROL
	GroupInfo * m_poGroupInfo;
	USERINFO_SHM_INT_INDEX_TYPE *m_poGroupInfoIndex;
	GroupMember * m_poGroupMember;
	USERINFO_SHM_STR_INDEX_TYPE *m_poGroupMemberIndex;
	NumberGroup * m_poNumberGroup;
	USERINFO_SHM_STR_INDEX_TYPE *m_poNumberGroupIndex;
	AccNbrOfferType* m_poAccNbr;
	USERINFO_SHM_INT_INDEX_TYPE *m_poAccNbrItemIndex;
	#endif
	ProdOfferIns * m_poProdOfferIns;
	USERINFO_SHM_INT_INDEX_TYPE * m_poProdOfferInsIndex;
	ProdOfferInsAttr * m_poProdOfferInsAttr;
	OfferDetailIns * m_poOfferDetailIns;
	USERINFO_SHM_INT_INDEX_TYPE * m_poOfferDetailInsOfferInsIndex;
	ProdOfferInsAgreement * m_poProdOfferInsAgreement;
	
	OfferInstanceInfo *m_poOfferInstance;
	CustPricingPlanInfo *m_poCustPlan;
	ServGroupInfo *m_poServGroup;
	USERINFO_SHM_INT_INDEX_TYPE *m_poOfferIndex;
	USERINFO_SHM_INT_INDEX_TYPE *m_poServGroupBaseIndex;
	USERINFO_SHM_STR_INDEX_TYPE *m_poCustTariffIndex;
	
	NpInfo *m_poNp;
	USERINFO_SHM_STR_INDEX_TYPE *m_poNpIndex;
};

struct fileName
{
    char sCust[64];
    char sProduct[64];
    char sServ[64];
    char sServType[64];
    char sServLoc[64];
    char sServState[64];
    char sServAttr[64];
    #ifdef IMSI_MDN_RELATION
    char sImsiMdnRelat[128];
    #endif
    char sServIden[64];
    char sServProd[64];
    char sServProdAttr[64];
    char sServAcct[64];
    char sAcct[64];
    #ifdef GROUP_CONTROL
    char sGroupInfo[64];
    char sGroupMember[64];
    char sNumberGroup[64];
    #endif
    char sACCNbrOfferType[64];
    char sOfferIns[64];
    char sOfferInsAttr[64];
    char sOfferInsDetail[64];
    char sOfferInsAgr[64];
    char sNpName[64];
};

#define CHANGE_DATA_STATE IpcKeyMgr::getIpcKey (-1,"SHM_CHANGE_DATA_STATE")

class ChangeDataState
{
    public:
        int m_iState[4];
		int m_iCurNum;
		int m_iProce[PROCE_NUM_MAX];
		int m_iProceMode[PROCE_NUM_MAX];
};

class SHMFileCtl
{
    public:
        SHMFileCtl();
		SHMFileCtl(bool bRebuild);
		SHMFileCtl(int iAppProgram);
        ~SHMFileCtl();
        
        static void * m_pChangePointer;
        static USERINFO_SHM_DATA_TYPE<ChangeDataState> *m_poState;
        static ChangeDataState * m_poStateData;
        static bool m_bAttached;
        
	    void initSHM();
		void initFile(bool bRebuild);
		
        void detachSHM();
        void detachFile();
        
        int changeUserInfo(int iChangeMode, bool bRebuild = false);
        int getDataState();
        
        pathFullName getFileName();
        void createFile(int iMode = 0);
        void removeFileAndIdx();
        
		int getSHMNattachNum();
        bool verifySHMFile(int iMark);
        
        void updateProce();
        void setProcessMode(int iMode, int iProc = 0);
		
	private:
		int initFileName();
		
		static SHMPointer * m_pSHMPointer;
		static FilePointer * m_pFilePointer;
		
		static UserInfoBase *m_poUserBase;
		static UserInfoMapBase *m_poUserMapBase;
		
		//pathFullName m_oFullName;
		char m_sTIBS_HOME[80];
		pathFullName m_oPathFullName;
		fileName m_oFileName;
};

#endif
