/*VER: 1*/ 
/*#include "syst.h"*/
#include <stdio.h>
#define FOUND 1
#define NOTFOUND 0
int aigCharAntiExt[256];
int aigCharAnti[256];
#include "trie.h"
/*#define DEBUG*/
int igTrieNodeCnt=0;

void InitGlobalAntiTableExt()
{
    int c;
    for(c=0;c<=255;c++)
    if(isdigit(c)){
    	aigCharAntiExt[c]=c-'0';
    	aigCharAnti[c]=c-'0';
    }
    else {
    	aigCharAntiExt[c]=c%10;
    	aigCharAnti[c]=c%10;
    }
}
TRIE *MallocNodeC()
{
     int i;
     TRIE *pTemp;

     igTrieNodeCnt++;

     if((pTemp=(TRIE*)malloc(sizeof(TRIE)) )==NULL){
         printf("malloc node error.\n");
         exit(1);
     }
     for(i=0;i<10;i++){
         pTemp->pNext[i]=NULL;
         pTemp->pData[i]=NULL;
     }
     return pTemp;
}
TRIE *MallocNode()
{
     int i;
     TRIE *pTemp;

     igTrieNodeCnt++;

     if((pTemp=(TRIE*)malloc(sizeof(TRIE)) )==NULL){
         printf("malloc node error.\n");
         exit(1);
     }
     for(i=0;i<10;i++){
         pTemp->pNext[i]=NULL;
         pTemp->pData[i]=NULL;
     }
     return pTemp;
}
void InsertTrie(TRIE *ptHead,char key[16],void *pValue,
     void (*pAssign)(void *,void *))
{
    int i,offset,iStringLen_1;
    TRIE *ptCur,*ptPre;
    void *ptPreLkHead=NULL;

    if (!key[0]) return;

    ptCur=ptPre=ptHead;
    iStringLen_1=strlen(key)-1;   

/* printf("the key is %s.\n",key);*/
    for(i=0;i<iStringLen_1;i++){
        offset=aigCharAntiExt[(unsigned char )key[i]];
/* printf("the offset is %d.\n",offset);*/
        if(ptPre->pData[offset]!=NULL) ptPreLkHead=ptPre->pData[offset];
        if((ptCur=ptPre->pNext[offset])==NULL){
            ptCur=MallocNode();        
            ptPre->pNext[offset]=ptCur;
        }
        ptPre=ptCur;          
    }
    offset=aigCharAntiExt[(unsigned char )key[i]];
    if(ptCur->pData[offset]==NULL){
         ptCur->pData[offset]=pValue;
         (*pAssign)(pValue,ptPreLkHead);
         return;
    }
    (*pAssign)(pValue,ptCur->pData[offset]);
    ptCur->pData[offset]=pValue;   
}
int InsertTrieExt(TRIE *ptHead,char key[16],void *pValue,
     int (*pAssignExt)(void **,void *))
{
    int i,offset,iStringLen_1;
    TRIE *ptCur,*ptPre;
    void *ptPreLkHead=NULL;
    
    if (!key[0]) return false;
    
    ptCur=ptPre=ptHead;
    iStringLen_1=strlen(key)-1;   

/* printf("the key is %s.\n",key);*/
    for(i=0;i<iStringLen_1;i++){
        offset=aigCharAntiExt[(unsigned char )key[i]];
/* printf("the offset is %d.\n",offset);*/
        if(ptPre->pData[offset]!=NULL) ptPreLkHead=ptPre->pData[offset];
        if((ptCur=ptPre->pNext[offset])==NULL){
            ptCur=MallocNode();        
            ptPre->pNext[offset]=ptCur;
        }
        ptPre=ptCur;          
    }
    offset=aigCharAntiExt[(unsigned char )key[i]];
/*   if(ptCur->pData[offset]==NULL){
         ptCur->pData[offset]=pValue;
         return FALSE;
    }
*/
    /*由外挂函数处理空异常 2000-02-18 17*/
    return (*pAssignExt)(&(ptCur->pData[offset]),pValue);
}
void** GetTrieLk(TRIE *ptHead,char key[])
{
    unsigned char *pts;
    int iStringLen,offset;
    TRIE *ptCur,*ptPre;

    pts=(unsigned char *)&key[0];
    if((iStringLen= strlen(key))==0) return NULL;
    ptCur=ptPre=ptHead;
         if(ptPre==NULL){
         	printf("not exist ServTrieTree.\n");
                exit(1);}
#ifdef DEBUG
   printf("gettrieLk:begin loop.\n");
#endif
    while(iStringLen-->0){
        if(ptCur==NULL){
             printf(" not exist like nbr%% length>= here.\n");
             return NULL; /* not exist like nbr && len>=*/
        }
        offset=aigCharAntiExt[*pts++];
#ifdef DEBUG
   printf("gettrieLk:in loop,offset %d.\n",offset);
#endif
        ptPre=ptCur;
        ptCur=ptCur->pNext[offset];
    }
    pts--;
    offset=aigCharAntiExt[*pts];
#ifdef DEBUG
    printf("gettrieLk: ok here.\n");
#endif
    return &(ptPre->pData[offset]) ;
}
void DestroyTrie(TRIE *ptHead)
{
    int i;
    for(i=0;i<10;i++){
        if(ptHead->pNext[i]!=NULL)DestroyTrie(ptHead->pNext[i]);
    }
    free(ptHead);
    igTrieNodeCnt--;
}
void DestroyTrieControlHead(struct TrieControlHead *ptHead)
{
    if(ptHead!=NULL){
        free(ptHead->pData);
        if(ptHead->pNode!=NULL)DestroyTrie(ptHead->pNode);
    }
}
int SearchTrie(TRIE *ptHead,char key[17],void **pptCur)
{
    unsigned char *pts;
    int  iStringLen,offset;
    TRIE *ptCur,*ptPre;
    pts=(unsigned char *)(&key[0]);
    if(ptHead==NULL) return NOTFOUND;

    if((iStringLen=strlen(key))==0) return NOTFOUND;

    *pptCur=NULL;
    ptCur=ptPre=ptHead;

    while(iStringLen-->0){
        if(ptCur==NULL)  break;
        offset=aigCharAntiExt[*pts++];
/*     printf("the offset is %d.\n",offset);      */
        if(ptCur->pData[offset]!=NULL)
            *pptCur=ptCur->pData[offset];
        ptPre=ptCur;
        ptCur=ptCur->pNext[offset];
    }
/*    printf("the %x,%d.\n",*pptCur,NULL);*/
    return (*pptCur==NULL)?NOTFOUND:FOUND;         
}

int EmptyTrieExtNode(TRIEEXT * pTrie)
{
	int i;
	
	for (i=0;i<10;i++){
		pTrie->ioffsetNext[i] = -1;
		pTrie->ioffsetData[i] = -1;
	}
	return 0;
}
