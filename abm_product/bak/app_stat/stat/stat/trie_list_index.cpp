/*VER: 1*/ 
/********************************************************************************/
/*	程序名称：键树索引操作                                                      */
/*	程序功能：实现键树的插入，查找，更新                                    */
/*	程序说明：1。使用字符串作为键值。字符串中的字符，通过键值转换           */
/*		转换成数值。取得的数值对应树的下层分支。                        */ 
/*		  2。本程序为实时合帐中服务，目的是用来维护帐目数据。           */
/*		由于帐目数据要定期与数据库同步，使用以下方法实现之：            */
/*		    树的节点定义一个subchanged数组和一个listchanged标识。       */
/*		    subchanged用来指示当前节点的那些子树的数据是否发生了        */
/*		    变化，                                                      */
/*		    listchaned用来指示当前节点指向的链表的数据是否发生了        */
/*		    变化。                                                      */
/*		    对于新插入或更新数据操作，要将经过的节点的subchanged        */
/*		    和listchanged做相应修改                                     */
/********************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "trie_list_index.h"

static char keyarray[256];

/*-------------------------------
 *	usage:	初始化键值数组
 *	comment:将char映射成0-9的数值
 *-------------------------------*/
void InitTrieList ()
{
	int	c;

	for (c = 0; c < 256; c++)
	{
		if (isdigit (c))
			keyarray[c] = c - '0';
		else
			keyarray[c] = c%10;
	}
}

static trie_t * NewNode ()
{
	int	i;
	trie_t	*node = (trie_t *)malloc (sizeof (trie_t));

	if (node)
	{
		node->list = NULL;
		for (i = 0; i < TRIE_FANOUT; i++)
		{
			node->subchanged[i] = 0;
			node->listchanged = 0;
			node->subtrie[i] = NULL;
		}
	}

	return node;
}

/*--------------------------------
 *	usage:	向键树中插入一个新的数据
 *	arguments:	*trie -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			data  -- 要插入的数据
 *			n     -- 要插入的数据的大小
 *			changed - 记录当前节点的子节点的内容是否发生了变化
 *					1 -- 有变化	0 -- 无变化
 *	return:	-1 -- 出错	0 -- 正常
 *	comment：键树的叶节点是链表，出入数据时，先根据键值找到叶节点，再向
 *		 叶节点所指的链表中插入数据。
 *---------------------------------*/
int InsertTrieList (trie_t **trie, const char str[], const int level, 
		void *data, size_t n, int changed)
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = changed;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = changed;
	
	return (InsertList (&(curnode->list), data, n));
}
/*--------------------------------
 *	usage:	向键树中插入一个新的数据
 *	arguments:	*trie -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			data  -- 要插入的数据
 *			n     -- 要插入的数据的大小
 *			changed - 记录当前节点的子节点的内容是否发生了变化
 *					1 -- 有变化	0 -- 无变化
 *	return:	-1 -- 出错	0 -- 正常
 *	comment：键树的叶节点是链表，出入数据时，先根据键值找到叶节点，再向
 *		 叶节点所指的链表中插入数据。
 *---------------------------------*/
int InsertIndexTrieList (trie_t **trie, const char str[], const int level, 
		void *data, int changed)
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = changed;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = changed;
	
	return (InsertIndexList (&(curnode->list), data));
}
/*--------------------------------
 *	usage:	向键树中插入一个新的数据
 *	arguments:	*trie -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			data  -- 要插入的数据
 *			n     -- 要插入的数据的大小
 *			changed - 记录当前节点的子节点的内容是否发生了变化
 *					1 -- 有变化	0 -- 无变化
 *	return:	-1 -- 出错	0 -- 正常
 *	comment：键树的叶节点是链表，出入数据时，先根据键值找到叶节点，再向
 *		 叶节点所指的链表中插入数据。
 *---------------------------------*/
int InsertTrieListOrderByKey (trie_t **trie, const char str[], const int level, 
		void *data, size_t n, int changed,int (*cmp) (const void *data1, const void *data2))
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = changed;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = changed;
	
	return (InsertListOrderByKey (&(curnode->list), data, n,cmp));
}

/*--------------------------------
 *	usage:	向键树中插入一个新的数据
 *	arguments:	*trie -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			data  -- 要插入的数据
 *			n     -- 要插入的数据的大小
 *			changed - 记录当前节点的子节点的内容是否发生了变化
 *					1 -- 有变化	0 -- 无变化
 *	return:	-1 -- 出错	0 -- 正常
 *	comment：键树的叶节点是链表，出入数据时，先根据键值找到叶节点，再向
 *		 叶节点所指的链表中插入数据。
 *---------------------------------*/
int InsertIndexTrieListOrderByKey (trie_t **trie, const char str[], const int level, 
		void *data, int changed,int (*cmp) (const void *data1, const void *data2))
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = changed;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = changed;
	
	return (InsertIndexListOrderByKey (&(curnode->list), data,cmp));
}
/*--------------------------------
 *	usage:	在键树中查找数据
 *	arguments:	trie  -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			data  -- 要查找的数据
 *			cmp   -- 比较函数指针
 *	return:	找到 -- 返回指向该数据的指针	没找到 -- NULL
 *	comment:查找规则由cmp函数指定
 *---------------------------------*/
void * SearchTrieList (trie_t *trie, const char str[], const int level, void *data, 
		int (*cmp) (const void *data1, const void *data2))
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (trie == NULL)
		return NULL;

	curnode = trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
			return NULL;
		
		curnode = curnode->subtrie[key];
	}

	return (SearchList (curnode->list, data, cmp));
}
/*--------------------------------
 *	usage:	在键树中查找键值指向的链头。并将经过的节点的changed字段置1
 *		表示该节点的子节点要发生变化。如节点不存在，则生成该节点
 *	arguments:	trie  -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			
 *	return:	找到 -- 返回指向该链表头的指针	没找到 -- NULL
 *	comment:
 *---------------------------------*/
int TouchTrieListByPriKey (trie_t **trie, const char str[], const int level)
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = 1;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = 1;
	
	return 0;
}

/*--------------------------------
 *	usage:	在键树中查找键值指向的链头。并将经过的节点的changed字段置1
 *		表示该节点的子节点要发生变化。如节点不存在，则生成该节点
 *	arguments:	trie  -- 键树头指针
 *			str   -- 键值字符串
 *			level -- 键值长度
 *			list  -- 保存指向链头list指针的指针，由于要保存指针的指针，
 *				 使用3层指针
 *	return:	找到 -- 返回指向该链表头的指针	没找到 -- NULL
 *	comment:
 *---------------------------------*/
int TouchTrieList (trie_t **trie, const char str[], const int level, list_t ***list)
{
	int	i;
	int	key;
	trie_t	*curnode;

	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL)
			return -1;
	}

	curnode = *trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = 1;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = 1;
	
	*list = &(curnode->list);
	
	return 0;
}

/*-------------------------------------------
 *
 *-------------------------------------------*/
list_t *GetListofTrieList (trie_t *trie, const char str[], const int level)
{
	int	i;
	int	key;
	trie_t	*curnode;
	
	if (trie == NULL)
		return NULL;
	
	curnode = trie;
	for (i = 0; i < level ; i++)
	{
		key = keyarray[str[i]];
		if (curnode->subtrie[key] == NULL)
			return NULL;
			
		curnode = curnode->subtrie[key];
	}
	
	return curnode->list;
}

/*-------------------------------
 *	usage:	释放键树
 *	arguments:	trie -- the head of trie
 *-------------------------------*/
void FreeTrieList (trie_t **trie)
{
	int	i;

	if (*trie)
	{
		for (i = 0; i < TRIE_FANOUT; i++)
			FreeTrieList (&((*trie)->subtrie[i]));
		FreeList (&((*trie)->list));
		free (*trie);
		*trie=NULL;
	}
}

/*-------------------------------
 *	usage:	释放键树
 *	arguments:	trie -- the head of trie
 *-------------------------------*/
void FreeIndexTrieList (trie_t **trie)
{
	int	i;

	if (*trie)
	{
		for (i = 0; i < TRIE_FANOUT; i++)
			FreeIndexTrieList (&((*trie)->subtrie[i]));
		FreeIndexList (&((*trie)->list));
		free (*trie);
		*trie=NULL;
	}
}

/*-------------------------------
 *	usage:	释放键树和数据
 *	arguments:	trie -- the head of trie
 *-------------------------------*/
void FreeIndexTrieListAndData (trie_t **trie,void (* free_op) (void *data))
{
	int	i;

	if (*trie)
	{
		for (i = 0; i < TRIE_FANOUT; i++)
			FreeIndexTrieListAndData (&((*trie)->subtrie[i]),free_op);
		FreeIndexListAndData (&((*trie)->list),free_op);
		free (*trie);
		*trie=NULL;
	}
}

/*----------------------------------
 *	usage:	print the data of the trie
 *----------------------------------*/
void PrintTrieList (trie_t *trie, const int level, const int key, void (*print) (const void *data))
{
	int	i;
	
	if (trie)
	{
		fprintf (stderr, "enter subtrie -- level:%d,key:%d\n", level, key);
		for (i = 0; i < TRIE_FANOUT; i++)
		{
			PrintTrieList (trie->subtrie[i], level + 1, i, print);
		}
		PrintList (trie->list, print);
	}
}

void OperateTrieList (trie_t *trie, void (* op_data) (void *data))
{
	int i;
	
	if (trie)
	{
		for (i = 0; i < TRIE_FANOUT; i++)
		{
			OperateTrieList (trie->subtrie[i], op_data);
		}
		OperateList (trie->list, op_data);
	}
}


/*------------------------------------------
 *	usage:	刷新TRIE，对changed为1的节点的子节点的链表做op_list指定的操作
 *	parameters:	trie   -- trie head pointer
 *			op_list-- 对list的操作
 *------------------------------------------*/
void RefreshTrieList (trie_t *trie, void (* op_data) (void *data))
{
	int	i;
	
	if (trie)
	{
		for (i = 0; i < TRIE_FANOUT; i++)
		{
			if (trie->subchanged[i]) /* 子节点发生过变化 */
			{
				RefreshTrieList (trie->subtrie[i], op_data);
				trie->subchanged[i] = 0;
			}
		}
		if (trie->listchanged)	
		{
			OperateList (trie->list, op_data);
			trie->listchanged = 0;
		}
	}
		
}
