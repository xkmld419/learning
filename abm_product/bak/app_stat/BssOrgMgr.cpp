/*VER: 1*/ 
#include "BssOrgMgr.h"
#include "SimpleMatrix.h"
#include "Environment.h"
#include "Exception.h"
#include <string.h>
#include <stdio.h>

#include "MBC.h"

//##ModelId=4248F8440199
void BssOrgMgr::load()
{
	int count, i;

	unload ();

	TOCIQuery qry(Environment::getDBConn());

	qry.setSQL ("select count(*) from bss_org");
	qry.open ();

	qry.next ();
	count = qry.field(0).asInteger () + 16;
	qry.close ();

	m_poOrgList = new BssOrg[count];
	m_poOrgIndex = new HashList<BssOrg *> (count >> 1);

	if (!m_poOrgList || !m_poOrgIndex)
		THROW(MBC_BssOrgMgr+1);

	qry.setSQL ("SELECT BSS_ORG_ID, nvl(BSS_PARENT_ORG_ID, -2), nvl(BSS_ORG_LEVEL_ID, -2) FROM BSS_ORG");
	qry.open ();

	i = 0;

	while (qry.next ()) {
		if (i==count) THROW (MBC_BssOrgMgr+2);

		m_poOrgList[i].m_iOrgID = qry.field(0).asInteger ();
		m_poOrgList[i].m_iParentID = qry.field(1).asInteger ();
		m_poOrgList[i].m_iLevel = qry.field(2).asInteger ();

		m_poOrgIndex->add (m_poOrgList[i].m_iOrgID, &(m_poOrgList[i]));

		i++;
	}
	qry.close ();

	while (i--) {
		m_poOrgIndex->get (m_poOrgList[i].m_iParentID, &(m_poOrgList[i].m_poParent));
	}

	m_bUploaded = true;
}

//##ModelId=4248F84401D5
void BssOrgMgr::unload()
{
	m_bUploaded = false;
	if (m_poOrgIndex)
		delete m_poOrgIndex;

	if (m_poOrgList)
		delete m_poOrgList;

        m_poOrgIndex = 0;
        m_poOrgList = 0;
}

//##ModelId=4248F8440211
bool BssOrgMgr::getBelong(int iChildOrg, int iParentOrg)
{
	BssOrg * pOrg;

	if (!m_poOrgIndex->get (iChildOrg, &pOrg)) return false;

	while (pOrg) {
		if (pOrg->m_iParentID == iParentOrg) return true;
		pOrg = pOrg->m_poParent;
	}

        return false;
}

//##ModelId=4248F844024D
BssOrgMgr::BssOrgMgr()
{
        if (!m_bUploaded)
                load();
}

//##ModelId=4248F8440289
BssOrgMgr::~BssOrgMgr()
{
}

//##ModelId=4248F8730331
HashList<BssOrg *> *BssOrgMgr::m_poOrgIndex = 0;

//##ModelId=42732EB50024
bool BssOrgMgr::m_bUploaded = false;

//##ModelId=42732E62022D
BssOrg *BssOrgMgr::m_poOrgList = 0;

//##ModelId=42732CFB01F5
int BssOrgMgr::getParentID(int iOrgID)
{
	BssOrg * pOrg;

	if (!m_poOrgIndex->get (iOrgID, &pOrg)) return -1;
	
	return pOrg->m_iParentID;
}

//##ModelId=42732D1800AC
int BssOrgMgr::getLevel(int iOrgID)
{
	BssOrg * pOrg;

	if (!m_poOrgIndex->get (iOrgID, &pOrg)) return -1;

	return pOrg->m_iLevel;
}

//##ModelId=427333930396
BssOrg::BssOrg() :
m_iOrgID (-2), m_iParentID (-2), m_iLevel (-2), m_poParent (0)
{
}


