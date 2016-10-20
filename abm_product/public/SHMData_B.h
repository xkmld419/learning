/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMDATA_B_H_HEADER_INCLUDED_BDBB63C4
#define SHMDATA_B_H_HEADER_INCLUDED_BDBB63C4

#include "HashList.h"
#include "SHMAccess.h"
#include <unistd.h>
#include <iostream>
using namespace std;

#define ATTACH_DATA_B(X, Y, Z) \
        X = new SHMData_B<Y> (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }


const int SHM_BLOCK_NUM =250 ;       /////内存分块数
const int SHM_BLOCK_KEY_NUM =250;    ////最多容纳的不同账期数
const int SHM_BLOCK_MAX_KEY_VALUE = 999999999;  ////不进行自动回收的key

class SHM_ARRAY{
    public:
unsigned int m_iTotalCnt;
unsigned int m_iUsedCnt;
int m_iKey;
};

class SHM_KEY_BLOCK_ARRAY{
    public:
SHM_ARRAY m_aBlock[SHM_BLOCK_NUM];
unsigned char m_icBKey[SHM_BLOCK_KEY_NUM];
unsigned char m_icBCur;

};

//## 共享内存数据区模板类，要求存放的数据的
//## 每个元素的大小不小于28个字节，如果元素的
//## 大小小于28个字节，会出现问题
template <class T>
class SHMData_B : public SHMAccess
{
  public:

    SHMData_B(char *shmname);

    SHMData_B(long shmkey);

    ~SHMData_B();

    operator T *();
    
    void create(
        unsigned int itemnumber) ;

    void reset();

//    unsigned int malloc();
    unsigned int malloc(const int iKey);
    int  revoke(const int iKey);
    int  getAutoRevokeKey();
    void showDetail();
    int  getKeyByOffset(unsigned int iOffset);

    unsigned int getCount();
    unsigned int getCountMem();
    unsigned int getTotal();
     time_t getLoadTime();
	unsigned  int  getMemSize();
	unsigned  int  getUsedSize();
	unsigned long   getMemAdress();

    inline void checkSafe(){
        if(0 == *m_piFlag) return;
        while(*m_piFlag){
            usleep(102400);
        }            
    };
    
    bool setRevokeFlag( unsigned int iFlag){
        if(m_poSHM && m_piFlag){
            *m_piFlag = iFlag; 
            return true;
        }
        return false;
    };
    
    bool ifSetRevokeFlag(){
        if( *m_piFlag )
            return true;
        return false;
        };
    
    void getBlockInfo(SHM_KEY_BLOCK_ARRAY *pBlockArray ){
        *pBlockArray = *m_pBArray;
    };

  private:

    void open();
    unsigned char mallocBlock(const unsigned char icCurBlock, const int iKey);
  protected :
    unsigned int *m_piTotal;
    unsigned int *m_piUsed;
    T *m_pPointer;
    ///内存结构模式ID，用作兼容性，老版本是0，第2版有1(按行)和2(按块)两个取值。
    unsigned int *m_piVersion; 
    unsigned int *m_piTotalSE;
    unsigned int *m_piUsedSE;
    unsigned int *m_piDeledSE;
    unsigned int *m_piFlag;     ///如果置位，表示在做内存清理操作，此时所有其他操作要暂停

    SHM_KEY_BLOCK_ARRAY *m_pBArray;

};


template <class T>
SHMData_B<T>::SHMData_B(char *shmname) :
SHMAccess (shmname)
{
    if (m_poSHM){
        open ();
        if( 2 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }
}


template <class T>
SHMData_B<T>::SHMData_B(long shmkey) :
SHMAccess (shmkey)
{
    if (m_poSHM){
        open ();
        if( 2 != *m_piVersion )
            THROW(MBC_SHMData+1);

    }
}


//##ModelId=42441AD801E8
template <class T>
SHMData_B<T>::~SHMData_B()
{
}

template <class T>
SHMData_B<T>::operator T *()
{
	return m_pPointer;
}


template <class T>
void SHMData_B<T>::create(unsigned int itemnumber) 
{
    if( sizeof(T)<28 )
        THROW(MBC_SHMData+3);
    SHMAccess::create((itemnumber+1)* sizeof(T)+ sizeof(SHM_KEY_BLOCK_ARRAY)  );
    open ();
    
    ////老程序连上新内存让其无法分配空间
	*m_piTotal = 1;
	*m_piUsed = 1;
        
    *m_piVersion = 2;
	*m_piTotalSE = itemnumber + 1 ;
	m_pBArray =(SHM_KEY_BLOCK_ARRAY *) &m_pPointer[(*m_piTotalSE)];

    for( int i=0; i<SHM_BLOCK_NUM; i++)
    {
        if( SHM_BLOCK_NUM-1 == i )
            m_pBArray->m_aBlock[i].m_iTotalCnt = *m_piTotalSE;
        else    
            m_pBArray->m_aBlock[i].m_iTotalCnt = ( (*m_piTotalSE)/SHM_BLOCK_NUM )*(i+1) + 1;
            
        if(0==i)
            m_pBArray->m_aBlock[i].m_iUsedCnt = 1;
        else
            m_pBArray->m_aBlock[i].m_iUsedCnt = m_pBArray->m_aBlock[i-1].m_iTotalCnt;
        
        m_pBArray->m_aBlock[i].m_iKey = 0;
    }
    for( int i=0; i<SHM_BLOCK_KEY_NUM; i++){
        m_pBArray->m_icBKey[i] = 255;
    }
    
    m_pBArray->m_icBCur = 0;
    (*m_piUsedSE) = 1;

}

template <class T>
void SHMData_B<T>::reset()
{
    for( int i=0; i<SHM_BLOCK_NUM; i++){
        if( SHM_BLOCK_NUM-1 == i )
            m_pBArray->m_aBlock[i].m_iTotalCnt = *m_piTotalSE;
        else    
            m_pBArray->m_aBlock[i].m_iTotalCnt = ( (*m_piTotalSE)/SHM_BLOCK_NUM )*i + 1;
            
        if(0==i)
            m_pBArray->m_aBlock[i].m_iUsedCnt = 1;
        else
            m_pBArray->m_aBlock[i].m_iUsedCnt = m_pBArray->m_aBlock[i-1].m_iTotalCnt;
        
        m_pBArray->m_aBlock[i].m_iKey = 0;
    }
    for( int i=0; i<SHM_BLOCK_KEY_NUM; i++){
        m_pBArray->m_icBKey[i] = 255;
    }
    
    m_pBArray->m_icBCur = 0;
    (*m_piUsedSE) = 1;

}

template <class T>
unsigned char SHMData_B<T>::mallocBlock(const unsigned char icCurBlock, const int iKey)
{
    unsigned char icRet;
    for(int j=0; j<SHM_BLOCK_NUM; j++){
        icRet = (j+icCurBlock)%SHM_BLOCK_NUM;
        if(0 == m_pBArray->m_aBlock[icRet].m_iKey ){
            m_pBArray->m_aBlock[icRet].m_iKey = iKey;
            return icRet;
        }
    }
    return 255;
}


//template <class T>
//unsigned int SHMData_B<T>::malloc()
//{
//    return 0;
//}

template <class T>
unsigned int SHMData_B<T>::malloc(const int iKey)
{
    unsigned char icBlock = m_pBArray->m_icBCur;
         
    if(iKey != m_pBArray->m_aBlock[icBlock].m_iKey ){
        int i=0;
        int iFreeKey = -1;
 
        for(i=0; i<SHM_BLOCK_KEY_NUM; i++){
            if( m_pBArray->m_icBKey[i] >= SHM_BLOCK_NUM ){
                if(iFreeKey<0)  iFreeKey = i;
                continue;
            }
            if(iKey == m_pBArray->m_aBlock[  m_pBArray->m_icBKey[i] ].m_iKey ){
                icBlock = m_pBArray->m_icBKey[i];
                break;
            }
        }
        if(i== SHM_BLOCK_KEY_NUM && -1 == iFreeKey){
            ////最大的key数到了
            return 0;
        }
        if(iKey != m_pBArray->m_aBlock[icBlock].m_iKey ){
            //分配一块
            icBlock = mallocBlock( icBlock , iKey);
            if(255 == icBlock){
                return 0;
            }else{
                (*m_piUsedSE)++;
                m_pBArray->m_icBCur = icBlock;
                m_pBArray->m_icBKey[iFreeKey] = icBlock;
                return m_pBArray->m_aBlock[icBlock].m_iUsedCnt++;
            }
        }
    }
    
    /////到这里 icBlock 为找到的一个块
    if( m_pBArray->m_aBlock[icBlock].m_iTotalCnt 
        == m_pBArray->m_aBlock[icBlock].m_iUsedCnt){
        ////满了
        icBlock = mallocBlock( icBlock , iKey);
        if(255 == icBlock){
            return 0;
        }else{
            m_pBArray->m_icBCur = icBlock;
            int iTemp=0;
            for(iTemp=0; iTemp<SHM_BLOCK_KEY_NUM; iTemp++){
                if( m_pBArray->m_icBKey[iTemp] >= SHM_BLOCK_NUM ){
                    m_pBArray->m_icBKey[iTemp] = icBlock;
                    break;
                }
                if(iKey == m_pBArray->m_aBlock[  m_pBArray->m_icBKey[iTemp] ].m_iKey ){
                    m_pBArray->m_icBKey[iTemp] = icBlock;
                    break;
                }
            }
            (*m_piUsedSE)++;
            return m_pBArray->m_aBlock[icBlock].m_iUsedCnt++;
        }
        
    }
    (*m_piUsedSE)++;
    return m_pBArray->m_aBlock[icBlock].m_iUsedCnt++;
    
}


template <class T>
void SHMData_B<T>::open()
{
	m_piTotal = (unsigned *)((char *)(*m_poSHM)) + 1;
	m_piUsed = m_piTotal + 1;
	m_pPointer = (T *)((char *)(*m_poSHM));
    
    m_piVersion = m_piTotal - 1;

    m_piTotalSE = m_piTotal + 2;
    m_piFlag    = m_piTotal + 3;
    m_piUsedSE  = m_piTotal + 4;
    m_piDeledSE = m_piTotal + 5; ////不使用


    if( (*m_piTotalSE) )
        m_pBArray =(SHM_KEY_BLOCK_ARRAY *) &m_pPointer[(*m_piTotalSE)];
    else
        m_pBArray = 0;

}


template <class T>
unsigned int SHMData_B<T>::getCountMem()
{
    unsigned int uiRet = 0;
    for(int i=0;i<SHM_BLOCK_NUM; i++){
        if( m_pBArray->m_aBlock[i].m_iKey != 0 ){
            if(0 == i){
                uiRet += (m_pBArray->m_aBlock[i].m_iUsedCnt - 1 );
            }else{
                uiRet += (m_pBArray->m_aBlock[i].m_iUsedCnt - m_pBArray->m_aBlock[i-1].m_iTotalCnt );
            }
        }
    }

    return uiRet;
}


template <class T>
unsigned int SHMData_B<T>::getCount()
{
    if (*m_piUsedSE)
        return (*m_piUsedSE)-1;

    return 0;   
}


template <class T>
unsigned int SHMData_B<T>::getTotal()
{
    if (*m_piTotalSE)
        return (*m_piTotalSE)-1;

    return 0;   
}


////自动回收掉一个最小的key，至少保留一个有效的key不释放掉
template <class T>
int SHMData_B<T>::getAutoRevokeKey()
{
    if(!m_poSHM)
        return -1;
    int i = 0;
    int iBlock=0;
    int iKey = SHM_BLOCK_MAX_KEY_VALUE-1;
    
    for( i=0; i<SHM_BLOCK_KEY_NUM; i++){
        if( m_pBArray->m_icBKey[i]< SHM_BLOCK_NUM ){
            int iTemp =  m_pBArray->m_aBlock[ m_pBArray->m_icBKey[i] ].m_iKey;
            if( 0 == iTemp
              ||SHM_BLOCK_MAX_KEY_VALUE == iTemp){
                continue;
            }else{
                iBlock++;
            }
            if( iTemp < iKey )
                iKey = iTemp;

        }
    }
    
    if( iBlock < 2 ){
        return 0;
    }
    
    return iKey;
           
}


///返回释放掉的块数
template <class T>
int SHMData_B<T>::revoke(const int iKey)
{
    if(!m_poSHM)
        return -1;
    if( 0==iKey)
        return -1;
            
    setRevokeFlag(1);
    
    int iRet = 0;
        
    int i = 0;

    for( i=0; i<SHM_BLOCK_KEY_NUM; i++){
        if( m_pBArray->m_icBKey[i]< SHM_BLOCK_NUM 
            && iKey == m_pBArray->m_aBlock[ m_pBArray->m_icBKey[i] ].m_iKey ){
            m_pBArray->m_icBKey[i] = 255;
            
        }
    }
    
    for(i=0;i<SHM_BLOCK_NUM; i++){
        if(iKey ==  m_pBArray->m_aBlock[i].m_iKey){
            iRet++;
            if(0==i){
                (*m_piUsedSE) -= (m_pBArray->m_aBlock[i].m_iUsedCnt -1);
                m_pBArray->m_aBlock[i].m_iUsedCnt = 1;
            }else{
                (*m_piUsedSE) -= (m_pBArray->m_aBlock[i].m_iUsedCnt - m_pBArray->m_aBlock[i-1].m_iTotalCnt);
                m_pBArray->m_aBlock[i].m_iUsedCnt = m_pBArray->m_aBlock[i-1].m_iTotalCnt;
            }
            ////
            m_pBArray->m_aBlock[i].m_iKey = 0;
            memset( & m_pPointer[m_pBArray->m_aBlock[i].m_iUsedCnt],
                0,
                (m_pBArray->m_aBlock[i].m_iTotalCnt - m_pBArray->m_aBlock[i].m_iUsedCnt )*sizeof(T) );
            
        }
    }

    setRevokeFlag(0);
    return iRet;   

}

template <class T>
void SHMData_B<T>::showDetail()
{
    if(!m_poSHM)
        return;
    
    int i;
    unsigned int iuse,itotal;
    
    iuse = 0;
    for( i=0; i<SHM_BLOCK_KEY_NUM; i++){
        if( m_pBArray->m_icBKey[i]< SHM_BLOCK_NUM){
            iuse++;
        }
    }
        
    cout<<"内存模式："<<  (*m_piVersion) <<", 内存数据条数："<< getCountMem() << endl;
    cout<<"数据区分块数："<<  SHM_BLOCK_NUM << ",当前指向："<< (int)m_pBArray->m_icBCur <<endl;
    cout<<"最大能容纳不同键值数："<<  SHM_BLOCK_KEY_NUM 
        << ",目前已存在不同键值数："<< iuse <<endl;


    for(i=0;i<SHM_BLOCK_NUM; i++){
        cout<< "  块" << i << "的key："<< m_pBArray->m_aBlock[i].m_iKey;
        cout<<" 已使用/总条数,百分比:["; 
        if(0 == i){
            iuse = m_pBArray->m_aBlock[i].m_iUsedCnt -1 ;
            itotal = m_pBArray->m_aBlock[i].m_iTotalCnt - 1 ;
            
        }else{
            iuse = m_pBArray->m_aBlock[i].m_iUsedCnt - m_pBArray->m_aBlock[i-1].m_iTotalCnt ;
            itotal = m_pBArray->m_aBlock[i].m_iTotalCnt - m_pBArray->m_aBlock[i-1].m_iTotalCnt ;
        }
        cout<<iuse <<"/" << itotal<<",\t" << iuse*100/itotal <<"%]" <<endl ;

    }
    
    
}

/////根据偏移返回属于哪个key
template <class T>
int  SHMData_B<T>::getKeyByOffset(unsigned int iOffset)
{
    if(0 ==iOffset)
        return -1;
        
    unsigned int i = (iOffset-1)/( (*m_piTotalSE)/SHM_BLOCK_NUM ) ;
    if( i>=0 && i<SHM_BLOCK_NUM ){
        return  m_pBArray->m_aBlock[i].m_iKey;
    }
    return -1;
    
}
template <class T>
unsigned int SHMData_B<T>::getMemSize()
{
	unsigned int  size = 0;
		
	if (*m_piTotal)
	{
	 	size = (*m_piTotalSE+1)* sizeof(T)+ sizeof(SHM_KEY_BLOCK_ARRAY);
	}
	
	if (size)
		return size;
	
	return 0;
}
template <class T>
unsigned int SHMData_B<T>::getUsedSize()
{
	unsigned int size = 0;
	
	if (*m_piUsed)
	{
		size = (*m_piUsedSE+1)* sizeof(T)+ sizeof(SHM_KEY_BLOCK_ARRAY) ;
				
				
	}
	if (size)
		return size;
	
	return 0;
}

template <class T>
unsigned long SHMData_B<T>::getMemAdress()
{		
	if (m_poSHM)
	{
		return  (unsigned long )m_poSHM;	
	}
	return  0;
}
template <class T>
time_t  SHMData_B<T>::getLoadTime()
{
	time_t shm_ctime;
	
	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();
		

		return shm_ctime ;
	}
	return  0;
}

#endif
