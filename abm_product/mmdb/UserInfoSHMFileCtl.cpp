/*VER: 1*/
#include "UserInfoSHMFileCtl.h"
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
void * SHMFileCtl::m_pChangePointer = 0;
SHMPointer * SHMFileCtl::m_pSHMPointer = 0;
FilePointer * SHMFileCtl::m_pFilePointer = 0;
UserInfoBase * SHMFileCtl::m_poUserBase = 0;
UserInfoMapBase * SHMFileCtl::m_poUserMapBase = 0;
bool SHMFileCtl::m_bAttached = false;

USERINFO_SHM_DATA_TYPE<ChangeDataState> * SHMFileCtl::m_poState = 0;
ChangeDataState * SHMFileCtl::m_poStateData = 0;

SHMFileCtl::SHMFileCtl()
{
    if (m_bAttached)
		return;
    
	m_bAttached = true;
    
    initFileName();
    m_pChangePointer = 0;
    m_pSHMPointer = new SHMPointer();
    m_pFilePointer = new FilePointer();
    //m_oFullName = oFullName;
    
    #ifdef  USERINFO_REUSE_MODE
    m_poState = new SHMData_A<ChangeDataState> (CHANGE_DATA_STATE);
    #else
    m_poState = new SHMData<ChangeDataState> (CHANGE_DATA_STATE);
    #endif
    
    if(!m_poState->exist()){
        m_poState->create(2);
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
        memset(m_poStateData->m_iState, 0, sizeof(m_poStateData->m_iState));
        m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
        m_poStateData->m_iCurNum = 0;
        memset(m_poStateData->m_iProce, -1, sizeof(m_poStateData->m_iProce));
        memset(m_poStateData->m_iProceMode, 0, sizeof(m_poStateData->m_iProceMode));
    }else
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
}

SHMFileCtl::SHMFileCtl(bool bRebuild)
{
    if(m_bAttached)
		return;
    
    m_bAttached = true;
    
    initFileName();
    m_pChangePointer = 0;
    m_pSHMPointer = new SHMPointer();
    m_pFilePointer = new FilePointer();
    
#ifdef  USERINFO_REUSE_MODE
    m_poState = new SHMData_A<ChangeDataState> (CHANGE_DATA_STATE);
#else
    m_poState = new SHMData<ChangeDataState> (CHANGE_DATA_STATE);
#endif
    
    if(!m_poState->exist()){
        m_poState->create(2);
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
        memset(m_poStateData->m_iState, 0, sizeof(m_poStateData->m_iState));
        m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
        m_poStateData->m_iCurNum = 0;
        memset(m_poStateData->m_iProce, -1, sizeof(m_poStateData->m_iProce));
        memset(m_poStateData->m_iProceMode, 0, sizeof(m_poStateData->m_iProceMode));
    }else
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
    
    if(!m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] && m_poStateData->m_iState[SHM_POINT_STATE]
        || m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] == SHM_POINT_STATE){
        m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
        initSHM();
    }else if(!m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] && m_poStateData->m_iState[FILE_POINT_STATE]
        || m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] == FILE_POINT_STATE){
        m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = FILE_POINT_STATE;
        initFile(bRebuild);
    }else{
        initSHM();
        if(!m_poStateData->m_iState[SHM_POINT_STATE])
            throw(12345);
    }
}

SHMFileCtl::SHMFileCtl(int iAppProgram)
{
    if(m_bAttached)
		return;
    
    m_bAttached = true;
    
    initFileName();
    
    #ifdef  USERINFO_REUSE_MODE
    m_poState = new SHMData_A<ChangeDataState> (CHANGE_DATA_STATE);
    #else
    m_poState = new SHMData<ChangeDataState> (CHANGE_DATA_STATE);
    #endif
    
    if(!m_poState->exist()){
        m_poState->create(2);
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
        memset(m_poStateData->m_iState, 0, sizeof(m_poStateData->m_iState));
        m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
        m_poStateData->m_iCurNum = 0;
        memset(m_poStateData->m_iProce, -1, sizeof(m_poStateData->m_iProce));
        memset(m_poStateData->m_iProceMode, 0, sizeof(m_poStateData->m_iProceMode));
    }else
        m_poStateData = (ChangeDataState *)(*m_poState)+2;
}

SHMFileCtl::~SHMFileCtl()
{
    if(m_pChangePointer){
        delete m_pChangePointer;
        m_pChangePointer = 0;
    }
    
    if(m_pSHMPointer){
        delete m_pSHMPointer;
        m_pSHMPointer = 0;
    }
    
    if(m_pFilePointer){
        delete m_pFilePointer;
        m_pFilePointer = 0;
    }
    
    if(m_poUserBase){
        delete m_poUserBase;
        m_poUserBase = 0;
    }
    
    if(m_poUserMapBase){
        delete m_poUserMapBase;
        m_poUserMapBase = 0;
    }
    
    if(m_poState){
        delete m_poState;
        m_poState = 0;
    }
}

void SHMFileCtl::initSHM()
{
    m_poUserBase = new UserInfoBase();
    UserInfoBase &oUserBase = *m_poUserBase;
    
    if(!oUserBase.m_bAttached){
        printf("Warning: share memory no attach!\n");
        m_poStateData->m_iState[SHM_POINT_STATE] = 0;
    }else{
        m_poStateData->m_iState[SHM_POINT_STATE] = SHM_POINT_STATE;
        
        m_pSHMPointer->m_poCustData = oUserBase.m_poCustData;
        m_pSHMPointer->m_poProductData = oUserBase.m_poProductData;
        m_pSHMPointer->m_poServData = oUserBase.m_poServData;
        m_pSHMPointer->m_poServTypeData = oUserBase.m_poServTypeData;
        m_pSHMPointer->m_poServLocationData = oUserBase.m_poServLocationData;
        m_pSHMPointer->m_poServStateData = oUserBase.m_poServStateData;
        m_pSHMPointer->m_poServAttrData = oUserBase.m_poServAttrData;
        #ifdef IMSI_MDN_RELATION
        m_pSHMPointer->m_poImsiMdnRelationData = oUserBase.m_poImsiMdnRelationData;
        #endif
        m_pSHMPointer->m_poServIdentData = oUserBase.m_poServIdentData;
        m_pSHMPointer->m_poServProductData = oUserBase.m_poServProductData;
        m_pSHMPointer->m_poServProductAttrData = oUserBase.m_poServProductAttrData;
        m_pSHMPointer->m_poServAcctData = oUserBase.m_poServAcctData;
        m_pSHMPointer->m_poAcctData = oUserBase.m_poAcctData;
        #ifdef GROUP_CONTROL
        m_pSHMPointer->m_poGroupInfoData = oUserBase.m_poGroupInfoData;
        m_pSHMPointer->m_poGroupMemberData = oUserBase.m_poGroupMemberData;
        m_pSHMPointer->m_poNumberGroupData = oUserBase.m_poNumberGroupData;
        m_pSHMPointer->m_poAccNbrData = oUserBase.m_poAccNbrData;
        #endif
        m_pSHMPointer->m_poProdOfferInsData = oUserBase.m_poProdOfferInsData;
        m_pSHMPointer->m_poProdOfferInsAttrData = oUserBase.m_poProdOfferInsAttrData;
        m_pSHMPointer->m_poOfferDetailInsData = oUserBase.m_poOfferDetailInsData;
        m_pSHMPointer->m_poProdOfferInsAgreementData = oUserBase.m_poProdOfferInsAgreementData;
        m_pSHMPointer->m_poNpData = oUserBase.m_poNpData;
        
        m_pSHMPointer->m_poCust = oUserBase.m_poCust;
        m_pSHMPointer->m_poCustIndex = oUserBase.m_poCustIndex;
        m_pSHMPointer->m_poProduct = oUserBase.m_poProduct;
        m_pSHMPointer->m_poProductIndex = oUserBase.m_poProductIndex;
        m_pSHMPointer->m_poServ = oUserBase.m_poServ;
        m_pSHMPointer->m_poServIndex = oUserBase.m_poServIndex;
        m_pSHMPointer->m_poServLocation = oUserBase.m_poServLocation;
        m_pSHMPointer->m_poServState = oUserBase.m_poServState;
        m_pSHMPointer->m_poServAttr = oUserBase.m_poServAttr;
        #ifdef IMSI_MDN_RELATION
        m_pSHMPointer->m_poImsiMdnRelation = oUserBase.m_poImsiMdnRelation;
        m_pSHMPointer->m_poImsiMdnRelationIndex = oUserBase.m_poImsiMdnRelationIndex;
        #endif
        m_pSHMPointer->m_poServIdent = oUserBase.m_poServIdent;
        m_pSHMPointer->m_poServIdentIndex = oUserBase.m_poServIdentIndex;
        m_pSHMPointer->m_poServProduct = oUserBase.m_poServProduct;
        m_pSHMPointer->m_poServProductIndex = oUserBase.m_poServProductIndex;
        m_pSHMPointer->m_poServProductAttr = oUserBase.m_poServProductAttr;
        m_pSHMPointer->m_poServAcct = oUserBase.m_poServAcct;
        m_pSHMPointer->m_poServAcctIndex = oUserBase.m_poServAcctIndex;
        m_pSHMPointer->m_poAcct = oUserBase.m_poAcct;
        m_pSHMPointer->m_poAcctIndex = oUserBase.m_poAcctIndex;
        #ifdef GROUP_CONTROL
        m_pSHMPointer->m_poGroupInfo = oUserBase.m_poGroupInfo;
        m_pSHMPointer->m_poGroupInfoIndex = oUserBase.m_poGroupInfoIndex;
        m_pSHMPointer->m_poGroupMember = oUserBase.m_poGroupMember;
        m_pSHMPointer->m_poGroupMemberIndex = oUserBase.m_poGroupMemberIndex;
        m_pSHMPointer->m_poNumberGroup = oUserBase.m_poNumberGroup;
        m_pSHMPointer->m_poNumberGroupIndex = oUserBase.m_poNumberGroupIndex;
        m_pSHMPointer->m_poAccNbr = oUserBase.m_poAccNbr;
        m_pSHMPointer->m_poAccNbrItemIndex = oUserBase.m_poAccNbrItemIndex;
        #endif
        m_pSHMPointer->m_poProdOfferIns = oUserBase.m_poProdOfferIns;
        m_pSHMPointer->m_poProdOfferInsIndex = oUserBase.m_poProdOfferInsIndex;
        m_pSHMPointer->m_poProdOfferInsAttr = oUserBase.m_poProdOfferInsAttr;
        m_pSHMPointer->m_poOfferDetailIns = oUserBase.m_poOfferDetailIns;
        m_pSHMPointer->m_poOfferDetailInsOfferInsIndex = oUserBase.m_poOfferDetailInsOfferInsIndex;
        m_pSHMPointer->m_poProdOfferInsAgreement = oUserBase.m_poProdOfferInsAgreement;
        
        m_pSHMPointer->m_poNp = oUserBase.m_poNp;
        m_pSHMPointer->m_poNpIndex = oUserBase.m_poNpIndex;
    }
}

void SHMFileCtl::initFile(bool bRebuild)
{
    if(bRebuild){
        if(!m_poUserBase)
            m_poUserBase = new UserInfoBase();
        
        m_poUserMapBase = new UserInfoMapBase(m_oPathFullName, m_poUserBase);
    }else
        m_poUserMapBase = new UserInfoMapBase(m_oPathFullName);
    
    UserInfoMapBase &oUserMapBase = *m_poUserMapBase;
    
    if(!oUserMapBase.m_bAttached){
        printf("Warning: file map memory no attach!\n");
        m_poStateData->m_iState[FILE_POINT_STATE] = 0;
    }else{
        m_poStateData->m_iState[FILE_POINT_STATE] = FILE_POINT_STATE;
    
        m_pFilePointer->m_poCustData = oUserMapBase.m_poCustData;
        m_pFilePointer->m_poProductData = oUserMapBase.m_poProductData;
        m_pFilePointer->m_poServData = oUserMapBase.m_poServData;
        m_pFilePointer->m_poServTypeData = oUserMapBase.m_poServTypeData;
        m_pFilePointer->m_poServLocationData = oUserMapBase.m_poServLocationData;
        m_pFilePointer->m_poServStateData = oUserMapBase.m_poServStateData;
        m_pFilePointer->m_poServAttrData = oUserMapBase.m_poServAttrData;
        #ifdef IMSI_MDN_RELATION
        m_pFilePointer->m_poImsiMdnRelationData = oUserMapBase.m_poImsiMdnRelationData;
        #endif
        m_pFilePointer->m_poServIdentData = oUserMapBase.m_poServIdentData;
        m_pFilePointer->m_poServProductData = oUserMapBase.m_poServProductData;
        m_pFilePointer->m_poServProductAttrData = oUserMapBase.m_poServProductAttrData;
        m_pFilePointer->m_poServAcctData = oUserMapBase.m_poServAcctData;
        m_pFilePointer->m_poAcctData = oUserMapBase.m_poAcctData;
        #ifdef GROUP_CONTROL
        m_pFilePointer->m_poGroupInfoData = oUserMapBase.m_poGroupInfoData;
        m_pFilePointer->m_poGroupMemberData = oUserMapBase.m_poGroupMemberData;
        m_pFilePointer->m_poNumberGroupData = oUserMapBase.m_poNumberGroupData;
        m_pFilePointer->m_poAccNbrData = oUserMapBase.m_poAccNbrData;
        #endif
        m_pFilePointer->m_poProdOfferInsData = oUserMapBase.m_poProdOfferInsData;
        m_pFilePointer->m_poProdOfferInsAttrData = oUserMapBase.m_poProdOfferInsAttrData;
        m_pFilePointer->m_poOfferDetailInsData = oUserMapBase.m_poOfferDetailInsData;
        m_pFilePointer->m_poProdOfferInsAgreementData = oUserMapBase.m_poProdOfferInsAgreementData;
        m_pFilePointer->m_poNpData = oUserMapBase.m_poNpData;
        
        m_pFilePointer->m_poCust = oUserMapBase.m_poCust;
        m_pFilePointer->m_poCustIndex = oUserMapBase.m_poCustIndex;
        m_pFilePointer->m_poProduct = oUserMapBase.m_poProduct;
        m_pFilePointer->m_poProductIndex = oUserMapBase.m_poProductIndex;
        m_pFilePointer->m_poServ = oUserMapBase.m_poServ;
        m_pFilePointer->m_poServIndex = oUserMapBase.m_poServIndex;
        m_pFilePointer->m_poServLocation = oUserMapBase.m_poServLocation;
        m_pFilePointer->m_poServState = oUserMapBase.m_poServState;
        m_pFilePointer->m_poServAttr = oUserMapBase.m_poServAttr;
        #ifdef IMSI_MDN_RELATION
        m_pFilePointer->m_poImsiMdnRelation = oUserMapBase.m_poImsiMdnRelation;
        m_pFilePointer->m_poImsiMdnRelationIndex = oUserMapBase.m_poImsiMdnRelationIndex;
        #endif
        m_pFilePointer->m_poServIdent = oUserMapBase.m_poServIdent;
        m_pFilePointer->m_poServIdentIndex = oUserMapBase.m_poServIdentIndex;
        m_pFilePointer->m_poServProduct = oUserMapBase.m_poServProduct;
        m_pFilePointer->m_poServProductIndex = oUserMapBase.m_poServProductIndex;
        m_pFilePointer->m_poServProductAttr = oUserMapBase.m_poServProductAttr;
        m_pFilePointer->m_poServAcct = oUserMapBase.m_poServAcct;
        m_pFilePointer->m_poServAcctIndex = oUserMapBase.m_poServAcctIndex;
        m_pFilePointer->m_poAcct = oUserMapBase.m_poAcct;
        m_pFilePointer->m_poAcctIndex = oUserMapBase.m_poAcctIndex;
        #ifdef GROUP_CONTROL
        m_pFilePointer->m_poGroupInfo = oUserMapBase.m_poGroupInfo;
        m_pFilePointer->m_poGroupInfoIndex = oUserMapBase.m_poGroupInfoIndex;
        m_pFilePointer->m_poGroupMember = oUserMapBase.m_poGroupMember;
        m_pFilePointer->m_poGroupMemberIndex = oUserMapBase.m_poGroupMemberIndex;
        m_pFilePointer->m_poNumberGroup = oUserMapBase.m_poNumberGroup;
        m_pFilePointer->m_poNumberGroupIndex = oUserMapBase.m_poNumberGroupIndex;
        m_pFilePointer->m_poAccNbr = oUserMapBase.m_poAccNbr;
        m_pFilePointer->m_poAccNbrItemIndex = oUserMapBase.m_poAccNbrItemIndex;
        #endif
        m_pFilePointer->m_poProdOfferIns = oUserMapBase.m_poProdOfferIns;
        m_pFilePointer->m_poProdOfferInsIndex = oUserMapBase.m_poProdOfferInsIndex;
        m_pFilePointer->m_poProdOfferInsAttr = oUserMapBase.m_poProdOfferInsAttr;
        m_pFilePointer->m_poOfferDetailIns = oUserMapBase.m_poOfferDetailIns;
        m_pFilePointer->m_poOfferDetailInsOfferInsIndex = oUserMapBase.m_poOfferDetailInsOfferInsIndex;
        m_pFilePointer->m_poProdOfferInsAgreement = oUserMapBase.m_poProdOfferInsAgreement;
        
        m_pFilePointer->m_poNp = oUserMapBase.m_poNp;
        m_pFilePointer->m_poNpIndex = oUserMapBase.m_poNpIndex;
    }
}

pathFullName SHMFileCtl::getFileName()
{
    return m_oPathFullName;
}

int SHMFileCtl::initFileName()
{
    ReadIni reader;
    char *p =  NULL;
    char sFile[254];
    int iRet = 0;

    if((p = getenv("TIBS_HOME")) == NULL)
        sprintf (m_sTIBS_HOME, "/home/bill/TIBS_HOME");
    else
        sprintf (m_sTIBS_HOME, "%s", p);

    if (m_sTIBS_HOME[strlen(m_sTIBS_HOME)-1] == '/')
        sprintf (sFile, "%setc/UserInfoParam.ini", m_sTIBS_HOME);
    else
        sprintf (sFile, "%s/etc/UserInfoParam.ini", m_sTIBS_HOME);

    reader.readIniString(sFile, "FILEPATH", "localpath", m_oPathFullName.sFilePath, "");

    DIR * pDir;
    if((pDir = opendir(m_oPathFullName.sFilePath)) ==NULL)
        mkdir(m_oPathFullName.sFilePath, 0770);
    if(pDir) closedir(pDir);
    
    reader.readIniString(sFile, "USERINFONAME", "custname", m_oFileName.sCust, "");
    reader.readIniString(sFile, "USERINFONAME", "productname", m_oFileName.sProduct, "");
    reader.readIniString(sFile, "USERINFONAME", "servname", m_oFileName.sServ, "");
    reader.readIniString(sFile, "USERINFONAME", "servtypename", m_oFileName.sServType, "");
    reader.readIniString(sFile, "USERINFONAME", "servlocationname", m_oFileName.sServLoc, "");
    reader.readIniString(sFile, "USERINFONAME", "servstateattrname", m_oFileName.sServState, "");
    reader.readIniString(sFile, "USERINFONAME", "servattrname", m_oFileName.sServAttr, "");
#ifdef IMSI_MDN_RELATION
    reader.readIniString(sFile, "USERINFONAME", "imsimdnrelatname", m_oFileName.sImsiMdnRelat, "");
#endif
    reader.readIniString(sFile, "USERINFONAME", "servidenname", m_oFileName.sServIden, "");
    reader.readIniString(sFile, "USERINFONAME", "servprodname", m_oFileName.sServProd, "");
    reader.readIniString(sFile, "USERINFONAME", "servprodattrname", m_oFileName.sServProdAttr, "");
    reader.readIniString(sFile, "USERINFONAME", "servacctname", m_oFileName.sServAcct, "");
    reader.readIniString(sFile, "USERINFONAME", "acctname", m_oFileName.sAcct, "");
#ifdef GROUP_CONTROL
    reader.readIniString(sFile, "USERINFONAME", "groupinfoname", m_oFileName.sGroupInfo, "");
    reader.readIniString(sFile, "USERINFONAME", "groupmembername", m_oFileName.sGroupMember, "");
    reader.readIniString(sFile, "USERINFONAME", "numbergroupname", m_oFileName.sNumberGroup, "");
    reader.readIniString(sFile, "USERINFONAME", "accnbroffertypename", m_oFileName.sACCNbrOfferType, "");
#endif
    reader.readIniString(sFile, "USERINFONAME", "prodofferinsname", m_oFileName.sOfferIns, "");
    reader.readIniString(sFile, "USERINFONAME", "prodofferinsattrname", m_oFileName.sOfferInsAttr, "");
    reader.readIniString(sFile, "USERINFONAME", "prodofferinsdetailname", m_oFileName.sOfferInsDetail, "");
    reader.readIniString(sFile, "USERINFONAME", "prodofferinsagreementname", m_oFileName.sOfferInsAgr, "");
    reader.readIniString(sFile, "USERINFONAME", "npname", m_oFileName.sNpName, "");
    
    if (m_oPathFullName.sFilePath[strlen(m_oPathFullName.sFilePath)-1] == '/'){
        sprintf(m_oPathFullName.sCustName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sCust);
        sprintf(m_oPathFullName.sProductName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sProduct);
        sprintf(m_oPathFullName.sServName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServ);
        sprintf(m_oPathFullName.sServTypeName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServType);
        sprintf(m_oPathFullName.sServLocName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServLoc);
        sprintf(m_oPathFullName.sServStateName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServState);
        sprintf(m_oPathFullName.sServAttrName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServAttr);
    #ifdef IMSI_MDN_RELATION
        sprintf(m_oPathFullName.sImsiMdnRelatName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sImsiMdnRelat);
    #endif
        sprintf(m_oPathFullName.sServIdenName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServIden);
        sprintf(m_oPathFullName.sServProdName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServProd);
        sprintf(m_oPathFullName.sServProdAttrName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServProdAttr);
        sprintf(m_oPathFullName.sServAcctName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sServAcct);
        sprintf(m_oPathFullName.sAcctName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sAcct);
    #ifdef GROUP_CONTROL
        sprintf(m_oPathFullName.sGroupInfoName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sGroupInfo);
        sprintf(m_oPathFullName.sGroupMemberName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sGroupMember);
        sprintf(m_oPathFullName.sNumberGroupName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sNumberGroup);
        sprintf(m_oPathFullName.sACCNbrOfferTypeName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sACCNbrOfferType);
    #endif
        sprintf(m_oPathFullName.sOfferInsName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sOfferIns);
        sprintf(m_oPathFullName.sOfferInsAttrName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsAttr);
        sprintf(m_oPathFullName.sOfferInsDetailName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsDetail);
        sprintf(m_oPathFullName.sOfferInsAgrName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsAgr);
        sprintf(m_oPathFullName.sNpName, "%s%s", m_oPathFullName.sFilePath, m_oFileName.sNpName);
    }else{
        sprintf(m_oPathFullName.sCustName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sCust);
        sprintf(m_oPathFullName.sProductName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sProduct);
        sprintf(m_oPathFullName.sServName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServ);
        sprintf(m_oPathFullName.sServTypeName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServType);
        sprintf(m_oPathFullName.sServLocName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServLoc);
        sprintf(m_oPathFullName.sServStateName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServState);
        sprintf(m_oPathFullName.sServAttrName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServAttr);
    #ifdef IMSI_MDN_RELATION
        sprintf(m_oPathFullName.sImsiMdnRelatName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sImsiMdnRelat);
    #endif
        sprintf(m_oPathFullName.sServIdenName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServIden);
        sprintf(m_oPathFullName.sServProdName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServProd);
        sprintf(m_oPathFullName.sServProdAttrName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServProdAttr);
        sprintf(m_oPathFullName.sServAcctName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sServAcct);
        sprintf(m_oPathFullName.sAcctName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sAcct);
    #ifdef GROUP_CONTROL
        sprintf(m_oPathFullName.sGroupInfoName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sGroupInfo);
        sprintf(m_oPathFullName.sGroupMemberName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sGroupMember);
        sprintf(m_oPathFullName.sNumberGroupName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sNumberGroup);
        sprintf(m_oPathFullName.sACCNbrOfferTypeName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sACCNbrOfferType);
    #endif
        sprintf(m_oPathFullName.sOfferInsName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sOfferIns);
        sprintf(m_oPathFullName.sOfferInsAttrName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsAttr);
        sprintf(m_oPathFullName.sOfferInsDetailName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsDetail);
        sprintf(m_oPathFullName.sOfferInsAgrName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sOfferInsAgr);
        sprintf(m_oPathFullName.sNpName, "%s/%s", m_oPathFullName.sFilePath, m_oFileName.sNpName);
    }

    return iRet;
}

void SHMFileCtl::detachSHM()
{
    if(!m_pSHMPointer)
        return;
    
    DETACHSHM(m_pSHMPointer->m_poCustData);
    DETACHSHM(m_pSHMPointer->m_poProductData);
    DETACHSHM(m_pSHMPointer->m_poServData);
    DETACHSHM(m_pSHMPointer->m_poServTypeData);
    DETACHSHM(m_pSHMPointer->m_poServLocationData);
    DETACHSHM(m_pSHMPointer->m_poServStateData);
    DETACHSHM(m_pSHMPointer->m_poServAttrData);
    #ifdef IMSI_MDN_RELATION
    DETACHSHM(m_pSHMPointer->m_poImsiMdnRelationData);
    #endif
    DETACHSHM(m_pSHMPointer->m_poServIdentData);
    DETACHSHM(m_pSHMPointer->m_poServProductData);
    DETACHSHM(m_pSHMPointer->m_poServProductAttrData);
    DETACHSHM(m_pSHMPointer->m_poServAcctData);
    DETACHSHM(m_pSHMPointer->m_poAcctData);
    #ifdef GROUP_CONTROL
    DETACHSHM(m_pSHMPointer->m_poGroupInfoData);
    DETACHSHM(m_pSHMPointer->m_poGroupMemberData);
    DETACHSHM(m_pSHMPointer->m_poNumberGroupData);
    DETACHSHM(m_pSHMPointer->m_poAccNbrData);
    #endif
    DETACHSHM(m_pSHMPointer->m_poProdOfferInsData);
    DETACHSHM(m_pSHMPointer->m_poProdOfferInsAttrData);
    DETACHSHM(m_pSHMPointer->m_poOfferDetailInsData);
    DETACHSHM(m_pSHMPointer->m_poProdOfferInsAgreementData);
    DETACHSHM(m_pSHMPointer->m_poNpData);
    
    DETACHSHM(m_pSHMPointer->m_poCustIndex);
    DETACHSHM(m_pSHMPointer->m_poProductIndex);
    DETACHSHM(m_pSHMPointer->m_poServIndex);
    #ifdef IMSI_MDN_RELATION
    DETACHSHM(m_pSHMPointer->m_poImsiMdnRelationIndex);
    #endif
    DETACHSHM(m_pSHMPointer->m_poServIdentIndex);
    DETACHSHM(m_pSHMPointer->m_poServProductIndex);
    DETACHSHM(m_pSHMPointer->m_poServAcctIndex);
    DETACHSHM(m_pSHMPointer->m_poAcctIndex);
    #ifdef GROUP_CONTROL
    DETACHSHM(m_pSHMPointer->m_poGroupInfoIndex);
    DETACHSHM(m_pSHMPointer->m_poGroupMemberIndex);
    DETACHSHM(m_pSHMPointer->m_poNumberGroupIndex);
    DETACHSHM(m_pSHMPointer->m_poAccNbrItemIndex);
    #endif
    DETACHSHM(m_pSHMPointer->m_poProdOfferInsIndex);
    DETACHSHM(m_pSHMPointer->m_poOfferDetailInsOfferInsIndex);
    
    DETACHSHM(m_pSHMPointer->m_poNpIndex);
    
    m_poUserBase->freeAll();
    delete m_poUserBase;
    m_poUserBase = 0;
}

void SHMFileCtl::detachFile()
{
    if(!m_poUserMapBase)
        return;
    
    m_poUserMapBase->freeAll();
    delete m_poUserMapBase;
    m_poUserMapBase = 0;
}

int SHMFileCtl::getDataState()
{

}

void SHMFileCtl::createFile(int iMode)
{
    if(!m_poUserBase)
        m_poUserBase = new UserInfoBase();
    
    if(m_poUserMapBase){
        m_poUserMapBase->m_bAttached = false;
        delete m_poUserMapBase;
        m_poUserMapBase = 0;
    }
    
    if(!iMode)
        m_poUserMapBase = new UserInfoMapBase(m_oPathFullName, m_poUserBase);
    else
        m_poUserMapBase = new UserInfoMapBase(m_oPathFullName, m_poUserBase, iMode);
}

void SHMFileCtl::removeFileAndIdx()
{
    #define REMOVEFILE(X) \
        if(X) \
            if (X->exist()){ \
                X->delFile(); \
            }
    
    #define REMOVEINDEX(X) \
        if(X) \
            if (X->exist()) { \
                X->remove (); \
            }
    
    if(!m_poUserMapBase)
        m_poUserMapBase = new UserInfoMapBase(m_oPathFullName);
    
    REMOVEFILE(m_poUserMapBase->m_poCustData);
    REMOVEFILE(m_poUserMapBase->m_poProductData);
    REMOVEFILE(m_poUserMapBase->m_poServData);
    REMOVEFILE(m_poUserMapBase->m_poServTypeData);
    REMOVEFILE(m_poUserMapBase->m_poServLocationData);
    REMOVEFILE(m_poUserMapBase->m_poServStateData);
    REMOVEFILE(m_poUserMapBase->m_poServAttrData);
    #ifdef IMSI_MDN_RELATION
    REMOVEFILE(m_poUserMapBase->m_poImsiMdnRelationData);
    #endif
    REMOVEFILE(m_poUserMapBase->m_poServIdentData);
    REMOVEFILE(m_poUserMapBase->m_poServProductData);
    REMOVEFILE(m_poUserMapBase->m_poServProductAttrData);
    REMOVEFILE(m_poUserMapBase->m_poServAcctData);
    REMOVEFILE(m_poUserMapBase->m_poAcctData);
    #ifdef GROUP_CONTROL
    REMOVEFILE(m_poUserMapBase->m_poGroupInfoData);
    REMOVEFILE(m_poUserMapBase->m_poGroupMemberData);
    REMOVEFILE(m_poUserMapBase->m_poNumberGroupData);
    REMOVEFILE(m_poUserMapBase->m_poAccNbrData);
    #endif
    REMOVEFILE(m_poUserMapBase->m_poProdOfferInsData);
    REMOVEFILE(m_poUserMapBase->m_poProdOfferInsAttrData);
    REMOVEFILE(m_poUserMapBase->m_poOfferDetailInsData);
    REMOVEFILE(m_poUserMapBase->m_poProdOfferInsAgreementData);
    REMOVEFILE(m_poUserMapBase->m_poNpData);
    
    REMOVEINDEX(m_poUserMapBase->m_poCustIndex);
    REMOVEINDEX(m_poUserMapBase->m_poProductIndex);
    REMOVEINDEX(m_poUserMapBase->m_poServIndex);
    REMOVEINDEX(m_poUserMapBase->m_poServLocationIndex);
    #ifdef IMSI_MDN_RELATION
    REMOVEINDEX(m_poUserMapBase->m_poImsiMdnRelationIndex);
    #endif
    REMOVEINDEX(m_poUserMapBase->m_poServIdentIndex);
    REMOVEINDEX(m_poUserMapBase->m_poServProductIndex);
    REMOVEINDEX(m_poUserMapBase->m_poServAcctIndex);
    REMOVEINDEX(m_poUserMapBase->m_poAcctIndex);
    #ifdef GROUP_CONTROL
    REMOVEINDEX(m_poUserMapBase->m_poGroupInfoIndex);
    REMOVEINDEX(m_poUserMapBase->m_poGroupMemberIndex);
    REMOVEINDEX(m_poUserMapBase->m_poNumberGroupIndex);
    REMOVEINDEX(m_poUserMapBase->m_poAccNbrIndex);
    REMOVEINDEX(m_poUserMapBase->m_poAccNbrItemIndex);
    #endif
    REMOVEINDEX(m_poUserMapBase->m_poProdOfferInsIndex);
    REMOVEINDEX(m_poUserMapBase->m_poOfferDetailInsOfferInsIndex);
    REMOVEINDEX(m_poUserMapBase->m_poNpIndex);
}

int SHMFileCtl::changeUserInfo(int iChangeMode, bool bRebuild)
{
    int iRet = 0;
    
    if(iChangeMode%2)
        m_poStateData->m_iState[CHANGE_POINT_STATE] = FILE_POINT_STATE;
    else
        m_poStateData->m_iState[CHANGE_POINT_STATE] = SHM_POINT_STATE;
    
    //if(m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] == m_poStateData->m_iState[CHANGE_POINT_STATE])
    //    return iRet;
    
    switch(iChangeMode){
        case 1:{
                initFile(bRebuild);
                detachSHM();
                m_pChangePointer = m_pFilePointer;
                m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = FILE_POINT_STATE;
            }
            break;
            
        case 2:{
                initSHM();
                detachFile();
                m_pChangePointer = m_pSHMPointer;
                m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
            }
            break;
            
        case 3:{
                initSHM();
                initFile(bRebuild);
                m_pChangePointer = m_pFilePointer;
                m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = FILE_POINT_STATE;
            }
            break;
            
        case 4:{
                initSHM();
                initFile(bRebuild);
                m_pChangePointer = m_pSHMPointer;
                m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
            }
            break;
            
        case 5:{
                if(m_pFilePointer){
                    m_pChangePointer = m_pFilePointer;
                    m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = FILE_POINT_STATE;
                }else{
                    printf("Err: file map pointer is null!\n");
                    iRet = -1;
                }
            }
            break;
            
        case 6:{
                if(m_pSHMPointer){
                    m_pChangePointer = m_pSHMPointer;
                    m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] = SHM_POINT_STATE;
                }else{
                    printf("Err: share mem pointer is null!\n");
                    iRet = -1;
                }
            }
            break;
            
        default:{
                printf("Err: change mode!\n");
                iRet = -1;
            }
            break;
    }
    
    return iRet;
}

int SHMFileCtl::getSHMNattachNum()
{
    #define NATTACHNUM(X) \
        if(X) \
            if(X->exist()) \
                if(X->connnum() > 1) \
                    return (++iNum);
    
    int iNum = 0;
    
    if(!m_poUserBase)
        m_poUserBase = new UserInfoBase();
    
    NATTACHNUM(m_poUserBase->m_poCustData);
    NATTACHNUM(m_poUserBase->m_poCustIndex);
    NATTACHNUM(m_poUserBase->m_poProductData);
    NATTACHNUM(m_poUserBase->m_poProductIndex);
    NATTACHNUM(m_poUserBase->m_poServData);
    NATTACHNUM(m_poUserBase->m_poServTypeData);
    NATTACHNUM(m_poUserBase->m_poServIndex);
    NATTACHNUM(m_poUserBase->m_poServLocationData);
    NATTACHNUM(m_poUserBase->m_poServLocationIndex);
    NATTACHNUM(m_poUserBase->m_poServStateData);
    NATTACHNUM(m_poUserBase->m_poServAttrData);
    NATTACHNUM(m_poUserBase->m_poServIdentData);
    NATTACHNUM(m_poUserBase->m_poServIdentIndex);
    NATTACHNUM(m_poUserBase->m_poServProductData);
    NATTACHNUM(m_poUserBase->m_poServProductIndex);
    NATTACHNUM(m_poUserBase->m_poServProductAttrData);
    NATTACHNUM(m_poUserBase->m_poServAcctData);
    NATTACHNUM(m_poUserBase->m_poServAcctIndex);
    NATTACHNUM(m_poUserBase->m_poAcctData);
    NATTACHNUM(m_poUserBase->m_poAcctIndex);
    NATTACHNUM(m_poUserBase->m_poProdOfferInsData);
    NATTACHNUM(m_poUserBase->m_poProdOfferInsIndex);
    NATTACHNUM(m_poUserBase->m_poOfferDetailInsData);
    NATTACHNUM(m_poUserBase->m_poOfferDetailInsOfferInsIndex);
    NATTACHNUM(m_poUserBase->m_poProdOfferInsAttrData);
    NATTACHNUM(m_poUserBase->m_poProdOfferInsAgreementData);
    #ifdef GROUP_CONTROL
    NATTACHNUM(m_poUserBase->m_poGroupInfoData);
    NATTACHNUM(m_poUserBase->m_poGroupMemberData);
    NATTACHNUM(m_poUserBase->m_poNumberGroupData);
    NATTACHNUM(m_poUserBase->m_poGroupInfoIndex);
    NATTACHNUM(m_poUserBase->m_poGroupMemberIndex);
    NATTACHNUM(m_poUserBase->m_poNumberGroupIndex);
    NATTACHNUM(m_poUserBase->m_poAccNbrData);
    NATTACHNUM(m_poUserBase->m_poAccNbrIndex);
    NATTACHNUM(m_poUserBase->m_poAccNbrItemIndex);
    #endif
    #ifdef IMSI_MDN_RELATION
    NATTACHNUM(m_poUserBase->m_poImsiMdnRelationData);
    NATTACHNUM(m_poUserBase->m_poImsiMdnRelationIndex);
    #endif
    #ifdef IMMEDIATE_VALID
    NATTACHNUM(m_poUserBase->m_poServInstRelationData);
    NATTACHNUM(m_poUserBase->m_poServInstIndex);
    #endif
    NATTACHNUM(m_poUserBase->m_poNpData);
    NATTACHNUM(m_poUserBase->m_poNpIndex);
    
    return iNum;
}

bool SHMFileCtl::verifySHMFile(int iMark)
{
    bool bRet = false;
    
    if(iMark == FILE_POINT_STATE){
        if(!m_poUserMapBase)
            m_poUserMapBase = new UserInfoMapBase(m_oPathFullName);
        
        if(m_poUserMapBase->m_bAttached)
            bRet = true;
    }else if(iMark == SHM_POINT_STATE){
        if(!m_poUserBase)
            m_poUserBase = new UserInfoBase();
        
        if(m_poUserBase->m_bAttached)
            bRet = true;
    }
    
    return bRet;
}

void SHMFileCtl::updateProce()
{
    int i = 0;
    
    for(i = 0; i < PROCE_NUM_MAX; i ++){
        if(m_poStateData->m_iProce[i] != -1){
            if(kill(m_poStateData->m_iProce[i], 0)){
                m_poStateData->m_iProce[i] = -1;
                m_poStateData->m_iCurNum --;
                m_poStateData->m_iProceMode[i] = 0;
            }
        }
    }
}

void SHMFileCtl::setProcessMode(int iMode, int iProc)
{
    int i = 0;
    
    if(m_poStateData->m_iCurNum == PROCE_NUM_MAX)
        updateProce();
    
    if(iProc){
        for(i = 0; i < PROCE_NUM_MAX; i ++)
            if(m_poStateData->m_iProce[i] == iProc){
                m_poStateData->m_iProceMode[i] = iMode;
                return;
            }
    }
    
    for(i = 0; i < PROCE_NUM_MAX; i ++){
        if(m_poStateData->m_iProce[i] == -1){
            m_poStateData->m_iProce[i] = getpid();
            m_poStateData->m_iProceMode[i] = iMode;
            m_poStateData->m_iCurNum ++;
            break;
        }
    }
}
