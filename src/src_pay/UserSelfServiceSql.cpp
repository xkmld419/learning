#include "UserSelfServiceSql.h"
#include "ABMException.h"
#include "LogV2.h"


using namespace std;

extern TimesTenCMD *m_poPublicConn;
extern TimesTenCMD *m_pBalanceTransReset;//更新划拨规则

PayAbilityQuerySql::PayAbilityQuerySql()
{
}

PayAbilityQuerySql::~PayAbilityQuerySql()
{
}

/* 
 * 根据号码支付状态
 *
 */
int PayAbilityQuerySql::PayAbilityStatusInfo(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 0;
	try 
	{
		string sPreSql;
		sPreSql = "select PAY_STATUS from serv where acc_nbr=:p0";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,pUserSelfService.m_payStatus);
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PayAbilityStatusInfo oSt.err_msg=%s", oSt.err_msg);
	}
	iRet = -1;
	
	return iRet;	
}



BalanceTransQuerySql::BalanceTransQuerySql()
{
}

BalanceTransQuerySql::~BalanceTransQuerySql()
{
}

/* 
 * 根据号码及号码类型，查询用户、帐户信息
 *
 */
int BalanceTransQuerySql::BalanceTransInfo(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{
		string sPreSql;
		sPreSql = "select c.DATA_TRANS_FREQ,c.TRANS_LIMIT,c.MONTH_TRANS_LIMIT,c.AUTO_TRANS_FLAG,c.AUTO_TRANS_THRD,c.AUTO_TRANS_AMOUNT "
			"from serv a,serv_acct b,TRANSFER_RULES c where a.acc_nbr=:p0 and a.serv_id=b.serv_id and b.acct_id=c.acct_id";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取划拨规则信息
			m_poPublicConn->getColumn(1,&pUserSelfService.m_dateTransFreq);
			m_poPublicConn->getColumn(2,&pUserSelfService.m_transLimit);
			m_poPublicConn->getColumn(3,&pUserSelfService.m_monthTransLimit);
			m_poPublicConn->getColumn(4,&pUserSelfService.m_autotransflag);
			m_poPublicConn->getColumn(5,&pUserSelfService.m_autobalancelimit);
			m_poPublicConn->getColumn(6,&pUserSelfService.m_autoAmountlimit);
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::BalanceTransQuerySql oSt.err_msg=%s", oSt.err_msg);
	}
	iRet = -1;
	
	return iRet;	
}



BalanceTransResetSql::BalanceTransResetSql()
{
}

BalanceTransResetSql::~BalanceTransResetSql()
{
}

/* 
 * 根据号码及号码类型，查询用户、帐户信息
 *
 */
int BalanceTransResetSql::BalanceTransResetInfo(UserSelfService &pUserSelfService,ABMException &oExp)
{

	try 
	{__DEBUG_LOG0_(0, "---------------sql-------------");
		__DEBUG_LOG1_(0, "m_dateTransFreq:%d",pUserSelfService.m_dateTransFreq);
		__DEBUG_LOG1_(0, "m_transLimit:%d",pUserSelfService.m_transLimit);
		__DEBUG_LOG1_(0, "m_monthTransLimit:%d",pUserSelfService.m_monthTransLimit);
		__DEBUG_LOG1_(0, "m_autotransflag:%d",pUserSelfService.m_autotransflag);
		__DEBUG_LOG1_(0, "m_autobalancelimit:%d",pUserSelfService.m_autobalancelimit);
		__DEBUG_LOG1_(0, "m_autoAmountlimit:%d",pUserSelfService.m_autoAmountlimit);
		__DEBUG_LOG1_(0, "m_servNbr:%s",pUserSelfService.m_servNbr);
		
		char sTmp[2];
		memset(sTmp,0,sizeof(sTmp));
		
		sprintf(sTmp,"%d",pUserSelfService.m_autotransflag);
		
		m_pBalanceTransReset->setParam(1,pUserSelfService.m_dateTransFreq);
		m_pBalanceTransReset->setParam(2,pUserSelfService.m_transLimit);
		m_pBalanceTransReset->setParam(3,pUserSelfService.m_monthTransLimit);
		m_pBalanceTransReset->setParam(4,sTmp);
		m_pBalanceTransReset->setParam(5,pUserSelfService.m_autobalancelimit);
		m_pBalanceTransReset->setParam(6,pUserSelfService.m_autoAmountlimit);
		m_pBalanceTransReset->setParam(7,pUserSelfService.m_servNbr);
		
		m_pBalanceTransReset->Execute();

		m_pBalanceTransReset->Close();
		
		__DEBUG_LOG0_(0, "updateBalanceTrans OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::BalanceTransResetInfo oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "updateBalanceTrans err");
	return -1;	
}



PasswordResetSql::PasswordResetSql()
{
}

PasswordResetSql::~PasswordResetSql()
{
}

/* 
 * 根据号码修改支付密码
 *
 */
int PasswordResetSql::PasswordResetUpdate(UserSelfService &pUserSelfService,ABMException &oExp)
{

	try 
	{   __DEBUG_LOG0_(0, "---------------sql-------------");
		__DEBUG_LOG1_(0, "m_newPassword:%s",pUserSelfService.m_newPassword);
		__DEBUG_LOG1_(0, "m_servNbr:%s",pUserSelfService.m_servNbr);
	
		
		m_pPasswordResetUpdate->setParam(1,pUserSelfService.m_newPassword);
		m_pPasswordResetUpdate->setParam(2,pUserSelfService.m_servNbr);
		
		m_pPasswordResetUpdate->Execute();

		m_pPasswordResetUpdate->Close();
		
		__DEBUG_LOG0_(0, "PasswordResetUpdate OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PasswordResetUpdate oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "PasswordResetUpdate err");
	return -1;	
}

/* 
 * 根据号码删除支付密码
 *
 */
int PasswordResetSql::PasswordResetdelete(UserSelfService &pUserSelfService,ABMException &oExp)
{

	try 
	{   __DEBUG_LOG0_(0, "---------------sql-------------");
		__DEBUG_LOG1_(0, "m_servNbr:%s",pUserSelfService.m_servNbr);
	
		m_pPasswordResetDelete->setParam(1,pUserSelfService.m_servNbr);
		
		m_pPasswordResetDelete->Execute();

		m_pPasswordResetDelete->Close();
		
		__DEBUG_LOG0_(0, "PasswordResetdelete OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PasswordResetdelete oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "PasswordResetdelete err");
	return -1;	
}


/* 
 * 根据号码新建支付密码
 *
 */
int PasswordResetSql::PasswordResetinsert(UserSelfService &pUserSelfService,ABMException &oExp)
{

	try 
	{   __DEBUG_LOG0_(0, "---------------sql-------------");
		__DEBUG_LOG1_(0, "m_userInfoId:%s",pUserSelfService.m_userInfoId);
		__DEBUG_LOG1_(0, "m_newPassword:%s",pUserSelfService.m_newPassword);
	
		m_pPasswordResetInsert->setParam(1,pUserSelfService.m_userInfoId);
		m_pPasswordResetInsert->setParam(2,pUserSelfService.m_newPassword);
	
		
		m_pPasswordResetInsert->Execute();

		m_pPasswordResetInsert->Close();
		
		__DEBUG_LOG0_(0, "PasswordResetinsert OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PasswordResetinsert oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "PasswordResetinsert err");
	return -1;	
}

int PasswordResetSql::PasswordResetSelect(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{
		string sPreSql;
		sPreSql = "select serv_id from serv where acc_nbr=:p0";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,pUserSelfService.m_userInfoId);
			__DEBUG_LOG1_(0, "m_userInfoId:%s",pUserSelfService.m_userInfoId);
			if(strlen(pUserSelfService.m_userInfoId)==0)
			{
				return -1;	
			}
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PasswordResetSelect oSt.err_msg=%s", oSt.err_msg);
	};
	
	return -1;	
}

int PasswordResetSql::PasswordResetAuthen(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{
		string sPreSql;
		sPreSql = "select a.ATTR_VALUES from ATT_USER_INFO a where a.serv_id in(select b.serv_id from serv b where b.acc_nbr=:p0) and a.ATTR_TYPE='04'";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,pUserSelfService.m_payPassword);
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PasswordResetAuthen oSt.err_msg=%s", oSt.err_msg);
	}
	iRet = -1;
	
	return iRet;	
}



PayStatusResetSql::PayStatusResetSql()
{
}

PayStatusResetSql::~PayStatusResetSql()
{
}

/* 
 * 根据号码修改支付密码
 *
 */
int PayStatusResetSql::PayStatusResetUpdate(UserSelfService &pUserSelfService,ABMException &oExp)
{

	try 
	{   __DEBUG_LOG0_(0, "---------------sql-------------");
		__DEBUG_LOG1_(0, "m_payPassword:%s",pUserSelfService.m_payPassword);
		__DEBUG_LOG1_(0, "m_servNbr:%s",pUserSelfService.m_servNbr);
	
		
		m_pPayStatusResetUpdate->setParam(1,pUserSelfService.m_payStatus);
		m_pPayStatusResetUpdate->setParam(2,pUserSelfService.m_servNbr);
		
		m_pPayStatusResetUpdate->Execute();

		m_pPayStatusResetUpdate->Close();
		
		__DEBUG_LOG0_(0, "PayStatusResetUpdate OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PayStatusResetUpdate oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "PayStatusResetUpdate err");
	return -1;	
}



PayRulesQuerySql::PayRulesQuerySql()
{
}

PayRulesQuerySql::~PayRulesQuerySql()
{
}

/* 
 * 根据号码支付状态
 *
 */
int PayRulesQuerySql::PayRulesQueryInfo(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{
		string sPreSql;
		sPreSql = "select c.DATA_PAY_FREQ,c.PAY_LIMIT,c.MONTH_PAY_LIMIT,c.MICRO_PAY_LIMIT "
			"from serv a,serv_acct b,TRANSFER_RULES c where a.acc_nbr=:p0 and a.serv_id=b.serv_id and b.acct_id=c.acct_id";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,&pUserSelfService.m_datePayFreq);
			m_poPublicConn->getColumn(2,&pUserSelfService.m_payLimit);
			m_poPublicConn->getColumn(3,&pUserSelfService.m_monthPayLimit);
			m_poPublicConn->getColumn(4,&pUserSelfService.m_microPayLimit);
			__DEBUG_LOG0_(0, "----------SQL---------");
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PayRulesQueryInfo oSt.err_msg=%s", oSt.err_msg);
	}
	
	return -1;	
}



PayRulesResetSql::PayRulesResetSql()
{
}

PayRulesResetSql::~PayRulesResetSql()
{
}

/* 
 * 变更支付规则
 *
 */
int PayRulesResetSql::PayRulesUpdate(UserSelfService &pUserSelfService,ABMException &oExp)
{
	try 
	{	__DEBUG_LOG0_(0, "---------------sql-------------");
	
		
		m_pPayRulesReset->setParam(1,pUserSelfService.m_datePayFreq);
		m_pPayRulesReset->setParam(2,pUserSelfService.m_payLimit);
		m_pPayRulesReset->setParam(3,pUserSelfService.m_monthPayLimit);
		m_pPayRulesReset->setParam(4,pUserSelfService.m_microPayLimit);
		m_pPayRulesReset->setParam(5,pUserSelfService.m_servNbr);
		
		m_pPayRulesReset->Execute();

		m_pPayRulesReset->Close();
		
		__DEBUG_LOG0_(0, "PayRulesUpdate OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PayRulesUpdate oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "PayRulesUpdate err");
	return -1;	
}


BindingAccNbrQuerySql::BindingAccNbrQuerySql()
{
}

BindingAccNbrQuerySql::~BindingAccNbrQuerySql()
{
}

/* 
 * 根据号码支付状态
 *
 */
int BindingAccNbrQuerySql::BindingAccNbrSelect(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 0;
	try 
	{__DEBUG_LOG0_(0, "---------------sql-------------");
		string sPreSql;
		sPreSql = "select ATTR_VALUES from att_user_info where  serv_id in (select serv_id from serv where acc_nbr=:p0) and ATTR_TYPE ='01'";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,pUserSelfService.m_randomAccNbr);
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "UserSelfServiceSql::PayAbilityStatusInfo oSt.err_msg=%s", oSt.err_msg);
	}
	iRet = -1;
	
	return iRet;	
}


TransferRulesSql::TransferRulesSql()
{
}

TransferRulesSql::~TransferRulesSql()
{
}

/* 
 * 查询划拨支付规则
 *
 */
int TransferRulesSql::TransferRulesQuery(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{__DEBUG_LOG0_(0, "---------------TransferRulesQuery:SQL-------------");
		string sPreSql;
		sPreSql = "select DATA_TRANS_FREQ,TRANS_LIMIT,MONTH_TRANS_LIMIT,AUTO_TRANS_FLAG,AUTO_TRANS_THRD,AUTO_TRANS_AMOUNT,DATA_PAY_FREQ,"
		"PAY_LIMIT,MONTH_PAY_LIMIT,MICRO_PAY_LIMIT from TRANSFER_RULES where acct_id in( select b.acct_id from serv a,serv_acct b where a.acc_nbr=:p0 and a.serv_id=b.serv_id)";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		//m_poPublicConn->Commit();
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			/*
			m_poPublicConn->getColumn(1,&pUserSelfService.m_dateTransFreq);
			m_poPublicConn->getColumn(2,&pUserSelfService.m_transLimit);
			m_poPublicConn->getColumn(3,&pUserSelfService.m_monthTransLimit);
			m_poPublicConn->getColumn(4,&pUserSelfService.m_autotransflag);
			m_poPublicConn->getColumn(5,&pUserSelfService.m_autobalancelimit);
			m_poPublicConn->getColumn(6,&pUserSelfService.m_autoAmountlimit);
			m_poPublicConn->getColumn(7,&pUserSelfService.m_datePayFreq);
			m_poPublicConn->getColumn(8,&pUserSelfService.m_payLimit);
			m_poPublicConn->getColumn(9,&pUserSelfService.m_monthPayLimit);
			m_poPublicConn->getColumn(10,&pUserSelfService.m_microPayLimit);
			*/
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
			__DEBUG_LOG1_(0, "TransferRulesInsert OK,iRet:%d",iRet);
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "TransferRulesSql::TransferRulesQuery oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG1_(0, "TransferRulesInsert NULL,iRet:%d",iRet);
	return iRet;	
}

/* 
 * 查询账户标识
 *
 */
int TransferRulesSql::TransferRulesAcctId(UserSelfService &pUserSelfService,ABMException &oExp)
{
	char sql[1024];
	bool bFlag = false;
	int iRet = 1;
	try 
	{__DEBUG_LOG0_(0, "---------------TransferRulesAcctId:sql-------------");
		string sPreSql;
		sPreSql = "select acct_id from serv_acct where serv_id in(select serv_id from serv where acc_nbr=:p0)";
		
		strcpy(sql,sPreSql.c_str());	
		snprintf(sql, sizeof(sql), "%s", sql);
		m_poPublicConn->Prepare(sql);
		
		m_poPublicConn->setParam(1,pUserSelfService.m_servNbr);
		m_poPublicConn->Execute();
		
		while (!m_poPublicConn->FetchNext()) 
		{	// 取用户帐户信息
			m_poPublicConn->getColumn(1,&pUserSelfService.m_acctId);
			
			bFlag = true;
		}	
		m_poPublicConn->Close();
		if(bFlag)
		{
			iRet = 0;
		}
		
		return iRet;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "TransferRulesSql::TransferRulesAcctId oSt.err_msg=%s", oSt.err_msg);
	}
	iRet = -1;
	
	return iRet;	
}


/* 
 * 变更支付规则
 *
 */
int TransferRulesSql::TransferRulesInsert(UserSelfService &pUserSelfService,ABMException &oExp)
{
	try 
	{	__DEBUG_LOG0_(0, "---------------TransferRulesInsert:sql-------------");
		
		char sTmp[8];
		memset(sTmp,0,sizeof(sTmp));
		
		sprintf(sTmp,"%d",pUserSelfService.m_autotransflag);
	
		m_pTransferRulesInsert->setParam(1,pUserSelfService.m_acctId);
		
		m_pTransferRulesInsert->setParam(2,pUserSelfService.m_dateTransFreq);
		m_pTransferRulesInsert->setParam(3,pUserSelfService.m_transLimit);
		m_pTransferRulesInsert->setParam(4,pUserSelfService.m_monthTransLimit);
		m_pTransferRulesInsert->setParam(5,sTmp);
		m_pTransferRulesInsert->setParam(6,pUserSelfService.m_autobalancelimit);
		m_pTransferRulesInsert->setParam(7,pUserSelfService.m_autoAmountlimit);
		m_pTransferRulesInsert->setParam(8,pUserSelfService.m_datePayFreq);
		m_pTransferRulesInsert->setParam(9,pUserSelfService.m_payLimit);
		m_pTransferRulesInsert->setParam(10,pUserSelfService.m_monthPayLimit);
		m_pTransferRulesInsert->setParam(11,pUserSelfService.m_microPayLimit);
		
		__DEBUG_LOG1_(0, "m_acctId:%d",pUserSelfService.m_acctId);
		__DEBUG_LOG1_(0, "m_dateTransFreq:%d",pUserSelfService.m_dateTransFreq);
		__DEBUG_LOG1_(0, "m_transLimit:%d",pUserSelfService.m_transLimit);
		__DEBUG_LOG1_(0, "m_monthTransLimit:%d",pUserSelfService.m_monthTransLimit);
		__DEBUG_LOG1_(0, "m_autotransflag:%s",pUserSelfService.m_autotransflag);
		__DEBUG_LOG1_(0, "m_autobalancelimit:%d",pUserSelfService.m_autobalancelimit);
		__DEBUG_LOG1_(0, "m_autoAmountlimit:%d",pUserSelfService.m_autoAmountlimit);
		__DEBUG_LOG1_(0, "m_datePayFreq:%d",pUserSelfService.m_datePayFreq);
		__DEBUG_LOG1_(0, "m_payLimit:%d",pUserSelfService.m_payLimit);
		__DEBUG_LOG1_(0, "m_monthPayLimit:%d",pUserSelfService.m_monthPayLimit);
		__DEBUG_LOG1_(0, "m_microPayLimit:%d",pUserSelfService.m_microPayLimit);
		
		
		m_pTransferRulesInsert->Execute();

		m_pTransferRulesInsert->Close();
		
		__DEBUG_LOG0_(0, "TransferRulesInsert OK");
		return 0;
	}
	catch (TTStatus oSt)
	{
		ADD_EXCEPT1(oExp, "TransferRulesSql::TransferRulesInsert oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG0_(0, "TransferRulesInsert err");
	return -1;	
}
