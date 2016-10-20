/*VER: 1*/ 
/*
 *	链表操作封装函数
 *
 *	creator		何海峰
 *	date		02/23/2001
 */

#include <stdio.h>
#include <stdlib.h>
#if __DOS__
#include <malloc.h>
#include <mem.h>
#endif

#include "h_list.h"


/*------------------------
 *	usage:	申请新节点
 *	arguments:	data -- 新节点要包容的数据
 *------------------------*/
static listnode_t * NewNode ()
{
	listnode_t	*newnode = (listnode_t *) malloc(sizeof (listnode_t));
	
	if (newnode)
	{
		newnode->next = NULL;
		newnode->u.data = NULL;
	}
	return newnode;
}

static void freeNode (listnode_t *node)
{
	if(node)
	{
		if(node->u.data)
			free (node->u.data);
		free(node);
	}
}

/*-------------------------------------
 *	usage:	释放链表
 *	arguments:	list -- 链表表头指针
 *-------------------------------------*/
void FreeList (list_t *list)
{
	listnode_t	*curnode;
	listnode_t	*prenode;
	
	if(list)
	{
		curnode = list->head; 
		while (curnode != NULL)
		{
			prenode = curnode;
			curnode = curnode->next;
			freeNode (prenode);
		}
		free (list);
	}
}


/*-------------------------------------
 *	usage:	相链表插入新数据函数
 *	pre:  第一次调用时，要满足*list=NULL
 *	arguments:	list -- 链表表头指针 !!
 *			data -- 新插入数据指针
 *			n    -- 插入数据大小
 *			cmp  -- 数据比较函数指针
 *	return:		成功 -- 0
 *			失败 -- -1
 *	comment:新节点插入链表头
 *-------------------------------------*/
int InsertList (list_t **list, const void *data, size_t n)
{
	listnode_t	*tempnode;
	void		*tempdata;

	if (*list == NULL)
	{
		*list = (list_t *) malloc (sizeof (list_t));
		if (*list == NULL) {
			printf("msg:InsertList: malloc list_t");
			return -1;
		}
		(*list)->head = NULL;
	}

	if ((tempdata = (void *) malloc (n)) == NULL) {/* allocate data space*/
		printf("msg:InsertList: malloc data space");
		return -1;
	}
	memcpy (tempdata, data, n);

	if ((tempnode = NewNode ()) == NULL) {
		printf("msg:InsertList: NewNode()");
		return -1;
	}
	tempnode->u.data = tempdata;

	tempnode->next = (*list)->head;
	(*list)->head = tempnode;
		
	return 0;
}

/*-------------------------------------
 *	usage:	搜索节点
 *	arguments:	list -- 链表表头指针
 *			data -- 用于比较的数据
 *			cmp  -- 用于比较的函数，相同返回0，
 * 				data1大于data2返回1，data1小于data2返回-1
 *	return:		找到 -- 返回指向找到数据的指针
 *			没找到- NULL
 *-------------------------------------*/
void * SearchList (list_t *list, const void *data,
	int (*cmp) (const void *data1, const void *data2))
{
	listnode_t	*curnode;

	if(list == NULL)
		return NULL;

	for (curnode = list->head; curnode != NULL; curnode = curnode->next)
	{
		if (!cmp (curnode->u.data, data))/* use the function provided by user to do comparation*/
		{
			return curnode->u.data;	/*find data we need,return the pointer of the data */
		}
	}

	return NULL;
}

/*-------------------------------------
 *	usage:	删除链表中指定数据所在的节点
 *	arguments:	list -- 链表表头指针
 *			data -- 指定要删除的数据
 *			cmp -- 比较函数指针
 *	comment:输出定为stderr
 *-------------------------------------*/
void DeleteList (list_t *list, const void *data, 
	int (*cmp) (const void *data1, const void *data2))
{
	listnode_t	**curptr;
	listnode_t	*tempnode;
	
	if (list)
	{	
		curptr = &(list->head);
		
		while (*curptr)
		{
			if (!cmp ((*curptr)->u.data, data))
			{
				tempnode = *curptr;
				*curptr = (*curptr)->next;
				freeNode (tempnode);
			}
			else
				curptr = &((*curptr)->next);
		}
	}
}

/*-------------------------------------
 *	usage:	打印链表内容
 *	arguments:	list -- 链表表头指针
 *			print - 打印函数指针
 *	comment:输出定为stderr
 *-------------------------------------*/
void PrintList (list_t *list, void (*print) (const void *data))
{
	listnode_t	*curnode;
	
	if (list)
	{
		curnode = list->head;
		while (curnode)
		{
			print (curnode->u.data);
			curnode = curnode->next;
		}
		fprintf (stderr, "===========\n");
	}
}

/*------------------------------------------
 *	usage:	对链表中的各节点数据进行指定操作
 *	parameters:	list   -- 链表头指针
 *			op_data-- 指定的操作函数
 *------------------------------------------*/
void OperateList (list_t *list, void (*op_data) (void *data))
{
	listnode_t	*curnode;
	
	if (list)
	{
		curnode = list->head;
		while (curnode)
		{
			op_data (curnode->u.data);
			curnode = curnode->next;
		}
	}
}

void *GetFirst (list_t *list)
{
	listnode_t	*curnode;
	
	if (list)
	{
		curnode = list->head;
		if (curnode)
			return curnode->u.data;
	}
	
	return NULL;
}

void DeleteFirst (list_t *list)
{
	listnode_t	*cur;
	
	if (list)
	{
		cur = list->head;
		if (cur)
		{
			list->head = cur->next;
			free (cur);
		}
	}
}

listnode_t *GetNode (list_t *list)
{
	if (list)
		return list->head;
	else
		return NULL;
}

/*----------------------------------------
 *	判断链表是否为空（有无数据）
 *----------------------------------------*/
int ListIsNull (list_t *list)
{
	if (list)
	{
		if (list->head)
			return 0;
	}
	
	return 1;
}

/*-------------------------------------------
 *	usage: 遍历链表各节点，每次调用，按顺序逐个返回链表中的数据
 *	parameter:	list   -- 链表头指针
 *				  首次调用时填要遍历的链表，下次调用时
 *				  用NULL
 *-------------------------------------------*/
void *TravelList (list_t *list)
{
	static listnode_t	*curnode = NULL;

	
	if (list)
	{
		curnode = list->head;
		
		if (curnode)
			return curnode->u.data;
	}
	else
	{
		if (curnode)
		{		
			curnode = curnode->next;
			if (curnode)
				return curnode->u.data;	
		}
	}
	
	return NULL;
	
}

/*------------------------------------------
 *	usage:	对链表中的各节点数据进行指定操作
 *
 *	notice:	节点操作可中断
 *
 *	parameters:	list   -- 链表头指针
 *			op_data-- 指定的操作函数
 *------------------------------------------*/
int DealList (list_t *list, int (*op_data) (void *data))
{
	listnode_t	*curnode;
	
	if (list)
	{
		curnode = list->head;
		while (curnode)
		{
			/*
			 *	 操作被中断，返回0
			 */
			if (op_data (curnode->u.data) == 0)
				return 0;
				
			curnode = curnode->next;
		}
	}
	
	return 1;
}
