/*VER: 1*/ 
#include "RegionBilling.h"
#include "Log.h"

bool RegionBilling::m_bIsLoad = false;
HashList<RegionBill *>* RegionBilling::m_poRelation = 0;
HashList<RegionNasBill *>* RegionBilling::m_poNasRelation = 0;

bool RegionBilling::LoadRule(void)
{
	int iCount = 1;
	RegionBill * pTemp;
	RegionBill * p;
	DEFINE_QUERY(qry);
	qry.setSQL ("select count(*) from b_cell_group_member");
	qry.open (); qry.next ();
	iCount = qry.field(0).asInteger() + 1;
	qry.close ();
	qry.commit();
    m_poRelation = new HashList<RegionBill *>(iCount);
	if(NULL==m_poRelation){
		Log::log(0,"RegionBilling::LoadRule->new HashList<int>(iCount) Ê§°Ü£¬ÇëÈ·ÈÏÄÚ´æ¿Õ¼ä×ã¹»£¡");
		return false;
	}
    qry.setSQL("select nvl(cell_id,0),nvl(cell_group_id,0) from b_cell_group_member");
    qry.open();
    while (qry.next())
	{
		pTemp =  new RegionBill();
		strncpy(pTemp->m_sCellID,qry.field(0).asString(),sizeof(pTemp->m_sCellID)-1);
		pTemp->m_iGroupID = qry.field(1).asInteger();
		pTemp->m_poNext = 0;
		if(m_poRelation->get(pTemp->m_sCellID,&p))
		{
			for(;p->m_poNext;p=p->m_poNext);
			p->m_poNext = pTemp;
		}else
		{
			m_poRelation->add(pTemp->m_sCellID,pTemp);
		}
	}
    qry.close();
	qry.commit();
	
    LoadNasRule();
    return true;
}

bool RegionBilling::LoadNasRule(void)
{
	int iCount = 1;
	RegionNasBill * pTemp;
	RegionNasBill * p;
	DEFINE_QUERY(qry);
	qry.setSQL ("select count(*) from b_cell_group_nas_member");
	qry.open (); qry.next ();
	iCount = qry.field(0).asInteger() + 1;
	qry.close ();
	qry.commit();

    m_poNasRelation = new HashList<RegionNasBill *>(iCount);
    
	if(NULL==m_poNasRelation){
		Log::log(0,"m_poNasRelation:RegionBilling::LoadRule->new HashList<int>(iCount) Ê§°Ü£¬ÇëÈ·ÈÏÄÚ´æ¿Õ¼ä×ã¹»£¡");
		return false;
	}
    qry.setSQL("select nvl(nas_ip,0),nvl(cell_group_id,0) from b_cell_group_nas_member");
    qry.open();
    while ( qry.next() )
	{
		pTemp =  new RegionNasBill();
		strncpy(pTemp->m_sNasIP,qry.field(0).asString(),sizeof(pTemp->m_sNasIP)-1);
		pTemp->m_iGroupID = qry.field(1).asInteger();
		pTemp->m_poNext = 0;
		if(m_poNasRelation->get(pTemp->m_sNasIP,&p))
		{
			for(;p->m_poNext;p=p->m_poNext);
			p->m_poNext = pTemp;
		}else
		{
			m_poNasRelation->add(pTemp->m_sNasIP,pTemp);
		}
	}
    qry.close();
	qry.commit();

    return true;
}

bool RegionBilling::CheckRelation(char* sCellID,int iGroupID)
{
	bool bRet =  false;
	if(NULL == sCellID) return bRet;
	RegionBill * pTemp = 0;
	if(!m_bIsLoad)
	{
		if(!LoadRule())
		{
			bRet =  false;
			return bRet;
		}
		m_bIsLoad = true;
	}
    if(m_poRelation->get(sCellID,&pTemp))
	{
		while(pTemp)
		{
			if(pTemp->m_iGroupID == iGroupID)
			{
				bRet = true;
				break;
			}
			pTemp = pTemp->m_poNext;
		}
	}
    return bRet;
}

bool RegionBilling::CheckNasRelation(char* sNasIP, int iGroupID)
{
	bool bRet =  false;
	if(NULL == sNasIP || sNasIP[0]==0 ) return bRet;
	RegionNasBill * pTemp = 0;
	if(!m_bIsLoad)
	{
		if(!LoadRule())
		{
			bRet =  false;
			return bRet;
		}
		m_bIsLoad = true;
	}
    if(m_poNasRelation->get(sNasIP,&pTemp))
	{
		while(pTemp)
		{
			if(pTemp->m_iGroupID == iGroupID)
			{
				bRet = true;
				break;
			}
			pTemp = pTemp->m_poNext;
		}
	}
    return bRet;
}

