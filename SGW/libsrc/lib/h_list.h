/********************************
 *
 *	链表操作头文件
 *
 ********************************/

#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* - - - 链表节点结构 - - - */
typedef struct listnode_s
{
	struct listnode_s	*next;
	union
	{
		void		*data;
		struct list_s	*list;
		const char	*str;
	}u;
}listnode_t;

/* - - - 链表头结构 - - - */
typedef struct list_s
{
	listnode_t	*head;
	listnode_t	*tail;
}list_t;

extern int InsertList (list_t ** ppList, const void *data, size_t n);

extern void * SearchList (list_t *ppList, const void *data, 
		int (*cmp) (const void *data1, const void *data2));

extern void DeleteList (list_t *ppList, const void *data, 
		int (*cmp) (const void *data1, const void *data2));
		
extern void FreeList (list_t *);

extern void PrintList (list_t *ppList, void (*print) (const void *data));

extern void OperateList (list_t *ppList, void (*op_data) (void *data));

extern int ListIsNull (list_t *ppList);

extern void * GetFirst (list_t *ppList);

extern void DeleteFirst (list_t *ppList);

extern listnode_t *GetNode (list_t *ppList);

extern void *TravelList (list_t *ppList);

int DealList (list_t *ppList, int (*op_data) (void *data));



#endif




