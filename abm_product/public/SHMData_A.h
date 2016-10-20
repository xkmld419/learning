/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMDATA_A_H_HEADER_INCLUDED_BDBB63C4
#define SHMDATA_A_H_HEADER_INCLUDED_BDBB63C4
////说明：可按条重用的内存分配
////      使用一个数组记录删除的项，分配时先分配空闲的，再分配原来删除的项
////优点：分配快，且分配时不会影响删除，只要分配做到单入，删除做到单入，分配和删除操作可以同时进行，不需要锁。
////不足：返回总条数以及对在用数据项导出时需要进行复杂的运算才能得到结果。
////zhangap
////最后修改时间：2010-01-29 


//unsigned int i = (* m_piUsedSE );
//if(i!=0  && 0== m_pIArray[i]){
//   ////判断到还没分配满过
//   
//}
//if( 0== m_pIArray[(* m_piTotalSE)] ){
//    ////判断到还没有删除满过
//    
//}
//if( 0 == (*m_piDelOver) ){
//    ////也是判断到还没有删除满过
//}



#include "HashList.h"
#include "SHMAccess.h"
#include <iostream>
#include <unistd.h>

using namespace std;

#define ATTACH_DATA_A(X, Y, Z) \
        X = new SHMData_A<Y> (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }


//## 共享内存数据区模板类，要求存放的数据的
//## 每个元素的大小不小于32个字节，如果元素的
//## 大小小于32个字节，会出现问题
template <class T>
class SHMData_A : public SHMAccess
{
  public:
	  typedef T value_type;

	
	class Iteration
	{
      private:
        unsigned int m_iCurOffset;
        SHMData_A<T> * m_pDataA;
        unsigned char *pData;
        unsigned int m_iStateCnt;
    
	  private:

        unsigned int getInfo()
        {   /// '0'表示数据项， '1'表示空闲项
            ////返回里面有状态的总条数
            unsigned int uiTemp = (*(m_pDataA->m_piTotalSE));
            
            pData = new unsigned char[uiTemp];
            memset(pData, '0', sizeof(unsigned char)*uiTemp );

            unsigned int i = (*(m_pDataA->m_piUsedSE));
            unsigned int j =0;

            if(i!=0  && 0==m_pDataA->m_pIArray[i]){
                ////还没分配满过，删除项直接从0开始，返回值返回实际最大分配项所在的偏移
                uiTemp = i;
                i=0;
                
                for(; i != (*(m_pDataA->m_piDeledSE)); i++){
                    if( i>= uiTemp ){
                        ////可能在这个中间其他进程进行了大量数据的分配和释放操作
                        break;
                    }
                    j = m_pDataA->m_pIArray[i];
                    if( 0== j){
                        j = i;
                    }
                    pData[j]++;                       
                }
                return uiTemp;
            } 
            

            for(; i != (*(m_pDataA->m_piDeledSE)); i++){
                if( i>= uiTemp ){
                    i = 0;
                }
                j = m_pDataA->m_pIArray[i];
                if( 0== j){
                    j = i;
                }
                if( '0' == pData[j] ){
                    ////此条记录置成空闲
                    pData[j] = '1';
                    
                }else{
                    ////此条记录出现多次删除
                    pData[j]++;
                }
            }
            
            return uiTemp;

        }

	  public:
	    void close(){ 
	        if(pData){ delete [] pData; pData =0;}
	        m_iCurOffset = 1;
	        m_iStateCnt = 0;
	    }
	    
	    
	    
		bool next(T & tOut, unsigned int &uiOffsetOut)
		{
			if(!pData){
			    m_iStateCnt = getInfo();
			}
			if(pData && m_iCurOffset>0){
			    while( m_iCurOffset< m_iStateCnt ){
    			    if('0' == pData[m_iCurOffset] ){
    			        uiOffsetOut = m_iCurOffset;
        			    tOut = m_pDataA->m_pPointer[ m_iCurOffset ];
        			    m_iCurOffset++;
        			    
        			    return true;
        			}
        			m_iCurOffset++;
        		}
    			return false;
			}      
			return false;		
		}

		bool nextBatch(unsigned int uiMax, T tOut[], unsigned int &iOutCnt)
		{
			if(!pData){
			    m_iStateCnt = getInfo();
			}
			unsigned int iTotal = (*(m_pDataA->m_piTotalSE));
			unsigned int iCnt=0;
			if(pData && m_iCurOffset>0 && uiMax){
			    while(iCnt<uiMax && m_iCurOffset < m_iStateCnt ){
    			    if('0' == pData[m_iCurOffset] ){
        			    tOut[iCnt] = m_pDataA->m_pPointer[ m_iCurOffset ];
        			    iCnt++;
        			}
        			m_iCurOffset++;
        			
        		}
        		if(iCnt){
        		    iOutCnt = iCnt;
        		    return true;
        		}
        		iOutCnt = 0;
    			return false;
			}      
			return false;		
		}

		Iteration ( unsigned int iCur, SHMData_A<T> * pSHMDataA):
		    m_iCurOffset(iCur), m_pDataA(pSHMDataA),pData(0),m_iStateCnt(0)
		{}

		Iteration (const Iteration & right)
		{
			m_iCurOffset = right.m_iCurOffset;
			m_pDataA = right.m_pDataA;
            pData = 0;
            m_iStateCnt = 0;
		}
		
		~Iteration () 
		{ if(pData)
		    { delete [] pData; pData =0;}
		}
		     

	};
	
	Iteration getIteration ()
	{
		return Iteration ( 1, this);
	}

  public:
	friend class Iteration;
    SHMData_A(char *shmname, bool bShm = true);

    SHMData_A(long shmkey);

    ~SHMData_A();
    
    operator T *();

    void create(
        unsigned int itemnumber) ;

    void reset();

    bool revoke( unsigned int uiOffset);
    void showDetail();
    bool checkHealth();
    unsigned int kick(unsigned int uiBeginOff, 
        int iNxtOffset, int iExpOffset, const char * sDate, unsigned int * o_uiKickSelf, bool * o_bKickSelf );
    unsigned int kickAndRevoke(unsigned int uiBeginOff, 
        int iNxtOffset, int iExpOffset, const char * sDate,
        int iChkOffset1, int iChkOffset2, unsigned int * o_uiKickSelf , bool * o_bKickSelf);
   
    unsigned int malloc();

    unsigned int getCount();

    unsigned int getTotal();

    unsigned long getDelTotal();
    unsigned int  getDelFree ();
    time_t getLoadTime();
	unsigned  int  getMemSize();
	unsigned  int  getUsedSize();
	unsigned long  getMemAdress();
	unsigned long getRealSize();
    
    
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

    
  private:

    void open();

  protected :
    unsigned int *m_piTotal;
    unsigned int *m_piUsed;
    T *m_pPointer;
    ///内存结构模式ID，用作兼容性，老版本是0，第2版有1(按行)和2(按块) 等取值。
    unsigned int *m_piVersion; 
    unsigned int *m_piTotalSE;
    unsigned int *m_piUsedSE;
    unsigned int *m_piDeledSE;
    unsigned int *m_piFlag;     ///如果置位，表示在做内存清理操作，此时所有其他操作要暂停
    unsigned int *m_piDelOver;  ///记录删除个数达到total的次数
    unsigned int *m_pIArray; 
};


template <class T>
SHMData_A<T>::SHMData_A(char *shmname, bool bShm) :
SHMAccess (shmname, bShm)
{
    if (m_poSHM){
        open ();
        if( 1 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }
}


template <class T>
SHMData_A<T>::SHMData_A(long shmkey) :
SHMAccess (shmkey)
{
    if (m_poSHM){
        open ();
        if( 1 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }

}


//##ModelId=42441AD801E8
template <class T>
SHMData_A<T>::~SHMData_A()
{
}

template <class T>
SHMData_A<T>::operator T *()
{
    return m_pPointer;
}


template <class T>
void SHMData_A<T>::create(unsigned int itemnumber) 
{
    if( sizeof(T)< 16 )
        THROW(MBC_SHMData+2);
    SHMAccess::create((itemnumber+2)* (sizeof(T)+sizeof(int))  );
    open ();

    ////老程序连上新内存让其无法分配空间
    *m_piTotal = 2;
    *m_piUsed = 2;

    *m_piVersion = 1;
    *m_piTotalSE = itemnumber + 1 ;
    
    m_pIArray =(unsigned int *) &m_pPointer[(*m_piTotalSE)+1];

    *m_piUsedSE = 1;
    *m_piDeledSE= 0;

    if( sizeof(T)< 32 ){
        ///1个结构体放不下控制信息，此时让下标从2开始,到itemnumber+1结束
        ///平时下标是从1到itemnumber结束
        *m_piUsedSE = 2;
        *m_piDeledSE = 1;
        m_pIArray[0] = itemnumber+1;
    }
}

template <class T>
void SHMData_A<T>::reset()
{
    *m_piUsedSE = 1;
    *m_piDeledSE= 0;
    memset( m_pIArray, 0, (*m_piTotalSE) * sizeof(int) );
    *m_piDelOver =0;
    
    if( sizeof(T)< 32 ){
        ///1个结构体放不下控制信息，此时让下标从2开始,到itemnumber+1结束
        ///平时下标是从1到itemnumber结束
        *m_piUsedSE  = 2;
        *m_piDeledSE = 1;
        m_pIArray[0] = *m_piTotalSE;///itemnumber+1;
    }

}


template <class T>
unsigned int SHMData_A<T>::malloc()
{
    if (*m_piDeledSE == *m_piUsedSE)
        return 0;
    unsigned int uiRet = m_pIArray[(*m_piUsedSE)];
    if(!uiRet)
        uiRet = (*m_piUsedSE);
    
    (*m_piUsedSE)++;
    if( (*m_piUsedSE) >= (*m_piTotalSE) )
        (*m_piUsedSE) = 0 ;
   
    return uiRet;
    
}


template <class T>
void SHMData_A<T>::open()
{
    m_piTotal = (unsigned *)((char *)(*m_poSHM)) + 1;
    m_piUsed = m_piTotal + 1;
    m_pPointer = (T *)( (char *)(*m_poSHM));
    
    m_piVersion = m_piTotal - 1;
    m_piTotalSE = m_piTotal + 2;
    m_piFlag    = m_piTotal + 3;
    m_piUsedSE  = m_piTotal + 4;
    m_piDeledSE = m_piTotal + 5;
    m_piDelOver = m_piTotal + 6;
    if( (*m_piTotalSE) ) {
        m_pIArray =(unsigned int *) &m_pPointer[(*m_piTotalSE)+1];

    }else{
        m_pIArray = 0;
    }



}
template <class T>
unsigned int SHMData_A<T>::getCount()
{
    unsigned int iCnt = 0;

    if( (*m_piUsedSE) > (*m_piDeledSE) ){
        iCnt = ( (*m_piUsedSE) - (*m_piDeledSE) -1 );
    }else{
        iCnt = (*m_piTotalSE)- 1 -  (  (*m_piDeledSE) - (*m_piUsedSE) );    
    }

    return iCnt;
}


template <class T>
unsigned int SHMData_A<T>::getTotal()
{
    if (*m_piTotalSE)
        return (*m_piTotalSE)-1;

    return 0;   
}

///需要在程序外面防止同一个uiOffset调用revoke两次，一旦调用两次就会乱掉内存了
template <class T>
bool SHMData_A<T>::revoke( unsigned int uiOffset)
{
    if(!m_poSHM)
        return false;
        
    if( ((*m_piDeledSE)+1 )%(*m_piTotalSE) == (*m_piUsedSE) )
        return false;
    
    T oTemp;
    memset(&oTemp, 0, sizeof(T) );
    if(memcmp(& m_pPointer[uiOffset], &oTemp, sizeof(T)) ==0){
        ////可能是重复删除，除非是一个数据项分配了但是没进行任何赋值马上就被删除
        return false;
    }
    
    m_pIArray[(*m_piDeledSE)] = uiOffset;
    memset( & m_pPointer[uiOffset], 0, sizeof(T) );

///不能直接先 ++再判断回0，防止此时正好在分配空间
    if( (*m_piDeledSE)+1 >= (*m_piTotalSE) ){
        (*m_piDeledSE) = 0;
        (*m_piDelOver)++;
    }else{
        (*m_piDeledSE)++;
    }
        
    return true;   

}

////获取自创建以来所删除的总条数
template <class T>
unsigned long SHMData_A<T>::getDelTotal( )
{
 
    
    if( sizeof(T) <32 )
        return ( (*m_piTotalSE) * (*m_piDelOver) + (*m_piDeledSE)  ) - 1;

    return ( (*m_piTotalSE) * (*m_piDelOver) + (*m_piDeledSE)  );
}


////获取 目前处于删除状态但还未被重新分配出去的条数
template <class T>
unsigned int SHMData_A<T>::getDelFree( )
{
    if( (*m_piDelOver) ){
        ////已经全部删除过，那么所有空闲状态的都是先前删除得到的位置
        return getTotal()-getCount();
    }else{
        if( (*m_piUsedSE) > (*m_piDeledSE) ){
            if( sizeof(T) <32 )
                return (*m_piDeledSE)-1;
            else
                return (*m_piDeledSE) ;
        }else{
            return (*m_piDeledSE)-(*m_piUsedSE);
        }
    }
    
    
}

////检查重复删除
////返回是否健康
template <class T>
bool SHMData_A<T>::checkHealth()
{
    unsigned char * pData = new unsigned char[(*m_piTotalSE)];
    memset(pData, '0', sizeof(char)*(*m_piTotalSE)  );
    unsigned int i = (* m_piUsedSE );
    unsigned int j =0;
    bool bRet = true;
    
    for(; i != (* m_piDeledSE ); i++){
        if( i>= (* m_piTotalSE ) ){
            i = 0;
        }
        j = m_pIArray[i];
        if( 0== j){
            j = i;
        }
        if( '0' == pData[j] ){
            ////此条记录空闲
            pData[j] = '1';
            
        }else{
            ////此条记录出现多次删除
            pData[j]++;
            bRet = false;
            cout<< pData[j]<<"次删除记录:"<< j<<",";
        }
        cout<<endl;
    }
    delete []pData;

    if( 0== m_pIArray[(* m_piTotalSE)]  ){
        ////还没有删除满过
        if((*m_piDelOver)!=0 ){
            bRet = false;
            cout<<"删除记录的内存校验1失败.";
        }
    }
    
    if( 0!= m_pIArray[(* m_piTotalSE)]  ){
        ////已经删除满过
        if( 0 == (*m_piDelOver) ){
            bRet = false;
            cout<<"删除记录的内存校验2失败.";
        }
        unsigned int i = (* m_piUsedSE );
        if( i!=0  && 0== m_pIArray[i] ){
            bRet = false;
            cout<<"删除记录的内存校验3失败.";
        }
        ////如果删除满过，就必须分配满过
        ////以上判断基于一个数据项只有先分配了他才可能存在删除他

    }

    return bRet;
    
}

template <class T>
void SHMData_A<T>::showDetail()
{
    if(!m_poSHM)
        return ;
//    cout<<"内存模式："<<  (*m_piVersion)<<", 单条结构体大小："<< sizeof(T) << endl;
//    cout<<"内存容量："<<  getTotal()  <<", 已使用条数："<< getCount() << endl;
//    cout<<"自创建以来删除的总条数："<< getDelTotal() << endl;
//    cout<<"空闲可用数量："<< getTotal()-getCount() << ", 其中来自删除的数量为："<< getDelFree() << endl;

    char sText[256];
    char keys[64];
    if(m_sSHMName) {
        strncpy(keys, m_sSHMName, sizeof(keys));
    }else{
        sprintf(keys, "%ld", m_lSHMKey);
    }
    if( !m_poSHM ){
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "DATA_A", sizeof(T), 0, 0, 0 );
    }else{
    char * pp = (char * )(*m_poSHM);
    unsigned int uiTotal = getTotal();
    unsigned int uiCount = getCount();
    int percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4u%% %-9u %-10lu",
        keys, pp,(*m_piVersion), "DATA_A", sizeof(T), uiTotal, uiCount, percent,
        getDelFree(), getDelTotal() );
    }
    cout<< sText<<endl;


}


////如果更新掉了第1个节点，则更新输出o_uiKickSelf
////返回脱链的个数
template <class T>
unsigned int SHMData_A<T>::kick(unsigned int uiBeginOff, 
    int iNxtOffset, int iExpOffset, const char * sDate, unsigned int * o_uiKickSelf, bool * o_bKickSelf )
{
    if(!sDate || iNxtOffset<0 || iExpOffset<0 )
        return 0 ;

    unsigned int uiDataOff = uiBeginOff;
    unsigned int iCnt=0;
    *o_bKickSelf=false;
    if( uiDataOff ){
        unsigned int uiDataNext = *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
        unsigned int uiSelf= uiDataOff;
        
        while(uiDataNext){
            char * pExp = ( ( (char *)&m_pPointer[uiDataNext] ) + iExpOffset);
            if( strncmp(pExp, sDate, 8)<0 ){
                ///过期的数据
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset) =
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataNext])+ iNxtOffset);
                
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataNext])+ iNxtOffset)=0;
                iCnt++;
                uiDataNext=*(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
                continue;
            }
            uiDataOff = uiDataNext;
            uiDataNext=*(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
        }
        char * pExp = ( ( (char *)&m_pPointer[uiSelf] ) + iExpOffset);
        if( strncmp(pExp, sDate, 8)<0 ){
            *o_uiKickSelf =
            *(unsigned int *)( ( (char *)&m_pPointer[uiSelf])+ iNxtOffset);
            
            *(unsigned int *)( ( (char *)&m_pPointer[uiSelf])+ iNxtOffset)=0;
            iCnt++;
            *o_bKickSelf = true;
            
        }
        
    }
    return iCnt;
}


////如果更新掉了第1个节点，则更新输出o_uiKickSelf
////返回删掉的个数
template <class T>
unsigned int SHMData_A<T>::kickAndRevoke(unsigned int uiBeginOff, 
    int iNxtOffset, int iExpOffset, const char * sDate,
    int iChkOffset1, int iChkOffset2 , unsigned int  *o_uiKickSelf, bool * o_bKickSelf)
{
    if(!sDate || iNxtOffset<0 || iExpOffset<0 )
        return 0 ;

    unsigned int uiDataOff = uiBeginOff;
    unsigned int iCnt=0;
    *o_bKickSelf=false;
    
    if( uiDataOff ){
        unsigned int uiDataNext = *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
        unsigned int uiSelf= uiDataOff;
        
        while(uiDataNext){
            char * pExp = ( ( (char *)&m_pPointer[uiDataNext] ) + iExpOffset);
            if( strncmp(pExp, sDate, 8)<0 
                && ( iChkOffset1<0 
                    || 0 == *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iChkOffset1) )
                && ( iChkOffset2<0 
                    || 0 == *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iChkOffset2) )
                ){
                ///过期的数据且check的数据已经清空
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset) =
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataNext])+ iNxtOffset);
                
                *(unsigned int *)( ( (char *)&m_pPointer[uiDataNext])+ iNxtOffset)=0;
                revoke(uiDataNext);
                iCnt++;
                uiDataNext=*(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
                continue;
            }
            uiDataOff = uiDataNext;
            uiDataNext=*(unsigned int *)( ( (char *)&m_pPointer[uiDataOff] )+ iNxtOffset);
        }
        
        char * pExp = ( ( (char *)&m_pPointer[uiSelf] ) + iExpOffset);
        if( strncmp(pExp, sDate, 8)<0 
                && ( iChkOffset1<0 
                    || 0 == *(unsigned int *)( ( (char *)&m_pPointer[uiSelf] )+ iChkOffset1) )
                && ( iChkOffset2<0 
                    || 0 == *(unsigned int *)( ( (char *)&m_pPointer[uiSelf] )+ iChkOffset2) )
            ){
            *o_uiKickSelf =
            *(unsigned int *)( ( (char *)&m_pPointer[uiSelf])+ iNxtOffset);
            
            *(unsigned int *)( ( (char *)&m_pPointer[uiSelf])+ iNxtOffset)=0;
            iCnt++;
            revoke(uiSelf);
            *o_bKickSelf = true;
            
        }
    }
    return iCnt;
}
template <class T>
unsigned int SHMData_A<T>::getMemSize()
{
	unsigned int  size = 0;
	if (*m_piTotalSE)
	{
		size = (*m_piTotalSE)* sizeof(T);
	}
	if (size)
		return size ;
	
	return 0;
}
template <class T>
unsigned int SHMData_A<T>::getUsedSize()
{
	unsigned int size = 0;
	
	if (*m_piUsedSE)
	{
		 size = (*m_piUsedSE)* sizeof(T);
	}
	if (size)
		return size ;
	
	return 0;
}

template <class T>
unsigned long SHMData_A<T>::getMemAdress()
{		
	if (m_poSHM)
	{
		return   (unsigned long)m_poSHM;
	}
	
	return 0;
}

template <class T>
unsigned long SHMData_A<T>::getRealSize()
{
	  unsigned long size = m_poSHM->size();
		 return size;

}

template <class T>
time_t  SHMData_A<T>::getLoadTime()
{
	time_t shm_ctime;
	
	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();	
		
		return shm_ctime;
	}
	return  0;
	
}


//////返回基本信息
//template <class T>
//int SHMData_A<T>::getInfo(char * sOut, int iMax )
//{
//    char keys[64];
//    if(m_sSHMName) {
//        strncpy(keys, m_sSHMName, sizeof(name));
//    }else{
//        sprintf(keys, "%ld", m_lSHMKey);
//    }
//    int i = snprintf(sOut, iMax, "%10s 0x%10x %12s %9d %10u %10u %4d%%",
//        keys, (*m_poSHM), "DATA_A", sizeof(T), getTotal(), getCount(), 100L*getCount()/getTotal() );
//
//    return i;
//}

#endif
