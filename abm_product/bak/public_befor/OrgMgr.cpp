/*VER: 1*/ 
#include "OrgMgr.h"
#include "SimpleMatrix.h"
#include "Environment.h"
#include "Exception.h"
#include <string.h>
#include <stdio.h>

#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=4248F8440199
void OrgMgr::load()
{
	int count, i;

	unload ();

	TOCIQuery qry(Environment::getDBConn());

	qry.setSQL ("select count(*) from org");
	qry.open ();

	qry.next ();
	count = qry.field(0).asInteger () + 16;
	qry.close ();

	m_poOrgList = new Org[count];
	m_poOrgIndex = new HashList<Org *> ((count>>1) | 1);

	if (!m_poOrgList || !m_poOrgIndex)
		THROW(MBC_OrgMgr+1);

	qry.setSQL ("SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), nvl(ORG_LEVEL_ID, -2) FROM ORG");
	qry.open ();

	i = 0;

	while (qry.next ()) {
		if (i==count) THROW (MBC_OrgMgr+2);

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
void OrgMgr::unload()
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
#ifdef PRIVATE_MEMORY
bool OrgMgr::getBelong(int iChildOrg, int iParentOrg)
{
	Org * pOrg;

	if (!m_poOrgIndex->get (iChildOrg, &pOrg)) return false;

	if (iChildOrg == iParentOrg ) return true;

	while (pOrg) {
		if (pOrg->m_iParentID == iParentOrg) return true;
		pOrg = pOrg->m_poParent;
	}

        return false;
}
#else
bool OrgMgr::getBelong(int iChildOrg, int iParentOrg)
{
	Org *pOrg = 0;
    unsigned int iIdx = 0;
    
    if(!(G_PPARAMINFO->m_poOrgIndex->get(iChildOrg, &iIdx)))
        return false;
    else if(iChildOrg == iParentOrg)
        return true;
    else
    {
        pOrg = (Org *)G_PPARAMINFO->m_poOrgList + iIdx;
        while(1)
        {
            if(pOrg->m_iParentID == iParentOrg)
                return true;
            else if(pOrg->m_iParentOffset != 0)
                pOrg = (Org *)G_PPARAMINFO->m_poOrgList + pOrg->m_iParentOffset;
            else
                return false;
        }
        return false;
    }
}
#endif

//##ModelId=4248F844024D
#ifdef PRIVATE_MEMORY
OrgMgr::OrgMgr()
{
        if (!m_bUploaded)
                load();
}
#else
OrgMgr::OrgMgr()
{
}
#endif

//##ModelId=4248F8440289
OrgMgr::~OrgMgr()
{
}

//##ModelId=4248F8730331
HashList<Org *> *OrgMgr::m_poOrgIndex = 0;

//##ModelId=42732EB50024
bool OrgMgr::m_bUploaded = false;

//##ModelId=42732E62022D
Org *OrgMgr::m_poOrgList = 0;

//##ModelId=42732CFB01F5
#ifdef PRIVATE_MEMORY
int OrgMgr::getParentID(int iOrgID)
{
	Org * pOrg;

	if (!m_poOrgIndex->get (iOrgID, &pOrg)) return -1;
	
	return pOrg->m_iParentID;
}
#else
int OrgMgr::getParentID(int iOrgID)
{
    unsigned int iIdx = 0;
    Org *pOrg = 0;
    if(G_PPARAMINFO->m_poOrgIndex->get(iOrgID, &iIdx))
    {
        pOrg = (Org *)G_PPARAMINFO->m_poOrgList + iIdx;
        return pOrg->m_iParentID;
    }
    else
        return -1;
}
#endif

//added by ap//根据level来获取那一级的parentID
#ifdef PRIVATE_MEMORY
int OrgMgr::getLevelParentID(int iOrgID, int iLevel)
{
	Org * pOrg;

	if (!m_poOrgIndex->get (iOrgID, &pOrg)) return -1;
	while (pOrg) {
		if (pOrg->m_iLevel == iLevel)
		 return pOrg->m_iOrgID;
		pOrg = pOrg->m_poParent;
	}
	
	return -1;
}
#else
int OrgMgr::getLevelParentID(int iOrgID, int iLevel)
{
	unsigned int iIdx = 0;
    Org *pOrg = 0;
    if(G_PPARAMINFO->m_poOrgIndex->get(iOrgID, &iIdx))
    {
        pOrg = (Org *)G_PPARAMINFO->m_poOrgList + iIdx;
        while(1)
        {
            if(pOrg->m_iLevel == iLevel)
                return pOrg->m_iOrgID;
            else if(pOrg->m_iParentOffset != 0)
                pOrg = (Org *)G_PPARAMINFO->m_poOrgList + pOrg->m_iParentOffset;
            else
                return -1;
        }
    }
    else
        return -1;
}
#endif

//##ModelId=42732D1800AC
#ifdef PRIVATE_MEMORY
int OrgMgr::getLevel(int iOrgID)
{
	Org * pOrg;

	if (!m_poOrgIndex->get (iOrgID, &pOrg)) return -1;

	return pOrg->m_iLevel;
}
#else
int OrgMgr::getLevel(int iOrgID)
{
	unsigned int iIdx = 0;
    Org *pOrg = 0;
    if(G_PPARAMINFO->m_poOrgIndex->get(iOrgID, &iIdx))
    {
        pOrg = (Org *)G_PPARAMINFO->m_poOrgList + iIdx;
        return  pOrg->m_iLevel;
    }
    else
        return -1;
}
#endif

//##ModelId=427333930396
Org::Org() :
m_iOrgID (-2), m_iParentID (-2), m_iLevel (-2), m_poParent (0)
{
}


