/*VER: 10*/ 
#include "UserInfoCtl.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include <string.h>
#include "Stack.h"
#include <iostream>
#include "Log.h"
#include "WorkFlow.h"
#include "ParamDefineMgr.h"
#include "LoadUserInfoMgr.h"
#include "CommonMacro.h"
#include "Process.h"
#include "BillingCycleMgr.h"

using namespace std;

CSemaphore * UserInfoAdmin::m_poServStateLock = 0;
CSemaphore * UserInfoAdmin::m_poServLoadInsertLock = 0;
CSemaphore * UserInfoAdmin::m_poServLoadUpdateLock = 0;


OfferMgr * UserInfoAdmin::m_poOfferMgr = 0;

bool UserInfoAdmin::m_bSPOK = false;
bool UserInfoAdmin::m_bSPAOK = false;
bool UserInfoAdmin::m_bPOIOK = false;
bool UserInfoAdmin::m_bPOIDOK = false;
bool UserInfoAdmin::m_bLogClose = false;
bool UserInfoAdmin::m_bFilterServ = false;

//##ModelId=42391DCD02AF
UserInfoAdmin::UserInfoAdmin(bool bForce)
{
    init(bForce);
}
UserInfoAdmin::UserInfoAdmin()
{
    init(false);
}

void UserInfoAdmin::init(bool bForce)
{
    m_bForce = bForce;
    if (!m_poOfferMgr) {
        m_poOfferMgr = new OfferMgr;
        if (!m_poOfferMgr) {
            THROW(MBC_UserInfoCtl);
        }
        m_poOfferMgr->load();
    }
    if (!m_poServStateLock) {
        char sTemp[32];
        memset(sTemp,0,sizeof(sTemp));
        m_poServStateLock = new CSemaphore;
        sprintf (sTemp, "%d", SERV_STATE_LOCK);
        m_poServStateLock->getSem (sTemp);
        memset(sTemp,0,sizeof(sTemp));
        m_poServLoadInsertLock = new CSemaphore;
        sprintf (sTemp, "%d", SERV_LOAD_INSERT);
        m_poServLoadInsertLock->getSem (sTemp);
        memset(sTemp,0,sizeof(sTemp));
        m_poServLoadUpdateLock = new CSemaphore;
        sprintf (sTemp, "%d", SERV_LOAD_UPDATE);
        m_poServLoadUpdateLock->getSem (sTemp);
    }
    isSuccess = false;
    char countbuf[2];       
    if (ParamDefineMgr::getParam(USERINFO_SEGMENT, "LOG_CLOSE", countbuf,1)) {
        if (atoi(countbuf) == 1) {
            m_bLogClose =  true;
        }
    }
    memset(countbuf, 0, sizeof(countbuf));
    if (ParamDefineMgr::getParam(USERINFO_SEGMENT, "UPLOAD_SERV_FILTER", countbuf,1)) {
        if (atoi(countbuf) == 1) {
            m_bFilterServ =  true;
        }
    }
}

//##ModelId=4249068903D7
void sortCustPlan(unsigned int u1, unsigned int u2)
{
}

int  UserInfoAdmin::setRunInfo(const int iMode,char * msg)
{
    this->m_iRunMode = iMode;
    this->strMsg = msg;
    return 0;
}

int UserInfoAdmin::run()
{
    try {
        pthread_detach(pthread_self() );
    } catch (...) {
        //Log::log(0,"加载%s异常!",strMsg);
        cout<<"分离线程 "<<strMsg<<" 时发生异常!"<<endl;
        throw 1;
    }
    Log::log(0,"启动 %s 的加载",strMsg);
    this->loadTable(this->m_iRunMode);
    return 0;
}

int UserInfoAdmin::runDB()
{
    try {
        pthread_detach(pthread_self() );
    } catch (...) {
        //Log::log(0,"加载%s异常!",strMsg);
        cout<<"分离线程 "<<strMsg<<" 时发生异常!"<<endl;
        throw 1;
    }
    Log::log(0,"启动 %s 的加载",strMsg);
    this->loadTableDB(this->m_iRunMode);
    return 0;
}

int UserInfoAdmin::runMEM()
{
    try {
        pthread_detach(pthread_self() );
    } catch (...) {
        //Log::log(0,"加载%s异常!",strMsg);
        cout<<"分离线程 "<<strMsg<<" 时发生异常!"<<endl;
        throw 1;
    }
    Log::log(0,"启动 %s 的加载",strMsg);
    this->loadTableMEM(this->m_iRunMode);
    return 0;
}

void UserInfoAdmin::loadTable(const int iMode)
{
    switch (iMode) {
    case A_SERV_TYPE_CHANGE:
        {
            try {
                this->loadServTypeInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 A_SERV_TYPE_CHANGE 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 A_SERV_TYPE_CHANGE 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_LOCATION:
        {
            try {
                this->loadServLocationInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_LOCATION 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_LOCATION 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_STATE_ATTR:
        {
            try {
                this->loadServStateInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_STATE_ATTR 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_STATE_ATTR 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_ATTR:
        {
            try {
                this->loadServAttrInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_ATTR 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_ATTR 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_IDEN:
        {
#ifdef  IMSI_MDN_RELATION
            try {
                Log::log(0,"启动 IMSI_MDN_RELATION 的加载");
                this->loadImsiMdnRelation();
                Log::log(0,"完成 IMSI_MDN_RELATION 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 IMSI_MDN_RELATION 时发生异常!"<<endl;
                throw 1;
            }
#endif
            try {
                this->loadServIdentInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_IDENTIFICATION 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_IDENTIFICATION 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_PRODUCT_ANDATTR:
        {
            try {
                this->loadServProductInfo();
                this->loadServProductAttrInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_PRODUCT_ANDATTR 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_PRODUCT_ANDATTR 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_PRODUCT:
        {
            try {
                this->loadServProductInfo();
                LoadUserInfoMgr::thread_num++;
                m_bSPOK = true;
                Log::log(0,"完成 SERV_PRODUCT 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_PRODUCT 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case SERV_ACCT:
        {
            try {
                this->loadServAcctInfo();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_ACCT 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_ACCT 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case ACCT:
        {
            try {
                this->loadAcctInfo();
#ifdef GROUP_CONTROL
                this->loadGroupInfo();
                this->loadGroupMember();
                this->loadNumberGroup();
#endif
                LoadUserInfoMgr::thread_num++;
                LoadUserInfoMgr::bAcct = true;
                Log::log(0,"完成 ACCT 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 ACCT 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case PRODUCT_OFFER_INFO:
        {
            try {
                this->loadProdOfferIns();
                this->loadProdOfferInsAttr();
                this->loadProdOfferInsDetail();
                //add by zhaoziwei
                this->loadProdOfferInsAgreement();
                //end
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 PRODUCT_OFFER_INFO 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 PRODUCT_OFFER_INFO 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
#ifdef  GROUP_CONTROL
    case A_ACC_NBR_OFFER_TYPE:
        {
            try {
                this->loadAccNbrOfferType();
                LoadUserInfoMgr::thread_num++;
			    Log::log(0,"完成 A_ACC_NBR_OFFER_TYPE 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 A_ACC_NBR_OFFER_TYPE 时发生异常!"<<endl;
                throw 1;
            }
        }break;
#endif

    case OFFER_INSTANCE:
        {
            try {
                this->loadProdOfferIns();
                m_bPOIOK = true;
                this->loadProdOfferInsAttr();
                //add by zhaoziwei
                this->loadProdOfferInsAgreement();
                //end
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 PRODUCT_OFFER_INSTANCE 的加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 PRODUCT_OFFER_INSTANCE 时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
		case B_NP:
			{
					try
					{
						this->loadNpInfo();
						LoadUserInfoMgr::thread_num++;
						Log::log(0,"完成 B_NP 的加载");
					}
					catch(Exception &e)
					{
						cout<< e.descript()<<endl;
						throw e;
					}				
					catch(...){
						cout<<"加载 NP 时发生异常!"<<endl;
						throw 1;
					}
			}
			break;
    default:
        Log::log(0,"加载对象不存在");
        break;
    }
    this->isSuccess = true;
}

void UserInfoAdmin::loadTableDB(const int iMode)
{
    switch (iMode) {
    case SERV_PRODUCT_ATTR:
        {
            try {
                this->loadServProductAttrInfoDB();
                LoadUserInfoMgr::thread_num++;
                m_bSPAOK = true;
                Log::log(0,"完成 SERV_PRODUCT_ATTR 的数据库加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_PRODUCT_ATTR 的数据库时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case INSTANCE_DETAIL:
        {
            try {
                this->loadProdOfferInsDetailDB();
                LoadUserInfoMgr::thread_num++;
                m_bPOIDOK = true;
                Log::log(0,"完成 PRODUCT_OFFER_INSTANCE_DETAIL 的数据库加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 PRODUCT_OFFER_INSTANCE_DETAIL 的数据库时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    default:
        Log::log(0,"加载对象不存在");
        break;
    }
    this->isSuccess = true;
}

void UserInfoAdmin::loadTableMEM(const int iMode)
{
    switch (iMode) {
    case SERV_PRODUCT_ATTR:
        {
            try {
                this->loadServProductAttrInfoMEM();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 SERV_PRODUCT_ATTR 的内存加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 SERV_PRODUCT_ATTR 的内存时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    case INSTANCE_DETAIL:
        {
            try {
                this->loadProdOfferInsDetailMEM();
                LoadUserInfoMgr::thread_num++;
                Log::log(0,"完成 PRODUCT_OFFER_INSTANCE_DETAIL 的内存加载");
            } catch (Exception &e) {
                cout<< e.descript()<<endl;
                throw e;
            } catch (...) {
                cout<<"加载 PRODUCT_OFFER_INSTANCE_DETAIL 的内存时发生异常!"<<endl;
                throw 1;
            }
        }
        break;
    default:
        Log::log(0,"加载对象不存在");
        break;
    }
    this->isSuccess = true;
}

void UserInfoAdmin::unloadTable(const int iMode)
{
    #define UNLOAD(X) \
				if(X) \
                if (X->exist()) { \
                      if(!m_bForce)\
                        if(X->connnum() > 1) \
                            { \
                                THROW(MBC_UserInfoCtl+54); \
                            } \
                        X->remove (); \
                }
    switch (iMode) {
    case A_SERV_TYPE_CHANGE:
        {
            UNLOAD(m_poServTypeData);
        }
        break;
    case SERV_LOCATION:
        {
            UNLOAD(m_poServLocationData);
            UNLOAD(m_poServLocationIndex);
        }
        break;
    case SERV_STATE_ATTR:
        {
            UNLOAD(m_poServStateData);
        }
        break;
    case SERV_ATTR:
        {
            UNLOAD(m_poServAttrData);
        }
        break;
    case SERV_IDEN:
        {
            UNLOAD(m_poServIdentData);
            UNLOAD(m_poServIdentIndex);
#ifdef  IMSI_MDN_RELATION
            UNLOAD(m_poImsiMdnRelationData);
            UNLOAD(m_poImsiMdnRelationIndex);
#endif
        }
        break;
    case SERV_PRODUCT_ANDATTR:
        {
            UNLOAD(m_poServProductData);
            UNLOAD(m_poServProductIndex);
            UNLOAD(m_poServProductAttrData);
        }
        break;
    case SERV_ACCT:
        {
            UNLOAD(m_poServAcctData);
            UNLOAD(m_poServAcctIndex);
        }
        break;
    case ACCT:
        {
            UNLOAD(m_poAcctData);
            UNLOAD(m_poAcctIndex);

#ifdef GROUP_CONTROL
            UNLOAD(m_poGroupInfoData);
            UNLOAD(m_poGroupInfoIndex);
            UNLOAD(m_poGroupMemberData);
            UNLOAD(m_poGroupMemberIndex);
            UNLOAD(m_poNumberGroupData);
            UNLOAD(m_poNumberGroupIndex);
#endif
        }
        break;
    case PRODUCT_OFFER_INFO:
        {
            UNLOAD(m_poProdOfferInsData);
            UNLOAD(m_poProdOfferInsIndex);
            UNLOAD(m_poOfferDetailInsData);
            UNLOAD(m_poOfferDetailInsOfferInsIndex);
            UNLOAD(m_poProdOfferInsAttrData);
            //add by zhaoziwei
            UNLOAD(m_poProdOfferInsAgreementData);
            //end
        }
        break;
#ifdef  GROUP_CONTROL
    case A_ACC_NBR_OFFER_TYPE:
        {
            UNLOAD(m_poAccNbrData);
            UNLOAD(m_poAccNbrIndex);
            UNLOAD(m_poAccNbrItemIndex);
        }break;
#endif
		//开发测试 add by xn 2009.10.26
		case B_NP:
			{
				UNLOAD(m_poNpData);
				UNLOAD(m_poNpIndex);
			}
			break;
    default:
        Log::log(0,"卸载对象不存在");
        break;
    }
    Log::log(0,"卸载对象完成");
    //m_bAttached = false; 为了同时加载多个单表，这里不能置为 false
}

bool UserInfoAdmin::cleanOffset(const int iMode)
{       

    switch (iMode) {
    case A_SERV_TYPE_CHANGE:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 A_SERV_TYPE_CHANGE 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iServTypeOffset = 0;
            }
        }
        break;
    case SERV_LOCATION:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_LOCATION 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iLocationOffset = 0;
            }
        }
        break;
    case SERV_STATE_ATTR:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_STATE_ATTR 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iStateOffset = 0;
            }
        }
        break;
    case SERV_ATTR:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_ATTR 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iAttrOffset = 0;
            }
        }
        break;
    case SERV_IDEN:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_IDEN 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iServIdentOffset = 0;
            }
        }
        break;
    case SERV_PRODUCT_ANDATTR:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_PRODUCT_ANDATTR 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iServProductOffset = 0;
            }
        }
        break;
    case SERV_ACCT:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 SERV_ACCT 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iServAcctOffset = 0;
            }
        }
        break;
    case ACCT:
        {
            int m_iCurOffset = m_poCustData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 ACCT 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poCust) {
                m_poCust = (CustInfo *)(*m_poCustData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poCust[i].m_iAcctOffset = 0;
            }
/*  m_poGroupInfoData 都UNLOAD了，就不能得到m_poGroupInfoData->getCount()了         
#ifdef GROUP_CONTROL
            m_iCurOffset = m_poGroupInfoData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"固话群信息不存在，加载 A_GROUP_INFO 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poGroupInfo) {
                m_poGroupInfo = (GroupInfo *)(*m_poGroupInfoData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poGroupInfo[i].m_iNextOffset = 0;
            }
            m_iCurOffset = m_poGroupMemberData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"群成员信息不存在，加载 A_GROUP_MEMBER 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poGroupMember) {
                m_poGroupMember = (GroupMember *)(*m_poGroupMemberData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poGroupMember[i].m_iNextOffset = 0;
            }
            m_iCurOffset = m_poNumberGroupData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"号码群关系信息不存在，加载 A_NUMBER_GROUP_RELATION 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poNumberGroup) {
                m_poNumberGroup = (NumberGroup *)(*m_poNumberGroupData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poNumberGroup[i].m_iNextOffset = 0;
            }
#endif
*/

        }
        break;
    case PRODUCT_OFFER_INFO:
        {
            int m_iCurOffset = m_poServData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"用户信息不存在，加载 PRODUCT_OFFER_INSINFO 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poServ) {
                m_poServ = (ServInfo *)(*m_poServData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poServ[i].m_iBaseOfferOffset = 0;
                m_poServ[i].m_iOfferDetailOffset = 0;
            }
            m_iCurOffset = m_poCustData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"客户信息不存在，加载 PRODUCT_OFFER_INSINFO 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poCust) {
                m_poCust = (CustInfo *)(*m_poCustData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poCust[i].m_iOfferDetailOffset = 0;

            }
            m_iCurOffset = m_poAcctData->getCount();
            if (m_iCurOffset == 0) {
                Log::log(0,"帐户信息不存在，加载 PRODUCT_OFFER_INSINFO 失败");
                return false;
            }
            if (m_iCurOffset>0 && !m_poAcct) {
                m_poAcct = (AcctInfo *)(*m_poAcctData);
            }
            for (int i=1;i<=m_iCurOffset;i++) {
                m_poAcct[i].m_iOfferDetailOffset = 0;
            }
        }
        break;
    default:
        Log::log(0,"对象不存在");
        break;
    }
    Log::log(0,"关联信息清理完成");
    return true;
}

void UserInfoAdmin::loadPrefix()
{
    Log::log(0,"启动 %s 的加载",strMsg);
    try {
        this->loadCustInfo();
        this->loadProductInfo();
        this->loadServInfo();
    } catch (Exception &e) {
        cout<< e.descript()<<endl;
        throw e;
    }
    Log::log(0,"完成 %s 的加载",strMsg);
}

void UserInfoAdmin::bindDataEx(const int iMode)
{
    switch (iMode) {
    case ALL_INFO:
        {
            this->bindData();
        }
    case A_SERV_TYPE_CHANGE:
        {
            //没有
        }
        break;
    case SERV_LOCATION:
        {
            m_poServLocation = (ServLocationInfo *)(*m_poServLocationData);
        }
        break;
    case SERV_STATE_ATTR:
        {
            m_poServState = (ServStateInfo *)(*m_poServStateData);
        }
        break;
    case SERV_ATTR:
        {
            m_poServAttr = (ServAttrInfo *)(*m_poServAttrData);
        }
        break;
    case SERV_IDEN:
        {  
            m_poServIdent = (ServIdentInfo *)(*m_poServIdentData);
//--xgs-- add            
#ifdef  IMSI_MDN_RELATION
        		m_poImsiMdnRelation = (ImsiMdnRelation *)m_poImsiMdnRelationData;
#endif        
        }
        break;
    case SERV_PRODUCT_ANDATTR:
        {
            m_poServProduct = (ServProductInfo *)(*m_poServProductData);
            m_poServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
        }
        break;
    case SERV_ACCT:
        {
            m_poServAcct = (ServAcctInfo *)(*m_poServAcctData);
        }
        break;
    case ACCT:
        {
            m_poAcct = (AcctInfo *)(*m_poAcctData);
#ifdef GROUP_CONTROL
            m_poGroupInfo = (GroupInfo *)(*m_poGroupInfoData);
            m_poGroupMember = (GroupMember *)(*m_poGroupMemberData);
            m_poNumberGroup = (NumberGroup *)(*m_poNumberGroupData);
#endif
        }
        break;
    case PRODUCT_OFFER_INFO:
        {
            m_poProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);
            m_poOfferDetailIns = (OfferDetailIns *)(*m_poOfferDetailInsData);
            m_poProdOfferInsAttr = (ProdOfferInsAttr *)(*m_poProdOfferInsAttrData);
            //add by zhaoziwei
            m_poProdOfferInsAgreement = (ProdOfferInsAgreement *)(*m_poProdOfferInsAgreementData);
            //end
        }
        break;
		//开发测试 add by xn 2009.10.26
		case B_NP:
			m_poNp = (NpInfo*)(*m_poNpData);
			break;
    default:
        Log::log(0,"绑定对象不存在");
        break;
    }
    Log::log(0,"对象绑定完成");
}

//##ModelId=42391ED4025D
void UserInfoAdmin::load()
{
    unload ();

    Log::log (0,  "LOAD CUST INFO..." );
    loadCustInfo ();

    Log::log (0,  "LOAD PRODUCT INFO..." );
    loadProductInfo ();

    Log::log (0,  "LOAD SERV INFO..." );
    loadServInfo ();

    Log::log (0,  "LOAD SERV TYPE INFO..." );
    loadServTypeInfo ();

    Log::log (0,  "LOAD SERV LOCATION INFO..." );
    loadServLocationInfo ();

    Log::log (0,  "LOAD SERV STATE INFO..." );
    loadServStateInfo ();

    Log::log (0,  "LOAD SERV ATTR INFO..." );
    loadServAttrInfo ();

    Log::log (0,  "LOAD SERV IDENTIFICATION INFO..." );
    loadServIdentInfo ();

    Log::log (0,  "LOAD SERV PRODUCT INFO..." );
    loadServProductInfo ();

    Log::log (0,  "LOAD SERV PRODUCT ATTR INFO..." );
    loadServProductAttrInfo ();

    Log::log (0,  "LOAD SERV ACCT INFO..." );
    loadServAcctInfo ();

    Log::log (0,  "LOAD ACCT INFO..." );
    loadAcctInfo ();

    Log::log (0,  "LOAD OFFER INSTANCE INFO..." );
    loadProdOfferIns();

    Log::log (0,  "LOAD OFFER INSTANCE DETAIL INFO..." );
    loadProdOfferInsDetail();

    Log::log (0,  "LOAD OFFER INSTANCE ATTRIBUTE INFO" );
    loadProdOfferInsAttr();

		//add by zhaoziwei
		Pprintf (0, false, 0, "LOAD OFFER INSTANCE AGREEMENT INFO" );
    loadProdOfferInsAgreement();
    //end

#ifdef GROUP_CONTROL
    Log::log (0,  "LOAD A_GROUP_INFO INFO" );
    loadGroupInfo();
    Log::log (0,  "LOAD A_GROUP_MEMBER INFO" );
    loadGroupMember();
    Log::log (0,  "LOAD A_NUMBER_GROUP_RELATION INFO" );
    loadNumberGroup();
#endif
#ifdef  GROUP_CONTROL
    Pprintf(0,false,0,"LOAD ACC_NBR_OFFER_TYPE INFO");
    loadAccNbrOfferType();
#endif

#ifdef IMSI_MDN_RELATION
    Log::log (0,  "LOAD A_IMSI_MDN_RELATION INFO" );
    loadImsiMdnRelation();
#endif
//开发测试 add by xn 2009.10.26
	Pprintf (0, false, 0, "LOAD B_NP INFO" );
	loadNpInfo();

    bindData ();
    Log::log (0, "loaduserinfo end");
	Pprintf (0, false, 0, "Load end");
}
#ifdef GROUP_CONTROL
void UserInfoAdmin::loadAccNbrOfferType()
{
    char sSql[2048]={0};
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"asaot","A_ACC_NBR_OFFER_TYPE",qry)) return;

    char countbuf[32];      
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ACC_NBR_OFFER_TYPE, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    int count = atoi(countbuf);

    m_poAccNbrData->create (CAL_COUNT(count));
    m_poAccNbrIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);
    m_poAccNbrItemIndex->create (CAL_COUNT(count));


    AccNbrOfferType* p=0;

    p = (AccNbrOfferType*)(*m_poAccNbrData);

    strcpy(sSql," select ITEM_ID,ACC_NBR,TO_CHAR(EFF_DATE,'YYYYMMDDHH24MISS'), "
           " TO_CHAR(EXP_DATE,'YYYYMMDDHH24MISS'),NVL(OFFER_TYPE,1) from A_ACC_NBR_OFFER_TYPE ");
    qry.setSQL (sSql);
    qry.open ();

    unsigned int iTemp=0;
    unsigned int i=0;

    while (qry.next()) {
        i = m_poAccNbrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+1);

        p[i].m_lItemID = qry.field(0).asLong ();
        strcpy (p[i].m_sAccNbr, qry.field(1).asString ());
        strcpy( p[i].m_sEffDate,qry.field(2).asString());
        strcpy( p[i].m_sExpDate,qry.field(3).asString());
        p[i].m_iOfferType = qry.field(4).asInteger ();
        m_poAccNbrItemIndex->add(p[i].m_lItemID,i);
        if (m_poAccNbrIndex->get(p[i].m_sAccNbr,&iTemp)) {
            if (strcmp(p[i].m_sEffDate ,p[iTemp].m_sEffDate)>=0) {
                p[i].m_iNextOffset = iTemp;
                m_poAccNbrIndex->add (p[i].m_sAccNbr, i);
                continue;
            }
            unsigned int *piTemp=&p[iTemp].m_iNextOffset;
            while ((*piTemp)&&(strcmp(p[i].m_sEffDate ,p[*piTemp].m_sEffDate)<0)) {
                piTemp=&p[*piTemp].m_iNextOffset;
            }
            p[i].m_iNextOffset =*piTemp;
            *piTemp =i;
        } else
            m_poAccNbrIndex->add (p[i].m_sAccNbr, i);
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

#endif

//##ModelId=42391ED40299
void UserInfoAdmin::unload()
{
    #define UNLOAD(X) \
				if(X) \
                if (X->exist()) { \
                      if(!m_bForce)\
                        if(X->connnum() > 1) \
                            { \
                                THROW(MBC_UserInfoCtl+54); \
                            } \
                        X->remove (); \
                }

    //##try to attach
    UNLOAD(m_poCustData);

    UNLOAD(m_poCustIndex);

    UNLOAD(m_poProductData);

    UNLOAD(m_poProductIndex);

    UNLOAD(m_poServData);

    UNLOAD(m_poServTypeData);

    UNLOAD(m_poServIndex);

    UNLOAD(m_poServLocationData);

    UNLOAD(m_poServLocationIndex);

    UNLOAD(m_poServStateData);

    UNLOAD(m_poServAttrData);

    UNLOAD(m_poServIdentData);

    UNLOAD(m_poServIdentIndex);

    UNLOAD(m_poServProductData);

    UNLOAD(m_poServProductIndex);

    UNLOAD(m_poServProductAttrData);

    UNLOAD(m_poServAcctData);

    UNLOAD(m_poServAcctIndex);

    UNLOAD(m_poAcctData);

    UNLOAD(m_poAcctIndex);

    UNLOAD(m_poProdOfferInsData);
    UNLOAD(m_poProdOfferInsIndex);
    UNLOAD(m_poOfferDetailInsData);
    UNLOAD(m_poOfferDetailInsOfferInsIndex);
    UNLOAD(m_poProdOfferInsAttrData);
		//add by zhaoziwei
		UNLOAD(m_poProdOfferInsAgreementData);
		//end

#ifdef GROUP_CONTROL
    UNLOAD(m_poGroupInfoData);
    UNLOAD(m_poGroupMemberData);
    UNLOAD(m_poNumberGroupData);
    UNLOAD(m_poGroupInfoIndex);
    UNLOAD(m_poGroupMemberIndex);
    UNLOAD(m_poNumberGroupIndex);
#endif
#ifdef  GROUP_CONTROL
    UNLOAD(m_poAccNbrData);
    UNLOAD(m_poAccNbrIndex);
    UNLOAD(m_poAccNbrItemIndex);
#endif
#ifdef IMSI_MDN_RELATION
    UNLOAD(m_poImsiMdnRelationData);
    UNLOAD(m_poImsiMdnRelationIndex);
#endif
#ifdef IMMEDIATE_VALID
	UNLOAD(m_poServInstRelationData);
	UNLOAD(m_poServInstIndex);
#endif
	//开发测试 add by xn 2009.10.26
	UNLOAD(m_poNpData);
	UNLOAD(m_poNpIndex);



    m_bAttached = false;
}

//##ModelId=4243D18002F6
void UserInfoAdmin::update()
{
}

bool UserInfoAdmin::getBillingCycleOffsetTime(int iOffset,char * strDate,TOCIQuery &qry)
{
    Date d;
	char sDate[16];
	memset(sDate,0,sizeof(sDate));
	strcpy(sDate,d.toString());
	BillingCycleMgr * pBCM = new BillingCycleMgr();
	if (!pBCM){
		Log::log (0, "获取帐期管理类失败，默认使用当前时间[%s]进行偏移过滤",sDate);
	}
	if(pBCM){
		BillingCycle * pCycle = pBCM->getBillingCycle (1, sDate);
		if(!pCycle){
			Log::log (0, "获取当前帐期失败，默认使用当前时间[%s]进行偏移过滤",sDate);
		}
		strcpy(sDate,pCycle->getStartDate());
	}
    char sql[1024];
    memset(sql,0,sizeof(sql));
    iOffset = iOffset -1;
    sprintf(sql, "select to_char(add_months(cycle_begin_date,-%d),'yyyymmddhh24miss') begin_date FROM billing_cycle where "
            " billing_cycle_type_id=1 AND to_date(%s,'yyyymmddhh24miss')>=cycle_begin_date and "
            " to_date(%s,'yyyymmddhh24miss')<cycle_end_date ",iOffset,sDate,sDate);
    qry.setSQL (sql);
    qry.open ();
    qry.next();
    strcpy(strDate,qry.field(0).asString());
    qry.commit();
    qry.close ();
	if(pBCM){
		delete pBCM;
		pBCM = NULL;
	}
	if(strlen(strDate) != 14){
		strDate[0]='\0';
	}
    return true;
}

bool UserInfoAdmin::getBillingCycleOffsetTime(int iOffset,char * strDate)
{
    Date d;
    char sql[1024];
    memset(sql,0,sizeof(sql));
	DEFINE_QUERY (qry);
    iOffset = iOffset -1;
    sprintf(sql, "select to_char(add_months(cycle_begin_date,-%d),'yyyymmddhh24miss') begin_date FROM billing_cycle where "
            " billing_cycle_type_id=1 AND to_date(%s,'yyyymmddhh24miss')>=cycle_begin_date and "
            " to_date(%s,'yyyymmddhh24miss')<cycle_end_date ",iOffset,d.toString(),d.toString());
    qry.setSQL (sql);
    qry.open ();
    qry.next();
    strcpy(strDate,qry.field(0).asString());
    qry.commit();
    qry.close ();
    return true;
}

//##ModelId=4247900203A0
void UserInfoAdmin::loadCustInfo()
{
    int count;
    char sql[2048];
    CustInfo * p;
    unsigned int i;
    DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"c","cust",qry)) return;

    /*strcpy (sql, "select count(*) from cust");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];      
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_CUST, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poCustData->create (CAL_COUNT(count));
    m_poCustIndex->create (CAL_COUNT(count));
#ifdef IMMEDIATE_VALID
    m_poServInstRelationData->create (CAL_COUNT(count));
    m_poServInstIndex->create (CAL_COUNT(count));
#endif

    p = (CustInfo *)(*m_poCustData);

    strcpy (sql, "select cust_id, cust_type_id cust_type, "
            "is_vip, nvl(parent_id, -1) parent_id, state "
            "from cust");

    qry.setSQL (sql);
    qry.open ();

    while (qry.next()) {
        i = m_poCustData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+1);

        p[i].m_lCustID = qry.field(0).asLong ();
        //p[i].m_iCustLocation = qry.field(2).asInteger ();
        strcpy (p[i].m_sCustType, qry.field(1).asString ());
  ////    p[i].m_bIsVip = *(qry.field(2).asString()) - 'F';
  ////    p[i].m_lParentID = qry.field(3).asLong ();
        strcpy (p[i].m_sState, qry.field(4).asString ());
        //strcpy (p[i].m_sEffDate, qry.field(6).asString ());
        //strcpy (p[i].m_sExpDate, qry.field(7).asString ());

        m_poCustIndex->add (p[i].m_lCustID, i);
    }
    qry.close ();
    updateLog(iSeq,qry);
}

/*#ifdef CREDIT_ADD_SERV
void UserInfoAdmin::loadAcctInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    pConn->connect(sUser, sPwd, sStr);
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int count,countBk;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"a","acct",qry)) return;
    AcctInfo * pAcct;
    CustInfo * pCust = (CustInfo *)(*m_poCustData);

    unsigned int i, lasti=0, k;
    unsigned int * uiTemp;
    long lastid = 0;

    int m = 0;

    qry.setSQL("select count(*) from acct_credit");
    qry.open (); qry.next ();
    count = qry.field(0).asInteger () +20;
    countBk = count-1;
    qry.close ();

    HashList<int >  * m_poAcctCreditIndex = new HashList<int>(count);
    AcctCreditInfo * m_poAcctCreditData = new AcctCreditInfo[count];
    AcctCreditInfo * m_poTemp;
    if (!m_poAcctCreditIndex || !m_poAcctCreditData) {
        THROW(MBC_UserInfoCtl+2);//借用
    }

    qry.setSQL("select acct_id,0 serv_id,nvl(credit_grade, 1500),nvl(credit_amount, 1500) from acct_credit");

    count = 1;
    qry.open();
    while (qry.next()) {
        if (count>countBk) THROW(MBC_UserInfoCtl+2);
        m_poTemp = m_poAcctCreditData+count;
        m_poTemp->m_lAcctID = qry.field(0).asLong();
        m_poTemp->m_lServID = qry.field(1).asLong();
        m_poTemp->m_iCreditGrade = qry.field(2).asInteger();
        m_poTemp->m_iCreditAmount = qry.field(3).asInteger();
        m_poTemp->m_iNextOffset = 0;
        if (m_poAcctCreditIndex->get(m_poTemp->m_lAcctID,&m)) {
            m_poTemp->m_iNextOffset = m;
        }
        m_poAcctCreditIndex->add(m_poTemp->m_lAcctID,count);
        count++;
    }
    qry.close();

    char countbuf[32];
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ACCT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poAcctData->create (CAL_COUNT(count));
    m_poAcctIndex->create (CAL_COUNT(count));

    pAcct = (AcctInfo *)(*m_poAcctData);

    qry.setSQL ("select acct_id, cust_id, state from acct a");
    qry.open ();

    while (qry.next()) {
        i = m_poAcctData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+2);

        pAcct[i].m_lAcctID = qry.field(0).asLong ();
        pAcct[i].m_lCustID = qry.field(1).asLong ();
        strcpy (pAcct[i].m_sState, qry.field(2).asString ());
        if (m_poAcctCreditIndex->get(pAcct[i].m_lAcctID,&m)) {
            pAcct[i].m_iCreditGrade = m_poAcctCreditData[m].m_iCreditGrade;
            while (m) {
                pAcct[i].m_iCreditAmount = pAcct[i].m_iCreditAmount +m_poAcctCreditData[m].m_iCreditAmount;
                m = m_poAcctCreditData[m].m_iNextOffset;
            }
        } else {
            pAcct[i].m_iCreditGrade = 1500;
            pAcct[i].m_iCreditAmount = 1500;
        }

        if (pAcct[i].m_lCustID != 0) {
            if (!m_poCustIndex->get(pAcct[i].m_lCustID, &k)) {
                Log::log (0, "帐户找不到对应的客户，帐户ID:%ld", pAcct[i].m_lAcctID);
                continue;
            }
            uiTemp = &(pCust[k].m_iAcctOffset);
            while (*uiTemp) {
                pAcct[i].m_iNextOffset=*uiTemp;
                break;
            }
            *uiTemp = i;
            m_poAcctIndex->add (pAcct[i].m_lAcctID, i);
        }
    }

    qry.close ();
    updateLog(iSeq,qry);
    delete [] m_poAcctCreditData;
    m_poAcctCreditData = NULL;
    delete m_poAcctCreditIndex;
    m_poAcctCreditIndex = NULL;
    pConn->disconnect();
    delete pConn;
}
#else*/
void UserInfoAdmin::loadAcctInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int count;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"a","acct",qry)) return;
    AcctInfo * pAcct;
    CustInfo * pCust = (CustInfo *)(*m_poCustData);

    unsigned int i, lasti=0, k;
    unsigned int * uiTemp;
    long lastid = 0;

    /*qry.setSQL ("select count(*) from acct");
    qry.open (); qry.next ();

    count = qry.field(0).asInteger ();

    qry.close ();*/

    char countbuf[32];
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ACCT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poAcctData->create (CAL_COUNT(count));
    m_poAcctIndex->create (CAL_COUNT(count));

    pAcct = (AcctInfo *)(*m_poAcctData);

    qry.setSQL ("select a.acct_id, a.cust_id, a.state, nvl(b.credit_grade, 1500), "
                " nvl(b.credit_amount, 1500) "
                "from acct a, acct_credit b where a.acct_id=b.acct_id(+) ");
    qry.open ();

	long lTemp = 0;
	while (qry.next())
	{
		lTemp = qry.field(0).asLong ();
		if (m_poAcctIndex->get (lTemp, &k)){
			pAcct[k].m_iCreditAmount += qry.field(4).asInteger ();
			continue;
		}
        i = m_poAcctData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+2);
        
        pAcct[i].m_lAcctID = lTemp;
        pAcct[i].m_lCustID = qry.field(1).asLong ();
        strcpy (pAcct[i].m_sState, qry.field(2).asString ());
        pAcct[i].m_iCreditGrade = qry.field(3).asInteger ();
        pAcct[i].m_iCreditAmount = qry.field(4).asInteger ();

        if (pAcct[i].m_lCustID != 0) {
            if (!m_poCustIndex->get(pAcct[i].m_lCustID, &k)) {
                if (!m_bLogClose) Log::log (0, "帐户找不到对应的客户，帐户ID:%ld", pAcct[i].m_lAcctID);
                continue;
            }
            uiTemp = &(pCust[k].m_iAcctOffset);
            while (*uiTemp) {
                pAcct[i].m_iNextOffset=*uiTemp;
                break;
            }
            *uiTemp = i;
            m_poAcctIndex->add (pAcct[i].m_lAcctID, i);
        }
        /*uiTemp = &(pCust[k].m_iAcctOffset);
        while(*uiTemp)
        {
                uiTemp = &(pAcct[*uiTemp].m_iNextOffset);
        }
        *uiTemp = i;
        m_poAcctIndex->add (pAcct[i].m_lAcctID, i);
        */
        /*
        if (pAcct[i].m_lCustID == lastid) {
            pAcct[lasti].m_iNextOffset = i;
        } else {
            if (m_poCustIndex->get(pAcct[i].m_lCustID, &k)) {
                pCust[k].m_iAcctOffset = i;
            }
        }

    m_poAcctIndex->add (pAcct[i].m_lAcctID, i);

    lastid = pAcct[i].m_lCustID;
    lasti = i;
        */
    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}
//#endif

//##ModelId=42479018038D
void UserInfoAdmin::loadCustGroupInfo()
{
    int count;
    char sql[2048];
    DEFINE_QUERY (qry);

    CustGroupInfo * p;
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    unsigned int i, lasti=0, k;
    long lastid = 0;

    strcpy (sql, "select count(*) total from cust_group_member");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poCustGroupData->create (CAL_COUNT(count));
    p = (CustGroupInfo *)(*m_poCustGroupData);

    strcpy (sql, "select cust_id, cust_group_id from cust_group_member order by cust_id, cust_group_id");

    qry.setSQL (sql); qry.open ();

    while (qry.next()) {
        i = m_poCustGroupData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+2);

        p[i].m_lCustID = qry.field(0).asLong ();
        p[i].m_lGroupID = qry.field(1).asLong ();

        if (p[i].m_lCustID == lastid) {
            p[lasti].m_iNextOffset = i;
        } else {
            if (m_poCustIndex->get(p[i].m_lCustID, &k)) {
                pCust[k].m_iGroupOffset = i;
            }
        }
        lastid = p[i].m_lCustID;
        lasti = i;
    }

    qry.close ();
}

void UserInfoAdmin::loadCustPricingTariffInfo()
{
    int count;
    char sql[2048];
    DEFINE_QUERY (qry);

    CustPricingTariffInfo * pTariff;
    unsigned int i, k;

    strcpy (sql, "select count(*) from b_cust_price_plan_tariff");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poCustTariffData->create (CAL_COUNT(count));
    pTariff = (CustPricingTariffInfo *)(*m_poCustTariffData);

    m_poCustTariffIndex->create (CAL_COUNT(count), MAX_CALLING_NBR_LEN);

    strcpy (sql, "select cust_price_plan_id, unit, rate, precision, called_area_code "
            "from b_cust_price_plan_tariff");

    qry.setSQL (sql);
    qry.open ();

    while (qry.next()) {
        i = m_poCustTariffData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+3);

        pTariff[i].m_lCustPlanID = qry.field(0).asLong ();
        pTariff[i].m_iUnit = qry.field(1).asInteger ();
        pTariff[i].m_iRate = qry.field(2).asInteger ();
        pTariff[i].m_iPrecision = qry.field(3).asInteger ();
        pTariff[i].m_iBalance = pTariff[i].m_iLeft = pTariff[i].m_iRight = 0;

        if (!m_poCustTariffIndex->get (qry.field(4).asString (), &k)) {
            m_poCustTariffIndex->add (qry.field (4).asString (), i);
            continue;
        }

        //-----根据北方的数据量，不调整高度了
        unsigned int * pk = 
        (pTariff[i].m_lCustPlanID<pTariff[k].m_lCustPlanID) ? &(pTariff[k].m_iLeft) : &(pTariff[k].m_iRight);

        while (*pk) {
            pk = (pTariff[i].m_lCustPlanID<pTariff[*pk].m_lCustPlanID) ? \
                 &(pTariff[*pk].m_iLeft) : &(pTariff[*pk].m_iRight);
        }

        *pk = i;
    }

    qry.close ();

}

//##ModelId=424790220157
void UserInfoAdmin::loadCustPricingPlanInfo()
{
    int count;
    char sql[2048];
    DEFINE_QUERY (qry);

    CustPricingPlanInfo * p;
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    unsigned int i, k;

    unsigned int * pTemp;

    char type[4];

    strcpy (sql, "select count(*) from cust_price_plan");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poCustPricingPlanData->create (CAL_COUNT(count));
    p = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);

    m_poCustPricingPlanIndex->create (CAL_COUNT(count));

    strcpy (sql, "select cust_id, pricing_plan_id, nvl(belong_object_type, '80Z'),"
            "nvl(belong_object_id, 0), cust_price_plan_id, "
            "nvl(eff_date, to_date('19000101', 'yyyymmdd')) eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from cust_price_plan order by cust_id, eff_date desc");

    qry.setSQL (sql);
    qry.open ();

    while (qry.next()) {
        i = m_poCustPricingPlanData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+3);

        p[i].m_lCustID = qry.field(0).asLong ();
        p[i].m_iPricingPlanID = qry.field(1).asInteger ();
        strcpy (type, qry.field(2).asString ());
        p[i].m_lObjectID = qry.field(3).asLong ();
        p[i].m_lCustPlanID = qry.field(4).asLong ();
        strcpy (p[i].m_sEffDate, qry.field(5).asString ());
        strcpy (p[i].m_sExpDate, qry.field(6).asString ());

        switch (type[2] | 0x20) {
        case 'a':
            p[i].m_iObjectType = SERV_INSTANCE_TYPE;
            break;

        case 'j':
            p[i].m_iObjectType = ACCT_TYPE;
            break;

        case 'i':
            p[i].m_iObjectType = CUST_TYPE;
            break;

        case 'g':
            p[i].m_iObjectType = SERV_GROUP_TYPE;
            break;

        default:
            if (!m_bLogClose) Log::log (0, "[ERROR] unknown belong object type %s", type);
            continue;
            break;
        }

        if (!m_poCustIndex->get(p[i].m_lCustID, &k)) {
            if (!m_bLogClose) Log::log (0, "套餐没有找到对应的客户:%ld", p[i].m_lCustID);
            continue;
        }

        pTemp = &(pCust[k].m_iCustPlanOffset);

        while (*pTemp) {
            if (strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate)>=0) {
                break;
            }

            pTemp = &(p[*pTemp].m_iNextOffset);
        }

        p[i].m_iNextOffset = *pTemp;
        *pTemp = i;

        m_poCustPricingPlanIndex->add (p[i].m_lCustPlanID, i);

    }

    qry.close ();

}

void UserInfoAdmin::loadCustPricingParamInfo()
{
    DEFINE_QUERY(qry);
    int count;

    CustPricingParamInfo * pParam;
    CustPricingPlanInfo * pCustPlan = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);
    unsigned int i, iLasti=0, k;
    long lLastID = 0;

    qry.setSQL ("select count(*) from cust_price_plan_para");
    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poCustPricingParamData->create (CAL_COUNT(count));
    pParam = (CustPricingParamInfo *)(*m_poCustPricingParamData);

    qry.setSQL ("select cust_price_plan_para_id, pricing_param_id, info_val"
                " from cust_price_plan_para order by cust_price_plan_para_id, pricing_param_id");
    qry.open ();

    while (qry.next()) {
        i = m_poCustPricingParamData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+3);

        pParam[i].m_lCustPlanID = qry.field(0).asLong ();
        pParam[i].m_iParamID = qry.field(1).asInteger ();
        pParam[i].m_lValue = strtol (qry.field(2).asString (), 0, 10);

        if (pParam[i].m_lCustPlanID == lLastID) {
            pParam[iLasti].m_iNextOffset = i;
        } else {
            if (m_poCustPricingPlanIndex->get(pParam[i].m_lCustPlanID, &k)) {
                pCustPlan[k].m_iParamOffset = i;
            }
        }
        lLastID = pParam[i].m_lCustPlanID;
        iLasti = i;
    }

    qry.close ();
}

//Load Cust Pricing Object Info
void UserInfoAdmin::loadCustPricingObjectInfo()
{
    DEFINE_QUERY(qry);
    int count;

    CustPricingObjectInfo * pObject;
    CustPricingPlanInfo * pCustPlan = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);
    unsigned int i, iLasti=0, k;
    long lLastID = 0;
    char sType[4];

    qry.setSQL ("select count(*) from cust_price_plan_object");
    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poCustPricingObjectData->create (CAL_COUNT(count));
    pObject = (CustPricingObjectInfo *)(*m_poCustPricingObjectData);

    qry.setSQL ("select cust_price_plan_object_id, product_objects_id, object_type, object_id "
                "from cust_price_plan_object order by cust_price_plan_object_id, product_objects_id");
    qry.open ();

    while (qry.next()) {
        i = m_poCustPricingObjectData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+3);

        pObject[i].m_lCustPlanID = qry.field(0).asLong ();
        pObject[i].m_iPricingObjectID = qry.field(1).asInteger ();
        strcpy (sType, qry.field(2).asString ());
        pObject[i].m_lObjectID = qry.field(3).asInteger ();

        switch (sType[2] | 0x20) {
        case 'a':
            pObject[i].m_iObjectType = SERV_INSTANCE_TYPE;
            break;

        case 'c':
            pObject[i].m_iObjectType = OFFER_INSTANCE_TYPE;
            break;

        case 'i':
            pObject[i].m_iObjectType = CUST_TYPE;
            break;

        case 'j':
            pObject[i].m_iObjectType = ACCT_TYPE;
            break;

        case 'g':
            pObject[i].m_iObjectType = SERV_GROUP_TYPE;
            break;

        default:
            pObject[i].m_iObjectType = GENERAL_TYPE;
            break;
        }

        if (pObject[i].m_lCustPlanID == lLastID) {
            pObject[iLasti].m_iNextOffset = i;
        } else {
            if (m_poCustPricingPlanIndex->get(pObject[i].m_lCustPlanID, &k)) {
                pCustPlan[k].m_iObjectOffset = i;
            }
        }
        lLastID = pObject[i].m_lCustPlanID;
        iLasti = i;
    }

    qry.close ();
}

//##ModelId=4247902A0195
void UserInfoAdmin::loadProductInfo()
{
    int count;
    char sql[2048];
    ProductInfo * p;
    unsigned int i;
    DEFINE_QUERY (qry);

    strcpy (sql, "select count(*) from product");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poProductData->create (CAL_COUNT(count));
    m_poProductIndex->create (CAL_COUNT(count));
    p = (ProductInfo *)(*m_poProductData);

    strcpy (sql, "SELECT P.PRODUCT_ID,"                  
            "       P.PRODUCT_PROVIDER_ID,"         
            "       NVL(P.PRICING_PLAN_ID, 0),"     
            "       NVL(PN.NETWORK_ID,0),"
            "       NVL(NI.PRIORITY, 0)" 
            "  FROM PRODUCT P, PRODUCT_NETWORK PN, NETWORK_INFO NI"  
            " WHERE P.PRODUCT_ID = PN.PRODUCT_ID(+)"
            "   AND PN.NETWORK_ID = NI.NETWORK_ID");

    qry.setSQL (sql);
    qry.open (); 

    while (qry.next()) {
        i = m_poProductData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+4);

        p[i].m_iProductID = qry.field(0).asInteger ();
        p[i].m_iProviderID = qry.field(1).asInteger ();
        p[i].m_iPricingPlanID = qry.field(2).asInteger ();
        p[i].m_iNetworkID = qry.field(3).asInteger ();
        p[i].m_iNetworkPriority = qry.field(4).asInteger ();

        m_poProductIndex->add (p[i].m_iProductID, i);
    }

    qry.close ();
}

//##ModelId=424790320006
void UserInfoAdmin::loadOfferInfo()
{
    int count;
    char sql[2048];
    OfferInfo * p;
    unsigned int i;
    DEFINE_QUERY (qry);


    strcpy (sql, "select count(*) total from product_offer where offer_type=0");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poOfferData->create (CAL_COUNT(count));
    m_poOfferIndex->create (CAL_COUNT(count));
    p = (OfferInfo *)(*m_poOfferData);

    strcpy (sql, "select offer_id, nvl(pricing_plan_id,0) from product_offer where offer_type=0");

    qry.setSQL (sql);
    qry.open ();

    while (qry.next()) {
        i = m_poOfferData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+5);

        p[i].m_iOfferID = qry.field(0).asInteger ();
        p[i].m_iPricingPlan = qry.field(1).asInteger ();
        m_poOfferIndex->add (p[i].m_iOfferID, i);
    } 

    qry.close ();
}

void UserInfoAdmin::loadNpInfo()
{
	TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);

	int iSeq = getSeq(qry);
	if(iSeq<0) return;
	if(!insertLog(iSeq,"np","b_np",qry)) return;
		
	int count;
	char sql[2048];
	
	char countBuf[32];
	memset(countBuf,'\0',sizeof(countBuf));

	if(!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_NP, countBuf))
	{
	    THROW(MBC_NP+1);
	}
	count = atoi(countBuf);

	m_poNpData->create (CAL_COUNT(count));
	m_poNpIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);
	
	NpInfo *p;
	p = (NpInfo*) (*m_poNpData);
	
	strcpy (sql, "select  acc_nbr, nvl(org_network,-1), network, np_state, eff_date, nvl(exp_date, to_date('20500101', 'yyyymmdd')) from b_np");
	
	qry.setSQL (sql);
	
	qry.open ();
	
	unsigned int i,k;

	while (qry.next()) 
	{
		
		i = m_poNpData->malloc ();
		
		if (!i) THROW(MBC_NP+1);
	  
	 
		strcpy( p[i].m_sAccNbr, qry.field(0).asString());
		p[i].m_iOrgNetwork = qry.field(1).asInteger();
		p[i].m_iNetwork = qry.field(2).asInteger();
		p[i].m_iNpState = qry.field(3).asInteger();
		strcpy( p[i].m_sEffDate, qry.field(4).asString() );
		strcpy( p[i].m_sExpDate, qry.field(5).asString() );
		
		p[i].m_iNext = 0;
					
		//按照生效时间链起来
		if (m_poNpIndex->get(p[i].m_sAccNbr, &k))
		{
			if(strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0)
			{
				p[i].m_iNext = k;
				m_poNpIndex->add (p[i].m_sAccNbr, i);
			}
			else
			{
				unsigned int * temp = &(p[k].m_iNext);
				while(*temp)
			{
					if(strcmp(p[i].m_sEffDate,p[*temp].m_sEffDate)>=0)
						break;
					temp = &(p[*temp].m_iNext);
				}
				p[i].m_iNext = *temp;
				*temp = i;
			}
			/*
			int pre;
			while(k)//找到最后一个
			{
				pre = k;
				k = p[k].m_iNext;
			}
			p[pre].m_iNext = i;
			*/
		}
		else
		{
			m_poNpIndex->add (p[i].m_sAccNbr, i);
		}
		
  } 

	qry.close ();
	updateLog(iSeq,qry);
	pConn->disconnect();
	delete pConn;
}
//##ModelId=4247903802B7
void UserInfoAdmin::loadServInfo()
{
    TOCIQuery qry(Environment::getDBConn ());
    int count;
    char sql[2048];
    ServInfo * p;
    unsigned int i, k = 0;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"s","serv",qry)) return;

    CustInfo * pCust = (CustInfo *)(*m_poCustData);

    unsigned int * pTemp;

/*      strcpy (sql, "select count(*) from serv");
        qry.setSQL (sql);
        qry.open ();
        qry.next ();
        count = qry.field(0).asInteger ();
        qry.close ();*/
    char countbuf[32];
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERV, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServData->create (CAL_COUNT(count));
    m_poServIndex->create (CAL_COUNT(count));
    p = (ServInfo *)(*m_poServData);

    if( !m_bFilterServ ){
        strcpy (sql, "select serv_id, cust_id, product_id, state, lower(acc_nbr) acc_nbr,"
                "cycle_type_id, billing_mode_id, "
                "nvl(free_type_id, -1) free_type_id,"
                "create_date,"
                " state_date,"
                " rent_date,"
                " nsc_date "
                "from serv");
    }
    if( m_bFilterServ ){
        strcpy (sql, "select serv_id, cust_id, product_id, state, lower(acc_nbr) acc_nbr,"
                "cycle_type_id, billing_mode_id, "
                "nvl(free_type_id, -1) free_type_id,"
                "create_date,"
                " state_date,"
                " rent_date,"
                " nsc_date "
                "from serv where substr(state, 3, 1) != 'X' ");
    }

    qry.setSQL (sql); qry.open ();

    while (qry.next()) {
        i = m_poServData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+6);

        p[i].m_lServID = qry.field(0).asLong ();
        p[i].m_lCustID = qry.field(1).asLong ();
        p[i].m_iProductID = qry.field(2).asInteger ();
        strcpy (p[i].m_sState, qry.field(3).asString ());
        strcpy (p[i].m_sAccNbr, qry.field(4).asString ());
        p[i].m_iCycleType = qry.field(5).asInteger ();
        p[i].m_iBillingMode = qry.field(6).asInteger ();
        p[i].m_iFreeType = qry.field(7).asInteger ();
        strcpy (p[i].m_sCreateDate, qry.field(8).asString ());
        strcpy (p[i].m_sStateDate, qry.field(9).asString ());
        strcpy (p[i].m_sRentDate, qry.field(10).asString ());
        strcpy (p[i].m_sNscDate, qry.field(11).asString ());

        m_poServIndex->add (p[i].m_lServID, i);

        if (!m_poCustIndex->get(p[i].m_lCustID, &k)) {
            if (!m_bLogClose) Log::log (0, "用户[%ld]找不到对应的cust", p[i].m_lServID);
            continue;
        }

        p[i].m_iCustOffset = k; 

        pTemp = &( pCust[k].m_iServOffset );

        p[i].m_iCustNext = *pTemp;
        *pTemp = i;

    } 

    qry.close ();
    updateLog(iSeq,qry);
}

//##ModelId=4247903F025D
void UserInfoAdmin::loadServLocationInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"sl","serv_location",qry)) return;

    int count,num;
    char sql[2048];
    //DEFINE_QUERY (qry);

    ServLocationInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*strcpy (sql, "select count(*) from serv_location");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVLOCATION, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }
    
	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

	memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVLOCATION, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);	

    m_poServLocationData->create (CAL_COUNT(count));
    p = (ServLocationInfo *)(*m_poServLocationData);

   if( ParamDefineMgr::getParam("USERINFO_UPLOAD","INDEX_SERVLOCATION",countbuf,qry) )
   	{
   		count = atoi(countbuf);
   		m_poServLocationIndex->create (count);
   		
   		}
   	else
    m_poServLocationIndex->create (10000);

    strcpy (sql, "select serv_id, nvl(org_id, -1), "
            "eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from serv_location");

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (!m_poServIndex->get (qry.field(0).asLong(), &k)) {
            if (!m_bLogClose) Log::log (0, "地域属性找不到对应的用户:%ld", qry.field(0).asLong() );
            continue;
        }
        if (strcmp(qry.field(3).asString(),strDate)<0) {
            continue;
        }
        i = m_poServLocationData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+7);

        p[i].m_lServID = qry.field(0).asLong ();
        p[i].m_iOrgID = qry.field(1).asInteger ();
        strcpy (p[i].m_sEffDate, qry.field(2).asString ());
        strcpy (p[i].m_sExpDate, qry.field(3).asString ());

        //if (!m_poServIndex->get (p[i].m_lServID, &k)) {
        //    if (!m_bLogClose) Log::log (0, "地域属性找不到对应的用户:%ld", p[i].m_lServID);
        //    continue;
        //}

        pTemp = &(pServ[k].m_iLocationOffset);
        //20060724 xueqt
        p[i].m_lServID = k;

        while (*pTemp) {
            if (strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate) >= 0) {
                break;
            }

            pTemp = &(p[*pTemp].m_iNextOffset);
        }

        p[i].m_iNextOffset = *pTemp;
        *pTemp = i;

        if (m_poServLocationIndex->get (p[i].m_iOrgID, &k)) {
            p[i].m_iOrgNext = k;
        } else {
            p[i].m_iOrgNext = 0;
        }

        m_poServLocationIndex->add (p[i].m_iOrgID, i);
    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadServStateInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"ssa","serv_state_attr",qry)) return;

    int count,num;
    char sql[2048];

    ServStateInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*qry.setSQL ("select count(*) from serv_state_attr");
    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVSTATEATTR, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVSTATEATTR, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServStateData->create (CAL_COUNT(count));
    p = (ServStateInfo *)(*m_poServStateData);

    strcpy (sql, "select serv_id, owe_business_type_id owe_type, state, "
            "eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from serv_state_attr");

    qry.setSQL (sql); qry.open ();

    //限制最近三个月的记录
    /*Date d;
    d.addMonth(-3);
    char sDate[16];
    strcpy(sDate,d.toString());*/
    
    while (qry.next()) {
        if (!m_poServIndex->get (qry.field(0).asLong(), &k)) {
            if (!m_bLogClose) Log::log (0, "状态属性找不到对应的用户:%ld", qry.field(0).asLong());
            continue;
        }
        /*if(strcmp(qry.field(4).asString (), sDate)<0)
        {
                continue;
        }*/
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        i = m_poServStateData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+8);

        p[i].m_lServID = qry.field(0).asLong ();
        p[i].m_iOweType = qry.field(1).asInteger ();
        strcpy (p[i].m_sState, qry.field(2).asString ());
        strcpy (p[i].m_sEffDate, qry.field(3).asString ());
        strcpy (p[i].m_sExpDate, qry.field(4).asString ());

        pTemp = &(pServ[k].m_iStateOffset);

        while (*pTemp) {
            if (strcmp (p[i].m_sExpDate, p[*pTemp].m_sExpDate)>=0) {
                break;
            }

            pTemp = &(p[*pTemp].m_iNextOffset);
        }

        p[i].m_iNextOffset = *pTemp;
        *pTemp = i;
    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

//##ModelId=4247904D03BC
void UserInfoAdmin::loadServIdentInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"si","serv_identification",qry)) return;

    int count,num;
    char sql[2048];

    ServIdentInfo * p;
    unsigned int i, k;

    int iPriority;

    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServInfo * pTemp;

    /*strcpy (sql, "select count(*) total from serv_identification" );
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    strcpy (sql, "select count(*) total from a_serv_ex" );
    qry.setSQL (sql); qry.open (); qry.next ();
    count += qry.field(0).asInteger ();
    qry.close ();*/

    char sAccNbr[32];
    char * pAccNbr;
    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVIDENT, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVIDENT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServIdentData->create (CAL_COUNT(count));
    m_poServIdentIndex->create (CAL_COUNT(count), MAX_CALLING_NBR_LEN);
    p = (ServIdentInfo *)(*m_poServIdentData);

    ProductInfo * ppi = (ProductInfo *)(*m_poProductData);

    sprintf (sql, "select serv_id, acc_nbr, "
             "eff_date,"
             "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, '50Z' acc_nbr_type "
             "from serv_identification "
             "union all "
             "select serv_id, acc_nbr, "
             "eff_date, "
             "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, acc_nbr_type "
             "from a_serv_ex");

    qry.setSQL (sql); qry.open ();
    
    unsigned int iTemp = 0;
    while (qry.next()) {
        if (!m_poServIndex->get(qry.field(0).asLong(), &iTemp) ) {
            if (!m_bLogClose) Log::log (0, "号码:%s没有找到对应的用户%ld", 
                                        qry.field(1).asString(),
                                        qry.field(0).asLong() );
            continue;
        }
        if (strcmp(qry.field(3).asString(),strDate)<0) {
            continue;
        }
        i = m_poServIdentData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+9);

        p[i].m_lServID = qry.field(0).asLong ();

#ifdef SPECIAL_NBR_CUT
        strcpy (sAccNbr, qry.field(1).asString ());
        pAccNbr =sAccNbr+4;
        if (strncmp(sAccNbr,pAccNbr,4)==0 && strncmp(sAccNbr,"085",3)==0) {
            strcpy(p[i].m_sAccNbr,pAccNbr);
        } else {
            strcpy(p[i].m_sAccNbr,sAccNbr);
        }
#else
        strcpy (p[i].m_sAccNbr, qry.field(1).asString ());
#endif

//      strcpy (p[i].m_sAccNbr, qry.field(1).asString ());

        char * s = p[i].m_sAccNbr;
        while (*s) {

            if (*s>='A' && *s<='Z') *s |= 0x20;

            s++;
        }

        strcpy (p[i].m_sEffDate, qry.field(2).asString ());
        strcpy (p[i].m_sExpDate, qry.field(3).asString ());

        char sType[8];
        strcpy (sType, qry.field (4).asString ());

        p[i].m_iType = (sType[2] | 0x20) - 'a';

        //if (!m_poServIndex->get(p[i].m_lServID, &(p[i].m_iServOffset))) {
        //    if (!m_bLogClose) Log::log (0, "号码:%s没有找到对应的用户", p[i].m_sAccNbr);
        //    continue;
        //}
        p[i].m_iServOffset = iTemp;

        pTemp = pServ + p[i].m_iServOffset;

        p[i].m_iServNextOffset = pTemp->m_iServIdentOffset;
        pTemp->m_iServIdentOffset = i;

        if (m_poProductIndex->get (pTemp->m_iProductID, &k)) {
            p[i].m_iNetworkID = ppi[k].m_iNetworkID;
            p[i].m_iNetworkPriority = ppi[k].m_iNetworkPriority;
        } else {
            //if (!m_bLogClose) Log::log (0, "号码:%s没有找到对应的网络类型", p[i].m_sAccNbr);
            p[i].m_iNetworkID = 0;
            p[i].m_iNetworkPriority = 0;
        }

        if (!m_poServIdentIndex->get (p[i].m_sAccNbr, &k)) {
            m_poServIdentIndex->add (p[i].m_sAccNbr, i);
        } else {
            //先按照优先级(高的在前),再按照时间来排序?
            if (p[i].m_iNetworkPriority > p[k].m_iNetworkPriority
                ||
                ( p[i].m_iNetworkPriority == p[k].m_iNetworkPriority &&
                  p[i].m_iType > p[k].m_iType )
                ||
                (p[i].m_iNetworkPriority == p[k].m_iNetworkPriority &&
                 p[i].m_iType == p[k].m_iType &&
                 strcmp (p[i].m_sExpDate, p[k].m_sExpDate)>=0 &&
                 strcmp (p[i].m_sEffDate, p[k].m_sEffDate) >= 0) 
               ) {
                p[i].m_iNextOffset = k;
                m_poServIdentIndex->add (p[i].m_sAccNbr, i);        //--覆盖掉
            } else {
                unsigned int * pTemp = &(p[k].m_iNextOffset);
                while (*pTemp) {
                    //找到第一个优先级不大于的地方
                    if (p[i].m_iNetworkPriority >= p[*pTemp].m_iNetworkPriority) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                while (*pTemp &&
                       p[i].m_iNetworkPriority == p[*pTemp].m_iNetworkPriority) {
                    //找到第一个优先级不大于的地方
                    if (p[i].m_iType >= p[*pTemp].m_iType) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                while (*pTemp &&
                       p[i].m_iNetworkPriority == p[*pTemp].m_iNetworkPriority &&
                       p[i].m_iType == p[*pTemp].m_iType) {
                    if (strcmp (p[i].m_sExpDate, p[*pTemp].m_sExpDate)>=0 &&
                        strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate)>=0) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                p[i].m_iNextOffset = *pTemp;
                *pTemp = i;
            }
        }

    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadServGroupBaseInfo()
{
    DEFINE_QUERY (qry);
    int count;
    char sql[2048];

    ServGroupBaseInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, k;

    strcpy (sql, "select count(*) from serv_group");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poServGroupBaseData->create (CAL_COUNT(count));
    m_poServGroupBaseIndex->create (CAL_COUNT(count));
    p = (ServGroupBaseInfo *)(*m_poServGroupBaseData);

    qry.setSQL ("select serv_group_id, group_serv_id, nvl(serv_group_type, '90T') from serv_group");
    qry.open ();

    while (qry.next()) {
        i = m_poServGroupBaseData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+10);

        p[i].m_lGroupID = qry.field(0).asLong ();
        p[i].m_lGroupServID = qry.field(1).asLong ();

        switch ((qry.field(2).asString ())[2] | 0x20) {
        case 't':
            p[i].m_iGroupType = 1;
            break;
        default:
            p[i].m_iGroupType = 2;
            break;
        }

        if (m_poServIndex->get (p[i].m_lGroupServID, &k)) {
            //p[i].m_iCustPlanOffset = pCust[pServ[k].m_iCustOffset].m_iCustPlanOffset;
            p[i].m_iCustOffset = pServ[k].m_iCustOffset;
        } else {
            if (!m_bLogClose) Log::log (0, "[ERROR] 用户群的代表用户不存在[GROUP:%ld SERV:%ld]", 
                                        p[i].m_lGroupID, p[i].m_lGroupServID);
            continue;
        }

        m_poServGroupBaseIndex->add (p[i].m_lGroupID, i);

    }
    qry.close ();

}

//##ModelId=4247905500D8
void UserInfoAdmin::loadServGroupInfo()
{
    DEFINE_QUERY (qry);
    int count;
    char sql[2048];

    ServGroupInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);
    unsigned int i, lasti=0, k;
    long lastid = 0;

    strcpy (sql, "select count(*) from serv_group_member");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poServGroupData->create (CAL_COUNT(count));
    p = (ServGroupInfo *)(*m_poServGroupData);

    strcpy (sql, "select serv_id, serv_group_id,"
            " eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from serv_group_member order by serv_id, serv_group_id");

    qry.setSQL (sql); qry.open ();

    while (qry.next()) {
        i = m_poServGroupData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+10);

        p[i].m_lServID = qry.field(0).asLong ();
        p[i].m_lGroupID = qry.field(1).asLong ();
        strcpy (p[i].m_sEffDate, qry.field(2).asString ());
        strcpy (p[i].m_sExpDate, qry.field(3).asString ());

        if (p[i].m_lServID == lastid) {
            p[lasti].m_iNextOffset = i;
        } else {
            if (m_poServIndex->get(p[i].m_lServID, &k)) {
                pServ[k].m_iGroupOffset = i;
            }
        }

        if (m_poServGroupBaseIndex->get (p[i].m_lGroupID, &k)) {
            p[i].m_iGroupNext = pGroupBase[k].m_iServOffset;
            pGroupBase[k].m_iServOffset = i;
        }

        lastid = p[i].m_lServID;
        lasti = i;
    }

    qry.close ();
}

//##ModelId=4247906101C6
void UserInfoAdmin::loadServProductInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"sp","serv_product",qry)) return;

    int count,num;
    char sql[2048];

    ServProductInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*qry.setSQL ("select count(*) total from serv_product");

    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVPRODUCT, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }
	
	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVPRODUCT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServProductData->create (CAL_COUNT(count));
    m_poServProductIndex->create (CAL_COUNT(count));
    p = (ServProductInfo *)(*m_poServProductData);

    strcpy (sql, "select serv_id, serv_product_id, prod_type_id, "
            " eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from serv_product");

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (!m_poServIndex->get (qry.field(0).asLong(), &k)) {
            if (!m_bLogClose) Log::log (0, "附属产品找不到对应的SERV_ID:[%ld]", qry.field(0).asLong());
            continue;
        }
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        i = m_poServProductData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+11);

        p[i].m_lServID = qry.field(0).asLong ();
        p[i].m_lServProductID = qry.field(1).asLong ();
        p[i].m_iProductID = qry.field(2).asInteger ();
        strcpy (p[i].m_sEffDate, qry.field(3).asString ());
        strcpy (p[i].m_sExpDate, qry.field(4).asString ());

        m_poServProductIndex->add (p[i].m_lServProductID, i);

        //if (!m_poServIndex->get (p[i].m_lServID, &k)) {
        //    if (!m_bLogClose) Log::log (0, "附属产品找不到对应的SERV_ID:[%ld]", p[i].m_lServProductID);
        //    continue;
        //}

        p[i].m_iServOffset = k;

        /*      xueqt 20051024 提高性能 */
        pTemp = &(pServ[k].m_iServProductOffset);

        while (*pTemp) {
            if (strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate) > 0) {
                pTemp = &(p[*pTemp].m_iNextOffset);
            } else {
                break;
            }
        }

        p[i].m_iNextOffset = *pTemp;
        *pTemp = i;
    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

//##ModelId=4247906C009F
void UserInfoAdmin::loadServAcctInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"sa","serv_acct",qry)) return;

    int count,num;
    char sql[2048];

    ServAcctInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, lasti=0, k;
    long lastid = 0;
    char state[4];
    //state order
    unsigned int  iTemp;
    unsigned int * pTemp = NULL;
    unsigned int iUpState, iNext,iNextTemp, iUp=0;
    int iZero=0;
    //end state order

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVACCT, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT,USERINFO_NUM_SERVACCT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServAcctData->create (CAL_COUNT(count));
    p = (ServAcctInfo *)(*m_poServAcctData);

    m_poServAcctIndex->create (CAL_COUNT(count));

    strcpy (sql,
            "select serv_acct_id,serv_id, acct_id, item_group_id, created_date,state_date, state  "
            "from serv_acct ");

    qry.setSQL (sql); qry.open ();

    /*Date d;
    d.addMonth (-4);

    char sStdDate[16];
    strcpy (sStdDate, d.toString ());*/
    int iState;
    
    while (qry.next()) {
        strcpy (state, qry.field(6).asString ());

        if ((state[2]|0X20) == 'a') {
            iState = 1;
        } else {
            iState = 0;
        }

        /*if (!iState && strcmp(qry.field(5).asString(), sStdDate)<0)
                {
            continue;
        }*/
        if (!iState && strcmp(qry.field(5).asString(),strDate)<0) {
            continue;
        }
        i = m_poServAcctData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+12);

        p[i].m_lServAcctID = qry.field(0).asLong();
        p[i].m_lServID = qry.field(1).asLong ();
        p[i].m_lAcctID = qry.field(2).asLong ();
        p[i].m_iAcctItemGroup = qry.field(3).asInteger ();
        p[i].m_iState = iState;
        strcpy (p[i].m_sEffDate, qry.field (4).asString ());
        if (strcmp(p[i].m_sEffDate,qry.field(5).asString()) == 0) {
            strcpy (p[i].m_sExpDate, "20500101000000");
        } else {
            strcpy(p[i].m_sExpDate,qry.field(5).asString());
        }
        if (m_poServIndex->get (p[i].m_lServID, &k)) {
            //date order
            unsigned int *pInt = &(pServ[k].m_iServAcctOffset);
            while (*pInt) {
                if (strcmp (p[i].m_sEffDate, p[*pInt].m_sEffDate) >=0) {
                    //strcpy (p[*pInt].m_sExpDate, p[i].m_sEffDate);
                    p[i].m_iNextOffset = *pInt;
                    *pInt = i;
                    break;
                }
                /*else
                {
    strcpy (p[i].m_sExpDate, p[*pInt].m_sEffDate);
}*/
                pInt = &(p[*pInt].m_iNextOffset);
            }

            if (!(*pInt)) {
                p[i].m_iNextOffset = 0;
                *pInt = i;
            }
            //state order
            iTemp = pServ[k].m_iServAcctOffset;
            pTemp =&(pServ[k].m_iServAcctOffset);
            iUp=0;
            iZero=0;
            while (iTemp) {
                iUpState = p[iTemp].m_iState;
                iNext = p[iTemp].m_iNextOffset;
                if (iUpState == 0) {
                    iZero++;
                } else {
                    if (iZero>0) {
                        while (*pTemp) {
                            if (p[*pTemp].m_iState == 0) {
                                iNextTemp = p[iTemp].m_iNextOffset;
                                p[iTemp].m_iNextOffset = *pTemp;
                                *pTemp = p[iUp].m_iNextOffset;
                                p[iUp].m_iNextOffset = iNextTemp;
                                break;
                            }
                            pTemp=&(p[*pTemp].m_iNextOffset);
                        }
                        pTemp =&(pServ[k].m_iServAcctOffset);
                    }
                }
                iUp = iTemp;
                iTemp = iNext;
            }
        }

        if (m_poServAcctIndex->get (p[i].m_lAcctID, &k)) {
            p[i].m_iAcctNextOffset = k;
        }
        m_poServAcctIndex->add (p[i].m_lAcctID, i);
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

//##ModelId=424790740029
void UserInfoAdmin::loadOfferInstanceInfo()
{
    int count;
    char sql[2048];

    DEFINE_QUERY (qry);

    OfferInstanceInfo * p;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    OfferInfo * pOffer = (OfferInfo *)(*m_poOfferData);
    unsigned int i, lasti=0, k;
    long instanceid, lastinstance = 0;
    char instancetype[4] = {0};
    char lasttype[4] = {0};

    strcpy (sql, "select count(*) total from product_offer_instance_detail");
    qry.setSQL (sql); qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();

    m_poOfferInstanceData->create (CAL_COUNT(count));
    m_poOfferInstanceIndex->create (CAL_COUNT(count));
    p = (OfferInstanceInfo *)(*m_poOfferInstanceData);

    //##第一步，加载数据        
    strcpy (sql, "select a.instance_type child_instance_type,"
            "a.instance_id child_instance, a.instance_relation_id detail_id,"
            "a.state state,"
            "a.eff_date,"
            "nvl(a.exp_date, to_date('20500101', 'yyyymmdd')) exp_date,"
            "a.product_offer_instance_id parent_instance,"
            "b.product_offer_id parent_offer_id "
            "from product_offer_instance_detail a,"
            "product_offer_instance b where "
            "a.product_offer_instance_id=b.product_offer_instance_id "
            "order by child_instance_type, child_instance, eff_date");

    qry.setSQL (sql); qry.open ();

    while (qry.next()) {
        i = m_poOfferInstanceData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+13);

        p[i].m_lDetailID = qry.field(2).asLong ();
        p[i].m_lOfferInstanceID = qry.field(6).asLong ();
        p[i].m_iOfferID = qry.field(7).asInteger ();
        strcpy (p[i].m_sEffDate, qry.field(4).asString ());
        strcpy (p[i].m_sExpDate, qry.field(5).asString ());
        strcpy (p[i].m_sState, qry.field(3).asString ());

        strcpy (instancetype, qry.field(0).asString ());
        instanceid = qry.field(1).asLong ();

        if (instanceid!=lastinstance || strcmp(instancetype, lasttype)) {
            //##是新的
            if ((instancetype[2]|0X20) == 'a') {
                //##实例类型为产品
                //##实例类型可能为产品和商品实例，现在没有产品包实例
                ////p[i].m_lChildInstanceID = 0;
                if (m_poServIndex->get(instanceid, &k)) {
                    pServ[k].m_iOfferInstanceOffset = i;
                }
            } else {
                //##实例类型为商品实例
                ////p[i].m_lChildInstanceID = instanceid;
                //##现在存放的时间上ChildInstanceID，最后做反转
                m_poOfferInstanceIndex->add(instanceid, i);
            }
        } else {
            //##和上一笔记录相同
            if ((instancetype[2]|0X20) == 'a') {
                ////p[i].m_lChildInstanceID = 0;
                p[lasti].m_iNextOffset = i;
            } else {
                ////p[i].m_lChildInstanceID = instanceid;
                p[lasti].m_iNextOffset = i;
            }
        }

        lasti = i;
        strcpy (lasttype, instancetype);
        lastinstance = instanceid;    
    }//end while

    qry.close ();
    //##第二步，整理数据，做反转

    for (i=1; i<=m_poOfferInstanceData->getCount(); i++) {
        //##1，确定定价计划
        if (m_poOfferIndex->get (p[i].m_iOfferID, &k)) {
            p[i].m_iPricingPlan = pOffer[k].m_iPricingPlan;    
        }

        m_poOfferInstanceIndex->get(p[i].m_lOfferInstanceID, &(p[i].m_iParentOffset));

    } //end for 
}

//##ModelId=4247907B0318
void UserInfoAdmin::loadServPricingPlanInfo()
{
}

//##ModelId=424BE38D0031
void UserInfoAdmin::updateCustInfo(CustInfo &data, bool isinsert,bool bLog)
{
    unsigned int i;

    CustInfo * p = (CustInfo *)(*m_poCustData);

    if (isinsert) {
        i = m_poCustData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+14);
    } else {
        if (!(m_poCustIndex->get(data.m_lCustID, &i))) THROW(MBC_UserInfoCtl+15);
    }


    //p[i].m_iCustLocation = data.m_iCustLocation;
    strcpy (p[i].m_sCustType, data.m_sCustType);
 //// p[i].m_bIsVip = data.m_bIsVip;
 //// p[i].m_lParentID = data.m_lParentID;
    strcpy (p[i].m_sState, data.m_sState);             
    //strcpy (p[i].m_sEffDate, data.m_sEffDate);
    //strcpy (p[i].m_sExpDate, data.m_sExpDate);

    if (isinsert) {
        p[i].m_lCustID = data.m_lCustID;
        m_poCustIndex->add (p[i].m_lCustID, i);
    }
}

void UserInfoAdmin::updateAcctInfo(AcctInfo &data, bool isinsert,bool bLog)
{
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    AcctInfo * pAcct = (AcctInfo *)(*m_poAcctData);

    unsigned int i, k;

    if (isinsert) {
        i = m_poAcctData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+14);
    } else {
        if (!(m_poAcctIndex->get (data.m_lAcctID, &i))) THROW(MBC_UserInfoCtl+15);
    }

    if (isinsert) {
        pAcct[i].m_lCustID = data.m_lCustID;
        pAcct[i].m_lAcctID = data.m_lAcctID;
        pAcct[i].m_iCreditAmount = data.m_iCreditAmount;
        pAcct[i].m_iCreditGrade = data.m_iCreditGrade;

        strcpy (pAcct[i].m_sState, data.m_sState);

        m_poAcctIndex->add (pAcct[i].m_lAcctID, i);

        if (m_poCustIndex->get(pAcct[i].m_lCustID, &k)) {
            pAcct[i].m_iNextOffset = pCust[k].m_iAcctOffset;
            pCust[k].m_iAcctOffset = i;
        }
    } else {

        strcpy (pAcct[i].m_sState, data.m_sState);
        pAcct[i].m_iCreditAmount = data.m_iCreditAmount;
        pAcct[i].m_iCreditGrade = data.m_iCreditGrade;
    }
}

//##ModelId=424BE3A30322
void UserInfoAdmin::updateCustGroupInfo(CustGroupInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    CustGroupInfo * p = (CustGroupInfo *)(*m_poCustGroupData);
    CustInfo * pCust = (CustInfo *)(*m_poCustData);

    //##对客户群信息，只有insert        
    if (!isinsert) return;

    //##首先定位CUST
    if (!(m_poCustIndex->get(data.m_lCustID, &k))) THROW(MBC_UserInfoCtl+16);

    i = m_poCustGroupData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+17);

    p[i].m_lCustID = data.m_lCustID;
    p[i].m_lGroupID = data.m_lGroupID;

    //##按照GroupID从小到大排序
    unsigned int * temp;
    temp = &(pCust[k].m_iGroupOffset);
    while (*temp && p[*temp].m_lGroupID<data.m_lGroupID) {
        temp = &(p[*temp].m_iNextOffset);
    }
    p[i].m_iNextOffset = *temp;
    *temp = i;
}

//##ModelId=424BE3AD00EC
void UserInfoAdmin::updateCustPricingPlanInfo(CustPricingPlanInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    //char type[4];
    CustPricingPlanInfo * p = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    unsigned int * temp;

    //##首先定位CUST
    if (!(m_poCustIndex->get(data.m_lCustID, &k))) THROW(MBC_UserInfoCtl+18);

    if (isinsert) {
        i = m_poCustPricingPlanData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+19);

        p[i].m_lCustPlanID = data.m_lCustPlanID;
        p[i].m_lCustID = data.m_lCustID;
        p[i].m_iPricingPlanID = data.m_iPricingPlanID;
        p[i].m_lObjectID = data.m_lObjectID;
        strcpy (p[i].m_sEffDate, data.m_sEffDate);
        strcpy (p[i].m_sExpDate, data.m_sExpDate);
        p[i].m_iObjectType = data.m_iObjectType; 

        m_poCustPricingPlanIndex->add (p[i].m_lCustPlanID, i);

    } else {
        temp = &(pCust[k].m_iCustPlanOffset);
        i = pCust[k].m_iCustPlanOffset;
        while (i) {
            if (p[i].m_lCustPlanID == data.m_lCustPlanID)
                break;
            temp = &(p[i].m_iNextOffset);
            i = p[i].m_iNextOffset;
        }
        if (!i) THROW(MBC_UserInfoCtl+20);

        *temp = p[i].m_iNextOffset;
        strcpy (p[i].m_sExpDate, data.m_sExpDate);
    }

    temp = &(pCust[k].m_iCustPlanOffset);
    while (*temp) {
        if (strcmp (p[*temp].m_sEffDate, p[i].m_sEffDate) <= 0) {
            p[i].m_iNextOffset = *temp;
            *temp = i;
            break;
        }

        temp = &(p[*temp].m_iNextOffset);
    }

    if (!(*temp)) {
        *temp = i;
    }

}

void UserInfoAdmin::updateCustTariffInfo(CustPricingTariffInfo &data, char *sCalledAreaCode, bool isinsert,bool bLog)
{
    if (!isinsert) return;

    unsigned int i = m_poCustTariffData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+3);

    data.m_iBalance = data.m_iLeft = data.m_iRight = 0;

    unsigned int k;

    if (!m_poCustTariffIndex->get (sCalledAreaCode, &k)) {
        m_poCustTariffIndex->add (sCalledAreaCode, i);
        return;
    }

    CustPricingTariffInfo * pTariff = (CustPricingTariffInfo *)(*m_poCustTariffData);

    //-----根据北方的数据量，不调整高度了
    unsigned int * pk =
    (data.m_lCustPlanID<pTariff[k].m_lCustPlanID) ? &(data.m_iLeft) : &(data.m_iRight);

    while (*pk) {
        pk = (pTariff[i].m_lCustPlanID<pTariff[*pk].m_lCustPlanID) ? \
             &(pTariff[*pk].m_iLeft) : &(pTariff[*pk].m_iRight);
    }

    *pk = i; 
    return;
}

void UserInfoAdmin::updateCustPricingParamInfo(CustPricingParamInfo &data, bool isinsert,bool bLog)
{
    //客户个性化参数只可以插入，不可以更改和删除
    if (!isinsert) return;

    unsigned int i, k;
    unsigned int * temp;
    CustPricingParamInfo * pParam = (CustPricingParamInfo *)(*m_poCustPricingParamData);
    CustPricingPlanInfo * pCustPlan = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);

    if (!m_poCustPricingPlanIndex->get(data.m_lCustPlanID, &k)) {
        THROW (MBC_UserInfoCtl+14);
    }

    i = m_poCustPricingParamData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+3);

    pParam[i].m_lCustPlanID = data.m_lCustPlanID;
    pParam[i].m_iParamID = data.m_iParamID;
    pParam[i].m_lValue = data.m_lValue;

    temp = &(pCustPlan[k].m_iParamOffset);
    while (*temp) {
        if (pParam[i].m_iParamID < pParam[*temp].m_iParamID) {
            pParam[i].m_iNextOffset = *temp;
            *temp = i;
            break;
        }

        temp = &(pParam[*temp].m_iNextOffset);
    }

    if (!(*temp))
        *temp = i;
}

void UserInfoAdmin::updateCustPricingObjectInfo(CustPricingObjectInfo &data, bool isinsert,bool bLog)
{
    //客户个性化参数只可以插入，不可以更改和删除
    if (!isinsert) return;

    unsigned int i, k;
    unsigned int * temp;
    CustPricingObjectInfo * pObject = (CustPricingObjectInfo *)(*m_poCustPricingObjectData);
    CustPricingPlanInfo * pCustPlan = (CustPricingPlanInfo *)(*m_poCustPricingPlanData);

    if (!m_poCustPricingPlanIndex->get(data.m_lCustPlanID, &k)) {
        THROW (MBC_UserInfoCtl+14);
    }

    i = m_poCustPricingObjectData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+3);

    pObject[i].m_lCustPlanID = data.m_lCustPlanID;
    pObject[i].m_lObjectID = data.m_lObjectID;
    pObject[i].m_iPricingObjectID = data.m_iPricingObjectID;
    pObject[i].m_iObjectType = data.m_iObjectType;

    temp = &(pCustPlan[k].m_iObjectOffset);
    while (*temp) {
        if (pObject[i].m_iPricingObjectID < pObject[*temp].m_iPricingObjectID) {
            pObject[i].m_iNextOffset = *temp;
            *temp = i;
            break;
        }

        temp = &(pObject[*temp].m_iNextOffset);
    }

    if (!(*temp))
        *temp = i;
}

//##ModelId=424BE3C40012
void UserInfoAdmin::updateProductInfo(ProductInfo &data, bool isinsert,bool bLog)
{
    unsigned int i;
    ProductInfo * p = (ProductInfo *)(*m_poProductData);

    if (isinsert) {
        i = m_poProductData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+21);
    } else {
        if (!(m_poProductIndex->get(data.m_iProductID, &i))) THROW(MBC_UserInfoCtl+22);
    }

    p[i].m_iPricingPlanID = data.m_iPricingPlanID;
    p[i].m_iProviderID = data.m_iProviderID;
    p[i].m_iNetworkID = data.m_iNetworkID;
    p[i].m_iNetworkPriority = data.m_iNetworkPriority;

    if (isinsert) {
        m_poProductIndex->add(data.m_iProductID, i);
    }
}

//##ModelId=424BE3CE0388
void UserInfoAdmin::updateOfferInfo(OfferInfo &data, bool isinsert,bool bLog)
{
    unsigned int i;
    OfferInfo * p = (OfferInfo *)(*m_poOfferData);

    if (isinsert) {
        i = m_poOfferData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+23);
    } else {
        if (!(m_poOfferIndex->get(data.m_iOfferID, &i))) THROW(MBC_UserInfoCtl+24);
    }

    p[i].m_iOfferID = data.m_iOfferID;
    p[i].m_iPricingPlan = data.m_iPricingPlan;

    if (isinsert) {
        m_poOfferIndex->add (data.m_iOfferID, i);
    }
}
//开发测试 add by xn 2009.10.22
void UserInfoAdmin::updateNpInfo(NpInfo &data, bool isInsert, bool bLog)
{

	unsigned int i,k,pre;
	NpInfo * p = (NpInfo *)(*m_poNpData);
	
	if(!isInsert)//更新时
	{
		if (!(m_poNpIndex->get(data.m_sAccNbr, &k)))
  	{
  		THROW(MBC_NP+2);//
  	}
  	/*
  	while(k>0)
  	{
  		pre = k;
  		k = p[k].m_iNext;
  	}
  	
  	strcpy(p[pre].m_sExpDate , data.m_sExpDate);//
  	*/
  	strcpy(p[k].m_sExpDate , data.m_sEffDate);
	}
	else
	{
		
	  i = m_poNpData->malloc ();
	  if (!i) THROW(MBC_NP+1);
	 
		strcpy( p[i].m_sAccNbr, data.m_sAccNbr );
		p[i].m_iOrgNetwork = data.m_iOrgNetwork;
		p[i].m_iNetwork = data.m_iNetwork;
		p[i].m_iNpState = data.m_iNpState;
		strcpy( p[i].m_sEffDate, data.m_sEffDate );
		strcpy( p[i].m_sExpDate, data.m_sExpDate );
		p[i].m_iNext = 0;
		
		//按照生效时间从小到大链起来
		
		if (m_poNpIndex->get(p[i].m_sAccNbr, &k))
		{
			/*
			while(k)
			{
				pre = k;
				k = p[k].m_iNext;
			}
			p[pre].m_iNext = i;
			*/
			if(strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0)
			{
				p[i].m_iNext = k;
				m_poNpIndex->add (p[i].m_sAccNbr, i);
			}
			else
			{
				unsigned int * temp = &(p[k].m_iNext);
				while(*temp)
			{
					if(strcmp(p[i].m_sEffDate,p[*temp].m_sEffDate)>=0)
						break;
					temp = &(p[*temp].m_iNext);
			}
				p[i].m_iNext = *temp;
				*temp = i;
			}
		}
		else
		{
			m_poNpIndex->add (p[i].m_sAccNbr, i);
		}
		
	}
	
}
//##ModelId=424BE3D7009C
void UserInfoAdmin::updateServInfo(ServInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    ServInfo * p = (ServInfo *)(*m_poServData);

    if (!m_poCustIndex->get (data.m_lCustID, &k)) {
        THROW (MBC_UserInfoCtl+50);
    }
    if (isinsert) {
        i = m_poServData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+25);
    } else {
        if (!(m_poServIndex->get(data.m_lServID, &i))) THROW(MBC_UserInfoCtl+26);
    }

    p[i].m_lServID = data.m_lServID;
    p[i].m_lCustID = data.m_lCustID;
    p[i].m_iProductID = data.m_iProductID;
    strcpy (p[i].m_sState, data.m_sState);
    strcpy (p[i].m_sAccNbr, data.m_sAccNbr);
    p[i].m_iCycleType = data.m_iCycleType;
    p[i].m_iBillingMode = data.m_iBillingMode;
    p[i].m_iFreeType = data.m_iFreeType;
    strcpy (p[i].m_sCreateDate, data.m_sCreateDate);
    strcpy (p[i].m_sStateDate, data.m_sStateDate);
    strcpy (p[i].m_sRentDate, data.m_sRentDate);
    strcpy (p[i].m_sNscDate, data.m_sNscDate);

    if (isinsert) {
        p[i].m_iCustNext = pCust[k].m_iServOffset;
        pCust[k].m_iServOffset = i;
        p[i].m_iCustOffset = k;

        m_poServIndex->add (p[i].m_lServID, i);
    } else {
        if (p[i].m_iCustOffset != k) {
            /* 过户     */
            unsigned int * pTemp = &(pCust[p[i].m_iCustOffset].m_iServOffset);
            while (*pTemp && *pTemp!=i) {
                pTemp = &(p[*pTemp].m_iCustNext);
            }

            if (*pTemp == i) {
                *pTemp = p[i].m_iCustNext;
            }

            p[i].m_iCustNext = pCust[k].m_iServOffset;
            pCust[k].m_iServOffset = i;
            p[i].m_iCustOffset = k;
        }
    }

}

//##ModelId=424BE3E30035
void UserInfoAdmin::updateServLocationInfo(ServLocationInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;

    ServLocationInfo * p = (ServLocationInfo *)(*m_poServLocationData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);

    //##用户地域属性只支持insert，因为一个用户某时刻只可能属于一个org，
    //##所以，按照生效时间就可以判断，在内存里面只需要生效时间就可以了
    //if (!isinsert) return;

    //##定位
    if (!(m_poServIndex->get(data.m_lServID, &k))) THROW(MBC_UserInfoCtl+27);
    if (!isinsert) {
        unsigned int uiTemp = pServ[k].m_iLocationOffset;
        while (uiTemp) {
            if (p[uiTemp].m_iOrgID == data.m_iOrgID && strcmp(p[uiTemp].m_sEffDate,data.m_sEffDate) == 0) {
                strcpy(p[uiTemp].m_sExpDate,data.m_sExpDate);
                return;
            }
            uiTemp = p[uiTemp].m_iNextOffset;
        }
        THROW(MBC_UserInfoCtl+27);
    }
    i = m_poServLocationData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+28);

    p[i].m_lServID = data.m_lServID;
    p[i].m_iOrgID = data.m_iOrgID;
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    //20060724
    p[i].m_lServID = k;

    //##按照生效时间插入到链中
    unsigned int * temp = &(pServ[k].m_iLocationOffset);
    while (*temp && strcmp(p[*temp].m_sEffDate, data.m_sEffDate)>0 ) {
        temp = &(p[*temp].m_iNextOffset);
    }
    if (*temp) {
        strcpy(p[*temp].m_sExpDate, data.m_sEffDate);
    }

    p[i].m_iNextOffset = *temp;
    *temp = i;

    if (m_poServLocationIndex->get (p[i].m_iOrgID, &k)) {
        p[i].m_iOrgNext = k;
    } else {
        p[i].m_iOrgNext = 0;
    }

    m_poServLocationIndex->add (p[i].m_iOrgID, i);
}

//##ModelId=424BE3EC0377
void UserInfoAdmin::updateServStateInfo(ServStateInfo &data, bool isinsert,bool bLog)
{
    m_poServStateLock->P();
    unsigned int i, k;
    unsigned int iTemp;

    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServStateInfo * p = (ServStateInfo *)(*m_poServStateData);

    //##用户状态属性只支持insert，因为一个用户某时刻只可能一个状态，
    //##所以，按照生效时间就可以判断，在内存里面只需要生效时间就可以了
    /*if (!isinsert)
    {
            m_poServStateLock->V();
            return;
    }*/
    //##定位
    if (!(m_poServIndex->get(data.m_lServID, &k))) {
        m_poServStateLock->V();
        THROW(MBC_UserInfoCtl+29);
    }
    if (!isinsert) {
        iTemp =  pServ[k].m_iStateOffset;
        while (iTemp) {
            if (strcmp(p[iTemp].m_sEffDate,data.m_sEffDate) == 0) {
                p[iTemp].m_iOweType = data.m_iOweType;
                strcpy(p[iTemp].m_sState,data.m_sState);
                strcpy(p[iTemp].m_sExpDate,data.m_sExpDate);
                m_poServStateLock->V();
                return;
            }
            iTemp = p[iTemp].m_iNextOffset;
        }
        m_poServStateLock->V();
        THROW(MBC_UserInfoCtl+29);
    }
    //检查记录是否已存在
    iTemp = pServ[k].m_iStateOffset;
    while (iTemp) {
        if (!strcmp(p[iTemp].m_sState,data.m_sState) && !strcmp(p[iTemp].m_sEffDate,data.m_sEffDate)) {
            m_poServStateLock->V();
            return;
        }
        iTemp = p[iTemp].m_iNextOffset;
    }
    i = m_poServStateData->malloc ();
    if (!i) {
        m_poServStateLock->V();
        THROW(MBC_UserInfoCtl+30);
    }
    p[i].m_lServID = data.m_lServID;
    p[i].m_iOweType = data.m_iOweType;
    strcpy (p[i].m_sState, data.m_sState);
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    //##按照生效时间插入到链中
    unsigned int * temp = &(pServ[k].m_iStateOffset);
    while (*temp && strcmp(p[*temp].m_sEffDate, data.m_sEffDate)>0 ) {
        temp = &(p[*temp].m_iNextOffset);
    }

    p[i].m_iNextOffset = *temp;
    *temp = i;

    m_poServStateLock->V();
}

//##ModelId=424BE3F60032
void UserInfoAdmin::updateServIdentInfo(ServIdentInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k = 0;

    ServIdentInfo * p = (ServIdentInfo *)(*m_poServIdentData);
    ServInfo *pServ = (ServInfo *)(*m_poServData);
    ServInfo * pTemp;

    if (!m_poServIndex->get(data.m_lServID, &k)) {
        THROW (MBC_UserInfoCtl+33);
    }

    if (!isinsert) {
        if (!m_poServIdentIndex->get (data.m_sAccNbr, &k)) {
            THROW (MBC_UserInfoCtl+32);    
        }

        while (k) {
            if (p[k].m_lServID==data.m_lServID &&
                !strcmp (p[k].m_sEffDate, data.m_sEffDate) &&
                p[k].m_iType == data.m_iType) {
                strcpy (p[k].m_sExpDate, data.m_sExpDate);
                return;
            }

            k = p[k].m_iNextOffset;
        }

        THROW (MBC_UserInfoCtl+32);
    }

    i = m_poServIdentData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+31);

    p[i].m_lServID = data.m_lServID;
    strcpy (p[i].m_sAccNbr, data.m_sAccNbr);
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);
    p[i].m_iType = data.m_iType;
    p[i].m_iServOffset = k;

    pTemp = pServ + p[i].m_iServOffset;
    p[i].m_iServNextOffset = pTemp->m_iServIdentOffset;
    pTemp->m_iServIdentOffset = i;

    ProductInfo * ppi = (ProductInfo *)(*m_poProductData);
    if (m_poProductIndex->get (pTemp->m_iProductID, &k)) {
        p[i].m_iNetworkID = ppi[k].m_iNetworkID;
        p[i].m_iNetworkPriority = ppi[k].m_iNetworkPriority;
    } else {
        if (bLog) Log::log (0, "号码:%s没有找到对应的网络类型", p[i].m_sAccNbr);
        p[i].m_iNetworkID = 0;
        p[i].m_iNetworkPriority = 0;
    }

    if (m_poServIdentIndex->get(data.m_sAccNbr, &k)) {
        if (p[i].m_iNetworkPriority > p[k].m_iNetworkPriority
            ||
            ( p[i].m_iNetworkPriority == p[k].m_iNetworkPriority &&
              p[i].m_iType > p[k].m_iType )
            ||
            (p[i].m_iNetworkPriority == p[k].m_iNetworkPriority &&
             p[i].m_iType == p[k].m_iType &&
             strcmp (p[i].m_sEffDate, p[k].m_sEffDate) >= 0) 
           ) {
            p[i].m_iNextOffset = k;
            m_poServIdentIndex->add (p[i].m_sAccNbr, i);        //--覆盖掉
        } else {
            unsigned int * temp = &(p[k].m_iNextOffset);
            while (*temp) {
                //找到第一个优先级不大于的地方
                if (p[i].m_iNetworkPriority >= p[*temp].m_iNetworkPriority) {
                    break;
                }
                temp = &(p[*temp].m_iNextOffset);
            }
            while (*temp &&
                   p[i].m_iNetworkPriority == p[*temp].m_iNetworkPriority) {
                //找到第一个优先级不大于的地方
                if (p[i].m_iType >= p[*temp].m_iType) {
                    break;
                }
                temp = &(p[*temp].m_iNextOffset);
            }
            while (*temp &&
                   p[i].m_iNetworkPriority == p[*temp].m_iNetworkPriority &&
                   p[i].m_iType == p[*temp].m_iType) {
                if (strcmp (p[i].m_sEffDate, p[*temp].m_sEffDate)>=0) {
                    break;
                }
                temp = &(p[*temp].m_iNextOffset);
            }
            p[i].m_iNextOffset = *temp;
            *temp = i;
        }
    } else {
        m_poServIdentIndex->add (data.m_sAccNbr, i);
    }
}

void UserInfoAdmin::updateServGroupBaseInfo(ServGroupBaseInfo &data, bool isinsert,bool bLog)
{
    //只能添加
    if (!isinsert) return;

    unsigned int i, k;
    ServGroupBaseInfo * p = (ServGroupBaseInfo *)(*m_poServGroupBaseData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);

    i = m_poServGroupBaseData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+10);

    p[i].m_lGroupID = data.m_lGroupID;
    p[i].m_lGroupServID = data.m_lGroupServID;
    p[i].m_iGroupType = data.m_iGroupType;

    if (m_poServIndex->get (p[i].m_lGroupServID, &k)) {
        //p[i].m_iCustPlanOffset = pCust[pServ[k].m_iCustOffset].m_iCustPlanOffset;
        p[i].m_iCustOffset = pServ[k].m_iCustOffset;
    } else {
        if (bLog) Log::log (0, "[ERROR] 用户群的代表用户不存在[GROUP:%ld SERV:%ld]",
                            p[i].m_lGroupID, p[i].m_lGroupServID);
    }

    m_poServGroupBaseIndex->add (p[i].m_lGroupID, i);
}

void UserInfoAdmin::updateServAttrInfo(ServAttrInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    unsigned int *temp;
    ServAttrInfo * pServAttr = (ServAttrInfo *)(*m_poServAttrData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);

    if (!m_poServIndex->get (data.m_lServID, &k)) {
        THROW (MBC_UserInfoCtl+14);
    }

    if (isinsert) {
        i = m_poServAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+10);

        pServAttr[i].m_lServID = data.m_lServID;
        pServAttr[i].m_iAttrID = data.m_iAttrID;
        strcpy (pServAttr[i].m_sValue, data.m_sValue);
        strcpy (pServAttr[i].m_sEffDate, data.m_sEffDate);
        strcpy (pServAttr[i].m_sExpDate, data.m_sExpDate);

        temp = &(pServ[k].m_iAttrOffset);
        while (*temp) {
            if (pServAttr[*temp].m_iAttrID > data.m_iAttrID) {
                pServAttr[i].m_iNextOffset = *temp;
                *temp = i;
                return;
            } else if (pServAttr[*temp].m_iAttrID == data.m_iAttrID &&
                       strcmp (pServAttr[i].m_sExpDate, pServAttr[*temp].m_sExpDate)>=0) {
                pServAttr[i].m_iNextOffset = *temp;
                *temp = i;
                return;
            } else {
                temp = &(pServAttr[*temp].m_iNextOffset);
            }
        }

        *temp = i;
        return;
    }

    k = pServ[k].m_iAttrOffset;
    while (k) {
        if (pServAttr[k].m_iAttrID==data.m_iAttrID && !strcmp (pServAttr[k].m_sValue, data.m_sValue)
            && !strcmp(pServAttr[k].m_sEffDate, data.m_sEffDate)  ) {
            strcpy (pServAttr[k].m_sExpDate, data.m_sExpDate);
            return;
        }

        k = pServAttr[k].m_iNextOffset;
    }
}

//##ModelId=424BE40101E7
void UserInfoAdmin::updateServGroupInfo(ServGroupInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServGroupInfo * p = (ServGroupInfo *)(*m_poServGroupData);

    //##理论上说，只有主表支持update，但这里ServGroupInfo也考虑支持update

    //##定位
    if (!(m_poServIndex->get(data.m_lServID, &k))) THROW(MBC_UserInfoCtl+32);

    if (isinsert) {
        i = m_poServGroupData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+33);
    } else {
        //##找到组合关系
        i = pServ[k].m_iGroupOffset;
        while (i) {
            if (p[i].m_lGroupID == data.m_lGroupID)
                break;
            i = p[i].m_iNextOffset;
        }
        if (!i) THROW(MBC_UserInfoCtl+34);
    }

    p[i].m_lServID = data.m_lServID;
    p[i].m_lGroupID = data.m_lGroupID;
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    //##如果是insert的话，要按照groupid的顺序插入到链中
    if (isinsert) {
        unsigned int * temp = &(pServ[k].m_iGroupOffset);
        while (*temp && (p[*temp].m_lGroupID < data.m_lGroupID)) {
            temp = &(p[*temp].m_iNextOffset);
        }

        p[i].m_iNextOffset = *temp;
        *temp = i;
    }
}

//##ModelId=424BE41302B5
void UserInfoAdmin::updateServProductInfo(ServProductInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    unsigned int * temp;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServProductInfo * p = (ServProductInfo *)(*m_poServProductData);

    if (!(m_poServIndex->get(data.m_lServID, &k))) THROW(MBC_UserInfoCtl+35);

    if (isinsert) {
        i = m_poServProductData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+36);
    } else {
        temp = &(pServ[k].m_iServProductOffset);
        i = pServ[k].m_iServProductOffset;
        while (i) {
            if (p[i].m_lServProductID == data.m_lServProductID)
                break;
            temp = &(p[i].m_iNextOffset);
            i = p[i].m_iNextOffset;
        }
        if (!i) THROW(MBC_UserInfoCtl+37);
        *temp = p[i].m_iNextOffset;
    }

    p[i].m_lServID = data.m_lServID;
    p[i].m_lServProductID = data.m_lServProductID;
    p[i].m_iProductID = data.m_iProductID;
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    temp = &(pServ[k].m_iServProductOffset);
    while (*temp && (strcmp(data.m_sEffDate, p[*temp].m_sEffDate)>0)) {
        temp = &(p[*temp].m_iNextOffset);
    }

    p[i].m_iNextOffset = *temp;
    *temp = i;

    m_poServProductIndex->add (data.m_lServProductID, i);
}

//##ModelId=424BE41C03BC
void UserInfoAdmin::updateServAcctInfo(ServAcctInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k,m;

    //state order
    unsigned int  iTemp;
    unsigned int * pTemp = NULL;
    unsigned int iUpState, iNext,iNextTemp, iUp=0;
    int iZero=0;
    //end state order

    ServAcctInfo * p = (ServAcctInfo *)(*m_poServAcctData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);

    if (!(m_poServIndex->get(data.m_lServID, &k))) THROW(MBC_UserInfoCtl+38);

    if (isinsert) {
        i = m_poServAcctData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+39);
    } else {
        i = pServ[k].m_iServAcctOffset;
        while (i) {
            if (p[i].m_lServAcctID== data.m_lServAcctID) {
                p[i].m_iState = data.m_iState;
                strcpy (p[i].m_sExpDate,data.m_sExpDate);
                goto __ORDER;
            }
            i = p[i].m_iNextOffset;
        }
        if (!i) THROW(MBC_UserInfoCtl+40);
    }
    p[i].m_lServAcctID = data.m_lServAcctID;
    p[i].m_lServID = data.m_lServID;
    p[i].m_lAcctID = data.m_lAcctID;
    p[i].m_iAcctItemGroup = data.m_iAcctItemGroup;
    p[i].m_iState = data.m_iState;

    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    if (isinsert) {
        unsigned int * temp = &(pServ[k].m_iServAcctOffset);
        //date order
        while (*temp) {
            if (strcmp(p[*temp].m_sEffDate, data.m_sEffDate)<=0) {
                //strcpy (p[*temp].m_sExpDate, data.m_sEffDate);
                p[i].m_iNextOffset = *temp;
                *temp = i;
                break;
            }
            //失效时间取自state_date，此处不再处理
            /*else 
{
    strcpy (p[i].m_sExpDate, p[*temp].m_sEffDate);
}*/
            temp = &(p[*temp].m_iNextOffset);
        }

        if (!(*temp)) {
            p[i].m_iNextOffset = 0;
            *temp = i;
        }
        if (m_poServAcctIndex->get (p[i].m_lAcctID, &m)) {
            p[i].m_iAcctNextOffset = m;
        }
        m_poServAcctIndex->add (p[i].m_lAcctID, i);
    }
    __ORDER:
    //state order
    iTemp = pServ[k].m_iServAcctOffset;
    pTemp =&(pServ[k].m_iServAcctOffset);
    iUp=0;
    iZero=0;
    while (iTemp) {
        iUpState = p[iTemp].m_iState;
        iNext = p[iTemp].m_iNextOffset;
        if (iUpState == 0) {
            iZero++;
        } else {
            if (iZero>0) {
                while (*pTemp) {
                    if (p[*pTemp].m_iState == 0) {
                        iNextTemp = p[iTemp].m_iNextOffset;
                        p[iTemp].m_iNextOffset = *pTemp;
                        *pTemp = p[iUp].m_iNextOffset;
                        p[iUp].m_iNextOffset = iNextTemp;
                        break;
                    }
                    pTemp=&(p[*pTemp].m_iNextOffset);
                }
                pTemp =&(pServ[k].m_iServAcctOffset);
            }
        }
        iUp = iTemp;
        iTemp = iNext;
    }
    return;
}

//##ModelId=424BE4270047
void UserInfoAdmin::updateOfferInstanceInfo(InstanceDetailUpdateInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;
    OfferInstanceInfo * p = (OfferInstanceInfo *)(*m_poOfferInstanceData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    OfferInfo * pOffer = (OfferInfo *)(*m_poOfferData);

    //##update 商品明细实例表，当update商品实例表时，不需要对这个进行操作
    //##只有当发生对明细实例表的更改的时候，才调用此接口
    //##这个的更新比较特殊，要求高层次的先更新（insert）
    if (isinsert) {
        i = m_poOfferInstanceData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+41);

        p[i].m_lDetailID = data.m_lDetailID;
        p[i].m_lOfferInstanceID = data.m_lOfferInstanceID;
        p[i].m_iOfferID = data.m_iOfferID;
        strcpy (p[i].m_sEffDate, data.m_sEffDate);
        strcpy (p[i].m_sExpDate, data.m_sExpDate);
        strcpy (p[i].m_sState, data.m_sState);
        //##确定定价计划
        if (m_poOfferIndex->get (p[i].m_iOfferID, &k)) {
            p[i].m_iPricingPlan = pOffer[k].m_iPricingPlan;    
        }
        //##确定父亲
        m_poOfferInstanceIndex->get(p[i].m_lOfferInstanceID, 
                                    &(p[i].m_iParentOffset));

        //##新增的实例明细
        if ((data.m_sObjectType[2]|0x20) == 'a') {
            //如果child类型为主产品实例
            /////p[i].m_lChildInstanceID = 0;
            if (!(m_poServIndex->get(data.m_lObjectID, &k))) {
                THROW(MBC_UserInfoCtl+42);
            }
            p[i].m_iNextOffset = pServ[k].m_iOfferInstanceOffset;
            pServ[k].m_iOfferInstanceOffset = i;
        } else {
            //如果child类型为商品实例
            //插到头部的下一个
            /////p[i].m_lChildInstanceID = data.m_lObjectID;

            if (m_poOfferInstanceIndex->get(data.m_lObjectID, &k)) {
                p[i].m_iNextOffset = p[k].m_iNextOffset;
                p[k].m_iNextOffset = i;
            } else {
                m_poOfferInstanceIndex->add(data.m_lObjectID, i);
            }    
        }
    } else {
        //更新
        //##更新为更新时间或者更新商品或者更新状态
        //##首先要找到
        //##理论上说不应该有修改？

        //##更新的实例明细
        if ((data.m_sObjectType[2]|0x20) == 'a') {
            //如果child类型为主产品实例
            if (!(m_poServIndex->get(data.m_lObjectID, &k))) THROW(MBC_UserInfoCtl+42);
            i = pServ[k].m_iOfferInstanceOffset;

        } else {
            //如果child类型为商品实例
            if (!(m_poOfferInstanceIndex->get(data.m_lObjectID,&i)))
                THROW(MBC_UserInfoCtl+43);
        }

        while (i) {
            if (p[i].m_lDetailID == data.m_lDetailID)
                break;
            i = p[i].m_iNextOffset;
        }
        if (!i) THROW(MBC_UserInfoCtl+44);

        p[i].m_iOfferID = data.m_iOfferID;
        strcpy (p[i].m_sEffDate, data.m_sEffDate);
        strcpy (p[i].m_sExpDate, data.m_sExpDate);
        strcpy (p[i].m_sState, data.m_sState);

        //##更改商品带来定价计划的修改
        if (m_poOfferIndex->get (p[i].m_iOfferID, &k)) {
            p[i].m_iPricingPlan = pOffer[k].m_iPricingPlan;    
        }
    }
}

#ifdef GROUP_CONTROL
void UserInfoAdmin::updateAccNbrOfferType(AccNbrOfferType& data,bool isinsert,bool bLog)
{
    unsigned int i, k,m;
    unsigned int *temp;

    AccNbrOfferType * pAccNbrOfferType = (AccNbrOfferType*)(*m_poAccNbrData);
    if (!isinsert) {
        if (!m_poAccNbrItemIndex->get(data.m_lItemID,&i)) {
            THROW (MBC_UserInfoCtl+61); //内存里面没有发现记录
        } else {
            strcpy(pAccNbrOfferType[i].m_sEffDate,data.m_sEffDate);
            strcpy(pAccNbrOfferType[i].m_sExpDate,data.m_sExpDate);
            strcpy(pAccNbrOfferType[i].m_sAccNbr,data.m_sAccNbr);
            pAccNbrOfferType[i].m_iOfferType = data.m_iOfferType;
            //需要重新排序
        }
    } else {
        if (m_poAccNbrItemIndex->get(data.m_lItemID,&i)) {
            THROW (MBC_UserInfoCtl+62); //内存里面有发现记录
        } else {
            i = m_poAccNbrData->malloc ();
            if (!i) THROW(MBC_UserInfoCtl+60);
            pAccNbrOfferType[i].m_iOfferType = data.m_iOfferType;
            pAccNbrOfferType[i].m_lItemID = data.m_lItemID;
            strcpy(pAccNbrOfferType[i].m_sAccNbr,data.m_sAccNbr);
            strcpy(pAccNbrOfferType[i].m_sEffDate,data.m_sEffDate);
            strcpy(pAccNbrOfferType[i].m_sExpDate,data.m_sExpDate);

            m_poAccNbrItemIndex->add(data.m_lItemID,i);
            if (m_poAccNbrIndex->get(data.m_sAccNbr,&k)) {
                if (strcmp(data.m_sEffDate ,pAccNbrOfferType[k].m_sEffDate)>=0) {
                    pAccNbrOfferType[i].m_iNextOffset = k;
                    m_poAccNbrIndex->add (pAccNbrOfferType[i].m_sAccNbr, i);
                    return ;
                }
                unsigned int *piTemp=&(pAccNbrOfferType[k].m_iNextOffset);
                while ((*piTemp)&&(strcmp(data.m_sEffDate ,pAccNbrOfferType[*piTemp].m_sEffDate)<0)) {
                    piTemp=&(pAccNbrOfferType[*piTemp].m_iNextOffset);
                }
                pAccNbrOfferType[i].m_iNextOffset =*piTemp;
                *piTemp =i;
            } else
                m_poAccNbrIndex->add (pAccNbrOfferType[i].m_sAccNbr, i);
        }
    }
}
#endif
void UserInfoAdmin::updateServTypeInfo(ServTypeInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k,m;
    unsigned int *temp;

    ServTypeInfo * pServType = (ServTypeInfo *)(*m_poServTypeData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ProductInfo * pProduct = (ProductInfo *)(*m_poProductData);

    if (!m_poServIndex->get (data.m_lServID, &k)) {
        THROW (MBC_UserInfoCtl+14);
    }

    if (isinsert) {
        i = m_poServTypeData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+45);

        pServType[i].m_lServID = data.m_lServID;
        pServType[i].m_iProductID = data.m_iProductID;
        strcpy (pServType[i].m_sEffDate, data.m_sEffDate);
        strcpy (pServType[i].m_sExpDate, data.m_sExpDate);

        if (m_poProductIndex->get (pServType[i].m_iProductID, &m)) {
            pServType[i].m_iPricingPlanID = pProduct[m].m_iPricingPlanID;
        }

        /*if (!m_poServIndex->get (pServType[i].m_lServID, &k))
                THROW (MBC_UserInfoCtl+46);*/

        temp = &(pServ[k].m_iServTypeOffset);
        while (*temp) {
            if (strcmp (data.m_sEffDate, pServType[*temp].m_sEffDate)>=0) {
                pServType[i].m_iNextOffset = *temp;
                *temp = i;
                return;
            } else {
                temp = &(pServType[*temp].m_iNextOffset);
            }
        }
        *temp = i;
        return;
    }

    i = pServ[k].m_iServTypeOffset;
    while (i) {
        if (pServType[i].m_iProductID==data.m_iProductID &&
            !strcmp (pServType[i].m_sEffDate, data.m_sEffDate)) {
            strcpy (pServType[i].m_sExpDate, data.m_sExpDate);
        }
        i = pServType[i].m_iNextOffset;
    }
}

void UserInfoAdmin::loadServTypeInfo()
{
    /*int j=0;
    for(int i=0;i<1000;i++)
    {
            j++;
            if(j==100)
            {
                    Log::log(0,"loadServTypeInfo");
                    j=0;
                    sleep(1);
            }
    }
    return ;*/

    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"astc","a_serv_type_change",qry)) return;

    int count,num;

    ServTypeInfo * pServType;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ProductInfo * pProduct = (ProductInfo *)(*m_poProductData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*qry.setSQL ("select count(*) from a_serv_type_change");

    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_ASERVTYPECHANGE, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ASERVTYPECHANGE, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServTypeData->create (CAL_COUNT(count));
    pServType = (ServTypeInfo *)(*m_poServTypeData);

    qry.setSQL ("select serv_id, product_id, "
                " eff_date, "
                "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
                "from a_serv_type_change");
    qry.open ();
    
    while (qry.next()) {
        if (!m_poServIndex->get(qry.field(0).asLong(), &k)) {
            if (!m_bLogClose) Log::log (0, "产品信息找不到对应的用户:%ld", qry.field(0).asLong());
            continue;
        }
        if (strcmp(qry.field(3).asString(),strDate)<0) {
            continue;
        }
        i = m_poServTypeData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+45);

        pServType[i].m_lServID = qry.field(0).asLong ();
        pServType[i].m_iProductID = qry.field(1).asInteger ();
        strcpy (pServType[i].m_sEffDate, qry.field(2).asString ());
        strcpy (pServType[i].m_sExpDate, qry.field(3).asString ());

        //if (!m_poServIndex->get(pServType[i].m_lServID, &k)) {
        //    if (!m_bLogClose) Log::log (0, "产品信息找不到对应的用户:%ld", pServType[i].m_lServID);
        //    continue;
        //}

        pTemp = &(pServ[k].m_iServTypeOffset);

        while (*pTemp) {
            if (strcmp (pServType[i].m_sEffDate, pServType[*pTemp].m_sEffDate)>=0) {
                break;
            }

            pTemp = &(pServType[*pTemp].m_iNextOffset);
        }

        pServType[i].m_iNextOffset = *pTemp;
        *pTemp = i;

        if (m_poProductIndex->get (pServType[i].m_iProductID, &k)) {
            pServType[i].m_iPricingPlanID = pProduct[k].m_iPricingPlanID;
        }
    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadServAttrInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);   
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"sattr","serv_attr",qry)) return;

    int count,num;

    ServAttrInfo * pServAttr;
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*qry.setSQL ("select count(*) from serv_attr");

    qry.open (); qry.next ();
    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVATTR, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVATTR, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServAttrData->create (CAL_COUNT(count));
    pServAttr = (ServAttrInfo *)(*m_poServAttrData);

    qry.setSQL ("select serv_id, attr_id, attr_val, "
                " eff_date, "
                "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
                "from serv_attr");

    qry.open ();
    
    while (qry.next()) {
        if (!m_poServIndex->get (qry.field(0).asLong (), &k)) {
            if (!m_bLogClose) Log::log (0, "用户属性找不到对应的用户:%ld", qry.field(0).asLong ());
            continue;
        }
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        i = m_poServAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+45);

        pServAttr[i].m_lServID = qry.field(0).asLong ();
        pServAttr[i].m_iAttrID = qry.field(1).asInteger ();
        strncpy (pServAttr[i].m_sValue, qry.field(2).asString (), sizeof (pServAttr[i].m_sValue)-1);
        strcpy (pServAttr[i].m_sEffDate, qry.field(3).asString ());
        strcpy (pServAttr[i].m_sExpDate, qry.field(4).asString ());

        if (!m_poServIndex->get (pServAttr[i].m_lServID, &k)) {
            if (!m_bLogClose) Log::log (0, "用户属性找不到对应的用户:%ld", pServAttr[i].m_lServID);
            continue;
        }

        pTemp = &(pServ[k].m_iAttrOffset);

        while (*pTemp) {
            if (pServAttr[i].m_iAttrID < pServAttr[*pTemp].m_iAttrID ) {
                break;
            }

            if (pServAttr[i].m_iAttrID == pServAttr[*pTemp].m_iAttrID) {
                if (strcmp (pServAttr[i].m_sEffDate, pServAttr[*pTemp].m_sEffDate)>=0) {
                    break;
                }
            }

            pTemp = &(pServAttr[*pTemp].m_iNextOffset);
        }

        pServAttr[i].m_iNextOffset = *pTemp;
        *pTemp = i;

    }

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

//##ModelId=425771C00357
void UserInfoAdmin::loadServProductAttrInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
        #ifdef DEF_JIANGSU
    if (!insertLog(iSeq,"bspac","b_serv_product_attr_change",qry)) return;
        #else
    if (!insertLog(iSeq,"spa","serv_product_attr",qry)) return;
        #endif  

    int count,num;

    ServProductAttrInfo * pServProductAttr;
    ServProductInfo * pServProduct = (ServProductInfo *)(*m_poServProductData);
    unsigned int i, k;

    unsigned int * pTemp;

    /*qry.setSQL ("select count(*) from serv_product_attr");

    qry.open (); qry.next ();

    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVPRODUCTATTR, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVPRODUCTATTR, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);
    
	m_poServProductAttrData->create (CAL_COUNT(count));
    pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
        #ifdef DEF_JIANGSU
    qry.setSQL ("select serv_product_id, attr_id, attr_val, "
                " eff_date, "
                "nvl(exp_date,  to_date('20500101', 'yyyymmdd')) exp_date "
                "from b_serv_product_attr_change");
        #else
    qry.setSQL ("select serv_product_id, attr_id, attr_val, "
                " eff_date, "
                "nvl(exp_date,  to_date('20500101', 'yyyymmdd')) exp_date "
                "from serv_product_attr");
        #endif
    qry.open ();

    while (qry.next()) {
        if (!m_poServProductIndex->get(qry.field(0).asLong (), &k)) {
            if (!m_bLogClose) Log::log (0, "附属产品属性没有找到对应的附属产品[%ld][%d]",
                                        qry.field(0).asLong (), qry.field(1).asInteger () );
            continue;
        }
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        i = m_poServProductAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+45);

        pServProductAttr[i].m_lServProductID = qry.field(0).asLong ();
        pServProductAttr[i].m_iAttrID = qry.field(1).asInteger ();
        strncpy (pServProductAttr[i].m_sValue, qry.field(2).asString (),
                 sizeof (pServProductAttr[i].m_sValue)-1);
        strcpy (pServProductAttr[i].m_sEffDate, qry.field(3).asString ());
        strcpy (pServProductAttr[i].m_sExpDate, qry.field(4).asString ());

        if (!m_poServProductIndex->get(pServProductAttr[i].m_lServProductID, &k)) {
             if (!m_bLogClose) Log::log (0, "附属产品属性没有找到对应的附属产品[%ld][%d]",
                                         pServProductAttr[i].m_lServProductID, pServProductAttr[i].m_iAttrID);
             continue;
        }

        pTemp = &(pServProduct[k].m_iAttrOffset);

        while (*pTemp) {
            if (pServProductAttr[i].m_iAttrID < pServProductAttr[*pTemp].m_iAttrID) {
                break;
            }

            if (pServProductAttr[i].m_iAttrID == pServProductAttr[*pTemp].m_iAttrID) {
                if (strcmp (pServProductAttr[i].m_sEffDate, 
                            pServProductAttr[*pTemp].m_sEffDate) >= 0) {
                    break;
                }
            }

            pTemp = &(pServProductAttr[*pTemp].m_iNextOffset);
        }

        pServProductAttr[i].m_iNextOffset = *pTemp;
        *pTemp = i;

    }//end while

    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadServProductAttrInfoDB()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
        #ifdef DEF_JIANGSU
    if (!insertLog(iSeq,"bspac","b_serv_product_attr_change",qry)) return;
        #else
    if (!insertLog(iSeq,"spa","serv_product_attr",qry)) return;
        #endif  

    int count,num;

    ServProductAttrInfo * pServProductAttr;
    unsigned int i, k;

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVPRODUCTATTR, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVPRODUCTATTR, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServProductAttrData->create (CAL_COUNT(count));
    pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);

        #ifdef DEF_JIANGSU
    qry.setSQL ("select serv_product_id, attr_id, attr_val, "
                " eff_date, "
                "nvl(exp_date,  to_date('20500101', 'yyyymmdd')) exp_date "
                "from b_serv_product_attr_change");
        #else
    qry.setSQL ("select serv_product_id, attr_id, attr_val, "
                " eff_date, "
                "nvl(exp_date,  to_date('20500101', 'yyyymmdd')) exp_date "
                "from serv_product_attr");
        #endif
    qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        i = m_poServProductAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+45);
        pServProductAttr[i].m_lServProductID = qry.field(0).asLong ();
        pServProductAttr[i].m_iAttrID = qry.field(1).asInteger ();
        strncpy (pServProductAttr[i].m_sValue, qry.field(2).asString (),sizeof (pServProductAttr[i].m_sValue)-1);
        strcpy (pServProductAttr[i].m_sEffDate, qry.field(3).asString ());
        strcpy (pServProductAttr[i].m_sExpDate, qry.field(4).asString ());
        pServProductAttr[i].m_iNextOffset = 0;
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadServProductAttrInfoMEM()
{
    while (!m_bSPAOK || !m_bSPOK) {
        sleep(2);
    }
    unsigned int i, k;
    unsigned int * pTemp;

    ServProductAttrInfo * pServProductAttr;
    ServProductInfo * pServProduct = (ServProductInfo *)(*m_poServProductData);
    pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);

    unsigned int iCount = m_poServProductAttrData->getCount();
    i = 1;
    for (;i<=iCount;i++) {
        if (!m_poServProductIndex->get(pServProductAttr[i].m_lServProductID, &k)) {
            if (!m_bLogClose) Log::log (0, "附属产品属性没有找到对应的附属产品[%ld][%d]",
                                        pServProductAttr[i].m_lServProductID, pServProductAttr[i].m_iAttrID);
            continue;
        }
        pTemp = &(pServProduct[k].m_iAttrOffset);
        while (*pTemp) {
            if (pServProductAttr[i].m_iAttrID < pServProductAttr[*pTemp].m_iAttrID) {
                break;
            }
            if (pServProductAttr[i].m_iAttrID == pServProductAttr[*pTemp].m_iAttrID) {
                if (strcmp (pServProductAttr[i].m_sEffDate, 
                            pServProductAttr[*pTemp].m_sEffDate) >= 0) {
                    break;
                }
            }
            pTemp = &(pServProductAttr[*pTemp].m_iNextOffset);
        }
        pServProductAttr[i].m_iNextOffset = *pTemp;
        *pTemp = i;
    }
}

//##ModelId=425771CC008D
void UserInfoAdmin::updateServProductAttrInfo(ServProductAttrInfo &data, bool isinsert,bool bLog)
{
    unsigned int i, k;

    ServProductInfo * pServProduct = (ServProductInfo *)(*m_poServProductData);
    ServProductAttrInfo * p = (ServProductAttrInfo *)(*m_poServProductAttrData);

    //##用户状态属性只支持insert，因为一个用户某时刻只可能一个状态，
    //##所以，按照生效时间就可以判断，在内存里面只需要生效时间就可以了
    if (!isinsert) {//更新
        if (!(m_poServProductIndex->get(data.m_lServProductID,&k))) {
            THROW(MBC_UserInfoCtl+46);
        }
        unsigned int j = pServProduct[k].m_iAttrOffset;
        while (j && p[j].m_iAttrID<data.m_iAttrID) {
            j = p[j].m_iNextOffset;
        }
        while (j && p[j].m_iAttrID==data.m_iAttrID) {
            if (strcmp(p[j].m_sValue,data.m_sValue)==0 && 
                strcmp(p[j].m_sEffDate,data.m_sEffDate)==0) {
                strcpy(p[j].m_sExpDate,data.m_sExpDate);
                return ;
            }
            j = p[j].m_iNextOffset;    
        }
        THROW(MBC_UserInfoCtl+46);
    }

    //##定位
    if (!(m_poServProductIndex->get(data.m_lServProductID, &k))) THROW(MBC_UserInfoCtl+46);

    i = m_poServProductAttrData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+47);

    p[i].m_lServProductID = data.m_lServProductID;
    p[i].m_iAttrID = data.m_iAttrID;
    strcpy (p[i].m_sValue, data.m_sValue);
    strcpy (p[i].m_sEffDate, data.m_sEffDate);
    strcpy (p[i].m_sExpDate, data.m_sExpDate);

    //##按照生效时间插入到链中
    unsigned int * temp = &(pServProduct[k].m_iAttrOffset);
    while (*temp && p[*temp].m_iAttrID<data.m_iAttrID ) {
        temp = &(p[*temp].m_iNextOffset);
    }

    while (*temp && p[*temp].m_iAttrID==data.m_iAttrID &&
           strcmp(p[*temp].m_sEffDate, data.m_sEffDate)>0 ) {
        temp = &(p[*temp].m_iNextOffset);
    }

    p[i].m_iNextOffset = *temp;
    *temp = i;
}

void UserInfoAdmin::loadProdOfferIns()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int count,num;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"poi","product_offer_instance",qry)) return;
    unsigned int i;

    /*qry.setSQL ("SELECT COUNT(1) FROM PRODUCT_OFFER_INSTANCE");

    qry.open (); 
    qry.next ();

    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_PRODOFFERINS, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINS, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsData->create(CAL_COUNT(count));
    m_poProdOfferInsIndex->create(CAL_COUNT(count));

    ProdOfferIns * pProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);
    qry.setSQL ("SELECT PRODUCT_OFFER_INSTANCE_ID, PRODUCT_OFFER_ID, "
                "EFF_DATE,NVL(EXP_DATE,TO_DATE('20500101','YYYYMMDD')) "
                "FROM PRODUCT_OFFER_INSTANCE");

    qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(3).asString(),strDate)<0) {
            continue;
        }
        i = m_poProdOfferInsData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+48);

        pProdOfferIns[i].m_lProdOfferInsID = qry.field(0).asLong ();
        pProdOfferIns[i].m_iOfferID = qry.field(1).asInteger ();
        strcpy (pProdOfferIns[i].m_sEffDate, qry.field(2).asString ());

        strcpy (pProdOfferIns[i].m_sExpDate,qry.field(3).asString());
        m_poProdOfferInsIndex->add (pProdOfferIns[i].m_lProdOfferInsID, i);

    }

    qry.close();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadProdOfferInsDetail()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int count,num;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"poid","product_offer_instance_detail",qry)) return;

    unsigned int i,j,k;

    ServInfo * pServInfo = (ServInfo *)(*m_poServData);
    AcctInfo * pAcctInfo = (AcctInfo *)(*m_poAcctData);
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    ProdOfferIns * pProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);


    /*qry.setSQL ("SELECT COUNT(1) FROM PRODUCT_OFFER_INSTANCE_DETAIL");

    qry.open(); 
    qry.next();
    
    count = qry.field(0).asInteger ();
    qry.close();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_PRODOFFERINSDETAIL, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    char * pTempbuf = 0;
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSDETAIL, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(countbuf, &pTempbuf, 10);

    m_poOfferDetailInsData->create(CAL_COUNT(count));

    OfferDetailIns * pDetailIns = (OfferDetailIns*)(*m_poOfferDetailInsData);
    //创建索引
    /*qry.setSQL ("SELECT COUNT(1) FROM PRODUCT_OFFER_INSTANCE_DETAIL WHERE INSTANCE_TYPE = '10C'");

    qry.open(); 
    qry.next();
    
    count = qry.field(0).asInteger ();
    qry.close();*/
    if (!pTempbuf || *pTempbuf != '_') {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(pTempbuf+1, 0, 10);

    m_poOfferDetailInsOfferInsIndex->create(CAL_COUNT(count));

    //开始加载
    qry.setSQL ("SELECT INSTANCE_RELATION_ID, PRODUCT_OFFER_INSTANCE_ID, "
                "INSTANCE_TYPE, INSTANCE_ID, "
                "EFF_DATE, NVL(EXP_DATE,  TO_DATE('20500101', 'yyyymmdd')) EXP_DATE,OFFER_DETAIL_ID "
                "FROM PRODUCT_OFFER_INSTANCE_DETAIL");

    qry.open();

    char pType[4];
    long pOfferInsID;
    Offer * tempOffer;
    
    unsigned int * pTemp ;
    unsigned int iPre = 0;
    while (qry.next()) {
        if (strcmp(qry.field(5).asString(),strDate)<0) {
            continue;
        }
        i = m_poOfferDetailInsData->malloc();
        if (!i) THROW(MBC_UserInfoCtl+49);
        pDetailIns[i].m_lOfferDetailInsID = qry.field(0).asLong();
        pDetailIns[i].m_lInsID = qry.field(3).asLong();
        pDetailIns[i].m_iMemberID = qry.field(6).asInteger();
        strcpy(pDetailIns[i].m_sEffDate, qry.field(4).asString());
        strcpy(pDetailIns[i].m_sExpDate, qry.field(5).asString());
        strcpy(pType,qry.field(2).asString());
        pOfferInsID = qry.field(1).asLong();
        //同一个商品实例下面的关系的建立
        if (!m_poProdOfferInsIndex->get(pOfferInsID, &k)) {
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的商品实例:%ld",pDetailIns[i].m_lOfferDetailInsID, pOfferInsID);
            continue;
        }
        pDetailIns[i].m_iParentOffset = k;


        pDetailIns[i].m_iNextDetailOffset = pProdOfferIns[k].m_iOfferDetailInsOffset;
        pProdOfferIns[k].m_iOfferDetailInsOffset = i;

        iPre = 0;
        pTemp = 0;
        //同一个明细实例下面的关系的建立
        switch (pType[2] | 0x20) {
        case 'a':
            pDetailIns[i].m_iInsType = SERV_INSTANCE_TYPE;
            tempOffer = m_poOfferMgr->getOfferInfoByID(pProdOfferIns[k].m_iOfferID);
            if (!tempOffer) {//商品不存在
                THROW(MBC_UserInfoCtl+50);
            }
            if (!m_poServIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的产品实例:%ld",pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            if (tempOffer->m_iOfferType == OFFER_TYPE_BASE) {//基本商品 
                j = pServInfo[k].m_iBaseOfferOffset;
                if (!j) {
                    pServInfo[k].m_iBaseOfferOffset = i;
                } else {
                    while (pDetailIns[j].m_iNextProdOfferOffset) {
                        j = pDetailIns[j].m_iNextProdOfferOffset;
                    }
                    pDetailIns[j].m_iNextProdOfferOffset = i;
                }
            } else {
                j = pServInfo[k].m_iOfferDetailOffset;
                if (!j) {
                    pServInfo[k].m_iOfferDetailOffset = i;
                } else {
                    pTemp =  &(pServInfo[k].m_iOfferDetailOffset);
                    /*while(pDetailIns[j].m_iNextProdOfferOffset)
                    {
                        j = pDetailIns[j].m_iNextProdOfferOffset;
                    }
                    pDetailIns[j].m_iNextProdOfferOffset = i;*/
                }
            }
            break;

        case 'j':
            pDetailIns[i].m_iInsType = ACCT_TYPE;
            if (!m_poAcctIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的帐户:%ld",pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            j = pAcctInfo[k].m_iOfferDetailOffset;
            if (!j) {
                pAcctInfo[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pAcctInfo[k].m_iOfferDetailOffset);
                /*while(pDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = pDetailIns[j].m_iNextProdOfferOffset;
                }
                pDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case 'i':
            pDetailIns[i].m_iInsType = CUST_TYPE;
            if (!m_poCustIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的客户:%ld",pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            j = pCust[k].m_iOfferDetailOffset;
            if (!j) {
                pCust[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pCust[k].m_iOfferDetailOffset);
                /*while(pDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = pDetailIns[j].m_iNextProdOfferOffset;
                }
                pDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case 'c':
            pDetailIns[i].m_iInsType = OFFER_INSTANCE_TYPE;//TODO商品实例类型
            if (!m_poOfferDetailInsOfferInsIndex->get(pDetailIns[i].m_lInsID, &k)) {
                m_poOfferDetailInsOfferInsIndex->add(pDetailIns[i].m_lInsID, i);
            } else {
                if (strcmp(pDetailIns[i].m_sEffDate,pDetailIns[k].m_sEffDate)<0) {
                    pDetailIns[i].m_iNextProdOfferOffset = k;
                    m_poOfferDetailInsOfferInsIndex->add(pDetailIns[i].m_lInsID, i);
                    pTemp = 0;
                } else {
                    pTemp =  &(pDetailIns[k].m_iNextProdOfferOffset);
                    if (*pTemp == 0) {
                        iPre = k;
                    }
                }
                /*while(pDetailIns[k].m_iNextProdOfferOffset)
                {
                    k = pDetailIns[k].m_iNextProdOfferOffset;
                }
                pDetailIns[k].m_iNextProdOfferOffset = i;*/
            }
            break;

        default:
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld类型错误，类型不应该为：%s",pDetailIns[i].m_lOfferDetailInsID, pType);
            continue;
            break;
        }
        if (pTemp) {
            while (*pTemp) {
                if (strcmp(pDetailIns[i].m_sEffDate,pDetailIns[*pTemp].m_sEffDate)<0) {
                    pDetailIns[i].m_iNextProdOfferOffset = *pTemp;
                    *pTemp = i;
                    break;
                }
                iPre = *pTemp;
                pTemp = &(pDetailIns[*pTemp].m_iNextProdOfferOffset);
            }
            if (*pTemp == 0) {
                pDetailIns[iPre].m_iNextProdOfferOffset = i;
            }
        }
    }

    qry.close();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadProdOfferInsDetailDB()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    int count,num;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"poid","product_offer_instance_detail",qry)) return;

    unsigned int i,j,k;

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_PRODOFFERINSDETAIL, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    char * pTempbuf = 0;
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSDETAIL, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(countbuf, &pTempbuf, 10);

    m_poOfferDetailInsData->create(CAL_COUNT(count));
    OfferDetailIns * pDetailIns = (OfferDetailIns*)(*m_poOfferDetailInsData);

    if (!pTempbuf || *pTempbuf != '_') {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(pTempbuf+1, 0, 10);

    m_poOfferDetailInsOfferInsIndex->create(CAL_COUNT(count));
    //开始加载
    qry.setSQL ("SELECT INSTANCE_RELATION_ID, PRODUCT_OFFER_INSTANCE_ID, "
                "INSTANCE_TYPE, INSTANCE_ID, "
                "EFF_DATE, NVL(EXP_DATE,  TO_DATE('20500101', 'yyyymmdd')) EXP_DATE,OFFER_DETAIL_ID "
                "FROM PRODUCT_OFFER_INSTANCE_DETAIL");

    qry.open();

    char pType[4];
    long pOfferInsID;
    Offer * tempOffer;
    
    unsigned int * pTemp ;
    unsigned int iPre = 0;
    while (qry.next()) {
        if (strcmp(qry.field(5).asString(),strDate)<0) {
            continue;
        }
        i = m_poOfferDetailInsData->malloc();
        if (!i) THROW(MBC_UserInfoCtl+49);
        pDetailIns[i].m_lOfferDetailInsID = qry.field(0).asLong();
        pDetailIns[i].m_lOfferInsID = qry.field(1).asLong();
        pDetailIns[i].m_lInsID = qry.field(3).asLong();
        pDetailIns[i].m_iMemberID = qry.field(6).asInteger();
        strcpy(pDetailIns[i].m_sEffDate, qry.field(4).asString());
        strcpy(pDetailIns[i].m_sExpDate, qry.field(5).asString());
        strcpy(pType,qry.field(2).asString());
        pDetailIns[i].m_iParentOffset = 0;
        pDetailIns[i].m_iNextDetailOffset = 0;
        switch (pType[2] | 0x20) {
        case 'a':
            pDetailIns[i].m_iInsType = SERV_INSTANCE_TYPE;
            break;
        case 'j':
            pDetailIns[i].m_iInsType = ACCT_TYPE;
            break;
        case 'i':
            pDetailIns[i].m_iInsType = CUST_TYPE;
            break;
        case 'c':
            pDetailIns[i].m_iInsType = OFFER_INSTANCE_TYPE;
            break;
        default:
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld类型错误，类型不应该为：%s",pDetailIns[i].m_lOfferDetailInsID, pType);
            pDetailIns[i].m_iInsType = -1;
            continue;
            break;
        }
    }
    qry.close();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

void UserInfoAdmin::loadProdOfferInsDetailMEM()
{
    while (!m_bPOIDOK || !m_bPOIOK) {
        sleep(2);
    }
    unsigned int i,j,k;

    ServInfo * pServInfo = (ServInfo *)(*m_poServData);
    AcctInfo * pAcctInfo = (AcctInfo *)(*m_poAcctData);
    CustInfo * pCust = (CustInfo *)(*m_poCustData);
    ProdOfferIns * pProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);
    OfferDetailIns * pDetailIns = (OfferDetailIns*)(*m_poOfferDetailInsData);

    Offer * tempOffer;

    unsigned int * pTemp ;
    unsigned int iPre = 0;

    unsigned int iCount = m_poOfferDetailInsData->getCount();
    i = 1;
    for (;i<=iCount;i++) {
        //同一个商品实例下面的关系的建立
        if (!m_poProdOfferInsIndex->get(pDetailIns[i].m_lOfferInsID, &k)) {
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的商品实例:%ld",pDetailIns[i].m_lOfferDetailInsID,
                                        pDetailIns[i].m_lOfferInsID);
            continue;
        }
        pDetailIns[i].m_iParentOffset = k;
        pDetailIns[i].m_iNextDetailOffset = pProdOfferIns[k].m_iOfferDetailInsOffset;
        pProdOfferIns[k].m_iOfferDetailInsOffset = i;

        iPre = 0;
        pTemp = 0;
        //同一个明细实例下面的关系的建立
        switch (pDetailIns[i].m_iInsType) {
        case SERV_INSTANCE_TYPE:
            tempOffer = m_poOfferMgr->getOfferInfoByID(pProdOfferIns[k].m_iOfferID);
            if (!tempOffer) {//商品不存在
                THROW(MBC_UserInfoCtl+50);
            }
            if (!m_poServIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的产品实例:%ld",
                                            pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            if (tempOffer->m_iOfferType == OFFER_TYPE_BASE) {//基本商品 
                j = pServInfo[k].m_iBaseOfferOffset;
                if (!j) {
                    pServInfo[k].m_iBaseOfferOffset = i;
                } else {
                    while (pDetailIns[j].m_iNextProdOfferOffset) {
                        j = pDetailIns[j].m_iNextProdOfferOffset;
                    }
                    pDetailIns[j].m_iNextProdOfferOffset = i;
                }
            } else {
                j = pServInfo[k].m_iOfferDetailOffset;
                if (!j) {
                    pServInfo[k].m_iOfferDetailOffset = i;
                } else {
                    pTemp =  &(pServInfo[k].m_iOfferDetailOffset);
                    /*while(pDetailIns[j].m_iNextProdOfferOffset)
                    {
                        j = pDetailIns[j].m_iNextProdOfferOffset;
                    }
                    pDetailIns[j].m_iNextProdOfferOffset = i;*/
                }
            }
            break;

        case ACCT_TYPE:
            if (!m_poAcctIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的帐户:%ld",
                                            pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            j = pAcctInfo[k].m_iOfferDetailOffset;
            if (!j) {
                pAcctInfo[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pAcctInfo[k].m_iOfferDetailOffset);
                /*while(pDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = pDetailIns[j].m_iNextProdOfferOffset;
                }
                pDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case CUST_TYPE:
            if (!m_poCustIndex->get(pDetailIns[i].m_lInsID, &k)) {
                if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的客户:%ld",
                                            pDetailIns[i].m_lOfferDetailInsID, pDetailIns[i].m_lInsID);
                continue;
            }
            j = pCust[k].m_iOfferDetailOffset;
            if (!j) {
                pCust[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pCust[k].m_iOfferDetailOffset);
                /*while(pDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = pDetailIns[j].m_iNextProdOfferOffset;
                }
                pDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case OFFER_INSTANCE_TYPE:
            if (!m_poOfferDetailInsOfferInsIndex->get(pDetailIns[i].m_lInsID, &k)) {
                m_poOfferDetailInsOfferInsIndex->add(pDetailIns[i].m_lInsID, i);
            } else {
                if (strcmp(pDetailIns[i].m_sEffDate,pDetailIns[k].m_sEffDate)<0) {
                    pDetailIns[i].m_iNextProdOfferOffset = k;
                    m_poOfferDetailInsOfferInsIndex->add(pDetailIns[i].m_lInsID, i);
                    pTemp = 0;
                } else {
                    pTemp =  &(pDetailIns[k].m_iNextProdOfferOffset);
                    if (*pTemp == 0) {
                        iPre = k;
                    }
                }
                /*while(pDetailIns[k].m_iNextProdOfferOffset)
                {
                    k = pDetailIns[k].m_iNextProdOfferOffset;
                }
                pDetailIns[k].m_iNextProdOfferOffset = i;*/
            }
            break;

        default:
            //Log::log (0, "[ERROR] 商品实例明细:%ld类型错误",pDetailIns[i].m_lOfferDetailInsID);
            continue;
            break;
        }
        if (pTemp) {
            while (*pTemp) {
                if (strcmp(pDetailIns[i].m_sEffDate,pDetailIns[*pTemp].m_sEffDate)<0) {
                    pDetailIns[i].m_iNextProdOfferOffset = *pTemp;
                    *pTemp = i;
                    break;
                }
                iPre = *pTemp;
                pTemp = &(pDetailIns[*pTemp].m_iNextProdOfferOffset);
            }
            if (*pTemp == 0) {
                pDetailIns[iPre].m_iNextProdOfferOffset = i;
            }
        }
    }
}

void UserInfoAdmin::loadProdOfferInsAttr()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    //DEFINE_QUERY (qry);
    int count,num;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"poia","product_offer_instance_attr",qry)) return;

    unsigned int i, j, k;
    unsigned int lastIndex;
    long pOfferInsID = -1;
    unsigned int * uiTemp;

    ProdOfferIns * pProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);

    /*qry.setSQL ("SELECT COUNT(1) FROM PRODUCT_OFFER_INSTANCE_ATTR");

    qry.open (); 
    qry.next ();

    count = qry.field(0).asInteger ();
    qry.close ();*/

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_PRODOFFERINSATTR, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSATTR, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsAttrData->create(CAL_COUNT(count));

    ProdOfferInsAttr * pProdOfferInsAttr = (ProdOfferInsAttr *)(*m_poProdOfferInsAttrData);
    qry.setSQL ("SELECT PRODUCT_OFFER_INSTANCE_ID, ATTR_ID, ATTR_VAL, OFFER_ATTR_INST_ID,"
                "EFF_DATE, NVL(EXP_DATE,  TO_DATE('20500101', 'yyyymmdd')) EXP_DATE "
                "FROM PRODUCT_OFFER_INSTANCE_ATTR ");

    qry.open ();
    
    while (qry.next()) {
		if (strcmp(qry.field(5).asString(),strDate)<0) {
            continue;
        }
        if (!m_poProdOfferInsIndex->get(qry.field(0).asLong(), &k)) {
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例属性:%d没有找到对应的商品实例:%ld",
                                        qry.field(1).asLong(), qry.field(0).asLong());
            continue;
        }
        i = m_poProdOfferInsAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+51);

        pProdOfferInsAttr[i].m_iAttrID = qry.field(1).asLong();
        pProdOfferInsAttr[i].m_iAttrSeq = qry.field(3).asLong();
        strcpy(pProdOfferInsAttr[i].m_sAttrValue, qry.field(2).asString());
        strcpy(pProdOfferInsAttr[i].m_sEffDate, qry.field(4).asString());
        strcpy(pProdOfferInsAttr[i].m_sExpDate, qry.field(5).asString());
        /*if(pOfferInsID != qry.field(0).asLong())//下一个商品实例
        {
            pOfferInsID = qry.field(0).asLong();
            k = 0;
            if (!m_poProdOfferInsIndex->get(pOfferInsID, &k)) {
                if(!m_bLogClose) Log::log (0, "[ERROR] 商品实例属性:%d没有找到对应的商品实例:%ld",pProdOfferInsAttr[i].m_iAttrID, pOfferInsID);
                continue;
            }
                pProdOfferIns[k].m_iOfferAttrOffset= i;
        }
        else//同一个商品实例
        {
            pProdOfferInsAttr[lastIndex].m_iNextOfferInsAttrOffset= i;
        }
        lastIndex = i;
                */
        pOfferInsID = qry.field(0).asLong();
        //  if (!m_poProdOfferInsIndex->get(pOfferInsID, &k)) {
        //      if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例属性:%d没有找到对应的商品实例:%ld",
        //      continue;
        //  }
        uiTemp = &(pProdOfferIns[k].m_iOfferAttrOffset);
        while (*uiTemp) {
            if (pProdOfferInsAttr[i].m_iAttrID<pProdOfferInsAttr[*uiTemp].m_iAttrID) {
                break;
            }
            uiTemp = &(pProdOfferInsAttr[*uiTemp].m_iNextOfferInsAttrOffset);
        }
        pProdOfferInsAttr[i].m_iNextOfferInsAttrOffset=*uiTemp;
        *uiTemp = i;
    }

    qry.close();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}

//add by zhaoziwei
void UserInfoAdmin::loadProdOfferInsAgreement()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
    char sPwd[32];
    char sStr[32];
    Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
    if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
    TOCIQuery qry(pConn);
    int count,num;
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"poig","product_offer_inst_agreement",qry)) return;

    unsigned int i,k;
    long pOfferInsID = -1;
    
    ProdOfferIns * pProdOfferIns = (ProdOfferIns *)(*m_poProdOfferInsData);

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_PRODOFFERINSAGREEMENT, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSAGREEMENT, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsAgreementData->create(CAL_COUNT(count));

    ProdOfferInsAgreement * pProdOfferInsAgreement = (ProdOfferInsAgreement *)(*m_poProdOfferInsAgreementData);
    qry.setSQL ("SELECT AGREEMENT_INST_ID,PRODUCT_OFFER_INSTANCE_ID,"
                "EFF_DATE,NVL(EXP_DATE,TO_DATE('20500101','yyyymmdd')) EXP_DATE "
                "FROM PRODUCT_OFFER_INST_AGREEMENT");

    qry.open ();
    
    while (qry.next()) {
        pOfferInsID = qry.field(1).asLong();
        if (!m_poProdOfferInsIndex->get(pOfferInsID, &k))
        {
            if (!m_bLogClose) Log::log (0, "[ERROR] 商品实例协议期:%d没有找到对应的商品实例:%ld",qry.field(0).asLong(), pOfferInsID);
            continue;
        }
        if (strcmp(qry.field(3).asString(),strDate)<0) {
            continue;
        }
        i = m_poProdOfferInsAgreementData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+51);

        pProdOfferInsAgreement[i].m_lAgreementInstID = qry.field(0).asLong();
        pProdOfferInsAgreement[i].m_lProdOfferInsID = qry.field(1).asLong();
        strcpy(pProdOfferInsAgreement[i].m_sEffDate,qry.field(2).asString());
        strcpy(pProdOfferInsAgreement[i].m_sExpDate,qry.field(3).asString());

				//在头位置添加
				unsigned int j = pProdOfferIns[k].m_iOfferAgreementOffset;
        pProdOfferInsAgreement[i].m_iNextOfferInsAgreementOffset=j;
        pProdOfferIns[k].m_iOfferAgreementOffset=i;
    }

    qry.close();
    updateLog(iSeq,qry);
    pConn->disconnect();
    delete pConn;
}
//end

void UserInfoAdmin::updateProdOfferIns(ProdOfferIns &data, bool isinsert,bool bLog)
{
    unsigned int k;
    //如果是插入并且已经存在，或者是更新并且不存在，都是错误的
    if (m_poProdOfferInsIndex->get(data.m_lProdOfferInsID, &k)^(!isinsert)) {
        if (bLog) {
            Log::log(0, "Error occurs while updating prodofferins. check the id %d.", data.m_lProdOfferInsID);
        }
        THROW(MBC_UserInfoCtl+50);
    }
    if (isinsert) {
        k = m_poProdOfferInsData->malloc();
        if (!k) THROW(MBC_UserInfoCtl+53);
        m_poProdOfferInsIndex->add (data.m_lProdOfferInsID, k);

    }
    m_poProdOfferIns[k].m_lProdOfferInsID = data.m_lProdOfferInsID;
    m_poProdOfferIns[k].m_iOfferID = data.m_iOfferID;
    strcpy(m_poProdOfferIns[k].m_sEffDate, data.m_sEffDate);
    strcpy(m_poProdOfferIns[k].m_sExpDate, data.m_sExpDate);
}

void UserInfoAdmin::updateProdOfferInsDetail(long lOfferInsID, OfferDetailIns &data, bool isinsert,bool bLog)
{
    unsigned int k;
    ServInfo * pServInfo = (ServInfo *)(*m_poServData);
    AcctInfo * pAcctInfo = (AcctInfo *)(*m_poAcctData);
    CustInfo * pCust = (CustInfo *)(*m_poCustData);

    if (!m_poProdOfferInsIndex->get(lOfferInsID, &k)) {
        if (bLog) {
            Log::log(0, "Error occurs while updating prodofferinsdetail. check the id %ld.", lOfferInsID);
        }
        THROW(MBC_UserInfoCtl+51);
    }

    if (isinsert) {
        unsigned int j = 0,tmp_k = k;

        //试图插入已经存在的数据是错误的
        j = m_poProdOfferIns[k].m_iOfferDetailInsOffset;
        for (; j ; j = m_poOfferDetailIns[j].m_iNextDetailOffset) {
            if (m_poOfferDetailIns[j].m_lOfferDetailInsID == data.m_lOfferDetailInsID) {
                if (bLog) {
                    Log::log (0, "[ERROR] 需要插入的商品实例明细[%ld]已经存在.",data.m_lOfferDetailInsID);
                }
                THROW(MBC_UserInfoCtl+124);
            }
        }

        //关联数据检查
        Offer *tempOffer = 0;
        switch (data.m_iInsType) {
        case SERV_INSTANCE_TYPE:
            tempOffer = m_poOfferMgr->getOfferInfoByID(m_poProdOfferIns[k].m_iOfferID);
            if (!tempOffer) {//商品不存在
                m_poOfferMgr->load();
                THROW(MBC_UserInfoCtl+52);
            }

            if (!m_poServIndex->get(data.m_lInsID, &k)) {
                if (bLog) {
                    Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的产品实例:%ld",
                              data.m_lOfferDetailInsID, data.m_lInsID);
                }
                THROW(MBC_UserInfoCtl+53);
            }
            break;
        case ACCT_TYPE:
            if (!m_poAcctIndex->get(data.m_lInsID, &k)) {
                if (bLog) {
                    Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的帐户:%ld",
                              data.m_lOfferDetailInsID, data.m_lInsID);
                }
                THROW(MBC_UserInfoCtl+54);
            }
            break;
        case CUST_TYPE:
            if (!m_poCustIndex->get(data.m_lInsID, &k)) {
                if (bLog) {
                    Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的客户:%ld",
                              data.m_lOfferDetailInsID, data.m_lInsID);
                }
                THROW(MBC_UserInfoCtl+55);
            }
            break;
        }

        unsigned int * pTemp = 0;
        unsigned int iPre = 0;

        unsigned int i = m_poOfferDetailInsData->malloc();
        if (!i) THROW(MBC_UserInfoCtl+53);
        //unsigned int j;
        //m_poOfferDetailIns[i].m_iNextDetailOffset = m_poProdOfferIns[k].m_iOfferDetailInsOffset;
        // m_poProdOfferIns[k].m_iOfferDetailInsOffset = i;
        // m_poOfferDetailIns[i].m_iParentOffset = k;
        m_poOfferDetailIns[i].m_iNextDetailOffset = m_poProdOfferIns[tmp_k].m_iOfferDetailInsOffset;
        m_poProdOfferIns[tmp_k].m_iOfferDetailInsOffset = i;
        m_poOfferDetailIns[i].m_iParentOffset = tmp_k;
        m_poOfferDetailIns[i].m_iInsType = data.m_iInsType;
        m_poOfferDetailIns[i].m_lInsID = data.m_lInsID;
        m_poOfferDetailIns[i].m_lOfferDetailInsID = data.m_lOfferDetailInsID;
        m_poOfferDetailIns[i].m_iMemberID = data.m_iMemberID;
        strcpy(m_poOfferDetailIns[i].m_sEffDate, data.m_sEffDate);
        strcpy(m_poOfferDetailIns[i].m_sExpDate, data.m_sExpDate);

        //  Offer * tempOffer;
        //int iOfferType = -1;
        switch (data.m_iInsType) {
        case SERV_INSTANCE_TYPE:
            // tempOffer = m_poOfferMgr->getOfferInfoByID(m_poProdOfferIns[k].m_iOfferID);
            //    if(!tempOffer)//商品不存在
            //   {
            /*char sql[1024];
            memset(sql,0,sizeof(sql));
                 sprintf(sql,"select nvl(min(offer_type),-1) offer_type  from PRODUCT_OFFER where offer_id=%d ",
                         m_poProdOfferIns[k].m_iOfferID);
                 DEFINE_QUERY (qry);
                 qry.setSQL (sql);
                 qry.open();
                 qry.next();
                 iOfferType = qry.field(0).asInteger ();
                 qry.close();
                 if( iOfferType == -1 )
                 {
                         THROW(MBC_UserInfoCtl+52);
                 }*/
            //m_poOfferMgr->load();
            //      THROW(MBC_UserInfoCtl+52);
            //          }
            /*else
                                {
                                        if(tempOffer->m_iOfferType == OFFER_TYPE_BASE)
                                        {
                                                     iOfferType = 0;
                                        }
                                }*/
            /*         if (!m_poServIndex->get(m_poOfferDetailIns[i].m_lInsID, &k)) {
                                                       if(bLog){
                                                               Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的产品实例:%ld",
                           m_poOfferDetailIns[i].m_lOfferDetailInsID, m_poOfferDetailIns[i].m_lInsID);
                                                       }
                           THROW(MBC_UserInfoCtl+53);
                      }  */
            if (tempOffer->m_iOfferType == OFFER_TYPE_BASE)//基本商品 
            //if(iOfferType == 0)//基本商品 
            {
                j = pServInfo[k].m_iBaseOfferOffset;
                if (!j) {
                    pServInfo[k].m_iBaseOfferOffset = i;
                } else {
                    while (m_poOfferDetailIns[j].m_iNextProdOfferOffset) {
                        j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                    }
                    m_poOfferDetailIns[j].m_iNextProdOfferOffset = i;
                }
            } else {
                j = pServInfo[k].m_iOfferDetailOffset;
                if (!j) {
                    pServInfo[k].m_iOfferDetailOffset = i;
                } else {
                    pTemp =  &(pServInfo[k].m_iOfferDetailOffset);
                    /*while(m_poOfferDetailIns[j].m_iNextProdOfferOffset)
                    {
                        j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                    }
                    m_poOfferDetailIns[j].m_iNextProdOfferOffset = i;*/
                }
            }
            break;

        case ACCT_TYPE:
            /*         if (!m_poAcctIndex->get(m_poOfferDetailIns[i].m_lInsID, &k)) 
                     {
                                                     if(bLog){
                                                             Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的帐户:%ld",
                         m_poOfferDetailIns[i].m_lOfferDetailInsID, m_poOfferDetailIns[i].m_lInsID);
                                                     }
                         THROW(MBC_UserInfoCtl+54);
                     } */
            j = pAcctInfo[k].m_iOfferDetailOffset;
            if (!j) {
                pAcctInfo[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pAcctInfo[k].m_iOfferDetailOffset);
                /*while(m_poOfferDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                }
                m_poOfferDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case CUST_TYPE:
            /*         if (!m_poCustIndex->get(m_poOfferDetailIns[i].m_lInsID, &k)) 
                     {
                                                     if(bLog){
                                                             Log::log (0, "[ERROR] 商品实例明细:%ld没有找到对应的客户:%ld",m_poOfferDetailIns[i].m_lOfferDetailInsID, m_poOfferDetailIns[i].m_lInsID);
                                                     }
                         THROW(MBC_UserInfoCtl+55);
                     } */
            j = pCust[k].m_iOfferDetailOffset;
            if (!j) {
                pCust[k].m_iOfferDetailOffset = i;
            } else {
                pTemp =  &(pCust[k].m_iOfferDetailOffset);
                /*while(m_poOfferDetailIns[j].m_iNextProdOfferOffset)
                {
                    j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                }
                m_poOfferDetailIns[j].m_iNextProdOfferOffset = i;*/
            }
            break;

        case OFFER_INSTANCE_TYPE:
            if (!m_poOfferDetailInsOfferInsIndex->get(m_poOfferDetailIns[i].m_lInsID, &k)) {
                m_poOfferDetailInsOfferInsIndex->add(m_poOfferDetailIns[i].m_lInsID, i);
            } else {
                if (strcmp(m_poOfferDetailIns[i].m_sEffDate,m_poOfferDetailIns[k].m_sEffDate)<0) {
                    m_poOfferDetailIns[i].m_iNextProdOfferOffset = k;
                    m_poOfferDetailInsOfferInsIndex->add(m_poOfferDetailIns[i].m_lInsID, i);
                    pTemp = 0;
                } else {
                    pTemp =  &(m_poOfferDetailIns[k].m_iNextProdOfferOffset);
                    if (*pTemp == 0) {
                        iPre = k;
                    }
                }
                /*while(m_poOfferDetailIns[k].m_iNextProdOfferOffset)
                {
                    k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
                }
                m_poOfferDetailIns[k].m_iNextProdOfferOffset = i;*/
            }
            break;
        }
        if (pTemp) {
            while (*pTemp) {
                if (strcmp(m_poOfferDetailIns[i].m_sEffDate,m_poOfferDetailIns[*pTemp].m_sEffDate)<0) {
                    m_poOfferDetailIns[i].m_iNextProdOfferOffset = *pTemp;
                    *pTemp = i;
                    break;
                }
                iPre = *pTemp;
                pTemp = &(m_poOfferDetailIns[*pTemp].m_iNextProdOfferOffset);
            }
            if (*pTemp == 0) {
                m_poOfferDetailIns[iPre].m_iNextProdOfferOffset = i;
            }
        }
    } else {
        unsigned int j = m_poProdOfferIns[k].m_iOfferDetailInsOffset;
        bool isFound = false;
        for (; j ; j=m_poOfferDetailIns[j].m_iNextDetailOffset) {
            if (m_poOfferDetailIns[j].m_lOfferDetailInsID == data.m_lOfferDetailInsID) {
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            if (bLog) {
                Log::log (0, "[ERROR] 需要更新的商品实例明细[%ld]不存在。",data.m_lOfferDetailInsID);
            }
            THROW(MBC_UserInfoCtl+56);
        }

        strcpy(m_poOfferDetailIns[j].m_sExpDate, data.m_sExpDate);
    }


}

void UserInfoAdmin::updateProdOfferInsAttr(long lOfferInsID, ProdOfferInsAttr &data, bool isinsert,bool bLog)
{
    unsigned int k;
    unsigned int j, p = 0;


    if (!m_poProdOfferInsIndex->get(lOfferInsID, &k)) {
        if (bLog) {
            Log::log(0, "商品实例：[%ld]不存在", lOfferInsID);
        }
        THROW(MBC_UserInfoCtl+57);
    }

    j = m_poProdOfferIns[k].m_iOfferAttrOffset;
    while (j&&m_poProdOfferInsAttr[j].m_iAttrID < data.m_iAttrID) {
        p = j;
        j = m_poProdOfferInsAttr[j].m_iNextOfferInsAttrOffset;
    }
    if (isinsert) {
        unsigned int i = m_poProdOfferInsAttrData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+51);
        m_poProdOfferInsAttr[i].m_iAttrID = data.m_iAttrID;
        m_poProdOfferInsAttr[i].m_iAttrSeq = data.m_iAttrSeq;
        strcpy(m_poProdOfferInsAttr[i].m_sAttrValue, data.m_sAttrValue);
        strcpy(m_poProdOfferInsAttr[i].m_sEffDate, data.m_sEffDate);
        strcpy(m_poProdOfferInsAttr[i].m_sExpDate, data.m_sExpDate);
        if (!p) {
            m_poProdOfferIns[k].m_iOfferAttrOffset = i ;
        } else {
            m_poProdOfferInsAttr[p].m_iNextOfferInsAttrOffset = i;
        }
        m_poProdOfferInsAttr[i].m_iNextOfferInsAttrOffset = j;
    } else {//update
        unsigned int m = 0;
        while (j&&m_poProdOfferInsAttr[j].m_iAttrID == data.m_iAttrID) {
            if (m_poProdOfferInsAttr[j].m_iAttrSeq == data.m_iAttrSeq) {
                m = j;//found it...
                break;
            }
            j = m_poProdOfferInsAttr[j].m_iNextOfferInsAttrOffset;
        }
        if (m) {
            strcpy(m_poProdOfferInsAttr[m].m_sAttrValue, data.m_sAttrValue);                
            strcpy(m_poProdOfferInsAttr[m].m_sExpDate, data.m_sExpDate);
        } else {
            if (bLog) {
                Log::log(0, "[ERROR]: id: %ld don't exist when updating prodofferinsattr", data.m_iAttrID);
            }
            THROW(MBC_UserInfoCtl+58);
        }
    }
}

//add by zhaoziwei
void UserInfoAdmin::updateProdOfferInsAgreement(long lOfferInsID, ProdOfferInsAgreement &data, bool isinsert,bool bLog)
{
		unsigned int k;
		unsigned int j=0;
	  if (!m_poProdOfferInsIndex->get(lOfferInsID, &k))
	  {
        if (bLog)
        {
            Log::log(0, "商品实例：[%ld]不存在", lOfferInsID);
        }
        THROW(MBC_UserInfoCtl+57);
    }
    j = m_poProdOfferIns[k].m_iOfferAgreementOffset;
    if(isinsert)
    {
        unsigned int i = m_poProdOfferInsAgreementData->malloc ();
        if (!i)
        	THROW(MBC_UserInfoCtl+51);
        m_poProdOfferInsAgreement[i].m_lAgreementInstID = data.m_lAgreementInstID;
        m_poProdOfferInsAgreement[i].m_lProdOfferInsID = data.m_lProdOfferInsID;
        strcpy(m_poProdOfferInsAgreement[i].m_sEffDate, data.m_sEffDate);
        strcpy(m_poProdOfferInsAgreement[i].m_sExpDate, data.m_sExpDate);

				//在头位置添加
        m_poProdOfferInsAgreement[i].m_iNextOfferInsAgreementOffset=j;
        m_poProdOfferIns[k].m_iOfferAgreementOffset=i;
    }
    else
    {
			for (; j; j=m_poProdOfferInsAgreement[j].m_iNextOfferInsAgreementOffset)
    	{
				if(m_poProdOfferInsAgreement[j].m_lAgreementInstID==data.m_lAgreementInstID)
				{
      	  strcpy(m_poProdOfferInsAgreement[j].m_sEffDate, data.m_sEffDate);
      	  strcpy(m_poProdOfferInsAgreement[j].m_sExpDate, data.m_sExpDate);
      	  break;
				}
			}
			if(!j)
			{
        if (bLog)
        {
					Log::log(0, "[ERROR]: id: %ld don't exist when updating prodofferinsagreement", data.m_lAgreementInstID);
        }
        THROW(MBC_UserInfoCtl+58);
      }
    }
}
//end

void UserInfoAdmin::updateServInfoState(long lServID, char * sState, char * sDate,bool bLog)
{
    unsigned int i, k;
    ServInfo * p = (ServInfo *)(*m_poServData);

    if (!(m_poServIndex->get(lServID, &i))) THROW(MBC_UserInfoCtl+26);

    strcpy (p[i].m_sState, sState);
    strcpy (p[i].m_sStateDate, sDate);
    strcpy (p[i].m_sRentDate, sDate);
    strcpy (p[i].m_sNscDate, sDate);

}

#ifdef GROUP_CONTROL
void UserInfoAdmin::loadGroupInfo()
{
    TOCIDatabase * pConn = new TOCIDatabase();
    char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);
	
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"gi","a_group_info",qry)) return;

    int count,num;
    char sql[2048]; 

    GroupInfo * p;
    unsigned int i, k;

    unsigned int * pTemp;

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_GROUPINFO, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_GROUPINFO, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poGroupInfoData->create (CAL_COUNT(count));
    p = (GroupInfo *)(*m_poGroupInfoData);

    m_poGroupInfoIndex->create (CAL_COUNT(count));

    strcpy (sql, "select group_id,group_name,group_desc,nvl(condition_id, 0),"
            "nvl(strategy_id,-1),eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date,nvl(priority,0), "
            "nvl(org_id,0) "
            "from a_group_info");

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(6).asString(),strDate)<0) {
            continue;
        }
        i = m_poGroupInfoData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+104);

        p[i].m_iGroupID = qry.field(0).asInteger();
        strcpy (p[i].m_sGroupName,qry.field(1).asString());
        strcpy (p[i].m_sGroupDesc,qry.field(2).asString());
        p[i].m_iConditionID = qry.field(3).asInteger();
        p[i].m_iStrategyID = qry.field(4).asInteger();
        strcpy (p[i].m_sEffDate,qry.field(5).asString());
        strcpy (p[i].m_sExpDate,qry.field(6).asString());
        p[i].m_iPriority = qry.field(7).asInteger();
        p[i].m_iOrgID = qry.field(8).asInteger();
        p[i].m_iNextOffset = 0;

        if (!m_poGroupInfoIndex->get (p[i].m_iGroupID, &k)) {
            m_poGroupInfoIndex->add(p[i].m_iGroupID,i);
        } else {
            if (p[i].m_iPriority>p[k].m_iPriority ||
                (p[i].m_iPriority == p[k].m_iPriority &&
                 strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0)) {
                p[i].m_iNextOffset = k;
                m_poGroupInfoIndex->add(p[i].m_iGroupID,i);
            } else {
                pTemp = &(p[k].m_iNextOffset);
                while (*pTemp) {
                    if (p[i].m_iPriority>=p[*pTemp].m_iPriority) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                while (*pTemp && p[i].m_iPriority == p[*pTemp].m_iPriority) {
                    if (strcmp(p[i].m_sEffDate,p[*pTemp].m_sEffDate)>=0) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                p[i].m_iNextOffset = *pTemp;
                *pTemp = i;
            }
        }
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
		delete pConn;
}

void UserInfoAdmin::loadGroupMember()
{
    TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);
		
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"gm","a_group_member",qry)) return;

    int count,num;
    char sql[2048];


    GroupMember * p;
    unsigned int i, k;

    unsigned int * pTemp;

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_GROUPMEMBER, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_GROUPMEMBER, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poGroupMemberData->create (CAL_COUNT(count));
    p = (GroupMember *)(*m_poGroupMemberData);

    m_poGroupMemberIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);

    strcpy (sql, "select member_id,acc_nbr,group_id,eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
            "from a_group_member");

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        if (!m_poGroupInfoIndex->get(qry.field(2).asInteger(),&k)) {
            if (!m_bLogClose) Log::log(0, "ERROR: MEMBER_ID=%d 归属的GROUP_ID不存在", qry.field(0).asInteger());
            continue;
        }
        i = m_poGroupMemberData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+105);

        p[i].m_lMemberID = qry.field(0).asLong();
        strcpy (p[i].m_sAccNbr,qry.field(1).asString());
        p[i].m_iGroupID = qry.field(2).asInteger();
        strcpy (p[i].m_sEffDate,qry.field(3).asString());
        strcpy (p[i].m_sExpDate,qry.field(4).asString());
        p[i].m_iNextOffset = 0;

        if (!m_poGroupMemberIndex->get (p[i].m_sAccNbr, &k)) {
            m_poGroupMemberIndex->add(p[i].m_sAccNbr,i);
        } else {
            if (strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0) {
                p[i].m_iNextOffset = k;
                m_poGroupMemberIndex->add(p[i].m_sAccNbr,i);
            } else {
                pTemp = &(p[k].m_iNextOffset);
                while (*pTemp) {
                    if (strcmp(p[i].m_sEffDate,p[*pTemp].m_sEffDate)>0) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                p[i].m_iNextOffset = *pTemp;
                *pTemp = i;
            }
        }
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
		delete pConn;
}

void UserInfoAdmin::updateGroupMember(GroupMember &data,bool isInsert,bool bLog)
{
    unsigned int i,k;
    unsigned int * pTemp;

    GroupMember * p = (GroupMember *)(*m_poGroupMemberData);
    if (!isInsert && !m_poGroupMemberIndex->get(data.m_sAccNbr,&k)) {
        THROW(MBC_UserInfoCtl+107);
    }
    if (!isInsert) {
        while (k) {
            if (p[k].m_lMemberID == data.m_lMemberID) {
                strcpy(p[k].m_sExpDate,data.m_sExpDate);
                return;
            }
            k = p[k].m_iNextOffset;
        }
        THROW(MBC_UserInfoCtl+108);
    }
    i = m_poGroupMemberData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+105);

    p[i].m_lMemberID = data.m_lMemberID;
    strcpy (p[i].m_sAccNbr,data.m_sAccNbr);
    p[i].m_iGroupID = data.m_iGroupID;
    strcpy (p[i].m_sEffDate,data.m_sEffDate);
    strcpy (p[i].m_sExpDate,data.m_sExpDate);
    p[i].m_iNextOffset = 0;

    if (!m_poGroupMemberIndex->get (p[i].m_sAccNbr, &k)) {
        m_poGroupMemberIndex->add(p[i].m_sAccNbr,i);
    } else {
        if (strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0) {
            p[i].m_iNextOffset = k;
            m_poGroupMemberIndex->add(p[i].m_sAccNbr,i);
        } else {
            pTemp = &(p[k].m_iNextOffset);
            while (*pTemp) {
                if (strcmp(p[i].m_sEffDate,p[*pTemp].m_sEffDate)>0) {
                    break;
                }
                pTemp = &(p[*pTemp].m_iNextOffset);
            }
            p[i].m_iNextOffset = *pTemp;
            *pTemp = i;
        }
    }
}

void UserInfoAdmin::loadNumberGroup()
{
    TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);
		
    int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"ng","a_number_group_relation",qry)) return;

    int count,num;
    char sql[2048];


    NumberGroup * p;
    unsigned int i, k;

    unsigned int * pTemp;

    char countbuf[32];
    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_NUMBERGROUP, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf,'\0',sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_NUMBERGROUP, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poNumberGroupData->create(CAL_COUNT(count));
    p = (NumberGroup *)(*m_poNumberGroupData);

    m_poNumberGroupIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);

    strcpy (sql, "select relation_id,acc_nbr,group_id,eff_date,"
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date,offer_inst_id "
            " from a_number_group_relation");

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(4).asString(),strDate)<0) {
            continue;
        }
        if (!m_poGroupInfoIndex->get(qry.field(2).asInteger(),&k)) {
            if (!m_bLogClose) Log::log(0, "ERROR: RELATION_ID=%d 订购的GROUP_ID不存在", qry.field(0).asInteger());
            continue;
        }
        i = m_poNumberGroupData->malloc ();
        if (!i) THROW(MBC_UserInfoCtl+106);

        p[i].m_lRelationID = qry.field(0).asLong();
        strcpy (p[i].m_sAccNbr,qry.field(1).asString());
        p[i].m_iGroupID = qry.field(2).asInteger();
        strcpy (p[i].m_sEffDate,qry.field(3).asString());
        strcpy (p[i].m_sExpDate,qry.field(4).asString());
        p[i].m_lOfferInstID = qry.field(5).asLong();
        p[i].m_iNextOffset = 0;

        if (!m_poNumberGroupIndex->get (p[i].m_sAccNbr, &k)) {
            m_poNumberGroupIndex->add(p[i].m_sAccNbr,i);
        } else {
            if (strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0) {
                p[i].m_iNextOffset = k;
                m_poNumberGroupIndex->add(p[i].m_sAccNbr,i);
            } else {
                pTemp = &(p[k].m_iNextOffset);
                while (*pTemp) {
                    if (strcmp(p[i].m_sEffDate,p[*pTemp].m_sEffDate)>0) {
                        break;
                    }
                    pTemp = &(p[*pTemp].m_iNextOffset);
                }
                p[i].m_iNextOffset = *pTemp;
                *pTemp = i;
            }
        }
    }
    qry.close ();
    updateLog(iSeq,qry);
    pConn->disconnect();
		delete pConn;
}

void UserInfoAdmin::updateNumberGroup(NumberGroup &data,bool isInsert,bool bLog)
{
    unsigned int i,k;
    unsigned int * pTemp;

    NumberGroup * p = (NumberGroup *)(*m_poNumberGroupData);
    if (!isInsert && !m_poNumberGroupIndex->get(data.m_sAccNbr,&k)) {
        THROW(MBC_UserInfoCtl+109);
    }
    if (!isInsert) {
        while (k) {
            if (p[k].m_lRelationID == data.m_lRelationID) {
                strcpy(p[k].m_sExpDate,data.m_sExpDate);
                return;
            }
            k = p[k].m_iNextOffset;
        }
        THROW(MBC_UserInfoCtl+110);
    }
    i = m_poNumberGroupData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+105);

    p[i].m_lRelationID = data.m_lRelationID;
    strcpy (p[i].m_sAccNbr,data.m_sAccNbr);
    p[i].m_iGroupID = data.m_iGroupID;
    strcpy (p[i].m_sEffDate,data.m_sEffDate);
    strcpy (p[i].m_sExpDate,data.m_sExpDate);
    p[i].m_lOfferInstID = data.m_lOfferInstID;
    p[i].m_iNextOffset = 0;

    if (!m_poNumberGroupIndex->get (p[i].m_sAccNbr, &k)) {
        m_poNumberGroupIndex->add(p[i].m_sAccNbr,i);
    } else {
        if (strcmp(p[i].m_sEffDate,p[k].m_sEffDate)>=0) {
            p[i].m_iNextOffset = k;
            m_poNumberGroupIndex->add(p[i].m_sAccNbr,i);
        } else {
            pTemp = &(p[k].m_iNextOffset);
            while (*pTemp) {
                if (strcmp(p[i].m_sEffDate,p[*pTemp].m_sEffDate)>0) {
                    break;
                }
                pTemp = &(p[*pTemp].m_iNextOffset);
            }
            p[i].m_iNextOffset = *pTemp;
            *pTemp = i;
        }
    }
}
#endif

#ifdef IMSI_MDN_RELATION
void UserInfoAdmin::loadImsiMdnRelation()
{
	TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);    
	int iSeq = getSeq(qry);
    if (iSeq<0) return;
    if (!insertLog(iSeq,"ImsiMdnRelation","a_imsi_mdn_relation",qry)) return;

    int count,num;
    char sql[2048]; 

    ImsiMdnRelation * p;
    unsigned int i, j, k;

    unsigned int * pTemp;

    char countbuf[32];
    memset(countbuf, 0 ,sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_IMSI_MDN, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }

	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0) {
        getBillingCycleOffsetTime(num,strDate,qry);
    }

    memset(countbuf, 0,sizeof(countbuf));
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_IMSI_MDN, countbuf,qry)) {
        THROW(MBC_UserInfoCtl+120);
    }
    count = atoi(countbuf);

    m_poImsiMdnRelationData->create (CAL_COUNT(count));
    p = (ImsiMdnRelation *)(*m_poImsiMdnRelationData);
    m_poImsiMdnRelationIndex->create (CAL_COUNT(count), MAX_IMSI_LEN);


    strcpy (sql, "select relation_id, imsi, acc_nbr, area_code, eff_date, "
            "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date ,nvl(type,-1) "
            "from a_imsi_mdn_relation" );

    qry.setSQL (sql); qry.open ();
    
    while (qry.next()) {
        if (strcmp(qry.field(5).asString(),strDate)<0) {
            continue;
        }
        i = m_poImsiMdnRelationData->malloc();
        if (!i) THROW(MBC_UserInfoCtl+121);

        p[i].m_lRelationID = qry.field(0).asLong();
        strcpy(p[i].m_sImsi, qry.field(1).asString());
        strcpy(p[i].m_sAccNbr, qry.field(2).asString());
        strcpy(p[i].m_sAreaCode, qry.field(3).asString());
        strcpy(p[i].m_sEffDate, qry.field(4).asString());
        strcpy(p[i].m_sExpDate, qry.field(5).asString());
		p[i].m_itype = qry.field(6).asInteger();

        p[i].m_iNextOffset = 0;

        if (!m_poImsiMdnRelationIndex->get(p[i].m_sImsi, &k)) {
            m_poImsiMdnRelationIndex->add(p[i].m_sImsi, i);
        } else {
            j = k;
            //sort by exp_date in descending order.
            while (j && strcmp(p[i].m_sExpDate, p[j].m_sExpDate)<0) {
                k = j;
                j=p[j].m_iNextOffset;
            }
            if (j!=k) {
                p[k].m_iNextOffset = i;
                p[i].m_iNextOffset = j;
            } else {
                //swap p[i] and p[k]
                p[i].m_iNextOffset = p[k].m_iNextOffset;

                p[i].m_lRelationID = p[k].m_lRelationID;
                strcpy(p[i].m_sAccNbr, p[k].m_sAccNbr);
                strcpy(p[i].m_sAreaCode, p[k].m_sAreaCode);
                strcpy(p[i].m_sEffDate, p[k].m_sEffDate);
                strcpy(p[i].m_sExpDate, p[k].m_sExpDate);
				p[i].m_itype = p[k].m_itype;

                p[k].m_lRelationID = qry.field(0).asLong();
                strcpy(p[k].m_sAccNbr, qry.field(2).asString());
                strcpy(p[k].m_sAreaCode, qry.field(3).asString());
                strcpy(p[k].m_sEffDate, qry.field(4).asString());
                strcpy(p[k].m_sExpDate, qry.field(5).asString());
				p[k].m_itype = qry.field(6).asInteger();

                p[k].m_iNextOffset = i;

            }
        }
    }
    qry.close();
    updateLog(iSeq,qry);
		pConn->disconnect();
    delete pConn;

}

void UserInfoAdmin::updateImsiMdnRelation(ImsiMdnRelation &data, bool isInsert, bool bLog)
{
    unsigned int i, j, k, head, prev;
    unsigned int * pTemp;

    ImsiMdnRelation * p = (ImsiMdnRelation *)(*m_poImsiMdnRelationData);
    if (!isInsert && !m_poImsiMdnRelationIndex->get(data.m_sImsi, &k)) {
        THROW(MBC_UserInfoCtl+123);
    }
    if (!isInsert) {
        i = 0;
        j = k;
        head = k;
        char sExpDate[20];
        while (k) {
            if (p[k].m_lRelationID == data.m_lRelationID) {
                i = k;
                strcpy(sExpDate, p[k].m_sExpDate);

                strcpy(p[k].m_sAccNbr, data.m_sAccNbr);
                strcpy(p[k].m_sAreaCode, data.m_sAreaCode);
                strcpy(p[k].m_sEffDate, data.m_sEffDate);
                strcpy(p[k].m_sExpDate, data.m_sExpDate);
				p[k].m_itype = data.m_itype;

                if (strcmp(sExpDate, data.m_sExpDate)==0 ) {
                    return;
                } else if (strcmp(sExpDate, data.m_sExpDate)>0) {

                    k = p[i].m_iNextOffset;
                    if (k==0) return;
                    if (strcmp(p[i].m_sExpDate, p[k].m_sExpDate)>=0) return;

                    if (head==i) {
                        //swap p[i] and p[k]
                        p[i].m_lRelationID = p[k].m_lRelationID;
                        strcpy(p[i].m_sAccNbr,   p[k].m_sAccNbr);
                        strcpy(p[i].m_sAreaCode, p[k].m_sAreaCode);
                        strcpy(p[i].m_sEffDate,  p[k].m_sEffDate);
                        strcpy(p[i].m_sExpDate,  p[k].m_sExpDate);
						p[i].m_itype = p[k].m_itype;

                        p[k].m_lRelationID = data.m_lRelationID;
                        strcpy(p[k].m_sAccNbr,   data.m_sAccNbr);
                        strcpy(p[k].m_sAreaCode, data.m_sAreaCode);
                        strcpy(p[k].m_sEffDate,  data.m_sEffDate);
                        strcpy(p[k].m_sExpDate,  data.m_sExpDate);
						p[k].m_itype = data.m_itype;

                        i = k;
                        k = p[k].m_iNextOffset;
                    }

                    if (!k) return;
                    if (strcmp(p[i].m_sExpDate, p[k].m_sExpDate)>=0) return;

                    while (k && strcmp(p[i].m_sExpDate, p[k].m_sExpDate)<0) {
                        prev = k;
                        k = p[k].m_iNextOffset;
                    }
                    p[j].m_iNextOffset = p[i].m_iNextOffset;
                    p[prev].m_iNextOffset = i;
                    p[i].m_iNextOffset = k;
                    return;
                } else {
                    if (head==i) return;
                    if (strcmp(p[j].m_sExpDate, p[i].m_sExpDate)>=0) return;

                    k = head; 
                    if (strcmp(p[i].m_sExpDate, p[k].m_sExpDate)>=0) {
                        //swap p[i] and p[k]
                        p[j].m_iNextOffset = p[i].m_iNextOffset;
                        p[i].m_iNextOffset = p[k].m_iNextOffset; 
                        p[k].m_iNextOffset = i;

                        p[i].m_lRelationID = p[k].m_lRelationID;
                        strcpy(p[i].m_sAccNbr,   p[k].m_sAccNbr);
                        strcpy(p[i].m_sAreaCode, p[k].m_sAreaCode);
                        strcpy(p[i].m_sEffDate,  p[k].m_sEffDate);
                        strcpy(p[i].m_sExpDate,  p[k].m_sExpDate);
						p[i].m_itype = p[k].m_itype;

                        p[k].m_lRelationID = data.m_lRelationID;
                        strcpy(p[k].m_sAccNbr,   data.m_sAccNbr);
                        strcpy(p[k].m_sAreaCode, data.m_sAreaCode);
                        strcpy(p[k].m_sEffDate,  data.m_sEffDate);
                        strcpy(p[k].m_sExpDate,  data.m_sExpDate);
						p[k].m_itype = data.m_itype;


                        return;
                    }

                    while (k && strcmp(p[i].m_sExpDate, p[k].m_sExpDate)<0) {
                        prev = k;
                        k = p[k].m_iNextOffset;
                    }
                    p[j].m_iNextOffset = p[i].m_iNextOffset;
                    p[prev].m_iNextOffset = i;  
                    p[i].m_iNextOffset = k; 
                    return; 
                }
            }

            j = k;
            k = p[k].m_iNextOffset;
        }

        THROW(MBC_UserInfoCtl+122);
    }
    i = m_poImsiMdnRelationData->malloc ();
    if (!i) THROW(MBC_UserInfoCtl+121);

    p[i].m_lRelationID = data.m_lRelationID;
    strcpy (p[i].m_sImsi, data.m_sImsi);
    strcpy (p[i].m_sAccNbr,data.m_sAccNbr);
    strcpy (p[i].m_sAreaCode, data.m_sAreaCode);
    strcpy (p[i].m_sEffDate,data.m_sEffDate);
    strcpy (p[i].m_sExpDate,data.m_sExpDate);
	p[i].m_itype = data.m_itype;
    p[i].m_iNextOffset = 0;

    if (!m_poImsiMdnRelationIndex->get(p[i].m_sImsi, &k)) {
        m_poImsiMdnRelationIndex->add(p[i].m_sImsi, i);
    } else {
        j = k;
        //sort by exp_date in descending order.
        while (j && strcmp(p[i].m_sExpDate, p[j].m_sExpDate)<0) {
            k = j;
            j=p[j].m_iNextOffset;
        }
        if (j!=k) {
            p[k].m_iNextOffset = i;
            p[i].m_iNextOffset = j;
        } else {
            //swap p[i] and p[k]
            p[i].m_iNextOffset = p[k].m_iNextOffset;

            p[i].m_lRelationID = p[k].m_lRelationID;
            strcpy(p[i].m_sAccNbr, p[k].m_sAccNbr);
            strcpy(p[i].m_sAreaCode, p[k].m_sAreaCode);
            strcpy(p[i].m_sEffDate, p[k].m_sEffDate);
            strcpy(p[i].m_sExpDate, p[k].m_sExpDate);
			p[i].m_itype = p[k].m_itype;

            p[k].m_lRelationID = data.m_lRelationID;
            strcpy (p[k].m_sAccNbr,data.m_sAccNbr);
            strcpy (p[k].m_sAreaCode, data.m_sAreaCode);
            strcpy (p[k].m_sEffDate,data.m_sEffDate);
            strcpy (p[k].m_sExpDate,data.m_sExpDate);
			p[k].m_itype = data.m_itype;

            p[k].m_iNextOffset = i;
        }
    }

    return;
}
#endif  /* end if IMSI_MDN_RELATION */

int UserInfoAdmin::getSeq(TOCIQuery &qry)
{
    m_poServStateLock->P();
    int iSeq;
    char sql[64];
    memset(sql,'\0',sizeof(sql));
    try {
        strcpy (sql, "select seq_mem_load_log_id.nextval from dual");
        qry.setSQL (sql); qry.open (); qry.next ();
        iSeq = qry.field(0).asInteger ();
        qry.close ();
    } catch (TOCIException & e) {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        m_poServStateLock->V();
        return -1;
    }
    m_poServStateLock->V();
    return  iSeq;
}

bool UserInfoAdmin::insertLog(int iSeq,char const * mem_table ,char const * db_table,TOCIQuery &qry)
{
    m_poServLoadInsertLock->P();
    char sSQL[256];
    memset(sSQL,'\0',sizeof(sSQL));
    try {
        sprintf (sSQL, "insert into b_mem_load_log(log_id,mem_table_name,db_table_name,begin_date,end_date) "
                 " values (%d,'%s','%s',sysdate,null)",iSeq,mem_table,db_table);
        qry.setSQL (sSQL);
        qry.execute ();
        qry.commit ();
        qry.close ();
    } catch (TOCIException & e) {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        m_poServLoadInsertLock->V();
        return false;
    }
    m_poServLoadInsertLock->V();
    return true;
}

bool UserInfoAdmin::updateLog(int iSeq,TOCIQuery &qry)
{
    m_poServLoadUpdateLock->P();
    char sSQL[64];
    memset(sSQL,'\0',sizeof(sSQL));
    try {
        sprintf (sSQL, "update b_mem_load_log set end_date=sysdate where log_id=%d",iSeq);
        qry.setSQL (sSQL);
        qry.execute ();
        qry.commit ();
        qry.close ();
    } catch (TOCIException & e) {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        m_poServLoadUpdateLock->V();
        return false;
    }
    //Log::log(0,"本次加载的日志ID =%d ",iSeq);
    m_poServLoadUpdateLock->V();
    return true;
}

void UserInfoAdmin::createAll()
{
    unload();
    int count;
    char countbuf[32];      
    //cust
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_CUST, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poCustData->create (CAL_COUNT(count));
    m_poCustIndex->create (CAL_COUNT(count));
	#ifdef IMMEDIATE_VALID
	m_poServInstRelationData->create (CAL_COUNT(count));
	m_poServInstIndex->create (CAL_COUNT(count));
	#endif
    //product
    loadProductInfo();
    //serv
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERV, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServData->create (CAL_COUNT(count));
    m_poServIndex->create (CAL_COUNT(count));
    //a_serv_type
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ASERVTYPECHANGE, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServTypeData->create (CAL_COUNT(count));
    //serv_location
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVLOCATION, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServLocationData->create (CAL_COUNT(count));

   if( ParamDefineMgr::getParam("USERINFO_UPLOAD","INDEX_SERVLOCATION",countbuf) )
   	{
   		count = atoi(countbuf);
   		m_poServLocationIndex->create (count);   	
   		}
   	else
    m_poServLocationIndex->create (10000);
    //serv_sate_attr
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVSTATEATTR, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServStateData->create (CAL_COUNT(count));
    //serv_attr
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVATTR, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServAttrData->create (CAL_COUNT(count));
    //serv_ident
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVIDENT, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServIdentData->create (CAL_COUNT(count));
    m_poServIdentIndex->create (CAL_COUNT(count), MAX_CALLING_NBR_LEN);
    //serv_product
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVPRODUCT, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServProductData->create (CAL_COUNT(count));
    m_poServProductIndex->create (CAL_COUNT(count));
    //serv_product_attr
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVPRODUCTATTR, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServProductAttrData->create (CAL_COUNT(count));
    //serv_acct
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT,
                                  USERINFO_NUM_SERVACCT, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poServAcctData->create (CAL_COUNT(count));

    m_poServAcctIndex->create (CAL_COUNT(count));
    //acct
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_ACCT, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poAcctData->create (CAL_COUNT(count));
    m_poAcctIndex->create (CAL_COUNT(count));
    //product_offer_instance
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINS, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsData->create(CAL_COUNT(count));
    m_poProdOfferInsIndex->create(CAL_COUNT(count));
    //product_offer_instance_detail
    char * pTempbuf = 0;
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSDETAIL, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(countbuf, &pTempbuf, 10);

    m_poOfferDetailInsData->create(CAL_COUNT(count));

    OfferDetailIns * pDetailIns = (OfferDetailIns*)(*m_poOfferDetailInsData);
    //创建索引
    /*qry.setSQL ("SELECT COUNT(1) FROM PRODUCT_OFFER_INSTANCE_DETAIL WHERE INSTANCE_TYPE = '10C'");

    qry.open(); 
    qry.next();
    
    count = qry.field(0).asInteger ();
    qry.close();*/
    if (!pTempbuf || *pTempbuf != '_') {
        THROW(MBC_UserInfoCtl+100);
    }
    count = strtol(pTempbuf+1, 0, 10);

    m_poOfferDetailInsOfferInsIndex->create(CAL_COUNT(count));
    //product_offer_instance_attr
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSATTR, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsAttrData->create(CAL_COUNT(count));
		
    //add by zhaoziwei
    //product_offer_inst_agreement
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_PRODOFFERINSAGREEMENT, countbuf))
    {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);

    m_poProdOfferInsAgreementData->create(CAL_COUNT(count));
    //end

#ifdef GROUP_CONTROL
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_GROUPINFO, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);
    m_poGroupInfoData->create (CAL_COUNT(count));
    m_poGroupInfoIndex->create (CAL_COUNT(count));

    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_GROUPMEMBER, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);
    m_poGroupMemberData->create (CAL_COUNT(count));
    m_poGroupMemberIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);

    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_NUMBERGROUP, countbuf)) {
        THROW(MBC_UserInfoCtl+100);
    }
    count = atoi(countbuf);
    m_poNumberGroupData->create (CAL_COUNT(count));
    m_poNumberGroupIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);
#endif

#ifdef IMSI_MDN_RELATION
    if (!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_IMSI_MDN, countbuf)) {
        THROW(MBC_UserInfoCtl + 120);
    }
    count = atoi(countbuf);
    m_poImsiMdnRelationData->create(CAL_COUNT(count));
    m_poImsiMdnRelationIndex->create(CAL_COUNT(count), MAX_IMSI_LEN);
#endif

//开发测试 add by xn 2009.10.26
	if(!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_NP, countbuf))
	{
	    THROW(MBC_NP+1);
	}
	count = atoi(countbuf);

	m_poNpData->create (CAL_COUNT(count));
	m_poNpIndex->create (CAL_COUNT(count),MAX_CALLING_NBR_LEN);

}
/*
#ifdef  USERINFO_REUSE_MODE
void  UserInfoAdmin::revokeOwe()
{		 
	tidyUserInfo(m_poOweData, "acct_item_owe",	
	m_poOweAcctIndex,(SHMData_A<int> *)0,0,offsetof(AcctItemOweData,iNextAcctOffset),
	m_poOweServIndex,(SHMData_A<int> *)0,0,offsetof(AcctItemOweData,iNextServOffset),
	0,(SHMData_A<int> *)0,0,-1,
	0,(SHMData_A<int> *)0,0,-1,
	0,(SHMData_A<int> *)0,0,-1 );
}
#endif
*/
//============================================================================
////iChkOffset为-1 表示 对应的idx是主键
#ifdef  USERINFO_REUSE_MODE
template < template<class> class dataT,class Y ,template<class> class getT1,class Z1,template<class> class getT2,class Z2,template<class> class getT3,class Z3,template<class> class getT4,class Z4,template<class> class getT5,class Z5 >
unsigned int UserInfoAdmin::tidyUserInfo(dataT<Y> * pData, const char * sParamCode,
    SHMIntHashIndex_A * pIdx1, getT1<Z1> * pGet1, int iGetOffset1, int iChkOffset1, 
    SHMIntHashIndex_A * pIdx2, getT2<Z2> * pGet2, int iGetOffset2, int iChkOffset2,
    SHMIntHashIndex_A * pIdx3, getT3<Z3> * pGet3, int iGetOffset3, int iChkOffset3,
    SHMIntHashIndex_A * pIdx4, getT4<Z4> * pGet4, int iGetOffset4, int iChkOffset4,
    SHMIntHashIndex_A * pIdx5, getT5<Z5> * pGet5, int iGetOffset5, int iChkOffset5)
{
    long key;
    unsigned int uiKeyOffset;
    char countbuf[32];    
    char sLimitDate[16];
    unsigned int iKickCnt[6];
    unsigned int iRevokeCnt=0;
    
	#ifndef TEMPLATE_LOW_SUPPORT
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(!ParamDefineMgr::getParam(USERINFO_LIMIT, sParamCode, countbuf))
        return 0;

    int iNum = atoi(countbuf);
    if(iNum<=0)
        return 0;

    memset(iKickCnt, 0, sizeof(iKickCnt) );
    int  iExpOffset = offsetof(Y, m_sExpDate);
    Y* p = (Y*)( * pData);
    
    getBillingCycleOffsetTime(iNum,sLimitDate);
    SHMIntHashIndex_A * pIdx[]={pIdx1, pIdx2, pIdx3,pIdx4,pIdx5, 0 };
    int  iChkOffset[]={iChkOffset1, iChkOffset2, iChkOffset3,iChkOffset4,iChkOffset5, -1};
    void * pGet[]={pGet1,pGet2,pGet3,pGet4,pGet5, 0};
    int  iGetOffset[]={iGetOffset1,iGetOffset2,iGetOffset3,iGetOffset4,iGetOffset5, -1};
    
    
    int iNxtOffset = -1;
    int iLoop=0;
    unsigned int uiDataOff=0;
    for( iLoop=0; iLoop<5; iLoop++){
        
        if(0== pIdx[iLoop+1] )
            break;
        SHMIntHashIndex_A::Iteration iter = pIdx[iLoop]->getIteration();
        iNxtOffset =  iChkOffset[iLoop];
        if( iNxtOffset<0 ){
            while( iter.next(key, uiKeyOffset) ){
                /////主键索引，直接更新为0
                /////如果是主键，只在index上面记录了 offset值，数据区没有next的存放
                ////主键不必支持再从另外的偏移获取offset了
                char * pExp = ( ( (char *)&p[uiKeyOffset] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    iter.removeMe( );
                    iKickCnt[iLoop]++;
                }
            
            }
            
            ////for循环继续
            continue;
        }
        
        while( iter.next(key, uiKeyOffset) ){
            unsigned int *pFirst = 0;
            if(!pGet[iLoop]){
                uiDataOff = uiKeyOffset;
            }else{
                switch(iLoop){
                case 0:
                    {
                    Z1 * pGetFrom = (Z1 *)(* pGet1);
                    
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 1:
                    {
                    Z2 * pGetFrom = (Z2 *)(* pGet2);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 2:
                    {
                    Z3 * pGetFrom = (Z3 *)(* pGet3);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 3:
                    {
                    Z4 * pGetFrom = (Z4 *)(* pGet4);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 4:
                    {
                    Z5 * pGetFrom = (Z5 *)(* pGet5);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                default:
                    uiDataOff = 0;
                    break;
                }
            }
            if(0==uiDataOff)
                continue;

            unsigned int uiFirst=uiDataOff;
            unsigned int uiDataNext = *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);

            while(uiDataNext){
                char * pExp = ( ( (char *)&p[uiDataNext] ) + iExpOffset);
                if( strncmp(pExp, sLimitDate, 8)<0 ){
                    ///过期的数据
                    *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset) =
                    *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset);
                    
                    *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset)=0;

                    iKickCnt[iLoop]++;
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);
                    continue;
                }
                
                uiDataOff = uiDataNext;
                uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataNext] )+ iNxtOffset);
                
            }
            char * pExp = ( ( (char *)&p[uiFirst] ) + iExpOffset);
            if(  strncmp(pExp, sLimitDate, 8)<0 ){
                ////第一个也要删掉，此时得更新索引
                uiDataNext=*(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset);
                if(pFirst){
                    ////更新第一个的索引，他是另外的类的成员
                    *pFirst = uiDataNext;
                }else{
                    ////是类本身的
                    if( 0 == uiDataNext){
                        iter.removeMe();
                    }else{
                        iter.setValue( uiDataNext );
                    }
                }
                *(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset) = 0;
                iKickCnt[iLoop]++;
            }

        }
        

    }
    
    
    if(iLoop>=0 && iLoop<=4 ){

        SHMIntHashIndex_A::Iteration iter = pIdx[iLoop]->getIteration();
        iNxtOffset =  iChkOffset[iLoop];

        if( iNxtOffset<0 ){
            while( iter.next(key, uiKeyOffset) ){
                /////主键索引，直接更新为0
                /////如果是主键，只在index上面记录了 offset值，数据区没有next的存放
                ////主键不必支持再从另外的偏移获取offset了
                char * pExp = ( ( (char *)&p[uiKeyOffset] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    iter.removeMe( );
                    iKickCnt[iLoop]++;
                    unsigned int iDelete = uiKeyOffset;
                    if(    ( iChkOffset1<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset1) )
                        && ( iChkOffset2<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset2) )
                        && ( iChkOffset3<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset3) )
                        && ( iChkOffset4<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset4) )
                        ){
                        ////需要revoke掉
                        pData->revoke(uiKeyOffset);
                        iRevokeCnt++;
                    }
                }
            
            }
            
            ////主键的处理结束
        }else{

            while( iter.next(key, uiKeyOffset) ){
                unsigned int *pFirst = 0;
                if(!pGet[iLoop]){
                    uiDataOff = uiKeyOffset;
                }else{
                    switch(iLoop){
                    case 0:
                        {
                        Z1 * pGetFrom = (Z1 *)(* pGet1);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 1:
                        {
                        Z2 * pGetFrom = (Z2 *)(* pGet2);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 2:
                        {
                        Z3 * pGetFrom = (Z3 *)(* pGet3);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 3:
                        {
                        Z4* pGetFrom = (Z4*)(* pGet4);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 4:
                        {
                        Z5* pGetFrom = (Z5*)(* pGet5);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    default:
                        uiDataOff = 0;
                        break;
                    }
    
                }
                if(0==uiDataOff)
                    continue;
                
                unsigned int uiFirst=uiDataOff;
                unsigned int uiDataNext = *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);
    
                while(uiDataNext){
                    char * pExp = ( ( (char *)&p[uiDataNext] ) + iExpOffset);
                    if( strncmp(pExp, sLimitDate, 8)<0 ){
                        ///过期的数据
                        unsigned int iDelete = uiDataNext;
                        *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset) =
                        *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset);
                        
                        *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset)=0;
    
    
                        iKickCnt[iLoop]++;
    
                        if(    ( iChkOffset1<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset1) )
                            && ( iChkOffset2<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset2) )
                            && ( iChkOffset3<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset3) )
                            && ( iChkOffset4<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset4) )
                            ){
                            ////需要revoke掉
                            pData->revoke(iDelete);
                            iRevokeCnt++;
                        }
                        
                        uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);                        
                        continue;
                    }
                    
                    uiDataOff = uiDataNext;
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataNext] )+ iNxtOffset);
                    
                }
                char * pExp = ( ( (char *)&p[uiFirst] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    ////第一个也要删掉，此时得更新索引
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset);
                    if(pFirst){
                        ////更新第一个的索引，他是另外的类的成员
                        *pFirst = uiDataNext;
                    }else{
                        ////是类本身的
                        if( 0 == uiDataNext){
                            iter.removeMe();
                        }else{
                            iter.setValue( uiDataNext );
                        }
                    }
                    *(unsigned int *)( ( (char *)&p[uiFirst])+ iNxtOffset)=0;
                    
                    iKickCnt[iLoop]++;
                    
                      if(
                           ( iChkOffset1<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset1) )
                        && ( iChkOffset2<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset2) )
                        && ( iChkOffset3<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset3) )
                        && ( iChkOffset4<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset4) )
                       ){
                        pData->revoke(uiFirst);
                        iRevokeCnt++;
                    }            
                }
            }
        }////非主键的处理结束

        cout<< "kick "<<sParamCode<<":" << iKickCnt[iLoop] <<endl;
        for(int iTmp=0;iTmp<=iLoop; iTmp++ ){
            if(iRevokeCnt != iKickCnt[iTmp] ){
                cout<< "memery index somewhere wrong,"
                <<"or revoke operation executed by another program at the same time."
                <<endl;
            }
        }
    }
	#endif //TEMPLATE_LOW_SUPPORT

    return iRevokeCnt;
}


//============================================================================
#endif
#ifdef  USERINFO_REUSE_MODE

////重新把整个资料扫描一遍，去掉过期的数据
void UserInfoAdmin::revokeAll()
{
    unsigned int iKickCnt = 0;

    char countbuf[32];    
    char sLimitDate[16];
    int iNum = 0;
    
#ifdef GROUP_CONTROL
////m_poAccNbrData  虽然有expdate，但是没有限制账期数，另外： m_poAccNbrIndex 是string型的
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_ACC_NBR_OFFER_TYPE, countbuf)){
        iNum = atoi(countbuf);
        if(iNum>0){
            getBillingCycleOffsetTime(iNum,sLimitDate);
            const char * skey;
            unsigned int uiKeyOffset=0;
            unsigned int uiIndex=0;
            unsigned int puiPre=0;
            iKickCnt = 0;
            SHMStringTreeIndex_A::Iteration iter = m_poAccNbrIndex->getIteration();
            AccNbrOfferType * pAccNbrOffer = (AccNbrOfferType *)(*m_poAccNbrData);    

            while(  iter.next(skey, uiKeyOffset) ){
                uiIndex = uiKeyOffset;
                puiPre = pAccNbrOffer[uiKeyOffset].m_iNextOffset;
                uiKeyOffset = pAccNbrOffer[uiKeyOffset].m_iNextOffset;
                while(uiKeyOffset){
                    if( strncmp(pAccNbrOffer[uiKeyOffset].m_sExpDate, sLimitDate, 8)<0 ){
                        unsigned int iTmp;
                        puiPre = pAccNbrOffer[uiKeyOffset].m_iNextOffset;
                        m_poAccNbrItemIndex->revokeIdx(pAccNbrOffer[uiKeyOffset].m_lItemID , iTmp);
                        m_poAccNbrData->revoke(uiKeyOffset);
                        
                        iKickCnt++;
                        
                    }
                    puiPre = pAccNbrOffer[uiKeyOffset].m_iNextOffset;
                    uiKeyOffset = pAccNbrOffer[uiKeyOffset].m_iNextOffset;
                }
                /////第一个节点检查是否需要删除
                if( strncmp(pAccNbrOffer[uiIndex].m_sExpDate, sLimitDate, 8)<0 ){
                    unsigned int iTmp=pAccNbrOffer[uiIndex].m_iNextOffset;
                    if( 0 == iTmp){
                        iter.removeMe();
                    }else{
                        iter.setValue( iTmp );
                    }
                    m_poAccNbrItemIndex->revokeIdx(pAccNbrOffer[uiIndex].m_lItemID , iTmp);
                    m_poAccNbrData->revoke(uiIndex);
                    
                    iKickCnt++;
                    
                }
            }////end while iter.next
        cout<<"kick ACC_NBR_OFFER_TYPE:"<< iKickCnt<<"."<<endl;
        }////end if iNum>0

    }
#endif

////m_poCustData 没有限制账期数
////m_poAcctData 没有限制账期数--acct_credit
////m_poCustGroupData 取消
////m_poCustTariffData 取消
////m_poCustPricingPlanData  取消
////m_poCustPricingParamData  取消
////m_poCustPricingObjectData 取消
////m_poProductData 没有限制账期数
////m_poOfferData  没有限制账期数
////m_poServData  没有限制账期数

////m_poServLocationData



tidyUserInfo(m_poServLocationData, USERINFO_NUM_SERVLOCATION ,
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iLocationOffset),offsetof(ServLocationInfo, m_iNextOffset),
    m_poServLocationIndex, (SHMData_A<int> *)0,0, offsetof(ServLocationInfo, m_iOrgNext),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );


////m_poServStateData
tidyUserInfo(m_poServStateData, USERINFO_NUM_SERVSTATEATTR ,
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iStateOffset),offsetof(ServStateInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );

////m_poServIdentData,m_poServIdentIndex ;  string类型的索引 
////
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVIDENT, countbuf)){
        iNum = atoi(countbuf);
        if(iNum>0){
            getBillingCycleOffsetTime(iNum,sLimitDate);
            const char * skey;
            long key;
            unsigned int uiKeyOffset;
            iKickCnt = 0;

            SHMStringTreeIndex_A::Iteration iter2 
                = m_poServIdentIndex->getIteration();
            ///ServIdentInfo oServIdent;
            
            while( iter2.next(skey, uiKeyOffset) ){
                unsigned int uiTemp=0;
                bool bKickSelf=false;

                ///ServIdentInfo *psii = (ServIdentInfo *)(*m_poServIdentData);
                iKickCnt+=m_poServIdentData->kick(uiKeyOffset,
                    offsetof(ServIdentInfo, m_iNextOffset),
                    offsetof(ServIdentInfo, m_sExpDate),  sLimitDate, 
                    &uiTemp, &bKickSelf);
                ////cout << skey << "," << uiKeyOffset << ",kick:"<< iKickCnt <<","<< bKickSelf << "," << uiTemp<<endl;
                if(bKickSelf ){
                    if(uiTemp){
                        iter2.setValue( uiTemp );
                    }else{
                        iter2.removeMe();

                    }
                }    

            }
            cout<< "kick "<< USERINFO_NUM_SERVIDENT <<":"<< iKickCnt<<"."<<endl;
            iKickCnt = 0;
            SHMIntHashIndex_A::Iteration iter = m_poServIndex->getIteration();
            ServInfo * pServ = (ServInfo *)(*m_poServData);    
            while(  iter.next(key, uiKeyOffset) ){
                if(0==pServ[uiKeyOffset].m_iServIdentOffset)
                    continue;
                unsigned int uiTemp=0;
                bool bKickSelf=false;
                iKickCnt+=m_poServIdentData->kickAndRevoke( pServ[uiKeyOffset].m_iServIdentOffset,
                    offsetof(ServIdentInfo, m_iServNextOffset),
                    offsetof(ServIdentInfo, m_sExpDate),  sLimitDate,
                    offsetof(ServIdentInfo, m_iNextOffset),
                    -1,
                    &pServ[uiKeyOffset].m_iServIdentOffset,
                    &bKickSelf);
                ////cout << key << "," << uiKeyOffset << ",kick:"<< iKickCnt <<","<< bKickSelf << "," << pServ[uiKeyOffset].m_iServIdentOffset<<endl;

            }
            
            cout<< "kickAndRevoke "<< USERINFO_NUM_SERVIDENT <<":"<< iKickCnt<<"."<<endl;
        }     
    }
////m_poServGroupBaseData 取消
////m_poServGroupData  取消
////m_poServProductAttrData 需要先移除ServProductAttr再移除ServProduct
tidyUserInfo(m_poServProductAttrData, USERINFO_NUM_SERVPRODUCTATTR ,
    m_poServProductIndex, m_poServProductData,offsetof(ServProductInfo, m_iAttrOffset),offsetof(ServProductAttrInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1);

//m_poServProductData : serv_product_id是主键


tidyUserInfo(m_poServProductData, USERINFO_NUM_SERVPRODUCT ,
    m_poServProductIndex, (SHMData_A<int> *)0,0, -1,
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iServProductOffset),offsetof(ServProductInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );
    
tidyUserInfo(m_poServAcctData, USERINFO_NUM_SERVACCT ,
	m_poServAcctIndex,(SHMData_A<int> *) 0,0,offsetof(ServAcctInfo, m_iAcctNextOffset),
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iServAcctOffset),offsetof(ServAcctInfo, m_iNextOffset),
  
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );

////m_poOfferInstanceData  没有限制账期数
////m_poServTypeData, 跟m_poProductIndex不存在关联链
tidyUserInfo(m_poServTypeData, USERINFO_NUM_ASERVTYPECHANGE ,
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iServTypeOffset),offsetof(ServTypeInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );

////m_poServAttrData

tidyUserInfo(m_poServAttrData, USERINFO_NUM_SERVATTR ,
    m_poServIndex, m_poServData,offsetof(ServInfo, m_iAttrOffset),offsetof(ServAttrInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );
///m_poServProductAttrData --在ServProduct之前删
////       m_poProdOfferInsData, --在detail 和 attr之后删
////m_poOfferDetailInsData  OfferDetailIns 在ProdOfferIns之前删


 tidyUserInfo(m_poOfferDetailInsData, USERINFO_NUM_PRODOFFERINSDETAIL ,    	
     m_poProdOfferInsIndex,m_poProdOfferInsData, offsetof(ProdOfferIns, m_iOfferDetailInsOffset),offsetof(OfferDetailIns, m_iNextDetailOffset),        	 
	0,(SHMData_A<int> *)0,0,-1 ,
	0,(SHMData_A<int> *)0,0,-1 ,
	0,(SHMData_A<int> *)0,0,-1 ,
	0,(SHMData_A<int> *)0,0,-1 );

////m_poProdOfferInsAttrData,  在ProdOfferIns之前删
tidyUserInfo(m_poProdOfferInsAttrData, USERINFO_NUM_PRODOFFERINSATTR ,
    m_poProdOfferInsIndex, m_poProdOfferInsData,offsetof(ProdOfferIns, m_iOfferAttrOffset) ,offsetof(ProdOfferInsAttr, m_iNextOfferInsAttrOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );


////m_poProdOfferInsData, m_poProdOfferInsIndex是主键 
tidyUserInfo(m_poProdOfferInsData, USERINFO_NUM_PRODOFFERINS ,
    m_poProdOfferInsIndex,(SHMData_A<int> *) 0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );


#ifdef GROUP_CONTROL
////m_poGroupInfoData
tidyUserInfo(m_poGroupInfoData, USERINFO_NUM_GROUPINFO ,
    m_poGroupInfoIndex,(SHMData_A<int> *)0,0,offsetof(GroupInfo, m_iNextOffset),
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1,
    0,(SHMData_A<int> *)0,0,-1    );


////m_poGroupMemberData,  m_poGroupMemberIndex是 string的索引
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_GROUPMEMBER, countbuf)){
        iNum = atoi(countbuf);
        if(iNum>0){
            getBillingCycleOffsetTime(iNum,sLimitDate);
            long key;
            unsigned int uiKeyOffset;
            iKickCnt = 0;

            SHMStringTreeIndex_A::Iteration iter = m_poGroupMemberIndex->getIteration();
            const char * pKey;
            unsigned int uiOffset;
            
            while( iter.next(pKey, uiOffset) ){
                unsigned int uiTemp=0;
                bool bKickSelf=false;
                //GroupMember *pGroupMember = (GroupMember *)(*m_poGroupMemberData);
                
                if( uiOffset ){
                    iKickCnt += m_poGroupMemberData->kickAndRevoke(
                        uiOffset,
                        offsetof(GroupMember, m_iNextOffset),
                        offsetof(GroupMember, m_sExpDate),  sLimitDate,
                        -1,
                        -1,
                        &uiTemp,
                        &bKickSelf);
                    if( bKickSelf ){
                        if(0 == uiTemp)
                            iter.removeMe();
                        else
                            iter.setValue(uiTemp);
                    }

                }
            }///end while iter.next
            cout<< "kick "<< USERINFO_NUM_GROUPMEMBER <<":"<< iKickCnt<<"."<<endl;
        }////end if iNum>0
    }////end if getParam

////m_poNumberGroupData,  m_poNumberGroupIndex 是 string的索引
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_NUMBERGROUP, countbuf)){
        iNum = atoi(countbuf);
        if(iNum>0){
            getBillingCycleOffsetTime(iNum,sLimitDate);
            long key;
            unsigned int uiKeyOffset;
            iKickCnt = 0;

            SHMStringTreeIndex_A::Iteration iter = m_poNumberGroupIndex->getIteration();
            const char * pKey;
            unsigned int uiOffset;
            
            while( iter.next(pKey, uiOffset) ){
                unsigned int uiTemp=0;
                bool bKickSelf=false;
                //NumberGroup *pNumberGroup = (NumberGroup *)(*m_poNumberGroupData);
                
                if( uiOffset ){
                    iKickCnt += m_poNumberGroupData->kickAndRevoke(
                        uiOffset,
                        offsetof(NumberGroup, m_iNextOffset),
                        offsetof(NumberGroup, m_sExpDate),  sLimitDate,
                        -1,
                        -1,
                        &uiTemp,
                        &bKickSelf);
                    if( bKickSelf ){
                        if(0 == uiTemp)
                            iter.removeMe();
                        else
                            iter.setValue(uiTemp);
                    }

                }
            }///end while iter.next
            cout<< "kick "<<USERINFO_NUM_NUMBERGROUP <<":"<< iKickCnt<<"."<<endl;
        }////end if iNum>0
    }////end if getParam



#endif


#ifdef IMSI_MDN_RELATION
////m_poImsiMdnRelationData,  m_poImsiMdnRelationIndex 是 string的索引
    memset( sLimitDate,0,sizeof(sLimitDate) );
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_IMSI_MDN, countbuf)){
        iNum = atoi(countbuf);
        if(iNum>0){
            getBillingCycleOffsetTime(iNum,sLimitDate);
            long key;
            unsigned int uiKeyOffset;
            iKickCnt = 0;

            SHMStringTreeIndex_A::Iteration iter = m_poImsiMdnRelationIndex->getIteration();
            const char * pKey;
            unsigned int uiOffset;
            
            while( iter.next(pKey, uiOffset) ){
                unsigned int uiTemp=0;
                bool bKickSelf=false;
                
                if( uiOffset ){
                    iKickCnt += m_poImsiMdnRelationData->kickAndRevoke(
                        uiOffset,
                        offsetof(ImsiMdnRelation, m_iNextOffset),
                        offsetof(ImsiMdnRelation, m_sExpDate),  sLimitDate,
                        -1,
                        -1,
                        &uiTemp,
                        &bKickSelf);
                    if( bKickSelf ){
                        if(0 == uiTemp)
                            iter.removeMe();
                        else
                            iter.setValue(uiTemp);
                    }

                }
            }///end while iter.next
            cout<< "kick "<< USERINFO_NUM_IMSI_MDN <<":"<< iKickCnt<<"."<<endl;
        }////end if iNum>0
    }////end if getParam

#endif
}

template <class dataT >
void UserInfoAdmin::printClassInfo( dataT * pClass, char const * sName)
{
    cout << sName << ":"<<endl;
    if(pClass) {
        pClass->showDetail() ;
    }else{
        cout<<"none.";
    }
    cout << endl;
}

void UserInfoAdmin::printInfo()
{
    char sText[256];
    snprintf(sText, sizeof(sText), 
        "%-10s 0x%-15s %2s %-12s %-9s %-10s %-10s %4s %-9s %-10s",
        "IpcKey","Addr","Ver","TypeName","DataSize","Total","Used","used%","FreeByDel","TotalDel");
    cout << "Name:" << endl;
    cout << sText << endl;
    
	printClassInfo(m_poCustIndex ,"CUST_INFO_INDEX");
	printClassInfo(m_poCustData, "CUST_INFO_DATA");

	printClassInfo(m_poProductIndex, "PRODUCT_INFO_INDEX");
	printClassInfo(m_poProductData, "PRODUCT_INFO_DATA");

	printClassInfo(m_poServIndex, "SERV_INFO_INDEX");
	printClassInfo(m_poServData, "SERV_INFO_DATA");
	printClassInfo(m_poServTypeData, "SERVTYPE_INFO_DATA");
	printClassInfo(m_poServAttrData, "SERVATTR_INFO_DATA");
    printClassInfo(m_poServLocationIndex, "SERVLOCATION_INFO_INDEX");
	printClassInfo(m_poServLocationData,  "SERVLOCATION_INFO_DATA");
	printClassInfo(m_poServStateData, "SERVSTATE_INFO_DATA");
	printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
	printClassInfo(m_poServProductIndex, "SERVPRODUCT_INFO_INDEX");
	printClassInfo(m_poServProductData,  "SERVPRODUCT_INFO_DATA");
	printClassInfo(m_poServProductAttrData,  "SERVPRODUCTATTR_INFO_DATA");
	printClassInfo(m_poServAcctIndex, "SERVACCT_ACCT_INDEX");
	printClassInfo(m_poServAcctData,  "SERVACCT_INFO_DATA");

    printClassInfo(m_poAcctIndex, "ACCT_INFO_INDEX");
    printClassInfo(m_poAcctData, "ACCT_INFO_DATA");
    printClassInfo(m_poProdOfferInsIndex, "PRODOFFERINS_INDEX");
    printClassInfo(m_poProdOfferInsData,"PRODOFFERINS_DATA");

    printClassInfo(m_poOfferDetailInsOfferInsIndex, "OFFERDETAILINS_INSIDOFFER_INDEX");
    printClassInfo(m_poOfferDetailInsData,"OFFERDETAILINS_DATA");
    printClassInfo(m_poProdOfferInsAttrData,"PRODOFFERINSATTR_DATA");

#ifdef GROUP_CONTROL
	printClassInfo(m_poGroupInfoIndex, "GROUP_INFO_INDEX");
	printClassInfo(m_poGroupInfoData, "GROUP_INFO_DATA");
	printClassInfo(m_poGroupMemberIndex, "GROUP_MEMBER_INDEX");
	printClassInfo(m_poGroupMemberData,  "GROUP_MEMBER_DATA");
    printClassInfo(m_poNumberGroupIndex, "NUMBER_GROUP_INDEX");
	printClassInfo(m_poNumberGroupData, "NUMBER_GROUP_DATA");
	
	printClassInfo(m_poAccNbrIndex,"ACCNBROFFERTYPE_INDEX");
	printClassInfo(m_poAccNbrItemIndex,"SHM_ACCNBROFFERTYPE_ITEM_INDEX");
	printClassInfo(m_poAccNbrData,"ACCNBROFFERTYPE_DATA");

#endif

#ifdef IMSI_MDN_RELATION
	printClassInfo(m_poImsiMdnRelationIndex, "IMSI_MDN_RELATION_INDEX");
	printClassInfo(m_poImsiMdnRelationData,  "IMSI_MDN_RELATION_DATA");
#endif

    cout<<endl;

}



void UserInfoAdmin::testLoadAndRevoke()
{

////先加载过期数据，然后删掉，反复循环，删除的数据都足够整个数据量的10倍
////最后加入正常数据然后导出

    loadServInfo();

//    for(int i=0; i<110; i++ ){
//        loadServLocationInfo_test(1);
//        printClassInfo(m_poServLocationIndex, "SERVLOCATION_INFO_INDEX");
//        printClassInfo(m_poServLocationData,  "SERVLOCATION_INFO_DATA");
//        revokeAll();
//    }
//    exportServLocationInfo_test("serv_location_ex_test1");
//    loadServLocationInfo_test(0);
//        printClassInfo(m_poServLocationIndex, "SERVLOCATION_INFO_INDEX");
//        printClassInfo(m_poServLocationData,  "SERVLOCATION_INFO_DATA");
//    exportServLocationInfo_test("serv_location_ex_test2");
//
//////先加载不过期数据，
//////然后反复加载过期数据，然后删掉，反复循环，删除的数据都足够整个数据量的10倍
//////最后加入过期数据然后导出
//    revokeAll();        
//    for(int i=0; i<110; i++ ){
//        loadServLocationInfo_test(1);
//        printClassInfo(m_poServLocationIndex, "SERVLOCATION_INFO_INDEX");
//        printClassInfo(m_poServLocationData,  "SERVLOCATION_INFO_DATA");
//        revokeAll();
//    }
//    exportServLocationInfo_test("serv_location_ex_test3");
//    loadServLocationInfo_test(1);
//        printClassInfo(m_poServLocationIndex, "SERVLOCATION_INFO_INDEX");
//        printClassInfo(m_poServLocationData,  "SERVLOCATION_INFO_DATA");
//    exportServLocationInfo_test("serv_location_ex_test4");
//    

////serv_location_ex_test1  --空表
////serv_location_ex_test2  --57条正常数据
////serv_location_ex_test3  --同2一样，57条数据
////serv_location_ex_test4  --252条全量数据

////----------------------------------------------------
    for(int i=0; i<110; i++ ){
        loadServIdentInfo_test(1);
	    printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	    printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
        revokeAll();
	    printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	    printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
    }
	printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");    
    exportServIdentInfo_test("serv_ident_ex_test1");
    loadServIdentInfo_test(0);
	printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
    exportServIdentInfo_test("serv_ident_ex_test2");
    revokeAll();        
    for(int i=0; i<110; i++ ){
        loadServIdentInfo_test(1);
	    printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	    printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
        revokeAll();
    }
    exportServIdentInfo_test("serv_ident_ex_test3");
    loadServIdentInfo_test(1);
	printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
    exportServIdentInfo_test("serv_ident_ex_test4");
    
////serv_ident_ex_test1  --空表
////serv_ident_ex_test2  --61条正常数据
////serv_ident_ex_test3  --同2一样，61条数据
////serv_ident_ex_test4  --252条全量数据,其中 250条serv_ident, 2条 a_serv_ex

}


void UserInfoAdmin::testRevoke()
{
      revokeAll();
	    printClassInfo(m_poServIdentIndex, "SERVIDENT_INFO_INDEX");
	    printClassInfo(m_poServIdentData, "SERVIDENT_INFO_DATA");
}


////iMode为0，表示正常过滤过期的数据；
////iMode为1 ，表示只加载过期的数据，这些加载的数据是用来删掉他们的。
void UserInfoAdmin::loadServLocationInfo_test( int iMode)
{
	TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);
 

	int count,num;
	char sql[2048];
	//DEFINE_QUERY (qry);

	ServLocationInfo * p;
	ServInfo * pServ = (ServInfo *)(*m_poServData);
	unsigned int i, k;

	unsigned int * pTemp;

	char countbuf[32];
	memset(countbuf,'\0',sizeof(countbuf));
	if(!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVLOCATION, countbuf,qry))
	{
	    num = 0;
	}else
	{
		num = atoi(countbuf);
	}
	memset(countbuf,'\0',sizeof(countbuf));

	if(!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVLOCATION, countbuf,qry))
	{
	    THROW(MBC_UserInfoCtl+100);
	}
	count = atoi(countbuf);

	p = (ServLocationInfo *)(*m_poServLocationData);
 

	strcpy (sql, "select serv_id, nvl(org_id, -1), "
        "eff_date,"
        "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
        "from serv_location");

	qry.setSQL (sql); qry.open ();
	
	char strDate[16];
	memset(strDate,0,sizeof(strDate));
	if(num>0)
	{
		getBillingCycleOffsetTime(num,strDate);
	}
	while (qry.next()) {
	    if(0 == iMode ){
    		if(strcmp(qry.field(3).asString(),strDate) < 0)
    		{
    			continue;
    		}
		}else{
    		if(strcmp(qry.field(3).asString(),strDate) >= 0)
    		{
    			continue;
    		}

		}
    	i = m_poServLocationData->malloc ();
    	if (!i) THROW(MBC_UserInfoCtl+7);

    	p[i].m_lServID = qry.field(0).asLong ();
    	p[i].m_iOrgID = qry.field(1).asInteger ();
    	strcpy (p[i].m_sEffDate, qry.field(2).asString ());
    	strcpy (p[i].m_sExpDate, qry.field(3).asString ());

    	if (!m_poServIndex->get (p[i].m_lServID, &k)) {
        	if(!m_bLogClose) Log::log (0, "地域属性找不到对应的用户:%ld", p[i].m_lServID);
        	continue;
        }

    	pTemp = &(pServ[k].m_iLocationOffset);
        //20060724 xueqt
        p[i].m_lServID = k;

    	while (*pTemp) {
        	if (strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate) >= 0) {
            	break;
            }

        	pTemp = &(p[*pTemp].m_iNextOffset);
        }

    	p[i].m_iNextOffset = *pTemp;
        *pTemp = i;

        if (m_poServLocationIndex->get (p[i].m_iOrgID, &k)) {
            p[i].m_iOrgNext = k;
        } else {
            p[i].m_iOrgNext = 0;
        }

        m_poServLocationIndex->add (p[i].m_iOrgID, i);
    }

	qry.close ();

	pConn->disconnect();
	delete pConn;
}

void UserInfoAdmin::exportServLocationInfo_test(char const * sTable)
{
	if(!strcmp(sTable,"serv_location"))
	{
		Log::log (0, "错误：导出表名为 %s ，不能与正式表同名，请重新指定导出表名！",sTable);
		return;
	}
	Log::log (0, "开始导出 serv_location 内存数据到数据库表 %s",sTable);
    DEFINE_QUERY (qry);
	char sSQL[2048];
	memset(sSQL,'\0',sizeof(sSQL));


    sprintf (sSQL, "insert into %s "
		"(serv_id,org_id,eff_date,exp_date)"
		" values (:serv_id, :org_id, to_date(:eff_date,'yyyymmddhh24miss'),"
		"to_date(:exp_date,'yyyymmddhh24miss'))",sTable);
    qry.setSQL (sSQL);
	
	unsigned int count = m_poServLocationData->getCount ();
	unsigned int count_serv = m_poServData->getCount ();

	ServInfo * pStart;
    ServLocationInfo * pServLocation = (ServLocationInfo *)(*m_poServLocationData);
	ServInfo * pServ = (ServInfo *)(*m_poServData);

	ServLocationInfo aServLocationInfo[20000];
	int i =0,j =0,k =0;
	unsigned int iOffset = 0;

    USERINFO_SHM_DATA_TYPE<ServLocationInfo>::Iteration iter = m_poServLocationData->getIteration();
    ///ServLocationInfo batchData[batchnum];
    unsigned int iCnt=0;
    ///pStart=aServLocationInfo;
    while( iter.nextBatch(20000, aServLocationInfo, iCnt) ){
        for(i=0;i<iCnt;i++){
            long lOffset = aServLocationInfo[i].m_lServID;
            aServLocationInfo[i].m_lServID = pServ[lOffset].m_lServID;
        }
        if(iCnt){
			qry.setParamArray ("serv_id",	&(aServLocationInfo[0].m_lServID),	sizeof (ServLocationInfo));
			qry.setParamArray ("org_id",&(aServLocationInfo[0].m_iOrgID),sizeof (ServLocationInfo));
			qry.setParamArray ("eff_date",	(char**)&(aServLocationInfo[0].m_sEffDate),	sizeof (ServLocationInfo)
				,sizeof(aServLocationInfo[0].m_sEffDate));
			qry.setParamArray ("exp_date",	(char**)&(aServLocationInfo[0].m_sExpDate),	sizeof (ServLocationInfo)
				,sizeof(aServLocationInfo[0].m_sExpDate));
    
            qry.execute (iCnt);
            qry.commit();
            count -= iCnt;
            i++;
            if (!count || !(i&1))
                Log::log (0, "剩余 %d 条", count);
        }
    }

    qry.close ();

    return;
}


////iMode为0，表示正常过滤过期的数据；
////iMode为1 ，表示只加载过期的数据，这些加载的数据是用来删掉他们的。
void UserInfoAdmin::loadServIdentInfo_test(int iMode)
{
	TOCIDatabase * pConn = new TOCIDatabase();
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	Environment::getConnectInfo("USER_INFO", sUser, sPwd, sStr);
	if(!pConn->connect(sUser, sPwd, sStr))
	{
		ALARMLOG28(0,MBC_CLASS_Fatal,12,"数据库[%s]连接失败 用户名[%s] 密码[%s],TNS[%s]","USER_INFO",sUser,sPwd,sStr);
	}
	TOCIQuery qry(pConn);


	int count,num;
	char sql[2048];

	ServIdentInfo * p;
	unsigned int i, k;
    
    int iPriority;

    ServInfo * pServ = (ServInfo *)(*m_poServData);
    ServInfo * pTemp;
    

	char sAccNbr[32];
	char * pAccNbr;
	char countbuf[32];
	memset(countbuf,'\0',sizeof(countbuf));
	if(!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVIDENT, countbuf,qry))
	{
	    num = 0;
	}else
	{
		num = atoi(countbuf);
	}
	memset(countbuf,'\0',sizeof(countbuf));

	if(!ParamDefineMgr::getParam(USERINFO_SEGMENT, USERINFO_NUM_SERVIDENT, countbuf,qry))
	{
	    THROW(MBC_UserInfoCtl+100);
	}
	count = atoi(countbuf);

	p = (ServIdentInfo *)(*m_poServIdentData);
	
	ProductInfo * ppi = (ProductInfo *)(*m_poProductData);

	sprintf (sql, "select serv_id, acc_nbr, "
        "eff_date,"
        "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, '50Z' acc_nbr_type "
        "from serv_identification "
////        "where serv_id in (5247503,5247594,5248332,5248274)"
        "union all "
        "select serv_id, acc_nbr, "
        "eff_date, "
        "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, acc_nbr_type "
        "from a_serv_ex"
        );

	qry.setSQL (sql); qry.open ();

	char strDate[16];
	memset(strDate,0,sizeof(strDate));
	if(num>0)
	{
		getBillingCycleOffsetTime(num,strDate);
	}
	while (qry.next()) {

	    if(0 == iMode ){
    		if(strcmp(qry.field(3).asString(),strDate) < 0)
    		{
    			continue;
    		}
		}else{
    		if(strcmp(qry.field(3).asString(),strDate) >= 0)
    		{
    			continue;
    		}

		}

    	i = m_poServIdentData->malloc ();
    	if (!i) THROW(MBC_UserInfoCtl+9);

    	p[i].m_lServID = qry.field(0).asLong ();

#ifdef SPECIAL_NBR_CUT
		strcpy (sAccNbr, qry.field(1).asString ());
		pAccNbr =sAccNbr+4;
		if(strncmp(sAccNbr,pAccNbr,4)==0 && strncmp(sAccNbr,"085",3)==0){
			strcpy(p[i].m_sAccNbr,pAccNbr);
		}else{
			strcpy(p[i].m_sAccNbr,sAccNbr);
		}
#else
    	strcpy (p[i].m_sAccNbr, qry.field(1).asString ());
#endif


    	char * s = p[i].m_sAccNbr;
    	while (*s) {

            if (*s>='A' && *s<='Z') *s |= 0x20;

        	s++;
        }

    	strcpy (p[i].m_sEffDate, qry.field(2).asString ());
    	strcpy (p[i].m_sExpDate, qry.field(3).asString ());

        char sType[8];
        strcpy (sType, qry.field (4).asString ());

        p[i].m_iType = (sType[2] | 0x20) - 'a';
        
    	if (!m_poServIndex->get(p[i].m_lServID, &(p[i].m_iServOffset))) {
        	Log::log (0, "号码:%s没有找到对应的用户", p[i].m_sAccNbr);
        	continue;
        }

        pTemp = pServ + p[i].m_iServOffset;

        p[i].m_iServNextOffset = pTemp->m_iServIdentOffset;
        pTemp->m_iServIdentOffset = i;
        
        if (m_poProductIndex->get (pTemp->m_iProductID, &k)) {
            p[i].m_iNetworkID = ppi[k].m_iNetworkID;
            p[i].m_iNetworkPriority = ppi[k].m_iNetworkPriority;
        }
        else
        {
            if(!m_bLogClose) Log::log (0, "号码:%s没有找到对应的网络类型", p[i].m_sAccNbr);
            p[i].m_iNetworkID = 0;
            p[i].m_iNetworkPriority = 0;
        }

    	if (!m_poServIdentIndex->get (p[i].m_sAccNbr, &k)) {
        	m_poServIdentIndex->add (p[i].m_sAccNbr, i);
        } else {
            //先按照优先级(高的在前),再按照时间来排序?
            if(p[i].m_iNetworkPriority > p[k].m_iNetworkPriority
               ||
        	(p[i].m_iNetworkPriority == p[k].m_iNetworkPriority &&
        	strcmp (p[i].m_sExpDate, p[k].m_sExpDate)>=0 &&
                	strcmp (p[i].m_sEffDate, p[k].m_sEffDate) >= 0)) {
            	p[i].m_iNextOffset = k;
            	m_poServIdentIndex->add (p[i].m_sAccNbr, i);
            } else {
            	unsigned int * pTemp = &(p[k].m_iNextOffset);
            	while (*pTemp) {
            	    //找到第一个优先级不大于的地方
                	if (p[i].m_iNetworkPriority >= p[*pTemp].m_iNetworkPriority) {
                    	break;
                    }
                	pTemp = &(p[*pTemp].m_iNextOffset);
                }
                while (*pTemp && 
                p[i].m_iNetworkPriority == p[*pTemp].m_iNetworkPriority) {
                    if (strcmp (p[i].m_sExpDate, p[*pTemp].m_sExpDate)>=0 &&
                        	strcmp (p[i].m_sEffDate, p[*pTemp].m_sEffDate)>=0) {
                    	break;
                    }
                	pTemp = &(p[*pTemp].m_iNextOffset);
                }
            	p[i].m_iNextOffset = *pTemp;
                *pTemp = i;
            }
        }

    }

	qry.close ();

	pConn->disconnect();
	delete pConn;
}


void UserInfoAdmin::exportServIdentInfo_test(char const * sTable)
{
	if(!strcmp(sTable,"serv_identification"))
	{
		Log::log (0, "错误：导出表名为 %s ，不能与正式表同名，请重新指定导出表名！",sTable);
		return;
	}
	Log::log (0, "开始导出 serv_identification 内存数据到数据库表 %s",sTable);
    DEFINE_QUERY (qry);
	char sSQL[2048];
	memset(sSQL,'\0',sizeof(sSQL));

    sprintf (sSQL, "insert into %s "
		"(serv_id,acc_nbr,acc_nbr_type,eff_date,exp_date)"
		" values (:serv_id, :acc_nbr,:acc_nbr_type, to_date(:eff_date,'yyyymmddhh24miss'),"
		"to_date(:exp_date,'yyyymmddhh24miss'))",sTable);
    qry.setSQL (sSQL);

	unsigned int count = m_poServIdentData->getCount ();
    int const batchnum = 20000;

    ServIdentInfo *pStart ;//= (ServIdentInfo *)(*m_poServIdentData) + 1;

    class ServIdentification
    {
    public:
        //主业务号码
        char m_sAccNbr[32];
        //主产品标识
        long m_lServID;
        //生效日期
        char m_sEffDate[16];
        //失效日期
        char m_sExpDate[16];
    	//号码业务类型
        char m_sType[4];
    };

	ServIdentification aServIdentification[20000];
	int i =0,j =0;


    USERINFO_SHM_DATA_TYPE<ServIdentInfo>::Iteration iter = m_poServIdentData->getIteration();
    ServIdentInfo batchData[batchnum];
    unsigned int iCnt=0;
    ////pStart=batchData;
    while( iter.nextBatch(batchnum, batchData, iCnt) ){

        for(i=0; i< iCnt; i++){
            pStart = &batchData[i];
    		aServIdentification[i].m_lServID=pStart->m_lServID;
    		strcpy(aServIdentification[i].m_sAccNbr,pStart->m_sAccNbr);
    		strcpy(aServIdentification[i].m_sEffDate,pStart->m_sEffDate);
    		strcpy(aServIdentification[i].m_sExpDate,pStart->m_sExpDate);
    		char c = (char)pStart->m_iType+'a';
    		switch(c)
    		{
    			case 'z':
    				strcpy(aServIdentification[i].m_sType,"50Z");
    				break;
    			case 'a':
    				strcpy(aServIdentification[i].m_sType,"50A");
    				break;
    			case 'b':
    				strcpy(aServIdentification[i].m_sType,"52B");
    				break;
    			case 'c':
    				strcpy(aServIdentification[i].m_sType,"52C");
    				break;
    			case 'd':
    				strcpy(aServIdentification[i].m_sType,"52D");
    				break;
    			default :
    			    strcpy(aServIdentification[i].m_sType,"52 ");
    			    aServIdentification[i].m_sType[2]=c-0x20;
    				break;
    		}
        }
        if(iCnt){
			qry.setParamArray ("serv_id",	&(aServIdentification[0].m_lServID),	sizeof (ServIdentification));
			qry.setParamArray ("acc_nbr",(char**)&(aServIdentification[0].m_sAccNbr),sizeof (ServIdentification),
				sizeof(aServIdentification[0].m_sAccNbr));
			qry.setParamArray ("acc_nbr_type",(char**)&(aServIdentification[0].m_sType),sizeof (ServIdentification)
				,sizeof(aServIdentification[0].m_sType));
			qry.setParamArray ("eff_date",(char**)(aServIdentification[0].m_sEffDate),sizeof (ServIdentification)
				,sizeof(aServIdentification[0].m_sEffDate));
			qry.setParamArray ("exp_date",	(char**)&(aServIdentification[0].m_sExpDate),	sizeof (ServIdentification)
				,sizeof(aServIdentification[0].m_sExpDate));
    
            qry.execute (iCnt);
            qry.commit();
            count -= iCnt;
            i++;
            if (!count || !(i&1))
                Log::log (0, "剩余 %d 条", count);
        }
    }


    qry.close ();
    return;
}
#endif    ////end of #ifdef--USERINFO_REUSE_MODE

//============================================================================


