/*VER: 3*/ 
/*
**
**  2007.11.02 caidk
**      对于数据库中OFFER_TYPE为1的当初基本商品来处理
**  2007.10.17 xueqt 
**      增加loadFreeOffer; isFreeOffer; 修改OfferMgr::OfferMgr
*/


#include "OfferMgr.h"
#include "Environment.h"
#include "Exception.h"
#include "TOCIQuery.h"
#include "Queue.h"
#include "Log.h"
#include "MBC.h"
#include "ParamDefineMgr.h"

bool OfferMgr::m_bUploaded = false;
void OfferMgr::load()
{
	unload ();
    char sql[2048];
    int count;
    Offer * pOfferInfo;
	DEFINE_QUERY (qry);
	
	qry.setSQL ("select count(*) total from PRODUCT_OFFER");
	qry.open();
	qry.next();
	count = qry.field(0).asInteger ();
	qry.close();
	
	m_poOfferInfoList = new Offer[count];
	if (!m_poOfferInfoList) THROW(MBC_OfferMgr+1);
	m_poOfferInfoIndex = new HashList<Offer *>(count);
	if (!m_poOfferInfoIndex) THROW(MBC_OfferMgr+2);
	
	//modify by zhaoziwei
	/*
	qry.setSQL ("SELECT A.OFFER_ID, A.PRICING_PLAN_ID, nvl(A.OFFER_PRIORITY,0), "
                      "DECODE(A.OFFER_TYPE,1,0,A.OFFER_TYPE), A.EFF_DATE,"
                      "nvl(A.EXP_DATE,TO_DATE('20500101','yyyymmdd')),nvl(A.BRAND_ID,0)"
                 "FROM PRODUCT_OFFER A "
                 "WHERE STATE = '10A' ");
	*/
	qry.setSQL ("SELECT A.OFFER_ID, A.PRICING_PLAN_ID, nvl(A.OFFER_PRIORITY,0), "
                      "DECODE(A.OFFER_TYPE,1,0,A.OFFER_TYPE), A.EFF_DATE,"
                      "nvl(A.EXP_DATE,TO_DATE('20500101','yyyymmdd')),nvl(A.BRAND_ID,0),nvl(A.AGREEMENT_LENGTH,0)"
                 " FROM PRODUCT_OFFER A "
                 " WHERE STATE = '10A' ");
	//end
                        
    qry.open();
    
    while (qry.next()) {
        count--;
        pOfferInfo = m_poOfferInfoList + count;
        pOfferInfo->m_iOfferID = qry.field(0).asInteger();
        pOfferInfo->m_iPricingPlan = qry.field(1).asInteger();
        pOfferInfo->m_iPriority = qry.field(2).asInteger();
        pOfferInfo->m_iOfferType = (OfferType)qry.field(3).asInteger();
        strcpy(pOfferInfo->m_sEffDate,qry.field(4).asString());
        strcpy(pOfferInfo->m_sExpDate,qry.field(5).asString());
        pOfferInfo->m_poOfferAttr = 0;
        pOfferInfo->m_iBrandID = qry.field(6).asInteger();
        //add by zhaoziwei
        pOfferInfo->m_iAgreementLength = qry.field(7).asInteger();
        //end
        m_poOfferInfoIndex->add(pOfferInfo->m_iOfferID, pOfferInfo);
    }
    qry.close();
    
  OfferAttr *pOfferAttr;
	qry.setSQL ("select count(*) from product_offer_attr");
	qry.open ();
	if(qry.next ())
	{
		count= qry.field(0).asInteger () + 2;
	}
	qry.close();

	m_poOfferAttrList =  new OfferAttr[count];
  if (!m_poOfferAttrList) THROW(MBC_OfferMgr+1);
	m_poOfferAttrIndex = new HashList<OfferAttr *> (count);
	if (!m_poOfferAttrIndex) THROW(MBC_OfferMgr+1);
  memset(m_poOfferAttrList,0,sizeof(OfferAttr)*count);
  
	qry.setSQL ("select A.offer_id, A.attr_id, A.offer_attr_value from  product_offer_attr A,"
	 "product_offer B where B.state = '10A' and A.offer_id = B.offer_id order by A.offer_id, A.attr_id");
	qry.open ();
	while(qry.next ())
	{
		if(!m_poOfferInfoIndex->get(qry.field(0).asInteger(),&pOfferInfo)) continue;
		
		count--;
		pOfferAttr = m_poOfferAttrList + count;
       
		pOfferAttr->m_iAttrId = qry.field(1).asInteger();
		strcpy(pOfferAttr->m_sAttrValue,qry.field(2).asString());
		pOfferAttr->m_poNext = pOfferInfo->m_poOfferAttr;
	
		pOfferInfo->m_poOfferAttr = pOfferAttr;
       
		m_poOfferAttrIndex->add(pOfferAttr->m_iAttrId,pOfferAttr);
	}
	qry.close();
	
	char str[8]={0};
  if(ParamDefineMgr::getParam("PRODOFFER","USEDEFAULT",str))
  {
    m_iUsedDefaultAttr = atoi(str);
  }
	
	m_bUploaded = true;
}

void OfferMgr::loadSpecialAttr()
{
	if (m_poSpeicalOfferAttrIndex) {
        delete m_poSpeicalOfferAttrIndex;
        m_poSpeicalOfferAttrIndex = 0;
    }
	DEFINE_QUERY(qry);		
	int iCount = 0;
	char sql[128];
	strcpy(sql,"select count(*) from product_offer_attr  where attr_value_type_id=15");
	qry.setSQL (sql);
	qry.open ();
	if(qry.next ())
	{
		iCount= qry.field(0).asInteger () ;
	}
	qry.close();

	m_poSpeicalOfferAttrIndex =  new HashList<int> (iCount+2);
	strcpy(sql,"select attr_id,nvl(to_number(offer_attr_value),0) from product_offer_attr  where attr_value_type_id=15");
	qry.setSQL (sql);
	qry.open ();
	while(qry.next ())
	{
		m_poSpeicalOfferAttrIndex->add(qry.field(0).asInteger(),qry.field(1).asInteger());
	}
	qry.close();
}

void OfferMgr::loadNumberAttr()
{
	if (m_poNumberOfferAttrIndex) {
        delete m_poNumberOfferAttrIndex;
        m_poNumberOfferAttrIndex = 0;
    }
	DEFINE_QUERY(qry);		
	int iCount = 0;
	char sql[128];
	strcpy(sql,"select count(*) from product_offer_attr  where attr_value_type_id in (3,5,15)");
	qry.setSQL (sql);
	qry.open ();
	if(qry.next ())
	{
		iCount= qry.field(0).asInteger () ;
	}
	qry.close();

	m_poNumberOfferAttrIndex =  new HashList<int> (iCount+2);
	strcpy(sql,"select attr_id from product_offer_attr  where attr_value_type_id in (3,5,15)");
	qry.setSQL (sql);
	qry.open ();
	while(qry.next ())
	{
		m_poNumberOfferAttrIndex->add(qry.field(0).asInteger(),1);
	}
	qry.close();

	char countbuf[32];
	memset(countbuf,'\0',sizeof(countbuf));
	if(ParamDefineMgr::getParam("PRODOFFER", "NUMBERCONVERT", countbuf))
	{
		m_iFlag = atoi(countbuf);
	}
}

bool OfferMgr::checkSpecialAttr(int iAttrID,int &iOperation)
{
	if(m_poSpeicalOfferAttrIndex->get(iAttrID,&iOperation)){
		return true;
	}else{
		return false;
	}
}

bool OfferMgr::checkNumberAttr(int iAttrID)
{
	if(m_iFlag == 1)
	{
		int i;
		if(m_poNumberOfferAttrIndex->get(iAttrID,&i)){
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

//##ModelId=42369FF001FD
void OfferMgr::unload()
{
	m_bUploaded = false;
	if(m_poOfferInfoList)
	{
	  delete[] m_poOfferInfoList;
	  m_poOfferInfoList = 0;
	}
	if(m_poOfferInfoIndex)
	{
	  delete m_poOfferInfoIndex;
	  m_poOfferInfoIndex = 0;
	}	
	if(m_poOfferAttrList)
	{
	  delete[] m_poOfferAttrList;
	  m_poOfferAttrList = 0;    
	}
	if(m_poOfferAttrIndex)
	{
	  delete m_poOfferAttrIndex;
    m_poOfferAttrIndex = 0;    
  }
}

Offer * OfferMgr::getOfferInfoByID(int iOfferID)
{
    Offer * pOffer;
    if(!m_poOfferInfoIndex->get(iOfferID, &pOffer))
    {
        return 0;
    }
    return pOffer;
}

OfferMgr::OfferMgr():m_poOfferInfoList(0),m_poOfferAttrList(0),m_poOfferAttrIndex(0),
m_poOfferInfoIndex(0),m_poFreeOfferIndex(0),m_poSpeicalOfferAttrIndex(0),m_poNumberOfferAttrIndex(0),m_iFlag(0),m_iUsedDefaultAttr(0)
{
}

void OfferMgr::loadFreeOffer()
{
    if (m_poFreeOfferIndex) {
        delete m_poFreeOfferIndex;
        m_poFreeOfferIndex = 0;
    }
    
    m_poFreeOfferIndex = new HashList<int> (128);
    
    DEFINE_QUERY (qry);
    qry.setSQL ("select offer_id from b_offer_item_group_relation "
                "where free_type=1");
    qry.open ();
    
    while (qry.next ()) {
        m_poFreeOfferIndex->add (qry.field (0).asInteger (), 1);
    }
    
    qry.close ();
}    

bool OfferMgr::isFreeOffer(int iOfferID)
{
    int i;
    
    return m_poFreeOfferIndex->get (iOfferID, &i);
}

char * OfferMgr::getOfferAttrValue(int iOfferId,int iAttrId,char *sDate)
{
	if(m_iUsedDefaultAttr == 1)
	{
     Offer *pOfferInfo;
     OfferAttr *pOfferAttr;
    
     if(m_poOfferInfoIndex->get(iOfferId,&pOfferInfo)) 
     {
	     if( strcmp(sDate, pOfferInfo->m_sEffDate) < 0 || 
	         strcmp(sDate, pOfferInfo->m_sExpDate) >= 0 ) return 0;
	         
	     if(m_poOfferAttrIndex->get(iAttrId,&pOfferAttr))
     	 {
	       return pOfferAttr->m_sAttrValue;
	     }
     }
     
  }
    
  return 0;
}

//得到商品品牌的id
bool OfferMgr::getbrand(int iOfferId,int &ibrandid )
{
      Offer * pOffer;
    if(!m_poOfferInfoIndex->get(iOfferId, &pOffer))
    {
        return false;
    }
	else if(0==pOffer->m_iBrandID)
	
		return false;
		
   else
       ibrandid = pOffer->m_iBrandID;
    return true;
}

OfferMgr::~OfferMgr()
{
    unload();
	if (m_poFreeOfferIndex)
	{
        delete m_poFreeOfferIndex;
        m_poFreeOfferIndex = 0;
    }
	if (m_poSpeicalOfferAttrIndex) {
        delete m_poSpeicalOfferAttrIndex;
        m_poSpeicalOfferAttrIndex = 0;
    }
	if (m_poNumberOfferAttrIndex) {
        delete m_poNumberOfferAttrIndex;
        m_poNumberOfferAttrIndex = 0;
    }
}
