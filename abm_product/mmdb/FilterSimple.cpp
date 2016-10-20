/*简化扣费逻辑
  专供过滤使用
  */  
#include "FilterSimple.h"

FilterSimpleMgr::FilterSimpleMgr()
{
	m_pOweMgr = new AcctItemOweMgr();
	m_pBalanceMgr = new BalanceMgr();
	
#ifdef ABM_BALANCE
	int iRet;
	if (!m_pABM)
		 {
		   m_pABM = new ABM();
		   iRet = m_pABM->init();
		   if (iRet)
          {
	          printf("abm接口初始化失败,错误码:%d\n", iRet);
	          if (m_pABM)
	          	 {
	          	   delete m_pABM;
	          	   m_pABM = NULL;
	          	 }
	        }
		 }
#endif
}

FilterSimpleMgr::~FilterSimpleMgr()
{
	delete m_pOweMgr;
	delete m_pBalanceMgr;
#ifdef ABM_BALANCE
	if (m_pABM)
		 {
		   delete m_pABM;
		   m_pABM = NULL;
		 }
#endif

}

void FilterSimpleMgr::loadOweCharge(long lAcctID)
{
	m_vOweChargeTmp.clear();
	
	m_pOweMgr->getAcctOweV(lAcctID,m_vOweChargeTmp);
	
	return;
}

void FilterSimpleMgr::loadAcctBalance(long lAcctID)
{
	m_vBalance.clear();
	
	m_pBalanceMgr->getAcctBalanceV(lAcctID,m_vBalance);
	
	return;
}

void FilterSimpleMgr::loadAggrCharge(long lAcctID,long lServID,int iBillingCycleID)
{
	struct AcctItemOweData tempOweCharge;
	vector<AcctItemData *> vAggrData;
	vector<ServAcctInfo> vPayInfo;
	vector<ServAcctInfo>::iterator iterPay;
	vector<struct AcctItemData *>::iterator itrAggrData;
	int iTemp,iPreBillingCycle,iPartyRoleID;
	BillingCycleMgr bcm;
	char sState[5];
	
	static Serv ServInfo;
	Date d;
	int iBillingModeID;
	
	strcpy(sState,bcm.getPreBillingCycle(iBillingCycleID)->getState());
	if(sState[2]=='R'||sState[2]=='A'||sState[2]=='B')
	{
		iPreBillingCycle =  bcm.getPreBillingCycle (iBillingCycleID)->getBillingCycleID ();
	}
	
	if (!G_PUSERINFO->getPayInfo (vPayInfo, lAcctID)) return;
	
	for (iterPay=vPayInfo.begin(); iterPay!=vPayInfo.end(); iterPay++) {

		iPartyRoleID = 0;
		
		if (!(G_PUSERINFO->getServ (ServInfo, (*iterPay).m_lServID, d.toString()))) {

			Log::log (91, "没有找到SERVID:%ld %s", (*iterPay).m_lServID, d.toString());
			continue;
		}

		iBillingModeID = atoi(ServInfo.getAttr (ID_BILLING_MODE));

		if (iBillingModeID==0)
			iBillingModeID = ServInfo.getServInfo ()->m_iBillingMode;

		if (iBillingModeID!=2) continue;
		
		
		
		if(sState[2]=='R'||sState[2]=='A'||sState[2]=='B')
		{
			vAggrData.clear();
		
			int iReturn  = G_PAGGRMGR->getAllItemAggr((*iterPay).m_lServID,iPreBillingCycle,vAggrData);
		
			if (!iReturn) continue;
		
			for(itrAggrData=vAggrData.begin();itrAggrData!=vAggrData.end();itrAggrData++)
			{
			
				tempOweCharge.iAcctID = lAcctID;
				tempOweCharge.iServID = (*iterPay).m_lServID;
				tempOweCharge.iAmount  = (*itrAggrData)->m_lValue;
			
				if (tempOweCharge.iAmount != 0)
					m_vOweChargeTmp.push_back(tempOweCharge);
			
			}
		}
		
		vAggrData.clear();
		
		int iReturn  = G_PAGGRMGR->getAllItemAggr((*iterPay).m_lServID,iBillingCycleID,vAggrData);
		
		if (!iReturn) continue;
		
		for(itrAggrData=vAggrData.begin();itrAggrData!=vAggrData.end();itrAggrData++)
		{
			
			tempOweCharge.iAcctID = lAcctID;
			tempOweCharge.iServID = (*iterPay).m_lServID;
			tempOweCharge.iAmount  = (*itrAggrData)->m_lValue;
			
			if (tempOweCharge.iAmount != 0)
				m_vOweChargeTmp.push_back(tempOweCharge);
			
		}
		
		
	}//for(iterPay)
					
	
}

long FilterSimpleMgr::getFilterBalance(long lAcctID,long lServID,int iBillingCycleID)
{
	long lValidBalance;
	
	loadOweCharge(lAcctID);
	loadAggrCharge(lAcctID,lServID,iBillingCycleID);
	loadAcctBalance(lAcctID);
	
	lValidBalance = calcFilterBalance(lAcctID,lServID,iBillingCycleID);
	
	return lValidBalance;
}

#ifdef ABM_BALANCE
long FilterSimpleMgr::getFilterBalanceByABM(long lAcctID,long lServID,int iBillingCycleID)
{
	long lValidBalance;
	
	loadOweCharge(lAcctID);
	loadAggrChargeByABM(lAcctID,lServID,iBillingCycleID);
	loadAcctBalanceByABM(lAcctID);
	
	lValidBalance = calcFilterBalance(lAcctID,lServID,iBillingCycleID);
	
	return lValidBalance;
}

void FilterSimpleMgr::loadAcctBalanceByABM(long lAcctID)
{
	m_vBalance.clear();
	QueryBalance qb;
	QryBalanceSCmd qbSCmd;
	QryBalanceSResult qbSResult;
  BalanceData strBalanceData;
  BalanceInfoS *pBalanceInfoS;
	vector<BalanceInfoS *> vecBalanceInfoS;
	vector<BalanceInfoS *>::iterator itrBalanceInfoS;
	
	qb.m_lAcctID = lAcctID;
	qb.m_iQueryFlag = 1;
	qb.m_lQueryID = lAcctID;
	qb.m_iQueryState = 1;
	qbSCmd.set(qb);
	
	m_pABM->beginTrans();
	m_pABM->synExecute(&qbSCmd, &qbSResult);
	
	int iRet = qbSResult.getResultCode();
	if (!iRet)
		 {
		   qbSResult.getRecord(vecBalanceInfoS);
		   for(itrBalanceInfoS=vecBalanceInfoS.begin(); itrBalanceInfoS!=vecBalanceInfoS.end(); itrBalanceInfoS++)
		      {
            //不等于说明还有余额可用，等于说明余额已经抵扣完毕，不需要再取出来比较
            pBalanceInfoS = *itrBalanceInfoS;
            if (pBalanceInfoS->m_lBalance != pBalanceInfoS->m_lCharge)
            	 {
            	   strBalanceData.lAcctID = pBalanceInfoS->m_lAcctID;
            	   strBalanceData.lServID = pBalanceInfoS->m_lServID;
            	   strBalanceData.iAcctItemTypeID = pBalanceInfoS->m_iItemGroupID;
            	   strBalanceData.iBalance = pBalanceInfoS->m_lBalance-pBalanceInfoS->m_lCharge;
            	   m_vBalance.push_back(strBalanceData);
            	 }
		      }
		 }
	
	return;
}

//实时欠费中包含后付费用户的帐单，这里不需要，相关接口abm正在开发
void FilterSimpleMgr::loadAggrChargeByABM(long lAcctID,long lServID,int iBillingCycleID)
{
	AcctItemOweData strOweCharge;
	QueryAcctItemCmd qaiCmd;
	QueryAcctItemResult qaiResult;
	AcctItemInfoHB *pAcctItemInfoHB;
	vector<AcctItemInfoHB *> vAcctItemInfoHB;
	vector<AcctItemInfoHB *>::iterator itrAcctItemInfoHB;

  qaiCmd.setAcctQry(lAcctID, 1);  	
  
	m_pABM->beginTrans();
	m_pABM->synExecute(&qaiCmd, &qaiResult);
	
	int iRet = qaiResult.getResultCode();
	if (!iRet)
		 {
		   qaiResult.getRecord(vAcctItemInfoHB);
		   for(itrAcctItemInfoHB=vAcctItemInfoHB.begin(); itrAcctItemInfoHB!=vAcctItemInfoHB.end(); itrAcctItemInfoHB++)
		      {
            //m_lCharge1不等于0说明还有欠费，等于0的已经抵扣完毕，不需要再取出来比较
            pAcctItemInfoHB = *itrAcctItemInfoHB;
            if ((pAcctItemInfoHB->m_lCharge1 != 0) && (pAcctItemInfoHB->m_iBillingMode == 2))
            	 {            	   
            	   strOweCharge.iServID = pAcctItemInfoHB->m_lServID;
            	   strOweCharge.iAcctID = pAcctItemInfoHB->m_lAcctID;
            	   strOweCharge.iAmount = pAcctItemInfoHB->m_lCharge1;
            	   
            	   m_vOweChargeTmp.push_back(strOweCharge);
            	 }
		      }
		 }
	
	return;
}
#endif

long FilterSimpleMgr::mergeOweCharge()
{
	int  iTemp,i,iFound;
	long lCharge = 0;
	AcctItemOweData strTemp;
	
	m_vOweCharge.clear();
	
	for (iTemp=0;iTemp<m_vOweChargeTmp.size();iTemp++)
	{
		iFound = 0;
		lCharge += m_vOweChargeTmp[iTemp].iAmount;
		
		for (i=0;i<m_vOweCharge.size();i++)
		{
			if (m_vOweChargeTmp[iTemp].iServID == m_vOweCharge[i].iServID)
			{
				m_vOweCharge[i].iAmount += m_vOweChargeTmp[iTemp].iAmount;
				iFound = 1;
			}
		}
		
		if (!iFound)
		{
			strTemp.iServID = m_vOweChargeTmp[iTemp].iServID;
			strTemp.iAcctID = m_vOweChargeTmp[iTemp].iAcctID;
			strTemp.iAmount = m_vOweChargeTmp[iTemp].iAmount;
			m_vOweCharge.push_back(strTemp);
		}
	}
	
	return lCharge;
}

long FilterSimpleMgr::calcFilterBalance(long lAcctID,long lServID,int iBillingCycleID)
{
	long lSumCharge,lCurBalance,lItemGroupID=-1,lBalanceServID,lBilledCharge,lValidBalance,lCurCharge;
	BillingCycleMgr bcm;
	BillingCycle *pCycle = NULL;
	Date sys_date;
	char sDate[16];
	int iFirst,iCurBillingCycleID;
	
	lSumCharge =0;lCurBalance = 0;lCurCharge = 0;
	strcpy(sDate,sys_date.toString());
	
	vector <struct BalanceData>::iterator rcBalance;

	lSumCharge = mergeOweCharge();

	sort(m_vOweCharge.begin(),m_vOweCharge.end(),sortServCharge);
	sort(m_vBalance.begin(),m_vBalance.end(),sortServBalance);

	lValidBalance =0;
	
	for(rcBalance= m_vBalance.begin();rcBalance!=m_vBalance.end();rcBalance++)
	{
		lCurBalance=(*rcBalance).iBalance;
		if(lCurBalance <= 0) continue;
		//lItemGroupID = (*rcBalance).lAcctItemGroupID;
		lBalanceServID = (*rcBalance).lServID;
		iCurBillingCycleID = -1;
    
		for(iFirst=0;iFirst<m_vOweCharge.size();iFirst++)
		{
			if (lCurBalance == 0) break;
			if (lSumCharge == 0)break;
			lCurCharge = m_vOweCharge[iFirst].iAmount;
			
			//if (lCurCharge==0) continue;//提高精度
			if (lCurCharge<=0) continue;
/*
			pCycle =  bcm.getBillingCycle(m_strOweCharge[iFirst].iBillingCycleID);
			if(!((strcmp((*rcBalance).sEffDate,pCycle->getStartDate())>=0 &&
				strcmp((*rcBalance).sEffDate,pCycle->getEndDate())<0)||
				(strcmp((*rcBalance).sExpDate,pCycle->getStartDate())>=0 &&
				strcmp((*rcBalance).sExpDate,pCycle->getEndDate())<0)||
				(strcmp(pCycle->getStartDate(),(*rcBalance).sEffDate)>=0 &&
				strcmp(pCycle->getStartDate(),(*rcBalance).sExpDate)<=0)||
				(strcmp(pCycle->getEndDate(),(*rcBalance).sEffDate)>0 &&
				strcmp(pCycle->getEndDate(),(*rcBalance).sExpDate)<0)
				)) continue;
*/
			
			if(lBalanceServID != -1 && lBalanceServID!=m_vOweCharge[iFirst].iServID) continue;
			
			if(lCurBalance>lCurCharge) lBilledCharge = lCurCharge;
				else lBilledCharge = lCurBalance;
							
			lCurBalance -= lBilledCharge;
			(*rcBalance).iBalance -= lBilledCharge;
			m_vOweCharge[iFirst].iAmount -= lBilledCharge;
			lSumCharge -= lBilledCharge;
			
			//iCurBillingCycleID = m_vOweCharge[iFirst].iBillingCycleID;
		}
/*
		pCycle =  bcm.getBillingCycle(iBillingCycleID);
		if(!((strcmp((*rcBalance).sEffDate,pCycle->getStartDate())>=0 &&
				strcmp((*rcBalance).sEffDate,pCycle->getEndDate())<0)||
				(strcmp((*rcBalance).sExpDate,pCycle->getStartDate())>=0 &&
				strcmp((*rcBalance).sExpDate,pCycle->getEndDate())<0)||
				(strcmp(pCycle->getStartDate(),(*rcBalance).sEffDate)>=0 &&
				strcmp(pCycle->getStartDate(),(*rcBalance).sExpDate)<=0)||
				(strcmp(pCycle->getEndDate(),(*rcBalance).sEffDate)>0 &&
				strcmp(pCycle->getEndDate(),(*rcBalance).sExpDate)<0)
			)) continue;
		else */ if ( (lBalanceServID==lServID || lBalanceServID==-1) && lItemGroupID==-1 )
		{
			lValidBalance += lCurBalance;	
		}
	}

	long lRetCharge = 0;           
	            
	for(iFirst=0;iFirst<m_vOweCharge.size();iFirst++)
	{            
		if (m_vOweCharge[iFirst].iServID == lServID && m_vOweCharge[iFirst].iAmount>0 )
			lRetCharge += m_vOweCharge[iFirst].iAmount;
	}         	
	
	if (lValidBalance>0)
	{
		if (lRetCharge <= 0)
		{
			return lValidBalance;
		}
		else
		{
			return -lRetCharge;
		}		
	}
	else
	{   	            
		return -lRetCharge;            
	}	

}

bool FilterSimpleMgr::sortServCharge(const AcctItemOweData & m1,const AcctItemOweData & m2)
{
	return m1.iServID < m2.iServID;
}

bool FilterSimpleMgr::sortServBalance(const BalanceData & m1,const BalanceData & m2)
{
	return m1.lServID > m2.lServID;
}

/*
int main()
{
	FilterSimpleMgr t;
	long lAcctID,lServID;
	long lServBalance,lServCharge,lServAggr;
	long lcnt=0,lFcnt=0;
	
	//t.getFilterBalance(120000632962,123012092557,201010);
	
	DEFINE_QUERY (qry);
	DEFINE_QUERY (qry1);
	char sSQL[4096];
	char sSQL1[4096];
	sprintf (sSQL, " select acct_id, serv_id from acct_item_aggr_xdltest ");
	
	qry.setSQL (sSQL); qry.open ();
	
	Date d0;
	printf("start:%s\n",d0.toString());
	
	while (qry.next ()) {
	
		lAcctID = qry.field (0).asLong ();
		lServID = qry.field (1).asLong ();
	
		lServBalance = t.getFilterBalance(lAcctID,lServID,201010);
	
		if ( lServBalance<5000 )
		{
			lFcnt++;
			
			sprintf (sSQL1, " insert into b_check_stop (acct_id,serv_id) values(:ACCT_ID,:SERV_ID) ");
			qry1.close();
			qry1.setSQL(sSQL1);
			qry1.setParameter("ACCT_ID",lAcctID);
			qry1.setParameter("SERV_ID",lServID);
			qry1.execute();
			qry1.commit();
			
		}
		lcnt++;
		
		if(lcnt%100000 == 0)
		{
			Date d2;
			printf("end:%s,Total:%ld,Filt:%ld \n",d2.toString(),lcnt,lFcnt);
		}
	}
	
	Date d1;
	printf("end:%s,Total:%ld,Filt:%ld \n",d1.toString(),lcnt,lFcnt);
	
	
	qry.close();
	return 0;
}
*/

/*
int main()
{
//测试T600-T700-T90800<1200
	FilterSimpleMgr t;
	Rt_Stop_Make s(1,1);
	long lAcctID,lServID;
	long lServBalance,lServCharge,lServAggr;
	long lcnt=0,lFcnt=0;
	
	//测试表 acct_item_aggr_xdltest
	DEFINE_QUERY (qry);
	DEFINE_QUERY (qry1);
	char sSQL[4096];
	char sSQL1[4096];
	sprintf (sSQL, " select acct_id, serv_id from acct_item_aggr_xdltest ");
	qry.setSQL (sSQL); qry.open ();
	
	Date d0;
	printf("start:%s\n",d0.toString());
	
	while (qry.next ()) {
	
		lAcctID = qry.field (0).asLong ();
		lServID = qry.field (1).asLong ();
	
		lServBalance = t.m_pBalanceMgr->getServBalance(lServID);
		lServCharge = t.m_pOweMgr->getServOwe(lServID);
		lServAggr = s.GetRealCharge(lServID,lAcctID,2,201010,1);
	
		if ( (lServBalance - lServCharge - lServAggr)<=5000 )
		{
			lFcnt++;
			
			sprintf (sSQL1, " insert into b_check_stop (acct_id,serv_id) values(:ACCT_ID,:SERV_ID) ");
			qry1.close();
			qry1.setSQL(sSQL1);
			qry1.setParameter("ACCT_ID",lAcctID);
			qry1.setParameter("SERV_ID",lServID);
			qry1.execute();
			qry1.commit();
		}
		
		lcnt++;
		
		if(lcnt%100000 == 0)
		{
			Date d2;
			printf("end:%s,Total:%ld,Filt:%ld \n",d2.toString(),lcnt,lFcnt);
		}
	}
	
	Date d1;
	printf("end:%s,Total:%ld,Filt:%ld \n",d1.toString(),lcnt,lFcnt);
	
	
	qry.close();
	return 0;
}
*/



