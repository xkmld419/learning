/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：checkFunc.h 
* 摘    要：文件定义了用于校验配置的所有函数
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年11月11日
*/

#ifndef __CHECK_FUNC_
#define __CHECK_FUNC_

#include "string.h"
#include "Date.h"


/* ************ 单个值比较*********** */
int compare_H(const int & src, const int & dest);
int compare_H(const long & src, const long & dest);
int compare_H(const char* src, const char* dest);

/* ************ 日期值比较*********** */
inline int biggerDateCheck_H(const char* src, const char* dest, const long & second){
	if (!src) return -1;
	
	long s = second;

	if (strcmp(src, dest) <= 0) return 0;

	
	Date srcDate(src);
	Date destDate;
	if (!dest) destDate.parse(dest);

	return destDate.diffSec(srcDate) >= second ? 0 : 1;
}

/* ************ 日期值比较*********** */
inline int lowerDateCheck_H(const char* src, const char* dest, const long & second) {
	if (!src) return -1;
	
	long s = second;

	if (strcmp(src, dest) >= 0) return 0;

	
	Date srcDate(src);
	Date destDate;
	if (!dest) destDate.parse(dest);

	return srcDate.diffSec(destDate) >= second ? 0 : 1;
}

#endif

