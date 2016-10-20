/*VER: 2*/ 
#include "ZoneMgr.h"
#include "Exception.h"
#include "Environment.h"
#include "TOCIQuery.h"

#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=4255FA8D0354
ZoneItem::ZoneItem() :
m_iZoneID (0), m_iZoneItemID (0), m_iParentItemID (0), m_poParent (0)
{

}

bool ZoneMgr::reload()
{
    if(m_bUploaded){
        unload();
        load();
    }
    return true;
}

//##ModelId=4256019403D3
void ZoneMgr::load()
{
	unload ();
	
	int i, count;
	ZoneItem * pZoneItem = 0;
	KeyTree<ZoneItem *> * pKeyTree = 0;

	TOCIQuery qry (Environment::getDBConn ());

	m_poIndex = new HashList<KeyTree<ZoneItem *> *> (512);
	if (!m_poIndex) THROW(MBC_ZoneMgr+1);

	m_poTempIndex = new HashList<ZoneItem *> (1024);
	if (!m_poTempIndex) THROW(MBC_ZoneMgr+1);

	qry.setSQL ("select count(*) from zone_item");

	qry.open ();
	qry.next ();

	count = qry.field(0).asInteger () + 64;

	qry.close ();

	m_poZoneItemList = new ZoneItem[count];
	if (!m_poZoneItemList) THROW(MBC_ZoneMgr+1);

	qry.setSQL ("select zone_item_id, nvl(parent_zone_item_id, -1), "
			"zone_id from zone_item");

	qry.open ();

	i = 0;
	while (qry.next ()) {
		if (i == count) THROW(MBC_ZoneMgr+2);

		pZoneItem = m_poZoneItemList + i;

		pZoneItem->m_iZoneID = qry.field(2).asInteger ();
		pZoneItem->m_iZoneItemID = qry.field(0).asInteger ();
		pZoneItem->m_iParentItemID = qry.field(1).asInteger ();

		m_poTempIndex->add (pZoneItem->m_iZoneItemID, pZoneItem);

		i++;
	}

	qry.close ();

	count = i;

	for (i=0; i<count; i++) {
		pZoneItem = m_poZoneItemList + i;

		if (pZoneItem->m_iParentItemID != -1) {
			m_poTempIndex->get (pZoneItem->m_iParentItemID, &(pZoneItem->m_poParent));
		}
	}


	//#######

	int iZoneItemID;
	char sKey[64];

	qry.setSQL ("select zone_item_id, item_ref_value from zone_item_value");

	qry.open ();

	while (qry.next ()) {
		iZoneItemID = qry.field(0).asInteger ();

		if (!(m_poTempIndex->get (iZoneItemID, &pZoneItem))) THROW(MBC_ZoneMgr+3);

		if (!(m_poIndex->get (pZoneItem->m_iZoneID, &pKeyTree))) {
			pKeyTree = new KeyTree<ZoneItem *> ();
			if (!pKeyTree) THROW(MBC_ZoneMgr+1);
			m_poIndex->add (pZoneItem->m_iZoneID, pKeyTree);
		}

		sprintf (sKey, "%d_%s", qry.field(0).asInteger (), qry.field(1).asString ());

		//pKeyTree->add (qry.field(1).asString (), pZoneItem);
		pKeyTree->add (sKey, pZoneItem);
	}

	qry.close ();

	//########
	delete m_poTempIndex;
	m_poTempIndex = 0;

	m_bUploaded = true;
}

//##ModelId=425601990113
void ZoneMgr::unload()
{
	m_bUploaded = false;

	if (m_poIndex) {
	    KeyTree<ZoneItem *> * pKeyTree = 0;
	    HashList<KeyTree<ZoneItem *> *>::Iteration iter = m_poIndex->getIteration();
	    while( iter.next(pKeyTree) ){
	        if( pKeyTree ){
	            delete pKeyTree;
	            pKeyTree = 0;
	        }
	    }
		delete m_poIndex;
		m_poIndex = 0;
	}

	if (m_poTempIndex) {
		delete m_poTempIndex;
		m_poTempIndex = 0;
	}

	if (m_poZoneItemList) {
		delete [] m_poZoneItemList;
		m_poZoneItemList = 0;
	}
}

#ifdef PRIVATE_MEMORY
bool ZoneMgr::checkMax(int iZoneID, char *sValue, int iZoneItemID)
{
	KeyTree<ZoneItem *> * pKeyTree = 0;
	ZoneItem * pZoneItem = 0;
	char sKey[80];

	if (!(m_poIndex->get (iZoneID, &pKeyTree)))  return false;

	sprintf (sKey, "%d_%s", iZoneItemID, sValue);

	return pKeyTree->getMax (sKey, &pZoneItem);
}
#else
bool ZoneMgr::checkMax(int iZoneID, char *sValue, int iZoneItemID)
{
	char sTempValue[32];
    strncpy(sTempValue, sValue, 32);
    ZoneItem *pZoneItem = 0;
    ZoneItem *pTmpZoneItem = 0;
    unsigned int iIdx = 0;
    int i = 32;
    
    if(!(G_PPARAMINFO->m_poZoneItemIndex->get(iZoneID, &iIdx)))
        return false;
    else
    {
        pZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + iIdx;
        while(1)
        {
            if(pZoneItem->m_iZoneItemID == iZoneItemID)
                break;
            else if(pZoneItem->m_iNextDifOffset != 0)
                pZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + pZoneItem->m_iNextDifOffset;
            else
                return false;
        }
        pTmpZoneItem = pZoneItem;
        while(i > 0)
        {
            if(!strncmp(pTmpZoneItem->m_sZoneItemValue, sTempValue, 32))
                return true;
            else if(pTmpZoneItem->m_iNextOffset != 0)
                pTmpZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + pTmpZoneItem->m_iNextOffset;
            else
            {
                i--;
                sTempValue[i] = '\0';
                pTmpZoneItem = pZoneItem;
            }
        }
        
        return false;
    }
}
#endif



//##ModelId=425601CF00A3
#ifdef PRIVATE_MEMORY
bool ZoneMgr::check(int iZoneID, char *sValue, int iZoneItemID)
{
	KeyTree<ZoneItem *> * pKeyTree = 0;
	ZoneItem * pZoneItem = 0;
	char sKey[80];

	if (!(m_poIndex->get (iZoneID, &pKeyTree)))  return false;

	sprintf (sKey, "%d_%s", iZoneItemID, sValue);

	return pKeyTree->get (sKey, &pZoneItem);
}
#else
bool ZoneMgr::check(int iZoneID, char *sValue, int iZoneItemID)
{
	ZoneItem *pZoneItem = 0;
    unsigned int iIdx = 0;
    
    if(!(G_PPARAMINFO->m_poZoneItemIndex->get(iZoneID, &iIdx)))
        return false;
    else
    {
    	//pZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + iIdx;
        //while(1)
        //{
            //if(pZoneItem->m_iZoneItemID == iZoneItemID)
            //{
                /*if(!strncmp(pZoneItem->m_sZoneItemValue, sValue, 32))
                    return true;
                else if(pZoneItem->m_iNextOffset != 0)
                    pZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + pZoneItem->m_iNextOffset;
                else
                    return false;*/
				unsigned int iStrIdx = 0;
				char sTmp[64] = {0};
				sprintf(sTmp,"%d_%d_%s",iIdx,iZoneItemID,sValue);
				if(G_PPARAMINFO->m_poZoneItemStrIndex->get(sTmp,&iStrIdx))
				{				
					return true;
				} else {
					return false;
				}
            //}
            /*else if(pZoneItem->m_iNextDifOffset != 0)
                pZoneItem = (ZoneItem *)G_PPARAMINFO->m_poZoneItemList + pZoneItem->m_iNextDifOffset;*/
            //else
            //    return false;
        //}
        //return false;
    }
}
#endif

//##ModelId=4256026A00A6
#ifdef PRIVATE_MEMORY
ZoneMgr::ZoneMgr()
{
	if (!m_bUploaded) load ();
}
#else
ZoneMgr::ZoneMgr()
{
}
#endif



//##ModelId=4256010801FB
bool ZoneMgr::m_bUploaded = 0;

//##ModelId=42561C00016D
HashList<KeyTree<ZoneItem *> *> *ZoneMgr::m_poIndex = 0;

//##ModelId=425616620309
ZoneItem *ZoneMgr::m_poZoneItemList = 0;

//##ModelId=4256166401E0
HashList<ZoneItem *> *ZoneMgr::m_poTempIndex = 0;

