/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMIntHashIndex_KR.h"

#include "MBC.h"

//##ModelId=424426D6039B
SHMIntHashIndex_KR::SHMIntHashIndex_KR(char *shmname) :
SHMAccess(shmname), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424426D603B9
SHMIntHashIndex_KR::SHMIntHashIndex_KR(long shmkey) :
SHMAccess(shmkey), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
	if (m_poSHM)
		open ();
}


//##ModelId=424426D603D7
SHMIntHashIndex_KR::~SHMIntHashIndex_KR()
{
}

//##ModelId=42442A29038B
void SHMIntHashIndex_KR::add(long key, unsigned int value)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;
	unsigned int iReuse = 0;
	
	if (m_poHash[i]) {
		j = m_poHash[i];
		while (j) {
			if (m_poList[j].m_lKey == key) {
				m_poList[j].m_iValue = value;
				return;
			} else {
			    if(!iReuse && SHM_INT_KEY_REUSE == m_poList[j].m_lKey){
			        iReuse = j;
			    }
				j = m_poList[j].m_iNext;
			}
		}
		if(iReuse){
			m_poList[iReuse].m_lKey = key;
			m_poList[iReuse].m_iValue = value;
		}else{
		
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
bool SHMIntHashIndex_KR::get(long key, unsigned int *pvalue)
{
	unsigned int i = (key & (*m_piHashValue));
	unsigned int j;

	if (!(j=m_poHash[i]))
		return false;

	while (j) {
		if (m_poList[j].m_lKey == key) {
			//*pvalue = m_poList[j].m_iValue;
			i = m_poList[j].m_iValue;
			if(i){
			    *pvalue = i;
			    return true;
			}
			return false;
		}
		
		j = m_poList[j].m_iNext;
	}

	return false;
}

//删除
bool SHMIntHashIndex_KR::eraseData(const long key)
{
    unsigned int i = (key & (*m_piHashValue));
    unsigned int j;

    if (!(j=m_poHash[i]))
        return false;
    unsigned int * puiLast = &m_poHash[i];
    while(j){
        if(key == m_poList[j].m_lKey){
            m_poList[j].m_lKey = SHM_INT_KEY_REUSE;
            m_poList[j].m_iValue = 0;
            *puiLast = m_poList[j].m_iNext;
            m_poList[j].m_iNext = 0;
            return true;
            break;
            ////*m_piUsed = (*m_piUsed)--;
        }
        puiLast = & m_poList[j].m_iNext;
        j = m_poList[j].m_iNext;
        //m_poList[j].m_iNext = 0;
        
    }
    //m_poHash[i] = 0;
    return false;
}

bool SHMIntHashIndex_KR::revokeIdx(const long key, unsigned int &value)
{
    unsigned int i = (key & (*m_piHashValue));
    unsigned int j;

    if (!(j=m_poHash[i]))
        return false;
    unsigned int * puiLast = &m_poHash[i];
    while(j){
        if(key == m_poList[j].m_lKey){
            value = m_poList[j].m_iValue;
            m_poList[j].m_lKey = SHM_INT_KEY_REUSE;
            m_poList[j].m_iValue = 0;
            *puiLast = m_poList[j].m_iNext;
            m_poList[j].m_iNext = 0;
            
            return true;
            break;

        }
        puiLast = & m_poList[j].m_iNext;
        j = m_poList[j].m_iNext;

        
    }

    return false;

}

void SHMIntHashIndex_KR::reset()
{
	*m_piUsed = 1;
}

//##ModelId=42442B02030B
void SHMIntHashIndex_KR::create(unsigned int itemnumber) 
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
				itemnumber*sizeof(SHMIntList_KR) + \
				4*sizeof(unsigned int);
	SHMAccess::create (size);

	open ();

	*m_piHashValue = hashvalue;
	*m_piTotal = itemnumber;
	*m_piUsed = 1;

	
	m_poList = (SHMIntList_KR *)(m_poHash + (*m_piHashValue) + 1) - 1;
}

//##ModelId=4244D04101A3
void SHMIntHashIndex_KR::open()
{
	m_piHashValue = (unsigned int *)((char *)(*m_poSHM)) + 1;
	m_piTotal = m_piHashValue + 1;
	m_piUsed = m_piTotal + 1;

	m_poHash = m_piUsed + 1;
	m_poList = (SHMIntList_KR *)(m_poHash + (*m_piHashValue) + 1) - 1;
}

void SHMIntHashIndex_KR::empty()
{
        memset (m_poHash, 0, sizeof (unsigned int)*(*m_piHashValue));
		memset (m_poList, 0, sizeof (SHMIntList_KR)*(*m_piTotal));//wangs add

        *m_piUsed = 1;
}

unsigned int SHMIntHashIndex_KR::getUsedSize()
{
	    int  size = *m_piHashValue*sizeof(unsigned int) + \
					    (*m_piUsed)*sizeof(SHMIntList_KR) + \
						    4*sizeof(unsigned int) ;
		       if(size)
				              return size;
			          return 0;
}

unsigned long  SHMIntHashIndex_KR::getMemAdress()
{		
	if (m_poSHM)
	{
		return  (unsigned long )m_poSHM;	
	}
	return  0;
}
time_t SHMIntHashIndex_KR ::getLoadTime()
{
	time_t shm_ctime;
	
	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();
		
		return shm_ctime ;
	}
	return  0;

}
unsigned int SHMIntHashIndex_KR::getMemSize()
{
	int  size = *m_piHashValue * sizeof(unsigned int) + \
			(*m_piTotal)*sizeof(SHMIntList_KR) + \
				4*sizeof(unsigned int) ;
		
	 if (size > 0)
		   return size;
		   
	 return 0;
}


