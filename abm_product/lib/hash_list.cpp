/*VER: 1*/ 
/*
 *	HASH表查找函数(用于将表数据存放于HASH表，通过KEY值查找指定数据)
 *
 *	creator		何海峰
 *	date		02/23/2001
 */
#include "./hash_list.h"

/*
list_t	**hash_list;
*/

/*此HASH表采用HASH再散裂技术，链表操作调用公用list.c中的操作 comentate by wangzy*/

static int hash (const char *key)
{
	int	hval;
	const char	*ptr;
	char	c;
	int	i;
	
	hval = 0;
	for (ptr = key, i = 1; c=*ptr++; i++)
		hval += c*i; /* ascii char times its 1-based index */
		
	return (hval%HASH_NUM);
}

void initHashList (THashList *h_list)
{
	int	i;
	list_t	**pplist;
	
	pplist = (list_t **) calloc (sizeof (list_t *), HASH_NUM);
	
	for (i = 0; i < HASH_NUM; i++)
		pplist[i] = NULL;
	
	*h_list = pplist;
}

/*
 *	以给定KEY为搜索键的查找链表
 *	KEY相同的数据接到相同的链表，供以后查找用
 *	pre:  第一次调用时，要满足*keylist=NULL
 */
int insertHashList (THashList h_list, const char *key, void *data, int data_size)
{
	int	hval;
	
	hval = hash (key);
	
	if (InsertList (&(h_list[hval]), data, data_size) == -1)
			return -1;	
	return 0;
}

/*
 *	搜索键值链表
 *
 *	key	为键值
 *	data	为比较数据
 *	cmp	为比较函数指针，返回值定义为返回0表示相等，其他为不等
 *		data1为链表中的数据，data2为用于比较的数据
 *
 *	返回值	指向匹配的数据的指针
 */
void *searchHashList (THashList h_list, char *key, void *cmp_data,
	int (*cmp) (const void *list_data, const void *cmp_data))
{
	list_t	*pcur;
	int	hval;
	
	hval = hash (key);
	pcur = h_list[hval];
	
	return SearchList (pcur, cmp_data, cmp);
}

void freeHashList (THashList h_list)
{	
	int	i;
	
	for (i = 0; i < HASH_NUM; i++)
	{
		FreeList (h_list[i]);
	}
	
	free (h_list);
}

void printHashList (THashList h_list, void (*print) (const void *data))
{
	int	i;
	
	for (i = 0; i < HASH_NUM; i++)
		PrintList (h_list[i], print);
}
