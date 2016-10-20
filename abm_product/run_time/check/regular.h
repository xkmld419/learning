/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：regular.h 
* 摘    要：文件定义了regular express模板
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年11月10日
*/
#ifndef __REGULAR_H_
#define __REGULAR_H_

#include <regex.h>
#include <string.h>
#include "stack.h"

#ifndef NULL
#define NULL 0
#endif

class Regex {
public:
	Regex(const char* reg) {
		memset(errbuf, 0x00, sizeof(errbuf));
		regexComp(reg);
	}	

	virtual ~Regex() {
		if (errbuf) {
			delete errbuf;
		}

		regfree(&regex);
	}

	int exec(const char* str);

protected:
	Regex() {
		memset(errbuf, 0x00, sizeof(errbuf));
	}


	int regexComp(const char* reg);
public:
	regex_t 		regex; 			/* 编译后的正则表达式 */ 
	regmatch_t 		pMatchs; 		/* 匹配的字符串位置 */ 
	char 			errbuf[28]; 	/* 错误消息 */ 
	int 			compNo;
	int 			execNo;
};

#endif

