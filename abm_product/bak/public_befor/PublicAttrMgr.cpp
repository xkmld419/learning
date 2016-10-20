#include "PublicAttrMgr.h"
#include "Environment.h"

HashList<PublicAttrData *>  *PublicAttrMgr::m_poDataIndex = 0;
bool PublicAttrMgr::bInit=false;
PublicAttrData * PublicAttrMgr::m_poData = 0;

PublicAttrMgr::PublicAttrMgr()
{
    bInit=false;
}

PublicAttrMgr::~PublicAttrMgr()
{
	if(m_poDataIndex){
		delete m_poDataIndex;
		m_poDataIndex = NULL;
	}
	if(m_poData){
		delete [] m_poData;
		m_poData = NULL;
	}
	bInit = false;
}

bool PublicAttrMgr::queryByOfferID (int iOfferID,int &iMemberID)
{
	if(bInit==false){
		load();
		bInit=true;
	}
	PublicAttrData * pData = 0;
	if(m_poDataIndex->get(iOfferID,&pData)){
		while(pData != NULL){
			if (pData->m_iAttrID == G_CURATTRID){
				iMemberID = pData->m_iMemberID;
				return true;
			}else{
				pData = pData->m_poNext;
			}
		}
	}
	return false;
}

bool PublicAttrMgr::queryByOfferIDAndMember (int iOfferID,int iInMemberID,int &iOutMemberID)
{
	if(bInit==false){
		load();
		bInit=true;
	}
	PublicAttrData * pData = 0;
	if(m_poDataIndex->get(iOfferID,&pData)){
		while(pData != NULL){
			if (pData->m_iAttrID == G_CURATTRID){
				if (pData->m_iMemberID == iInMemberID || pData->m_iMemberID == 0){
					iOutMemberID = pData->m_iMemberID;
					return true;
				}else{
					pData = pData->m_poNext;
				}
			}else{
				pData = pData->m_poNext;
			}
		}
	}
	return false;
}

void  PublicAttrMgr::load ()
{
	int iCount = 0,m=0;
	PublicAttrData * pTemp = 0;
	PublicAttrData * pTempNext = 0;

	TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from product_offer_public_attr_rela");
	qry.open();
	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
	qry.commit();

	iCount = iCount+1;
	m_poData = new PublicAttrData[iCount];
	m_poDataIndex = new HashList<PublicAttrData *>(iCount);
    
    qry.setSQL("select offer_id,nvl(offer_detail_id,0) member_id,attr_id from product_offer_public_attr_rela");
    qry.open();
	while(qry.next())
	{
		pTemp = m_poData+m;
		pTemp->m_iOfferID = qry.field(0).asInteger();
		pTemp->m_iMemberID = qry.field(1).asInteger();
		pTemp->m_iAttrID = qry.field(2).asInteger();
		pTemp->m_poNext = 0;
		if (m_poDataIndex->get(pTemp->m_iOfferID,&pTempNext)){
			pTemp->m_poNext = pTempNext;
		}
		m_poDataIndex->add(pTemp->m_iOfferID,pTemp);
		m++;
	}
	qry.close();
	qry.commit();
}
