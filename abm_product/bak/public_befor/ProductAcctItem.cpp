/*VER: 1*/ 
#include "ProductAcctItem.h"
#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif
HashList<HashList<int> *> * ProductAcctItem::m_poChecker = 0;
bool ProductAcctItem::m_bUploaded = false;

ProductAcctItem::ProductAcctItem()
{
#ifdef PRIVATE_MEMORY
	if (!m_bUploaded) load();
#endif
}

#ifdef PRIVATE_MEMORY
bool ProductAcctItem::check(int iProductID, int iAcctItemTypeID)
{
	HashList<int> * pList;
	int iTemp;

	if (!m_poChecker->get (iProductID, &pList)) return true;

	return pList->get (iAcctItemTypeID, &iTemp);
}
#else
bool ProductAcctItem::check(int iProductID, int iAcctItemTypeID)
{
	unsigned int iTemp = 0;
	if (!G_PPARAMINFO->m_poProductIDIndex->get (iProductID, &iTemp)) return true;
	char m_sTmpBuf[23] = {0};
	sprintf(m_sTmpBuf,"%d_%d", iProductID, iAcctItemTypeID);	
	return G_PPARAMINFO->m_poProIDAcctItemStringIndex->get (m_sTmpBuf, &iTemp);
}
#endif

void ProductAcctItem::add(HashList<int> *pList, int iAcctItemTypeID)
{
	DEFINE_QUERY(qry);
	char sSQL[2048];

	sprintf (sSQL, "select acct_item_type_id from acct_item_type"
            		" connect by summary_acct_item_type_id = prior acct_item_type_id "
            		" start with acct_item_type_id = %d", iAcctItemTypeID);
	qry.setSQL (sSQL); qry.open ();
	while (qry.next ()) {
		pList->add (qry.field (0).asInteger (), 1);
	}
	qry.close ();
}

void ProductAcctItem::load()
{
	DEFINE_QUERY (qry);
	HashList<int> * pList;
	int iProductID;

	m_poChecker = new HashList<HashList<int> *> (91);
	if (!m_poChecker) THROW (10);

	qry.setSQL ("select product_id, acct_item_type_id from b_prod_acct_item_type");
	qry.open ();

	while (qry.next ()) {
		iProductID = qry.field (0).asInteger ();
		if (!m_poChecker->get (iProductID, &pList)) {
			pList = new HashList<int> (53);
			if (!pList) THROW (10);

			m_poChecker->add (iProductID, pList);
		}

		add (pList, qry.field (1).asInteger ());
	}

	qry.close ();
	m_bUploaded = true;
}


void ProductAcctItem::unload()
{
	//œ»≤ªπ‹
}
