/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMStringTreeIndex_A_H_HEADER_INCLUDED
#define SHMStringTreeIndex_A_H_HEADER_INCLUDED
#include "SHMAccess.h"
#include <string.h>

#define SHM_STRING_KEY_LEN_A	12
//##实际影响key的长度，考虑目前电信号码一般是11位，所以，这里设置为11

#define ATTACH_STRING_INDEX_A(X, Z) \
        X = new SHMStringTreeIndex_A (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }

////zhangap 增加索引list区和tree区的循环使用的功能


//##ModelId=424431C703DF
//##Documentation
//## 这里要考虑内存8个字节对齐
class SHMStringList_A
{
    friend class SHMStringTreeIndex_A;
public:

    //##ModelId=424431FD01A2
    unsigned int m_iValue;

    //##ModelId=4244320B00DA
    unsigned int m_iNext;

    //##ModelId=424431EF02EC
    char m_sKey[8];

};

//##ModelId=424433A902A2
class SHMStringTree_A
{
    friend class SHMStringTreeIndex_A;
public:
    //##ModelId=424433B603B9
    unsigned int m_iNext[10];

    //##ModelId=424433C400B6
    unsigned int m_iValue;

};


class SHMPreTree_A
{
public:
    SHMStringTree_A * pTree;
    unsigned int  iOffset;
};


//##ModelId=4244298B0031
//##Documentation
//## 字符串型共享内存索引，主要用于对主业务号码类的索引，
//## 该类采用KeyTree算法，很耗空间，预计用户数超过800万
//## 的时候，建议使用sHMStringHashIndex。
class SHMStringTreeIndex_A : public SHMAccess
{
public:


	class Iteration
	{
	  public:
        ////该函数依次返回整个索引中的key以及key所对应的value
        bool next(const char * &pkey, unsigned int & value)
        {
            SHMStringList_A *pl;
            
            if(m_iCurI > m_pIdx->getlTotal() 
                || m_iCurJ >= m_iCount ){
                return false;
            }
            m_puiPrePosition = 0;
            m_iCurOffset = 0;
            m_iTreePre = 0;
            for( ; m_iCurI <= m_pIdx->getlTotal() ; m_iCurI++) {
                pl = m_pIdx->getList(m_iCurI);
                
                if(!pl)
                    continue;
                
                ////空着的是删掉的或者还没有使用的
            	if(! pl->m_sKey[0] || !pl->m_iValue )
            	    continue;
            	
            	unsigned int i, j;
            	char *p = pl->m_sKey;
            	unsigned int index = 0;
           	    
            	for(i=0; i<SHM_STRING_KEY_LEN_A && *p; p++,i++) {
            		j = (*p)%10;
            		
            		if (m_pIdx->m_poTree[index].m_iNext[j]) {
            		    m_pPre[i].pTree = &m_pIdx->m_poTree[index];
            		    m_pPre[i].iOffset = j;
            			index = m_pIdx->m_poTree[index].m_iNext[j];
            		} else {
            			index = 0;
            			break;
            		}
            		
            	}
                if(0==index) 
                    continue;

                ////老版本的最后这个tree节点只用了 m_iValue 一个值，其他的10个next空间都浪费了
                ////此处继承吧 为了兼容                
            	if (!m_pIdx->m_poTree[index].m_iValue)
            	    continue;
            	
            	m_iTreePre = i-1;
            	

            	j = m_pIdx->m_poTree[index].m_iValue;
            	p = pl->m_sKey;
            	pl = m_pIdx->getList(j);
            	m_puiPrePosition = &m_pIdx->m_poTree[index].m_iValue;
            	m_iCurOffset = j;
            	m_iNextOffset = pl->m_iNext;
            	while ( pl ) {
            		if (!strncmp(pl->m_sKey, p, m_iStringKeyLen) ){
            			m_pl = pl;
                        pkey = m_pl->m_sKey;
                        value = m_pl->m_iValue;
                        m_iCurJ++;
                        m_iCurI++;
            			return true;
            		}
            		m_puiPrePosition = &pl->m_iNext;
            		m_iCurOffset = pl->m_iNext;
            		pl = m_pIdx->getList (pl->m_iNext);
            		m_iNextOffset = pl->m_iNext;
            	}

            }
            m_puiPrePosition = 0;
            m_iCurOffset = 0;
            m_iTreePre = 0;
            m_pl = 0;
            return false;
            
        }


		Iteration ( SHMStringTreeIndex_A * pIdx, int iKeyLen):
		m_iCurI(1),m_iCurJ(0), 
		m_iCurOffset(0),m_puiPrePosition(0),m_pl(0),m_iTreePre(0),
		m_pIdx(pIdx),m_iStringKeyLen(iKeyLen)
		{
		    memset(m_pPre,0,sizeof(m_pPre) );
		    m_iCount = pIdx->getlCount();
		}

		Iteration (const Iteration & right)
		{
			m_iCurI = right.m_iCurI;
			m_iCurJ = right.m_iCurJ;
			m_iCurOffset = right.m_iCurOffset;
			m_iNextOffset= right.m_iNextOffset;
			m_puiPrePosition = right.m_puiPrePosition;
			
			m_pIdx = right.m_pIdx;
			m_pl = right.m_pl;
			m_iStringKeyLen = right.m_iStringKeyLen;
            
            m_iTreePre = right.m_iTreePre;
            memcpy(m_pPre, right.m_pPre, sizeof(m_pPre) );
            m_iCount = right.m_iCount;
		}

        bool setValue( unsigned int iValue ){
            if( m_iCurOffset && m_pl){
                
                if(0 == iValue){
                    return removeMe();
                }
                m_pl->m_iValue = iValue;
                return true;
            }
            return false;
        }
        
        bool removeMe(){
            ////使用迭代器，删除本节点
            unsigned int j=0;
            unsigned int * m_piListUsed = m_pIdx->m_piListUsed;
            unsigned int * m_piListTotal = m_pIdx->m_piListTotal;
            unsigned int * m_piListTail = m_pIdx->m_piListTail;
            unsigned int * m_piListHead = m_pIdx->m_piListHead;

            if(m_puiPrePosition && (*m_puiPrePosition == m_iCurOffset) ){
                *m_puiPrePosition = m_iNextOffset;
                
                j = m_iCurOffset;
                m_pl->m_sKey[0] = 0;
                m_pl->m_iValue = 0;
                memset(m_pl->m_sKey, 0, m_iStringKeyLen );
                m_pl->m_iNext = 0;
                
                if( *m_piListUsed <= *m_piListTotal ){
                    //索引空间未用完
                    SHMStringList_A * pList = m_pIdx->getList(*m_piListTail);
                    pList->m_iNext = j;
                }else{
                    *m_piListHead = j;
                }
                *m_piListTail = j;
                (*m_piListUsed)--;

                j = m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ];
    			if(m_iTreePre && 0 == m_pIdx->m_poTree[ j ].m_iValue ){
                    unsigned int * m_piTreeUsed = m_pIdx->m_piTreeUsed;
                    unsigned int * m_piTreeTotal = m_pIdx->m_piTreeTotal;
                    unsigned int * m_piTreeTail = m_pIdx->m_piTreeTail;
                    unsigned int * m_piTreeHead = m_pIdx->m_piTreeHead;    			    
    			    SHMStringTree_A treeTemp;
    			    memset(&treeTemp, 0, sizeof(SHMStringTree_A) );
    			    
    			    while(0==memcmp(&m_pIdx->m_poTree[j], &treeTemp, sizeof(SHMStringTree_A)) ){
    			        ////从最底层的Tree，一直往上删

                        m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ] = 0;
                        if( *m_piTreeUsed < *m_piTreeTotal ){
                            //索引空间未用完
                            m_pIdx->m_poTree[*m_piTreeTail].m_iNext[0] = j;
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
            return false;
            
        }
	  private:
		unsigned int m_iCurOffset;
		unsigned int m_iNextOffset;
		unsigned int * m_puiPrePosition;

        unsigned int m_iCurI;
        unsigned int m_iCurJ;
        SHMStringList_A *m_pl;
		SHMStringTreeIndex_A * m_pIdx;
		int m_iStringKeyLen;

		SHMPreTree_A m_pPre[SHM_STRING_KEY_LEN_A];
		int  m_iTreePre;
		unsigned int m_iCount;///---保存获取迭代器时的条数
	};
	
	    
  public:
    Iteration getIteration ()
	{
		return Iteration (this, *m_piKeyLen );
	}

	friend class Iteration;

    //##ModelId=424429AB039E
    SHMStringTreeIndex_A(char *shmname);

    //##ModelId=424429AB03BC
    SHMStringTreeIndex_A(long shmkey);

    //##ModelId=424429AC001A
    ~SHMStringTreeIndex_A();

    //##ModelId=42442A8B0093
    void add(char *key, unsigned int value);

    //##ModelId=42442A9101C8
    bool get(char *key, unsigned int *pvalue);

    bool getMax(char *key, unsigned int *pvalue);

    bool revokeIdx(char * key, unsigned int &value);
    void reset(){ empty(); };
    void empty();
    void showDetail();
    
    unsigned int mallocTree();
    unsigned int mallocList();
    

    //##ModelId=42442B0802C3
    //##Documentation
    //## 创建数据区
    void create(
        //##Documentation
        //## 最多元素个数
        unsigned int itemnumber, 
        //##Documentation
        //## key的最大长度
        unsigned int keylen, 
        //##Documentation
        //## keytree的节点数是元素数目的倍数
        unsigned int multiple = 3) ;
        
//M_poTree[0]是默认的开始项，已经被分配掉了
	unsigned int getCount()
	{
		if (*m_piTreeUsed)
			return (*m_piTreeUsed);

		return 0;
	};

	unsigned int getTotal()
	{
		if (*m_piTreeTotal)
			return (*m_piTreeTotal);

		return 0;
	};

	unsigned int getlCount()
	{
		if (*m_piListUsed)
			return (*m_piListUsed)-1;

		return 0;		
	};

	unsigned int getlTotal()
	{
		if (*m_piListTotal)
			return (*m_piListTotal);

		return 0;
	};
	
  private:
    //##ModelId=4244EBC6017B
    void open();
    //##ModelId=424512FC000C
    SHMStringList_A *getList(unsigned int offset);


    //##ModelId=4244331C0050
    unsigned int *m_piListTotal;

    //##ModelId=4244332E0088
    unsigned int *m_piListUsed;

    //##ModelId=424433510223
    unsigned int *m_piTreeTotal;

    //##ModelId=4244335F02B9
    unsigned int *m_piTreeUsed;

    //##ModelId=42443696006E
    //##Documentation
    //## SHMStringList的真正长度
    unsigned int *m_piListLen;

    //##ModelId=424432C50326
    char *m_poList;

    //##ModelId=424433EE034B
    SHMStringTree_A *m_poTree;
    //##ModelId=424501EC00DE
    unsigned int *m_piKeyLen;



    unsigned int *m_piTreeHead;//指向可分配链的头结点，每次需要分配一个，则从头部取
    unsigned int *m_piTreeTail;//指向可分配链的尾接点，每次删除一个，则加到尾部
    unsigned int *m_piListHead;//指向可分配链的头结点，每次需要分配一个，则从头部取
    unsigned int *m_piListTail;//指向可分配链的尾接点，每次删除一个，则加到尾部
    unsigned int *m_piVersion; ////内存结构的版本号 置4


};


#endif /* SHMStringTreeIndex_A_H_HEADER_INCLUDED_BDBB74AD */
