#include <iostream>
#include "Environment.h"
#include "Log.h"
#include "OrgMgr.h"
#include "InstTableListMgrKerEx.h"

OrgMgr * m_gpOrgMgr;

InstTableListMgrKerEx::InstTableListMgrKerEx(char const *sTable)
{
  m_bLoad = false;
  if(!m_gpOrgMgr) 
		m_gpOrgMgr = new OrgMgr();

	if(!m_bLoad)
	{
		load(sTable);
		m_bLoad = true;
	}
}

InstTableListMgrKerEx::~InstTableListMgrKerEx()
{
	m_bLoad = false;
	delete m_gpOrgMgr;
	freeHashList(table_list);
}

void InstTableListMgrKerEx::load(char const *sTable)
{
  char sSql[4096];

  sprintf (sSql, "SELECT "
                "NVL(BILLING_CYCLE_ID,-2) BILLING_CYCLE_ID, "
                "TABLE_TYPE, "
                "NVL(ORG_ID,-2) ORG_ID, "
                "TABLE_NAME "
                "FROM %s where TABLE_TYPE=11 "
                " ORDER BY NVL(BILLING_CYCLE_ID,-2), "
          			"TABLE_TYPE,NVL(ORG_ID,-2)",sTable);
	
	InstTableListEx temp;
	char key[20];
	
	if (m_bLoad) return;
	
	initHashList (&table_list);
	
	DEFINE_QUERY(qry);	
	qry.setSQL(sSql);
	qry.open();
	
	m_iCnt = 0;
	m_iMinOrgLevel=1;
	
	while (qry.next())
		m_iCnt++; 
	
	if (m_iCnt == 0){
		printf("InstTableListMgrKerEx::load[B_INST_TABLE_LIST_CFG_EX表为空]\n");
		Log::log(0,"InstTableListMgrKerEx::load[B_INST_TABLE_LIST_CFG_EX表为空]\n");
		return;
	}
	
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	while(qry.next()){
		temp.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asInteger();
		temp.iTableType = qry.field("TABLE_TYPE").asInteger();
		temp.iOrgID = qry.field("ORG_ID").asInteger();
		strcpy(temp.sTableName,qry.field("TABLE_NAME").asString());
		
		if(m_iCnt==1)
			strcpy(m_sTableName,qry.field("TABLE_NAME").asString());
		int level=m_gpOrgMgr->getLevel(temp.iOrgID);
		m_iMinOrgLevel=m_iMinOrgLevel>level?m_iMinOrgLevel:level;
		
		sprintf (key, "%dB%d",temp.iOrgID,temp.iBillingCycleID);
    
		if (insertHashList(table_list,key,&temp,sizeof(InstTableListEx)) == -1)
			THROW(MBC_InstTableListMgr+1);
	}
  
	qry.close();    
}

int cmpEx (const void *list_data, const void *cmp_data)
{
	InstTableListEx *p1;
	InstTableListEx *p2;
	
	p1 = (InstTableListEx *)list_data;
	p2 = (InstTableListEx *)cmp_data;
	
	if(	(p1->iBillingCycleID == p2->iBillingCycleID || p1->iBillingCycleID==-2) && (p2->iOrgID==p1->iOrgID)	)
		return 0;
	return 1;
}

int cmp2Ex (const void *list_data, const void *cmp_data)
{
	InstTableListEx *p1;
	InstTableListEx *p2;
	
	p1 = (InstTableListEx *)list_data;
	p2 = (InstTableListEx *)cmp_data;
	
	if( (p1->iBillingCycleID == p2->iBillingCycleID || p1->iBillingCycleID==-2) && (p1->iOrgID==-2) )
		return 0;
	return 1;
}

bool InstTableListMgrKerEx::getTableName(int i_iOrgID,int i_iBillingCycleID,char * o_sTableName)
{
	InstTableListEx temp;
	InstTableListEx * node=NULL;
	
	char key[32];
	int orgid=i_iOrgID;
	
	while(node == NULL)
	{
		//总账中的org_id不存在的情况
		if(orgid==-1)
		{
			temp.iOrgID = i_iOrgID;
			temp.iBillingCycleID = i_iBillingCycleID;			
			sprintf (key, "-2B%d",i_iBillingCycleID);			
			node = (InstTableListEx *)searchHashList(table_list, key, &temp, &cmp2Ex);
			break;
		}
		
		temp.iOrgID = orgid;
		temp.iBillingCycleID = i_iBillingCycleID;		
		sprintf (key, "%dB%d",orgid,i_iBillingCycleID);		
		node = (InstTableListEx *)searchHashList(table_list, key, &temp, &cmpEx);
		
		orgid=m_gpOrgMgr->getParentID(orgid);
	}
	
	if(node == NULL)
		return false;
	
	strcpy(o_sTableName,node->sTableName);
	if(strlen(o_sTableName) == 0)
		return false;

	return true;
}
