/*VER: 1*/ 
#ifndef __TRIE_H_
#define __TRIE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



typedef struct _Trie_ {
    struct _Trie_  *pNext[10];
    void  *pData[10];
} TRIE;

typedef struct _Trie_O_ {

    struct _Trie_O_  *pNext[10];

    long  ioffsetData[10];

} TRIEO;

typedef struct _Trie_Ext_{

    long  ioffsetNext[10];

    long  ioffsetData[10];

} TRIEEXT;


struct TrieControlHead
{
     TRIE *pNode;
     void *pData;
};

void InitGlobalAntiTableExt();
TRIE *MallocNode();
TRIE *MallocNodeC();
void InsertTrie(TRIE *ptHead,char key[16],void *pValue,
     void (*pAssign)(void *,void *));
int  InsertTrieExt(TRIE *ptHead,char key[16],void *pValue,
     int (*pAssignExt)(void **,void *));
void DestroyTrie(TRIE *ptHead);
void DestroyTrieControlHead(struct TrieControlHead *ptHead);
int SearchTrie(TRIE *ptHead,char key[17],void **pptCur);
void** GetTrieLk(TRIE *ptHead,char key[]);
int EmptyTrieExtNode(TRIEEXT * pTrie);

extern int aigCharAnti[256];
extern int igTrieNodeCnt;
#endif

