/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMStringTreeIndex.h"
#include <string.h>
#include "MBC.h"
#include "Stack.h"

#define STRING_KEY_LEN	12
//##实际影响key的长度，考虑目前电信号码一般是11位，所以，这里设置为11

//##ModelId=424429AB039E
SHMStringTreeIndex::SHMStringTreeIndex(char *shmname) :
SHMAccess(shmname), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424429AB03BC
SHMStringTreeIndex::SHMStringTreeIndex(long shmkey) :
SHMAccess(shmkey), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424429AC001A
SHMStringTreeIndex::~SHMStringTreeIndex()
{
}

//##ModelId=42442A8B0093
void SHMStringTreeIndex::add(char *key, unsigned int value)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList *pl;
	
	for(i=0; i<STRING_KEY_LEN && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
		} else {
			if (*m_piTreeUsed < *m_piTreeTotal) {
				m_poTree[index].m_iNext[j] = (*m_piTreeUsed)++;
				index = m_poTree[index].m_iNext[j];
			} else {
				THROW(MBC_SHMStringTreeIndex+1);
			}
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
	if (*m_piListUsed < *m_piListTotal) {
		j = (*m_piListUsed)++;
		pl = getList(j);
		pl->m_iNext = m_poTree[index].m_iValue;
		m_poTree[index].m_iValue = j;
		pl->m_iValue = value;
		memcpy (pl->m_sKey, key, strlen(key));
		pl->m_sKey[strlen(key)] = 0;
	} else {
		THROW(MBC_SHMStringTreeIndex+2);
	}
	return;
}

bool SHMStringTreeIndex::getMax(char *key, unsigned int *pvalue)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList *pl;

    Stack<int> stack;
	
	for(i=0; i<STRING_KEY_LEN && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
            stack.push (index);
		} else {
            break;
		}
	}

    //--xueqt 20060710 STRING_KEY_LEN=12 
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

bool SHMStringTreeIndex::getMax(char *key, unsigned int *pvalue,unsigned int *pMatchLen)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList *pl;
    Stack<int> stack;
	for(i=0; i<STRING_KEY_LEN && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
            stack.push (index);
		} else {
            break;
		}
	}
    //--xueqt 20060710 STRING_KEY_LEN=12 
    while (stack.pop (index)) {
        j = m_poTree[index].m_iValue;
        if (j) {
            pl = getList (j);
            while (pl ) {
                if (!strncmp(pl->m_sKey, key, i)) {
					*pMatchLen = i;
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
bool SHMStringTreeIndex::get(char *key, unsigned int *pvalue)
{
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList *pl;
	
	for(i=0; i<STRING_KEY_LEN && *p; p++,i++) {
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
void SHMStringTreeIndex::create(unsigned int itemnumber, unsigned int keylen, unsigned int multiple)

{
	unsigned long size1, size2, size3;
	unsigned int iListLen;
	unsigned int iKeyLen = keylen;

	//##计算SHMStringList的长度，内存按照8个字节对齐。
	if (keylen <= 8) {
		iListLen = sizeof(SHMStringList);
	} else { 
		iListLen = sizeof(SHMStringList) + (keylen&(~7u));
	}

	//##SHMStringList所占的长度
	size1 = iListLen * itemnumber;
	
	//##SHMStringTree所占的长度
	size2 = sizeof(SHMStringTree)*itemnumber*multiple;

	size3 = size1 + size2 + sizeof(unsigned int)*8;
	
	SHMAccess::create (size3);
	open ();
	
	*m_piListLen = iListLen;
	*m_piKeyLen = iKeyLen;
	*m_piListTotal = itemnumber;
	*m_piTreeTotal = itemnumber*multiple;
	*m_piListUsed = 1;
	*m_piTreeUsed = 1; 
	
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;
}

//##ModelId=4244EBC6017B
void SHMStringTreeIndex::open()
{
	m_piListTotal = ((unsigned int *)((char *)(*m_poSHM))) + 1;
	m_piListUsed = m_piListTotal + 1;
	m_piTreeTotal = m_piListUsed + 1;
	m_piTreeUsed = m_piTreeTotal + 1;
	m_piKeyLen = m_piTreeUsed + 1;
	m_piListLen = m_piKeyLen + 1;

	m_poTree = (SHMStringTree *) (m_piListLen + 2);	
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;
}

//##ModelId=424512FC000C
SHMStringList *SHMStringTreeIndex::getList(unsigned int offset)
{
	if (!offset)
		return 0;

	return (SHMStringList *)(m_poList + (*m_piListLen)*offset);
}

void SHMStringTreeIndex::empty()
{
		memset (m_poTree, 0, sizeof (SHMStringTree)*(*m_piTreeTotal));
		*m_piTreeUsed = 1;
		memset (m_poList, 0, sizeof (SHMStringList)*(*m_piListTotal));
		*m_piListUsed = 1;
}