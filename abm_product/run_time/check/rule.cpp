#include "rule.h"
#include "regular.h"
#include "checkFunc.h"

const char* loadRuleSql  = "SELECT CHECK_ID, TYPE, RULE, RESULT FROM URULETABLE ORDER BY CHECK_ID";
const char* coutRuleSql  = "SELECT count(1) FROM URULETABLE ";

const char* loadURuleSql = "SELECT RULE_ID, RULE_TYPE, RULE_FLAG, RULE_VALUES FROM RULETABLE ORDER  BY RULE_ID";
const char* coutURuleSql = "SELECT count(1) FROM RULETABLE ";

RuleData* RuleData::m_poInstance = NULL;

int RuleData::init()
{
	TimesTenCMD * cursor = NULL;
	TimesTenConn * conn  = NULL;

	ABMException exp;
	const char * conInfo = "HSSTT";
	int iRet = GetTTCursor(cursor, conn, conInfo, exp);
	if (0 != iRet) {
		directLog(V3_FATAL, V3_DB, MBC_DOCHECK_CONNECTDB, "get timesTen Cursor failed(result = %d), conifo is %s.", iRet, conInfo);
		return MBC_DOCHECK_CONNECTDB;
	}

	iRet = loadURule(cursor);
	if (0 != iRet) {
		directLog(V3_FATAL, V3_DB, MBC_DOCHECK_EXECDBFAULT, "load union rule failed(result = %d).", iRet);
		return iRet;
	}

	iRet = loadRule(cursor);
	if (0 != iRet) {
		directLog(V3_FATAL, V3_DB, MBC_DOCHECK_EXECDBFAULT, "load rule failed(result = %d).", iRet);
		return iRet;
	}
	
	return iRet;
}

int RuleData::loadURule(TimesTenCMD * cursor)
{
	cursor->Close();
	cursor->Prepare(coutURuleSql);
	cursor->Commit();

	try {
		cursor->Execute();
		if(!cursor->FetchNext()) {
			long num				= -1;
			cursor->getColumn(1, &num);
			m_uiURuleSize = num;
			if (!m_uiURuleSize) {
				return 0;
			}
			m_poURule = new URule[m_uiURuleSize];
		}
		else {
			return 0;
		}
		
		cursor->Close();
		cursor->Prepare(loadURuleSql);
		cursor->Commit();
		cursor->Execute();
		int idx = 0;
		long type				= -1;
		long flag 				= -1;
		long id					= -1;
		while(!cursor->FetchNext()) {
			cursor->getColumn(1, &id);
			cursor->getColumn(2, &type);
			cursor->getColumn(3, &flag);
			cursor->getColumn(4, m_poURule[idx].ruleValues);
			m_poURule[idx].ruleId = id;
			m_poURule[idx].ruleType = type;
			m_poURule[idx].ruleFlag = flag;
			idx++;
		}
	}
	catch(TTStatus st)
	{
		cursor->Close();
		__DEBUG_LOG1_(0, "ErrorCode::ParseErrorTable failed! err_msg=%s", st.err_msg);
		return MBC_DOCHECK_EXECDBFAULT;
	}

	return 0;
}

int RuleData::loadRule(TimesTenCMD * cursor)
{
	cursor->Close();
	cursor->Prepare(coutRuleSql);
	cursor->Commit();

	try {
		cursor->Execute();
		if(!cursor->FetchNext()) {
			long num				= -1;
			cursor->getColumn(1, &num);
			m_uiRuleSize = num;
			if (!m_uiRuleSize) {
				return 0;
			}
			m_poRule = new Rule[m_uiRuleSize];
		}
		else {
			return 0;
		}
		
		cursor->Close();
		cursor->Prepare(loadRuleSql);
		cursor->Commit();
		cursor->Execute();
		int idx = 0;
		long type				= -1;
		long result				= -1;
		long id 				= -1;
		while(!cursor->FetchNext()) {
			cursor->getColumn(1, &id);
			cursor->getColumn(2, &type);
			cursor->getColumn(3, m_poRule[idx].rule);
			cursor->getColumn(4, &result);
			m_poRule[idx].checkId = id;
			m_poRule[idx].type = type;
			m_poRule[idx].result = result;
			idx++;
		}
	}
	catch(TTStatus st)
	{
		cursor->Close();
		__DEBUG_LOG1_(0, "ErrorCode::ParseErrorTable failed! err_msg=%s", st.err_msg);
		return MBC_DOCHECK_EXECDBFAULT;
	}

	return 0;
}


int RuleData::getRule(const int& idx, Rule* &rule) const
{
	if (idx < 0 || !m_uiRuleSize) {
		return NULL;
	}

	int begin	= 0;
	int end 	= m_uiRuleSize - 1;
	int interator = ( begin + end ) / 2;
	int found   = -1;
	

	while (begin <= end) {
		if (m_poRule[interator].checkId == idx) {
			found = interator;
			break;
		}
		else if (m_poRule[interator].checkId > idx) {
			end = interator - 1;
		}
		else {
			begin = interator + 1;
		}

		interator = ( begin + end ) / 2;
	}

	if (-1 == found) {
		__DEBUG_LOG1_(0, "RuleData::getRule failed! idx=%s", idx);
		return -1;
	}

	rule = m_poRule + found;
	return 0;
}

int RuleData::getURule(const int& idx, URule* &rule, int& cout) const
{
	if (idx < 0 || !m_uiURuleSize) {
		return NULL;
	}

	int begin	= 0;
	int end 	= m_uiURuleSize - 1;
	int interator = ( begin + end ) / 2;
	int found	= -1;
	

	while (begin <= end) {
		if (m_poURule[interator].ruleId == idx) {
			found = interator;
			break;
		}
		else if (m_poURule[interator].ruleId > idx) {
			end = interator - 1;
		}
		else {
			begin = interator + 1;
		}

		interator = ( begin + end ) / 2;
	}

	if (-1 == found) {
		__DEBUG_LOG1_(0, "RuleData::getURule failed! idx=%s", idx);
		return -1;
	}
	else {
		while(interator >= 0 && m_poURule[interator].ruleId == idx ) {
			interator--;
			cout++;
		}
		begin = interator + 1;
		interator = found + 1;
		found = begin;
		while(interator < m_uiURuleSize && m_poURule[interator].ruleId == idx) {
			interator++;
			cout++;
		}
	}

	rule = m_poURule + found;
	return 0;
}


RuleCheck* RuleCheck::m_poInstance = NULL;

/*
* 函数功能：校验的入口函数
* 输入参数：待校验的数据
*					idx 是被校验, argc 是指用于校验的参数个数, argv是参数值
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int RuleCheck::doCheck(int idx, int argc, Param argv[])
{
	if (argc <= 0 || idx < 0) {
		return MBC_DOCHECK_PARAMINVALID;
	}

	// 根据idx获取校验式
	Rule * rule = NULL;
	char * ruleValues = NULL;
	int iRet 	= 0;
	char cResult = 0;
	int iRuleId = 0;
	
	iRet = RuleData::getInstance()->getRule(idx, rule);

	if (iRet || !rule) {
		__DEBUG_LOG1_(0, "RuleData::getRule failed! idx=%s", idx);
		return MBC_DOCHECK_FINDRULEFAILED;
	}

	ruleValues = rule->rule;

	// 为了方便下面进行格式判定，首先插入1& 字符
	stack.push('1');
	stack.push('&');

	while ('\0' != *ruleValues) {
		switch (*ruleValues) {
		case '|' :
		case '&' : {
			if (iRuleId != -1) {
				if ('0' == stack.top() || '1' == stack.top()) {
					return MBC_DOCHECK_VALIDREGEX;
				}
				cResult = URuleheck(iRuleId, argc, argv) ? '0' : '1';
				stack.push(cResult);
				iRuleId = -1;
			}
			if ('|' == stack.top() || '&' == stack.top()
				|| '(' == stack.top() || '!' == stack.top()) {
				return MBC_DOCHECK_VALIDREGEX;
			}
			stack.push(*ruleValues);
			ruleValues++;
			break;
		}
		case '(' :
		case '!' : {
			if (iRuleId != -1) {
				return MBC_DOCHECK_VALIDREGEX;
			}
			if ('&' != stack.top() || '|' != stack.top()) {
				return MBC_DOCHECK_VALIDREGEX;
			}
			stack.push(*ruleValues);
			ruleValues++;
			break;
		}
		case ')' : {
			while (stack.low() && '(' != (cResult = stack.pop())) {
				if (cResult == '|') {
					iRet = iRet || (stack.pop() == '0' ? 0 : 1);
				}
				else if (cResult == '&') {
					iRet = iRet && (stack.pop() == '0' ? 0 : 1);
				}
				else if (cResult == '!') {
					iRet = !iRet ? 1 : 0;
				}
				else {
					iRet = (cResult == '0' ? 0 : 1);
				}
			}

			if (!stack.empty()) {
				stack.push(iRet ? '1' : '0');
			}
			else {
				return iRet == 0 ? 0 : rule->result;
			}
			ruleValues++;
			break;
		}
		default: {
			if (*ruleValues > '9' || *ruleValues < '0') {
				return MBC_DOCHECK_VALIDREGEX;
			}
			iRuleId = iRuleId == -1 ? 0 : iRuleId;
			iRuleId *= 10;
			iRuleId += int(*ruleValues) - 48;
			ruleValues++;
			break;
		}
		}
	}

	if (-1 != iRuleId) {
		if ('0' == stack.top() || '1' == stack.top()) {
			return MBC_DOCHECK_VALIDREGEX;
		}
		cResult = URuleheck(iRuleId, argc, argv) ? '0' : '1';
		stack.push(cResult);
	}

	while (stack.low() && '(' != (cResult = stack.pop())) {
		if (cResult == '|') {
			iRet = (stack.pop() == '0' ? 0 : 1) || iRet;
		}
		else if (cResult == '&') {
			iRet = (stack.pop() == '0' ? 0 : 1) && iRet;
		}
		else if (cResult == '!') {
			iRet = !iRet ? 1 : 0;
		}
		else {
			iRet = (cResult == '0' ? 0 : 1);
		}
	}

	return !iRet ? rule->result : 0;
}

/*
* 函数功能：对一个简单的校验式进行计算
* 输入参数：待校验的数据
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int RuleCheck::URuleheck(int idx, int argc, Param argv[])
{
	if (argc <= 0 || idx < 0) {
		return MBC_DOCHECK_PARAMINVALID;
	}
	
	int iRet = 0;
	int iCout = 0;
	URule * uRule = NULL;

	iRet = RuleData::getInstance()->getURule(idx, uRule, iCout);

	if (iRet || !uRule) {
		__DEBUG_LOG1_(0, "RuleData::getURule failed! idx=%s", idx);
		return MBC_DOCHECK_FINDURULEFAILED;
	}

	
	switch (uRule->ruleFlag) {
		case FUNC_ID_DEFUALT : {
			iRet = doCompare(argv[0], uRule);
			break;
		}
		case FUNC_ID_HASH : {
			iRet = 1;
			while (iCout--) {
				if (!doCompare(argv[0], uRule)) {
					iRet = 0;
					break;
				}
				uRule++;
			}
			break;
		}
		case FUNC_ID_MAXVALUES : {
			iRet = doCompare(argv[0], uRule);
			iRet = iRet <= 0 ? 0 : 1;
			break;
		}
		case FUNC_ID_MINVALUES : {
			iRet = doCompare(argv[0], uRule);
			iRet = iRet >= 0 ? 0 : 1;
			break;
		}
		case FUNC_ID_MAXLEN : {
			iRet = doCompare(argv[0], uRule);
			iRet = iRet <= 0 ? 0 : 1;
			break;
		}
		case FUNC_ID_MINLEN : {
			iRet = doCompare(argv[0], uRule);
			iRet = iRet >= 0 ? 0 : 1;
			break;
		}
		case FUNC_ID_REGEX : {
			iRet = Regex(uRule->ruleValues).exec(argv[0].values.sData);
			break;
		}
		case FUNC_ID_CURDATE_BIGLIMIT : {
			iRet = biggerDateCheck_H(argv[0].values.sData, NULL, atol(uRule->ruleValues));
			break;
		}
		case FUNC_ID_CURDATE_SMALLLIMIT : {
			iRet = lowerDateCheck_H(argv[0].values.sData, NULL, atol(uRule->ruleValues));
			break;
		}
		case FUNC_ID_SPEDATE_BIGLIMIT : {
			iRet = biggerDateCheck_H(argv[0].values.sData, argv[1].values.sData, atol(uRule->ruleValues));
			break;
		}
		case FUNC_ID_SPEDATE_SMALLLIMIT : {
			iRet = lowerDateCheck_H(argv[0].values.sData, argv[1].values.sData, atol(uRule->ruleValues));
			break;
		}
		default : {
			break;
		}
	}
	return iRet;
}

int RuleCheck::doCompare(const Param & argv, const URule * uRule)
{
	int iRet = 0;
	switch(argv.type) {
	case type_int: {
		iRet = compare_H(argv.values.iData, atoi(uRule->ruleValues));
		break;
	}
	case type_long: {
		iRet = compare_H(argv.values.lData, atol(uRule->ruleValues));
		break;
	}
	case type_string: {
		iRet = compare_H(argv.values.sData, uRule->ruleValues);
		break;
	}
	default: {
		iRet = 0;
		break;
	}
	}

	return iRet;
}

