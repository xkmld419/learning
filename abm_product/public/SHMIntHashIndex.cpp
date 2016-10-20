/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMIntHashIndex.h"

#include "MBC.h"

//##ModelId=424426D6039B
SHMIntHashIndex::SHMIntHashIndex(char *shmname) :
SHMAccess(shmname), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424426D603B9
SHMIntHashIndex::SHMIntHashIndex(long shmkey) :
SHMAccess(shmkey), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424426D603D7
SHMIntHashIndex::~SHMIntHashIndex()
{
}

//##ModelId=42442A29038B
void SHMIntHashIndex::add(long key, unsigned int value)
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
		
		if (*m_piUsed <= *m_piTotal) {
			j = ((*m_piUsed)++);
			m_poList[j].m_iNext = m_poHash[i];
			m_poHash[i] = j;
			m_poList[j].m_lKey = key;
			m_poList[j].m_iValue = value;
		} else {
			//##空间不够
			THROW(MBC_SHMIntHashIndex+1);
		}

	       		
	} else {
		if (*m_piUsed <= *m_piTotal) {
			j = ((*m_piUsed)++);
			m_poHash[i] = j;
			m_poList[j].m_lKey = key;
			m_poList[j].m_iValue = value;
			m_poList[j].m_iNext = 0;
		} else {
			//##空间不够
			THROW(MBC_SHMIntHashIndex+1);
		}
	}
}

//##ModelId=42442A5B0134
bool SHMIntHashIndex::get(long key, unsigned int *pvalue)
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

//删除
void SHMIntHashIndex::eraseData(const long key)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;

	if (!(j=m_poHash[i]))
		return;
    unsigned int * puiLast = &m_poHash[i];
	while(j){
        if(key == m_poList[j].m_lKey){
			m_poList[j].m_lKey = 0;
            m_poList[j].m_iValue = 0;
            *puiLast = m_poList[j].m_iNext;
            m_poList[j].m_iNext = 0;
            break;
            ////*m_piUsed = (*m_piUsed)--;
		}
        puiLast = & m_poList[j].m_iNext;
		j = m_poList[j].m_iNext;
        //m_poList[j].m_iNext = 0;
	}
    //m_poHash[i] = 0;
	return;
}

void SHMIntHashIndex::reset()
{
	*m_piUsed = 1;
}

//##ModelId=42442B02030B
void SHMIntHashIndex::create(unsigned int itemnumber) 
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
	unsigned long size = hashvalue*sizeof(unsigned int) + \
				itemnumber*sizeof(SHMIntList) + \
				4*sizeof(unsigned int)+ sizeof(SHMIntList) +1*sizeof(unsigned long) ;
	SHMAccess::create (size);
	open ();
	*m_piHashValue = hashvalue;
	*m_piTotal = itemnumber;
	*m_piUsed = 1;
	
	//m_poList = (SHMIntList *)(m_poHash + (*m_piHashValue) + 1) - 1;
   m_poList = (SHMIntList *)(m_poHash + ((*m_piHashValue)/8)*8 + 8);

}

//##ModelId=4244D04101A3
void SHMIntHashIndex::open()
{
	m_piHashValue = (unsigned int *)((char *)(*m_poSHM)) + 1;
	m_piTotal = m_piHashValue + 1;
	m_piUsed = m_piTotal + 1;
	m_poHash = m_piUsed + 1;

   m_poList = (SHMIntList *)(m_poHash + ((*m_piHashValue)/8)*8 + 8);
}

void SHMIntHashIndex::empty()
{
        memset (m_poHash, 0, sizeof (unsigned int)*(*m_piHashValue));
		memset (m_poList, 0, sizeof (SHMIntList)*(*m_piTotal));//wangs add

        *m_piUsed = 1;
}

int SHMIntHashIndex::getMemSize()
{
	int  size = *m_piHashValue*sizeof(unsigned int) + \
	(*m_piTotal)*sizeof(SHMIntList) + \
		4*sizeof(unsigned int)+ sizeof(SHMIntList) +1*sizeof(unsigned long) ;
		
	 if(size)
		   return size;
	 return 0;
}
int SHMIntHashIndex::getUsedSize()
{
	int  size = *m_piHashValue*sizeof(unsigned int) + \
					(*m_piUsed)*sizeof(SHMIntList) + \
							4*sizeof(unsigned int)+ sizeof(SHMIntList) +1*sizeof(unsigned long) ;
	if(size)
		return size;
			          
	return 0;
}

unsigned long  SHMIntHashIndex::getMemAdress()
{		
		
	if (m_poSHM)
	{
		return   (unsigned long)m_poSHM;
	}
	
	return 0;
}
time_t SHMIntHashIndex ::getLoadTime()
{
	time_t shm_ctime;
	
	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();	
		
		return shm_ctime ;
	}
	return  0;

}

