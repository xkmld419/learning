/*VER: 1*/
//#include <pthread.h>
#include <vector>
#include <algorithm>

#include "UserInfoMap.h"
#include "Log.h"
#include <sys/wait.h>
using namespace std;

//int UserInfoMapBase::m_iThreadNum = 0;
bool UserInfoMapBase::m_bAttached = false;

SHMMapFile<CustInfo> * UserInfoMapBase::m_poCustData = 0;
SHMMapFile<ProductInfo> * UserInfoMapBase::m_poProductData = 0;
SHMMapFile<ServInfo> * UserInfoMapBase::m_poServData = 0;
SHMMapFile<ServTypeInfo> * UserInfoMapBase::m_poServTypeData = 0;
SHMMapFile<ServLocationInfo> * UserInfoMapBase::m_poServLocationData = 0;
SHMMapFile<ServStateInfo> * UserInfoMapBase::m_poServStateData = 0;
SHMMapFile<ServAttrInfo> * UserInfoMapBase::m_poServAttrData = 0;
#ifdef IMSI_MDN_RELATION
SHMMapFile<ImsiMdnRelation> * UserInfoMapBase::m_poImsiMdnRelationData = 0;
#endif
SHMMapFile<ServIdentInfo> * UserInfoMapBase::m_poServIdentData = 0;
SHMMapFile<ServProductInfo> * UserInfoMapBase::m_poServProductData = 0;
SHMMapFile<ServProductAttrInfo> * UserInfoMapBase::m_poServProductAttrData = 0;
SHMMapFile<ServAcctInfo> * UserInfoMapBase::m_poServAcctData = 0;
SHMMapFile<AcctInfo> * UserInfoMapBase::m_poAcctData = 0;
#ifdef GROUP_CONTROL
SHMMapFile<GroupInfo> * UserInfoMapBase::m_poGroupInfoData = 0;
SHMMapFile<GroupMember> * UserInfoMapBase::m_poGroupMemberData = 0;
SHMMapFile<NumberGroup> * UserInfoMapBase::m_poNumberGroupData = 0;
SHMMapFile<AccNbrOfferType> * UserInfoMapBase::m_poAccNbrData = 0;
#endif
SHMMapFile<ProdOfferIns> * UserInfoMapBase::m_poProdOfferInsData = 0;
SHMMapFile<ProdOfferInsAttr> * UserInfoMapBase::m_poProdOfferInsAttrData = 0;
SHMMapFile<OfferDetailIns> * UserInfoMapBase::m_poOfferDetailInsData = 0;
SHMMapFile<ProdOfferInsAgreement> *UserInfoMapBase::m_poProdOfferInsAgreementData = 0;
SHMMapFile<NpInfo> *UserInfoMapBase::m_poNpData = 0;

CustInfo * UserInfoMapBase::m_poCust = 0;
ProductInfo * UserInfoMapBase::m_poProduct = 0;
ServInfo * UserInfoMapBase::m_poServ = 0;
//ServTypeInfo * UserInfoMapBase::m_poServTypeMap = 0;
ServLocationInfo * UserInfoMapBase::m_poServLocation = 0;
ServStateInfo * UserInfoMapBase::m_poServState = 0;
ServAttrInfo * UserInfoMapBase::m_poServAttr = 0;
#ifdef IMSI_MDN_RELATION
ImsiMdnRelation * UserInfoMapBase::m_poImsiMdnRelation = 0;
#endif
ServIdentInfo * UserInfoMapBase::m_poServIdent = 0;
ServProductInfo * UserInfoMapBase::m_poServProduct = 0;
ServProductAttrInfo * UserInfoMapBase::m_poServProductAttr = 0;
ServAcctInfo * UserInfoMapBase::m_poServAcct = 0;
AcctInfo * UserInfoMapBase::m_poAcct = 0;
#ifdef GROUP_CONTROL
GroupInfo * UserInfoMapBase::m_poGroupInfo = 0;
GroupMember * UserInfoMapBase::m_poGroupMember = 0;
NumberGroup * UserInfoMapBase::m_poNumberGroup = 0;
AccNbrOfferType * UserInfoMapBase::m_poAccNbr = 0;
#endif
ProdOfferIns * UserInfoMapBase::m_poProdOfferIns = 0;
ProdOfferInsAttr * UserInfoMapBase::m_poProdOfferInsAttr = 0;
OfferDetailIns * UserInfoMapBase::m_poOfferDetailIns = 0;
ProdOfferInsAgreement * UserInfoMapBase::m_poProdOfferInsAgreement = 0;
NpInfo * UserInfoMapBase::m_poNp = 0;

USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poCustIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poProductIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poServIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poServLocationIndex = 0;
#ifdef IMSI_MDN_RELATION
USERINFO_SHM_STR_INDEX_TYPE *UserInfoMapBase::m_poImsiMdnRelationIndex = 0;
#endif
USERINFO_SHM_STR_INDEX_TYPE *UserInfoMapBase::m_poServIdentIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poServProductIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poServAcctIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poAcctIndex = 0;
#ifdef GROUP_CONTROL
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poGroupInfoIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poGroupMemberIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poNumberGroupIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poAccNbrIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poAccNbrItemIndex = 0;
#endif
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poProdOfferInsIndex = 0;
USERINFO_SHM_INT_INDEX_TYPE *UserInfoMapBase::m_poOfferDetailInsOfferInsIndex = 0;
USERINFO_SHM_STR_INDEX_TYPE *UserInfoMapBase::m_poNpIndex = 0;

/*void * threadRun(void* param)
{
    UserInfoMapBase * thr = (UserInfoMapBase *)param;
    
	try{
		pthread_detach(pthread_self());
	}catch(...){
		cout<<"分离线程时发生异常!"<<endl;
		throw 1;
	}
    
	try{
        int iTemp = 1;
        
        while(1){
            if(iTemp > NP_DATA)
                break;
            else if(thr->m_oUserInfoManage.iOperMode[iTemp] == iTemp){
                iTemp ++;
                continue;
            }else
                thr->m_oUserInfoManage.iOperMode[iTemp] = iTemp;
            
    		switch(iTemp){
                case ALL_INDEX:
                    thr->setAllIndex();
                    //UserInfoMapBase::m_iThreadNum ++;
                    break;
                    
                default:
                    thr->setFileData(iTemp);
                    //UserInfoMapBase::m_iThreadNum ++;
                    break;
            }
        }
        
        UserInfoMapBase::m_iThreadNum ++;
	}catch(...){
		cout<<"发生异常的线程退出,等待进程退出!"<<endl;
		pthread_exit((void *)"线程退出!");
	}
    
	return NULL;
}*/

UserInfoMapBase::UserInfoMapBase(pathFullName sFullName, UserInfoBase * poUserInfoBase, int iMode)
{
    if(m_bAttached)
        return;
    
    m_bAttached = true;
    unsigned int iCnt[20];
    int i = 0;
    bool bCreate = false;
    
    m_oUserInfoManage.m_sFullName = sFullName;
    
    if(poUserInfoBase){
        m_oUserInfoManage.m_oUserInfoBase = *poUserInfoBase;
        
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poCustIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poProductIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poServIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poServLocationIndex->getTotal();
        #ifdef IMSI_MDN_RELATION
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poImsiMdnRelationIndex->getlTotal();
        #endif
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poServIdentIndex->getlTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poServProductIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poServAcctIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poAcctIndex->getTotal();
        #ifdef GROUP_CONTROL
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poGroupInfoIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poGroupMemberIndex->getlTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poNumberGroupIndex->getlTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrIndex->getlTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrItemIndex->getTotal();
        #endif
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poOfferDetailInsOfferInsIndex->getTotal();
        iCnt[i++] = m_oUserInfoManage.m_oUserInfoBase.m_poNpIndex->getlTotal();
        
        bCreate = true;
    }
    
    i = 0;
    
    USERINFO_ATTACH_INT_IDX(m_poCustIndex, CUSTFILE_INFO_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poProductIndex, PRODFILE_INFO_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poServIndex, SERVFILE_INFO_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poServLocationIndex, SERVLOCFILE_INFO_IDX, iCnt[i++], bCreate);
    #ifdef IMSI_MDN_RELATION
    USERINFO_ATTACH_STRING_IDX(m_poImsiMdnRelationIndex, IMSIMDNRELATFILE_INFO_IDX, iCnt[i++], MAX_IMSI_LEN, bCreate);
    #endif
    USERINFO_ATTACH_STRING_IDX(m_poServIdentIndex, SERVIDENTFILE_INFO_IDX, iCnt[i++], MAX_CALLING_NBR_LEN, bCreate);
    USERINFO_ATTACH_INT_IDX(m_poServProductIndex, SERVPRODFILE_INFO_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poServAcctIndex, SERVACCTFILE_ACCT_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poAcctIndex, ACCTFILE_INFO_IDX, iCnt[i++], bCreate);
    #ifdef GROUP_CONTROL
    USERINFO_ATTACH_INT_IDX(m_poGroupInfoIndex, GROUPFILE_INFO_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_STRING_IDX(m_poGroupMemberIndex, GROUPFILE_MEMBER_IDX, iCnt[i++], MAX_CALLING_NBR_LEN, bCreate);
    USERINFO_ATTACH_STRING_IDX(m_poNumberGroupIndex, NUMBERFILE_GROUP_IDX, iCnt[i++], MAX_CALLING_NBR_LEN, bCreate);
    USERINFO_ATTACH_STRING_IDX(m_poAccNbrIndex, ACCNBROFFERTYPEFILE_IDX, iCnt[i++], MAX_CALLING_NBR_LEN, bCreate);
    USERINFO_ATTACH_INT_IDX(m_poAccNbrItemIndex, ACCNBROFFERTYPEITEMFILE_IDX, iCnt[i++], bCreate);
    #endif
    USERINFO_ATTACH_INT_IDX(m_poProdOfferInsIndex, PRODOFFERINSFILE_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_INT_IDX(m_poOfferDetailInsOfferInsIndex, OFFERDETAILINS_IDX, iCnt[i++], bCreate);
    USERINFO_ATTACH_STRING_IDX(m_poNpIndex, NPINFOFILE_IDX, iCnt[i++], MAX_CALLING_NBR_LEN, bCreate);
    
    if(poUserInfoBase){
        if(iMode){
            int iTemp = 0;
            memset(m_oUserInfoManage.iOperMode, 0, sizeof(m_oUserInfoManage.iOperMode));
            
            assignData();
            
            int iForkRet = 0;
            iForkRet = fork();
            switch(iForkRet){
                case 0:
                    setAllData(2, m_oUserInfoManage.iOperMode);
                    break;
                    
                case -1:
                    perror("fork luodi");
                    cout<<"start_Command: fork luodi err"<<endl;
                    exit(0);
                    break;
                    
                default:
                    Log::log (0, "Child process create ,pid:%d", iForkRet);
                    setAllData(1, m_oUserInfoManage.iOperMode);
                    /*for(iTemp = ALL_INDEX; iTemp <= NP_DATA; ){
                        if(m_oUserInfoManage.iOperMode[iTemp] != 3){
                            Log::log(0,"等待辅助进程完成!");
                            sleep(5);
                        }
                        
                        iTemp ++;
                    }*/
                    waitpid(iForkRet, NULL, 0);
                    Log::log(0, "并行资料文件落地、新索引创建，结束...!\n");
                    break;
            }
            
            //createThread(ALL_INDEX); iCount++;
            
            /*for(int i = 0; i < 2; i ++){
                createThread();
                iCount++;
            }
            
            int iTemp = 1;
            while(1){
                if(iTemp > NP_DATA)
                    break;
                else if(m_oUserInfoManage.iOperMode[iTemp] == iTemp){
                    iTemp ++;
                    continue;
                }else
                    m_oUserInfoManage.iOperMode[iTemp] = iTemp;
                
        		switch(iTemp){
                    case ALL_INDEX:
                        setAllIndex();
                        //UserInfoMapBase::m_iThreadNum ++;
                        break;
                        
                    default:
                        setFileData(iTemp);
                        //UserInfoMapBase::m_iThreadNum ++;
                        break;
                }
            }*/
            //UserInfoMapBase::m_iThreadNum ++;
            
            /*while(1){
                if(m_iThreadNum == iCount){
        			Log::log(0,"完成线程数 %d 个", m_iThreadNum);
        			break;
        		}
                
        		Log::log(0,"完成线程数 %d 个", m_iThreadNum);
        		sleep(5);
            }*/
        }else{
            setAllIndex();
            setFileData(ALL_DATA);
        }
    }else{
        USERINFO_ATTACH_MAP_DATA1(m_poCustData, CustInfo, sFullName.sCustName);
        USERINFO_ATTACH_MAP_DATA1(m_poProductData, ProductInfo, sFullName.sProductName);
        USERINFO_ATTACH_MAP_DATA1(m_poServData, ServInfo, sFullName.sServName);
        USERINFO_ATTACH_MAP_DATA1(m_poServTypeData, ServTypeInfo, sFullName.sServTypeName);
        USERINFO_ATTACH_MAP_DATA1(m_poServLocationData, ServLocationInfo, sFullName.sServLocName);
        USERINFO_ATTACH_MAP_DATA1(m_poServStateData, ServStateInfo, sFullName.sServStateName);
        USERINFO_ATTACH_MAP_DATA1(m_poServAttrData, ServAttrInfo, sFullName.sServAttrName);
        #ifdef IMSI_MDN_RELATION
        USERINFO_ATTACH_MAP_DATA1(m_poImsiMdnRelationData, ImsiMdnRelation, sFullName.sImsiMdnRelatName);
        #endif
        USERINFO_ATTACH_MAP_DATA1(m_poServIdentData, ServIdentInfo, sFullName.sServIdenName);
        USERINFO_ATTACH_MAP_DATA1(m_poServProductData, ServProductInfo, sFullName.sServProdName);
        USERINFO_ATTACH_MAP_DATA1(m_poServProductAttrData, ServProductAttrInfo, sFullName.sServProdAttrName);
        USERINFO_ATTACH_MAP_DATA1(m_poServAcctData, ServAcctInfo, sFullName.sServAcctName);
        USERINFO_ATTACH_MAP_DATA1(m_poAcctData, AcctInfo, sFullName.sAcctName);
        #ifdef GROUP_CONTROL
        USERINFO_ATTACH_MAP_DATA1(m_poGroupInfoData, GroupInfo, sFullName.sGroupInfoName);
        USERINFO_ATTACH_MAP_DATA1(m_poGroupMemberData, GroupMember, sFullName.sGroupMemberName);
        USERINFO_ATTACH_MAP_DATA1(m_poNumberGroupData, NumberGroup, sFullName.sNumberGroupName);
        USERINFO_ATTACH_MAP_DATA1(m_poAccNbrData, AccNbrOfferType, sFullName.sACCNbrOfferTypeName);
        #endif
        USERINFO_ATTACH_MAP_DATA1(m_poProdOfferInsData, ProdOfferIns, sFullName.sOfferInsName);
        USERINFO_ATTACH_MAP_DATA1(m_poProdOfferInsAttrData, ProdOfferInsAttr, sFullName.sOfferInsAttrName);
        USERINFO_ATTACH_MAP_DATA1(m_poOfferDetailInsData, OfferDetailIns, sFullName.sOfferInsDetailName);
        USERINFO_ATTACH_MAP_DATA1(m_poProdOfferInsAgreementData, ProdOfferInsAgreement, sFullName.sOfferInsAgrName);
        USERINFO_ATTACH_MAP_DATA1(m_poNpData, NpInfo, sFullName.sNpName);
        
        USERINFO_ATTACH_INT_INDEX(m_poCustIndex, CUSTFILE_INFO_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poProductIndex, PRODFILE_INFO_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poServIndex, SERVFILE_INFO_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poServLocationIndex, SERVLOCFILE_INFO_IDX);
        #ifdef IMSI_MDN_RELATION
        USERINFO_ATTACH_STRING_INDEX(m_poImsiMdnRelationIndex, IMSIMDNRELATFILE_INFO_IDX);
        #endif
        USERINFO_ATTACH_STRING_INDEX(m_poServIdentIndex, SERVIDENTFILE_INFO_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poServProductIndex, SERVPRODFILE_INFO_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poServAcctIndex, SERVACCTFILE_ACCT_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poAcctIndex, ACCTFILE_INFO_IDX);
        #ifdef GROUP_CONTROL
        USERINFO_ATTACH_INT_INDEX(m_poGroupInfoIndex, GROUPFILE_INFO_IDX);
        USERINFO_ATTACH_STRING_INDEX(m_poGroupMemberIndex, GROUPFILE_MEMBER_IDX);
        USERINFO_ATTACH_STRING_INDEX(m_poNumberGroupIndex, NUMBERFILE_GROUP_IDX);
        USERINFO_ATTACH_STRING_INDEX(m_poAccNbrIndex, ACCNBROFFERTYPEFILE_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poAccNbrItemIndex, ACCNBROFFERTYPEITEMFILE_IDX);
        #endif
        USERINFO_ATTACH_INT_INDEX(m_poProdOfferInsIndex, PRODOFFERINSFILE_IDX);
        USERINFO_ATTACH_INT_INDEX(m_poOfferDetailInsOfferInsIndex, OFFERDETAILINS_IDX);
        USERINFO_ATTACH_STRING_INDEX(m_poNpIndex, NPINFOFILE_IDX);
    }
    
    if(m_bAttached)
		bindData();
}

UserInfoMapBase::~UserInfoMapBase()
{
    /*if(m_poUserInfoBase){
        delete m_poUserInfoBase;
        m_poUserInfoBase = 0;
    }*/
    
    freeAll();
}

/*void UserInfoMapBase::createThread()
{
    pthread_t iThreadID;
    
    //m_oUserInfoManage.iOperMode = iOperMode;
    if(pthread_create(&iThreadID, NULL, &threadRun, (void*)this)){
        Log::log(0,"创建线程失败,进程退出!");
        throw 1;
    }
    
    usleep(100000);
}*/

void UserInfoMapBase::setAllData(int iMode, int * iOper)
{
    int iTemp = ALL_INDEX;
    
    for(iTemp = ALL_INDEX; iTemp <= NP_DATA; iTemp ++){
        if(iMode != iOper[iTemp])
            continue;
        
        switch(iTemp){
            case ALL_INDEX:
                setAllIndex();
                break;
                
            default:
                setFileData(iTemp);
                break;
        }
        
        iOper[iTemp] = 3;
    }
}

bool UserInfoMapBase::sortFileLen(const vectorData & m1, const vectorData & m2)
{
	return m1.lLen > m2.lLen;
}

void UserInfoMapBase::assignData()
{
    int i = CUST_DATA, j = CUST_DATA;
    
    struct vectorData oTotalLen[50];
    vector<struct vectorData> oVectorLen;
    
    memset(oTotalLen, 0, sizeof(oTotalLen));
    
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poCustData->getTotal()*sizeof(CustInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poProductData->getTotal()*sizeof(ProductInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServData->getTotal()*sizeof(ServInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServTypeData->getTotal()*sizeof(ServTypeInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServLocationData->getTotal()*sizeof(ServLocationInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServStateData->getTotal()*sizeof(ServStateInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServAttrData->getTotal()*sizeof(ServAttrInfo);
    #ifdef IMSI_MDN_RELATION
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poImsiMdnRelationData->getTotal()*sizeof(ImsiMdnRelation);
    #endif
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServIdentData->getTotal()*sizeof(ServIdentInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServProductData->getTotal()*sizeof(ServProductInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServProductAttrData->getTotal()*sizeof(ServProductAttrInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poServAcctData->getTotal()*sizeof(ServAcctInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poAcctData->getTotal()*sizeof(AcctInfo);
    #ifdef GROUP_CONTROL
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poGroupInfoData->getTotal()*sizeof(GroupInfo);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poGroupMemberData->getTotal()*sizeof(GroupMember);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poNumberGroupData->getTotal()*sizeof(NumberGroup);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrData->getTotal()*sizeof(AccNbrOfferType);
    #endif
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsData->getTotal()*sizeof(ProdOfferIns);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAttrData->getTotal()*sizeof(ProdOfferInsAttr);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poOfferDetailInsData->getTotal()*sizeof(OfferDetailIns);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAgreementData->getTotal()*sizeof(ProdOfferInsAgreement);
    oTotalLen[i++].lLen = m_oUserInfoManage.m_oUserInfoBase.m_poNpData->getTotal()*sizeof(NpInfo);
    
    for(j = CUST_DATA; j < i; j ++){
        oTotalLen[j].iOperType = j;
        
        oVectorLen.push_back(oTotalLen[j]);
    }
    
    sort(oVectorLen.begin(), oVectorLen.end(), sortFileLen);
    
    long lLen1 = 0, lLen2 = 0;
    vector<struct vectorData>::iterator rvLen;
    for(rvLen= oVectorLen.begin(); rvLen!=oVectorLen.end(); rvLen++){
        if(lLen1 >= lLen2){
            lLen2 += (*rvLen).lLen;
            m_oUserInfoManage.iOperMode[(*rvLen).iOperType] = 2;
        }else{
            lLen1 += (*rvLen).lLen;
            m_oUserInfoManage.iOperMode[(*rvLen).iOperType] = 1;
        }
    }
    
    if(lLen1 >= lLen2)
        m_oUserInfoManage.iOperMode[ALL_INDEX] = 2;
    else
        m_oUserInfoManage.iOperMode[ALL_INDEX] = 1;
}

void UserInfoMapBase::setAllIndex()
{
    unsigned long lLen = 0;
    char * sDes = 0;
    char * sSrc = 0;
    
    // COPY INDEX
    USERINFO_COPY_IDX(sDes, m_poCustIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poCustIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poProductIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poProductIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poServIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poServIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poServLocationIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poServLocationIndex, lLen);
#ifdef IMSI_MDN_RELATION
    USERINFO_COPY_IDX(sDes, m_poImsiMdnRelationIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poImsiMdnRelationIndex, lLen);
#endif
    USERINFO_COPY_IDX(sDes, m_poServIdentIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poServIdentIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poServProductIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poServProductIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poServAcctIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poServAcctIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poAcctIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poAcctIndex, lLen);
#ifdef GROUP_CONTROL
    USERINFO_COPY_IDX(sDes, m_poGroupInfoIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poGroupInfoIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poGroupMemberIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poGroupMemberIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poNumberGroupIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poNumberGroupIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poAccNbrIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poAccNbrItemIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrItemIndex, lLen);
#endif
    USERINFO_COPY_IDX(sDes, m_poProdOfferInsIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poOfferDetailInsOfferInsIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poOfferDetailInsOfferInsIndex, lLen);
    USERINFO_COPY_IDX(sDes, m_poNpIndex, sSrc, m_oUserInfoManage.m_oUserInfoBase.m_poNpIndex, lLen);
}

void UserInfoMapBase::setFileData(int iOperMode)
{
    // DATA
    switch(iOperMode){
        case ALL_DATA:{
                USERINFO_ATTACH_MAP_DATA(m_poCustData, CustInfo, m_oUserInfoManage.m_sFullName.sCustName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poCustData);
                //m_poCustData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poProductData, ProductInfo, m_oUserInfoManage.m_sFullName.sProductName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poProductData);
                //m_poProductData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServData, ServInfo, m_oUserInfoManage.m_sFullName.sServName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServData);
                //m_poServData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServTypeData, ServTypeInfo, m_oUserInfoManage.m_sFullName.sServTypeName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServTypeData);
                //m_poServTypeData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServLocationData, ServLocationInfo, m_oUserInfoManage.m_sFullName.sServLocName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServLocationData);
                //m_poServLocationData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServStateData, ServStateInfo, m_oUserInfoManage.m_sFullName.sServStateName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServStateData);
                //m_poServStateData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServAttrData, ServAttrInfo, m_oUserInfoManage.m_sFullName.sServAttrName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServAttrData);
                //m_poServAttrData->munmapFile();
                #ifdef IMSI_MDN_RELATION
                USERINFO_ATTACH_MAP_DATA(m_poImsiMdnRelationData, ImsiMdnRelation, m_oUserInfoManage.m_sFullName.sImsiMdnRelatName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poImsiMdnRelationData);
                //m_poImsiMdnRelationData->munmapFile();
                #endif
                USERINFO_ATTACH_MAP_DATA(m_poServIdentData, ServIdentInfo, m_oUserInfoManage.m_sFullName.sServIdenName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServIdentData);
                //m_poServIdentData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServProductData, ServProductInfo, m_oUserInfoManage.m_sFullName.sServProdName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServProductData);
                //m_poServProductData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServProductAttrData, ServProductAttrInfo, m_oUserInfoManage.m_sFullName.sServProdAttrName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServProductAttrData);
                //m_poServProductAttrData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poServAcctData, ServAcctInfo, m_oUserInfoManage.m_sFullName.sServAcctName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poServAcctData);
                //m_poServAcctData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poAcctData, AcctInfo, m_oUserInfoManage.m_sFullName.sAcctName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poAcctData);
                //m_poAcctData->munmapFile();
                #ifdef GROUP_CONTROL
                USERINFO_ATTACH_MAP_DATA(m_poGroupInfoData, GroupInfo, m_oUserInfoManage.m_sFullName.sGroupInfoName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poGroupInfoData);
                //m_poGroupInfoData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poGroupMemberData, GroupMember, m_oUserInfoManage.m_sFullName.sGroupMemberName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poGroupMemberData);
                //m_poGroupMemberData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poNumberGroupData, NumberGroup, m_oUserInfoManage.m_sFullName.sNumberGroupName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poNumberGroupData);
                //m_poNumberGroupData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poAccNbrData, AccNbrOfferType, m_oUserInfoManage.m_sFullName.sACCNbrOfferTypeName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrData);
                //m_poAccNbrData->munmapFile();
                #endif
                USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsData, ProdOfferIns, m_oUserInfoManage.m_sFullName.sOfferInsName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsData);
                //m_poProdOfferInsData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsAttrData, ProdOfferInsAttr, m_oUserInfoManage.m_sFullName.sOfferInsAttrName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAttrData);
                //m_poProdOfferInsAttrData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poOfferDetailInsData, OfferDetailIns, m_oUserInfoManage.m_sFullName.sOfferInsDetailName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poOfferDetailInsData);
                //m_poOfferDetailInsData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsAgreementData, ProdOfferInsAgreement, m_oUserInfoManage.m_sFullName.sOfferInsAgrName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAgreementData);
                //m_poProdOfferInsAgreementData->munmapFile();
                USERINFO_ATTACH_MAP_DATA(m_poNpData, NpInfo, m_oUserInfoManage.m_sFullName.sNpName,
                    m_oUserInfoManage.m_oUserInfoBase.m_poNpData);
                //m_poNpData->munmapFile();
            }
            break;
            
        case CUST_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poCustData, CustInfo, m_oUserInfoManage.m_sFullName.sCustName,
                m_oUserInfoManage.m_oUserInfoBase.m_poCustData);
            break;
        case PROD_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poProductData, ProductInfo, m_oUserInfoManage.m_sFullName.sProductName,
                m_oUserInfoManage.m_oUserInfoBase.m_poProductData);
            break;
        case SERV_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServData, ServInfo, m_oUserInfoManage.m_sFullName.sServName,
                m_oUserInfoManage.m_oUserInfoBase.m_poServData);
            break;
        case SERVTYPE_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServTypeData, ServTypeInfo, m_oUserInfoManage.m_sFullName.sServTypeName,
                m_oUserInfoManage.m_oUserInfoBase.m_poServTypeData);
            break;
        case SERVLOC_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServLocationData, ServLocationInfo, m_oUserInfoManage.m_sFullName.sServLocName,
                m_oUserInfoManage.m_oUserInfoBase.m_poServLocationData);
            break;
        case SERVSTATE_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServStateData, ServStateInfo, m_oUserInfoManage.m_sFullName.sServStateName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServStateData);
            break;
        case SERVATTR_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServAttrData, ServAttrInfo, m_oUserInfoManage.m_sFullName.sServAttrName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServAttrData);
            break;
        case IMSIMDN_DATA:
            #ifdef IMSI_MDN_RELATION
              USERINFO_ATTACH_MAP_DATA(m_poImsiMdnRelationData, ImsiMdnRelation, m_oUserInfoManage.m_sFullName.sImsiMdnRelatName,
                  m_oUserInfoManage.m_oUserInfoBase.m_poImsiMdnRelationData);
            #endif
            break;
        case SERVIDENT_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServIdentData, ServIdentInfo, m_oUserInfoManage.m_sFullName.sServIdenName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServIdentData);
            break;
        case SERVPROD_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServProductData, ServProductInfo, m_oUserInfoManage.m_sFullName.sServProdName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServProductData);
            break;
        case SERVPRODATTR_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServProductAttrData, ServProductAttrInfo, m_oUserInfoManage.m_sFullName.sServProdAttrName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServProductAttrData);
            break;
        case SERVACCT_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poServAcctData, ServAcctInfo, m_oUserInfoManage.m_sFullName.sServAcctName,
              m_oUserInfoManage.m_oUserInfoBase.m_poServAcctData);
            break;
        case ACCT_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poAcctData, AcctInfo, m_oUserInfoManage.m_sFullName.sAcctName,
              m_oUserInfoManage.m_oUserInfoBase.m_poAcctData);
            break;
        #ifdef GROUP_CONTROL
        case GROUP_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poGroupInfoData, GroupInfo, m_oUserInfoManage.m_sFullName.sGroupInfoName,
              m_oUserInfoManage.m_oUserInfoBase.m_poGroupInfoData);
            break;
        case GROUPMEMBER_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poGroupMemberData, GroupMember, m_oUserInfoManage.m_sFullName.sGroupMemberName,
              m_oUserInfoManage.m_oUserInfoBase.m_poGroupMemberData);
            break;
        case NUMBERGROUP_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poNumberGroupData, NumberGroup, m_oUserInfoManage.m_sFullName.sNumberGroupName,
              m_oUserInfoManage.m_oUserInfoBase.m_poNumberGroupData);
        case ACCNBR_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poAccNbrData, AccNbrOfferType, m_oUserInfoManage.m_sFullName.sACCNbrOfferTypeName,
              m_oUserInfoManage.m_oUserInfoBase.m_poAccNbrData);
            break;
        #endif
        case OFFERINS_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsData, ProdOfferIns, m_oUserInfoManage.m_sFullName.sOfferInsName,
              m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsData);
            break;
        case OFFERINSATTR_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsAttrData, ProdOfferInsAttr, m_oUserInfoManage.m_sFullName.sOfferInsAttrName,
              m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAttrData);
            break;
        case OFFDETAILINS_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poOfferDetailInsData, OfferDetailIns, m_oUserInfoManage.m_sFullName.sOfferInsDetailName,
              m_oUserInfoManage.m_oUserInfoBase.m_poOfferDetailInsData);
            break;
        case OFFERINSAGGR_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poProdOfferInsAgreementData, ProdOfferInsAgreement, m_oUserInfoManage.m_sFullName.sOfferInsAgrName,
              m_oUserInfoManage.m_oUserInfoBase.m_poProdOfferInsAgreementData);
            break;
        case NP_DATA:
            USERINFO_ATTACH_MAP_DATA(m_poNpData, NpInfo, m_oUserInfoManage.m_sFullName.sNpName,
              m_oUserInfoManage.m_oUserInfoBase.m_poNpData);
            break;
    }
}

/*long UserInfoMapBase::getSHMIdxSize(char * pSHM)
{
    long lSize = 0;
    unsigned int *piTotal;

    #ifdef  USERINFO_REUSE_MODE
    piTotal =  pSHM + 16;
    #else
    piTotal =  (unsigned int *)(pSHM + 24);
    #endif
    
    unsigned int hashvalue;
    hashvalue = (unsigned int)((*piTotal)*0.8);
    hashvalue |= 1;
    unsigned int i = 2;

    while(i && (hashvalue > i))
        i = (i << 1);

    if(i){
        hashvalue = i - 1;
        
        if(hashvalue > 1)
            hashvalue = (hashvalue >> 1);
    }else
        hashvalue = ((~0)>>1);
    
    #ifdef  USERINFO_REUSE_MODE
    lSize = (hashvalue+1)*sizeof(unsigned int) + \
                ((*piTotal)+1)*sizeof(SHMIntList_A) + \
             	10*sizeof(unsigned int);
    #else
    lSize = hashvalue*sizeof(unsigned int) + \
				(*piTotal)*sizeof(SHMIntList) + \
				4*sizeof(unsigned int);
    #endif
    
    return lSize;
}*/

void UserInfoMapBase::freeAll()
{
    USERINFO_FREE_MAP_DATA(m_poCustData);
    USERINFO_FREE_MAP_DATA(m_poProductData);
    USERINFO_FREE_MAP_DATA(m_poServData);
    USERINFO_FREE_MAP_DATA(m_poServTypeData);
    USERINFO_FREE_MAP_DATA(m_poServLocationData);
    USERINFO_FREE_MAP_DATA(m_poServStateData);
    USERINFO_FREE_MAP_DATA(m_poServAttrData);
    #ifdef IMSI_MDN_RELATION
    USERINFO_FREE_MAP_DATA(m_poImsiMdnRelationData);
    #endif
    USERINFO_FREE_MAP_DATA(m_poServIdentData);
    USERINFO_FREE_MAP_DATA(m_poServProductData);
    USERINFO_FREE_MAP_DATA(m_poServProductAttrData);
    USERINFO_FREE_MAP_DATA(m_poServAcctData);
    USERINFO_FREE_MAP_DATA(m_poAcctData);
    #ifdef GROUP_CONTROL
    USERINFO_FREE_MAP_DATA(m_poGroupInfoData);
    USERINFO_FREE_MAP_DATA(m_poGroupMemberData);
    USERINFO_FREE_MAP_DATA(m_poNumberGroupData);
    USERINFO_FREE_MAP_DATA(m_poAccNbrData);
    #endif
    USERINFO_FREE_MAP_DATA(m_poProdOfferInsData);
    USERINFO_FREE_MAP_DATA(m_poProdOfferInsAttrData);
    USERINFO_FREE_MAP_DATA(m_poOfferDetailInsData);
    USERINFO_FREE_MAP_DATA(m_poProdOfferInsAgreementData);
    USERINFO_FREE_MAP_DATA(m_poNpData);
    
    USERINFO_FREE_MAP_DATA(m_poCustIndex);
    USERINFO_FREE_MAP_DATA(m_poProductIndex);
    USERINFO_FREE_MAP_DATA(m_poServIndex);
    USERINFO_FREE_MAP_DATA(m_poServLocationIndex);
    #ifdef IMSI_MDN_RELATION
    USERINFO_FREE_MAP_DATA(m_poImsiMdnRelationIndex);
    #endif
    USERINFO_FREE_MAP_DATA(m_poServIdentIndex);
    USERINFO_FREE_MAP_DATA(m_poServProductIndex);
    USERINFO_FREE_MAP_DATA(m_poServAcctIndex);
    USERINFO_FREE_MAP_DATA(m_poAcctIndex);
    #ifdef GROUP_CONTROL
    USERINFO_FREE_MAP_DATA(m_poGroupInfoIndex);
    USERINFO_FREE_MAP_DATA(m_poGroupMemberIndex);
    USERINFO_FREE_MAP_DATA(m_poNumberGroupIndex);
    USERINFO_FREE_MAP_DATA(m_poAccNbrIndex);
    USERINFO_FREE_MAP_DATA(m_poAccNbrItemIndex);
    #endif
    USERINFO_FREE_MAP_DATA(m_poProdOfferInsIndex);
    USERINFO_FREE_MAP_DATA(m_poOfferDetailInsOfferInsIndex);
    USERINFO_FREE_MAP_DATA(m_poNpIndex);
}

void UserInfoMapBase::bindData()
{
    m_poCust = (CustInfo *)(*m_poCustData);
    m_poProduct = (ProductInfo *)(*m_poProductData);
    m_poServ = (ServInfo *)(*m_poServData);
    //m_poServTypeMap = (ServTypeInfo *)(*m_poServTypeData);
    m_poServLocation = (ServLocationInfo *)(*m_poServLocationData);
    m_poServState = (ServStateInfo *)(*m_poServStateData);
    m_poServAttr = (ServAttrInfo *)(*m_poServAttrData);
    #ifdef IMSI_MDN_RELATION
    m_poImsiMdnRelation = (ImsiMdnRelation *)(*m_poImsiMdnRelationData);
    #endif
    m_poServIdent = (ServIdentInfo *)(*m_poServIdentData);
    m_poServProduct = (ServProductInfo *)(*m_poServProductData);
    m_poServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
    m_poServAcct = (ServAcctInfo *)(*m_poServAcctData);
    m_poAcct = (AcctInfo *)(*m_poAcctData);
    #ifdef GROUP_CONTROL
    m_poGroupInfo = (GroupInfo *)(*m_poGroupInfoData);
    m_poGroupMember = (GroupMember *)(*m_poGroupMemberData);
    m_poNumberGroup = (NumberGroup *)(*m_poNumberGroupData);
    m_poAccNbr = (AccNbrOfferType *)(*m_poAccNbrData);
    #endif
    m_poProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);
    m_poProdOfferInsAttr = (ProdOfferInsAttr *)(*m_poProdOfferInsAttrData);
    m_poOfferDetailIns = (OfferDetailIns *)(*m_poOfferDetailInsData);
    m_poProdOfferInsAgreement = (ProdOfferInsAgreement *)(*m_poProdOfferInsAgreementData);
    m_poNp = (NpInfo *)(*m_poNpData);
}

UserInfoMapBase::operator bool()
{
	return m_bAttached;
}

