#ifndef __ALLOCATE_BALANCE_SQL_H_INCLUDED_
#define __ALLOCATE_BALANCE_SQL_H_INCLUDED_

#include "TTTns.h"
#include "dcc_ra.h"
#include "PublicConndb.h"

class ABMException;

class AllocateBalanceSql : public TTTns
{
	public:
		AllocateBalanceSql();
		~AllocateBalanceSql();
		
		//int initDbConn(ABMException &oExp);
		
		// 查询用户信息
		int preQryServAcctInfo(string servNbr,int iServAttr,ServAcctInfo &oServAcct,ABMException &oExp);
		
		// 余额划拨规则查询
		int preQryDeductRule(long lAcctId,TransRule &oTransRule,ABMException &oExp);
		
		// 统计用户余额划拨频度及额度
		int preQryServDayDeductInfo(long lServId,TransRule &oTransRuleDay,TransRule &oTransRuleMonth,ABMException &oExp);
		
};

#endif/*__ALLOCATE_BALANCE_SQL_H_INCLUDED_*/

