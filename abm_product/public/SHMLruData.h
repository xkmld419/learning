/*VER: 1  专门给集团修改的版本*/
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef SHMLRUDATA_H_HEADER_INCLUDED
#define SHMLRUDATA_H_HEADER_INCLUDED

#include "HashList.h"
#include "SHMAccess.h"
#include "CSemaphore.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
 
/* =======================
ShmLruData 类：
提供共享内存访问的LRU功能，
提供按条删除功能，
提供提交、回滚功能，
提供多session的支持，
支持动态扩展(传入的key其后面9个key都用来进行扩展，默认可以使用，因此key在外部需要规划，目前规定需要0结尾)

访问时直接使用下标操作符进行访问表示是修改，需要及时commit()
外部如果是调用修改，则在修改前需要先进行上锁操作，否则，试图修改其他session未提交的数据时抛异常。
外部调用时，需要在try块内截获异常，处理失败。

使用方式：
    SHMLruData<AcctItemAddAggrData> *  pLruAggr2 
      = new SHMLruData<AcctItemAddAggrData>(100057804L, 
        "/cbilling/cbill_test/pub/tibs_bill_c/FileTest/MFile.bin" , 1,
        67108864, 8 ) ;
参数说明： 
long shmkey,   
const char * sFileName, ----
int iSession, 
long extSize, 
int iMaxExtCnt

其中iMaxExtCnt最多8 

自动多文件方式：
sFileName是总控文件，记录整个LRU需要落地的控制数据，程序会自动在所在目录按序号建立文件，
单个文件控制在512M左右


创建者先调用 create(total_cnt) 创建；
使用者调用 malloc() 分配得到下标，如果数据准备好，
【如果数据在分配之前能准备好，使用带参数的malloc速度会快一点点,带参数的malloc将直接赋值。】
然后使用下标操作符进行读取操作，或者使用getForWrite getForRead 分别写操作和读操作

使用者调用 revoke(index) 删除分配到的下标
使用者调用commit()确认修改，使用rollback回滚修改

使用者不能删除未分配过的下标，不能对已经删除的下标执行多次删除，否则将抛异常。
删除其他session还未提交的下标将引发抛异常，因此需要处理异常

异常编号如下：
MBC_SHMLruData
MBC_SHMLruData+1 : 共享内存结构不符--请检查创建程序和连接使用的程序是否同样版本
MBC_SHMLruData+2 : 访问下标越界--
MBC_SHMLruData+3 : 试图修改其他session正在修改的数据--
MBC_SHMLruData+4 : 回滚空间不足--
MBC_SHMLruData+5 : 所有内存都在进行修改，没有可供换出的空间--
MBC_SHMLruData+6 : 访问的下标已经被删除--
MBC_SHMLruData+7 : 对应的磁盘文件不能打开--
MBC_SHMLruData+8 : 对应的磁盘文件数据损坏--
MBC_SHMLruData+9 : 对应的磁盘文件写入数据出错--
MBC_SHMLruData+10 : 不能操作其他session--
MBC_SHMLruData+11 : session错误
MBC_SHMLruData+12 : 内存不足
MBC_SHMLruData+13 : 内存存在错误

zhangap
2010-5-5
======================= */


const int SHM_LRU_FILENAME_LEN=128;
const int SHM_LRU_MAX_SESSION =1024; 

#define SHM_LRU_CURR_VERSION   12

////脏数据超过40%才写文件
#define  SHM_LRU_DIRTY_HIGH_LEVEL   (*puiFreshHighPct)
#define  SHM_LRU_DIRTY_LOW_LEVEL    (*puiFreshLowPct)
#define  SHM_LRU_DIRTY_CLOCKCNT     (*puiFreshInterval)

const char SHM_LRU_FLAG_NORM = 0;   ////正常未曾修改
const char SHM_LRU_FLAG_MODIFY = 1u;   ////曾修改过
const char SHM_LRU_FLAG_FORDEL = 2u;  ////打了删除标记
const char SHM_LRU_FLAG_DEL = 3u;  ////已经被删除
const char SHM_LRU_FLAG_CANSEE = 0x0FE; ///能作为正常数据被查询到的分界点，小于它，能查询到；若大于则不能
const char SHM_LRU_FLAG_LOG = 0x40;  ////已作为Log区使用


const char sTestLruFile[]="/opt/bill/app/lru_data/lrutest.bin";
const long lTestLruKey=8100099L;

const char sTestLruFile_2[]="/opt/bill/app/lru_data/lrutest.2.bin";
const long lTestLruKey_2=2009162201L;


const int SHM_LRU_BATCH_SIZE = 67108864;
////268435456; //256M
////   536870912; ////512M 文件大小。
    

#define ATTACH_DATA_LRU(X, Y, Z, J,K,M,L ) \
        X = new SHMLruData<Y> (Z,J,K,M,L ); \
        if (!(X)) { \
            freeAll(); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }




class FileLruHead
{
    public:
        unsigned int uiHashValue;
        unsigned int uiIterCnt;
        unsigned int uiRealCnt;
        unsigned int uiMallocCnt;
        unsigned int uiExpandCnt;
        unsigned int uiVer;
        long lKey;
        unsigned int uiState;     ////写文件的状态0正常，1正在写
        unsigned int uiDelete;
        unsigned int uiContentSize;
        char sName[8];
};

template <class T1>
class FileLruData
{
    public:
      unsigned int uiNext;
      T1   oData;
};


template <class T>
class SHMLruData : public SHMAccess
{
  public:
//    class LruUnitDataSimple{
//    public:
//        LruUnitData oSimple;
//    };
//    
    class LruUnitData
    {
    public:
        unsigned int uiListPre; 
        unsigned int uiListNext;
        unsigned int uiListLog; ////正常块时，存放备份的log块；log块时，存放sessionID;
                
        unsigned int uiIndex;         ////索引
        unsigned int uiListIdxID;     ////所属索引id
        unsigned int uiListIdxNext;   ////索引的next

        unsigned char sFlag[1]; ////0表示正常数据 其他见SHM_LRU_FLAG_xxx的描述
        T   oListData;
    };

    class LruExpand{
    public:
        LruExpand( ){m_pLruData = 0;};
        LruUnitData &  operator [](unsigned int idx){
            LruUnitData *pBase = (LruUnitData *)(m_pLruData->m_sSegment[0]);

//            if( idx == 467283 )
//            printf("iSegment:  %lx:%u,%lx:%u,%lx:%u\n", 
//                m_pLruData->m_sSegment[0],*(m_pLruData->m_piSeg),
//                m_pLruData->m_sSegment[1],*(m_pLruData->m_piSeg+1) ,
//                m_pLruData->m_sSegment[2],*(m_pLruData->m_piSeg+2) );
                
            if( idx <= (*m_pLruData->m_piSeg)  ){
                return  *(pBase+idx) ;
            }

            
            for(int i=1; i<MAX_SHM_EX_CNT+1;i++){
                if( idx <= (*(m_pLruData->m_piSeg+i)) ){
                    pBase = (LruUnitData *)(m_pLruData->m_sSegment[i]);
                    idx -= (*(m_pLruData->m_piSeg+i-1)) ;
                    return  *(pBase+idx-1) ;
                }

            }
            THROW(MBC_SHMLruData+2);
        };

        SHMLruData * m_pLruData;

    };
    
    friend class LruExpand;
    
  public:
    static long getNeedSize(unsigned int itemnumber );

  public:

    SHMLruData(char *shmname, const char * sFileName, int iSession, long extSize, int iMaxExtCnt);
    SHMLruData(long shmkey,   const char * sFileName, int iSession, long extSize, int iMaxExtCnt);
    ~SHMLruData();
    
    T &  operator [](unsigned int indexBig);
    const T& operator [](unsigned int indexBig) const ;

    bool getForWrite(unsigned int indexBig, T& Tin);
    bool getForRead(unsigned int indexBig, T& Tout) ;
    T&   getForRead(unsigned int indexBig ) ;
    T&   getForWrite(unsigned int indexBig ) ;
        
    bool getForMemRead(unsigned int indexBig, T & Tout) ;
    
    unsigned int getDirty(int i ){ return *(puiDirty+i); };
    void create(unsigned int itemnumber) ;

    unsigned int malloc();
    unsigned int malloc(T & indata );
    
    ///数据个数
    unsigned int getCount(){ 
        if(puiRealCnt) return (*puiRealCnt);
        else return 0; };
        
    ///物理内存可容纳总量
    unsigned int getTotal(){
        if(puiIterCnt) return (*puiIterCnt);
        else return 0; };
    
    ///物理内存中的总量,包括Log占用的部分数据区的量
    unsigned int getMemCount(){
        if(puiFreeCnt && puiIterCnt ) return (*puiIterCnt)-(*puiFreeCnt);
        else return 0; };

    ///物理内存中的数据总量,去掉Log占用的部分，用于计算脏数据百分比
    unsigned int getMemDataCount(){
        if((*puiLogCnt)>(*puiLogTotalCnt) ) return (*puiIterCnt)-(*puiFreeCnt)+(*puiLogTotalCnt)-(*puiLogCnt);
        else return (*puiIterCnt)-(*puiFreeCnt); };
    
    ///脏数据总量
    unsigned int getDirtyCount(){
        if( puiDirtyCnt ) return (*puiDirtyCnt);
        else return 0; };

    ///脏数据百分比
    unsigned int getDirtyPercent(){
        if( getTotal() ) return 100*getDirtyCount()/getTotal();
        else return 0;
        };

    ///脏数据已提交量
    unsigned int getCommitDirtyCount(){
        return getDirtyCount()-(*puiLogCnt);
        };
    ///日志使用百分比，注意：可超过100%
    unsigned int getLogPercent(){
        if( *puiLogTotalCnt ) return 100*(*puiLogCnt)/(*puiLogTotalCnt);
        else return 0; };

    ///访问命中率
    unsigned int getHitrate(){
        if( (*puiAccessCnt) >= 100 ){
            (*puiHitrate) = 100*((*puiAccessCnt)-(*puiFailCnt))/(*puiAccessCnt);
            if( (*puiHitrate)>100){
                /////出现大于100 其实是负数
                (*puiHitrate)=0;
            }
            if((*puiAccessCnt) >= 1000){
                (*puiAccessCnt)=0;
                (*puiFailCnt)=0;
            }
        }
        return (*puiHitrate); 
    };

    unsigned int resetHitrateCalc(){
        (*puiAccessCnt) = 0;
        (*puiFailCnt) = 0;
        return (*puiHitrate); 
    };

    bool getState( unsigned int & uiState, long int & StateTime ){
        if(puiState ){
            uiState = *puiState;
            StateTime = *pulStateTime;
            return true;
        }
        return false; 
    };
    
    bool setState( unsigned int uiState ){
        if( puiState ){
            *puiState = uiState ;
            setStateTime();
            return true;
        }
        return false; 
    };    

    bool setStateTime( ){
        if( pulStateTime ){
        	struct timeval tv;
        	struct timezone tz;
            if(0 == gettimeofday (&tv , &tz) ){
                *pulStateTime = tv.tv_sec;
            }
            return true;
        }
        return false; 
    };   

    char * getBaseP(){ return (char *)(*m_poSHM);  };

    CSemaphore * getDataLock(){    return &m_oDataLock; };
    
    void setNextSize(long lNextSize ){SHMAccess::m_lNextSize =lNextSize; };
    void setMaxExCnt( int iMaxExCnt ){SHMAccess::m_iMaxExCnt =iMaxExCnt; };

    void setExthwm( int iExthwm ){ if(puiExthwm){ *puiExthwm=iExthwm;} };
    unsigned int  getExthwm(  ){ 
        if(puiExthwm) return (*puiExthwm);
        else return 0;
         };
         
    void setFreshHighPct(int ii){  if(puiFreshHighPct){(*puiFreshHighPct) = ii;} };
    unsigned int  getFreshHighPct(){ 
        if(puiFreshHighPct) return (*puiFreshHighPct);
        else return 0;
         };

    void setFreshLowPct(int ii){  if(puiFreshLowPct){(*puiFreshLowPct) = ii;} };
    unsigned int  getFreshLowPct(){ 
        if(puiFreshLowPct) return (*puiFreshLowPct);
        else return 0;
         };

    void setFreshInt(int ii){  if(puiFreshInterval){(*puiFreshInterval) = ii;} };
    unsigned int  getFreshInt(){ 
        if(puiFreshInterval) return (*puiFreshInterval);
        else return 0;
         };

         
    void setWrnhwm( int ii ){ if(puiWrnhwm){ *puiWrnhwm=ii;} };
    unsigned int  getWrnhwm( ){ 
        if(puiWrnhwm) return (*puiWrnhwm);
        else return 0;
         };

    void setUpperLength( int ii ){ if(puiUpperLength){ *puiUpperLength=ii;} };
    unsigned int  getUpperLength( ){ 
        if(puiUpperLength) return (*puiUpperLength);
        else return 0;
         };

    unsigned int  getLruLength( ){ 
        if(getTotal()>(*puiInitLength)) return getTotal();
        else return (*puiInitLength) ;
         };

    void setCommitDownFile( int ii ){ if(puiCommitDownFile){ *puiCommitDownFile=ii;} };
    unsigned int  getCommitDownFile( ){ 
        if(puiCommitDownFile) return (*puiCommitDownFile);
        else return 0;
         };

    unsigned int  getExpandCnt( ){ 
        if( puiExpandCnt ) return (*puiExpandCnt);
        else return 0;
         };
///正在落地文件的话，这个标记置1，否则置0
    void setWriteFileFlag( int ii ){ if(puiWriteFileFlag){ *puiWriteFileFlag=ii;} };
    unsigned int  getWriteFileFlag( ){ 
        if(puiWriteFileFlag) return (*puiWriteFileFlag);
        else return 0;
         };
	
    void setExpandMaxSize( int ii ){ if(puiExpandMaxSize){ *puiExpandMaxSize=ii;} };
    unsigned int  getExpandMaxSize( ){ 
        if(puiExpandMaxSize) return (*puiExpandMaxSize);
        else return 0;
         };


    void reset();
    bool revoke( unsigned int uiOffset);
    void showDetail();
    void showNormal( bool bDetail = false );

    unsigned int commit();
    unsigned int rollback();
    void commitSession(unsigned int iSession);
    void rollbackSession (unsigned int iSession);

    bool getByFile( unsigned int iOffset, T& outT );    
    unsigned int  writeToFile(bool bForce = false );
    unsigned int  recoverFromFile( );
    void truncFile();
    
    unsigned int expandMem(bool bTreatFile = true );
        
  private:
    void init();
    void initFile(bool bTrunc );
    void open();
    unsigned int mallocLog();
    unsigned int mallocFree();
    unsigned int doPageIn(unsigned int index, bool ifModify );
    unsigned int doPageOut(unsigned int index );
    unsigned int doDirtyOut(unsigned int index);
    unsigned int doListOut(unsigned int indexReal, unsigned int * pHead, unsigned int *pCnt );
    unsigned int doListMoveFirst(unsigned int indexReal, unsigned int * pHead);
    void doListIn(unsigned int indexReal, unsigned int * pHead, unsigned int *pCnt );
    
    void addToNormal( unsigned int index );
    void addToDirty( unsigned int index , int i_iSession );
    void addToFree( unsigned int index );
    void addToFreeLog( unsigned int index );
    
    void addToDelIndex( unsigned int indexBig, unsigned int index);
    unsigned int addToIndex(unsigned int indexBig, unsigned int index );
    unsigned int delFromIndex(unsigned int indexBig, int iFlag );


    unsigned int getLruDataForMalloc(unsigned int indexBig) ;
    unsigned int getLruDataForWrite(unsigned int indexBig, bool bFreshNew = false );
    unsigned int getLruDataForRead(unsigned int indexBig, FileLruData<T> * pInitData = 0);

    unsigned int moveIndex(unsigned int indexBig, unsigned int uiNewHash);
//    unsigned int writeFileAll( FILE * fp );
    unsigned int writeFileBatch( FILE * fpHead, bool bAll );
    unsigned int writeFileHead( FILE * fpHead, FileLruHead & oHead, unsigned int uiState );
    unsigned int expandFile(FILE *fp);
    FILE * prepareFile( int iTimes, bool bTrunc, bool bCreate=true);
  private:
///操作共享内存
     unsigned int * puiFree;
     unsigned int * puiNorm;
     unsigned int * puiLog;

     unsigned int * puiDirty;////[SHM_LRU_MAX_SESSION];
     unsigned int * puiDelete;  ///Delete 链是单向链表，记录的int是对外的下标

     unsigned int * puiFreeCnt;
     unsigned int * puiNormCnt;
     unsigned int * puiDirtyCnt;
     unsigned int * puiLogCnt;
     unsigned int * puiLogTotalCnt;
     unsigned int * puiLogBegin; ////Log区分配开始的地方
     unsigned int * puiHashValue; /////索引hash值， 如果内存不扩展，则(*puiHashValue)等于(*puiIterCnt)
    
     unsigned int * puiIterCnt;
     unsigned int * puiRealCnt;
     unsigned int * puiMallocCnt;
    //unsigned int * puiUsedCnt; -- 可以通过 NormCnt+DirtyCnt+LogCnt得到 或者 IterCnt-FreeCnt
     unsigned int * puiExpandCnt;
    
    unsigned int *puiVer;
    unsigned int *puiContentSize;
     unsigned int *puiSessionCnt;
   
     unsigned int *puiAccessCnt;   ///访问次数
     unsigned int *puiFailCnt;     ///未命中次数
     unsigned int *puiHitrate;    ///命中率

     unsigned int *puiState;     ////记录状态用,可用于checkpoint
     long   *pulStateTime;     ////state_date 
     unsigned int *puiSegment;
    
     unsigned int *puiExthwm;
     unsigned int *puiFreshHighPct;
     unsigned int *puiFreshLowPct;
     unsigned int *puiFreshInterval;
     unsigned int *puiWrnhwm;
     unsigned int *puiInitLength;
     unsigned int *puiUpperLength;
     unsigned int *puiCommitDownFile;
	 unsigned int *puiWriteFileFlag;
    unsigned int * puiExpandMaxSize;
  private:
///操作私有内存
    char m_sFileName[SHM_LRU_FILENAME_LEN];
    char m_sFileExName[SHM_LRU_FILENAME_LEN+8];

#ifdef SHM_EXPAND_GLOBAL_POS
    LruExpand  m_pPointer;
    LruUnitData *m_pLruUnit;
#else
    LruUnitData *m_pPointer;
#endif
    unsigned int m_iSessionID;
    unsigned int m_iLastTotal; 
    unsigned int m_iCntPerFile;/////一个文件容纳多少个项目
    
    CSemaphore m_oDataLock;
    unsigned int m_iLastAccess;
    long m_lClock;
    T m_oDelUnit;
    T m_oReadUnit;
};


///////////////////////////////////////////////////////////////////////////////

////25%的log空间
template <class T>
long SHMLruData<T>::getNeedSize(unsigned int itemnumber ){
        return (long)sizeof(LruUnitData) * (itemnumber +  itemnumber/4 )  +
         (64+SHM_LRU_MAX_SESSION)*sizeof(unsigned int);
};


template <class T>
void SHMLruData<T>::init()
{
    m_lClock = 0;
    
    puiFree=0;
    puiNorm=0;
    puiLog=0;
    puiDirty=0;
    puiDelete=0;

    puiFreeCnt=0;
    puiNormCnt=0;
    puiDirtyCnt=0;
    puiLogCnt=0;
    puiLogTotalCnt=0;

	puiInitLength=0;
	puiUpperLength=0;
    puiCommitDownFile=0;
	puiWriteFileFlag=0;
    puiExpandMaxSize=0;
    
    puiLogBegin=0; 
    puiHashValue=0;
        
    puiIterCnt=0;
    puiRealCnt=0;
    puiMallocCnt=0;
    puiExpandCnt=0;
    
    puiVer=0;
    puiContentSize=0;
    puiSessionCnt=0;
    
    puiAccessCnt=0; 
    puiFailCnt=0;
    puiHitrate=0;

    puiState=0;
    pulStateTime=0;
    
    puiSegment = 0;
    
#ifdef SHM_EXPAND_GLOBAL_POS
    m_pPointer.m_pLruData = this;
    m_pLruUnit = 0;
#else
    m_pPointer=0;
#endif

    m_iSessionID=0;
    m_iLastTotal = 0;
    
    m_iCntPerFile = SHM_LRU_BATCH_SIZE/sizeof( FileLruData<T> );
    m_iCntPerFile = (m_iCntPerFile/8)*8;
    
    memset(m_sFileName, 0 , sizeof(m_sFileName) );
    memset(m_sFileExName, 0 , sizeof(m_sFileExName) );    
    memset(&m_oDelUnit, 0, sizeof(m_oDelUnit) );

    m_iLastAccess=0;
}


template <class T>
void SHMLruData<T>::initFile(bool bTrunc )
{
    FILE *fp;
    if( !m_sFileName[0] ) 
        return ;
        
    if(bTrunc){
        if( (fp = fopen( m_sFileName,  "w" )) == NULL ){
            THROW(MBC_SHMLruData+7);
        }
    }else{
        if( (fp = fopen( m_sFileName,  "ab+" )) == NULL ){
            THROW(MBC_SHMLruData+7);
        }
    }
    fclose (fp);
    fp = 0;
    
    if( SHMAccess::exist() 
        && (*puiContentSize)==sizeof(T) ){ 
        if ((fp = fopen (m_sFileName, "rb+")) != NULL) {
            fseek(fp, 0,  SEEK_END);
            if( ftell(fp) < sizeof(FileLruHead) ) {
                FileLruHead oHead;
                if(0 == writeFileHead( fp, oHead, 0) ){
                    fclose (fp);
                    THROW(MBC_SHMLruData+9);
                }
            }

            fclose (fp);
            return;
        }
        THROW(MBC_SHMLruData+7);
    }
    
}


template <class T>
SHMLruData<T>::SHMLruData(char * shmname, const char * sFileName, int iSession, 
     long extSize, int iMaxExtCnt):
    SHMAccess (shmname, extSize, iMaxExtCnt )
{
    init();

    strncpy( m_sFileName , sFileName, sizeof(m_sFileName)-1 );
    m_iSessionID = iSession%SHM_LRU_MAX_SESSION;

    char sTemp[64];
    int  iTemp = strlen(shmname)>9 ? (strlen(shmname)-9 ) : 0;
    snprintf(sTemp, sizeof(sTemp), "%u", atoi(&shmname[iTemp])+9);
    m_oDataLock.getSem (sTemp, 1, 1);
    
        
    if (m_poSHM){
        open ();
        if( SHM_LRU_CURR_VERSION != *puiVer )
            THROW(MBC_SHMLruData+1);
            
    }

    initFile(false);
}


template <class T>
SHMLruData<T>::SHMLruData(long shmkey, const char * sFileName, int iSession, 
      long extSize, int iMaxExtCnt):
    SHMAccess (shmkey, extSize, iMaxExtCnt )
{
    init();

    strncpy( m_sFileName , sFileName, sizeof(m_sFileName)-1 );
    m_iSessionID = iSession%SHM_LRU_MAX_SESSION;

    char sTemp[64];
    snprintf(sTemp, sizeof(sTemp), "%ld", shmkey+9 );
    m_oDataLock.getSem (sTemp, 1, 1);    

    if (m_poSHM){
        open ();
        if( SHM_LRU_CURR_VERSION != *puiVer )
            THROW(MBC_SHMLruData+1);
    
    }


    
    initFile(false);
    
}

template <class T>
SHMLruData<T>::~SHMLruData()
{

}



// 非常量版本,
template <class T>
T & SHMLruData<T>::operator [](unsigned int indexBig){
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    while(!m_oDataLock.P());
try{
    unsigned int indexReal = getLruDataForWrite(indexBig);
    if(indexReal ){
        ////m_pPointer[indexReal].oListData;
        m_oDataLock.V();
        return m_pPointer[indexReal].oListData;
    }
    m_oDataLock.V();
    memset(&m_oReadUnit, 0,sizeof(m_oReadUnit) );
    return m_oReadUnit;

}catch(...){
    m_oDataLock.V();
    throw;
}

}

// 常量版本,对于自身session，返回的是修改的那块，对于非自身session，返回的是备份块
template <class T>
const T& SHMLruData<T>::operator [](unsigned int indexBig) const {
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    m_oDataLock.P();
try{
    unsigned int indexReal = getLruDataForRead(indexBig);
    if(indexReal ){
        m_oReadUnit=m_pPointer[indexReal].oListData;
        m_oDataLock.V();
        return m_oReadUnit;
    }
    m_oDataLock.V();
    memset(&m_oReadUnit, 0,sizeof(m_oReadUnit) );
    return m_oReadUnit;
}catch(...){
    m_oDataLock.V();
    throw;
}

}

template <class T>
bool SHMLruData<T>::getForWrite(unsigned int indexBig, T& Tin){
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    while(!m_oDataLock.P());
try{
    unsigned int indexReal = getLruDataForWrite(indexBig);
    if(indexReal){
        m_pPointer[indexReal].oListData = Tin;
        m_oDataLock.V();
        return true;
    }
    m_oDataLock.V();
    return false;
}catch(...){
    m_oDataLock.V();
    throw;
}


}


template <class T>
T& SHMLruData<T>::getForWrite(unsigned int indexBig){
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    while(!m_oDataLock.P());
try{
    unsigned int indexReal = getLruDataForWrite(indexBig);
    if(indexReal){
        m_oDataLock.V();
        return m_pPointer[indexReal].oListData;
    }
    m_oDataLock.V();
    THROW(MBC_SHMLruData+13);
}catch(...){
    m_oDataLock.V();
    throw;
}


}

template <class T>
bool SHMLruData<T>::getForRead(unsigned int indexBig, T& Tout) {


    while(!m_oDataLock.P());
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }


try{
    unsigned int indexReal = getLruDataForRead(indexBig);
    if(indexReal){
        Tout=m_pPointer[indexReal].oListData;
        m_oDataLock.V();
        return true;
    }
    m_oDataLock.V();
    return false;
}catch(...){
    m_oDataLock.V();
    throw;
}


}


template <class T>
T& SHMLruData<T>::getForRead(unsigned int indexBig ) {
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    while(!m_oDataLock.P());
try{
    unsigned int indexReal = getLruDataForRead(indexBig);
    if(indexReal ){
        m_oReadUnit=m_pPointer[indexReal].oListData;
        m_oDataLock.V();
        return m_oReadUnit;
    }
    m_oDataLock.V();
    memset(&m_oReadUnit, 0,sizeof(m_oReadUnit) );
    return m_oReadUnit;
}catch(...){
    m_oDataLock.V();
    throw;
}


}





template <class T>
bool SHMLruData<T>::getForMemRead(unsigned int indexBig, T & Tout) {


    while(!m_oDataLock.P());
try{
    
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);

    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }

///    (*puiAccessCnt)++;
    unsigned int indexReal = m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex;
    while(indexReal){
        if(indexBig == m_pPointer[indexReal].uiListIdxID ) {
            if( 0 == m_iSessionID ){
                Tout = m_pPointer[indexReal].oListData;
                m_oDataLock.V();
                return true;
            }

            if(SHM_LRU_FLAG_DEL ==  m_pPointer[indexReal].sFlag[0]  ){
                /////已经删除的不能被查到
                THROW(MBC_SHMLruData+6);
                
            }
            unsigned int indexReal = getLruDataForRead(indexBig);
            Tout = m_pPointer[indexReal].oListData;
            m_oDataLock.V();
            return true;            
        }
        indexReal = m_pPointer[indexReal].uiListIdxNext ;
    }
    
    ////
    
}catch(...){
    m_oDataLock.V();
    throw;
}

    m_oDataLock.V();
    return false;

}



/////删除的被重新释放出来时调用这个函数 这个函数只做从DEL状态转换到 MPDIFY 状态 
template <class T>
unsigned int SHMLruData<T>::getLruDataForMalloc(unsigned int indexBig) 
{        
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);
    
    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }
      	
    if(m_iLastAccess!=indexBig){
        (*puiAccessCnt)++;
        m_iLastAccess=indexBig;
    }
    unsigned int indexReal = (indexBig-1)%(*puiHashValue)+1 ;
    indexReal = m_pPointer[indexReal].uiIndex;
    while(indexReal){
        if( indexBig == m_pPointer[indexReal].uiListIdxID ) {
            ////删除的数据可以不落地，此时在脏数据链上面
            ////THROW(MBC_SHMLruData+2);
            
            ////分配一个log块
            unsigned int uitemp = mallocLog();
            if(0==uitemp){
                ////没有log块分配给他，
                THROW(MBC_SHMLruData+4);
            }

            ///从0号dirty放到dirty的session链上面去
            doListOut(indexReal, puiDirty+0, puiDirtyCnt);
            addToDirty(indexReal, m_iSessionID);

            ////备份flag
            m_pPointer[uitemp].sFlag[0] = m_pPointer[indexReal].sFlag[0];
            m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_MODIFY;

            ////mallocLog 出来的空间，不能进行memset，否则内存全乱
            ////m_pPointer[uitemp].sFlag[0] = SHM_LRU_FLAG_LOG;
            m_pPointer[uitemp].oListData = m_pPointer[indexReal].oListData;
            m_pPointer[uitemp].uiListLog = m_iSessionID;
            
            m_pPointer[indexReal].uiListLog = uitemp;
            
            return indexReal;

        }
        indexReal = m_pPointer[indexReal].uiListIdxNext ;
    }
        
    (*puiFailCnt)++;


    indexReal = doPageIn(indexBig, true );

    if( indexReal ){
        if(SHM_LRU_FLAG_DEL != m_pPointer[indexReal].sFlag[0] ){
            /////page in 的应该是已经删除的数据才对，
            ////既然不是del，索引已经加上了，flag为NORMAL, 链在session上面了
            doListOut(indexReal, puiDirty+m_iSessionID, puiDirtyCnt);
            addToNormal(indexReal);

            THROW(MBC_SHMLruData+8);
        }
        ////分配一个log块
        unsigned int uitemp = mallocLog();
        if(0==uitemp){
            ////目前pagein的数据项已经挂在dirty链上面，但是没有log块分配给他，
            ///删除的数据，没有索引，因此将它移到到Free
            doListOut(indexReal, puiDirty+m_iSessionID, puiDirtyCnt);
            addToFree(indexReal);
            addToIndex(indexBig, indexReal);
            THROW(MBC_SHMLruData+4);
        }
        ////备份flag
        m_pPointer[uitemp].sFlag[0] = m_pPointer[indexReal].sFlag[0];

        m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_MODIFY;

        ////mallocLog 出来的空间，不能进行memset，否则内存全乱
        ////m_pPointer[uitemp].sFlag[0] = SHM_LRU_FLAG_LOG;
        m_pPointer[uitemp].oListData = m_pPointer[indexReal].oListData;
        m_pPointer[uitemp].uiListLog = m_iSessionID;
        
        m_pPointer[indexReal].uiListLog = uitemp;
        
        addToIndex(indexBig, indexReal);

        
        return indexReal;
    }
    
    THROW(MBC_SHMLruData+5);


}


template <class T>
unsigned int SHMLruData<T>::getLruDataForWrite(unsigned int indexBig, bool bFreshNew) 
{        
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);
    
    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }
      	
    if(m_iLastAccess!=indexBig){
        (*puiAccessCnt)++;
        m_iLastAccess=indexBig;
    }
    unsigned int indexReal = (indexBig-1)%(*puiHashValue)+1 ;
    indexReal = m_pPointer[indexReal].uiIndex;
    while(indexReal){
        if( indexBig == m_pPointer[indexReal].uiListIdxID ) {
            if( 0 == m_iSessionID )
                return indexReal;
            
            if( m_pPointer[indexReal].uiListLog ){
                
                if( m_iSessionID ==  m_pPointer[ m_pPointer[indexReal].uiListLog ].uiListLog  ){ 
                    ////如果做了删除操作，就不允许再修改
                    if( SHM_LRU_FLAG_FORDEL == m_pPointer[indexReal].sFlag[0] ){
                        THROW(MBC_SHMLruData+6);
                    }
                    return indexReal;
                }else
                    THROW(MBC_SHMLruData+3);
            }else{
    
                ////分配一个log数据块
                unsigned int uiLogtemp = mallocLog();
                if(0==uiLogtemp)
                    THROW(MBC_SHMLruData+4);

                ///移动链表
                ///脱链
                ////如果是链表头，则需要移动链表头;如果是链表尾，则要修改链表头
                if(SHM_LRU_FLAG_NORM == m_pPointer[indexReal].sFlag[0]){
                    doListOut(indexReal, puiNorm, puiNormCnt );
                }else if(SHM_LRU_FLAG_MODIFY == m_pPointer[indexReal].sFlag[0] ){
                    doListOut(indexReal, puiDirty+0, puiDirtyCnt);
                }else {
                    ////
                    if( uiLogtemp < (*puiLogBegin) 
                        || uiLogtemp >= (*puiLogBegin)+(*puiLogTotalCnt)  ){
                        ////额外分配的Log空间
                        addToFree(uiLogtemp);
                        (*puiLogCnt)--;
                    }else{
                        ////Log原本的空间
                        addToFreeLog(uiLogtemp);
                    }
                    
                    THROW(MBC_SHMLruData+6);
                }

                ///接链到session链上面去,作为脏数据
                addToDirty(indexReal, m_iSessionID);

                ////备份flag
                ////mallocLog 出来的空间，不能进行memset，否则内存全乱
                m_pPointer[uiLogtemp].sFlag[0] = m_pPointer[indexReal].sFlag[0];

                if(SHM_LRU_FLAG_NORM == m_pPointer[indexReal].sFlag[0] ){
                    m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_MODIFY;
                }
//                if( SHM_LRU_FLAG_DEL == m_pPointer[indexReal].sFlag[0] ){
//                    m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_MODIFY;
//
//                }

                m_pPointer[uiLogtemp].oListData = m_pPointer[indexReal].oListData;
                m_pPointer[uiLogtemp].uiListLog = m_iSessionID;
                
                m_pPointer[indexReal].uiListLog = uiLogtemp;
                return indexReal;
            }
        }
        indexReal = m_pPointer[indexReal].uiListIdxNext ;
    }
        


    if(bFreshNew){
        unsigned int iTempxx = mallocFree( );
        if(0 == iTempxx){
            iTempxx = doPageOut(0);
            if(0 == iTempxx){
                THROW(MBC_SHMLruData+5);
            }
        }

        m_pPointer[iTempxx].uiListLog  = 0;
        m_pPointer[iTempxx].oListData  = m_oDelUnit;
        m_pPointer[iTempxx].sFlag[0] = '\0';
        
        m_pPointer[iTempxx].uiListIdxID = indexBig;
        
        addToDirty(iTempxx, m_iSessionID);
        addToIndex(indexBig, iTempxx);
        indexReal = iTempxx;
    }else{

    
        (*puiFailCnt)++;        

        indexReal = doPageIn(indexBig, true );
        if(indexReal 
            && SHM_LRU_FLAG_DEL == m_pPointer[indexReal].sFlag[0] ){
            /////page in 的是已经删除的数据，必须分配了才可以进行写操作
            ////既然是del的数据，那么索引是没有的
            doListOut(indexReal, puiDirty+m_iSessionID, puiDirtyCnt);
            addToFree(indexReal);
            delFromIndex(indexBig, 0);
            m_pPointer[indexReal].oListData  = m_oDelUnit;
            m_pPointer[indexReal].sFlag[0] = '\0';
            
            THROW(MBC_SHMLruData+6);
        }

    }
    
    if( indexReal ){

        if( 0 == m_iSessionID ){
            return indexReal;
        }

        ////分配一个log块
        unsigned int uitemp = mallocLog();
        if(0==uitemp){
            ////目前pagein的数据项已经挂在dirty链上面，但是没有log块分配给他，
            ////不是删除的项目，index已经放好，因此需要将它移到到normal
            doListOut(indexReal, puiDirty+m_iSessionID, puiDirtyCnt);
            addToNormal(indexReal);

            THROW(MBC_SHMLruData+4);
        }
        ////备份flag
        m_pPointer[uitemp].sFlag[0] = m_pPointer[indexReal].sFlag[0];

        if(SHM_LRU_FLAG_NORM == m_pPointer[indexReal].sFlag[0] 
            || SHM_LRU_FLAG_DEL == m_pPointer[indexReal].sFlag[0] ){
            m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_MODIFY;
        }
        ////mallocLog 出来的空间，不能进行memset，否则内存全乱
        ////m_pPointer[uitemp].sFlag[0] = SHM_LRU_FLAG_LOG;
        m_pPointer[uitemp].oListData = m_pPointer[indexReal].oListData;
        m_pPointer[uitemp].uiListLog = m_iSessionID;
        
        m_pPointer[indexReal].uiListLog = uitemp;
        return indexReal;
    }
    
    THROW(MBC_SHMLruData+5);


}

template <class T>
unsigned int SHMLruData<T>::getLruDataForRead(unsigned int indexBig, FileLruData<T> * pInitData )
{          
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);

    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }

    if(m_iLastAccess!=indexBig){
        (*puiAccessCnt)++;
        m_iLastAccess=indexBig;
    }
    unsigned int indexReal = m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex;
    while(indexReal){
        if(indexBig == m_pPointer[indexReal].uiListIdxID ) {
            if( 0 == m_iSessionID )
                return indexReal;

            if(SHM_LRU_FLAG_DEL ==  m_pPointer[indexReal].sFlag[0]  ){
                /////已经删除的不能被查到
                THROW(MBC_SHMLruData+6);
                
            }    
            if( m_pPointer[indexReal].uiListLog ){
                
                if( m_iSessionID ==  m_pPointer[ m_pPointer[indexReal].uiListLog ].uiListLog  ) {
                    if(SHM_LRU_FLAG_FORDEL ==  m_pPointer[indexReal].sFlag[0]  ){
                        /////自己已经删除的自己不能被查到
                        THROW(MBC_SHMLruData+6);
                    }
                    return indexReal;
                }else{
                    if(SHM_LRU_FLAG_DEL ==  m_pPointer[m_pPointer[indexReal].uiListLog].sFlag[0] ){
                        /////已经删除的不能被查到
                        THROW(MBC_SHMLruData+6);
                    }
                    return m_pPointer[indexReal].uiListLog ;
                }
            }else{
                
                ////如果是链表头，则需要移动链表头
                if(SHM_LRU_FLAG_NORM != m_pPointer[indexReal].sFlag[0] ){

                    if( indexReal == (*(puiDirty+0) ) ){
                        return indexReal;
                    }
                    
                    ///移动链表
                    ///脱链
                    unsigned int uitemp = m_pPointer[indexReal].uiListPre;
                    if(uitemp)m_pPointer[uitemp].uiListNext = m_pPointer[indexReal].uiListNext;
                    uitemp = m_pPointer[indexReal].uiListNext;
                    if(uitemp)m_pPointer[uitemp].uiListPre = m_pPointer[indexReal].uiListPre;                    
                    ///接链到Dirty[0]链上面去

                    if( indexReal == m_pPointer[*(puiDirty+0)].uiListPre ){
                        m_pPointer[indexReal].uiListNext = *(puiDirty+0);
                        *(puiDirty+0) = indexReal;
                        return  indexReal;
                    }


                    if( *(puiDirty+0 ) ){
                        m_pPointer[indexReal].uiListPre = m_pPointer[*(puiDirty+0)].uiListPre;
                        m_pPointer[indexReal].uiListNext = *(puiDirty+0) ;
                        m_pPointer[*(puiDirty+0)].uiListPre = indexReal; 
                        (*(puiDirty+0)) = indexReal;
                    }else{
                        m_pPointer[indexReal].uiListPre = indexReal;
                        m_pPointer[indexReal].uiListNext = 0;
                        *(puiDirty+0) = indexReal;
                    } 

                }else{
                    if( indexReal == (*puiNorm) ){
                        return  indexReal;
                    }

                    ///移动链表
                    ///脱链
                    unsigned int uitemp = m_pPointer[indexReal].uiListPre;
                    if(uitemp)m_pPointer[uitemp].uiListNext = m_pPointer[indexReal].uiListNext;
                    uitemp = m_pPointer[indexReal].uiListNext;
                    if(uitemp)m_pPointer[uitemp].uiListPre = m_pPointer[indexReal].uiListPre;
                    
                    if( indexReal == m_pPointer[(*puiNorm)].uiListPre ){
                        m_pPointer[indexReal].uiListNext = *puiNorm;
                        *puiNorm = indexReal;
                        return  indexReal;
                    }
                    
                    ///接链到normal链上面去
                    if(0 == (*puiNorm) ){
                        m_pPointer[indexReal].uiListPre = indexReal;
                        m_pPointer[indexReal].uiListNext = 0;
                        *puiNorm = indexReal;
                    }else{
                        ///接链到normal链上面去
                        m_pPointer[indexReal].uiListPre = m_pPointer[(*puiNorm)].uiListPre;
                        m_pPointer[indexReal].uiListNext = *puiNorm ;
                        m_pPointer[(*puiNorm)].uiListPre = indexReal; 
                        *puiNorm = indexReal;
                    }

                }

                return indexReal;
            }
        }
        indexReal = m_pPointer[indexReal].uiListIdxNext ;
    }
        

    if(pInitData){
        if( pInitData->uiNext ){
            ////是删除的数据
            ////不处理直接返回
            return 0;
            
        }else{
            unsigned int iTempxx = mallocFree( );
            if(0 == iTempxx){
                iTempxx = doPageOut(0);
                if(0 == iTempxx){
                    THROW(MBC_SHMLruData+5);
                }
            }
    
            m_pPointer[iTempxx].uiListLog  = 0;
            m_pPointer[iTempxx].oListData  = pInitData->oData;
            m_pPointer[iTempxx].sFlag[0] = '\0';
            
            m_pPointer[iTempxx].uiListIdxID = indexBig;
            
            addToNormal( iTempxx );
            addToIndex(indexBig, iTempxx);
            ////indexReal = iTempxx;
            return iTempxx;
        }
    }else{
        
        indexReal = doPageIn(indexBig, false );
//        if(indexReal 
//            && SHM_LRU_FLAG_DEL == m_pPointer[indexReal].sFlag[0] ){
//            ////删除的项目 在 doPageIn 的时候，就已经判断了，这里不需要再判定了
//            
//        }

    }

    (*puiFailCnt)++;

    if( indexReal ){
        return indexReal;
    }

    THROW(MBC_SHMLruData+5);
        

}



template <class T>
void SHMLruData<T>::create(unsigned int itemnumber) 
{
    SHMAccess::create( getNeedSize(itemnumber) );
    open();
    *puiSessionCnt = SHM_LRU_MAX_SESSION;
    *puiExthwm = 98;
    *puiFreshHighPct = 60;
    *puiFreshLowPct = 10;
    *puiFreshInterval=60;
    *puiWrnhwm = 90;

#ifdef SHM_EXPAND_GLOBAL_POS
    m_pLruUnit = 
        (LruUnitData *)( (char *)(*m_poSHM) + (64+(*puiSessionCnt))*sizeof(unsigned int) );
    m_pLruUnit--;  ////让下标从1开始到itemnumber
//    SHMAccess::m_iSegment[0] = itemnumber+itemnumber/4;
    *puiSegment = itemnumber+itemnumber/4;
    SHMAccess::m_sSegment[0] = (char *)m_pLruUnit;
        

#else

    m_pPointer = (LruUnitData *)( (char *)(*m_poSHM) + (64+(*puiSessionCnt))*sizeof(unsigned int) );
    m_pPointer--;  ////让下标从1开始到itemnumber

#endif
    
    *puiVer = SHM_LRU_CURR_VERSION;
    *puiContentSize = sizeof(T);
    
    *puiFree = 1;
    for(int i =1; i< itemnumber; i++){
        m_pPointer[i].uiListNext = i+1;
        m_pPointer[i].uiListPre = i-1;
    }
    m_pPointer[itemnumber].uiListNext = 0;
    m_pPointer[itemnumber].uiListPre = itemnumber-1;
    
    *puiNorm = 0;
    *puiLog = itemnumber+1;
    m_pPointer[itemnumber+1].uiListNext = itemnumber+2;
    m_pPointer[itemnumber+1].uiListPre = 0;    
    for(int i = 2 ; i< itemnumber/4; i++){
        m_pPointer[itemnumber+i].uiListNext = itemnumber+i+1;
        m_pPointer[itemnumber+i].uiListPre = itemnumber+i-1;
    }
    m_pPointer[itemnumber+itemnumber/4].uiListNext = 0;
    m_pPointer[itemnumber+itemnumber/4].uiListPre = itemnumber+ itemnumber/4 -1;


    *puiDelete = 0;

    *puiFreeCnt = itemnumber;
    *puiNormCnt = 0;
    *puiDirtyCnt = 0;
    *puiLogCnt = 0;
    *puiLogTotalCnt = itemnumber/4;
    *puiInitLength=itemnumber;
    *puiUpperLength=0;
    *puiCommitDownFile=0;
	*puiWriteFileFlag=0;
    *puiExpandMaxSize =0;
    
    *puiLogBegin = itemnumber+1; 
    *puiHashValue = itemnumber;
    
    *puiIterCnt = itemnumber;
    m_iLastTotal = (*puiIterCnt); 

    *puiRealCnt = 0;
    *puiMallocCnt =0;
    *puiExpandCnt = 0;
    
    *puiAccessCnt=0; 
    *puiFailCnt=0;
    *puiHitrate=0;
    *puiState=0;
    *pulStateTime=0;

///    *puiSegment = 0;
    
    *puiDirty =0;      

    setState(0);


    SHMAccess::initExpand( puiExpandCnt, sizeof(LruUnitData), puiSegment); 
    
    if(0 == recoverFromFile() ){
        initFile(true);
    }
    
  
}


////产生的下标为对外的下标
template <class T>
unsigned int  SHMLruData<T>::malloc()
{
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }

//    while( (*puiUpperLength) && (*puiRealCnt)>=(*puiUpperLength) ){
//        //ALARMLOG28(0,MBC_CLASS_Fatal, 2,"%s","内存空间不足，申请LRU内存将超lru_queue_max_len.");
//        sleep(1);
//    }
    
    while(!m_oDataLock.P());

try{
    if(0 == (*puiDelete) ){
        if( (*puiMallocCnt) < 0x00fffffffe){
            (*puiMallocCnt)++;
            (*puiRealCnt)++;
            getLruDataForWrite(*puiMallocCnt, true );
            m_oDataLock.V();
            return *puiMallocCnt;
        }else{
            m_oDataLock.V();
            return 0;
        }
    }else{
        unsigned int iTemp= *puiDelete;
        unsigned int iReal = getLruDataForMalloc( iTemp ); 

        
        unsigned int * pNext =(unsigned int *)(& m_pPointer[iReal].oListData);
        *puiDelete = (*pNext);
        
        if(iTemp == (*puiDelete) ){
            ////最后一个删除的项被分配出来
            (*puiDelete) =0;
        }
            
        //addToIndex(iTemp, iReal);
        
        (*puiRealCnt)++;
        m_oDataLock.V();
        return iTemp;
    }
}catch(...){
    m_oDataLock.V();
    throw;
}
    m_oDataLock.V();
    return 0;
}


////产生的下标为对外的下标
template <class T>
unsigned int  SHMLruData<T>::malloc(T & indata )
{
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
//    while( (*puiUpperLength) && (*puiRealCnt)>=(*puiUpperLength) ){
//        //ALARMLOG28(0,MBC_CLASS_Fatal, 2,"%s","内存空间不足，申请LRU内存将超lru_queue_max_len.");
//        sleep(1);
//    }
    while(!m_oDataLock.P());

try{
    if(0 == (*puiDelete) ){
        if( (*puiMallocCnt) < 0x00fffffffe){
            (*puiMallocCnt)++;
            (*puiRealCnt)++;
            unsigned int indexReal = getLruDataForWrite(*puiMallocCnt, true );
            if(indexReal){ 
                m_pPointer[indexReal].oListData = indata;
                m_oDataLock.V();
                return *puiMallocCnt;
            }else{
                m_oDataLock.V();
                return 0;
            }
        }else{
            m_oDataLock.V();
            return 0;
        }
    }else{
        unsigned int iTemp= *puiDelete;
        unsigned int iReal = getLruDataForMalloc( iTemp ); 

        
        unsigned int * pNext =(unsigned int *)(& m_pPointer[iReal].oListData);
        *puiDelete = (*pNext);
        
        if(iTemp == (*puiDelete) ){
            ////最后一个删除的项被分配出来
            (*puiDelete) = 0;
        }
        //addToIndex(iTemp, iReal);
        (*puiRealCnt)++;

        m_pPointer[iReal].oListData = indata;
        m_oDataLock.V();
        return iTemp;
    }
}catch(...){
    m_oDataLock.V();
    throw;
}
    m_oDataLock.V();
    return 0;
}

template <class T>
void SHMLruData<T>::reset()
{
    ////直接reset危险性太大
    
}


template <class T>
bool SHMLruData<T>::revoke(unsigned int indexBig)
{        
    while(  (*puiFreeCnt)+(*puiNormCnt) < 128  ){
        usleep(10000);
    }
    while(!m_oDataLock.P());
try{
    unsigned int indexReal = getLruDataForWrite(indexBig);
    if(indexReal) {
        m_pPointer[indexReal].sFlag[0] = SHM_LRU_FLAG_FORDEL;
        m_oDataLock.V();
        return true;
    }
}catch(...){
    m_oDataLock.V();
    throw;
}
    m_oDataLock.V();
    return false;
    
}




template <class T>
void SHMLruData<T>::showDetail()
{
    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }

    char sW[4][16]={
        "自动控制",
        "定时写",
        "定量写",
        "实时写"};
    
    unsigned int state;
    long  thetime;
    if(  getState(state,  thetime) ){
        if( state>3)
            printf("内存写方式未知, 最后一次修改写方式的时间:%s", ctime(&thetime) );
        else
            printf("内存写方式:%s, 最后一次修改写方式的时间:%s", sW[state], ctime(&thetime) );
    }
   unsigned int initsize=getNeedSize((*puiInitLength));
   unsigned int expsize=SHMAccess::m_lNextSize * (*(SHMAccess::m_piExpandCnt));
   unsigned int iiUsed=initsize+expsize ;
   iiUsed = iiUsed < getExpandMaxSize() ? iiUsed : getExpandMaxSize() ;
   printf("%-16s:%-10u %-16s:%-10u %-16s:%-10u %-16s:%-10u\n", 
      "总条数", getCount(), 
      "内存容量", getTotal(),
      "内存大小(字节)",iiUsed,
      "内存最大值(字节)",getExpandMaxSize() );
      //"内存(数据+日志)", getMemCount()  );
    printf("%-16s:%-10u %-16s:%-10u %-16s:%-10u\n", 
      "内存(数据)",getMemDataCount(),
      "内存可换出量",(*puiNormCnt),
      "内存空闲条数",(*puiFreeCnt));
    printf("%-16s:%-3u%%       %-16s:%-10u %-16s:%-3u%%      \n", 
      "内存占用百分比",100*getCount()/getTotal(),
      "内存脏数据条数",getDirtyCount(),
      "内存脏数据百分比",getDirtyPercent());
    printf("%-16s:%-10u %-16s:%-10u %-16s:%-3u%%      \n", 
      "未提交条数",(*puiLogCnt),
      "提交但未落地条数",getDirtyCount()-(*puiLogCnt),
      "物理日志百分比",getLogPercent());

    printf("%-16s:%-3u%%       %-16s:%-10u %-16s:%-10u\n", 
      "命中率",getHitrate(),
      "命中次数",(*puiAccessCnt)>(*puiFailCnt)? ((*puiAccessCnt)-(*puiFailCnt)):0 ,
      "换入换出次数", (*puiAccessCnt)>(*puiFailCnt)? (*puiFailCnt): (*puiAccessCnt) );
    printf("%-16s:%-10u %-16s:%-10u %-16s:%-10u\n", 
      "LRU队列长度", getLruLength(), 
      "LRU队列初始长度", (*puiInitLength),
      "LRU队列最大长度", (*puiUpperLength)  );

    printf("%-16s:%-3u%%       %-16s:%-3u%%       %-16s:%-10u\n", 
      "自动扩展阀值",  getExthwm(), 
      "高水位告警阀值", getWrnhwm(),
      "定时写间隔(秒)", getFreshInt()  );        

    printf("%-16s:%-3u%%       %-16s:%-3u%%       %-16s:%-10s\n", 
      "脏数据刷新阀值H", getFreshHighPct(), 
      "脏数据刷新阀值L", getFreshLowPct(),
      "业务刷新是否提交", getCommitDownFile()>0? "true":"false" );  
 
                
    SHMAccess::showExpandInfo();

}

template <class T>
void SHMLruData<T>::showNormal( bool bDetail )
{
    unsigned int *pui = puiNorm;
    unsigned int ucnt = 0;
    while( *pui){
        if(bDetail)
            printf("%u-pre%u,", *pui,  m_pPointer[*pui].uiListPre );
        
        pui = &m_pPointer[*pui].uiListNext;
        ucnt++;
    }
    printf("normal tree cnt %u, head:%u,tail:%u\n", ucnt, (*puiNorm), m_pPointer[*puiNorm].uiListPre );
}

template <class T>
void SHMLruData<T>::open()
{
    
    puiVer = (unsigned int *)( (char *)(*m_poSHM) ) ;

    puiFree = puiVer+1;    
    puiNorm = puiVer+2;
    puiLog = puiVer+3;
    puiDelete = puiVer+4;

    puiFreeCnt = puiVer+5;
    puiNormCnt = puiVer+6;
    puiDirtyCnt = puiVer+7;
    puiLogCnt = puiVer+8;
    puiLogTotalCnt =  puiVer+9;
    
    puiIterCnt = puiVer+10;
    puiRealCnt = puiVer+11;
    puiExpandCnt = puiVer + 12;    //扩展内存的次数
    puiMallocCnt = puiVer+13;
    puiSessionCnt = puiVer+14;
    
    puiState = puiVer+15; 
    pulStateTime = (long *) (puiVer+16);  //long型要8字节对齐：且占用两个int位置：16和17

    puiFailCnt  = puiVer+18;
    puiAccessCnt= puiVer+19;
    puiHitrate  = puiVer+20;    

    puiLogBegin = puiVer+21; 
    puiHashValue= puiVer+22;

  

    puiSegment = puiVer+23; ////接下来的8个int用来记录扩展的内存存放偏移分段情况
    ////segment end puiVer+23+1+MAX_SHM_EX_CNT =  puiVer+32
    ////后面从puiVer+ 32 开始到puiVer+52 预留以后扩展使用。
    puiExpandMaxSize=puiVer+53;
    puiWriteFileFlag=puiVer+54;
    puiCommitDownFile=puiVer+55;
    puiInitLength=puiVer+56;
    puiUpperLength=puiVer+57;
    puiContentSize = puiVer+58;
    puiFreshInterval= puiVer+59;
    puiWrnhwm = puiVer+60;
    puiFreshHighPct = puiVer+61;
    puiFreshLowPct = puiVer+62;
    puiExthwm = puiVer+63;////超过预定使用的阀值后自动扩展。
    puiDirty = puiVer+64;  
    
#ifdef SHM_EXPAND_GLOBAL_POS
    m_pLruUnit = 
        (LruUnitData *)( (char *)(*m_poSHM) + (64+(*puiSessionCnt))*sizeof(unsigned int) );
    m_pLruUnit--;  ////让下标从1开始到itemnumber
//    SHMAccess::m_iSegment[0] = (*puiLogBegin)+(*puiLogTotalCnt)-1; ////log 最初的地方
    (*puiSegment) = (*puiLogBegin)+(*puiLogTotalCnt)-1;
    SHMAccess::m_sSegment[0] = (char *)m_pLruUnit;
#else

    m_pPointer = (LruUnitData *)( (char *)(*m_poSHM) + (64+(*puiSessionCnt))*sizeof(unsigned int) );
    m_pPointer--;  ////让下标从1开始到itemnumber

#endif

    SHMAccess::initExpand( puiExpandCnt, sizeof(LruUnitData), puiSegment);
    
}


////返回值为对内的下标
template <class T>
unsigned int SHMLruData<T>::mallocFree( )
{
    unsigned int iRet=0;
    if( puiFree && (*puiFree) ){
        iRet = *puiFree;
        //unsigned int uitemp = m_pPointer[iRet].uiListPre;
        //if(uitemp) m_pPointer[uitemp].uiListNext = m_pPointer[iRet].uiListNext;
        *puiFree = m_pPointer[iRet].uiListNext;
        (*puiFreeCnt)--;
        if(*puiFree) m_pPointer[*puiFree].uiListPre = m_pPointer[iRet].uiListPre;        
        
        m_pPointer[iRet].uiListPre =0;
        m_pPointer[iRet].uiListNext=0;
        
        return iRet;
    }
    
    iRet = doPageOut(0);
//    if(0 == iRet)
//        THROW(MBC_SHMLruData+5);

    return iRet;

}


////返回值为对内的下标
////mallocLog 出来的空间，不能在外面进行memset，否则内存全乱
template <class T>
unsigned int SHMLruData<T>::mallocLog()
{
    unsigned int iTemp=0;
    if( (*puiLogCnt) < (*puiLogTotalCnt) ){
        iTemp = *puiLog;
        *puiLog = m_pPointer[iTemp].uiListNext;
        (*puiLogCnt)++;
        return iTemp;
    }
    
    unsigned int iTemp2 = mallocFree( );
    if(0 == iTemp2)
        return 0 ;

    (*puiLogCnt)++;
    return iTemp2;

}


////indexBig为对外的下标，index为对内的内存下标
template <class T>
unsigned int SHMLruData<T>::addToIndex(unsigned int indexBig, unsigned int index )
{
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);
        
    unsigned int indexReal = m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex;
    if(indexReal
      && m_pPointer[indexReal].uiListIdxID  == indexBig ){
        return indexReal;
    }
    if( indexReal==index 
      && 0==m_pPointer[indexReal].uiListIdxID ){
        return indexReal;
    }
    

    m_pPointer[index].uiListIdxNext = indexReal;
    m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex = index;

    
    return indexReal;
}

////indexBig为对外的下标，
template <class T>
unsigned int SHMLruData<T>::delFromIndex(unsigned int indexBig, int iFlag )
{
    if( indexBig < 1 || indexBig > (*puiMallocCnt) )
        THROW(MBC_SHMLruData+2);
    
    unsigned int *pPre = &m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex;
    unsigned int indexReal = *pPre;
    
    while(indexReal){
        if( m_pPointer[indexReal].uiListIdxID  == indexBig ){
            if(pPre){
                (*pPre)= m_pPointer[indexReal].uiListIdxNext;
            }
            ///m_pPointer[indexReal].uiListIdxNext =0 ;
            m_pPointer[indexReal].uiListIdxID =  0 ;
            m_pPointer[indexReal].sFlag[0] = 0;

            if(1==iFlag){
                addToDelIndex(indexBig, indexReal);
            }
            return indexReal;

        }
        pPre = &m_pPointer[indexReal].uiListIdxNext;
        indexReal = m_pPointer[indexReal].uiListIdxNext;
 
    }
    
    return 0;
}



////输入参数 indexBig 为对外的下标,index为目前在内存的下标
template <class T>
void SHMLruData<T>::addToDelIndex( unsigned int indexBig, unsigned int index)
{
/*

    ///接链到 DeleteIdx 链上面去
    m_pPointer[index].uiListIdxID = indexBig;
    m_pPointer[index].uiListIdxNext = *puiDelete ;
    (*puiDelete) = indexBig;

    m_pPointer[index].sFlag[0] = SHM_LRU_FLAG_DEL;

*/


}


////返回值为对内的下标
////输入参数index为对外的下标，不能为0，ifModify为true表示换入后要进行修改，false表示不修改

template <class T>
unsigned int SHMLruData<T>::doPageIn(unsigned int indexBig, bool ifModify )
{
    if(0 == indexBig)
        THROW(MBC_SHMLruData+2);

    unsigned int iTemp = mallocFree( );
    if(0==iTemp){
        iTemp=doPageOut(0);
        if(0 == iTemp)
        {
        	while(1)
        	{
        		usleep(10000);
        	}
            THROW(MBC_SHMLruData+5);
         }
    }

    
    if( ifModify ) addToDirty(iTemp, m_iSessionID);
    else addToNormal(iTemp);
    

    FILE *fp;
    FileLruData<T> oFiledata;
    int iErrID = 0;
    
    snprintf(m_sFileExName,sizeof(m_sFileExName)-1,
        "%s.%04u", m_sFileName, (indexBig-1)/m_iCntPerFile);
    
    if ((fp = fopen (m_sFileExName, "rb+")) != NULL) {
       
        long lFileOffset = ((indexBig-1)%m_iCntPerFile)*sizeof(FileLruData<T>);
        fseek(fp, 0,  SEEK_END);

        if( ( lFileOffset + sizeof(FileLruData<T>) ) > ftell(fp) ){
            ////第一次malloc出来的
            fseek(fp, 0,  SEEK_END);

            memset(&oFiledata, 0, sizeof(oFiledata) );
            while(  ( lFileOffset + sizeof(FileLruData<T>) ) > ftell(fp) ){
                int i = fwrite(&oFiledata, sizeof(oFiledata), 1, fp);
                if (i != 1){
                    fclose (fp);
                    iErrID = MBC_SHMLruData+9;
                    goto __LRU_PAGEIN_ERR;
    
                }
            }
            fseek(fp, lFileOffset ,SEEK_SET);
            if( lFileOffset != ftell(fp) ){
                fclose (fp);
                iErrID = MBC_SHMLruData+8;
                goto __LRU_PAGEIN_ERR;

            }
            fclose (fp);
            m_pPointer[iTemp].oListData = oFiledata.oData;
            m_pPointer[iTemp].uiListIdxID = indexBig;
            if(oFiledata.uiNext){
                m_pPointer[iTemp].sFlag[0] = SHM_LRU_FLAG_DEL;
                m_pPointer[iTemp].uiListIdxNext = oFiledata.uiNext;

            }else{
                m_pPointer[iTemp].sFlag[0]=SHM_LRU_FLAG_NORM;
                addToIndex(indexBig, iTemp);
            }
            return iTemp;
        }
        fseek(fp, lFileOffset ,SEEK_SET);
        int i = fread(&oFiledata, sizeof(FileLruData<T>), 1, fp );
        if(i != 1 ){
            fclose (fp);
            iErrID = MBC_SHMLruData+8;
            goto __LRU_PAGEIN_ERR;
        }
        m_pPointer[iTemp].oListData = oFiledata.oData;
        m_pPointer[iTemp].uiListIdxID = indexBig;
        if( oFiledata.uiNext ){
            ///属于删除的项目
            
            if( !ifModify ){
                ////删除的项目，被重分配后，只支持赋值，不支持查询
                fclose (fp);
                iErrID = MBC_SHMLruData+6;
                goto __LRU_PAGEIN_ERR; 
            }
            m_pPointer[iTemp].sFlag[0] = SHM_LRU_FLAG_DEL;
            //m_pPointer[iTemp].uiListIdxNext = oFiledata.uiNext;
            
        }else{
            m_pPointer[iTemp].sFlag[0]=SHM_LRU_FLAG_NORM;
            addToIndex(indexBig, iTemp);
        }
        fclose (fp);
        return iTemp;
    }
    
    iErrID = MBC_SHMLruData+7;
    
__LRU_PAGEIN_ERR:


    if( ifModify ) doDirtyOut(iTemp);
    else doPageOut(iTemp );

    addToFree(iTemp);

    THROW(iErrID);
    
}


////返回值为对内的下标
////输入参数index为对内的下标，如果为0，表示将最不常用的换出，否则将index换出
template <class T>
unsigned int SHMLruData<T>::doPageOut(unsigned int index)
{
    if(0 ==index){
        index = (*puiNorm);
        if(0==index){
            return 0;
        }
        index = m_pPointer[index].uiListPre; 

    }
    
    if(index!=0 && index <= (*puiIterCnt) ){
        ///脱链
        unsigned int uitemp = m_pPointer[index].uiListPre;

        if(index == m_pPointer[*puiNorm].uiListPre )
            m_pPointer[*puiNorm].uiListPre = m_pPointer[index].uiListPre;
        
        if(uitemp)
            m_pPointer[uitemp].uiListNext = m_pPointer[index].uiListNext;
        
        uitemp = m_pPointer[index].uiListNext;
        if(uitemp)
            m_pPointer[uitemp].uiListPre = m_pPointer[index].uiListPre;
        
        
        if(0 ==  m_pPointer[index].uiListIdxID ){
            printf("memory fault.");
        }
        delFromIndex(m_pPointer[index].uiListIdxID, 0);

        m_pPointer[index].uiListPre  = 0;
        m_pPointer[index].uiListNext = 0;
        m_pPointer[index].uiListIdxID= 0;
        m_pPointer[index].uiListLog  = 0;
        m_pPointer[index].oListData  = m_oDelUnit;
        m_pPointer[index].sFlag[0] = '\0';

        
        
        (*puiNormCnt)--;
        if( index == (*puiNorm) ) {
            (*puiNorm)=0;
        }
        
        return index;
        
    }
    
    return 0;
    
}

template <class T>
void SHMLruData<T>::doListIn(unsigned int indexReal, unsigned int * pHead, unsigned int *pCnt )
{
    if(0 == (*pHead) ){
        m_pPointer[indexReal].uiListPre = indexReal;
        m_pPointer[indexReal].uiListNext = 0;
        
    }else{
        ///接链到head上面去
        m_pPointer[indexReal].uiListPre = m_pPointer[(*pHead)].uiListPre;
        m_pPointer[indexReal].uiListNext = *pHead ;
        m_pPointer[(*pHead)].uiListPre = indexReal;
        
    }
    
    (*pHead)=indexReal;
    (*pCnt)++;

    return ;
}

template <class T>
unsigned int SHMLruData<T>::doListOut(unsigned int indexReal, unsigned int * pHead, unsigned int *pCnt )
{
    if(indexReal!=0 && indexReal <= (*puiIterCnt) ){
        ///脱链
        if( indexReal == (*pHead ) ){
            ///list head
            (*pHead ) =  m_pPointer[indexReal].uiListNext;
            if( *pHead ){
                m_pPointer[*pHead].uiListPre  = m_pPointer[indexReal].uiListPre;
            }

        }else{

            unsigned int uitemp = m_pPointer[indexReal].uiListPre;
            if(uitemp)
                m_pPointer[uitemp].uiListNext = m_pPointer[indexReal].uiListNext;
            
            uitemp = m_pPointer[indexReal].uiListNext;
            if(uitemp){
                m_pPointer[uitemp].uiListPre = m_pPointer[indexReal].uiListPre;
            }
    
            if( indexReal == m_pPointer[*pHead].uiListPre ){
                ////list tail 
                m_pPointer[*pHead].uiListPre = m_pPointer[indexReal].uiListPre;
            }
            
        }
        m_pPointer[indexReal].uiListPre=0;
        m_pPointer[indexReal].uiListNext=0;
        
        unsigned int uitemp = m_pPointer[indexReal].uiListLog;
        if(uitemp){
            if( uitemp < (*puiLogBegin) 
                || uitemp >= (*puiLogBegin)+(*puiLogTotalCnt)  ){
                ////额外分配的Log空间
                addToFree(uitemp);
                (*puiLogCnt)--;
            }else{
                ////Log原本的空间
                addToFreeLog(uitemp);
            }
        }
        m_pPointer[indexReal].uiListLog = 0;
        
        if(pCnt)
            (*pCnt)--;

        return indexReal;

    }
    return 0;
}

template <class T>
unsigned int SHMLruData<T>::doListMoveFirst(unsigned int indexReal, unsigned int * pHead)
{
    if(indexReal!=0 && indexReal <= (*puiIterCnt) ){
        ///脱链
        
        if( indexReal == (*pHead) ){
            ////list head 
            return  indexReal;
        }

        ///移动链表
        ///脱链
        unsigned int uitemp = m_pPointer[indexReal].uiListPre;
        if(uitemp)m_pPointer[uitemp].uiListNext = m_pPointer[indexReal].uiListNext;
        uitemp = m_pPointer[indexReal].uiListNext;
        if(uitemp){
            m_pPointer[uitemp].uiListPre = m_pPointer[indexReal].uiListPre;
        }
        
        if( indexReal == m_pPointer[*(pHead)].uiListPre ){
            ////list tail 
            m_pPointer[indexReal].uiListNext = *pHead;
            *pHead = indexReal;
            return  indexReal;
        }
        
        ///接链到head上面去
        if(0 == (*pHead) ){
            m_pPointer[indexReal].uiListPre = indexReal;
            m_pPointer[indexReal].uiListNext = 0;
            *pHead = indexReal;
        }else{
            m_pPointer[indexReal].uiListPre = m_pPointer[(*pHead)].uiListPre;
            m_pPointer[indexReal].uiListNext = *pHead ;
            m_pPointer[(*pHead)].uiListPre = indexReal; 
            *pHead = indexReal;
        }
        return  indexReal;
    }
    return 0;
}

////返回值为对内的下标
////输入参数index为对内的下标，不能为0
template <class T>
unsigned int SHMLruData<T>::doDirtyOut(unsigned int index)
{
     unsigned int * pDirty = (puiDirty+m_iSessionID);
    return doListOut( index, pDirty, puiDirtyCnt);

}


////输入参数index为对内的下标
template <class T>
void SHMLruData<T>::addToNormal( unsigned int index )
{
    
    if(0 == (*puiNorm) ){
        m_pPointer[index].uiListPre = index;
        m_pPointer[index].uiListNext = 0;
        
    }else{
        ///接链到normal链上面去
        m_pPointer[index].uiListPre = m_pPointer[(*puiNorm)].uiListPre;
        m_pPointer[index].uiListNext = *puiNorm ;
        m_pPointer[(*puiNorm)].uiListPre = index;
        
    }
    
    (*puiNorm)=index;
    (*puiNormCnt)++;

}


////输入参数index为对内的下标
template <class T>
void SHMLruData<T>::addToDirty( unsigned int index, int i_iSession )
{
     unsigned int * pDirty = puiDirty+i_iSession;

    if(0 == (*pDirty) ){
        m_pPointer[index].uiListPre = 0;
        m_pPointer[index].uiListNext = 0;

    }else{
        ///接链到Dirty链上面去
        m_pPointer[index].uiListPre = m_pPointer[(*pDirty)].uiListPre;
        m_pPointer[index].uiListNext = *pDirty ;
        m_pPointer[(*pDirty)].uiListPre = index; 

    }

    (*pDirty)=index;
    if(0!=i_iSession) (*puiDirtyCnt)++;

}



////输入参数index为对内的下标
template <class T>
void SHMLruData<T>::addToFree( unsigned int index )
{
 
    if(0 == (*puiFree) ){
        m_pPointer[index].uiListPre = 0;
        m_pPointer[index].uiListNext = 0;

    }else{
        ///接链到Free链上面去
        m_pPointer[index].uiListPre = m_pPointer[(*puiFree)].uiListPre;
        m_pPointer[index].uiListNext = *puiFree ;
        m_pPointer[(*puiFree)].uiListPre = index; 
    }

    (*puiFree)=index;    
    (*puiFreeCnt)++;
}


////输入参数index为对内的下标
template <class T>
void SHMLruData<T>::addToFreeLog( unsigned int index )
{
    
    if(0 == (*puiLog) ){
        m_pPointer[index].uiListPre = 0;
        m_pPointer[index].uiListNext = 0;

    }else{
        ///接链到FreeLog链上面去
        m_pPointer[index].uiListPre = m_pPointer[(*puiLog)].uiListPre;
        m_pPointer[index].uiListNext = *puiLog ;
        m_pPointer[(*puiLog)].uiListPre = index; 
    }

    (*puiLog)=index;    
    (*puiLogCnt)--;
}





////mallocLog 出来的内存块，只能通过commit或rollback来重新放到可用链表上去，因为在正常链不记录它了。
template <class T>
unsigned int SHMLruData<T>::commit()
{
     unsigned int * pDirty = puiDirty+m_iSessionID;
    unsigned int uCnt=0;
    while(!m_oDataLock.P());
    while( *pDirty ){
        unsigned int uitemp = m_pPointer[ *pDirty ].uiListLog;
        unsigned int uinext = m_pPointer[ *pDirty ].uiListNext ;
        

        
        if( SHM_LRU_FLAG_FORDEL ==  m_pPointer[ *pDirty ].sFlag[0]  ){
            m_pPointer[ *pDirty ].sFlag[0] = SHM_LRU_FLAG_DEL;
//            if( SHM_LRU_FLAG_DEL !=  m_pPointer[uitemp].sFlag[0]  ){
//                ////数据被删掉了
//                (*puiRealCnt)--;
//            }
            ////数据被删掉了
            (*puiRealCnt)--;
                            
            unsigned int iBig = m_pPointer[*pDirty].uiListIdxID;

            unsigned int * iDNext = (unsigned int *)(&m_pPointer[*pDirty].oListData);
            if( 0 == (*puiDelete) )
                (*iDNext) = iBig;
            else
                (*iDNext) = (*puiDelete);
                

            (*puiDelete) = iBig;
            
              
        }else if(SHM_LRU_FLAG_NORM == m_pPointer[ *pDirty ].sFlag[0]  ){
            m_pPointer[ *pDirty ].sFlag[0] = SHM_LRU_FLAG_MODIFY;
        }
        
        m_pPointer[ *pDirty ].uiListLog = 0;
        
        addToDirty(*pDirty, 0);
        
        *pDirty = uinext;
        if(uinext) m_pPointer[ uinext ].uiListPre = 0;
        
        ///*(puiDirty+m_iSessionID ) = uinext;
        if(uitemp){
            if( uitemp < (*puiLogBegin) 
                || uitemp >= (*puiLogBegin)+(*puiLogTotalCnt) ){
                ////额外分配的Log空间
                addToFree(uitemp);
                (*puiLogCnt)--;
            }else{
                ////Log原本的空间
                addToFreeLog(uitemp);
            }
        }
        uCnt++;


    }
    m_oDataLock.V();
    return uCnt;

}

template <class T>
unsigned int SHMLruData<T>::rollback()
{
     unsigned int * pDirty = puiDirty+m_iSessionID;
    unsigned int uCnt=0;
    while(!m_oDataLock.P());    
    while( *pDirty ){
        unsigned int uitemp = m_pPointer[ *pDirty ].uiListLog;
        unsigned int uinext = m_pPointer[ *pDirty ].uiListNext ;
        
        if(uitemp){
            if(m_pPointer[uitemp].uiListLog != m_iSessionID  ){
                THROW(MBC_SHMLruData+10);
            }
            ////恢复备份的数据
            m_pPointer[ *pDirty ].oListData = m_pPointer[uitemp].oListData;
            m_pPointer[ *pDirty ].sFlag[0] = m_pPointer[uitemp ].sFlag[0] ;

        }
        if(SHM_LRU_FLAG_NORM == m_pPointer[ *pDirty ].sFlag[0] ){
            m_pPointer[ *pDirty ].sFlag[0] = SHM_LRU_FLAG_MODIFY;
        }else if( SHM_LRU_FLAG_DEL == m_pPointer[ *pDirty ].sFlag[0] ){
            ///刚刚从删除的数据分配到的空间进行操作
            ///如果回滚，则分配的偏移收回
            (*puiRealCnt)--;
            unsigned int iBig = m_pPointer[*pDirty].uiListIdxID;
            unsigned int * iDNext = (unsigned int *)(&m_pPointer[*pDirty].oListData);
            if( 0 == (*puiDelete) )
                (*iDNext) = iBig;
            else
                (*iDNext) = (*puiDelete);
                

            (*puiDelete) = iBig;
        }
        addToDirty(*pDirty, 0);
        
        m_pPointer[ *pDirty ].uiListLog = 0;
        *pDirty = uinext;
        if(uinext) m_pPointer[ uinext ].uiListPre = 0;
        
        if(uitemp){
            if( uitemp < (*puiLogBegin) 
                || uitemp >= (*puiLogBegin)+(*puiLogTotalCnt)  ){
                ////额外分配的Log空间
                addToFree(uitemp);
                (*puiLogCnt)--;
            }else{
                ////Log原本的空间
                addToFreeLog(uitemp);
            }

        }
        uCnt++;

    }
    m_oDataLock.V();
    return uCnt;

}


/////提交指定的其他session,这种情况应该比较少
template <class T>
void SHMLruData<T>::commitSession(unsigned int iSession)
{
    if(m_iSessionID){
        THROW(MBC_SHMLruData+10);
    }
    
    if( iSession >= SHM_LRU_MAX_SESSION )
        THROW(MBC_SHMLruData+11);

    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }
        
    unsigned int * pDirty = puiDirty+iSession;


}



////session死掉后，由专门进程来回滚它
template <class T>
void SHMLruData<T>::rollbackSession (unsigned int iSession)
{
    if(m_iSessionID){
        THROW(MBC_SHMLruData+10);
    }
    if( iSession >= SHM_LRU_MAX_SESSION )
        THROW(MBC_SHMLruData+11);

    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }

    unsigned int * pDirty = puiDirty+iSession;

 
}


template <class T>
bool SHMLruData<T>::getByFile( unsigned int iOffset, T& outT )
{
    
    int iSeq=(iOffset-1)/m_iCntPerFile;
    FileLruData<T> oFileData;
    FILE *fp = prepareFile(iSeq, false, false);
    if(fp){
        long itemp = ((iOffset-1)%m_iCntPerFile)*sizeof(FileLruData<T>);
        fseek(fp, itemp, SEEK_SET);
        int i = fread( &oFileData, sizeof(FileLruData<T> ), 1, fp  );
        if(i!=1){
            fclose(fp);
            return false;
        }
        outT=oFileData.oData;
        return true;
    }
    
    return false;
    
}


/////写到磁盘进行落地
template <class T>
unsigned int SHMLruData<T>::writeToFile(bool bForce )
{
//    if(m_iSessionID){
//        THROW(MBC_SHMLruData+10);
//    }
    
    static unsigned int uiLastHash = 0;
    static unsigned int uiLastIter = 0;
    static unsigned int uiLastReal = 0;
    static unsigned int uiLastMalloc = 0;
    static unsigned int uiLastDelete = 0;
    
    
    
    if( (*puiContentSize)!= sizeof(T) )
        return 0;
    
    if( getCommitDownFile()== 1 ){
        bForce = true;
    }
    
    ///定时 定量
    if( !bForce ){
        switch( *puiState){
            case 1:
            {
                ////定时写
            	struct timeval tv;
            	struct timezone tz;
                if(0 == gettimeofday (&tv , &tz) ){
                    if(tv.tv_sec >= m_lClock + SHM_LRU_DIRTY_CLOCKCNT ){
                        ///时间到
                        m_lClock = tv.tv_sec;
                        printf("到达定时阀值\n");
                        break;
                    }else{
                        return 0;
                    }
                }
                //此处不break,如果获取时间失败，直接转成定量写方式。
                //break;
            }
            case 2:
                ////定量写
                if( getDirtyPercent() < SHM_LRU_DIRTY_HIGH_LEVEL){
                    ////未达到量的条件
                    return 0;
                }
                printf("到达定量阀值\n");

                break;
            case 3:
                ////实时写
                if( getCommitDirtyCount() == 0 )
                    return 0;
                
                break;
            default:
                ////程序自动控制，达到量或者达到时间，都会写
                if( getDirtyPercent() < SHM_LRU_DIRTY_HIGH_LEVEL){
                    ////未达到量的条件
                	struct timeval tv;
                	struct timezone tz;
                    if(0 == gettimeofday (&tv , &tz) ){
                        if(tv.tv_sec >= m_lClock + SHM_LRU_DIRTY_CLOCKCNT ){
                            ///时间到
                            m_lClock = tv.tv_sec;
                            printf("到达定时阀值\n");
                        }else{
                            return 0;
                        }
                    }else{
                        return 0;
                    }
                }else{
                    printf("到达定量阀值\n");
                }
                
                break;

        }
		if(getWriteFileFlag())
			return 0;
    }

    if(m_iLastTotal != (*puiIterCnt)) {
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);
    }


    FILE *fpHead;
    if ((fpHead = fopen (m_sFileName, "rb+")) == NULL) {
        THROW(MBC_SHMLruData+7);
    }
    if( uiLastHash != (*puiHashValue) 
        || uiLastIter != (*puiIterCnt)
        || uiLastReal!=(*puiRealCnt)
        || uiLastMalloc!=(*puiMallocCnt)
        || uiLastDelete != (*puiDelete)
        ){
        
        FileLruHead oHead;
        writeFileHead( fpHead, oHead, 1);

        uiLastHash = oHead.uiHashValue;
        uiLastIter = oHead.uiIterCnt;
        uiLastReal = oHead.uiRealCnt;
        uiLastMalloc = oHead.uiMallocCnt;
        uiLastDelete = oHead.uiDelete;

    }            



    unsigned int iWriteCnt = 0;

    if( getCommitDirtyCount() > 0  ){
        
        iWriteCnt =  writeFileBatch( fpHead,  bForce );
        if( iWriteCnt != 0){
            fflush(fpHead);
            fclose (fpHead);
            return iWriteCnt;
        }
        printf("写0条到文件\n");

    }

    fclose(fpHead);
    fpHead=0;    
    return iWriteCnt;
}


template <class T>
unsigned int SHMLruData<T>::writeFileHead( FILE * fp, FileLruHead & oHead, unsigned int uiState )
{
    memset(&oHead, 0, sizeof(FileLruHead) );

    oHead.uiHashValue=(*puiHashValue);
    oHead.uiIterCnt=(*puiIterCnt);
    oHead.uiRealCnt=(*puiRealCnt);
    oHead.uiMallocCnt=(*puiMallocCnt);
    oHead.uiExpandCnt= (*puiExpandCnt);

    oHead.uiVer=(*puiVer);
    oHead.lKey = m_lSHMKey;
    oHead.uiState = uiState;
    oHead.uiDelete = (*puiDelete);
    oHead.uiContentSize=(*puiContentSize);

    snprintf(oHead.sName, sizeof(oHead.sName), "V%d", (*puiVer)  );
    
    fseek(fp, 0,  SEEK_SET);
    int i = fwrite(&oHead, sizeof(oHead), 1, fp);
    if(i!=1){
        return 0;
    }

    return sizeof(FileLruHead);
    
}

template <class T>
unsigned int SHMLruData<T>::expandFile(FILE * fp )
{
    unsigned int iRet=0;
    
    unsigned long lneed;
    
//    if( (*puiMallocCnt) < (*puiIterCnt) )
//        lneed  =  (unsigned long)(*puiIterCnt)*sizeof(FileLruData<T>) +  sizeof(FileLruHead);
//    else
//        lneed  =  (unsigned long)(*puiMallocCnt)*sizeof(FileLruData<T>) +  sizeof(FileLruHead);
//    
    lneed = m_iCntPerFile * sizeof(FileLruData<T>);
   fseek(fp, 0,  SEEK_END);

    if( ftell(fp) < lneed ){
        ////文件末尾追加数据
        FileLruData<T> oFileData[8192];
        memset( &oFileData, 0, sizeof(oFileData) );
        unsigned int icnt = ( lneed - ftell(fp) - 1 )/sizeof(FileLruData<T>) + 1;
        int i = 0;
        iRet = icnt;
        while( icnt ){
            if(icnt>=8192){
                i = fwrite(&oFileData, sizeof(FileLruData<T>)*8192, 1, fp);
                if(i != 1){

                    return 0xffffffff;
                    
                }
                icnt -= 8192;
            }else{
                i = fwrite(&oFileData, sizeof(FileLruData<T>)*icnt , 1, fp);
                if(i != 1 ){

                    return 0xffffffff;

                }
                icnt = 0;
            }

            
        }
    }
    
    return iRet;
}


//template <class T>
//unsigned int SHMLruData<T>::writeFileAll( FILE * fp )
//{
//    FileLruHead oHead;
//    memset(&oHead, 0, sizeof(FileLruHead) );
//    
//    m_oDataLock.P();
//    
//    unsigned int iMaxCnt = (*puiMallocCnt);
//    unsigned long ulBufSize = sizeof( FileLruData<T> )*iMaxCnt;
//
// 
//    FileLruData<T> * poFileData = new FileLruData<T>[ iMaxCnt ];
//    
//        
//    if(!poFileData){
//        printf("内存申请不到，大小：%lu 字节。\n", ulBufSize );
//        m_oDataLock.V();
//        usleep(20000);
//        return 0;
//    }
//    expandFile(fp);
//
//    fseek(fp, sizeof(FileLruHead) ,SEEK_SET);
//    int i = fread( poFileData, sizeof(FileLruData<T> )*iMaxCnt, 1, fp  );
//
//    if(i!=1){
//        m_oDataLock.V();
//        delete [] poFileData;
//        return 0;
//    }
//        
//
//
//    if(m_iLastTotal != (*puiIterCnt)) {
//        ////如果有做内存扩展，这次写文件就不写了
//        m_oDataLock.V();
//        delete [] poFileData;
//        
//        SHMAccess::reopen();
//        m_iLastTotal = (*puiIterCnt);
//
//        return 0;
//    }
//
//    unsigned int uitemp = 0;
//    unsigned int uiFile = 0;
//    unsigned int iDirty = 0;
//    unsigned int uiTheNext=0;
//    unsigned int uiThePre = 0;
//    unsigned int uideleted = (*puiDelete);
//
//    unsigned int iWriteCnt = 0;
//
////////////////先在私有内存里面做好
//    
//    for(int i=0; i< SHM_LRU_MAX_SESSION; i++){
//        
//        iDirty = *(puiDirty+i);
//        
//        while( iDirty ){
//            
//            uitemp = m_pPointer[ iDirty ].uiListIdxID;
//            uiTheNext = m_pPointer[ iDirty ].uiListNext;
//            
//            uiFile = uitemp-1;
//
//            uitemp = m_pPointer[ iDirty ].uiListLog;
//            if( uitemp ){
//                ////还未提交
//                poFileData[uiFile].oData = m_pPointer[uitemp].oListData;
//                if( SHM_LRU_FLAG_DEL ==  m_pPointer[ uitemp ].sFlag[0] ){
//                    ////
//
//                }else{
//                    poFileData[uiFile].uiNext = 0;
//
//                }
//                
//            }else{
//                poFileData[uiFile].oData = m_pPointer[iDirty].oListData;
//                unsigned int iBig = m_pPointer[ iDirty ].uiListIdxID; 
//                
//                if( SHM_LRU_FLAG_DEL == m_pPointer[ iDirty ].sFlag[0] ){
//
//                    if( 0 == uideleted )
//                        poFileData[uiFile].uiNext = iBig;
//                    else
//                        poFileData[uiFile].uiNext = uideleted;
//                    
//                    uideleted = iBig;
//                    
//                }else{
//                    poFileData[uiFile].uiNext = 0;
//                }
//
//                
//            }
//
//            iWriteCnt++;
//            iDirty = uiTheNext;
//
//        }
//        
//    }
//
////////////////批量入文件
//    
//    fseek(fp, sizeof(FileLruHead) ,SEEK_SET);
//    int iw = fwrite( poFileData, sizeof(FileLruData<T> )*iMaxCnt, 1, fp  );
//    if( iw !=1 ){
//        ///没辙了
//        m_oDataLock.V();
//        delete [] poFileData;
//        return 0;
//    }
//
////////////////共享内存实际操作
//
//
//    unsigned int iWriteCntMem = 0;
//    for(int i=0; i< SHM_LRU_MAX_SESSION; i++){
//        unsigned int * pDirty = puiDirty+i;
//
//        while( *pDirty ){
//            
//            uitemp = m_pPointer[ *pDirty ].uiListIdxID;
//            uiTheNext = m_pPointer[ *pDirty ].uiListNext;
//            uiThePre  = m_pPointer[ *pDirty ].uiListPre;
//            
//            uitemp = m_pPointer[ *pDirty ].uiListLog;
//            if( uitemp ){
//                ////还未提交
//                pDirty = &m_pPointer[ *pDirty ].uiListNext;
//                iWriteCntMem++;
//                continue;
//            }else{
//
//                if( SHM_LRU_FLAG_DEL == m_pPointer[ *pDirty ].sFlag[0] ){
//                    delFromIndex(m_pPointer[ *pDirty ].uiListIdxID, 1);///改写了uiListIdxNext和uiListIdxID和sFlag和(*puiDelete)
//
//                    addToFree(*pDirty);
//                }else{
//
//                    m_pPointer[ *pDirty ].sFlag[0] = SHM_LRU_FLAG_NORM;
//                    addToNormal(*pDirty);
//                }
//
//                (*puiDirtyCnt)--;
//                
//            }
//
//            iWriteCntMem++;
//
//            *pDirty = uiTheNext;
//            if(uiTheNext)  m_pPointer[uiTheNext].uiListPre = uiThePre;
//            
//
//        }
//
//    }  
//
//
////////////////刷新文件头
//
//    writeFileHead(fp, oHead, 2);
//    uitemp = (*puiDelete);
//    
//    m_oDataLock.V();   
//    
//    if( iWriteCnt != iWriteCntMem
//        || uideleted != uitemp ){
//        ///哪个地方出问题了
//        printf("somewhere had problem,the memory and the file is not consistent, so sorry.\n");
//        
//    }
// 
//    delete [] poFileData;
//    poFileData = 0;
//    return iWriteCnt;
//    
//}

template <class T>
unsigned int SHMLruData<T>::writeFileBatch( FILE * fpHead , bool bAll)
{

    
    FileLruHead oHead;
    memset(&oHead, 0, sizeof(FileLruHead) );
    
    while(!m_oDataLock.P());

    if(!bAll && getWriteFileFlag() ){
        m_oDataLock.V();
		return 0;
	}
	setWriteFileFlag(1);

	if( !bAll ){
	    m_oDataLock.V();
        while(!m_oDataLock.P());	
    }



    if(m_iLastTotal != (*puiIterCnt)) {
        ////如果有做内存扩展
        SHMAccess::reopen();
        m_iLastTotal = (*puiIterCnt);

    }
        
    unsigned int iMaxCnt = (*puiMallocCnt);
    unsigned long ulBufSize = sizeof( FileLruData<T> )*iMaxCnt;
    unsigned int iBatchCnt = m_iCntPerFile;///SHM_LRU_BATCH_SIZE/sizeof( FileLruData<T> );
    unsigned int iBatchTimes = (iMaxCnt-1)/iBatchCnt + 1;
    
    unsigned int *pBatchInfo = new unsigned int[iBatchTimes];
    if(!pBatchInfo){
        printf("内存申请不到，大小：%lu 字节。\n", iBatchTimes*sizeof(unsigned int) );
        m_oDataLock.V();
        setWriteFileFlag(0);
        usleep(20000);
        return 0;
    }
    memset(pBatchInfo,0 , iBatchTimes*sizeof(unsigned int) );
    
    FileLruData<T> * poFileData = new FileLruData<T>[ iBatchCnt ];
    if(!poFileData){
        printf("内存申请不到，大小：%lu 字节。\n", iBatchCnt*sizeof(FileLruData<T>) );
        m_oDataLock.V();
        setWriteFileFlag(0);
        delete [] pBatchInfo;
        usleep(20000);
        return 0;
    }

/////    expandFile(fp);    
    
    unsigned int uitemp = 0;
    unsigned int uiFile = 0;
    unsigned int iDirty = 0;
    unsigned int uiTheNext=0;
    unsigned int uiThePre =0;
    unsigned int uideleted = (*puiDelete);
    unsigned int iWriteCnt = 0;   ////写到文件的条数
    unsigned int iWriteCntMem = 0;
        
    unsigned int iWCnt = 0;  /////目前批次的起始写的地方
    unsigned int iTimes = 0;

    FILE *fp=0;
    for(iTimes=0; iTimes<iBatchTimes; iTimes++  ){ 

        if( iTimes && (0==pBatchInfo[iTimes])  ){
            iWCnt += iBatchCnt;
            continue; 
        }
        
        if( !bAll && iTimes && ((*puiState)==2 )
            && ( getDirtyPercent() < SHM_LRU_DIRTY_LOW_LEVEL ) 
          ){
            ////非强制模式下，达到脏数据百分比的低值，不再进行落地
            break;
        }
        
        if( (iMaxCnt-iWCnt) < iBatchCnt  ){
            iBatchCnt = iMaxCnt-iWCnt;
        }

        fp=prepareFile(iTimes, false);
        if(!fp){
            m_oDataLock.V();
            setWriteFileFlag(0);
            delete [] poFileData;
            delete [] pBatchInfo;
            printf("文件%d打开失败\n", iTimes);
            return iWriteCnt;
        }
        
        ////fseek(fp, sizeof(FileLruHead) + iWCnt*sizeof(FileLruData<T> ) ,SEEK_SET);
        fseek(fp, 0, SEEK_SET);
        int i = fread( poFileData, sizeof(FileLruData<T> )*iBatchCnt, 1, fp  );
        if(i!=1){
            m_oDataLock.V();
            setWriteFileFlag(0);
            fclose(fp);
            delete [] poFileData;
            delete [] pBatchInfo;
            printf("fread文件%d失败\n", iTimes);
            return iWriteCnt;
        }

//////////////先在私有内存里面做好
    
    for(int i=0; i< SHM_LRU_MAX_SESSION; i++){
        
        iDirty = *(puiDirty+i);
        
        while( iDirty ){
            
            uitemp = m_pPointer[ iDirty ].uiListIdxID;
            uiTheNext = m_pPointer[ iDirty ].uiListNext;
//            uiThePre  = m_pPointer[ iDirty ].uiListPre;
            
            uiFile = uitemp-1;

            if(0==iTimes ){
                if( (uiFile/iBatchCnt)<iBatchTimes)
                    pBatchInfo[uiFile/iBatchCnt]++;
            }
            if( uiFile<iWCnt || uiFile>=(iWCnt+iBatchCnt ) ){
                ////不在范围之内的略过
                iDirty = uiTheNext;
                continue;
            }
            
            uiFile -=  iWCnt;
//            if(uitemp==2)
//                printf("2in file at %u\n", uiFile);
                
            uitemp = m_pPointer[ iDirty ].uiListLog;
            if( uitemp ){
                ////还未提交
                poFileData[uiFile].oData = m_pPointer[uitemp].oListData;
                if( SHM_LRU_FLAG_DEL ==  m_pPointer[ uitemp ].sFlag[0] ){
                    ////
                    
                }else{
                    poFileData[uiFile].uiNext = 0;

                }
                
            }else{
                poFileData[uiFile].oData = m_pPointer[iDirty].oListData;
                unsigned int iBig = m_pPointer[ iDirty ].uiListIdxID; 
                
                if( SHM_LRU_FLAG_DEL == m_pPointer[ iDirty ].sFlag[0] ){
                    unsigned int * pNext=(unsigned int *)(&m_pPointer[ iDirty ].oListData);
                    poFileData[uiFile].uiNext = (*pNext);

                }else{
                    poFileData[uiFile].uiNext = 0;
                }

            ///只统计提交的
            iWriteCnt++;
            //printf("mem:%u, big:%u\n",iDirty, uiFile+iWCnt+1);
                
            }


            iDirty = uiTheNext;

        }
        
    }

//////////////批量入文件
    ////fseek(fp, sizeof(FileLruHead) + iWCnt*sizeof(FileLruData<T> ) ,SEEK_SET);
    fseek(fp, 0, SEEK_SET);
    int iw = fwrite( poFileData, sizeof(FileLruData<T> )*iBatchCnt, 1, fp  );
    if( iw !=1 ){
        ///没辙了
        m_oDataLock.V();
        setWriteFileFlag(0);
        fclose(fp);
        delete [] poFileData;
        delete [] pBatchInfo;
        printf("fwrite文件%d失败\n", iTimes);
        return iWriteCnt;
    }
    fflush(fp);
    fclose(fp);
    fp=0;
//////////////共享内存实际操作


    for(int i=0; i< SHM_LRU_MAX_SESSION; i++){
         unsigned int * pDirty = puiDirty+i;

        while( *pDirty ){
            
            uitemp = m_pPointer[ *pDirty ].uiListIdxID;
            uiTheNext = m_pPointer[ *pDirty ].uiListNext;
            uiThePre  = m_pPointer[ *pDirty ].uiListPre;

            uiFile = uitemp-1;
            if( uiFile<iWCnt || uiFile>=(iWCnt+iBatchCnt ) ){
                ////不在范围之内的略过
                pDirty = &m_pPointer[ *pDirty ].uiListNext;
                continue;
            }

            
            uitemp = m_pPointer[ *pDirty ].uiListLog;
            if( uitemp ){
                ////还未提交
                pDirty = &m_pPointer[ *pDirty ].uiListNext;
                continue;
                
            }else{

                if( SHM_LRU_FLAG_DEL == m_pPointer[ *pDirty ].sFlag[0] ){
                    delFromIndex(m_pPointer[ *pDirty ].uiListIdxID, 0);
                    ///delFromIndex 原始传入1，现在传入0
                    ///传入1时改写了uiListIdxNext和uiListIdxID和sFlag和(*puiDelete)
                    ////传入0时没有改写(*puiDelete)
                    addToFree(*pDirty);
                }else{

                    m_pPointer[ *pDirty ].sFlag[0] = SHM_LRU_FLAG_NORM;
                    addToNormal(*pDirty);
                }

                (*puiDirtyCnt)--;
                
                iWriteCntMem++;                
               // printf("mem:%u, big:%u, -- %u\n",*pDirty, uiFile+1, (*(puiDirty+1)));
            }



            *pDirty = uiTheNext;
            if(uiTheNext)  m_pPointer[uiTheNext].uiListPre = uiThePre;

//            if(iWriteCntMem==iWriteCnt)
//                break;
        }

//        if(iWriteCntMem==iWriteCnt)
//            break;

    }  

    if(iWriteCnt!=iWriteCntMem){
        //
        printf("seq=%d,iWriteCnt=%u,iWriteCntMem=%u ,where is wrong?\n",
        iTimes, iWriteCnt,iWriteCntMem );
    }
//////////////刷新文件头

    writeFileHead(fpHead, oHead, 1);


    uitemp = (*puiDelete);
    
        

        iWCnt += iBatchCnt;    
    }

    writeFileHead(fpHead, oHead, 2);

    m_oDataLock.V();
    setWriteFileFlag(0);
    
//    if( uideleted != uitemp ){
//        ///哪个地方出问题了
//        printf("somewhere had problem,the memory and the file is not consistent, so sorry.\n");
//        
//    }
 
    delete [] poFileData;
    delete [] pBatchInfo;
    poFileData = 0;
    pBatchInfo = 0;
    return iWriteCnt;
    
}


template <class T>
FILE * SHMLruData<T>::prepareFile( int iTimes, bool bTrunc, bool bCreate)
{
    FILE *fp=0;
    if( !m_sFileName[0] ) 
        return 0;

    char sFileSeqName[SHM_LRU_FILENAME_LEN+8]={0};

    snprintf(sFileSeqName, sizeof(sFileSeqName)-1, 
      "%s.%04d", m_sFileName, iTimes );

    if(bTrunc){
        if(!bCreate){
            if( (fp = fopen( sFileSeqName,  "r" )) == NULL ){
                return 0;
            }
            fclose(fp);
            fp=0;
        }
        if( (fp = fopen( sFileSeqName,  "w" )) == NULL ){
            return 0;
        }
    }else{
        if( (fp = fopen( sFileSeqName,  "rb+" )) == NULL ){
            if(! bCreate )
                return 0;
            fp = fopen( sFileSeqName,  "ab+" );
            if(fp)
                fclose(fp);
            
            if( (fp = fopen( sFileSeqName,  "rb+" )) == NULL ){
                
                return 0;
            }            

        }
        
        if(SHMAccess::exist()){
            expandFile(fp);
        }
    }
    
    return fp;

}

template <class T>
void SHMLruData<T>::truncFile()
{   
    FILE *fp=0;

    initFile(true );

    for(int i=0; i<9999; i++){
        fp=prepareFile(i, true, false );
        if(!fp)
            break;
        fclose(fp);
        fp=0;
        
    }
}


template <class T>
unsigned int SHMLruData<T>::recoverFromFile()
{
    FILE * fp;
    
    if( (fp = fopen( m_sFileName,  "rb+" )) == NULL ){
        return 0;
    }

    fseek(fp, 0,  SEEK_END);
    long lSize = ftell(fp);

    
//    if( 0 != ( (lSize - sizeof(FileLruHead) )% sizeof(FileLruData<T>) )) {
//        fclose(fp);
//        return 0;
//    }
//    unsigned int uiTotal =  (lSize - sizeof(FileLruHead) ) / sizeof(FileLruData<T>);
//
//    if(0 == uiTotal){
//        fclose(fp);
//        return 0;
//    }
    
    fseek(fp, 0,  SEEK_SET);
    FileLruHead oHead;
    int i = fread(&oHead, sizeof(FileLruHead), 1, fp );

    if(i != 1 ){
        fclose(fp);
        return 0;
    }


    if( oHead.uiMallocCnt < oHead.uiRealCnt){
        fclose(fp);
        return 0;
    }
    
    if(1 == oHead.uiState){
        ////上次写文件没有正常退出
        ////没法从文件恢复,因为数据不能保证正确
        fclose(fp);
        printf("文件上次操作未成功，数据缺乏一致性，不进行恢复\n");
        THROW(MBC_SHMLruData+8);
    }


    if( oHead.uiVer!=(*puiVer)
        || oHead.lKey != m_lSHMKey
        || oHead.uiVer!= SHM_LRU_CURR_VERSION
        || oHead.uiContentSize!=sizeof(T) ){
        fclose(fp);    
        THROW(MBC_SHMLruData+8);
    }
    

    
    if( (*puiExpandCnt) != oHead.uiExpandCnt 
        || (*puiHashValue)!= oHead.uiHashValue
        || (*puiIterCnt)  != oHead.uiIterCnt ){
        /////内存做过扩展
        int i;///=m_iExpandCnt;
        int iexpand;
        if( oHead.uiRealCnt>(*puiIterCnt) )
            iexpand =( ( oHead.uiRealCnt-(*puiIterCnt)) * sizeof(LruUnitData) -1) /m_lNextSize + 1;
        else
            iexpand = oHead.uiExpandCnt;
            
        for(i=0; i< iexpand; i++){
            expandMem( false );
        }
        if(  (*puiExpandCnt) != oHead.uiExpandCnt 
            || (*puiHashValue)!= oHead.uiHashValue
            || (*puiIterCnt)  != oHead.uiIterCnt ){
            ////fclose(fp);
            printf("内存扩展参数和文件记录不一致, 将使用新的内存参数从文件初始数据...\n");
            ////THROW(MBC_SHMLruData+8);
        }
        
    }
    
    (*puiRealCnt)  = oHead.uiRealCnt;
    (*puiMallocCnt)= oHead.uiMallocCnt;
    (*puiDelete)  = oHead.uiDelete;
    
    unsigned int iCount = getTotal()/2;
    FileLruData<T> * pInitData = new FileLruData<T>[m_iCntPerFile] ;
    if(!pInitData){
        fclose(fp);    
        THROW(MBC_SHMLruData+12);
    }

    fclose(fp);
    fp=0;
    int iFileCnts = (oHead.uiMallocCnt-1)/m_iCntPerFile + 1;
    int iSeqCnt;
    const int LRU_INITLOAD_FILE_CNT=4;
    int iLoadSeq[LRU_INITLOAD_FILE_CNT];
    time_t Tmodify[LRU_INITLOAD_FILE_CNT];
    for(int iTmpp =0; iTmpp<LRU_INITLOAD_FILE_CNT; iTmpp++){
        iLoadSeq[iTmpp]=-1;
    }
    memset(Tmodify,0,sizeof(Tmodify));
    
    for(iSeqCnt=0; iSeqCnt<iFileCnts; iSeqCnt++ ){
        snprintf(m_sFileExName,sizeof(m_sFileExName)-1,
            "%s.%04d", m_sFileName, iSeqCnt);
        struct stat statbuf;
		if (stat (m_sFileExName, &statbuf) != 0){
		    THROW(MBC_SHMLruData+8);
		}
		if(statbuf.st_size != sizeof(FileLruData<T>)*m_iCntPerFile){
		    THROW(MBC_SHMLruData+8);
		}
		time_t TimeMin=statbuf.st_mtime;
		time_t TimeTmp;
		int iLoadSeqTemp;
		int iLoadSeqMin=iSeqCnt;
        for(int iTmpp =0; iTmpp<LRU_INITLOAD_FILE_CNT; iTmpp++){
		    if( TimeMin>Tmodify[iTmpp] ){
		        TimeTmp = Tmodify[iTmpp];
		        Tmodify[iTmpp]=TimeMin;
		        TimeMin = TimeTmp;
		        
		        iLoadSeqTemp=iLoadSeq[iTmpp];
		        iLoadSeq[iTmpp] = iLoadSeqMin;
		        iLoadSeqMin = iLoadSeqTemp;
		    }
		}
		 
    }    
    
    for(int iTmp =0; iTmp<LRU_INITLOAD_FILE_CNT; iTmp++){
        iSeqCnt=iLoadSeq[iTmp];
        if(iSeqCnt<0)
            continue;
            
        snprintf(m_sFileExName,sizeof(m_sFileExName)-1,
            "%s.%04d", m_sFileName, iSeqCnt);
        if( (fp = fopen( m_sFileExName,  "rb+" )) == NULL ){
            break;
        }
        
        i = fread( pInitData, sizeof(FileLruData<T>)*m_iCntPerFile, 1, fp );
        if( i!=1 ){
            delete [] pInitData;
            fclose(fp);    
            THROW(MBC_SHMLruData+8);
        }
        unsigned int iBigBase=iSeqCnt*m_iCntPerFile;
        for(iCount=0; iCount < m_iCntPerFile; iCount++){
            if( (iBigBase+iCount)>=oHead.uiMallocCnt)
                break;
            
            if(0 == (pInitData+iCount)->uiNext )
                getLruDataForRead(iBigBase+iCount+1, pInitData+iCount );
    
        }
    }
    
    delete [] pInitData;
    pInitData = 0;

    resetHitrateCalc();
    
    return iCount;

}


template <class T>
unsigned int SHMLruData<T>::expandMem( bool bTreatFile )
{
    while(!m_oDataLock.P());
    
    unsigned long lRet = SHMAccess::expandit();
    
    
    if( lRet ){
        /////已经成功扩展了 lRet个字节的空间
        ///新增的空间加到Free链表上面去，FreeCnt增加,puiIterCnt增加
        ///log链先不增加，
        ///

        unsigned int iExDataCnt = lRet/sizeof(LruUnitData);
        unsigned int iMax = (*puiIterCnt)+ 1 + (*puiLogTotalCnt);
        unsigned int iCnt = 0;
        unsigned int indexReal = 0;

        
        unsigned int iNewHash = (*puiHashValue)+iExDataCnt;
        unsigned int iOldHash = (*puiHashValue);
        
        iCnt=(*puiHashValue);
        for( ; iCnt<=(*puiMallocCnt); iCnt++){
            moveIndex(iCnt, iNewHash);
        }
        (*puiHashValue) = iNewHash;
        
        for(iCnt=0; iCnt<iExDataCnt; iCnt++){
            (*puiIterCnt)++;
            m_iLastTotal++;
            addToFree( iMax + iCnt);
        }

        ////if(bTreatFile) initFile(false, true);
        m_oDataLock.V();        
        return iExDataCnt;
    }
    m_oDataLock.V();
    return 0;
    
}

template <class T>
unsigned int SHMLruData<T>::moveIndex(unsigned int indexBig, unsigned int uiNewHash)
{
    unsigned int *pPre = &m_pPointer[(indexBig-1)%(*puiHashValue)+1].uiIndex;
    unsigned int indexReal = *pPre;
    
    while(indexReal){
        if( m_pPointer[indexReal].uiListIdxID  == indexBig ){
            if(pPre){
                (*pPre)= m_pPointer[indexReal].uiListIdxNext;
            }
            unsigned int indexNew = m_pPointer[(indexBig-1)%uiNewHash + 1].uiIndex;
            if(indexNew
              && m_pPointer[indexNew].uiListIdxID  == indexBig ){
                return indexNew;
            }
            if( indexNew == indexReal 
              && 0==m_pPointer[indexNew].uiListIdxID ){
                return indexNew;
            }
            m_pPointer[indexReal].uiListIdxNext = indexNew;
            m_pPointer[(indexBig-1)%(uiNewHash)+1].uiIndex = indexReal;
            return indexNew;
        }
        pPre = &m_pPointer[indexReal].uiListIdxNext;
        indexReal = m_pPointer[indexReal].uiListIdxNext;

    }

    return 0;

}


#endif
////SHMLRUDATA_H_HEADER_INCLUDED
 

 
