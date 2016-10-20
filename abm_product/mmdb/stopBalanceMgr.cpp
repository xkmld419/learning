/*VER: 3*/ 
#include "stopBalanceMgr.h"

//#define CREDIT_DEFAULT_VALUE1 (ParamDefineMgr::getLongParam("RT_MAKE_STOP", "CREDIT_DEFAULT_VALUE"))
	
StopBalanceMgr::StopBalanceMgr()
{
}
StopBalanceMgr::~StopBalanceMgr()
{
}

long StopBalanceMgr::getAcctBalance(long lAcctID,long lServID,int iLevel,int iCredit)
{
	//DEFINE_QUERY( qry2);
	vector<struct strStopAcctBalance>::iterator itr;
	long lBalance = 0,lOweCharge = 0;
	for(itr=m_strBalance.begin();itr!=m_strBalance.end();itr++)
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
		#ifdef DEF_SICHUAN	//只有四川判断
			if (iCredit == 0)//信用度不需要需要参与计算,过滤信用度
			{
				if ((*itr).isCreidt) continue;//是信用度不返回
			}	
		#endif 	
		}
		lBalance += (*itr).lBalance;
	}
/*	
	if (iLevel == 1){
		qry2.close();
  	qry2.setSQL(" select nvl(sum(amount),0) from acct_item_owe "
  	                  " where acct_id=:v_acct_id and state='5JA' ");
  	qry2.setParameter("v_acct_id",lAcctID);
  	      
  	qry2.open();
  	qry2.next();
  	lOweCharge= qry2.field(0).asLong();
  	qry2.close();
	}
	if (iLevel == 2){
		
		qry2.close();
    qry2.setSQL(" select nvl(sum(amount),0) from acct_item_owe "
                " where serv_id=:v_serv_id and state='5JA' ");
    qry2.setParameter("v_serv_id",lServID);
    
    qry2.open();
    qry2.next();
    lOweCharge= qry2.field(0).asLong();
    qry2.close();
	}
	if (iLevel == 3){
		qry2.close();
    qry2.setSQL(" select nvl(sum(amount),0) from acct_item_owe "
                " where acct_id=:v_acct_id and state='5JA' ");
    qry2.setParameter("v_acct_id",lAcctID);
    
    qry2.open();
    qry2.next();
    lOweCharge= qry2.field(0).asLong();
    qry2.close();
	}
*/
	return (lBalance - lOweCharge);
}

void StopBalanceMgr::loadAcctBalance(long lAcctID,int iBillingCnt)
{
   DEFINE_QUERY (qry) ;
   char sSql[2048];
   struct strStopAcctBalance tempStopBalance;
   char sBalanceType[1000];
   int iCreditFlag = 0;
   
   #ifdef DEF_SICHUAN
     iCreditFlag = 1;
   #endif
   
   #ifdef DEF_SHANDONG
     iCreditFlag = 1;
   #endif
   
   if (!ParamDefineMgr::getParam ("RT_MAKE_STOP","NO_BALANCE_TYPE", sBalanceType) )
        strcpy(sBalanceType,"0");
    
   AllTrim(sBalanceType);
   
   m_strBalance.clear();
   #ifdef DEF_HAINAN
   //海南增加押金的余额帐本不参与停机判断,写死ITEM_GROUP_ID=20001 and OBJECT_TYPE_ID<>3
   sprintf(sSql ,"select nvl(serv_id,-1) SERV_ID,nvl(item_group_id,-1) item_group_id,"
   "nvl(sum(nvl(decode(cycle_upper,Null,balance,decode(sign(cycle_upper*%d-balance),-1,cycle_upper*%d,balance)),0) ),0) BALANCE,"
   " cycle_upper,nvl(to_char(eff_date,'yyyymmddhh24miss'),'20000101000000') eff_date,"
   "nvl(to_char(exp_date,'yyyymmddhh24miss'),'20900101000000') exp_date "
   " from acct_balance where acct_id=%ld and state='10A' "
   " and (ITEM_GROUP_ID	<>20001 or OBJECT_TYPE_ID<>3)"
   " and (exists ( select * from billing_cycle where billing_cycle_id "
   " in (select billing_cycle_id from billing_cycle where "
   " billing_cycle_type_id <> 0 and "
   " state in ('10R','10A','10B','10D')) and   to_char(cycle_begin_date,'yyyymm') "
   " between to_char(nvl(eff_date,sysdate-1000),'yyyymm') "
   " and to_char(nvl(exp_date,sysdate+1000),'yyyymm') ) ) "
   " group by serv_id,item_group_id,cycle_upper,eff_date,exp_date",iBillingCnt,iBillingCnt,lAcctID);
   #else
   sprintf(sSql ,"select nvl(serv_id,-1) SERV_ID,nvl(item_group_id,-1) item_group_id,"
   "nvl(sum(nvl(decode(cycle_upper,Null,balance,decode(sign(cycle_upper*%d-balance),-1,cycle_upper*%d,balance)),0) ),0) BALANCE,"
   " cycle_upper,nvl(to_char(eff_date,'yyyymmddhh24miss'),'20000101000000') eff_date,"
   "nvl(to_char(exp_date,'yyyymmddhh24miss'),'20900101000000') exp_date "
   " from acct_balance where acct_id=%ld and state='10A' "
   " and (exists ( select * from billing_cycle where billing_cycle_id "
   " in (select billing_cycle_id from billing_cycle where "
   " billing_cycle_type_id <> 0 and "
   " state in ('10R','10A','10B','10D')) and   to_char(cycle_begin_date,'yyyymm') "
   " between to_char(nvl(eff_date,sysdate-1000),'yyyymm') "
   " and to_char(nvl(exp_date,sysdate+1000),'yyyymm') ) ) "
   " and balance_type_id not in ( %s ) "
   " group by serv_id,item_group_id,cycle_upper,eff_date,exp_date",iBillingCnt,iBillingCnt,lAcctID,sBalanceType);
   #endif
   qry.setSQL (sSql);
   qry.open ();

   while(qry.next ())
   {
   	 tempStopBalance.lAcctID = lAcctID;
   	 tempStopBalance.lServID = qry.field("SERV_ID").asLong();
   	 tempStopBalance.lAcctItemGroupID = qry.field("ITEM_GROUP_ID").asLong();
   	 tempStopBalance.lBalance  = qry.field("BALANCE").asLong();
   	 tempStopBalance.lUpper   = qry.field("CYCLE_UPPER").asLong();
   	 strcpy(tempStopBalance.sEffDate ,qry.field("EFF_DATE").asString());
   	 strcpy(tempStopBalance.sExpDate ,qry.field("EXP_DATE").asString());
   	 tempStopBalance.isCreidt = false;
   	 
   	 m_strBalance.push_back(tempStopBalance);
   	 
   }
   
   qry.close ();
   
   if (iCreditFlag == 1)
   {
      //增加帐户信用度
    	int iDefaultValue,lCredit;
    	iDefaultValue = 0;

    	qry.close();
    	qry.setSQL("select nvl(sum(credit_amount),:DEFAULT_VALUE) from acct_credit where acct_id = :ACCT_ID ");
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
   	    tempStopBalance.isCreidt = true;
   	    m_strBalance.push_back(tempStopBalance);
		
   }
    
}

void StopBalanceMgr::loadOweCharge(long lAcctID, int iPayRoleIDFlag)
{
    DEFINE_QUERY (qry) ;
    char sSql[2048];
    struct strOweCharge tempOweCharge;
//预付费历史欠费只取欠费、呆、坏帐
    m_strOweCharge.clear();
    #ifdef DEF_SICHUAN
    if (iPayRoleIDFlag == 1)//如果开关打开则剔除99（代收费）
    {	
    	sprintf(sSql ,"select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    	  " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
				" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
				" and ACCT_ID=%ld AND (ACCT_ITEM_TYPE_ID NOT IN (SELECT nvl(ACCT_ITEM_TYPE_ID,0) FROM ACCT_ITEM_TYPE WHERE nvl(party_role_id,0)=99)) "
				" group by serv_id,billing_cycle_id ",lAcctID);
	}
	else
	{
    	sprintf(sSql ,"select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    	  " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
				" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
				" and ACCT_ID=%ld "
				" group by serv_id,billing_cycle_id ",lAcctID);		
	}
    #else
    sprintf(sSql ," select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    " from acct_item_owe where acct_id=%ld "
    " and state in ('5JA','5JD','5JF') "
    " and billing_cycle_id not in "
    " (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
    " group by serv_id,billing_cycle_id ",lAcctID);
    #endif
    qry.setSQL (sSql);
    qry.open ();

    while(qry.next ())
    {
   	    tempOweCharge.lAcctID = lAcctID;
   	    tempOweCharge.lServID = qry.field("SERV_ID").asLong();
   	    tempOweCharge.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asLong();
   	    tempOweCharge.lCharge = qry.field("AMOUNT").asLong();
   	 
   	    m_strOweCharge.push_back(tempOweCharge);
    }
    qry.close ();
}


long StopBalanceMgr::getHistoryOweCharge(long lAcctID,long lServID,char sPreState[],int iBillingCycleID)
{
	vector<struct strOweCharge>::iterator itr;
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

//iPayRoleIDFlag为org值，找用户的增值业务的费用
void StopBalanceMgr::loadOweChargeZz(long lAcctID, int iPayRoleIDFlag)
{
    DEFINE_QUERY (qry) ;
    char sSql[2048];
    struct strOweCharge tempOweCharge;
//预付费历史欠费只取欠费、呆、坏帐
    m_strOweChargeZz.clear();
    #ifdef DEF_SICHUAN
    if (iPayRoleIDFlag == 1)//如果开关打开则剔除99（代收费）
    {	
    	sprintf(sSql ,"select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    	  " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
				" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
				" and ACCT_ID=%ld AND (ACCT_ITEM_TYPE_ID NOT IN (SELECT nvl(ACCT_ITEM_TYPE_ID,0) FROM ACCT_ITEM_TYPE WHERE nvl(party_role_id,0)=99)) "
				" group by serv_id,billing_cycle_id ",lAcctID);
	}
	else
	{
    	sprintf(sSql ,"select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    	  " from acct_item_owe where state in (select owe_state from A_PAY_OWE_STATE WHERE PAYMENT_METHOD=999) "
				" and billing_cycle_id not in (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
				" and ACCT_ID=%ld "
				" group by serv_id,billing_cycle_id ",lAcctID);		
	}
    #else
    sprintf(sSql ," select serv_id,billing_cycle_id,nvl(sum(nvl(amount,0)),0) amount "
    " from acct_item_owe where acct_id=%ld "
    " and state in ('5JA','5JD','5JF') "
    " and billing_cycle_id not in "
    " (select billing_cycle_id from billing_cycle where state in ('10B','10A') ) "
    " and acct_item_type_id in (select acct_item_type_id from A_INC_ACCT_ITEM_TYPE where org_id in (1,%d) and state='10A') "
    " group by serv_id,billing_cycle_id ",lAcctID,iPayRoleIDFlag);
    #endif
    qry.setSQL (sSql);
    qry.open ();

    while(qry.next ())
    {
   	    tempOweCharge.lAcctID = lAcctID;
   	    tempOweCharge.lServID = qry.field("SERV_ID").asLong();
   	    tempOweCharge.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asLong();
   	    tempOweCharge.lCharge = qry.field("AMOUNT").asLong();
   	 
   	    m_strOweChargeZz.push_back(tempOweCharge);
    }
    qry.close ();
}

long StopBalanceMgr::getHistoryOweChargeZz(long lAcctID,long lServID,char sPreState[],int iBillingCycleID)
{
	vector<struct strOweCharge>::iterator itr;
	long lOweCharge = 0;
	Date sys_date;
	char sDate[16];
	
	strcpy(sDate,sys_date.toString());
	
	for(itr=m_strOweChargeZz.begin();itr!=m_strOweChargeZz.end();itr++)
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
