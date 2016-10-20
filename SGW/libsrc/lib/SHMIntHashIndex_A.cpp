/*VER: 1*/ 
// Copyright (c) 2009,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMIntHashIndex_A.h"
#include "MBC.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

////zhangap 可循环使用数据区的索引类，使用SHMIntList里面固有的next，不增加内存开销
////增加版本记录，这个已经和 SHMIntHashIndex不兼容了
//// 

//##ModelId=424426D6039B
SHMIntHashIndex_A::SHMIntHashIndex_A(char *shmname) :
SHMAccess(shmname), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM){
		open ();
		if( 3 != *m_piVersion ){
			THROW(MBC_SHMData+1);
		}
	}

}


//##ModelId=424426D603B9
SHMIntHashIndex_A::SHMIntHashIndex_A(long shmkey) :
SHMAccess(shmkey), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM){
		open ();
		if( 3 != *m_piVersion ){
			printf("\n%s(%d):shmkey = %ld\n",__FILE__,__LINE__,shmkey);
			THROW(MBC_SHMData+1);
		}
	}
}


//##ModelId=424426D603D7
SHMIntHashIndex_A::~SHMIntHashIndex_A()
{
}


unsigned int SHMIntHashIndex_A::malloc()
{
	unsigned int j=0;    
	if (*m_piUsed <= *m_piTotal) {
		(*m_piUsed)++;
		j = *m_piHead;

		if( *m_piUsed == 1+(*m_piTotal) ){
			*m_piHead = 0;
			*m_piTail = 0;
		}else{
			*m_piHead = m_poList[j].m_iNext;
		}


	} else {
		//##空间不够
		THROW(MBC_SHMIntHashIndex+1);
	}
	return j;
}


//##ModelId=42442A29038B
void SHMIntHashIndex_A::add(long key, unsigned int value)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;

	if (m_poHash[i]) {
		j = m_poHash[i];
		while (j) {
			if (m_poList[j].m_lKey == key) {
				m_poList[j].m_iValue = value;
				return;
			} else {
				j = m_poList[j].m_iNext;
			}
		}

		j = malloc();

		m_poList[j].m_iNext = m_poHash[i];
		m_poHash[i] = j;
		m_poList[j].m_lKey = key;
		m_poList[j].m_iValue = value;

	} else {

		j = malloc();
		m_poList[j].m_iNext = 0;
		m_poHash[i] = j;
		m_poList[j].m_lKey = key;
		m_poList[j].m_iValue = value;


	}
}

//##ModelId=42442A5B0134
bool SHMIntHashIndex_A::get(long key, unsigned int *pvalue)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;

	if (!(j=m_poHash[i]))
		return false;

	while (j) {
		if (m_poList[j].m_lKey == key) {
			*pvalue = m_poList[j].m_iValue;
			return true;
		}

		j = m_poList[j].m_iNext;
	}

	return false;
}


void SHMIntHashIndex_A::reset()
{
	empty();
}

//##ModelId=42442B02030B
void SHMIntHashIndex_A::create(unsigned int itemnumber) 
{
	//##计算HASHVALUE
	unsigned int hashvalue;
	hashvalue = (unsigned int)(itemnumber*0.8);
	hashvalue |= 1;
	unsigned int i = 2;

	while (i && (hashvalue > i)) {
		i = (i << 1);
	}

	if (i) {
		hashvalue = i - 1;

		if (hashvalue > 1) {
			hashvalue = (hashvalue >> 1);
		}

	} else {
		hashvalue = ((~0)>>1);
	}

	//##创建
	unsigned long size = (hashvalue+1)*sizeof(unsigned int) + \
		(itemnumber+1)*sizeof(SHMIntList_A) + \
		10*sizeof(unsigned int);
	SHMAccess::create (size);

	open ();

	*m_piHashValue = hashvalue;
	*m_piTotal = itemnumber;
	*m_piUsed = 1;
	*m_piHead  = 1;
	*m_piTail  = *m_piTotal;
	*m_piVersion = 3;

	*(m_piHashValue+1) = 1; ///老版本对应的总量置1
	*(m_piHashValue+2) = 2; ///老版本对应的已使用量置2，置2表示使用数为2-1

	m_poList = (SHMIntList_A *)(m_poHash + ((*m_piHashValue)/8)*8 + 8);

	for( unsigned int i=*m_piHead;i < *m_piTail;i++)
	{
		m_poList[i].m_iNext = i+1;
	}
	m_poList[*m_piTail].m_iNext = 0;

}

//##ModelId=4244D04101A3
void SHMIntHashIndex_A::open()
{

	m_piHashValue = (unsigned int *)((char *)(*m_poSHM))+1;
	m_piVersion = m_piHashValue - 1;
	m_piTotal = m_piHashValue + 3;
	m_piUsed = m_piHashValue + 4;
	m_piHead = m_piHashValue + 5;
	m_piTail = m_piHashValue + 6;
	m_piDelOver = m_piHashValue + 7;
	m_poHash = m_piHashValue + 8;
	m_poHash = m_poHash + 1;
	m_poList = (SHMIntList_A *)(m_poHash + ((*m_piHashValue)/8)*8 +8);

}

void SHMIntHashIndex_A::empty()
{
	memset (m_poHash, 0, sizeof (unsigned int)*(*m_piHashValue+1));
	memset (m_poList, 0, sizeof (SHMIntList_A)*(*m_piTotal+1));
	*m_piUsed = 1;
	*m_piHead = 1;
	*m_piTail = *m_piTotal;

	for(int i=*m_piHead;i<*m_piTail;i++)
	{
		m_poList[i].m_iNext = i+1;
	}
	m_poList[*m_piTail].m_iNext = 0;

	*m_piDelOver = 0;
}

//删除
bool SHMIntHashIndex_A::revokeIdx(long key, unsigned int &value)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;


	if (!(j=m_poHash[i]))
		return false;

	unsigned     int* puiPrePosition = &m_poHash[i];

	while(j)
	{
		if(m_poList[j].m_lKey == key)//此元素为要删除的元素
		{
			value = m_poList[j].m_iValue;
			m_poList[j].m_lKey = 0;
			m_poList[j].m_iValue = 0;

			*puiPrePosition = m_poList[j].m_iNext;
			m_poList[j].m_iNext = 0;

			if( *m_piUsed <= *m_piTotal )//索引空间未用完
				m_poList[*m_piTail].m_iNext = j;
			else
				*m_piHead = j;

			*m_piTail = j;

			(*m_piUsed)--;

			return true;
		}
		else
		{

			puiPrePosition = &m_poList[j].m_iNext;
			j = m_poList[j].m_iNext;
		}
	}
	return false;
}

void SHMIntHashIndex_A::showDetail()
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
			keys, 0,(*m_piVersion), "I_INDEX_A", sizeof(SHMIntList_A), 0, 0, 0 );
	}else{
		char * pp = (char * )(*m_poSHM);
		unsigned int uiTotal = getTotal();
		unsigned int uiCount = getCount();
		int percent = 100*uiCount/uiTotal;
		if( (100*uiCount)%uiTotal )
			percent++;
		snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4u%%",
			keys, pp,(*m_piVersion), "I_INDEX_A", sizeof(SHMIntList_A), uiTotal, uiCount, percent );
	}
	cout<< sText<<flush;
}

int SHMIntHashIndex_A::getUsedSize()
{
	int  size = (*m_piHashValue+1)*sizeof(unsigned int) + \
		(*m_piUsed)*sizeof(SHMIntList_A) + \
		10*sizeof(unsigned int);
	if(size)
		return size;
	return 0;
}

unsigned long  SHMIntHashIndex_A::getMemAdress()
{		
	if (m_poSHM)
	{
		return  (unsigned long )m_poSHM;	
	}
	return  0;
}
time_t SHMIntHashIndex_A ::getLoadTime()
{
	time_t shm_ctime;

	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();

		return shm_ctime ;
	}
	return  0;


}
unsigned int SHMIntHashIndex_A::getMemSize()
{
	int  size = (*m_piHashValue+1)*sizeof(unsigned int) + \
		(*m_piTotal+1)*sizeof(SHMIntList_A) + \
		10*sizeof(unsigned int);

	if(size)
		return size;
	return 0;
}
