/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMStringTreeIndex_AX.h"
#include <string.h>
#include "MBC.h"
#include "Stack.h"
#include <stdio.h>
//#include <iostream.h>

#define DEBUG_TEST

//##ModelId=424429AB039E
SHMStringTreeIndex_AX::SHMStringTreeIndex_AX(char *shmname ) :
SHMAccess(shmname ), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
#ifdef SHM_EXPAND_GLOBAL_POS
    m_poTree.m_pShm = this;
    memset(m_iListEndSegment, 0, sizeof(m_iListEndSegment) );
#endif

	if (m_poSHM){
		open ();
        if( 24 != *m_piVersion )
            THROW(MBC_SHMData+1);

#ifdef SHM_EXPAND_GLOBAL_POS
    initListSeg();
#endif    

    }

    char sTemp[64];
    int  iTemp = strlen(shmname)>9 ? (strlen(shmname)-9 ) : 0;
    snprintf(sTemp, sizeof(sTemp), "%u", atoi(&shmname[iTemp])+9);
    m_oDataLock.getSem (sTemp, 1, 1);
}


//##ModelId=424429AB03BC
SHMStringTreeIndex_AX::SHMStringTreeIndex_AX(long shmkey, long extSize, int iMaxExtCnt) :
SHMAccess(shmkey, extSize, iMaxExtCnt), m_piListTotal(0), m_piListUsed(0), m_piTreeTotal(0),
m_piTreeUsed(0), m_piKeyLen(0)
{
#ifdef SHM_EXPAND_GLOBAL_POS
    m_poTree.m_pShm = this;
    memset(m_iListEndSegment, 0, sizeof(m_iListEndSegment) );
#endif

	if (m_poSHM){
		open ();
        if( 24 != *m_piVersion )
            THROW(MBC_SHMData+1);

#ifdef SHM_EXPAND_GLOBAL_POS
    initListSeg();
#endif

    }

    char sTemp[64];
    snprintf(sTemp, sizeof(sTemp), "%ld", shmkey+9 );
    m_oDataLock.getSem (sTemp, 1, 1);

}


//##ModelId=424429AC001A
SHMStringTreeIndex_AX::~SHMStringTreeIndex_AX()
{
}


#ifdef SHM_EXPAND_GLOBAL_POS

void SHMStringTreeIndex_AX::initListSeg()
{
m_oDataLock.P();
        char * pStart = (char *)(*m_poSHM);
        char * pEnd = pStart+ m_poSHM->size();
        if( m_poList >pStart && m_poList< pEnd ){
            ///后面的都是list数据
            m_iListEndSegment[0] = (pEnd-m_poList)/(*m_piListLen) ;
            for(int i=1; i<=(*m_piExpandCnt); i++){
                
                m_iListEndSegment[i] = m_poExSHM[i-1]->size()/(*m_piListLen);
                m_iListEndSegment[i] += m_iListEndSegment[i-1];
            }
        }else{
            ////list已经移动到扩展段去了
            for(int i=1; i<=(*m_piExpandCnt); i++){
                m_iListEndSegment[i] = m_poExSHM[i-1]->size()/(*m_piListLen);
                m_iListEndSegment[i] += m_iListEndSegment[i-1];
            }
        }

m_oDataLock.V();
}
#endif

//##ModelId=42442A8B0093
void SHMStringTreeIndex_AX::add(char *key, unsigned int value)
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }
    
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_AX *pl;

m_oDataLock.P();
	
	for(i=0; i<SHM_STRING_KEY_LEN_AX && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
		} else {
		    m_poTree[index].m_iNext[j] = mallocTree();
		    index = m_poTree[index].m_iNext[j];

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
			m_oDataLock.V();
			return ;
		}
		pl = getList (pl->m_iNext);
	}		

__ADD:
    
    j = mallocList();
	pl = getList(j);
	pl->m_iNext = m_poTree[index].m_iValue;
	m_poTree[index].m_iValue = j;
	pl->m_iValue = value;
	memcpy (pl->m_sKey, key, strlen(key));
    pl->m_sKey[strlen(key)] = 0;
m_oDataLock.V();
	return;
}

bool SHMStringTreeIndex_AX::getMax(char *key, unsigned int *pvalue)
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }

    *pvalue =0;
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_AX *pl;

    Stack<int> stack;

m_oDataLock.P();
	
	for(i=0; i<SHM_STRING_KEY_LEN_AX && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
            stack.push (index);
		} else {
            break;
		}
	}

    // SHM_STRING_KEY_LEN_AX=12 
    //所以不考虑区号超过12位的精确匹配问题
    //这个是个隐患，以后出现超过12位的，调整参数或者优化算法

    while (stack.pop (index)) {
        j = m_poTree[index].m_iValue;
        if (j) {
            pl = getList (j);
            while (pl ) {
                if (!strncmp(pl->m_sKey, key, i)) {
                    *pvalue = pl->m_iValue ;
                    m_oDataLock.V();
                    return true;
                }
                pl = getList (pl->m_iNext);
            }		
        }

        i--;
    }
m_oDataLock.V();
    return false;
}


//##ModelId=42442A9101C8
bool SHMStringTreeIndex_AX::get(char *key, unsigned int *pvalue)
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }
    
	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_AX *pl;
	
	*pvalue = 0;

m_oDataLock.P();
	
	for(i=0; i<SHM_STRING_KEY_LEN_AX && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
			index = m_poTree[index].m_iNext[j];
		} else {
		    m_oDataLock.V();
			return false;
		}
	}

	if (!m_poTree[index].m_iValue){ 
	    m_oDataLock.V();
	    return false;
	}


	j = m_poTree[index].m_iValue;
	pl = getList (j);
	while (pl ) {
		if (!strncmp(pl->m_sKey, key, *m_piKeyLen)) {
			*pvalue = pl->m_iValue ;
			m_oDataLock.V();
			return true;
		}
		pl = getList (pl->m_iNext);
	}
	m_oDataLock.V();
	return false;
}

//##ModelId=42442B0802C3
void SHMStringTreeIndex_AX::create(unsigned int itemnumber, unsigned int keylen, unsigned int multiple)
{
	unsigned long size1, size2, size3;
	unsigned int iListLen;
	unsigned int iKeyLen = keylen;

	//##计算SHMStringList的长度，内存按照8个字节对齐。
	if (keylen <= 8) {
		iListLen = sizeof(SHMStringList_AX);
	} else { 
		iListLen = sizeof(SHMStringList_AX) + (keylen&(~7u));
	}

	//##SHMStringList所占的长度
	size1 = iListLen * itemnumber;
	
	//##SHMStringTree所占的长度
	size2 = sizeof(SHMStringTree_AX)*itemnumber*multiple;

	size3 = size1 + size2 + sizeof(unsigned int)*24;
	
	SHMAccess::create (size3);
	open ();
	
	*m_piListLen = iListLen;
	*m_piKeyLen = iKeyLen;
	*m_piListTotal = itemnumber;
	*m_piTreeTotal = itemnumber*multiple;
	*m_piListUsed = 1;
	*m_piTreeUsed = 1; 


#ifdef SHM_EXPAND_GLOBAL_POS

    m_poTreeUnit = (SHMStringTree_AX *) (m_piListTotal + 23);
	m_poList = (char *)(((unsigned long)(char *)(m_poTreeUnit+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;

    *m_puiSegment = itemnumber*multiple;
    SHMAccess::m_sSegment[0] = (char *)m_poTreeUnit;			
#else
    m_poTree = (SHMStringTree_AX *) (m_piListTotal + 23);
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;

#endif


    *m_piListHead  = 1;
    *m_piListTail  = *m_piListTotal;
    *m_piTreeHead  = 1;
    *m_piTreeTail  = (*m_piTreeTotal)-1;
    *m_piVersion = 24;

    SHMStringList_AX * pList = 0;
    
    for(unsigned int ii=*m_piListHead;ii<*m_piListTail;ii++)
    {
        pList = getList(ii);
        pList->m_iNext = ii+1;
    }
    pList = getList(*m_piListTail);
    pList->m_iNext = 0;


    for(unsigned int ii=*m_piTreeHead;ii<*m_piTreeTail;ii++)
    {
        m_poTree[ii].m_iNext[0] = ii+1;
    }
    m_poTree[(*m_piTreeTail)].m_iNext[0] = 0;

    *m_piExpandCnt=0;
    m_iLastExpand = (*m_piExpandCnt);
}

//##ModelId=4244EBC6017B
void SHMStringTreeIndex_AX::open()
{
	m_piListTotal = ((unsigned int *)((char *)(*m_poSHM))) + 1;

    m_piVersion   = m_piListTotal - 1 ;
	m_piListUsed  = m_piListTotal + 1;
	m_piTreeTotal = m_piListTotal + 2;
	m_piTreeUsed  = m_piListTotal + 3;
	m_piKeyLen  =   m_piListTotal + 4;
	m_piListLen =   m_piListTotal + 5;

    m_piTreeHead = m_piListTotal+6 ;
    m_piTreeTail = m_piListTotal+7;
    m_piListHead = m_piListTotal+8;
    m_piListTail = m_piListTotal+9;
    
    m_puiSegment = m_piListTotal+10;
    ////uisegment要预留出9个int的空间
    m_piExpandCnt = m_piListTotal+20;

    SHMAccess::initExpand( m_piExpandCnt, sizeof(SHMStringTree_AX), m_puiSegment);

#ifdef SHM_EXPAND_GLOBAL_POS

    m_poTreeUnit = (SHMStringTree_AX *) (m_piListTotal + 23);
	m_poList = (char *)(((unsigned long)(char *)(m_poTreeUnit+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;

    SHMAccess::m_sSegment[0] = (char *)m_poTreeUnit;

#else

////m_poTree 和老版本不兼容了  m_poList会因为内存扩展而变动
	m_poTree = (SHMStringTree_AX *) (m_piListTotal + 23);
	m_poList = (char *)(((unsigned long)(char *)(m_poTree+*m_piTreeTotal)+8ul) & (~7ul)) \
			- (*m_piListLen) ;
#endif

    m_iLastExpand = (*m_piExpandCnt);
}

//##ModelId=424512FC000C
SHMStringList_AX *SHMStringTreeIndex_AX::getList(unsigned int offset)
{
	if (!offset)
		return 0;

#ifdef SHM_EXPAND_GLOBAL_POS

    
    if( offset <= m_iListEndSegment[0] ){
        return (SHMStringList_AX *)(m_poList + (*m_piListLen)*offset);
    }
    char *pBase;
    for(int i=1; i<MAX_SHM_EX_CNT+1; i++){
        if( offset <= m_iListEndSegment[i]  ){
            pBase =  (m_sSegment[i]);
            offset -= m_iListEndSegment[i-1];
            return (SHMStringList_AX *)(pBase + (*m_piListLen)*(offset-1) );
        }

    }
 
#else		
	return (SHMStringList_AX *)(m_poList + (*m_piListLen)*offset);

#endif

    return 0;

}




////zhangap 增加索引循环使用的功能 
unsigned int SHMStringTreeIndex_AX::mallocList( )
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }
    if( ( getlCount()*100/getlTotal() ) > 98 ){
        expandMem();
    }



    unsigned int j=0;
    if (*m_piListUsed <= *m_piListTotal) {
        (*m_piListUsed)++;
        j = *m_piListHead;
        
        if( *m_piListUsed == 1+(*m_piListTotal) ){
            *m_piListHead = 0;
            *m_piListTail = 0;
        }else{
            SHMStringList_AX * pList = getList(j);
            *m_piListHead = pList->m_iNext;
            pList->m_iNext = 0;
        }
        

    } else {
        //##空间不够
        THROW(MBC_SHMIntHashIndex+1);
    }
//    #ifdef  DEBUG_TEST
//    printf("分配到List偏移为%u。     ", j);
//    #endif        
    return j;
}


unsigned int SHMStringTreeIndex_AX::mallocTree()
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }
    if( ( getCount()*100/getTotal() ) > 98 ){
        expandMem();
    }

    unsigned int j=0;    
    if (*m_piTreeUsed < *m_piTreeTotal) {
        (*m_piTreeUsed)++;
        j = *m_piTreeHead;
        
        if( *m_piTreeUsed == (*m_piTreeTotal) ){
            *m_piTreeHead = 0;
            *m_piTreeTail = 0;
        }else{
            *m_piTreeHead = m_poTree[j].m_iNext[0];
            m_poTree[j].m_iNext[0] = 0;
        }
        

    } else {
        //##空间不够
        THROW(MBC_SHMIntHashIndex+1);
    }
//    #ifdef  DEBUG_TEST
//    printf("分配到Tree偏移为%u。     ", j);
//    #endif          
    return j;
}


//删除
///输入 key，根据key找到要删除的那个，然后删除，返回的是这个key所记录的value
bool SHMStringTreeIndex_AX::revokeIdx(char * key, unsigned int &value)
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }

	int i, j;
	char *p = key;
	int index = 0;
	SHMStringList_AX *pl;
	

	SHMPreTree_AX m_pPre[SHM_STRING_KEY_LEN_AX];
	int  m_iTreePre;

m_oDataLock.P();		
	for(i=0; i<SHM_STRING_KEY_LEN_AX && *p; p++,i++) {
		j = (*p)%10;
		if (m_poTree[index].m_iNext[j]) {
		    m_pPre[i].pTree = &m_poTree[index];
		    m_pPre[i].iOffset = j;
			index = m_poTree[index].m_iNext[j];
		} else {
		    m_oDataLock.V();
			return false;
		}
	}

	if (!m_poTree[index].m_iValue){ 
	    m_oDataLock.V();
	    return false;
	}

    m_iTreePre = i-1;
    
	unsigned int * puiPrePosition = &m_poTree[index].m_iValue;

	j = m_poTree[index].m_iValue;
	pl = getList (j);
	while (pl ) {
		if (!strncmp(pl->m_sKey, key, *m_piKeyLen)) {
		    j = *puiPrePosition;
			value = pl->m_iValue ;
			*puiPrePosition = pl->m_iNext;
			pl->m_sKey[0]='\0';
			memset(pl->m_sKey, 0, *m_piKeyLen );
			pl->m_iNext=0;
			pl->m_iValue=0;
			
            if( *m_piListUsed <= *m_piListTotal ){
                //索引空间未用完
                SHMStringList_AX * pList = getList(*m_piListTail);
                pList->m_iNext = j;
            }else{
                *m_piListHead = j;
            }
            *m_piListTail = j;
            (*m_piListUsed)--;
			
            j = m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ];
			if(m_iTreePre && 0 == m_poTree[ j ].m_iValue ){

			    SHMStringTree_AX treeTemp;
			    memset(&treeTemp, 0, sizeof(SHMStringTree_AX) );
			    
			    while(0==memcmp(& m_poTree[j], &treeTemp, sizeof(SHMStringTree_AX)) ){
			        ////从最底层的Tree，一直往上删

                    m_pPre[m_iTreePre].pTree->m_iNext[ m_pPre[m_iTreePre].iOffset ] = 0;
                    if( *m_piTreeUsed < *m_piTreeTotal ){
                        //索引空间未用完
                        m_poTree[*m_piTreeTail].m_iNext[0] = j;
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
			m_oDataLock.V();
			return true;
		}
		pl = getList (pl->m_iNext);

		puiPrePosition = &pl->m_iNext;
	}
	
	m_oDataLock.V();
	return false;

}




void SHMStringTreeIndex_AX::empty()
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }

m_oDataLock.P();

#ifdef SHM_EXPAND_GLOBAL_POS
    memset (m_poTreeUnit, 0, (sizeof(SHMStringTree_AX) ) *  (*m_puiSegment ) );
    char * pEnd =  (char *)(*m_poSHM) ;
    pEnd += m_poSHM->size();
    if( (char *)( m_poTreeUnit + (*m_puiSegment ) )  < pEnd 
       && pEnd > m_poList  ){
        
        memset (m_poList, 0,  (pEnd - m_poList)  );
    }
    
    SHMAccess::setExpandNull();
#else

    memset (m_poTree, 0, (sizeof(SHMStringTree_AX) ) *  (*m_piTreeTotal) );
    memset (m_poList, 0, (*m_piListLen) * ((*m_piListTotal)-1) );

#endif

	*m_piListUsed = 1;
	*m_piTreeUsed = 1;

    *m_piListHead  = 1;
    *m_piListTail  = *m_piListTotal;
    *m_piTreeHead  = 1;
    *m_piTreeTail  = (*m_piTreeTotal)-1;


    SHMStringList_AX * pList = 0;
    
    for(unsigned int ii=*m_piListHead;ii<*m_piListTail;ii++)
    {
        pList = getList(ii);
        pList->m_iNext = ii+1;
    }
    pList = getList(*m_piListTail);
    pList->m_iNext = 0;


    for(unsigned int ii=*m_piTreeHead;ii<*m_piTreeTail;ii++)
    {
        m_poTree[ii].m_iNext[0] = ii+1;
    }
    m_poTree[(*m_piTreeTail)].m_iNext[0] = 0;
    
m_oDataLock.V();
    
}


void SHMStringTreeIndex_AX::showDetail()
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }

    char sText[256];
    char keys[64];
    if(m_sSHMName) {
        strncpy(keys, m_sSHMName, sizeof(keys));
    }else{
        sprintf(keys, "%ld", m_lSHMKey);
    }

    if( !m_poSHM ){
    snprintf(sText, sizeof(sText), "%-10s 0x%-15lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "S_IDX_AX_LIST",sizeof(SHMStringList_AX) , 0, 0, 0 );
    //cout<< sText<<endl;
    snprintf(sText, sizeof(sText), "%-10s 0x%-15lx %2u %-12s %-9d %-10u %-10u %4d%%",
        keys, 0,(*m_piVersion), "S_IDX_AX_TREE", sizeof(SHMStringTree_AX),0, 0,0 );
    //cout<< sText<<endl;

    }else{
    char * pp = (char * )(*m_poSHM);
    unsigned int uiTotal = getlTotal();
    unsigned int uiCount = getlCount();
    int percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-15lx %2u %-12s %-9d %-10u %-10u %4u%%",
        keys, pp,(*m_piVersion), "S_IDX_AX_LIST", (*m_piListLen), uiTotal, uiCount, percent);
    //cout<< sText<<endl;

    uiTotal = getTotal();
    uiCount = getCount();
    percent = 100*uiCount/uiTotal;
    if( (100*uiCount)%uiTotal )
        percent++;
    snprintf(sText, sizeof(sText), "%-10s 0x%-15lx %2u %-12s %-9d %-10u %-10u %4u%%",
        keys, pp,(*m_piVersion), "S_IDX_AX_TREE", sizeof(SHMStringTree_AX),  uiTotal, uiCount, percent);
    //cout<< sText<<endl;    
    }

    SHMAccess::showExpandInfo();
}



unsigned int SHMStringTreeIndex_AX::expandMem()
{
    if(m_iLastExpand != (*m_piExpandCnt)) {
        SHMAccess::reopen();
        #ifdef SHM_EXPAND_GLOBAL_POS
        initListSeg();
        #endif
        m_iLastExpand = (*m_piExpandCnt);
    }

m_oDataLock.P();

    unsigned long lRet = SHMAccess::expandit();
    
    if( lRet ){
        /////已经成功扩展了 lRet个字节的空间

        /////计算目前的 tree 和 list 的空间占比
        /////扩展后，移动后，让空闲空间保持这个占比
        unsigned long lTreeSize = getCount()*sizeof(SHMStringTree_AX);
        unsigned long lListSize = getlCount()*( *m_piListLen );
        
        unsigned long ulNewSize =  lTreeSize+lListSize+lRet;
        int iListPecent = lListSize*100/(lTreeSize+lListSize);
        
        
        unsigned long lNewListSize = ulNewSize/100 * iListPecent;
        unsigned long lNewTreeSize = ulNewSize - lNewListSize ;
        
        unsigned int iNewListCnt = lNewListSize/( *m_piListLen );
        unsigned int iNewTreeCnt = lNewTreeSize/sizeof(SHMStringTree_AX);
        
        unsigned int iExDataCnt = 0;
        /////搬移List数据，给tree腾空间



    if( iNewTreeCnt < ( getTotal() + 256)  ){
        ////说明创建时Tree开得比较大 因此新增空间全部给list，tree就不动他，也不去减少tree
        iNewTreeCnt = getTotal();
        iNewListCnt = getlTotal() + lRet/( *m_piListLen ) ;
    
        iExDataCnt = 0 + iNewListCnt-getlTotal();
    ////list 原来的数据不动，直接增加空闲数据即可
        
    
    }else{

        ////list 需要做移动操作
        
        if( iNewListCnt < ( getlTotal() ) ) 
        {
            ////说明创建时list开得比较大
            iNewListCnt = getlTotal();
            iNewTreeCnt = getTotal() + lRet/sizeof(SHMStringTree_AX) ;
        
        }
        iExDataCnt = iNewTreeCnt-getTotal()+ iNewListCnt-getlTotal() ;

#ifdef SHM_EXPAND_GLOBAL_POS
        ( iNewTreeCnt - getTotal() )*sizeof(SHMStringTree_AX) ;
        long lcnt = ( iNewTreeCnt - getTotal() )*sizeof(SHMStringTree_AX) / (*m_piListLen);
        lcnt++;
        char * m_poNewList = m_poList + ( lcnt )*(*m_piListLen) ;
        
        int i;
        ////让list可以使用扩展出来的空间
        i = (*m_piExpandCnt);
        if(i){
            m_iListEndSegment[i] = m_poExSHM[i-1]->size()/(*m_piListLen);
            m_iListEndSegment[i] += m_iListEndSegment[i-1];
        }

        for(i=getlTotal(); i>0; i--){
            ///往高处移动，先移动高处的数据
            SHMStringList_AX *psrc = getList(i );
            SHMStringList_AX *pdst = getList(i+lcnt );
            memcpy( pdst,  psrc,  (*m_piListLen) );
        }
        m_poList = m_poNewList;
        initListSeg();
#else    
        char * m_poNewList = m_poList + ( iNewTreeCnt - getTotal() )*sizeof(SHMStringTree_AX) ;

        int i;
        for(i=getlTotal(); i>0; i--){
            ///往高处移动，先移动高处的数据
            memcpy( m_poNewList+(*m_piListLen)*i , m_poList+(*m_piListLen)*i, (*m_piListLen) );
        }
        m_poList = m_poNewList;
#endif    
    
        /// *m_piListHead 不变
        /// *m_piListTail
    }

//重新准备可用空间
    SHMStringList_AX * pList = 0;

    for(unsigned int ii=getlTotal()+1; ii<=iNewListCnt; ii++)
    {
        pList = getList(ii);
        pList->m_iNext = ii+1;
    }
    pList = getList(iNewListCnt);
    pList->m_iNext = 0;
    
///新增空间加入空闲链表        
    if(*m_piListTail ){
        pList = getList(*m_piListTail);
        pList->m_iNext = getlTotal()+1;
        (*m_piListTail) = iNewListCnt;
    }else{
        (*m_piListHead) = getlTotal()+1;
        (*m_piListTail) = iNewListCnt;
    }
    *m_piListTotal = iNewListCnt;


/// tree 和 list 索引范围不一样，
///tree是从0开始，到getTotal()-1; 
///list是从1开始，到getlTotal(); 


    if( iNewTreeCnt-getTotal() > 0 ){
        memset(&m_poTree[getTotal()], 0, (iNewTreeCnt-getTotal())*sizeof(SHMStringTree_AX));
        
        for(unsigned int ii=getTotal(); ii<iNewTreeCnt; ii++)
        {
            m_poTree[ii].m_iNext[0] = ii+1;
        }
        m_poTree[iNewTreeCnt-1].m_iNext[0] = 0;
        if(*m_piTreeTail ){
            m_poTree[*m_piTreeTail].m_iNext[0] = getTotal();
            (*m_piTreeTail) = iNewTreeCnt-1;
        }else{
            (*m_piTreeHead) = getTotal();
            (*m_piTreeTail) = iNewTreeCnt-1;
        }
        *m_piTreeTotal = iNewTreeCnt;
    }

#ifdef  DEBUG_TEST
printf("第%d次扩展，本次新增%u个数据项。", (*m_piExpandCnt), iExDataCnt );
#endif
    
    m_oDataLock.V();
    return iExDataCnt;

    }

    m_oDataLock.V();
    return 0;
}

