/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#include <string.h>
#include "Environment.h"
#include "AcctItemOweMgr.h"
#include "ParamDefineMgr.h"

SHMData<AcctItemOweData> * AcctItemOweMgr::m_poOweData = 0;
SHMIntHashIndex * AcctItemOweMgr::m_poOweAcctIndex = 0;
SHMIntHashIndex * AcctItemOweMgr::m_poOweServIndex = 0;

bool AcctItemOweMgr::m_bUploaded = false;

//#define SHM_ACCTITEMOWE_DATA 0x500
//#define SHM_ACCTITEMOWE_INDEX1 0x501
//#define SHM_ACCTITEMOWE_INDEX2 0x502

#define SHM_ACCTITEMOWE_DATA		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweBuf"))
#define SHM_ACCTITEMOWE_INDEX1		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweIndex1"))
#define SHM_ACCTITEMOWE_INDEX2		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweIndex2"))

#define ACCTITEMOWEBUF_DATA_COUNT1 (ParamDefineMgr::getLongParam("MEMORY_DB", "AcctItemOweDataCnt"))
int AcctItemOweMgr::getAcctOweV(long lAcctID,vector<AcctItemOweData> & vData)
{
        unsigned int k;

        AcctItemOweData * pData =  *m_poOweData;
        if (!m_poOweAcctIndex->get (lAcctID, &k))
        {
                return 0;
        }
        while (k)
        {
                if(pData[k].isValid == 'Y')
                {
                        if (pData[k].iAmount != 0)
                                vData.push_back(pData[k]);
                }
                k = pData[k].iNextAcctOffset;
        }

        return 0;

}
AcctItemOweMgr::AcctItemOweMgr()
{
	if (m_bUploaded) return;

	m_poOweData = new SHMData<AcctItemOweData> (SHM_ACCTITEMOWE_DATA);
	m_poOweAcctIndex = new SHMIntHashIndex (SHM_ACCTITEMOWE_INDEX1);
	m_poOweServIndex = new SHMIntHashIndex (SHM_ACCTITEMOWE_INDEX2);

	if (!m_poOweData->exist () || !m_poOweAcctIndex->exist () || !m_poOweServIndex->exist ()) {
		m_bUploaded = false;
		load ();
	} else {
		m_bUploaded = true;
	}
}

AcctItemOweMgr::~AcctItemOweMgr()
{
}

void AcctItemOweMgr::unload()
{
	if (m_poOweData->exist()) m_poOweData->remove ();
	if (m_poOweServIndex->exist ()) m_poOweServIndex->remove ();
	if (m_poOweAcctIndex->exist ()) m_poOweAcctIndex->remove ();
}

void AcctItemOweMgr::load()
{
	DEFINE_QUERY (qry);
	int iCount, iTemp;
	AcctItemOweData * pData;
	unsigned int k;

	if (m_poOweData->exist ()) m_poOweData->remove ();
	if (m_poOweServIndex->exist ()) m_poOweServIndex->remove ();
	if (m_poOweAcctIndex->exist ()) m_poOweAcctIndex->remove ();
/*
	qry.setSQL ("select count(distinct acct_id, serv_id ) from acct_item_owe");
	qry.open (); qry.next ();
	iCount = qry.field (0).asInteger () + 1000;
	qry.close ();
*/
	m_poOweData->create (ACCTITEMOWEBUF_DATA_COUNT1);
	m_poOweServIndex->create (ACCTITEMOWEBUF_DATA_COUNT1);
	m_poOweAcctIndex->create (ACCTITEMOWEBUF_DATA_COUNT1);

	pData = *m_poOweData;

	qry.setSQL ("select acct_id,nvl(serv_id,-1) serv_id,sum(amount) amount,count(distinct billing_cycle_id) billing_cnt "
		    "from acct_item_owe where state in ('5JA','5JC','5JD','5JF') group by acct_id,nvl(serv_id,-1) ");
	
	qry.open();
	
	while (qry.next ()) {
		iTemp = m_poOweData->malloc ();
		
		if (!iTemp) THROW (MBC_OweMgr+3);

		pData[iTemp].iAcctID = qry.field (0).asLong();
		pData[iTemp].iServID = qry.field (1).asLong();
		pData[iTemp].lAmount = qry.field (2).asLong();
		pData[iTemp].iBillingCnt = qry.field (3).asInteger ();
		
		if (m_poOweAcctIndex->get (pData[iTemp].iAcctID, &k)) {
			pData[iTemp].iNextAcctOffset = pData[k].iNextAcctOffset;
			pData[k].iNextAcctOffset  = iTemp;
		} else {
			m_poOweAcctIndex->add (pData[iTemp].iAcctID, iTemp);
		}

		if (m_poOweServIndex->get (pData[iTemp].iServID, &k)) {
			pData[iTemp].iNextServOffset = pData[k].iNextServOffset;
			pData[k].iNextServOffset  = iTemp;
		} else {
			m_poOweServIndex->add (pData[iTemp].iServID, iTemp);
		}
	}

	qry.close ();
}


int AcctItemOweMgr::getServOwe(long iServID)
{
	if (!m_bUploaded) THROW (MBC_OweMgr+4);

	AcctItemOweData * pData =  *m_poOweData;
	unsigned int k;
	int iRet = 0;

	if (!m_poOweServIndex->get (iServID, &k)) return 0;
	
	while (k) {
		iRet += pData[k].lAmount;
		k = pData[k].iNextServOffset;
	}

	return iRet;
}

int AcctItemOweMgr::getAcctOwe(long iAcctID)
{
	if (!m_bUploaded) THROW  (MBC_OweMgr+4);
	
	AcctItemOweData * pData =  *m_poOweData;
	unsigned int k;
	int iRet = 0;
	
	if (!m_poOweAcctIndex->get (iAcctID, &k)) return 0;
	
	while (k)
	{
		iRet += pData[k].lAmount;
		k = pData[k].iNextAcctOffset;
	}
	
	return iRet;
}

int AcctItemOweMgr::getBillingCnt(long iAcctID)
{
	if (!m_bUploaded) THROW  (MBC_OweMgr+4);
	
	AcctItemOweData * pData =  *m_poOweData;
	unsigned int k;
	int iRet = 0;
	
	if (!m_poOweAcctIndex->get (iAcctID, &k)) return 0;
	
	while (k)
	{
		if (iRet<pData[k].iBillingCnt)
			iRet = pData[k].iBillingCnt;
		k = pData[k].iNextAcctOffset;
	}
	
	return iRet;
}
