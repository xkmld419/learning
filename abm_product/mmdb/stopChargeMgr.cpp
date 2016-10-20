/*VER: 1*/
#include "stopChargeMgr.h"

/*
#ifndef RT_DEBUG
#define RT_DEBUG
#endif
*/

#if defined DEF_JIANGSU || DEF_HAINAN || DEF_NEIMENG || DEF_QINGHAI || DEF_BEIJING
#define STOP_RETURN_SERV_CHARGE
#endif

extern TOCIDatabase gpDBLink_STOP;

static char * sBalanceMemberSql="SELECT 1 value FROM A_BALANCE_ITEM_GROUP_MEMBER "
"WHERE ITEM_GROUP_ID=:ITEMGROUPID AND ACCT_ITEM_TYPE_ID=:ACCTITEMTYPEID AND STATE='M0A' ";
 
long StopChargeMgr::stopCharge = 0;

StopChargeMgr::StopChargeMgr(char sTableName[])
{
	strcpy(gsPreTableName,sTableName);
}
StopChargeMgr::~StopChargeMgr()
{
}

long StopChargeMgr::getAcctBalance(long lAcctID,long lServID,int iLevel)
{
	//DEFINE_QUERY( qry2);
	vector<struct strStopBalanceNew>::iterator itr;
	long lBalance = 0;
	for(itr=m_strBalanceNew.begin();itr!=m_strBalanceNew.end();itr++)
	{
		if (iLevel ==1 )
		{
		}
		if (iLevel == 2)
		{
			if ((*itr).lServID != lServID) continue;
		}
		if (iLevel == 3)
		{
			if ((*itr).lServID != -1 ) continue;
		}
		lBalance += (*itr).lBalance;
	}

	return lBalance;
}

void StopChargeMgr::loadAcctBalance(long lAcctID)
{
   DEFINE_QUERY (qry) ;
   char sSql[2048];
   struct strStopBalanceNew tempStopBalance;
   char sBalanceType[1000];
   
   if (!ParamDefineMgr::getParam ("RT_MAKE_STOP","NO_BALANCE_TYPE", sBalanceType) )
        strcpy(sBalanceType,"0");
    
   AllTrim(sBalanceType);
   
   m_strBalanceNew.clear();
   sprintf(sSql ,"select ACCT_BALANCE_ID,nvl(serv_id,-1) SERV_ID,nvl(item_group_id,-1) item_group_id,"
   "nvl(balance,0) BALANCE,A.BALANCE_TYPE_ID,B.ADJUST_FLAG,"
   " nvl(cycle_upper,-1) cycle_upper,nvl(to_char(eff_date,'yyyymmddhh24miss'),'20000101000000') eff_date,"
   "nvl(to_char(exp_date,'yyyymmddhh24miss'),'20900101000000') exp_date "
   " from acct_balance a,balance_type b,a_balance_object_type c where acct_id=:ACCTID and state='10A' "
   " and a.balance_type_id not in ( %s ) "
   " and a.balance_type_id=b.balance_type_id "
   " and a.balance<>0 "
   " and a.object_type_id=c.object_type_id "
   " order by nvl(a.cycle_upper,-1) desc,c.priority,b.priority ",sBalanceType);
   //这里order by要和账务保持一致, xudl 2009.09.23 suzhou
   qry.setSQL (sSql);
   qry.setParameter("ACCTID",lAcctID);
   //qry.setParameter("SBALANCETYPE",sBalanceType);
   qry.open ();

   while(qry.next ())
   {
   	 tempStopBalance.lAcctBalanceID = qry.field("ACCT_BALANCE_ID").asLong();
   	 tempStopBalance.lAcctID = lAcctID;
   	 tempStopBalance.lServID = qry.field("SERV_ID").asLong();
   	 tempStopBalance.lAcctItemGroupID = qry.field("ITEM_GROUP_ID").asLong();
   	 tempStopBalance.lBalance  = qry.field("BALANCE").asLong();
   	 tempStopBalance.lUpper   = qry.field("CYCLE_UPPER").asLong();
   	 strcpy(tempStopBalance.sEffDate ,qry.field("EFF_DATE").asString());
   	 strcpy(tempStopBalance.sExpDate ,qry.field("EXP_DATE").asString());
   	 tempStopBalance.iBalanceType = qry.field("BALANCE_TYPE_ID").asInteger();
   	 strcpy(tempStopBalance.sAdjustFlag ,qry.field("ADJUST_FLAG").asString());
   	 m_strBalanceNew.push_back(tempStopBalance);
   	 
   }
   
   qry.close ();
   
   #ifdef DEF_SICHUAN
      //增加帐户信用度
    	int iDefaultValue,lCredit;
    	iDefaultValue = 0;
    	//需要对默认信用度修改,暂时放在这里先不动

    	qry.close();
    	qry.setSQL("select nvl(sum(credit_amount),:DEFAULT_VALUE) from acct_credit where acct_id = :ACCT_ID "
    	" AND sysdate BETWEEN nvl(eff_date,sysdate-1) and nvl(exp_date,sysdate+1) ");
    	qry.setParameter("ACCT_ID",lAcctID);
    	qry.setParameter("DEFAULT_VALUE",iDefaultValue);
    	qry.open();
    	if (!qry.next())
    		lCredit = iDefaultValue;
    	else lCredit = qry.field(0).asInteger();
    		
    	tempStopBalance.lAcctID = lAcctID;
   	  tempStopBalance.lServID = -1;
   	  tempStopBalance.lAcctItemGroupID = 0;
   	  tempStopBalance.lBalance  = lCredit;
   	  tempStopBalance.lUpper   = 0;
   	  strcpy(tempStopBalance.sEffDate,"20000101000000");
   	  strcpy(tempStopBalance.sExpDate,"20900101000000");
   	  m_strBalanceNew.push_back(tempStopBalance);
      qry.close();
   #endif
    
}

void StopChargeMgr::loadOweCharge(long lAcctID)
{
  
    char sSql[1200] = {0};
    char sSqlEx[100]= {0};
	char sTableName [30][30] = {0};
	int  i = 0;
    struct strOweChargeNew tempOweCharge;
	
	DEFINE_QUERY (qry) ;
    TOCIQuery qryEx(&gpDBLink_STOP);
	
	sprintf(sSql, "SELECT B.TABLE_NAME,A.ACCT_ID FROM A_ACCT_OWE_STATE A,A_SPLIT_TABLE_CONFIG B "
					"WHERE A.STATE = '10A' and  A.ACCT_ID =:ACCT_ID AND A.BILLING_CYCLE_ID = B.BILLING_CYCLE_ID AND B.TABLE_TYPE = 1");	
	
	qry.close();
	qry.setSQL (sSql);
    qry.setParameter("ACCTID", lAcctID);
    qry.open ();
    while (qry.next())
    {
    	strcpy(sTableName[i++], qry.field("TABLE_NAME").asString());
    }
	while (--i >= 0)
	{		
    	#ifdef DEF_SICHUAN
    	sprintf(sSql ,"select serv_id,billing_cycle_id,acct_item_type_id,nvl(sum(nvl(amount,0)),0) amount "
      				" from %s where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
					" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
					" and ACCT_ID=:ACCTID AND (ACCT_ITEM_TYPE_ID NOT IN (SELECT nvl(ACCT_ITEM_TYPE_ID,0) FROM ACCT_ITEM_TYPE WHERE nvl(party_role_id,0)=99)) "
					" group by billing_cycle_id,serv_id ,acct_item_type_id", sTableName[i]);
    	#else
     	sprintf(sSql ,"select serv_id,billing_cycle_id,acct_item_type_id,nvl(sum(nvl(amount,0)),0) amount "
     				 " from %s where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
					" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
					" and ACCT_ID=:ACCTID group by billing_cycle_id,serv_id,acct_item_type_id ", sTableName[i]);
    	#endif
    	//这里group by要和账务保持一致, xudl 2009.09.23 suzhou
    	qryEx.close();
    	qryEx.setSQL (sSql);
    	qryEx.setParameter("ACCTID", lAcctID);
    	qryEx.open ();

    	while(qryEx.next ())
    	{
   	    	tempOweCharge.lAcctID = lAcctID;
   	    	tempOweCharge.lServID = qryEx.field("SERV_ID").asLong();
   	    	tempOweCharge.iBillingCycleID = qryEx.field("BILLING_CYCLE_ID").asLong();
   	    	tempOweCharge.iAcctItemTypeID = qryEx.field("ACCT_ITEM_TYPE_ID").asInteger();
   	    	tempOweCharge.lCharge = qryEx.field("AMOUNT").asLong();
   	    	tempOweCharge.lBilledCharge = 0;
   	    	//0记录不参与计算，xudl 2009.09.23 suzhou
   	    	if (tempOweCharge.lCharge != 0)
   	    		m_strOweCharge.push_back(tempOweCharge);
    	}
    }
    qryEx.close ();
    qry.close ();
}


long StopChargeMgr::getHistoryOweCharge(long lAcctID,long lServID,char sPreState[],int iBillingCycleID)
{
	vector<struct strOweChargeNew>::iterator itr;
	long lOweCharge = 0;
	Date sys_date;
	char sDate[16];
	
	strcpy(sDate,sys_date.toString());
	
	for(itr=m_strOweCharge.begin();itr!=m_strOweCharge.end();itr++)
	{
	   #ifdef DEF_HAINAN
	   if (sPreState[2] == 'D' && (*itr).iBillingCycleID == iBillingCycleID)
	   {
	    if (!(G_PUSERINFO->getServ (G_SERV, (*itr).lServID, sDate))) {
			
		Log::log (91, "取历史欠费没有找到SERVID:%ld %s", (*itr).lServID, sDate);
		return 0;
	   }
           if (atoi(G_PSERV->getAttr (ID_BILLING_MODE)) != 2 ) continue;
           }
	  #endif
	  
	   if (lAcctID == -1)
	   {
		if ((*itr).lServID != lServID) continue;
	   }
	   lOweCharge += (*itr).lCharge;
	}
    return lOweCharge;
}

void StopChargeMgr::loadRtCharge(long lAcctID)
{
    DEFINE_QUERY (qry) ;
    char sSql[2048];
    struct strOweChargeNew tempOweCharge;
//预付费历史欠费只取欠费、呆、坏帐
    //m_strOweCharge.clear();
    #ifdef DEF_SICHUAN
    sprintf(sSql ,"select serv_id,billing_cycle_id,acct_item_type_id,nvl(sum(nvl(amount,0)),0) amount "
      " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
			" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
			" and ACCT_ID=:ACCTID AND (ACCT_ITEM_TYPE_ID NOT IN (SELECT nvl(ACCT_ITEM_TYPE_ID,0) FROM ACCT_ITEM_TYPE WHERE nvl(party_role_id,0)=99)) "
			" group by serv_id,billing_cycle_id ,acct_item_type_id");
    #else
     sprintf(sSql ,"select serv_id,billing_cycle_id,acct_item_type_id,nvl(sum(nvl(amount,0)),0) amount "
      " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
			" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
			" and ACCT_ID=:ACCTID "
			" group by serv_id,billing_cycle_id,acct_item_type_id ");
    #endif
    qry.setSQL (sSql);
    qry.setParameter("ACCTID",lAcctID);
    qry.open ();

    while(qry.next ())
    {
   	    tempOweCharge.lAcctID = lAcctID;
   	    tempOweCharge.lServID = qry.field("SERV_ID").asLong();
   	    tempOweCharge.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asLong();
   	    tempOweCharge.iAcctItemTypeID = qry.field("ACCT_ITEM_TYPE_ID").asInteger();
   	    tempOweCharge.lCharge = qry.field("AMOUNT").asLong();
   	    tempOweCharge.lBilledCharge = 0;
   	    m_strOweCharge.push_back(tempOweCharge);
    }
    qry.close ();
    return;
}

void StopChargeMgr::loadAggrCharge(long lServID,long lAcctID,int iBillingCycleID)
{
	struct strOweChargeNew tempOweCharge;
	vector<AcctItemData *> vAggrData;
	vector<ServAcctInfo> vPayInfo;
	vector<ServAcctInfo>::iterator iterPay;
	//vector<struct strOweChargeNew>::iterator itrOweCharge;
	vector<struct AcctItemData *>::iterator itrAggrData;
	//HashList<int> oHashList (10000);
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
	
	//增加serv_id排序，xudl 2009.09.23 suzhou
	sort(vPayInfo.begin(),vPayInfo.end(),sortServAcct);
	
	for (iterPay=vPayInfo.begin(); iterPay!=vPayInfo.end(); iterPay++) {
		#ifdef DEF_SICHUAN
		  iPartyRoleID = 99;
		#else
		  iPartyRoleID = 0;
		#endif
		
		//以下判断是否预付费 xudl 2009.09.23 suzhou
		if (!(G_PUSERINFO->getServ (ServInfo, (*iterPay).m_lServID, d.toString()))) {
			//用户资料没有找到
			Log::log (91, "没有找到SERVID:%ld %s", (*iterPay).m_lServID, d.toString());
			continue;
		}

		iBillingModeID = atoi(ServInfo.getAttr (ID_BILLING_MODE));

		if (iBillingModeID==0)
			iBillingModeID = ServInfo.getServInfo ()->m_iBillingMode;

		if (iBillingModeID!=2) continue;
		//不是预付费，不纳入欠费
		
		if(sState[2]=='R'||sState[2]=='A'||sState[2]=='B')
		{
			GetDBPartyRoleCharge(lAcctID,(*iterPay).m_lServID,iPreBillingCycle,iPartyRoleID);
		}
		
		//增加数据清空，xudl 2009.09.23 suzhou
		vAggrData.clear();
		int iReturn  = G_PAGGRMGR->getAllItemAggr((*iterPay).m_lServID,iBillingCycleID,vAggrData);
		
		if (!iReturn) continue;
		for(itrAggrData=vAggrData.begin();itrAggrData!=vAggrData.end();itrAggrData++)
		{
			tempOweCharge.lAcctID = lAcctID;
			tempOweCharge.lServID = (*iterPay).m_lServID;
			tempOweCharge.iBillingCycleID = (*itrAggrData)->m_iBillingCycleID;
			tempOweCharge.iAcctItemTypeID = (*itrAggrData)->m_iPayItemTypeID;
			tempOweCharge.lCharge  = (*itrAggrData)->m_lValue;
			tempOweCharge.lBilledCharge = 0;
			
			//0记录不参与计算，xudl 2009.09.23 suzhou
			if (tempOweCharge.lCharge != 0)
				m_strOweCharge.push_back(tempOweCharge);
		}//for(itrOweCharge)
		
		
	}//for(iterPay)
					
	
}

long StopChargeMgr::getBillBalance(long lAcctID,long lServID,int iBillingCycleID,long * plServItemBalance,long * plAcctItemBalance)
{
	long lValidBalance;
	long lServItemBalance,lAcctItemBalance;
	
	m_strOweCharge.clear();
	
	//取出历史欠费
	loadOweCharge(lAcctID);
	//取出当月欠费
	loadAggrCharge(lServID,lAcctID,iBillingCycleID);
	//取出余额
	loadAcctBalance(lAcctID);
	//第一步做对冲,负的费用冲掉
	
	//取出余额,作循环比对
	lValidBalance = BalanceCompare(lAcctID,lServID,iBillingCycleID,&lServItemBalance,&lAcctItemBalance);
	*plServItemBalance = lServItemBalance;
	*plAcctItemBalance = lAcctItemBalance;
	
	return lValidBalance;
}

long StopChargeMgr::BalanceCompare(long lAcctID,long lServID,int iBillingCycleID,long * plServItemBalance,long * plAcctItemBalance)
{
	long lSumCharge,lCurBalance,lItemGroupID,lBalanceServID,lUsableBalance,lPayedBalance,lBilledCharge,lValidBalance,lUpperLimit,lCurCharge;
	BillingCycleMgr bcm;
	BillingCycle *pCycle = NULL;
	Date sys_date;
	char sDate[16];
	int iFirst,iCurBillingCycleID;
	long lCurUpperBalance = 0;///存放当前帐期最终的余额上限
	
	lSumCharge =0;lCurBalance = 0;lCurCharge = 0;
	strcpy(sDate,sys_date.toString());
	
	vector <struct strStopBalanceNew>::iterator rcBalance;
	#ifdef RT_DEBUG
	cout<<"余额变更前"<<endl;
	printBalance();
	//先作费用抵扣
	cout<<"费用冲抵前"<<endl;
	printOweCharge();
	#endif
	counteractNegativeItemID(lSumCharge);
	#ifdef RT_DEBUG
	cout<<"费用冲抵后"<<endl;
	printOweCharge();
	#endif
	lValidBalance =0;
	lPayedBalance =0;
	lUpperLimit = 0;
	lUsableBalance = 0;
	
	#ifdef RT_DEBUG
	cout<<"帐户总欠费："<<lSumCharge<<endl;
	cout<<"************************"<<endl;
	cout<<"开始抵扣余额"<<endl;
	#endif
	
	for(rcBalance= m_strBalanceNew.begin();rcBalance!=m_strBalanceNew.end();rcBalance++)
	{
		lCurBalance=(*rcBalance).lBalance;
		if(lCurBalance <= 0) continue;
		lUpperLimit = (*rcBalance).lUpper;
		lItemGroupID = (*rcBalance).lAcctItemGroupID;
		lBalanceServID = (*rcBalance).lServID;
		iCurBillingCycleID = -1;
		lCurUpperBalance = lUpperLimit; //记录当前帐期的帐期上限
    
		for(iFirst=0;iFirst<m_strOweCharge.size();iFirst++)
		{
			//比较余额有效期,逻辑为余额的生效时间落在帐期范围或失效时间落在帐期范围
			//或者帐期的开始、结束时间落在余额的有效期范围,均视为有效
			if (lCurBalance == 0) break;
			if (lSumCharge == 0)break;
			lCurCharge = m_strOweCharge[iFirst].lCharge - m_strOweCharge[iFirst].lBilledCharge;
			
			if (lCurCharge==0) continue;
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
			//判断帐目组
			if(	lItemGroupID != -1)
			{
				if(!getBalanceItemMember(lItemGroupID,m_strOweCharge[iFirst].iAcctItemTypeID)) continue;
			}
			//判断用户
			if(lBalanceServID != -1 && lBalanceServID!=m_strOweCharge[iFirst].lServID) continue;
			
			if(lUpperLimit != -1)
			{
				if(iCurBillingCycleID!=m_strOweCharge[iFirst].iBillingCycleID)
				{
					//getCyclePayoutBalance(iBillingCycleID,(*rcBalance).lAcctBalanceID,lPayedBalance);
					getCyclePayoutBalance(m_strOweCharge[iFirst].iBillingCycleID,(*rcBalance).lAcctBalanceID,lPayedBalance);
				}
				lUsableBalance = lUpperLimit - lPayedBalance;
				if (lUsableBalance <= 0) continue;
				if (lUsableBalance>lCurBalance) lUsableBalance=lCurBalance;
				if (lUsableBalance > lCurCharge) lBilledCharge = lCurCharge;
					else lBilledCharge = lUsableBalance;
				lPayedBalance += lBilledCharge;
				///对当前帐期的余额上限做处理
				if (m_strOweCharge[iFirst].iBillingCycleID == iBillingCycleID)
				{
					lCurUpperBalance -= lBilledCharge;
				}					
			}
			else
			{
				if(lCurBalance>lCurCharge) lBilledCharge = lCurCharge;
					else lBilledCharge = lCurBalance;
				
			}
			lCurBalance -= lBilledCharge;
			(*rcBalance).lBalance -= lBilledCharge;
			m_strOweCharge[iFirst].lBilledCharge = m_strOweCharge[iFirst].lBilledCharge+lBilledCharge;
			lSumCharge = lSumCharge - lBilledCharge;
			
			iCurBillingCycleID = m_strOweCharge[iFirst].iBillingCycleID;
		}
		#ifdef RT_DEBUG
		cout<<"抵扣后，欠费变更为"<<endl;
		printOweCharge();
		cout<<"余额变更为"<<endl;
		printBalance();
		#endif
		//在这里需要加入对当前时间的判断.这样的余额才有效.才可以进行相加
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
		else if ( (lBalanceServID==lServID || lBalanceServID==-1) && lItemGroupID==-1 )
		{
			if (lUpperLimit != -1) lValidBalance += (lCurUpperBalance>lCurBalance?lCurBalance:lCurUpperBalance);
			  	else lValidBalance += lCurBalance;	
			//lValidBalance += lCurBalance;
		}	
			//用户+帐户，且非账目组 xudl 2009.09.23 suzhou
	}

/*加入用户、帐户帐户组余额的计算 xudl 2009.11.12 suzhou*/
	long lServItemBalance=0,lAcctItemBalance=0;
	pCycle =  bcm.getBillingCycle(iBillingCycleID);
	for(rcBalance= m_strBalanceNew.begin();rcBalance!=m_strBalanceNew.end();rcBalance++)
	{
		
		if(!((strcmp((*rcBalance).sEffDate,pCycle->getStartDate())>=0 &&
		 strcmp((*rcBalance).sEffDate,pCycle->getEndDate())<0)||
		 (strcmp((*rcBalance).sExpDate,pCycle->getStartDate())>=0 &&
		 strcmp((*rcBalance).sExpDate,pCycle->getEndDate())<0)||
		 (strcmp(pCycle->getStartDate(),(*rcBalance).sEffDate)>=0 &&
		 strcmp(pCycle->getStartDate(),(*rcBalance).sExpDate)<=0)||
		 (strcmp(pCycle->getEndDate(),(*rcBalance).sEffDate)>0 &&
		 strcmp(pCycle->getEndDate(),(*rcBalance).sExpDate)<0)
		)) continue;
		
		if ( (*rcBalance).lServID==lServID && (*rcBalance).lAcctItemGroupID!=-1 )
			lServItemBalance += (*rcBalance).lBalance;
		if ( (*rcBalance).lServID==-1 && (*rcBalance).lAcctItemGroupID!=-1 )
			lAcctItemBalance += (*rcBalance).lBalance;
	}
	*plServItemBalance = lServItemBalance;
	*plAcctItemBalance = lAcctItemBalance;

//江苏、北京、海南、内蒙、青海
#ifdef STOP_RETURN_SERV_CHARGE
	long lRetCharge = 0;           
	            
	for(iFirst=0;iFirst<m_strOweCharge.size();iFirst++)
	{            
		if (m_strOweCharge[iFirst].lServID == lServID)
			lRetCharge += m_strOweCharge[iFirst].lCharge - m_strOweCharge[iFirst].lBilledCharge;
	}         	
	
	if (lValidBalance>0)
	{
		if (lRetCharge <= 0)
		{
			return lValidBalance;
		}
		else
		{
			//return lValidBalance-lRetCharge;
			return -lRetCharge;//符合账务前台显示逻辑
		}		
	}	
	else
	{   	            
		return -lRetCharge;            
	}	
#endif

	if(lSumCharge>0) return -lSumCharge;
	else return lValidBalance;
}

void StopChargeMgr::counteractNegativeItemID(long &lSumCharge)
{
  int iFirst,iSecond,iTemp,iType;
  long iSrcServID,lSrcCharge;
  int iSrcBillingCycleID,iSrcAcctItemTypeID;
  //对历史欠费作抵扣
	for(iFirst=0;iFirst<m_strOweCharge.size();iFirst++)
	{
		//lSumCharge = lSumCharge + m_strOweCharge[iFirst].lCharge-m_strOweCharge[iFirst].lBilledCharge;
		if(m_strOweCharge[iFirst].lCharge<0)
		{
			iSrcServID = m_strOweCharge[iFirst].lServID;
			iSrcBillingCycleID = m_strOweCharge[iFirst].iBillingCycleID;
			iSrcAcctItemTypeID = m_strOweCharge[iFirst].iAcctItemTypeID;
			lSrcCharge = abs(m_strOweCharge[iFirst].lCharge);
			for (iType=1;iType<=4;iType++)
			{
  			for(iSecond=0;iSecond<m_strOweCharge.size();iSecond++)
  			{
  				if(m_strOweCharge[iSecond].lCharge<0) continue;
  			  switch(iType)
  			  {
  			  	case 1:///同帐期同用户同帐目抵扣
  			  		if(m_strOweCharge[iSecond].lServID==iSrcServID && 
  			  			m_strOweCharge[iSecond].iBillingCycleID == iSrcBillingCycleID &&
  			  			m_strOweCharge[iSecond].iAcctItemTypeID == iSrcAcctItemTypeID)
  			  		{
  			  			if(lSrcCharge<m_strOweCharge[iSecond].lCharge)
  			  			{
  			  				m_strOweCharge[iSecond].lCharge = m_strOweCharge[iSecond].lCharge-lSrcCharge;
  			  				lSrcCharge =0;
  			  				m_strOweCharge[iFirst].lCharge = 0;
  			  			}
  			  			else
  			  			{
  			  				lSrcCharge = lSrcCharge - m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iFirst].lCharge = m_strOweCharge[iFirst].lCharge + m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iSecond].lCharge = 0;
  			  			}
  			  		}//if(m_str...)
  			  		break;
  			  	case 2:///同帐期同用户
  			  		if(m_strOweCharge[iSecond].lServID==iSrcServID && 
  			  			m_strOweCharge[iSecond].iBillingCycleID == iSrcBillingCycleID )
  			  		{
  			  			if(lSrcCharge<m_strOweCharge[iSecond].lCharge)
  			  			{
  			  				m_strOweCharge[iSecond].lCharge = m_strOweCharge[iSecond].lCharge-lSrcCharge;
  			  				lSrcCharge =0;
  			  				m_strOweCharge[iFirst].lCharge = 0;
  			  			}
  			  			else
  			  			{
  			  				lSrcCharge = lSrcCharge - m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iFirst].lCharge = m_strOweCharge[iFirst].lCharge + m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iSecond].lCharge = 0;
  			  			}
  			  		}//if(m_str...)
  			  		break;
  			  	case 3://同帐期
  			  		if(m_strOweCharge[iSecond].iBillingCycleID == iSrcBillingCycleID)
  			  		{
  			  			if(lSrcCharge<m_strOweCharge[iSecond].lCharge)
  			  			{
  			  				m_strOweCharge[iSecond].lCharge = m_strOweCharge[iSecond].lCharge-lSrcCharge;
  			  				lSrcCharge =0;
  			  				m_strOweCharge[iFirst].lCharge = 0;
  			  			}
  			  			else
  			  			{
  			  				lSrcCharge = lSrcCharge - m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iFirst].lCharge = m_strOweCharge[iFirst].lCharge + m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iSecond].lCharge = 0;
  			  			}
  			  		}//if(m_str...)
  			  		break;
  			  	case 4://从最早欠费帐期对抵剩余正帐单
  			  		if(lSrcCharge<m_strOweCharge[iSecond].lCharge)
  			  			{
  			  				m_strOweCharge[iSecond].lCharge = m_strOweCharge[iSecond].lCharge-lSrcCharge;
  			  				lSrcCharge =0;
  			  				m_strOweCharge[iFirst].lCharge = 0;
  			  			}
  			  			else
  			  			{
  			  				lSrcCharge = lSrcCharge - m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iFirst].lCharge = m_strOweCharge[iFirst].lCharge + m_strOweCharge[iSecond].lCharge;
  			  				m_strOweCharge[iSecond].lCharge = 0;
  			  			}
  			  			break;
  			  	default:
  			  		break;
  			  }//switch(iType)...
  			  if(lSrcCharge==0) break;
  			}//for(iSecond...)
  			//if(lSrcCharge==0){
  			//  	 m_strOweCharge[iFirst].lCharge = 0;
  			//  	 break;
  			//}
		 }//for(iType...)
		}//if(m_str...)
	}//for(iFirst...)
	
	//取新的 xudl 2009.09.23 suzhou
	for(iFirst=0;iFirst<m_strOweCharge.size();iFirst++)
	{
		lSumCharge = lSumCharge + m_strOweCharge[iFirst].lCharge-m_strOweCharge[iFirst].lBilledCharge;
	}
	
	stopCharge = lSumCharge;
}


bool StopChargeMgr::getBalanceItemMember(long lAcctItemGroupID,int  iAcctItemTypeID)
{
	DEFINE_QUERY (qry) ;
	qry.close();
  qry.setSQL(sBalanceMemberSql);
  qry.setParameter("ITEMGROUPID",lAcctItemGroupID);
  qry.setParameter("ACCTITEMTYPEID",iAcctItemTypeID);
  qry.open();
  if(!qry.next()) {qry.close();return false;}
  	else {qry.close();return true;}
}

void StopChargeMgr::getCyclePayoutBalance(int iBillingCycleID,long lAcctBalanceID,long & lPayedBalance)
{
	DEFINE_QUERY (qry) ;
	char sSql[1024];
	sprintf(sSql," SELECT NVL(SUM(BP.AMOUNT),0) PAY_AMOUNT "
	" FROM BALANCE_PAYOUT BP "
	" WHERE BP.ACCT_BALANCE_ID = :lAcctBalanceID "
	" AND BP.BILLING_CYCLE_ID = :lBillingCycleID "
	" AND BP.OPER_TYPE in ('5UI','5UJ','5UL','5UM') "
	"  AND BP.STATE = '00A' "	);
	qry.close();
	qry.setSQL(sSql);
	qry.setParameter("lAcctBalanceID",lAcctBalanceID);
	qry.setParameter("lBillingCycleID",iBillingCycleID);
	qry.open();
	if(!qry.next()) lPayedBalance =0;
		else lPayedBalance = qry.field("PAY_AMOUNT").asLong();
	qry.close();
}

void StopChargeMgr::GetDBPartyRoleCharge(long lAcctID,long lServID,int iBillingCycleID,int iPartyRoleID)
{
	TOCIQuery qry(&gpDBLink_STOP);
	//DEFINE_QUERY (qry);
	struct strOweChargeNew tempOweCharge;
	char	sSql[2000];

  if(iPartyRoleID==0)
  {
  	sprintf(sSql,"SELECT * FROM  %s "
	" where SERV_ID= :SERV_ID "
	" and billing_cycle_id= :BILLING_CYCLE_ID "
	" and charge<>0 "
	,gsPreTableName);
  }
  else
  {
	  sprintf(sSql,"SELECT * FROM  %s "
	    " where SERV_ID= :SERV_ID "
	    " and billing_cycle_id= :BILLING_CYCLE_ID "
	    " and charge<>0 "
	    " AND ACCT_ITEM_TYPE_ID IN (SELECT nvl(ACCT_ITEM_TYPE_ID,0) FROM ACCT_ITEM_TYPE WHERE nvl(party_role_id,0)<> :PARTY_ROLE_ID ) "
	    ,gsPreTableName);
  }
  //增加账期判断 xudl 2009.09.23 suzhou
	qry.close();
	qry.setSQL(sSql);
	
	qry.setParameter("SERV_ID",lServID);
	qry.setParameter("BILLING_CYCLE_ID",iBillingCycleID);
	if(iPartyRoleID != 0)
		qry.setParameter("PARTY_ROLE_ID",iPartyRoleID);
	
	qry.open();
	while(qry.next ())
  {
   	    tempOweCharge.lAcctID = lAcctID;
   	    tempOweCharge.lServID = qry.field("SERV_ID").asLong();
   	    tempOweCharge.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asLong();
   	    tempOweCharge.iAcctItemTypeID = qry.field("ACCT_ITEM_TYPE_ID").asInteger();
   	    tempOweCharge.lCharge = qry.field("CHARGE").asLong();
   	    tempOweCharge.lBilledCharge = 0;
   	    m_strOweCharge.push_back(tempOweCharge);
   }
   qry.close ();
}

bool StopChargeMgr::sortServAcct(const ServAcctInfo & m1,const ServAcctInfo & m2)
{
	return m1.m_lServID < m2.m_lServID;
}

void StopChargeMgr::printOweCharge()
{
	int iFirst;
	//cout<<"owecharge value:"<<endl;
	cout<<"servid   acctid  billingcycleid  acctitemtypeid  charge  billedcharge"<<endl;
	for(iFirst=0;iFirst<m_strOweCharge.size();iFirst++)
	{
		cout<<m_strOweCharge[iFirst].lServID<<"  ";
		cout<<m_strOweCharge[iFirst].lAcctID<<"  ";
		cout<<m_strOweCharge[iFirst].iBillingCycleID<<"  ";
		cout<<m_strOweCharge[iFirst].iAcctItemTypeID<<"  ";
		cout<<m_strOweCharge[iFirst].lCharge<<"  ";
		cout<<m_strOweCharge[iFirst].lBilledCharge<<endl;
	}
}

void StopChargeMgr::printBalance()
{
	int iFirst;
	//cout<<"owecharge value:"<<endl;
	cout<<"acct_balance_id   serv_id  acct_id  balance  eff_date  exp_date  item_group"<<endl;
	for(iFirst=0;iFirst<m_strBalanceNew.size();iFirst++)
	{
		cout<<m_strBalanceNew[iFirst].lAcctBalanceID<<"  ";
		cout<<m_strBalanceNew[iFirst].lServID<<"  ";
		cout<<m_strBalanceNew[iFirst].lAcctID<<"  ";
		cout<<m_strBalanceNew[iFirst].lBalance<<"  ";
		cout<<m_strBalanceNew[iFirst].sEffDate<<"  ";
		cout<<m_strBalanceNew[iFirst].sExpDate<<"  ";
		cout<<m_strBalanceNew[iFirst].lAcctItemGroupID<<endl;
	}
}
