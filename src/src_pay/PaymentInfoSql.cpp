#include "PaymentInfoSql.h"
#include "ABMException.h"
extern TimesTenCMD *m_poPublicConn;//查询
extern TimesTenCMD *m_pInsertPayout;//支出
extern TimesTenCMD *m_pInsertPayment;//记录业务操作
extern TimesTenCMD *m_pInsertRela;//记录支出来源关系
extern TimesTenCMD *m_pUpdateBalance;//更新账本
extern TimesTenCMD *m_pUpdateSource;//更新来源
extern TimesTenCMD *m_pInsertServInfo;//serv
extern TimesTenCMD *m_pInsertAcctInfo;//acct
extern TimesTenCMD *m_pInsertServAcctInfo;//serv_acct
extern TimesTenCMD *m_pInsertAcctBalance;//acct_balance
extern TimesTenCMD *m_pInsertBalanceSource;//balance_source
extern TimesTenCMD *m_pInsertUserInfoADD;//User_info_add
extern TimesTenCMD *m_pInsertAppUserInfo;//app_user_info
extern TimesTenCMD *m_pInsertAttUserInfo;//att_user_info

extern TimesTenCMD *m_pUpdatePayoutState;//更新支出记录状态
extern TimesTenCMD *m_pUpdateRealState;// 更新支出来源关系状态
extern TimesTenCMD *m_pUpdateOperState;// 更新业务流水记录状态-余额划拨冲正
extern TimesTenCMD *m_pRUpdateBalance;// 更新余额帐本-余额划拨冲正
using namespace std;
ABMException oExp;
PaymentInfoSql::PaymentInfoSql()
{
}

PaymentInfoSql::~PaymentInfoSql()
{

}

int PaymentInfoSql::ComparePasswd(AbmPayCond *&pAbmPayCond,PaymentInfoALLCCR &pPaymentInfoCCR,bool &bState)
{
	long lOptID=0L;
	string sql;
	try 
	{
		sql.clear();
		//支付密码校验
		if(bState)
			sql="SELECT SERV_ID FROM ATT_USER_INFO WHERE SERV_ID=:p0 and ATTR_TYPE='04' and ATTR_VALUES =:p1";
		else//二次验证随机密码
			sql="SELECT OPT_ID FROM RAMDOM_PASSWD WHERE RANDOM_PASSWD=:p0 and BAND_PHONE_ID=:p1";
		m_poPublicConn->Prepare(sql.c_str());
		//m_poPublicConn->Commit();
		if(bState)
		{
			m_poPublicConn->setParam(1, pPaymentInfoCCR.m_lServID);
			m_poPublicConn->setParam(2, pAbmPayCond->m_sPayPassword);
		}
		else
		{
			m_poPublicConn->setParam(1, pAbmPayCond->m_sRandomPassword);
			m_poPublicConn->setParam(2, pAbmPayCond->m_sDestinationAccount);
		}
		m_poPublicConn->Execute();
		while (!m_poPublicConn->FetchNext()) 
		{
			m_poPublicConn->getColumn(1, &lOptID);
		}
		__DEBUG_LOG1_(0, "PaymentInfo::lOptID =%d",lOptID );
		       
		m_poPublicConn->Close();
		if(lOptID<=0)
			return COMPARE_PASSWORD_ERR;	

		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::init oSt.err_msg=%s", oSt.err_msg);
	}
	return COMPARE_PASSWORD_ERR;
}
int PaymentInfoSql::QueryPaySeq(StructPaymentSeqInfo &sStructPaymentSeqInfo,bool &bState)
{
	string sql;
	try {
		sql.clear();
		//查询业务seq
		if(bState)
			sql="SELECT BALANCE_SOURCE_ID_SEQ.NEXTVAL,PAYOUT_ID_SEQ.NEXTVAL,SOURCE_PAYOUT_ID_SEQ.NEXTVAL FROM DUAL ";
		else//查询其他循环内的seq
			sql="SELECT PAYMENT_ID_SEQ.NEXTVAL FROM DUAL ";
				    
		m_poPublicConn->Prepare(sql.c_str());
		//m_poPublicConn->Commit();
		m_poPublicConn->Execute();
		while (!m_poPublicConn->FetchNext()) 
		{
			if(bState)
			{
				m_poPublicConn->getColumn(1, &sStructPaymentSeqInfo.m_lBalanceSourceSeq);
				m_poPublicConn->getColumn(2, &sStructPaymentSeqInfo.m_lBalancePayoutSeq);
				m_poPublicConn->getColumn(3, &sStructPaymentSeqInfo.m_lSourcePayoutSeq);
			}
			else
			{
				m_poPublicConn->getColumn(1, &sStructPaymentSeqInfo.m_lPaymentSeq);
			}
		}
		m_poPublicConn->Close();
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::QueryPaySeq oSt.err_msg=%s", oSt.err_msg);
	}
	return QUERY_SEQ_ERR;
}
int PaymentInfoSql::InsertPaymentInfo(PaymentInfoALLCCR &pPaymentInfoCCR,StructPaymentSeqInfo  &sStructPaymentSeqInfo)
{
    try 
    {
        
		m_pInsertPayment->setParam(1, sStructPaymentSeqInfo.m_lPaymentSeq);
         	__DEBUG_LOG1_(0, "m_lPaymentSeq =[%d]", sStructPaymentSeqInfo.m_lPaymentSeq);

		m_pInsertPayment->setParam(2, pPaymentInfoCCR.m_sOperateSeq);
         	__DEBUG_LOG1_(0, "m_sOperateSeq =[%s]", pPaymentInfoCCR.m_sOperateSeq);

		m_pInsertPayment->setParam(3, pPaymentInfoCCR.m_sCapabilityCode);
         	__DEBUG_LOG1_(0, "m_sCapabilityCode =[%s]", pPaymentInfoCCR.m_sCapabilityCode);

		m_pInsertPayment->setParam(4, pPaymentInfoCCR.m_lServID);
         	__DEBUG_LOG1_(0, "m_lServID =[%d]", pPaymentInfoCCR.m_lServID);

		if(sStructPaymentSeqInfo.m_lRbkPaymentSeq<=0)
			m_pInsertPayment->setParamNull(5);
		else
			m_pInsertPayment->setParam(5, sStructPaymentSeqInfo.m_lRbkPaymentSeq);
         	__DEBUG_LOG1_(0, "m_lRbkPaymentSeq =[%d]", sStructPaymentSeqInfo.m_lRbkPaymentSeq);

		m_pInsertPayment->setParam(6, pPaymentInfoCCR.m_sSpID);
         	__DEBUG_LOG1_(0, "m_sSpID =[%s]", pPaymentInfoCCR.m_sSpID);

		
		m_pInsertPayment->setParam(7, pPaymentInfoCCR.m_sSpName);
         	__DEBUG_LOG1_(0, "m_sSpName =[%s]", pPaymentInfoCCR.m_sSpName);
		
		m_pInsertPayment->setParam(8, pPaymentInfoCCR.m_sServicePlatformID);
         	__DEBUG_LOG1_(0, "m_sServicePlatformID =[%s]", pPaymentInfoCCR.m_sServicePlatformID);

		m_pInsertPayment->setParam(9, pPaymentInfoCCR.m_sDescription);
         	__DEBUG_LOG1_(0, "m_sDescription =[%s]", pPaymentInfoCCR.m_sDescription);

		m_pInsertPayment->setParam(10, pPaymentInfoCCR.m_sOrderID);
         	__DEBUG_LOG1_(0, "m_sOrderID =[%s]", pPaymentInfoCCR.m_sOrderID);

		m_pInsertPayment->setParam(11, pPaymentInfoCCR.m_sDescription);
         	__DEBUG_LOG1_(0, "m_sDescription =[%s]", pPaymentInfoCCR.m_sDescription);

		m_pInsertPayment->setParam(12, pPaymentInfoCCR.m_sOrderState);
         	__DEBUG_LOG1_(0, "m_sOrderState =[%s]", pPaymentInfoCCR.m_sOrderState);

		m_pInsertPayment->setParam(13, pPaymentInfoCCR.m_sMicropayType);
         	__DEBUG_LOG1_(0, "m_sMicropayType =[%s]", pPaymentInfoCCR.m_sMicropayType);

		m_pInsertPayment->setParam(14, (long)pPaymentInfoCCR.m_lPayAmount);
         	__DEBUG_LOG1_(0, "m_lPayAmount =[%d]", pPaymentInfoCCR.m_lPayAmount);

		m_pInsertPayment->Execute();
         	__DEBUG_LOG0_(0, "InsertPaymentInfo ok");

		//m_pInsertPayment->Commit();
		m_pInsertPayment->Close();
		return 0;
    }
    catch (TTStatus oSt)
    {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertPaymentInfo oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "InsertPaymentInfo error" );
    return INSERT_PAYMENT_ERR;
}
//查询账本表
int PaymentInfoSql::QueryAcctBalanceInfo(PaymentInfoALLCCR &pPaymentInfoCCR,vector<AcctBalanceInfo *>&vAcctBalanceInfo)
{
    string  sql;
    AcctBalanceInfo *pAcctBalanceInfo=NULL;
    try 
    {
	    	sql.clear();
		sql =" SELECT A.ACCT_BALANCE_ID,";
		sql +="       A.ACCT_ID,";
		sql +="       A.BALANCE_TYPE_ID,";
		sql +="       NVL(TO_CHAR(A.EFF_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       NVL(TO_CHAR(A.EXP_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       A.BALANCE,";
		sql +="       A.CYCLE_UPPER,";
		sql +="       A.CYCLE_LOWER";
		//sql +="       NVL(A.CYCLE_UPPER_TYPE,''),";
		//sql +="       NVL(A.CYCLE_LOWER_TYPE,''),";
		//sql +="       NVL(A.SERV_ID,-1),";
		//sql +="       NVL(A.ITEM_GROUP_ID,-1),";
		//sql +="       A.STATE,";
		//sql +="       TO_CHAR(A.STATE_DATE,'YYYYMMDDHH24MISS'),";
		//sql +="       NVL(A.OBJECT_TYPE_ID,-1),";
		//sql +="       NVL(A.BANK_ACCT,'')";
		sql +="  FROM ACCT_BALANCE A, SERV_ACCT B, SERV C ,BALANCE_TYPE D ";
		sql +=" WHERE A.ACCT_ID = B.ACCT_ID";
		sql +="   AND B.SERV_ID = C.SERV_ID";
		sql +="   AND A.BALANCE_TYPE_ID = D.BALANCE_TYPE_ID ";
		sql +="   AND C.STATE IN ('2HA')";
		sql +="   AND B.STATE = '10A'";
		sql +="   AND A.STATE = '10A'";
		sql +="   AND C.ACC_NBR = :p0 ";
		sql +="   AND A.BALANCE >0 ";
		sql +=" ORDER BY D.PRIORITY ASC ,";
		sql +=" A.EXP_DATE ASC, A.EFF_DATE ASC, A.BALANCE ASC ";
		
		m_poPublicConn->Prepare(sql.c_str());
		//m_poPublicConn->Commit();
		m_poPublicConn->setParam(1, pPaymentInfoCCR.m_sDestinationAccount);//用户标识
    		__DEBUG_LOG1_(0, "QueryAcctBalanceInfo pPaymentInfoCCR.m_sDestinationAccount =[%s]",pPaymentInfoCCR.m_sDestinationAccount );
    		__DEBUG_LOG1_(0, "QueryAcctBalanceInfo sql=[%s]",sql.c_str() );

		m_poPublicConn->Execute();
		while (!m_poPublicConn->FetchNext()) 
		{
			pAcctBalanceInfo =new AcctBalanceInfo;
			//pAcctBalanceInfo->clear();
			m_poPublicConn->getColumn(1, &pAcctBalanceInfo->m_lAcctBalanceID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctBalanceID=[%d]",pAcctBalanceInfo->m_lAcctBalanceID);
			m_poPublicConn->getColumn(2, &pAcctBalanceInfo->m_lAcctID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctID=[%d]",pAcctBalanceInfo->m_lAcctID);
			m_poPublicConn->getColumn(3, &pAcctBalanceInfo->m_lBalanceTypeID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalanceTypeID=[%d]",pAcctBalanceInfo->m_lBalanceTypeID);

			if(!m_poPublicConn->isColumnNull(4))
			{
				m_poPublicConn->getColumn(4, pAcctBalanceInfo->m_sEffDate);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sEffDate=[%s]",pAcctBalanceInfo->m_sEffDate);

			if(!m_poPublicConn->isColumnNull(5))
			{
				m_poPublicConn->getColumn(5, pAcctBalanceInfo->m_sExDate);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sExDate=[%s]",pAcctBalanceInfo->m_sExDate);

			m_poPublicConn->getColumn(6, &pAcctBalanceInfo->m_lBalance);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalance=[%ld]",pAcctBalanceInfo->m_lBalance);
			if(m_poPublicConn->isColumnNull(7))
			{
				pAcctBalanceInfo->m_lCycleUpper=-1;
			}
			else
			{
				m_poPublicConn->getColumn(7, &(pAcctBalanceInfo->m_lCycleUpper));
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleUpper=[%d]",pAcctBalanceInfo->m_lCycleUpper);
			if(m_poPublicConn->isColumnNull(8))
			{
				pAcctBalanceInfo->m_lCycleLower=-1;
			}
			else
			{
				m_poPublicConn->getColumn(8, &pAcctBalanceInfo->m_lCycleLower);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleLower=[%d]",pAcctBalanceInfo->m_lCycleLower);
			/*
			m_poPublicConn->getColumn(9, pAcctBalanceInfo->m_sCycleUpperType);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sCycleUpperType=[%s]",pAcctBalanceInfo->m_sCycleUpperType);
			m_poPublicConn->getColumn(10, pAcctBalanceInfo->m_sCycleLowerType);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sCycleLowerType=[%s]",pAcctBalanceInfo->m_sCycleLowerType);
			if(m_poPublicConn->isColumnNull(8))
			{
				pAcctBalanceInfo->m_lServID=-1;
			}
			else
			{
				m_poPublicConn->getColumn(11, &pAcctBalanceInfo->m_lServID);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lServID=[%d]",pAcctBalanceInfo->m_lServID);
			m_poPublicConn->getColumn(12, &pAcctBalanceInfo->m_lItemGroupID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lItemGroupID=[%d]",pAcctBalanceInfo->m_lItemGroupID);
			m_poPublicConn->getColumn(13, pAcctBalanceInfo->m_sState);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sState=[%s]",pAcctBalanceInfo->m_sState);
			m_poPublicConn->getColumn(14, pAcctBalanceInfo->m_sStateDate);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sStateDate=[%s]",pAcctBalanceInfo->m_sStateDate);
			m_poPublicConn->getColumn(15, &pAcctBalanceInfo->m_lObjectTypeID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lObjectTypeID=[%d]",pAcctBalanceInfo->m_lObjectTypeID);
			m_poPublicConn->getColumn(16, pAcctBalanceInfo->m_sBankAcct);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sBankAcct=[%s]",pAcctBalanceInfo->m_sBankAcct);
			*/
			vAcctBalanceInfo.push_back(pAcctBalanceInfo);
		}
		m_poPublicConn->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::QueryAcctBalanceInfo oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "QueryAcctBalanceInfo error" );
    return QRY_ACCT_BALANCE_ACC_NBR_ERR;
}
int PaymentInfoSql::QueryBalanceSourceInfo(long  &m_lAcctBalanceID,vector<BalanceSourceInfo *>&vBalanceSourceInfo)
{
    string  sql;
    BalanceSourceInfo *pBalanceSourceInfo=NULL;
    try 
    {
	    	sql.clear();
		sql ="  SELECT OPER_INCOME_ID,";
		sql +="       ACCT_BALANCE_ID,";
		sql +="       OPER_TYPE,";
		sql +="       TO_CHAR(OPER_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       AMOUNT,";
		/*
		sql +="       SOURCE_TYPE,";
		sql +="       SOURCE_DESC,";
		sql +="       STATE,";
		sql +="       TO_CHAR(STATE_DATE, 'YYYYMMDDHH24MISS'),";
		*/
		sql +="       OPT_ID,";
		sql +="       BALANCE,";
		/*
		sql +="       NVL(ALLOW_DRAW, ''),";
		sql +="       NVL(INV_OFFER, ''),";
		sql +="       NVL(CURR_STATE, ''),";
		sql +="       TO_CHAR(CURR_DATE, 'YYYYMMDDHH24MISS'),";
		*/
		sql +="       REMAIN_AMOUNT,";
		sql +="       STAFF_ID";
		/*
		sql +="       NVL(ACCT_BALANCE_OBJ_ID, -1),";
		sql +="       NVL(EFF_DATE_OFFSET, 0),";
		sql +="       NVL(EXP_DATE_OFFSET, 0)";
		*/
		sql +="  FROM BALANCE_SOURCE";
		sql +=" WHERE ACCT_BALANCE_ID =:p0 AND BALANCE>0 ";
    		__DEBUG_LOG1_(0, "QueryBalanceSourceInfo sql=[%s]",sql.c_str() );
    		__DEBUG_LOG1_(0, "QueryBalanceSourceInfo m_lAcctBalanceID=[%d]",m_lAcctBalanceID );
		
		m_poPublicConn->Prepare(sql.c_str());
		//m_poPublicConn->Commit();
		m_poPublicConn->setParam(1, m_lAcctBalanceID);//用户标识
		m_poPublicConn->Execute();
		while (!m_poPublicConn->FetchNext()) 
		{
			pBalanceSourceInfo =new BalanceSourceInfo;
			m_poPublicConn->getColumn(1, &pBalanceSourceInfo->m_lOperIncomeID);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lOperIncomeID=[%d]",pBalanceSourceInfo->m_lOperIncomeID );
			m_poPublicConn->getColumn(2, &pBalanceSourceInfo->m_lAcctBalanceId);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAcctBalanceId=[%d]",pBalanceSourceInfo->m_lAcctBalanceId );
			m_poPublicConn->getColumn(3, pBalanceSourceInfo->m_sOperType);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperType=[%s]",pBalanceSourceInfo->m_sOperType );
			m_poPublicConn->getColumn(4, pBalanceSourceInfo->m_sOperDate);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperDate=[%s]",pBalanceSourceInfo->m_sOperDate );
			m_poPublicConn->getColumn(5, &pBalanceSourceInfo->m_lAmount);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAmount=[%d]",pBalanceSourceInfo->m_lAmount );
			/*
			m_poPublicConn->getColumn(6, pBalanceSourceInfo->m_sSourceType);
			m_poPublicConn->getColumn(7, pBalanceSourceInfo->m_sSourceDesc);
			m_poPublicConn->getColumn(8, pBalanceSourceInfo->m_sState);
			m_poPublicConn->getColumn(9, pBalanceSourceInfo->m_sStateDate);
			*/
			m_poPublicConn->getColumn(6, &pBalanceSourceInfo->m_lPaymentID);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lPaymentID=[%d]",pBalanceSourceInfo->m_lPaymentID);
			m_poPublicConn->getColumn(7, &pBalanceSourceInfo->m_lBalance);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lBalance=[%d]",pBalanceSourceInfo->m_lBalance);
			/*
			m_poPublicConn->getColumn(12, pBalanceSourceInfo->m_sAllowDraw);
			m_poPublicConn->getColumn(13, pBalanceSourceInfo->m_sInvOffer);
			m_poPublicConn->getColumn(14, pBalanceSourceInfo->m_sCurrDate);
			*/
			if(m_poPublicConn->isColumnNull(8))
			{
				pBalanceSourceInfo->m_lRemainAmount=0;
			}
			else
			{
				m_poPublicConn->getColumn(8, &pBalanceSourceInfo->m_lRemainAmount);
			}
			m_poPublicConn->getColumn(9, &pBalanceSourceInfo->m_iStaffID);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_iStaffID=[%d]",pBalanceSourceInfo->m_iStaffID);
			/*
			m_poPublicConn->getColumn(17, &pBalanceSourceInfo->m_lAcctBalanceObjID);
			m_poPublicConn->getColumn(18, &pBalanceSourceInfo->m_lEffDateOffset);
			m_poPublicConn->getColumn(19, &pBalanceSourceInfo->m_lExpDateOffset);
			*/
			vBalanceSourceInfo.push_back(pBalanceSourceInfo);
		}
		m_poPublicConn->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::QueryBalanceSourceInfo oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "QueryBalanceSourceInfo error" );
    return QRY_ACCT_BALANCE_ID_ERR;

}
int PaymentInfoSql::InsertBalancePayout(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo,const string &m_sOperType)
{
    try 
    {
		m_pInsertPayout->setParam(1, sStructPaymentSeqInfo.m_lBalancePayoutSeq);
         	__DEBUG_LOG1_(0, "m_lBalancePayoutSeq =[%d]", sStructPaymentSeqInfo.m_lBalancePayoutSeq);

		m_pInsertPayout->setParam(2, pBalanceSourceInfo->m_lAcctBalanceId);
         	__DEBUG_LOG1_(0, "m_sOperateSeq =[%d]", pBalanceSourceInfo->m_lAcctBalanceId);

		m_pInsertPayout->setParam(3, sStructPaymentSeqInfo.m_lPaymentSeq);
         	__DEBUG_LOG1_(0, "m_lPaymentSeq =[%d]", sStructPaymentSeqInfo.m_lPaymentSeq);

		m_pInsertPayout->setParam(4, m_sOperType.c_str());
         	__DEBUG_LOG1_(0, "m_sOperType =[%s]", m_sOperType);

		m_pInsertPayout->setParam(5, sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "m_lPaymentAmount =[%d]", sStructPaymentSeqInfo.m_lPaymentAmount);

		m_pInsertPayout->setParam(6, pBalanceSourceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "m_Balance =[%d]", pBalanceSourceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount);

		m_pInsertPayout->setParam(7, 0);

		m_pInsertPayout->setParam(8, pBalanceSourceInfo->m_iStaffID);
         	__DEBUG_LOG1_(0, "pBalanceSourceInfo->m_iStaffID =[%d]", pBalanceSourceInfo->m_iStaffID);
		
		m_pInsertPayout->Execute();
         	__DEBUG_LOG0_(0, "InsertBalancePayout ok");
		

		//m_pInsertPayout->Commit();
		m_pInsertPayout->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertBalancePayout oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "InsertBalancePayout  error" );
    return INSERT_BALANCE_PAYOUT_ERR;
}
int PaymentInfoSql::InsertBalanceSourcePayoutRela(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo)
{
    try 
    {
		m_pInsertRela->setParam(1, sStructPaymentSeqInfo.m_lSourcePayoutSeq);
         	__DEBUG_LOG1_(0, "m_lSourcePayoutSeq =[%d]", sStructPaymentSeqInfo.m_lSourcePayoutSeq);

		m_pInsertRela->setParam(2, pBalanceSourceInfo->m_lOperIncomeID);
         	__DEBUG_LOG1_(0, "m_lOperIncomeID =[%d]", pBalanceSourceInfo->m_lOperIncomeID);

		m_pInsertRela->setParam(3, sStructPaymentSeqInfo.m_lBalancePayoutSeq);
         	__DEBUG_LOG1_(0, "m_lBalancePayoutSeq =[%d]", sStructPaymentSeqInfo.m_lBalancePayoutSeq);

		m_pInsertRela->setParam(4, pBalanceSourceInfo->m_lAcctBalanceId);
         	__DEBUG_LOG1_(0, "m_lAcctBalanceId =[%d]", pBalanceSourceInfo->m_lAcctBalanceId);

		m_pInsertRela->setParam(5, sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "m_lPaymentAmount =[%d]", sStructPaymentSeqInfo.m_lPaymentAmount);

		m_pInsertRela->setParam(6, pBalanceSourceInfo->m_lBalance);
         	__DEBUG_LOG1_(0, "pBalanceSourceInfo->m_lBalance =[%d]", pBalanceSourceInfo->m_lBalance);
		
		m_pInsertRela->Execute();
         	__DEBUG_LOG0_(0, "InsertBalanceSourcePayoutRela ok");
		

		//m_pInsertRela->Commit();
		m_pInsertRela->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertBalanceSourcePayoutRela oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "InsertBalanceSourcePayoutRela error" );
    return INSERT_BALANCE_SOURCE_PAYOUT_ERR;
}
int PaymentInfoSql::updateAcctBalance(StructPaymentSeqInfo &sStructPaymentSeqInfo,AcctBalanceInfo *&pAcctBalanceInfo)
{
    try 
    {
        
         	__DEBUG_LOG1_(0, "Acct_balance.Balance =[%ld]", pAcctBalanceInfo->m_lBalance);
         	__DEBUG_LOG1_(0, "Acct_balance.m_lPaymentAmount =[%ld]", sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "Acct_balance.Balance =[%ld]", pAcctBalanceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "pAcctBalanceInfo.m_lAcctBalanceID =[%ld]", pAcctBalanceInfo->m_lAcctBalanceID);
		pAcctBalanceInfo->m_lBalance -=sStructPaymentSeqInfo.m_lPaymentAmount;
		m_pUpdateBalance->setParam(1, pAcctBalanceInfo->m_lBalance);
		m_pUpdateBalance->setParam(2, pAcctBalanceInfo->m_lAcctBalanceID);
		
		m_pUpdateBalance->Execute();
         	__DEBUG_LOG0_(0, "updateAcctBalance ok");

		//m_pUpdateBalance->Commit();
		m_pUpdateBalance->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::updateAcctBalance oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "updateAcctBalance error" );
    return UPDATE_ACCT_BALANCE_ERR;
}
int PaymentInfoSql::updateBalanceSource(StructPaymentSeqInfo &sStructPaymentSeqInfo,BalanceSourceInfo *&pBalanceSourceInfo)
{
    try 
    {
        
		m_pUpdateSource->setParam(1, pBalanceSourceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount);
         	__DEBUG_LOG1_(0, "pBalanceSourceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount =[%ld]", pBalanceSourceInfo->m_lBalance-sStructPaymentSeqInfo.m_lPaymentAmount);

		m_pUpdateSource->setParam(2, pBalanceSourceInfo->m_lOperIncomeID);
         	__DEBUG_LOG1_(0, "pBalanceSourceInfo.m_lOperIncomeID =[%ld]", pBalanceSourceInfo->m_lOperIncomeID);
		
		m_pUpdateSource->Execute();
         	__DEBUG_LOG0_(0, "updateBalanceSource ok");

		//m_pUpdateSource->Commit();
		m_pUpdateSource->Close();
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::updateBalanceSource oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "updateBalanceSource error" );
    return UPDATE_BALANCE_SOURCE_ERR;
}


// 余额支出记录查询
int PaymentInfoSql::qryPayoutInfo(long lOldRequestId,vector<BalancePayoutInfo *>&vBalancePayOut)
{
	string  sql;
        BalancePayoutInfo *pBalancePayoutInfo=NULL;
    try 
    {

	    	sql.clear();
		sql ="  SELECT OPER_PAYOUT_ID,";
		sql +="       ACCT_BALANCE_ID,";
		sql +="       BILLING_CYCLE_ID,";
		sql +="       OPT_ID,";
		sql +="       OPER_TYPE,";
		sql +="       TO_CHAR(OPER_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       AMOUNT,";
		sql +="       BALANCE,";
		sql +="       PRN_COUNT,";
		sql +="       STATE,";
		sql +="       TO_CHAR(STATE_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       STAFF_ID";
		sql +="  FROM BALANCE_PAYOUT";

		sql +=" WHERE OPT_ID =:p0";

    		__DEBUG_LOG1_(0, "qryPayoutInfo sql=[%s]",sql.c_str() );
    		__DEBUG_LOG1_(0, "qryPayoutInfo lOldRequestId=[%ld]",lOldRequestId );

		m_poPublicConn->Prepare(sql.c_str());

		m_poPublicConn->setParam(1, lOldRequestId);//被冲正记录业务流水号

		m_poPublicConn->Execute();

		while (!m_poPublicConn->FetchNext()) 
		{

			pBalancePayoutInfo =new BalancePayoutInfo;
			m_poPublicConn->getColumn(1, &pBalancePayoutInfo->m_lOperPayoutId);
			m_poPublicConn->getColumn(2, &pBalancePayoutInfo->m_lAcctBalanceId);
			m_poPublicConn->getColumn(4, &pBalancePayoutInfo->m_lOptId);
			m_poPublicConn->getColumn(7, &pBalancePayoutInfo->m_lAmount);
			vBalancePayOut.push_back(pBalancePayoutInfo);

		}

		m_poPublicConn->Close();

		return 0;

    }

    catch (TTStatus oSt) {

        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryPayoutInfo oSt.err_msg=%s", oSt.err_msg);

    }

    __DEBUG_LOG0_(0, "qryPayoutInfo error" );

    return QRY_BALANCE_PAYOUT_ERR;
	
}

// 根据业务流水查余额来源信息
int PaymentInfoSql::qryBalanceSource(long lOptId,BalanceSourceInfo *&pBalanceSourceInfo)
{
    string  sql;
    int iRet = 1;
    try 
    {
	    	sql.clear();

		sql ="  SELECT A.OPER_INCOME_ID,";
		sql +="       A.ACCT_BALANCE_ID,";
		sql +="       A.OPER_TYPE,";
		sql +="       TO_CHAR(A.OPER_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       A.AMOUNT,";
		/*
		sql +="       SOURCE_TYPE,";

		sql +="       SOURCE_DESC,";

		sql +="       STATE,";

		sql +="       TO_CHAR(STATE_DATE, 'YYYYMMDDHH24MISS'),";
		*/
		sql +="       A.OPT_ID,";
		sql +="       A.BALANCE,";
		/*

		sql +="       NVL(ALLOW_DRAW, ''),";

		sql +="       NVL(INV_OFFER, ''),";

		sql +="       NVL(CURR_STATE, ''),";

		sql +="       TO_CHAR(CURR_DATE, 'YYYYMMDDHH24MISS'),";

		*/
		sql +="       A.REMAIN_AMOUNT,";
		sql +="       A.STAFF_ID";
		/*

		sql +="       NVL(ACCT_BALANCE_OBJ_ID, -1),";

		sql +="       NVL(EFF_DATE_OFFSET, 0),";

		sql +="       NVL(EXP_DATE_OFFSET, 0)";

		*/
		sql +="  FROM BALANCE_SOURCE A,BALANCE_PAYOUT B,BALANCE_SOURCE_PAYOUT_RELA C,OPERATION D ";
		sql +="  WHERE D.OPT_ID = B.OPT_ID ";
		sql +="  AND B.OPER_PAYOUT_ID = C.OPER_PAYOUT_ID ";
		sql +="  AND A.OPER_INCOME_ID = C.OPER_INCOME_ID ";
		sql +="  AND D.OPT_ID =:p0";
		//FOREIGN_ID
		__DEBUG_LOG0_(0, "===>根据冲正流水查业务流水对应的余额来源");
    		//__DEBUG_LOG1_(0, "queryBalanceSourceInfo sql=[%s]",sql.c_str() );
    		//__DEBUG_LOG1_(0, "queryBalanceSourceInfo m_lOptID=[%d]",lOptId ); // 结构体中未定义OPT_ID

		m_poPublicConn->Prepare(sql.c_str());
		string sForeignId;
		__DEBUG_LOG1_(0, "===>lOptId=[%d]",lOptId);
		sForeignId = ltoa(lOptId);
		__DEBUG_LOG1_(0, "sForeignId=[%s]",sForeignId.c_str());
		m_poPublicConn->setParam(1, lOptId);//流水号

		m_poPublicConn->Execute();

		while (!m_poPublicConn->FetchNext()) 
		{

			pBalanceSourceInfo =new BalanceSourceInfo;

			m_poPublicConn->getColumn(1, &pBalanceSourceInfo->m_lOperIncomeID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lOperIncomeID=[%d]",pBalanceSourceInfo->m_lOperIncomeID );

			m_poPublicConn->getColumn(2, &pBalanceSourceInfo->m_lAcctBalanceId);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAcctBalanceId=[%d]",pBalanceSourceInfo->m_lAcctBalanceId );

			m_poPublicConn->getColumn(3, pBalanceSourceInfo->m_sOperType);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperType=[%s]",pBalanceSourceInfo->m_sOperType );

			m_poPublicConn->getColumn(4, pBalanceSourceInfo->m_sOperDate);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperDate=[%s]",pBalanceSourceInfo->m_sOperDate );

			m_poPublicConn->getColumn(5, &pBalanceSourceInfo->m_lAmount);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAmount=[%d]",pBalanceSourceInfo->m_lAmount );

			/*

			m_poPublicConn->getColumn(6, pBalanceSourceInfo->m_sSourceType);

			m_poPublicConn->getColumn(7, pBalanceSourceInfo->m_sSourceDesc);

			m_poPublicConn->getColumn(8, pBalanceSourceInfo->m_sState);

			m_poPublicConn->getColumn(9, pBalanceSourceInfo->m_sStateDate);

			*/

			m_poPublicConn->getColumn(6, &pBalanceSourceInfo->m_lPaymentID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lPaymentID=[%d]",pBalanceSourceInfo->m_lPaymentID);

			m_poPublicConn->getColumn(7, &pBalanceSourceInfo->m_lBalance);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lBalance=[%d]",pBalanceSourceInfo->m_lBalance);

			/*

			m_poPublicConn->getColumn(12, pBalanceSourceInfo->m_sAllowDraw);

			m_poPublicConn->getColumn(13, pBalanceSourceInfo->m_sInvOffer);

			m_poPublicConn->getColumn(14, pBalanceSourceInfo->m_sCurrDate);

			*/

			if(m_poPublicConn->isColumnNull(8))
			{
				pBalanceSourceInfo->m_lRemainAmount=0;
			}
			else
			{
				m_poPublicConn->getColumn(8, &pBalanceSourceInfo->m_lRemainAmount);
			}
			m_poPublicConn->getColumn(9, &pBalanceSourceInfo->m_iStaffID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_iStaffID=[%d]",pBalanceSourceInfo->m_iStaffID);
			/*

			m_poPublicConn->getColumn(17, &pBalanceSourceInfo->m_lAcctBalanceObjID);

			m_poPublicConn->getColumn(18, &pBalanceSourceInfo->m_lEffDateOffset);

			m_poPublicConn->getColumn(19, &pBalanceSourceInfo->m_lExpDateOffset);

			*/

			//vBalanceSourceInfo.push_back(pBalanceSourceInfo);
			
			iRet = 0;
		}

		m_poPublicConn->Close();
		return iRet;
    }
    catch (TTStatus oSt) {

        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryBalanceSource oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "qryBalanceSource error" );
    
    iRet = QRY_BALANCE_SOURCE_ERR;
    return iRet;
	
}

// 更新余额支出记录状态10X
int PaymentInfoSql::updateBalancePayout(long lOperPayoutId)
{
	 try 
	{
		m_pUpdatePayoutState->setParam(1, lOperPayoutId);
		m_pUpdatePayoutState->Execute();

         	__DEBUG_LOG0_(0, "updateBalancePayout ok");

		m_pUpdatePayoutState->Close();

		return 0;
	}
	catch (TTStatus oSt)
	{
        	ADD_EXCEPT1(oExp, "PaymentInfoSql::updateBalancePayout oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "updateBalancePayout error" );

	return UPDATE_BALANCE_PAYOUT_ERR;
	
}
// 更新来源支出关系表状态10X
int PaymentInfoSql::updateBalanceSourcePayoutRela(long lOperPayoutId)
{
	try 
	{
		__DEBUG_LOG1_(0, "updateBalancePayout:lOperPayoutId=%d.",lOperPayoutId);
		m_pUpdateRealState->setParam(1, lOperPayoutId);
		
		m_pUpdateRealState->Execute();

		__DEBUG_LOG0_(0, "updateBalanceSourcePayoutRela ok");

		m_pUpdateRealState->Close();

		return 0;
	}
	catch (TTStatus oSt)
	{
        	ADD_EXCEPT1(oExp, "updateBalanceSourcePayoutRela::updateBalancePayout oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "updateBalanceSourcePayoutRela error" );

	return UPDATE_BALANCE_SOURCE_PAYOUT_ERR;
}
//查询用户资料序列
int  PaymentInfoSql::QueryServAcctSeq(ServAcctInfoSeq &oServAcctInfoSeq,bool &bState)
{
	string sql;
	try {
		sql.clear();
		if(bState)
			sql="SELECT SERV_ID_SEQ.NEXTVAL,ACCT_ID_SEQ.NEXTVAL,SERV_ACCT_ID_SEQ.NEXTVAL,ACCT_BALANCE_ID_SEQ.NEXTVAL,BALANCE_SOURCE_ID_SEQ.NEXTVAL,CUST_ID_SEQ.NEXTVAL FROM DUAL ";
		else
			sql="SELECT ACCT_BALANCE_ID_SEQ.NEXTVAL,BALANCE_SOURCE_ID_SEQ.NEXTVAL FROM DUAL ";

		m_poPublicConn->Prepare(sql.c_str());
		m_poPublicConn->Execute();

		while (!m_poPublicConn->FetchNext()) 
		{
			if(bState)
			{
				m_poPublicConn->getColumn(1, &oServAcctInfoSeq.m_lServID);
				m_poPublicConn->getColumn(2, &oServAcctInfoSeq.m_lAcctID);
				m_poPublicConn->getColumn(3, &oServAcctInfoSeq.m_lServAcctID);
				m_poPublicConn->getColumn(4, &oServAcctInfoSeq.m_lAcctBalanceID);
				m_poPublicConn->getColumn(5, &oServAcctInfoSeq.m_lBalanceSourceID);
				m_poPublicConn->getColumn(6, &oServAcctInfoSeq.m_lCustID);
			}
			else
			{
				m_poPublicConn->getColumn(1, &oServAcctInfoSeq.m_lAcctBalanceID);
				m_poPublicConn->getColumn(2, &oServAcctInfoSeq.m_lBalanceSourceID);
			}
		}
		m_poPublicConn->Close();
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "QueryServAcctSeq::QueryPaySeq oSt.err_msg=%s", oSt.err_msg);
	}
	return QRY_SERV_ACCT_SEQ_ERR;
}

int PaymentInfoSql::InsertServInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne)
{
	try {
		__DEBUG_LOG1_(0, "InsertServInfo serv_id=[%ld]",oServAcctInfoSeq.m_lServID);
		m_pInsertServInfo->setParam(1, oServAcctInfoSeq.m_lServID);
		__DEBUG_LOG1_(0, "InsertServInfo m_lCustID=[%ld]",oServAcctInfoSeq.m_lCustID);
		m_pInsertServInfo->setParam(2, oServAcctInfoSeq.m_lCustID);
		__DEBUG_LOG1_(0, "InsertServInfo pInOne->m_sDestinationAccount=[%s]",pInOne->m_sDestinationAccount);
		m_pInsertServInfo->setParam(3, pInOne->m_sDestinationAccount);
		__DEBUG_LOG1_(0, "InsertServInfo pInOne->m_iDestinationAttr=[%d]",pInOne->m_iDestinationAttr);
		//m_pInsertServInfo->setParam(4, ltoa(pInOne->m_iDestinationAttr));
		m_pInsertServInfo->setParam(4, "2");// VC没有传该值，默认为2
		m_pInsertServInfo->setParam(5, ltoa(pInOne->m_iDestinationAttr));
		m_pInsertServInfo->Execute();
		m_pInsertServInfo->Close();
		__DEBUG_LOG0_(0, "InsertServInfo ok");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertServInfo oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_SERV_ERR;
}
int PaymentInfoSql::InsertAcctInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne)
{
	try {
		__DEBUG_LOG1_(0, "InsertAcctInfo m_lAcctID=[%ld]",oServAcctInfoSeq.m_lAcctID);
		m_pInsertAcctInfo->setParam(1, oServAcctInfoSeq.m_lAcctID);
		__DEBUG_LOG1_(0, "InsertAcctInfo m_lCustID=[%ld]",oServAcctInfoSeq.m_lCustID);
		m_pInsertAcctInfo->setParam(2, oServAcctInfoSeq.m_lCustID);
		__DEBUG_LOG1_(0, "InsertAcctInfo m_sDestinationAccount=[%s]",pInOne->m_sDestinationAccount);
		m_pInsertAcctInfo->setParam(3, pInOne->m_sDestinationAccount);
		m_pInsertAcctInfo->Execute();
		m_pInsertAcctInfo->Close();
		__DEBUG_LOG0_(0, "InsertAcctInfo ok");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertAcctInfo oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_ACCT_ERR;
}
int PaymentInfoSql::InsertServAcctInfo(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne)
{
	try {
		__DEBUG_LOG1_(0, "InsertServAcctInfo m_lServAcctID=[%ld]",oServAcctInfoSeq.m_lServAcctID);
		m_pInsertServAcctInfo->setParam(1, oServAcctInfoSeq.m_lServAcctID);
		__DEBUG_LOG1_(0, "InsertServAcctInfo m_lAcctID=[%ld]",oServAcctInfoSeq.m_lAcctID);
		m_pInsertServAcctInfo->setParam(2, oServAcctInfoSeq.m_lAcctID);
		__DEBUG_LOG1_(0, "InsertServAcctInfo m_lServID=[%ld]",oServAcctInfoSeq.m_lServID);
		m_pInsertServAcctInfo->setParam(3, oServAcctInfoSeq.m_lServID);
		m_pInsertServAcctInfo->Execute();
		m_pInsertServAcctInfo->Close();
		__DEBUG_LOG0_(0, "InsertServAcctInfo ok");

		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertServAcctInfo oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_SERV_ACCT_ERR;
}
int PaymentInfoSql::InsertAcctBalance(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne)
{
	try {
		__DEBUG_LOG1_(0, "InsertAcctBalance m_lAcctBalanceID=[%ld]",oServAcctInfoSeq.m_lAcctBalanceID);
		m_pInsertAcctBalance->setParam(1, oServAcctInfoSeq.m_lAcctBalanceID);
		__DEBUG_LOG1_(0, "InsertAcctBalance m_lAcctID=[%ld]",oServAcctInfoSeq.m_lAcctID);
		m_pInsertAcctBalance->setParam(2, oServAcctInfoSeq.m_lAcctID);
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iBalanceType=[%d]",pInOne->m_iBalanceType);
		//if(strlen(pInOne->m_sBalanceType)<=0)
		if(pInOne->m_iBalanceType<=0)
			m_pInsertAcctBalance->setParam(3, 10);//默认余额类型为10
		else
			m_pInsertAcctBalance->setParam(3, (int)pInOne->m_iBalanceType);
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iBonusStartTime=[%d]",pInOne->m_iBonusStartTime);
		
		char sBonusStartTime[32]={0};
		char sBonusExpirationTime[32]={0};
		
		time_t tTime;
		
		if(pInOne->m_iBonusStartTime<=0)
		{
			/*
			// modify on 2011.7.5 生效日期默认当前时间
			//m_pInsertAcctBalance->setParamNull(4);
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sBonusStartTime);
			*/
			
			// modify on 2011.7.9 生效时间设为定值
			strcpy(sBonusStartTime,"19700101000000");
			m_pInsertAcctBalance->setParam(4, sBonusStartTime);
		}
		else
		{
			
			long2date(pInOne->m_iBonusStartTime,sBonusStartTime);
			//m_pInsertAcctBalance->setParam(4, ltoa(pInOne->m_iBonusStartTime));
			m_pInsertAcctBalance->setParam(4, sBonusStartTime);
		}
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iBonusExpirationTime=[%d]",pInOne->m_iBonusExpirationTime);
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iBonusExpirationTime=[%s]",sBonusStartTime);
		if(pInOne->m_iBonusExpirationTime<=0)
		{
			// modify on 2011.7.5 失效日期默认
			strcpy(sBonusExpirationTime,"20360101000000");
			m_pInsertAcctBalance->setParam(5, sBonusExpirationTime);
			//m_pInsertAcctBalance->setParamNull(5);
		}
		else
		{
			long2date(pInOne->m_iBonusExpirationTime,sBonusExpirationTime);
			//m_pInsertAcctBalance->setParam(5, ltoa(pInOne->m_iBonusExpirationTime));
			m_pInsertAcctBalance->setParam(5, sBonusExpirationTime);
		}
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iVoucherValue=[%d]",pInOne->m_iVoucherValue);
		__DEBUG_LOG1_(0, "InsertAcctBalance pInOne->m_iVoucherValue=[%s]",sBonusExpirationTime);
		m_pInsertAcctBalance->setParam(6, (int)pInOne->m_iVoucherValue);
		m_pInsertAcctBalance->Execute();
		m_pInsertAcctBalance->Close();
		__DEBUG_LOG0_(0, "InsertAcctBalance ok");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertAcctBalance oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_ACCT_BALANCE_ERR;
}

int PaymentInfoSql::InsertBalanceSource(DepositServCondOne *&pInOne,StructPaymentSeqInfo &sStructPaymentSeqInfo)
	
{
	try {

		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_lBalanceSourceSeq=[%ld]",sStructPaymentSeqInfo.m_lBalanceSourceSeq);
		m_pInsertBalanceSource->setParam(1, sStructPaymentSeqInfo.m_lBalanceSourceSeq);
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_lAcctBalanceID=[%ld]",sStructPaymentSeqInfo.m_lAcctBalanceID);
		m_pInsertBalanceSource->setParam(2, sStructPaymentSeqInfo.m_lAcctBalanceID);
		m_pInsertBalanceSource->setParam(3, "5UA");
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_iVoucherValue=[%d]",pInOne->m_iVoucherValue);
		m_pInsertBalanceSource->setParam(4, (int)pInOne->m_iVoucherValue);
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_sSourceType=[%s]",pInOne->m_sSourceType);
		m_pInsertBalanceSource->setParam(5, pInOne->m_sSourceType);
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_sSourceDesc=[%s]",pInOne->m_sSourceDesc);
		m_pInsertBalanceSource->setParam(6, pInOne->m_sSourceDesc);
		m_pInsertBalanceSource->setParam(7, "10A");
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_lPaymentSeq=[%ld]",sStructPaymentSeqInfo.m_lPaymentSeq);
		m_pInsertBalanceSource->setParam(8, sStructPaymentSeqInfo.m_lPaymentSeq);
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_iVoucherValue=[%d]",pInOne->m_iVoucherValue);
		m_pInsertBalanceSource->setParam(9, (int)pInOne->m_iVoucherValue);
		__DEBUG_LOG1_(0, "m_pInsertBalanceSource m_iVoucherValue=[%d]",pInOne->m_iVoucherValue);
		m_pInsertBalanceSource->setParam(10,pInOne->m_sAllowDraw);
		m_pInsertBalanceSource->Execute();
		m_pInsertBalanceSource->Close();
		__DEBUG_LOG0_(0, "m_pInsertBalanceSource 0k");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertBalanceSource oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_BALANCE_SOURCE_ERR;
}
int PaymentInfoSql::QueryAcctBalanceSimple(DepositServCondOne *&pInOne,AcctBalanceInfo *&pAcctBalanceInfo)
{
    string  sql;
    bool  bstate=false;
    try 
    {
	    	sql.clear();
		sql =" SELECT A.ACCT_BALANCE_ID,";
		sql +="       A.ACCT_ID,";
		sql +="       A.BALANCE_TYPE_ID,";
		sql +="       NVL(TO_CHAR(A.EFF_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       NVL(TO_CHAR(A.EXP_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       A.BALANCE,";
		sql +="       A.CYCLE_UPPER,";
		sql +="       A.CYCLE_LOWER";
		sql +="  FROM ACCT_BALANCE A, SERV_ACCT B, SERV C";
		sql +=" WHERE A.ACCT_ID = B.ACCT_ID";
		sql +="   AND B.SERV_ID = C.SERV_ID";
		sql +="   AND C.STATE IN ('2HA')";
		sql +="   AND B.STATE = '10A'";
		sql +="   AND A.STATE = '10A'";
		sql +="   AND A.BALANCE_TYPE_ID = :BALANCE_TYPE_ID";
		sql +="   AND C.ACC_NBR = :ACC_NBR ";
		
		m_poPublicConn->Prepare(sql.c_str());
		//m_poPublicConn->setParam(1, atol(pInOne->m_sBalanceType));//账本类型
		m_poPublicConn->setParam(1, (int)pInOne->m_iBalanceType);//账本类型
		m_poPublicConn->setParam(2, pInOne->m_sDestinationAccount);//用户标识
    		__DEBUG_LOG1_(0, "QueryAcctBalanceSimple pInOne.m_iBalanceType =[%d]",pInOne->m_iBalanceType);
    		__DEBUG_LOG1_(0, "QueryAcctBalanceSimple pInOne.m_sDestinationAccount =[%s]",pInOne->m_sDestinationAccount );

		m_poPublicConn->Execute();
		if(!m_poPublicConn->FetchNext()) 
		{
    			__DEBUG_LOG0_(0, "QueryAcctBalanceSimple begin" );
			m_poPublicConn->getColumn(1, &pAcctBalanceInfo->m_lAcctBalanceID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctBalanceID=[%ld]",pAcctBalanceInfo->m_lAcctBalanceID);
			m_poPublicConn->getColumn(2, &pAcctBalanceInfo->m_lAcctID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctID=[%ld]",pAcctBalanceInfo->m_lAcctID);
			m_poPublicConn->getColumn(3, &pAcctBalanceInfo->m_lBalanceTypeID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalanceTypeID=[%ld]",pAcctBalanceInfo->m_lBalanceTypeID);
			m_poPublicConn->getColumn(4, pAcctBalanceInfo->m_sEffDate);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sEffDate=[%s]",pAcctBalanceInfo->m_sEffDate);
			m_poPublicConn->getColumn(5, pAcctBalanceInfo->m_sExDate);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sExDate=[%s]",pAcctBalanceInfo->m_sExDate);
			m_poPublicConn->getColumn(6, &pAcctBalanceInfo->m_lBalance);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalance=[%ld]",pAcctBalanceInfo->m_lBalance);
			if(m_poPublicConn->isColumnNull(7))
			{
				pAcctBalanceInfo->m_lCycleUpper=-1;
			}
			else
			{
				m_poPublicConn->getColumn(7, &(pAcctBalanceInfo->m_lCycleUpper));
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleUpper=[%ld]",pAcctBalanceInfo->m_lCycleUpper);
			if(m_poPublicConn->isColumnNull(8))
			{
				pAcctBalanceInfo->m_lCycleLower=-1;
			}
			else
			{
				m_poPublicConn->getColumn(8, &pAcctBalanceInfo->m_lCycleLower);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleLower=[%ld]",pAcctBalanceInfo->m_lCycleLower);
			bstate=true;
		
		}
		m_poPublicConn->Close();
		if(bstate)
			return 1;
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::QueryAcctBalanceSimple oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "QueryAcctBalanceSimple error" );
    return QRY_ACCT_BALANCE_SIMPLE_ERR;
}


int PaymentInfoSql::qryAcctBalance(long &lAcctBalanceId,AcctBalanceInfo *&pAcctBalanceInfo)
{
	string  sql;
    bool  bstate=false;
    try 
    {
	    	sql.clear();
		sql =" SELECT A.ACCT_BALANCE_ID,";
		sql +="       A.ACCT_ID,";
		sql +="       A.BALANCE_TYPE_ID,";
		sql +="       NVL(TO_CHAR(A.EFF_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       NVL(TO_CHAR(A.EXP_DATE, 'YYYYMMDDHH24MISS'),''),";
		sql +="       A.BALANCE,";
		sql +="       A.CYCLE_UPPER,";
		sql +="       A.CYCLE_LOWER";
		sql +="  FROM ACCT_BALANCE A";
		sql +=" WHERE A.ACCT_BALANCE_ID =:p0";
		
		m_poPublicConn->Prepare(sql.c_str());
		m_poPublicConn->setParam(1, lAcctBalanceId);//余额帐本标识
    		__DEBUG_LOG1_(0, "qryAcctBalanceACCT_BALANCE_ID =[%d]",lAcctBalanceId);

		m_poPublicConn->Execute();
		if(!m_poPublicConn->FetchNext()) 
		{
    			__DEBUG_LOG0_(0, "QueryAcctBalanceSimple begin" );
			m_poPublicConn->getColumn(1, &pAcctBalanceInfo->m_lAcctBalanceID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctBalanceID=[%ld]",pAcctBalanceInfo->m_lAcctBalanceID);
			m_poPublicConn->getColumn(2, &pAcctBalanceInfo->m_lAcctID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lAcctID=[%ld]",pAcctBalanceInfo->m_lAcctID);
			m_poPublicConn->getColumn(3, &pAcctBalanceInfo->m_lBalanceTypeID);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalanceTypeID=[%ld]",pAcctBalanceInfo->m_lBalanceTypeID);
			m_poPublicConn->getColumn(4, pAcctBalanceInfo->m_sEffDate);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sEffDate=[%s]",pAcctBalanceInfo->m_sEffDate);
			m_poPublicConn->getColumn(5, pAcctBalanceInfo->m_sExDate);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_sExDate=[%s]",pAcctBalanceInfo->m_sExDate);
			m_poPublicConn->getColumn(6, &pAcctBalanceInfo->m_lBalance);
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lBalance=[%ld]",pAcctBalanceInfo->m_lBalance);
			if(m_poPublicConn->isColumnNull(7))
			{
				pAcctBalanceInfo->m_lCycleUpper=-1;
			}
			else
			{
				m_poPublicConn->getColumn(7, &(pAcctBalanceInfo->m_lCycleUpper));
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleUpper=[%ld]",pAcctBalanceInfo->m_lCycleUpper);
			if(m_poPublicConn->isColumnNull(8))
			{
				pAcctBalanceInfo->m_lCycleLower=-1;
			}
			else
			{
				m_poPublicConn->getColumn(8, &pAcctBalanceInfo->m_lCycleLower);
			}
    			__DEBUG_LOG1_(0, "pAcctBalanceInfo->m_lCycleLower=[%ld]",pAcctBalanceInfo->m_lCycleLower);
			bstate=true;
		
		}
		m_poPublicConn->Close();
		if(bstate)
			return 1;
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryAcctBalance oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "qryAcctBalance error" );
    return QRY_ACCT_BALANCE_ID_ERR;
}
int PaymentInfoSql::InsertUserInfoAdd(ServAcctInfoSeq &oServAcctInfoSeq,DepositServCondOne *&pInOne)
	
{
	try {

		__DEBUG_LOG1_(0, "InsertUserInfoAdd oServAcctInfoSeq.m_lServID=[%ld]",oServAcctInfoSeq.m_lServID);
		m_pInsertUserInfoADD->setParam(1, oServAcctInfoSeq.m_lServID);
		m_pInsertUserInfoADD->setParam(2, 1);
		__DEBUG_LOG1_(0, "InsertUserInfoAdd m_lCustID=[%ld]",oServAcctInfoSeq.m_lCustID);
		m_pInsertUserInfoADD->setParam(3, oServAcctInfoSeq.m_lCustID);
		m_pInsertUserInfoADD->setParam(4,1);
		m_pInsertUserInfoADD->setParam(5, 1);
		m_pInsertUserInfoADD->setParamNull(6);
		int iValue = pInOne->m_iDestinationAttr;
		char sUserType[4]={0};
		if(iValue<5 || iValue==12)
		{
			m_pInsertUserInfoADD->setParam(7, "1");
			strcpy(sUserType,"1");
		}
		else
		{
			m_pInsertUserInfoADD->setParam(7, "2");
			strcpy(sUserType,"2");
		}
		__DEBUG_LOG1_(0, "InsertUserInfoAdd m_UserTyep=[%s]",sUserType);
		__DEBUG_LOG1_(0, "InsertUserInfoAdd m_iDestinationAttr=[%d]",pInOne->m_iDestinationAttr);
		m_pInsertUserInfoADD->setParam(8, ltoa(pInOne->m_iDestinationAttr));
		m_pInsertUserInfoADD->setParam(9,"1");
		__DEBUG_LOG1_(0, "InsertUserInfoAdd m_lAcctID=[%ld]",oServAcctInfoSeq.m_lAcctID);
		m_pInsertUserInfoADD->setParam(10,oServAcctInfoSeq.m_lAcctID);
		__DEBUG_LOG1_(0, "InsertUserInfoAdd m_sDestinationAccount=[%s]",pInOne->m_sDestinationAccount);
		m_pInsertUserInfoADD->setParam(11,pInOne->m_sDestinationAccount);
		m_pInsertUserInfoADD->Execute();
		m_pInsertUserInfoADD->Close();
		__DEBUG_LOG0_(0, "InsertUserInfoAdd OK");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertUserInfoAdd oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_USER_INFO_ADD_ERR;
}

// 划拨，插入余额来源记录
int PaymentInfoSql::insertBalanceSource(BalanceSourceInfo &balanceSourceInfo)
{
	try {		
		m_pInsertBalanceSource->setParam(1, balanceSourceInfo.m_lOperIncomeID);
		
		m_pInsertBalanceSource->setParam(2, balanceSourceInfo.m_lAcctBalanceId);
		m_pInsertBalanceSource->setParam(3, "5UA");
		
		m_pInsertBalanceSource->setParam(4, balanceSourceInfo.m_lAmount);
		
		m_pInsertBalanceSource->setParam(5, balanceSourceInfo.m_sSourceType);
		
		m_pInsertBalanceSource->setParam(6, balanceSourceInfo.m_sSourceDesc);
		m_pInsertBalanceSource->setParam(7, "10A");
		
		m_pInsertBalanceSource->setParam(8, balanceSourceInfo.m_lPaymentID);
		
		m_pInsertBalanceSource->setParam(9, balanceSourceInfo.m_lBalance);
		
		m_pInsertBalanceSource->setParam(10,balanceSourceInfo.m_sAllowDraw);
		m_pInsertBalanceSource->Execute();
		m_pInsertBalanceSource->Close();
		__DEBUG_LOG0_(0, "m_pInsertBalanceSource 0k");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::InsertBalanceSource oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_BALANCE_SOURCE_ERR;
}
int PaymentInfoSql::InsertAttUserInfo(ServAcctInfoSeq &oServAcctInfoSeq,bool  &bState)
	
{
	try {

		__DEBUG_LOG1_(0, "m_pInsertAttUserInfo oServAcctInfoSeq.m_lServID=[%ld]",oServAcctInfoSeq.m_lServID);
		m_pInsertAttUserInfo->setParam(1, oServAcctInfoSeq.m_lServID);
		m_pInsertAttUserInfo->setParam(2, "123456");
		if(bState)
			m_pInsertAttUserInfo->setParam(3, "04");
		else
			m_pInsertAttUserInfo->setParam(3, "03");
		m_pInsertAttUserInfo->Execute();
		m_pInsertAttUserInfo->Close();
		__DEBUG_LOG0_(0, "m_pInsertAttUserInfo OK");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::m_pInsertAttUserInfo oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_ATT_USER_INFO_ERR;
}
int PaymentInfoSql::InsertAppUserInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq)
	
{
	try {
		string sTmp;
		// modify 2011.7.5.三位区号需补齐4位
		char sArea[5]={0};
		strcpy(sArea,pInOne->m_sAreaCode);
		if(sArea[0]=='0')
		{
			__DEBUG_LOG0_(0, "区号无须补0");
		}
		else
		{
			sTmp = sArea;
			sTmp = "0"+sTmp;
			strcpy(sArea,sTmp.c_str());
		}
		
		__DEBUG_LOG1_(0, "InsertAppUserInfo oServAcctInfoSeq.m_lServID=[%ld]",oServAcctInfoSeq.m_lServID);
		m_pInsertAppUserInfo->setParam(1, oServAcctInfoSeq.m_lServID);
		m_pInsertAppUserInfo->setParamNull(2);
		m_pInsertAppUserInfo->setParamNull(3);
		m_pInsertAppUserInfo->setParamNull(4);
		m_pInsertAppUserInfo->setParamNull(5);
		__DEBUG_LOG1_(0, "InsertAppUserInfo m_sAreaCode=[%s]",pInOne->m_sAreaCode);
		//m_pInsertAppUserInfo->setParam(6, pInOne->m_sAreaCode);
		m_pInsertAppUserInfo->setParam(6, sArea);
		m_pInsertAppUserInfo->setParamNull(7);
		m_pInsertAppUserInfo->Execute();
		m_pInsertAppUserInfo->Close();
		__DEBUG_LOG0_(0, "m_pInsertAppUserInfo OK");
		return 0;
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp, "PaymentInfoSql::m_pInsertAppUserInfo oSt.err_msg=%s", oSt.err_msg);
	}
	return INSERT_APP_USER_INFO_ERR;
}
// 更新付款记录状态C0R - 余额划拨冲正
int PaymentInfoSql::updateOperation(long lOperPayoutId)
{
	try 
	{
		m_pUpdateOperState->setParam(1, lOperPayoutId);
		m_pUpdateOperState->Execute();

         	__DEBUG_LOG0_(0, "updateOperation ok");

		m_pUpdateOperState->Close();

		return 0;
	}
	catch (TTStatus oSt)
	{
        	ADD_EXCEPT1(oExp, "PaymentInfoSql::updateOperation oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "updateOperation error" );

	return UPDATE_PAYMENT_ERR;
}

// 余额来源记录查询-划拨冲正
int PaymentInfoSql::qryBalanceSource(long lOptId,vector<BalanceSourceInfo *>&vBalanceSource)
{
	string  sql;
        BalanceSourceInfo *pBalanceSourceInfo=NULL;
        int iRet;
    try 
    {
	    	sql.clear();

		sql ="  SELECT A.OPER_INCOME_ID,";
		sql +="       A.ACCT_BALANCE_ID,";
		sql +="       A.OPER_TYPE,";
		sql +="       TO_CHAR(A.OPER_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       A.AMOUNT,";
		sql +="       SOURCE_TYPE,";
		/*
		sql +="       SOURCE_DESC,";
		sql +="       STATE,";
		sql +="       TO_CHAR(STATE_DATE, 'YYYYMMDDHH24MISS'),";
		*/
		sql +="       A.OPT_ID,";
		sql +="       A.BALANCE,";
		/*

		sql +="       NVL(ALLOW_DRAW, ''),";

		sql +="       NVL(INV_OFFER, ''),";

		sql +="       NVL(CURR_STATE, ''),";

		sql +="       TO_CHAR(CURR_DATE, 'YYYYMMDDHH24MISS'),";

		*/
		sql +="       A.REMAIN_AMOUNT,";
		sql +="       A.STAFF_ID";
		/*

		sql +="       NVL(ACCT_BALANCE_OBJ_ID, -1),";

		sql +="       NVL(EFF_DATE_OFFSET, 0),";

		sql +="       NVL(EXP_DATE_OFFSET, 0)";

		*/
		sql +="  FROM BALANCE_SOURCE A ";
		sql +=" WHERE A.OPT_ID =:p0";
		
    		__DEBUG_LOG1_(0, "queryBalanceSourceInfo sql=[%s]",sql.c_str() );
    		__DEBUG_LOG1_(0, "queryBalanceSourceInfo m_lOptID=[%d]",lOptId ); // 结构体中未定义OPT_ID

		m_poPublicConn->Prepare(sql.c_str());

		m_poPublicConn->setParam(1, lOptId);//流水号

		m_poPublicConn->Execute();

		while (!m_poPublicConn->FetchNext()) 
		{

			pBalanceSourceInfo =new BalanceSourceInfo;

			m_poPublicConn->getColumn(1, &pBalanceSourceInfo->m_lOperIncomeID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lOperIncomeID=[%d]",pBalanceSourceInfo->m_lOperIncomeID );

			m_poPublicConn->getColumn(2, &pBalanceSourceInfo->m_lAcctBalanceId);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAcctBalanceId=[%d]",pBalanceSourceInfo->m_lAcctBalanceId );

			m_poPublicConn->getColumn(3, pBalanceSourceInfo->m_sOperType);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperType=[%s]",pBalanceSourceInfo->m_sOperType );

			m_poPublicConn->getColumn(4, pBalanceSourceInfo->m_sOperDate);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sOperDate=[%s]",pBalanceSourceInfo->m_sOperDate );

			m_poPublicConn->getColumn(5, &pBalanceSourceInfo->m_lAmount);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lAmount=[%d]",pBalanceSourceInfo->m_lAmount );
			m_poPublicConn->getColumn(6, pBalanceSourceInfo->m_sSourceType);
    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_sSourceType=[%s]",pBalanceSourceInfo->m_sSourceType );

			/*


			m_poPublicConn->getColumn(7, pBalanceSourceInfo->m_sSourceDesc);

			m_poPublicConn->getColumn(8, pBalanceSourceInfo->m_sState);

			m_poPublicConn->getColumn(9, pBalanceSourceInfo->m_sStateDate);

			*/

			m_poPublicConn->getColumn(7, &pBalanceSourceInfo->m_lPaymentID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lPaymentID=[%d]",pBalanceSourceInfo->m_lPaymentID);

			m_poPublicConn->getColumn(8, &pBalanceSourceInfo->m_lBalance);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_lBalance=[%d]",pBalanceSourceInfo->m_lBalance);

			/*

			m_poPublicConn->getColumn(12, pBalanceSourceInfo->m_sAllowDraw);

			m_poPublicConn->getColumn(13, pBalanceSourceInfo->m_sInvOffer);

			m_poPublicConn->getColumn(14, pBalanceSourceInfo->m_sCurrDate);

			*/

			if(m_poPublicConn->isColumnNull(9))
			{
				pBalanceSourceInfo->m_lRemainAmount=0;
			}
			else
			{
				m_poPublicConn->getColumn(9, &pBalanceSourceInfo->m_lRemainAmount);
			}
			m_poPublicConn->getColumn(10, &pBalanceSourceInfo->m_iStaffID);

    			__DEBUG_LOG1_(0, "QueryBalanceSourceInfo pBalanceSourceInfo->m_iStaffID=[%d]",pBalanceSourceInfo->m_iStaffID);
			/*

			m_poPublicConn->getColumn(17, &pBalanceSourceInfo->m_lAcctBalanceObjID);

			m_poPublicConn->getColumn(18, &pBalanceSourceInfo->m_lEffDateOffset);

			m_poPublicConn->getColumn(19, &pBalanceSourceInfo->m_lExpDateOffset);

			*/

			vBalanceSource.push_back(pBalanceSourceInfo);
			
			iRet = 0;
		}

		m_poPublicConn->Close();
		return iRet;
    }
    catch (TTStatus oSt) {

        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryPayoutInfo oSt.err_msg=%s", oSt.err_msg);

    }

    __DEBUG_LOG0_(0, "qryPayoutInfo error" );

    return QRY_BALANCE_SOURCE_ERR;
	
}

// 更新余额帐本-划拨冲正
int PaymentInfoSql::updateAcctBalance(long &lAcctBalanceId,long &lPaymentAmount)
{
	try 
	    {
			m_pRUpdateBalance->setParam(1,lPaymentAmount);
	         	
			m_pRUpdateBalance->setParam(2,lAcctBalanceId);
			
			m_pRUpdateBalance->Execute();
	         	__DEBUG_LOG0_(0, "updateAcctBalance ok");
	
			//m_pRUpdateBalance->Commit();
			m_pRUpdateBalance->Close();
			return 0;
	    }
	    catch (TTStatus oSt) {
	        ADD_EXCEPT1(oExp, "PaymentInfoSql::updateAcctBalance oSt.err_msg=%s", oSt.err_msg);
	    }
	    __DEBUG_LOG0_(0, "updateAcctBalance error" );
	    return UPDATE_ACCT_BALANCE_ERR;
}
int PaymentInfoSql::QryPaymentInfo(long &lPaymentID,PaymentInfoALLCCR &pPaymentInfoCCR)
{
    string  sql;
    bool  bstate=false;
    try 
    {
	    	sql.clear();
		sql =" SELECT A.OPT_ID,";
		//sql +="       A.FOREIGN_ID,";
		//sql +="       A.CAPABILITY_CODE,";
		sql +="       A.SERV_ID,";
		sql +="       A.REC_TYPE,";
		//sql +="       A.RBK_OPT_ID,";
		sql +="       TO_CHAR(A.OPT_DATE, 'YYYYMMDDHH24MISS'),";
		sql +="       A.SP_ID,";
		sql +="       A.SERVICE_PLATFORM_ID,";
		//sql +="       A.ORDER_INFO,";
		sql +="       A.ORDER_ID,";
		//sql +="       A.ORDER_DESC,";
		//sql +="       A.ORDER_STATE,";
		sql +="       A.PAY_TYPE,";
		sql +="       A.AMOUNT";
		sql +="   FROM OPERATION A WHERE A.OPT_ID = :OPT_ID AND A.ORDER_STATE='C0C' AND A.CAPABILITY_CODE='0001' ";
		
		m_poPublicConn->Prepare(sql.c_str());
		m_poPublicConn->setParam(1, lPaymentID);
    		__DEBUG_LOG1_(0, "QryPaymentInfo lPaymentID =[%ld]",lPaymentID);

		m_poPublicConn->Execute();
		if(!m_poPublicConn->FetchNext()) 
		{
    			__DEBUG_LOG0_(0, "QryPaymentInfo begin" );
			m_poPublicConn->getColumn(1, &(pPaymentInfoCCR.m_lPaymentID));
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_lPaymentID=[%ld]",pPaymentInfoCCR.m_lPaymentID);
			m_poPublicConn->getColumn(2, &(pPaymentInfoCCR.m_lServID));
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_lServID=[%ld]",pPaymentInfoCCR.m_lServID);
			m_poPublicConn->getColumn(3, pPaymentInfoCCR.m_sRecType);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sRecType=[%s]",pPaymentInfoCCR.m_sRecType);
			m_poPublicConn->getColumn(4, pPaymentInfoCCR.m_sOptDate);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sOptDate=[%s]",pPaymentInfoCCR.m_sOptDate);
			m_poPublicConn->getColumn(5, pPaymentInfoCCR.m_sSpID);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sSpID=[%s]",pPaymentInfoCCR.m_sSpID);
			m_poPublicConn->getColumn(6, pPaymentInfoCCR.m_sServicePlatformID);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sServicePlatformID=[%s]",pPaymentInfoCCR.m_sServicePlatformID);
			m_poPublicConn->getColumn(7, pPaymentInfoCCR.m_sOrderID);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sOrderID=[%s]",pPaymentInfoCCR.m_sOrderID);
			m_poPublicConn->getColumn(8, pPaymentInfoCCR.m_sPayType);
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_sPayType=[%s]",pPaymentInfoCCR.m_sPayType);
			m_poPublicConn->getColumn(9,&(pPaymentInfoCCR.m_lPayAmount));
    			__DEBUG_LOG1_(0, "QryPaymentInfo->m_lPayAmount=[%ld]",pPaymentInfoCCR.m_lPayAmount);
			bstate=true;
		
		}
		m_poPublicConn->Close();
		if(bstate)
			return 1;
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::QryPaymentInfo oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "QryPaymentInfo error" );
    return QRY_PAYMENT_ERR;
}
int PaymentInfoSql::QueryOptID(char *m_sForeignID,long &lPaymentID)
{
    string  sql;
    bool  bstate=false;
    try 
    {
	    	sql.clear();
		sql =" SELECT A.OPT_ID   FROM OPERATION A WHERE A.FOREIGN_ID = :FOREIGN_ID AND A.ORDER_STATE='C0C' AND A.CAPABILITY_CODE='0001' ";
		
		m_poPublicConn->Prepare(sql.c_str());
		m_poPublicConn->setParam(1, m_sForeignID);
    		__DEBUG_LOG1_(0, "QueryOptID m_sForeignID =[%s]", m_sForeignID);

		m_poPublicConn->Execute();
		if(!m_poPublicConn->FetchNext()) 
		{
			m_poPublicConn->getColumn(1, &lPaymentID);
    			__DEBUG_LOG1_(0, "QueryOptID lPaymentID==[%ld]",lPaymentID );
			bstate=true;
		
		}
		m_poPublicConn->Close();
		if(bstate)
			return 1;
		return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "PaymentInfoSql::QueryOptID oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "QueryOptID error" );
    return QRY_PAYMENT_ERR;
}

// 根据被冲正流水号查找业务流水号

int PaymentInfoSql::qryOptidByForeignId(char *sForeignId,long &lOptId)
{
    string  sql;
    int iRet = 1;
    try 
    {
    	sql.clear();

	sql ="  SELECT A.OPT_ID,";
	sql +="       A.FOREIGN_ID ";
	sql +="  FROM OPERATION A ";
	sql +="  WHERE A.ORDER_STATE='C0C' AND A.FOREIGN_ID =:p0";

	m_poPublicConn->Prepare(sql.c_str());
	m_poPublicConn->setParam(1, sForeignId);//流水号
	__DEBUG_LOG1_(0, "sForeignId=[%s]",sForeignId);
	m_poPublicConn->Execute();

	while (!m_poPublicConn->FetchNext()) 
	{
		m_poPublicConn->getColumn(1, &lOptId);

		__DEBUG_LOG1_(0, "PaymentInfoSql::qryOptidByForeignId,opt_id=[%d]",lOptId );
		
		iRet = 0;
	}

	m_poPublicConn->Close();
	return iRet;
    }
    catch (TTStatus oSt) {

        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryOptidByForeignId  oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "qryOptidByForeignId error" );
    
    iRet = QRY_PAYMENT_ERR;
    return iRet;

}

// 扩展一个方法，冲正时还需要用户标识
int PaymentInfoSql::qryOptInfoByForeignId(char *sForeignId,long &lOptId,long &lServId)
{
    string  sql;
    int iRet = 1;
    try 
    {
    	sql.clear();

	sql ="  SELECT A.OPT_ID,";
	sql +="	       A.SERV_ID,";
	sql +="        A.FOREIGN_ID ";
	sql +="  FROM OPERATION A ";
	sql +="  WHERE A.ORDER_STATE='C0C' AND A.FOREIGN_ID =:p0";

	m_poPublicConn->Prepare(sql.c_str());
	m_poPublicConn->setParam(1, sForeignId);//流水号
	__DEBUG_LOG1_(0, "sForeignId=[%s]",sForeignId);
	m_poPublicConn->Execute();

	while (!m_poPublicConn->FetchNext()) 
	{
		m_poPublicConn->getColumn(1, &lOptId);
		m_poPublicConn->getColumn(2, &lServId);
		__DEBUG_LOG1_(0, "PaymentInfoSql::qryOptidByForeignId,opt_id=[%d]",lOptId );
		
		iRet = 0;
	}

	m_poPublicConn->Close();
	return iRet;
    }
    catch (TTStatus oSt) {

        ADD_EXCEPT1(oExp, "PaymentInfoSql::qryOptidByForeignId  oSt.err_msg=%s", oSt.err_msg);
    }
    __DEBUG_LOG0_(0, "qryOptidByForeignId error" );
    
    iRet = QRY_PAYMENT_ERR;
    return iRet;

}