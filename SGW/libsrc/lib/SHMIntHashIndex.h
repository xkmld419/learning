/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A
#define SHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A
#include <memory.h>
#include "SHMAccess.h"

#define ATTACH_INT_INDEX(X, Z) \
				if (Z==0) { \
				THROW(MBC_UserInfo+1); \
				} \
        X = new SHMIntHashIndex (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
			printf("%s(%d): ATTACH_INT_INDEX failed (key:%ld)\n",__FILE__,__LINE__,(long)Z); \
        }

//##ModelId=4244323E0173
class SHMIntList
{
    friend class SHMIntHashIndex;
public:
    //##ModelId=424432520367
    long m_lKey;

    //##ModelId=4244325B026F
    unsigned int m_iValue;

    //##ModelId=424432640377
    unsigned int m_iNext;

};

//##ModelId=424423010070
//##Documentation
//## Key为整数的共享内存索引
//## 采用HASHLIST算法
class SHMIntHashIndex : public SHMAccess
{
  public:
	class Iteration
	{
	  public:
		bool next(long & key, unsigned int & value)
		{
			unsigned int j;
            
            if(m_iCur>m_iHash){
                return false;
            }
            if( m_iNextOffset !=0
                && 0 != m_pIdx->m_poList[m_iNextOffset].m_iValue){
                m_iCurOffset = m_iNextOffset;
                
                value = m_pIdx->m_poList[m_iCurOffset].m_iValue;
                key = m_pIdx->m_poList[m_iCurOffset].m_lKey;
                m_iNextOffset = m_pIdx->m_poList[m_iCurOffset].m_iNext;
                return true;
                
            }
            if(0==m_iNextOffset){
                m_iCur++;
                m_iNextOffset = 0;
                m_iCurOffset = 0;
                if(m_iCur>m_iHash){
                    return false;
                }
            }
            while( 1 ){
                j=m_pIdx->m_poHash[m_iCur-1];
                if( 0 == j){
                    m_iCur++;
                    m_iNextOffset = 0;
                    m_iCurOffset = 0;
                    if(m_iCur>m_iHash){
                        return false;
                    }
                    continue;
                }
                if(0 == m_iNextOffset )
                    m_iNextOffset = j;
                    
                while( m_iNextOffset != 0 
                    && 0 == m_pIdx->m_poList[m_iNextOffset].m_iValue ){
                    m_iNextOffset = m_pIdx->m_poList[m_iNextOffset].m_iNext;
                }
                
                if(0 == m_iNextOffset ){
                    m_iCur++;
                    m_iNextOffset = 0;
                    m_iCurOffset = 0;
                    if(m_iCur>m_iHash){
                        return false;
                    }
                    continue;
                }
                
                break;
                
            }
            m_iCurOffset = m_iNextOffset;
			value = m_pIdx->m_poList[m_iCurOffset].m_iValue;
			key = m_pIdx->m_poList[m_iCurOffset].m_lKey;
			m_iNextOffset = m_pIdx->m_poList[m_iCurOffset].m_iNext;
			return true;		
		}

		Iteration ( unsigned int iHash, SHMIntHashIndex * pIdx):
		m_iCur(0), m_iCurOffset(0),m_iNextOffset(0), m_iHash(iHash), m_pIdx(pIdx)
		{}

		Iteration (const Iteration & right)
		{
			m_iCur = right.m_iCur;
			m_iHash = right.m_iHash;
			m_iCurOffset = right.m_iCurOffset;
			m_iNextOffset = right.m_iNextOffset;
			m_pIdx = right.m_pIdx;
		}

        bool setValue( unsigned int iValue ){
            if( m_iCurOffset ){
                m_pIdx->m_poList[m_iCurOffset].m_iValue = iValue;
                return true;
            }
            return false;
        }
        
	  private:
		unsigned int m_iCur;
		unsigned int m_iHash;
		unsigned int m_iCurOffset;
		unsigned int m_iNextOffset;
		
		SHMIntHashIndex * m_pIdx;
	};
	
	Iteration getIteration ()
	{
		return Iteration ( (*m_piHashValue)+1, this);
	}

    friend class Iteration;

  	
  	 time_t  getLoadTime();
    
    SHMIntHashIndex(char *shmname);
    //##ModelId=424426D603B9
    SHMIntHashIndex(long shmkey);

    //##ModelId=424426D603D7
    ~SHMIntHashIndex();

    //##ModelId=42442A29038B
    void add(long key, unsigned int value);

    //##ModelId=42442A5B0134
    bool get(long key, unsigned int *pvalue);

	void erase(long key);
	void reset();

    //##ModelId=42442B02030B
    //##Documentation
    //## 创建数据区
    void create(
        //##Documentation
        //## 最多元素个数
        unsigned int itemnumber) ;
	
	//##清空索引，为数据业务内存交换使用
	void empty();
	int  getMemSize();
	int getUsedSize();			
	unsigned long getMemAdress();//modify by jinx for app_guard 070402
	unsigned int getCount()
	{
		if (*m_piUsed)
			return (*m_piUsed)-1;

		return 0;
	};

	unsigned int getTotal()
	{
		if (*m_piTotal)
			return (*m_piTotal)-1;

		return 0;
	};

  private:
    //##ModelId=4244D04101A3
    void open();

    //##ModelId=42442E01039A
    //##Documentation
    //## hash离散度
    unsigned int *m_piHashValue;

    //##ModelId=42442F710136
    unsigned int *m_piTotal;

    //##ModelId=42442F8A02FF
    unsigned int *m_piUsed;

    //##ModelId=42442FA101B7
    unsigned int *m_poHash;
    unsigned long *m_piIndexTime;
	unsigned long *m_pimemAdr;
    //##ModelId=424432820167
    SHMIntList *m_poList;

};




#endif /* SHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A */



