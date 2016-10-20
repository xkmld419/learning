#ifndef HASHLIST_H
#define HASHLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h_list.h"

#define HASH_NUM	256		/* HASH表大小 */

#define THashList	list_t **

/*
 *	初始化HASH表
 */
void initHashList (THashList *h_list);

/*
 *	以给定KEY为搜索键的HASH链表
 *	hash(KEY)相同的数据接到相同的链表，供以后查找用
 *	pre:  第一次调用时，要满足*keylist=NULL
 */
int insertHashList (THashList h_list, const char *key, void *data, int data_size);

/*
 *	搜索HASH链表
 *
 *	key	为键值
 *	data	为比较数据
 *	cmp	为比较函数指针，返回值定义为返回0表示相等，其他为不等
 *		list_data为链表中的数据，cmp_data为用于比较的数据
 *
 *	返回值	指向匹配的数据的指针
 */
void *searchHashList (THashList h_list, char *key, void *cmp_data,
	int (*cmp) (const void *list_data, const void *cmp_data));

void freeHashList (THashList h_list);
#endif




