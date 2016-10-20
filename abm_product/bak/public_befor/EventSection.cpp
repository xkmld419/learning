/*VER: 2*/ 
/*
**
**
**
**  2007.10.17 xueqt 
**      增加一个开关，在绑定用户资料的时候，绑定公免商品ID到OFFER_ID2
*/


#include "EventSection.h"
#include "Exception.h"
#include "Environment.h"
#include "Log.h"
#include "LocalHead.h"
#include "OfferMgr.h"

#include "MBC.h"

//##ModelId=4254E05601AB
void EventSection::reset()
{
	m_iUsedNumber = 0;
}

//##ModelId=4254E6D2022B
EventSection *EventSection::malloc()
{
	if (m_iUsedNumber == m_iTotalNumber) THROW(MBC_EventSection+1);

	EventSection * pSection = m_poList + m_iUsedNumber;
	m_iUsedNumber++;

	pSection->m_poNext = 0;

	return pSection;
}

//##ModelId=4254E84C0007
EventSection *EventSection::initMemory(int number)
{
	freeMemory ();

	m_poList = new EventSection[number];
	if (!m_poList) THROW(MBC_EventSection+2);

	m_iTotalNumber = number;

	return m_poList;
}

//##ModelId=425A154500A0
void EventSection::freeMemory()
{
	if (m_poList) {
		delete [] m_poList;
	}

	m_iTotalNumber = 0;
	m_iUsedNumber = 0;
	m_poList = 0;
}

//##ModelId=4254E80D0396
int EventSection::m_iTotalNumber = 0;

//##ModelId=4254E92A02BA
int EventSection::m_iUsedNumber = 0;

//##ModelId=42672FE303C4
EventSection *EventSection::m_poList = 0;

//##ModelId=42647B21027B
EventSection *EventSection::insert()
{
	EventSection * pSection = this->malloc ();

	pSection->m_poNext = m_poNext;
	m_poNext = pSection;

	pSection->m_iCustPlanOffset = m_iCustPlanOffset;

	return pSection;
}

//##ModelId=4272EF3A0243
void EventSection::bindUserInfo(bool bRebind)
{
	char sTemp[MAX_BILLING_AREA_CODE_LEN+MAX_BILLING_NBR_LEN];

	if (m_iServID && !bRebind) return;

    if(!G_PSERV){
        Environment::useUserInfo();
    }

	sprintf (sTemp, "%s%s", m_sBillingAreaCode, m_sBillingNBR);

	//bind user info
	if (G_PUSERINFO->getServ (G_SERV, sTemp, m_sStartDate, m_iNetworkID) ||
            G_PUSERINFO->getServ (G_SERV, m_sBillingNBR, m_sStartDate, m_iNetworkID)) {
        setUserInfo (G_PSERV);
    }
    else {
        setUserInfo (0);
    }
}


void EventSection::unbindUserInfo()
{
    m_iServID = 0;
    
    if (m_iErrorID == ERROR_NO_OWNER)
        m_iErrorID = 0;
}


void EventSection::setUserInfo (Serv *p_Serv)
{
	BillingCycle const * pCycle;
	int iCycleType;

	if (p_Serv) {
        
		m_iServID = p_Serv->getServID ();
		m_iCustID = p_Serv->getCustID ();
		m_iProdID = p_Serv->getServInfo ()->m_iProductID;

		int iOrgID = p_Serv->getOrgID();
		if(iOrgID>0)
			m_iBillingOrgID = iOrgID;
		//m_iBillingOrgID = p_Serv->getOrgID ();

        m_lOffers[0] = p_Serv->getBaseOfferID (m_sStartDate);

        int i;

		for (i=1; i<MAX_OFFERS; i++) {
			m_lOffers[i] = -1;
		}

		iCycleType = p_Serv->getBillingCycleType ();

#ifdef DEF_SICHUAN
        m_iBillingTypeID = atoi(p_Serv->getAttr (ID_BILLING_MODE));
#else
		m_iBillingTypeID = p_Serv->getServInfo ()->m_iBillingMode;
#endif

//绑定公免商品
#ifdef FREE_SERV_USE_OFFER
        
        static OfferInsQO x[MAX_OFFER_INSTANCE_NUMBER];
        static OfferMgr * pOfferMgr = 0;
        
        if (!pOfferMgr) {
            if ( !(pOfferMgr=new OfferMgr () ) ) {
                Log::log (0, "申请内存失败: new OfferMgr ()");
                THROW (MBC_EventSection+3);
            }
            
            pOfferMgr->loadFreeOffer ();
        }
         
        int iNum = p_Serv->getOfferIns (x);

        for (int j=0; j<iNum; j++) {
            if (pOfferMgr->isFreeOffer (x[j].m_poOfferIns->m_iOfferID)) {
                m_lOffers[1] = x[j].m_poOfferIns->m_iOfferID;
                break;
            }
        }

#endif
//绑定公免商品结束

	} else {

		LocalHead pLocalHead[1];
	
		if (LocalHeadMgr::searchLocalHead (m_sBillingNBR, m_sBillingAreaCode,
									 m_sStartDate, pLocalHead)) {
			m_iBillingOrgID = pLocalHead->getOrgID ();
		} else {
			if(m_iBillingOrgID<=0)
				m_iBillingOrgID = 1;
		}
		
		m_iServID = -1;
		
		//如果话单之前已经判定是错单或者其它异常话单,ErrorID应保持不变
		if (!m_iErrorID)
		    m_iErrorID = ERROR_NO_OWNER;
		
		iCycleType = DEFAULT_CYCLE_TYPE;		
	}

	if (m_iBillingOrgID == -1) m_iBillingOrgID = 1;

	//set billing cycle info

    if(!p_Serv)
        return ; 
        
	pCycle = p_Serv->getBillingCycle (m_sStartDate, iCycleType);
    
	if (pCycle) {
		m_iBillingCycleID = pCycle->getBillingCycleID ();
	} else {
		Log::log (0, "[ERROR]: 找不到帐务周期[%d, %s]", iCycleType, m_sStartDate);
		THROW (MBC_EventSection+3);
	}
}
