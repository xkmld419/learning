/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMStringTreeIndex_A.h"
#include <string.h>
#include "MBC.h"
#include "Stack.h"
#include <stdio.h>
#include <iostream.h>



//##ModelId=424429AB039E
SHMStringTreeIndex_A::SHMStringTreeIndex_A(char *shmname) :
SHMAccess(shmname), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
	if (m_poSHM){
		open ();
        if( 4 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }
}


//##ModelId=424429AB03BC
SHMStringTreeIndex_A::SHMStringTreeIndex_A(long shmkey) :
SHMAccess(shmkey), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
	if (m_poSHM){
		open ();
        if( 4 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }
}


//##ModelId=424429AC001A
SHMStringTreeIndex_A::~SHMStringTreeIndex_A()
{
}




//##ModelId=42442A8B0093
void SHMStringTreeIndex_A::add(char *key, unsigned int value)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_A *pl;
	
	for(i=0; i<SHM_STRING_KEY_LEN_A && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
		} else {
		    m_poTree[index].m_iNext[j] = mallocTree();
		    index = m_poTree[index].m_iNext[j];

		}
	}

	if (!(m_poTree[index].m_iValue)) {
		goto __ADD;
	}

	j = m_poTree[index].m_iValue;
	pl = getList (j);
	while (pl ) {
		if (!strncmp(pl->m_sKey, key, *m_piKeyLen)) {
			pl->m_iValue = value;
			return ;
		}
		pl = getList (pl->m_iNext);
	}		

__ADD:
    
    j = mallocList();
	pl = getList(j);
	pl->m_iNext = m_poTree[index].m_iValue;
	m_poTree[index].m_iValue = j;
	pl->m_iValue = value;
	memcpy (pl->m_sKey, key, strlen(key));
    pl->m_sKey[strlen(key)] = 0;

	return;
}

bool SHMStringTreeIndex_A::getMax(char *key, unsigned int *pvalue)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_A *pl;

    Stack<int> stack;
	
	for(i=0; i<SHM_STRING_KEY_LEN_A && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
            stack.push (index);
		} else {
            break;
		}
	}

    //--xueqt 20060710 SHM_STRING_KEY_LEN_A=12 
    //所以不考虑区号超过12位的精确匹配问题
    //这个是个隐患，以后出现超过12位的，调整参数或者优化算法

    while (stack.pop (index)) {
        j = m_poTree[index].m_iValue;
        if (j) {
            pl = getList (j);
            while (pl ) {
                if (!strncmp(pl->m_sKey, key, i)) {
                    *pvalue = pl->m_iValue ;
                    return true;
                }
                pl = getList (pl->m_iNext);
            }		
        }

        i--;
    }

    return false;
}


//##ModelId=42442A9101C8
bool SHMStringTreeIndex_A::get(char *key, unsigned int *pvalue)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_A *pl;
	
	for(i=0; i<SHM_STRING_KEY_LEN_A && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
		} else {
			return false;
		}
	}

	if (!m_poTree[index].m_iValue) return false;


	j = m_poTree[index].m_iValue;
	pl = getList (j);
	while (pl ) {
		if (!strncmp(pl->m_sKey, key, *m_piKeyLen)) {
			*pvalue = pl->m_iValue ;
			return true;
		}
		pl = getList (pl->m_iNext);
	}		
	return false;
}

//##ModelId=42442B0802C3
void SHMStringTreeIndex_A::create(unsigned int itemnumber, unsigned int keylen, unsigned int multiple)

{
	unsigned long size1, size2, size3;
	unsigned int iListLen;
	unsigned int iKeyLen = keylen;

	//##计算SHMStringList的长度，内存按照8个字节对齐。
	if (keylen <= 8) {
		iListLen = sizeof(SHMStringList_A);
	} else { 
		iListLen = sizeof(SHMStringList_A) + (keylen&(~7u));
	}

	//##SHMStringList所占的长度
	size1 = iListLen * itemnumber;
	
	//##SHMStringTree所占的长度
	size2 = sizeof(SHMStringTree_A)*itemnumber*multiple;

	size3 = size1 + size2 + sizeof(unsigned int)*16;
	
	SHMAccess::create (size3);
	open ();
	
	*m_piListLen = iListLen;
	*m_piKeyLen = iKeyLen;
	*m_piListTotal = itemnumber;
	*m_piTreeTotal = itemnumber*multiple;
	*m_piListUsed = 1;
	*m_piTreeUsed = 1; 

    m_poTree = (SHMStringTree_A *) (m_piListLen + 2);	
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;

    m_piTreeHead = (unsigned int * )(m_poList+(*m_piListLen)*((*m_piListTotal)+1) ) ;
    m_piTreeTail = m_piTreeHead+1;
    m_piListHead = m_piTreeHead+2;
    m_piListTail = m_piTreeHead+3;
   
    *m_piListHead  = 1;
    *m_piListTail  = *m_piListTotal;
    *m_piTreeHead  = 1;
    *m_piTreeTail  = (*m_piTreeTotal)-1;
    *m_piVersion = 4;

    SHMStringList_A * pList = 0;
    
    for(unsigned int ii=*m_piListHead;ii<*m_piListTail;ii++)
    {
        pList = getList(ii);
        pList->m_iNext = ii+1;
    }
    pList = getList(*m_piListTail);
    pList->m_iNext = 0;


    for(unsigned int ii=*m_piTreeHead;ii<*m_piTreeTail;ii++)
    {
        m_poTree[ii].m_iNext[0] = ii+1;
    }
    m_poTree[(*m_piTreeTail)].m_iNext[0] = 0;


}

//##ModelId=4244EBC6017B
void SHMStringTreeIndex_A::open()
{
	m_piListTotal = ((unsigned int *)((char *)(*m_poSHM))) + 1;

    m_piVersion   = m_piListTotal - 1 ;
	m_piListUsed  = m_piListTotal + 1;
	m_piTreeTotal = m_piListUsed + 1;
	m_piTreeUsed  = m_piTreeTotal + 1;
	m_piKeyLen  = m_piTreeUsed + 1;
	m_piListLen = m_piKeyLen + 1;

////m_poTree不动，老版本的可以继续读取数据,[有地方1批环境读取2批环境的资料]
	m_poTree = (SHMStringTree_A *) (m_piListLen + 2);
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;

    m_piTreeHead = (unsigned int * )(m_poList+(*m_piListLen)*((*m_piListTotal)+1) ) ;
    m_piTreeTail = m_piTreeHead+1;
    m_piListHead = m_piTreeHead+2;
    m_piListTail = m_piTreeHead+3;

}

//##ModelId=424512FC000C
SHMStringList_A *SHMStringTreeIndex_A::getList(unsigned int offset)
{
	if (!offset)
		return 0;

	return (SHMStringList_A *)(m_poList + (*m_piListLen)*offset);
}



////zhangap 增加索引循环使用的功能 
unsigned int SHMStringTreeIndex_A::mallocList( )
{
    unsigned int j=0;
    if (*m_piListUsed <= *m_piListTotal) {
        (*m_piListUsed)++;
        j = *m_piListHead;
        
        if( *m_piListUsed == 1+(*m_piListTotal) ){
            *m_piListHead = 0;
            *m_piListTail = 0;
        }else{
            SHMStringList_A * pList = getList(j);
            *m_piListHead = pList->m_iNext;
            pList->m_iNext = 0;
        }
        

    } else {
        //##空间不够
        THROW(MBC_SHMIntHashIndex+1);
    }
    
    return j;
}


unsigned int SHMStringTreeIndex_A::mallocTree()
{
    unsigned int j=0;    
    if (*m_piTreeUsed < *m_piTreeTotal) {
        (*m_piTreeUsed)++;
        j = *m_piTreeHead;
        
        if( *m_piTreeUsed == (*m_piTreeTotal) ){
            *m_piTreeHead = 0;
            *m_piTreeTail = 0;
        }else{
            *m_piTreeHead = m_poTree[j].m_iNext[0];
            m_poTree[j].m_iNext[0] = 0;
        }
        

    } else {
        //##空间不够
        THROW(MBC_SHMIntHashIndex+1);
    }
    
    return j;
}


//删除
///输入 key，根据key找到要删除的那个，然后删除，返回的是这个key所记录的value
bool SHMStringTreeIndex_A::revokeIdx(char * key, unsigned int &value)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_A *pl;
	

	SHMPreTree_A m_pPre[SHM_STRING_KEY_LEN_A];
	int  m_iTreePre;

		
	for(i=0; i<SHM_STRING_KEY_LEN_A && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
		    m_pPre[i].pTree = &m_poTree[index];
		    m_pPre[i].iOffset = j;
			index = m_poTree[index].m_iNext[j];
		} else {
			return false;
		}
	}

	if (!m_poTree[index].m_iValue) return false;

    m_iTreePre = i-1;
    
	unsigned int * puiPrePosition = &m_poTree[index].m_iValue;

	j = m_poTree[index].m_iValue;
	pl = getList (j);
	while (pl ) {
		if (!strncmp(pl->m_sKey, key, *m_piKeyLen)) {
		    j = *puiPrePosition;
			value = pl->m_iValue ;
			*puiPrePosition = pl->m_iNext;
			pl->m_sKey[0]='\0';
			memset(pl->m_sKey, 0, *m_piKeyLen );
			pl->m_iNext=0;
			pl->m_iValue=0;
			
            if( *m_piListUsed <= *m_piListTotal ){
                //索引空间未用完
                SHMStringList_A * pList = getList(*m_piListTail);
                pList->m_iNext = j;
            }else{
                *m_piListHead = j;
            }
            *m_piListTail = j;
            (*m_piListUsed)--;
			
            j = m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ];
			if(m_iTreePre && 0 == m_poTree[ j ].m_iValue ){

			    SHMStringTree_A treeTemp;
			    memset(&treeTemp, 0, sizeof(SHMStringTree_A) );
			    
			    while(0==memcmp(& m_poTree[j], &treeTemp, sizeof(SHMStringTree_A)) ){
			        ////从最底层的Tree，一直往上删

                    m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ] = 0;
                    if( *m_piTreeUsed < *m_piTreeTotal ){
                        //索引空间未用完
                        m_poTree[*m_piTreeTail].m_iNext[0] = j;
                    }else{
                        *m_piTreeHead = j;
                    }
                    *m_piTreeTail = j;
                    (*m_piTreeUsed)--;
                    
                    if( 0==m_iTreePre )
                        break;
                    
			        m_iTreePre--;
			        j = m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ];
			        
			    }
			}
			
			return true;
		}
		pl = getList (pl->m_iNext);

		puiPrePosition = &pl->m_iNext;
	}		
	return false;

}




void SHMStringTreeIndex_A::empty()
{
    memset (m_poTree, 0, (sizeof(SHMStringTree_A) ) *  (*m_piTreeTotal) );
    memset (m_poList, 0, (*m_piListLen) * ((*m_piListTotal)-1) );
	*m_piListUsed = 1;
	*m_piTreeUsed = 1;

    *m_piListHead  = 1;
    *m_piListTail  = *m_piListTotal;
    *m_piTreeHead  = 1;
    *m_piTreeTail  = (*m_piTreeTotal)-1;
//    *m_piVersion = 4;

    SHMStringList_A * pList = 0;
    
    for(unsigned int ii=*m_piListHead;ii<*m_piListTail;ii++)
    {
        pList = getList(ii);
        pList->m_iNext = ii+1;
    }
    pList = getList(*m_piListTail);
    pList->m_iNext = 0;


    for(unsigned int ii=*m_piTreeHead;ii<*m_piTreeTail;ii++)
    {
        m_poTree[ii].m_iNext[0] = ii+1;
    }
    m_poTree[(*m_piTreeTail)].m_iNext[0] = 0;
    
}


void SHMStringTreeIndex_A::showDetail()
{
    char sText[256];
    char keys[64];
    if(m_sSHMName) {
        strncpy(keys, m_sSHMName, sizeof(keys));
    }else{
        sprintf(keys, "%ld", m_lSHMKey);
    }

    if( !m_poSHM ){
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "S_IDX_A_LIST",sizeof(SHMStringList_A) , 0, 0, 0 );
    cout<< sText<<endl;
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "S_IDX_A_TREE", sizeof(SHMStringTree_A),0, 0,0 );
    cout<< sText<<flush;

    }else{
    char * pp = (char * )(*m_poSHM);
    unsigned int uiTotal = getlTotal();
    unsigned int uiCount = getlCount();
    int percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4u%%",
        keys, pp,(*m_piVersion), "S_IDX_A_LIST", (*m_piListLen), uiTotal, uiCount, percent);
    cout<< sText<<endl;

    uiTotal = getTotal();
    uiCount = getCount();
    percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4u%%",
        keys, pp,(*m_piVersion), "S_IDX_A_TREE", sizeof(SHMStringTree_A),  uiTotal, uiCount, percent);
    cout<< sText<<flush;    
    }

}

