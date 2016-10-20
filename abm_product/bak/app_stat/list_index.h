/*VER: 1*/ 
/********************************
 *
 *	链表索引操作头文件
 *
 ********************************/

#ifndef _LIST_INDEX_H_
#define _LIST_INDEX_H_

#include <stdio.h>
#include <stdlib.h>

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

/* List 部分 */
extern int InsertList (list_t **list, const void *data, size_t n);

extern int InsertListOrderByKey (list_t **list, const void *data, size_t n,
	int (*cmp) (const void *data1, const void *data2));
	
extern void * SearchList (list_t *list, const void *data, 
		int (*cmp) (const void *data1, const void *data2));

extern void DeleteList (list_t *list, const void *data, 
		int (*cmp) (const void *data1, const void *data2));
		
extern void FreeList (list_t **);

extern void PrintList (list_t *list, void (*print) (const void *data));

extern void OperateList (list_t *list, void (*op_data) (void *data));

extern int ListIsNull (list_t *list);

extern void * GetFirst (list_t *list);

extern void DeleteFirst (list_t *list);

extern listnode_t *GetNode (list_t *list);

extern void *TravelList_1 (list_t *list);

extern void *TravelList (list_t *list);

/* old h_list.h */
extern int  DealList (list_t *list, int (*op_data) (void *data));
extern void FreeList (list_t *);

/* IndexList 部分 */
extern int InsertIndexList (list_t **list, const void *data);

extern int InsertIndexListOrderByKey (list_t **list, const void *data,
	int (*cmp) (const void *data1, const void *data2));
	
extern void DeleteIndexList (list_t *list, const void *data, 
		int (*cmp) (const void *data1, const void *data2));
		
extern void FreeIndexList (list_t **);

extern void FreeIndexListAndData(list_t **,void (* free_op) (void *data));

extern void DeleteIndexFirst (list_t *list);

#define	SearchIndexList	SearchList
#define PrintIndexList	PrintList
#define OperateIndexList OperateList
#define IndexListIsNull	ListIsNull
#define GetIndexFirst GetFirst
#define GetIndexNode GetNode
#define TravelIndexList_1	TravelList_1
#define TravelIndexList	TravelList


#endif
