/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMAccess.h"
#include <string.h>
#include <stdio.h>

#include "MBC.h"

#ifdef SHM_EXPAND_FREE_POS
    char * SHMAccess::g_sGlobalPos = 0;
    long   SHMAccess::g_lBlock = 0;
#endif




//##ModelId=42442817022A
SHMAccess::SHMAccess(char *shmname, bool bShm) :
  m_piSize (0), m_poSHM (0), m_lSHMKey (0)
{
    memset(m_poExSHM, 0, sizeof(m_poExSHM) );
    memset(m_sExSHMName, 0, sizeof(m_sExSHMName) );
    memset(m_lExSHMKey, 0, sizeof(m_lExSHMKey) );
    m_iExShmCnt = 0;
    m_iMaxExCnt = 0;
    m_lNextSize = 0;
    m_piExpandCnt= 0;

#ifdef SHM_EXPAND_GLOBAL_POS
//    memset(m_iSegment, 0, sizeof(m_iSegment) );
    m_piSeg=0;
    memset(m_sSegment, 0, sizeof(m_sSegment) );
#endif
    
	m_sSHMName = new char[strlen(shmname)+1];
	if (!m_sSHMName)
		THROW(MBC_SHMAccess+1);
	strcpy (m_sSHMName, shmname);
    m_bShm = bShm;
	open ();
}


//##ModelId=424428170248
SHMAccess::SHMAccess(long shmkey) :
  m_piSize (0), m_poSHM (0), m_sSHMName(0), m_lSHMKey (shmkey)
{
    memset(m_poExSHM, 0, sizeof(m_poExSHM) );
    memset(m_sExSHMName, 0, sizeof(m_sExSHMName) );
    memset(m_lExSHMKey, 0, sizeof(m_lExSHMKey) );
    m_iExShmCnt = 0;
    m_iMaxExCnt = 0;
    m_lNextSize = 0;
    m_piExpandCnt= 0;

#ifdef SHM_EXPAND_GLOBAL_POS
//    memset(m_iSegment, 0, sizeof(m_iSegment) );
    m_piSeg=0;
    memset(m_sSegment, 0, sizeof(m_sSegment) );
#endif
    m_bShm = true;
	open ();
}

 

SHMAccess::SHMAccess(unsigned int iType,char *pAddr):
  m_piSize (0), m_poSHM (0), m_sSHMName(0), m_lSHMKey(0)
{		
	memset(m_poExSHM, 0, sizeof(m_poExSHM) );
    memset(m_sExSHMName, 0, sizeof(m_sExSHMName) );
    memset(m_lExSHMKey, 0, sizeof(m_lExSHMKey) );
    m_iExShmCnt = 0;
    m_iMaxExCnt = 0;
    m_lNextSize = 0;
    m_piExpandCnt= 0;
#ifdef SHM_EXPAND_GLOBAL_POS
    m_piSeg=0;
    memset(m_sSegment, 0, sizeof(m_sSegment) );
#endif
	/*if(iType == 0)
	{
		m_sSHMName = new char[strlen(pAddr)+1];
		if (!m_sSHMName)
			THROW(MBC_SHMAccess+1);
		strcpy (m_sSHMName, pAddr);
		open ();
	}*/
}

SHMAccess::SHMAccess(char *sType,char *pAddr):
  m_piSize (0), m_poSHM (0), m_sSHMName(0), m_lSHMKey(0)
{
	memset(m_poExSHM, 0, sizeof(m_poExSHM) );
    memset(m_sExSHMName, 0, sizeof(m_sExSHMName) );
    memset(m_lExSHMKey, 0, sizeof(m_lExSHMKey) );
    m_iExShmCnt = 0;
    m_iMaxExCnt = 0;
    m_lNextSize = 0;
    m_piExpandCnt = 0;
#ifdef SHM_EXPAND_GLOBAL_POS
    m_piSeg=0;
    memset(m_sSegment, 0, sizeof(m_sSegment) );
#endif
	/*if(sType[0] != '\0')
	{
		m_sSHMName = new char[strlen(pAddr)+1];
		if (!m_sSHMName)
			THROW(MBC_SHMAccess+1);
		strcpy (m_sSHMName, pAddr);
		open ();
	}*/
}
SHMAccess::SHMAccess(long shmkey , long iNextSize, int iMaxExCnt ) :
  m_piSize (0), m_poSHM (0), m_sSHMName(0), m_lSHMKey (shmkey)
{
    memset(m_poExSHM, 0, sizeof(m_poExSHM) );
    memset(m_sExSHMName, 0, sizeof(m_sExSHMName) );
    memset(m_lExSHMKey, 0, sizeof(m_lExSHMKey) );
    m_iExShmCnt = 0;
    m_iMaxExCnt = 0;
    m_lNextSize = iNextSize;
    m_piExpandCnt= 0;
    m_iUnitSize = 0;
    
    m_iMaxExCnt = (iMaxExCnt<=MAX_SHM_EX_CNT? iMaxExCnt : MAX_SHM_EX_CNT);
    
    for(int i=0; i<m_iMaxExCnt; i++){
        m_lExSHMKey[i] = shmkey+i+1;
    }
#ifdef SHM_EXPAND_GLOBAL_POS
//    memset(m_iSegment, 0, sizeof(m_iSegment) );
    m_piSeg=0;
    memset(m_sSegment, 0, sizeof(m_sSegment) );
#endif
    
	open ();
}



//##ModelId=424428170266
SHMAccess::~SHMAccess()
{
	if (m_sSHMName)
		delete [] m_sSHMName;

    if (m_poSHM) {
        delete m_poSHM;
        m_poSHM = 0;
    }

    for(int i=0; i<m_iMaxExCnt; i++){
        if( m_sExSHMName[i])
            delete [] m_sExSHMName[i];
            m_sExSHMName[i]=0;
    }

}

//##ModelId=424428BF005F
bool SHMAccess::exist()
{
	return m_poSHM;
}

void SHMAccess::close()
{
    if(m_poSHM)
        m_poSHM->close();
}

/*char *SHMAccess::getMemAddr()
{
	if(m_poSHM)
	{
		return (char*)(*m_poSHM);
	} else {
		return NULL;
	}
}*/
//##ModelId=424428CB0099
void SHMAccess::remove()
{
	m_poSHM->remove ();
	delete m_poSHM;
	m_poSHM = 0;

    for(int i=0; i<m_iMaxExCnt; i++){
        if( m_poExSHM[i]){
            m_poExSHM[i]->remove ();
            delete  m_poExSHM[i];
            m_poExSHM[i] =0;
        }
    }
    
}

//##ModelId=4244C70600D3
void SHMAccess::open()
{
	unsigned long size; 
	if (m_sSHMName) {
		m_poSHM = new SimpleSHM (m_sSHMName, 4, m_bShm);
	} else {
		m_poSHM = new SimpleSHM (m_lSHMKey, 4);
	}

	if (!m_poSHM)
		THROW(MBC_SHMAccess+2);

	if (m_poSHM->exist()) {
		m_poSHM->open (false);
		//m_piSize = (unsigned int *)((char *)(*m_poSHM));
		size = m_poSHM->size ();
		m_poSHM->close ();
		delete m_poSHM;
		m_poSHM = 0;
		
#ifdef SHM_EXPAND_FREE_POS		
		if (m_sSHMName) {
			m_poSHM = new SimpleSHM (m_sSHMName, size, m_bShm, g_sGlobalPos);
		} else {
		    printf("key:0x%lx need addr-->%lx\n", m_lSHMKey, g_sGlobalPos);
			m_poSHM = new SimpleSHM (m_lSHMKey, size, g_sGlobalPos );
		}
#else
		if (m_sSHMName) {
			m_poSHM = new SimpleSHM (m_sSHMName, size, m_bShm, 0);
		} else {
		    ////printf("key:0x%lx need addr-->%lx\n", m_lSHMKey, 0);
			m_poSHM = new SimpleSHM (m_lSHMKey, size, 0 );
		}
#endif
		if (!m_poSHM)
			THROW(MBC_SHMAccess+2);

		m_poSHM->open (false);

#ifdef SHM_EXPAND_FREE_POS
        refreshPos();
#endif
#ifdef SHM_EXPAND_GLOBAL_POS
    ////m_sSegment[0] = (char *)(* m_poSHM); 
    ////第一个有控制信息，因此一般这个地址不是正确的
#endif

        for(int i=0; i<m_iMaxExCnt; i++){
            if(! openEx(i) )
                break;
              
            m_iExShmCnt++;
        }

	} else {
		delete m_poSHM;
		m_poSHM = 0;
	}		
}

bool SHMAccess::openEx(int i)
{
	unsigned long size; 
	if (m_sExSHMName[i]) {
		m_poExSHM[i] = new SimpleSHM (m_sExSHMName[i], 4, m_bShm);
	} else {
		m_poExSHM[i] = new SimpleSHM (m_lExSHMKey[i], 4);
	}

	if (!m_poExSHM[i])
		THROW(MBC_SHMAccess+2);

	if (m_poExSHM[i]->exist()) {
		m_poExSHM[i]->open (false);
		//m_piSize = (unsigned int *)((char *)(*m_poSHM));
		size = m_poExSHM[i]->size ();
		m_poExSHM[i]->close ();
		delete m_poExSHM[i];
		m_poExSHM[i] = 0;

#ifdef SHM_EXPAND_FREE_POS		
		char * sBase = (char *)(* m_poSHM);
		long lba = m_poSHM->getlba();
		char * sExBase =sBase + (( (m_poSHM->size()-1)/lba )+1)*lba;
		
		sExBase = sExBase +(i*lba) ;
		
		if (m_sExSHMName[i]) {
			m_poExSHM[i] = new SimpleSHM (m_sExSHMName[i], size, m_bShm, sExBase );
		} else {
			m_poExSHM[i] = new SimpleSHM (m_lExSHMKey[i], size, sExBase );
		}
#else
		if (m_sExSHMName[i]) {
			m_poExSHM[i] = new SimpleSHM (m_sExSHMName[i], size, m_bShm, 0 );
		} else {
			m_poExSHM[i] = new SimpleSHM (m_lExSHMKey[i], size, 0 );
		}

#endif

		if (!m_poExSHM[i])
			THROW(MBC_SHMAccess+2);

		m_poExSHM[i]->open (false);

#ifdef SHM_EXPAND_GLOBAL_POS
		m_sSegment[i+1] = (char *)(* m_poExSHM[i]);

#endif

		return true;

	} else {
		delete m_poExSHM[i];
		m_poExSHM[i] = 0;
		
	}	
	return false;	
}

//##ModelId=4244CAB6013E
void SHMAccess::create(unsigned long size)
{

#ifdef SHM_EXPAND_FREE_POS
	if (m_sSHMName) {
		m_poSHM = new SimpleSHM (m_sSHMName, size, m_bShm, g_sGlobalPos);
	} else {
	    printf("key:0x%lx need addr-->%lx\n", m_lSHMKey, g_sGlobalPos);
		m_poSHM = new SimpleSHM (m_lSHMKey, size, g_sGlobalPos);
	}
#else
	if (m_sSHMName) {
		m_poSHM = new SimpleSHM (m_sSHMName, size, m_bShm, 0);
	} else {
	    ////printf("key:0x%lx need addr-->%lx\n", m_lSHMKey, 0);
		m_poSHM = new SimpleSHM (m_lSHMKey, size, 0);
	}

#endif

	if (!m_poSHM)
		THROW(MBC_SHMAccess+2);
	
	m_poSHM->open (true);
	//m_piSize = (unsigned int *)((char *)(*m_poSHM));
	//*m_piSize = size;	
#ifdef SHM_EXPAND_FREE_POS    
    refreshPos();
#endif
	memset((char *)(*m_poSHM), 0, size);
}

#ifdef SHM_EXPAND_FREE_POS
void SHMAccess::refreshPos()
{
    if(!g_lBlock ){
        g_lBlock = m_poSHM->getlba();
    }
    if( !g_sGlobalPos ){
        g_sGlobalPos = (char *)(*m_poSHM) ; 
    }
    if( g_lBlock && g_sGlobalPos){
        if(m_lNextSize ){
            long ltemp = (m_iMaxExCnt * m_lNextSize) /g_lBlock;
            g_sGlobalPos += g_lBlock*( ltemp + 1 );
        }else{
            long ltemp = ( m_poSHM->size() * 4  ) /g_lBlock;
            g_sGlobalPos += g_lBlock*( ltemp + 1 );
        }
    }
    
}
#endif

int SHMAccess::connnum()
{
	if(m_poSHM)
		return m_poSHM->nattch();
	else
		return 0;
}

bool SHMAccess::reopen()
{
    unsigned long ulnewsize = m_poSHM->size();
	char * sBase = (char *)(* m_poSHM);
//	long lba = m_poSHM->getlba();
//	char * sExBase =sBase + (( (ulnewsize-1)/lba )+1)*lba;

    if( m_poSHM->getLastSize() != ulnewsize ){
        ////大小不一致，需要重新open
        m_poSHM->setLastSize(ulnewsize);
        m_poSHM->close();
        m_poSHM->open(false);
    }
    int i=0;
    for(i=0; i<m_iMaxExCnt; i++){
        if(m_poExSHM[i]){
            ulnewsize = m_poExSHM[i]->size();
            if( m_poExSHM[i]->getLastSize() != ulnewsize ){
                ////大小不一致，需要重新open
                m_poExSHM[i]->setLastSize(ulnewsize);
                m_poExSHM[i]->close();
                m_poExSHM[i]->open(false);
            }
        }else{
            if(! openEx(i) ){
                break;
            }
        }
    }
    m_iExShmCnt = i;
    return true;
}

void SHMAccess::showExpandInfo()
{

    printf("expand size:%ld,max expand count:%d,current expand count:%d\n",
        m_lNextSize, m_iMaxExCnt, (m_piExpandCnt==0?0:(*m_piExpandCnt) )  );

    long *pp = (long *) (char *)(*m_poSHM);
    if(pp){
        printf("address:0x%lx--" , pp );

        printf("DataBase:0x%lx, count:%u\n", m_sSegment[0], ( m_piSeg==0 ? 0 : (*m_piSeg) ) );
    }
    for(int i=0; i<m_iMaxExCnt; i++){
        if(m_poExSHM[i]){
            printf("DataBase:0x%lx, count:%u\n", m_sSegment[i+1], *(m_piSeg+i+1) );
        }else{
            break;
        }
    }


}

unsigned long SHMAccess::totalSize()
{
    unsigned long size = m_poSHM->size();
    
    for(int i=0; i<m_iMaxExCnt; i++){
        if(m_poExSHM[i]){
            size+= m_poExSHM[i]->size();
        }else{
            break;
        }
    }
    return size;

}

void SHMAccess::setExpandNull()
{
    unsigned long size;
    for(int i=0; i<m_iMaxExCnt; i++){
        if(m_poExSHM[i]){
            size= m_poExSHM[i]->size();
            char * pStart = (char *)( * (m_poExSHM[i]) );
            memset(pStart, 0, size);
        }else{
            break;
        }
    }
    
}

void SHMAccess::initExpand(unsigned int * pExCnt, int iUnitSize, unsigned int * pSeg )
{
    m_piExpandCnt = pExCnt;
    m_iUnitSize = iUnitSize;
    m_piSeg = pSeg;
    
}

unsigned long SHMAccess::expandit( )
{ 
    if(!m_poSHM || !m_piExpandCnt )
        return 0L;
    
    ////超过了扩张最大次数
    if( (*m_piExpandCnt) >=m_iMaxExCnt)
        return 0L;

#ifdef SHM_EXPAND_FREE_POS

    (*m_piExpandCnt)++;

    unsigned long ulSize=m_poSHM->size();
    unsigned long ulba=m_poSHM->getlba();
    
    unsigned long xSize = m_lNextSize;

    long lRest =  ulba - (ulSize % ulba);
    if( ulba == lRest)
        lRest = 0;
        
    if(xSize <= lRest ){

        return  m_poSHM->expand(xSize);
    }
    

    SimpleSHM * pLast = m_poSHM;
    
    if(m_iExShmCnt && m_poExSHM[m_iExShmCnt-1] ){
        ulSize=m_poExSHM[m_iExShmCnt-1]->size();
        ulba=m_poExSHM[m_iExShmCnt-1]->getlba();
        lRest =  ulba - (ulSize % ulba);
        if( ulba == lRest)
            lRest = 0;
        
        pLast = m_poExSHM[m_iExShmCnt-1];
        
        if( xSize <= lRest ){
            return  m_poExSHM[m_iExShmCnt-1]->expand(xSize);
        }
        
    }
    
    ///使用新key进行扩张
    if(m_iExShmCnt>=m_iMaxExCnt){
        return 0;
    }
    if(lRest) xSize -= pLast->expand(xSize);

    
	char * sBase = (char *)(* m_poSHM);
	long lba = m_poSHM->getlba();
	char * sExBase = sBase + (( (m_poSHM->size()-1)/lba )+1)*lba;
	sExBase = sExBase + (m_iExShmCnt)*lba;
		    
	if (m_sExSHMName[m_iExShmCnt]) {
		m_poExSHM[m_iExShmCnt] = new SimpleSHM (m_sExSHMName[m_iExShmCnt], m_lNextSize, m_bShm, sExBase  );
	} else {
		m_poExSHM[m_iExShmCnt] = new SimpleSHM (m_lExSHMKey[m_iExShmCnt], m_lNextSize,  sExBase );
	}

	if (!m_poSHM[m_iExShmCnt])
		THROW(MBC_SHMAccess+2);

	m_poExSHM[m_iExShmCnt]->open (true);
	memset((char *)(*(m_poExSHM[m_iExShmCnt])), 0, m_lNextSize);
    m_iExShmCnt++;

    return m_lNextSize;

#endif

#ifdef SHM_EXPAND_GLOBAL_POS
    ///每次都使用新key进行扩张
    ///(*m_piExpandCnt) 应该等于 m_iExShmCnt
    if( (*m_piExpandCnt)!= m_iExShmCnt)
        return 0;
    
    if(m_iExShmCnt>=m_iMaxExCnt){
        return 0;
    }
	if (m_sExSHMName[m_iExShmCnt]) {
		m_poExSHM[m_iExShmCnt] = new SimpleSHM (m_sExSHMName[m_iExShmCnt], m_lNextSize, m_bShm, 0 );
	} else {
		m_poExSHM[m_iExShmCnt] = new SimpleSHM (m_lExSHMKey[m_iExShmCnt], m_lNextSize,  0 );
	}

	if (!m_poSHM[m_iExShmCnt])
		THROW(MBC_SHMAccess+2);

	m_poExSHM[m_iExShmCnt]->open (true);
	memset((char *)(*(m_poExSHM[m_iExShmCnt])), 0, m_lNextSize);

    m_sSegment[m_iExShmCnt+1] = (char *)(* m_poExSHM[m_iExShmCnt]); 
//    m_iSegment[m_iExShmCnt+1] = m_iSegment[m_iExShmCnt] + m_lNextSize/m_iUnitSize;
    *(m_piSeg+m_iExShmCnt+1)=(*(m_piSeg+m_iExShmCnt)) + m_lNextSize/m_iUnitSize;
    m_iExShmCnt++;
    (*m_piExpandCnt)++;
    
    return m_lNextSize;    
    
#endif

    return 0;
    
}

long SHMAccess::getKey()
{
	 if(m_poSHM)
	 {
	 	return m_poSHM->getIPCKey();
	 }
	 return 0;
}

