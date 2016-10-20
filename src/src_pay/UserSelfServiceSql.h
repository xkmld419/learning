#ifndef __USERSELF_SERVICE_SQL_H_INCLUDED_
#define __USERSELF_SERVICE_SQL_H_INCLUDED_

#include "TTTns.h"
#include "dcc_ra.h"
#include "PublicConndb.h"

class ABMException;

class PayAbilityQuerySql : public TTTns
{
	public:
		PayAbilityQuerySql();
		~PayAbilityQuerySql();

		// 支付状态信息查询
		int PayAbilityStatusInfo(UserSelfService &pUserSelfService,ABMException &oExp);

	
};


class BalanceTransQuerySql : public TTTns
{
	public:
		BalanceTransQuerySql();
		~BalanceTransQuerySql();

		// 划拨规则查询
		int BalanceTransInfo(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class BalanceTransResetSql : public TTTns
{
	public:
		BalanceTransResetSql();
		~BalanceTransResetSql();

		// 划拨规则设置
		int BalanceTransResetInfo(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class PasswordResetSql : public TTTns
{
	public:
		PasswordResetSql();
		~PasswordResetSql();

		// 划拨规则设置
		int PasswordResetUpdate(UserSelfService &pUserSelfService,ABMException &oExp);
		int PasswordResetSelect(UserSelfService &pUserSelfService,ABMException &oExp);
		int PasswordResetdelete(UserSelfService &pUserSelfService,ABMException &oExp);
		int PasswordResetinsert(UserSelfService &pUserSelfService,ABMException &oExp);
		int PasswordResetAuthen(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class PayStatusResetSql : public TTTns
{
	public:
		PayStatusResetSql();
		~PayStatusResetSql();

		// 划拨规则设置
		int PayStatusResetUpdate(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class PayRulesQuerySql : public TTTns
{
	public:
		PayRulesQuerySql();
		~PayRulesQuerySql();

		// 划拨规则设置
		int PayRulesQueryInfo(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class PayRulesResetSql : public TTTns
{
	public:
		PayRulesResetSql();
		~PayRulesResetSql();

		// 划拨规则设置
		int PayRulesUpdate(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class BindingAccNbrQuerySql : public TTTns
{
	public:
		BindingAccNbrQuerySql();
		~BindingAccNbrQuerySql();

		// 划拨规则设置
		int BindingAccNbrSelect(UserSelfService &pUserSelfService,ABMException &oExp);

	
};

class TransferRulesSql : public TTTns
{
	public:
		TransferRulesSql();
		~TransferRulesSql();

		// 划拨规则设置
		int TransferRulesQuery(UserSelfService &pUserSelfService,ABMException &oExp);
		int TransferRulesAcctId(UserSelfService &pUserSelfService,ABMException &oExp);
		int TransferRulesInsert(UserSelfService &pUserSelfService,ABMException &oExp);
};

#endif/*__ALLOCATE_BALANCE_SQL_H_INCLUDED_*/

