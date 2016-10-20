/*VER: 1*/ 
// Copyright (c) 2009,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMIntHashIndex_AX.h"
#include "MBC.h"
#include <stdio.h>
#include <iostream.h>

///#define DEBUG_TEST


////zhangap 可循环使用数据区的索引类，使用SHMIntList里面固有的next，不增加内存开销
////增加版本记录，这个已经和 SHMIntHashIndex不兼容了
//// 

//##ModelId=424426D6039B
SHMIntHashIndex_AX::SHMIntHashIndex_AX(char *shmname ) :
SHMAccess(shmname ), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
#ifdef SHM_EXPAND_GLOBAL_POS
    m_poList.m_pShm = this;
#endif

    if (m_poSHM){
        open ();
        if( 23 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }

    char sTemp[64];
    int  iTemp = strlen(shmname)>9 ? (strlen(shmname)-9 ) : 0;
    snprintf(sTemp, sizeof(sTemp), "%u", atoi(&shmname[iTemp])+9);
    m_oDataLock.getSem (sTemp, 1, 1);
    
}


//##ModelId=424426D603B9
SHMIntHashIndex_AX::SHMIntHashIndex_AX(long shmkey, long extSize, int iMaxExtCnt) :
SHMAccess(shmkey, extSize, iMaxExtCnt), m_piHashValue(0), m_piTotal(0), m_piUsed(0), m_poHash(0)
{
#ifdef SHM_EXPAND_GLOBAL_POS
    m_poList.m_pShm = this;
#endif
    if (m_poSHM){
        open ();
        if( 23 != *m_piVersion )
            THROW(MBC_SHMData+1);
    }

    char sTemp[64];
    snprintf(sTemp, sizeof(sTemp), "%ld", shmkey+9 );
    m_oDataLock.getSem (sTemp, 1, 1);  
}


//##ModelId=424426D603D7
SHMIntHashIndex_AX::~SHMIntHashIndex_AX()
{
}


unsigned int SHMIntHashIndex_AX::malloc()
{

    if(m_iLastTotal != (*m_piTotal)) {
        SHMAccess::reopen();
        m_iLastTotal = (*m_piTotal);
    }
    if( ( getCount()*100/getTotal() ) > 98 ){
        expandMem();
    }

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

    #ifdef  DEBUG_TEST
    printf("分配到偏移为%u。     ", j);
    #endif    
    return j;
}


//##ModelId=42442A29038B
void SHMIntHashIndex_AX::add(long key, unsigned int value)
{
    if(m_iLastTotal != (*m_piTotal)) {
        SHMAccess::reopen();
        m_iLastTotal = (*m_piTotal);
    }

m_oDataLock.P();
    
    unsigned int i = (key & (*m_piHashValue));
    unsigned int j;
    
    if (m_poHash[i]) {
        j = m_poHash[i];
        while (j) {
            if (m_poList[j].m_lKey == key) {
                m_poList[j].m_iValue = value;
                m_oDataLock.V();
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
m_oDataLock.V();

    
}

//##ModelId=42442A5B0134
bool SHMIntHashIndex_AX::get(long key, unsigned int *pvalue)
{
    if(m_iLastTotal != (*m_piTotal)) {
        SHMAccess::reopen();
        m_iLastTotal = (*m_piTotal);
    }

m_oDataLock.P();

    unsigned int i = (key & (*m_piHashValue));
    unsigned int j;

    *pvalue = 0;
    
    if (!(j=m_poHash[i])){
        m_oDataLock.V();
        return false;
    }

    while (j) {
        if (m_poList[j].m_lKey == key) {
            *pvalue = m_poList[j].m_iValue;
            m_oDataLock.V();
            return true;
        }
        
        j = m_poList[j].m_iNext;
    }

m_oDataLock.V();
    return false;
}


void SHMIntHashIndex_AX::reset()
{
    empty();
}

//##ModelId=42442B02030B
void SHMIntHashIndex_AX::create(unsigned int itemnumber) 
{
    //##计算HASHVALUE
    unsigned int hashvalue;
    hashvalue = (unsigned int)(itemnumber*1.25); ///1.25用来预留保证扩展后的效率
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
                (itemnumber+1)*sizeof(SHMIntList_AX) + \
                32*sizeof(unsigned int);
    SHMAccess::create (size);

    open ();

    *m_piHashValue = hashvalue;
    *m_piTotal = itemnumber;
    *m_piUsed = 1;

    *m_piHead  = 1;
    *m_piTail  = *m_piTotal;
    *m_piVersion = 23;

    *(m_piHashValue+1) = 1; ///老版本对应的总量置1
    *(m_piHashValue+2) = 2; ///老版本对应的已使用量置2，置2表示使用数为2-1


#ifdef SHM_EXPAND_GLOBAL_POS
//    m_poList.m_pShm = this;
    m_pListUnit= (SHMIntList_AX *)(m_piHashValue + 31 + (*m_piHashValue) );
    *m_puiSegment = itemnumber;
    SHMAccess::m_sSegment[0] = (char *)m_pListUnit;

#else
    m_poList = (SHMIntList_AX *)(m_piHashValue + 31 + (*m_piHashValue) );
#endif



    for(unsigned int ii=*m_piHead;ii<*m_piTail;ii++)
    {
        m_poList[ii].m_iNext = ii+1;
    }
    m_poList[*m_piTail].m_iNext = 0;

    m_iLastTotal = *m_piTotal;
    *m_piExpandCnt = 0;
}

//##ModelId=4244D04101A3
void SHMIntHashIndex_AX::open()
{

//    m_piHashValue = (unsigned int *)((char *)(*m_poSHM)) + 1;
//    m_piTotal = m_piHashValue + 1;
//    m_piUsed = m_piTotal + 1;
//    m_poHash = m_piUsed + 1;
//    m_poList = (SHMIntList *)(m_poHash + (*m_piHashValue) + 1) - 1;
//以上是老版本的信息    

    m_piHashValue = (unsigned int *)((char *)(*m_poSHM)) + 1;

    m_piVersion = m_piHashValue - 1;
    ////m_piTotal = m_piHashValue + 1;////老的地方放置让其满
    ////m_piUsed = m_piHashValue + 2;////老的地方放置让其满


    m_piTotal = m_piHashValue + 3;
    m_piUsed =  m_piHashValue + 4;

    m_piHead =      m_piHashValue + 5;
    m_piTail =      m_piHashValue + 6;
    m_piDelOver =   m_piHashValue + 7;
    m_piExpandCnt = m_piHashValue + 8;
    m_puiSegment =  m_piHashValue + 9;////后面的地方存放段的划分信息。
    m_poHash =      m_piHashValue + 23;


#ifdef SHM_EXPAND_GLOBAL_POS
    ///m_poList;
    m_pListUnit= (SHMIntList_AX *)(m_piHashValue + 31 + (*m_piHashValue) );
    SHMAccess::initExpand( m_piExpandCnt, sizeof(SHMIntList_AX), m_puiSegment);

    *m_puiSegment = (*m_piTotal);
    SHMAccess::m_sSegment[0] = (char *)m_pListUnit;

#else
    m_poList = (SHMIntList_AX *)(m_piHashValue + 31 + (*m_piHashValue) );
#endif

    m_iLastTotal = *m_piTotal;	
}

void SHMIntHashIndex_AX::empty()
{
m_oDataLock.P();

    memset (m_poHash, 0, sizeof (unsigned int)*(*m_piHashValue+1));

#ifdef SHM_EXPAND_GLOBAL_POS
    memset (m_pListUnit, 0, sizeof (SHMIntList_AX)*(*m_puiSegment));
    SHMAccess::setExpandNull();
#else
    memset (m_poList, 0, sizeof (SHMIntList_AX)*(*m_piTotal+1));
#endif

    *m_piUsed = 1;
    *m_piHead = 1;
    *m_piTail = *m_piTotal;
    for(unsigned int ii=*m_piHead;ii<*m_piTail;ii++)
    {
        m_poList[ii].m_iNext = ii+1;
    }
    m_poList[*m_piTail].m_iNext = 0;

    *m_piDelOver = 0;
    
m_oDataLock.V();
}

//删除
bool SHMIntHashIndex_AX::revokeIdx(long key, unsigned int &value)
{
    if(m_iLastTotal != (*m_piTotal)) {
        SHMAccess::reopen();
        m_iLastTotal = (*m_piTotal);
    }

m_oDataLock.P();

    unsigned int i = (key & (*m_piHashValue));
    unsigned int j;


    if (!(j=m_poHash[i])){
        m_oDataLock.V();
        return false;
    }

    unsigned int * puiPrePosition = &m_poHash[i];
    
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
            m_oDataLock.V();
            return true;
        }
        else
        {

            puiPrePosition = &m_poList[j].m_iNext;
            j = m_poList[j].m_iNext;
        }
    }
    
    m_oDataLock.V();
    return false;
}


void SHMIntHashIndex_AX::showDetail()
{
    if(m_iLastTotal != (*m_piTotal)) {
        SHMAccess::reopen();
        m_iLastTotal = (*m_piTotal);
    }


    char sText[256];
    char keys[64];
    if(m_sSHMName) {
        strncpy(keys, m_sSHMName, sizeof(keys));
    }else{
        sprintf(keys, "%ld", m_lSHMKey);
    }
    if( !m_poSHM ){
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "I_INDEX_AX", sizeof(SHMIntList_AX), 0, 0, 0 );
    }else{
    char * pp = (char * )(*m_poSHM);
    unsigned int uiTotal = getTotal();
    unsigned int uiCount = getCount();
    int percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-10lx %2u %-12s %-9d %-10u %-10u %4u%%",
        keys, pp,(*m_piVersion), "I_INDEX_AX", sizeof(SHMIntList_AX), uiTotal, uiCount, percent );
    }
    cout<< sText<<endl;

    SHMAccess::showExpandInfo();
}



unsigned int SHMIntHashIndex_AX::expandMem()
{
m_oDataLock.P();

    unsigned long lRet = SHMAccess::expandit();
    
    if( lRet ){
        /////已经成功扩展了 lRet个字节的空间
        ///新增的空间加到Free链表上面去，totalCnt增加
        ///链先不增加，

        unsigned int iExDataCnt = lRet/sizeof(SHMIntList_AX);
        unsigned int iCnt = 0;

        unsigned int iNewHash = (*m_piHashValue)+iExDataCnt;
        unsigned int iOldHash = (*m_piHashValue);
        
//切换hashvalue 因为要涉及到预留hash数组空间，暂不切换hashvalue
//        for( iCnt=1; iCnt<=(*m_piTotal ); iCnt++){
//            moveIndex(iCnt, iNewHash);
//        }
//        (*m_piHashValue) = iNewHash;

//准备好新增空间        
        for(iCnt=(*m_piTotal)+1; iCnt<=(*m_piTotal + iExDataCnt) ; iCnt++){
            m_poList[iCnt].m_iNext = iCnt+1;
        }
        m_poList[iCnt-1].m_iNext = 0;///最后一个是0

///新增空间加入空闲链表        
        if(*m_piTail ){
            m_poList[*m_piTail].m_iNext = (*m_piTotal)+1;
            (*m_piTail) = (*m_piTotal + iExDataCnt);
        }else{
            (*m_piHead) = (*m_piTotal)+1;
            (*m_piTail) = (*m_piTotal + iExDataCnt);
        }

        *m_piTotal = (*m_piTotal + iExDataCnt);

//#ifdef  DEBUG_TEST
printf("第%d次扩展，本次新增%u个数据项。", (*m_piExpandCnt), iExDataCnt );
//#endif

        m_oDataLock.V();
        return iExDataCnt;
    }

    m_oDataLock.V();
    return 0;
}



