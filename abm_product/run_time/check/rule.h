/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：rule.h 
* 摘    要：文件定义了用于校验配置的所有函数
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年11月11日
*/

#ifndef __RULE_H_
#define __RULE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TimesTenAccess.h"
#include "common.h"
#include "hssstack.h"


struct URule {
	int 		ruleId;
	int 		ruleType;
	int 		ruleFlag;
	char		ruleValues[128];

	URule() {
		memset(ruleValues, 0x00, sizeof(ruleValues));
	}
};

struct Rule {
	int 		checkId;
	int 		type;
	char		rule[32];
	int 		result;
	Rule() {
		memset(rule, 0x00, sizeof(rule));
	}
};

class RuleData {
public:
	static RuleData* getInstance() {
		if (!m_poInstance) {
			m_poInstance = new RuleData();

			if (!m_poInstance) {
				exit(-1);
			}
		}

		return m_poInstance;
	}

	int getRule(const int& idx, Rule* &rule) const;

	int getURule(const int& idx, URule* &rule, int& cout) const;

	int init();
protected:
	RuleData() {
		m_poURule = NULL;
		m_poRule  = NULL;

		m_uiURuleSize = 0;
		m_uiRuleSize  = 0;
	}

	virtual ~RuleData() {
		if (m_poURule) {
			delete m_poURule;
			m_poURule = NULL;
		}
		if (m_poURule) {
			delete m_poURule;
			m_poURule = NULL;
		}
		m_uiURuleSize = 0;
		m_uiRuleSize  = 0;
	}

	int loadURule(TimesTenCMD * cursor);

	int loadRule(TimesTenCMD * cursor);
public:

	URule * m_poURule;
	Rule  * m_poRule;
	unsigned int m_uiURuleSize;
	unsigned int m_uiRuleSize;

	static RuleData* m_poInstance;
};

class RuleCheck {
public:
	static RuleCheck* getInstance() {
		if (!m_poInstance) {
			m_poInstance = new RuleCheck();

			if (!m_poInstance) {
				exit(-1);
			}
			
			if (RuleData::getInstance()->init()) {
				exit(-2);
			}
		}

		return m_poInstance;
	}

	/*
	* 函数功能：校验的入口函数
	* 输入参数：待校验的数据
	*					idx 是被校验, argc 是指用于校验的参数个数, argv是参数值
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int doCheck(int idx, int argc, Param argv[]);

	
protected:
	RuleCheck():stack(64) {

	}

	virtual ~RuleCheck() {

	}

	/*
	* 函数功能：对一个简单的校验式进行计算
	* 输入参数：待校验的数据
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int URuleheck(int idx, int argc, Param argv[]);

	
	int RuleCheck::doCompare(const Param & argv, const URule * uRule);
protected:
	HSS_Stack<char>	stack;

	static RuleCheck* m_poInstance;
};

#endif

