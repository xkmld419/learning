#include "checkFunc.h"

/* ************ 单个值比较*********** */
int compare_H(const int & src, const int & dest) {
	if (src > dest) return 1;
	if (src < dest) return -1;

	return 0;
}

int compare_H(const long & src, const long & dest) {
	if (src > dest) return 1;
	if (src < dest) return -1;

	return 0;
}

int compare_H(const char* src, const char* dest) {
	if (src && !dest) return 1;
	if (!src && dest) return -1;
	if (!src && !dest) return 0;

	return strcmp(src, dest);
}

/* ************ 日期值比较*********** */
/*
inline int biggerDateCheck_H(const char* src, const char* dest, const long & second) {
	if (!src) return -1;
	
	long s = second;

	if (strcmp(src, dest) <= 0) return 0;

	
	Date srcDate(src);
	Date destDate;
	if (!dest) destDate.parse(dest);

	return destDate.diffSec(srcDate) >= second ? 0 : 1;
}

/* ************ 日期值比较*********** */
/*
inline int lowerDateCheck_H(const char* src, const char* dest, const long & second) 
*/

