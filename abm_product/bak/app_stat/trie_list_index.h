/*VER: 1*/ 
/********************************
 *
 *	键树索引操作头文件
 *
 ********************************/

#ifndef _TRIE_LIST_INDEX__H_
#define _TRIE_LIST_INDEX__H_

#include "list_index.h" 

#define TRIE_FANOUT	10

/* - - - 键树节点结构 - - - */
typedef struct trie_s
{			
	int		subchanged[TRIE_FANOUT]; /* 记录子节点发生变化的情况 */
	int		listchanged;	/* 记录所属链表的变化情况 */
	list_t		*list;
	struct trie_s	*subtrie[TRIE_FANOUT];
}trie_t;


extern void InitTrieList ();

/* TrieList */
extern int InsertTrieList (trie_t **trie, const char str[], const int level, 
			void *data, size_t n, int changed);
extern int InsertTrieListOrderByKey (trie_t **trie, const char str[], const int level, 
		void *data, size_t n, int changed,int (*cmp) (const void *data1, const void *data2));
		
extern void * SearchTrieList (trie_t *trie, const char str[], const int level, void *data, 
		int (*cmp) (const void *data1, const void *data2));

extern int TouchTrieList (trie_t **trie, const char str[], const int level, list_t ***list);

extern int TouchTrieListByPriKey (trie_t **trie, const char str[], const int level);

extern list_t *GetListofTrieList (trie_t *trie, const char str[], const int level);
	
extern void PrintTrieList (trie_t *trie, const int level, const int key, 
		void (*print) (const void *data));

extern void OperateAllTrieList (trie_t *trie, void (* op_data) (void *data));

extern void RefreshTrieList (trie_t *trie, void (* op_data) (void *data));

extern void FreeTrieList (trie_t **trie);

/* IndexTrieList */

extern int InsertIndexTrieList (trie_t **trie, const char str[], const int level, 
			void *data,int changed);
extern int InsertIndexTrieListOrderByKey (trie_t **trie, const char str[], const int level, 
		void *data,int changed,int (*cmp) (const void *data1, const void *data2));
		
#define SearchIndexTrieList	SearchTrieList
#define TouchIndexTrieListByPriKey	TouchTrieListByPriKey
#define TouchIndexTrieList	TouchTrieList
#define GetIndexListofIndexTrieList	GetListofTrieList
#define PrintIndexTrieList	PrintTrieList
#define RefreshIndexTrieList	RefreshTrieList
#define OperateAllIndexTrieList 	OperateAllTrieList

extern void FreeIndexTrieList (trie_t **trie);
extern void FreeIndexTrieListAndData (trie_t **trie,void (* free_op) (void *data));

#endif

