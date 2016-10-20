/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMMAPFILE_H_HEADER_INCLUDED
#define SHMMAPFILE_H_HEADER_INCLUDED

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>

const int FILE_NAME_MAX = 128;
const int ONCE_MAX = 268435456;  // 256M
const int MEM_COPY_MAX = 1000000;

template <class T>
class SHMMapFile
{
    public:
        SHMMapFile(char * pFileName, USERINFO_SHM_DATA_TYPE<T> * pSHMPointer = NULL/*, unsigned int iUsed = 0, unsigned int iTotal = 0*/);
        
        ~SHMMapFile();
        
        operator T *();
        
        int create();
        
        int openMap();
        
        void reset();
        
        bool exist();
        
        unsigned int malloc();
        
        unsigned int getCount();
        
        unsigned int getTotal();
		
		int delFile();
        
		void munmapFile();
        
    private:
        
        int createFile();
        
        int openFile();
        
        int mmapFile(int iFd, long lSize = 0);
        
        void setMapFile();
        
        unsigned int * m_piTotal;
        
        unsigned int * m_piUsed;
		
        #ifdef  USERINFO_REUSE_MODE
		unsigned int * m_piVersion; 
        unsigned int * m_piTotalSE;
        unsigned int * m_piUsedSE;
        unsigned int * m_piDeledSE;
        unsigned int * m_piFlag;
        unsigned int * m_piDelOver;
        unsigned int * m_pIArray;
		#endif
		
        T *m_pPointer;
		
        //T *m_pSHMData;
		
        T *m_pSHMPointer;
        
        char * m_pFileName;
        
        unsigned long m_ulSize;
};

template <class T>
SHMMapFile<T>::SHMMapFile(char * pFileName, USERINFO_SHM_DATA_TYPE<T> * pSHMPointer/*, unsigned int iUsed, unsigned int iTotal*/)
{
    m_pFileName = new char[FILE_NAME_MAX+1];
    memcpy(m_pFileName, 0, FILE_NAME_MAX);
    memcpy(m_pFileName, pFileName, strlen(pFileName));
    
    if(pSHMPointer){
        m_pSHMPointer = (T *)( (char *)(pSHMPointer->getMemAddr()) );
        
        m_piTotal = (((unsigned int *)m_pSHMPointer) + 1);
        m_piUsed = m_piTotal + 1;
        
        #ifdef  USERINFO_REUSE_MODE
        m_piVersion = m_piTotal - 1;
        m_piTotalSE = m_piTotal + 2;
        m_piFlag    = m_piTotal + 3;
        m_piUsedSE  = m_piTotal + 4;
        m_piDeledSE = m_piTotal + 5;
        m_piDelOver = m_piTotal + 6;
        
        if(*m_piTotalSE)
            m_pIArray = (unsigned int *) &m_pSHMPointer[(*m_piTotalSE)+1];
        else
            m_pIArray = 0;
        #endif
        
        create();
    }else
        openMap();
}

template <class T>
SHMMapFile<T>::~SHMMapFile()
{
    delete[] m_pFileName;
}

template <class T>
SHMMapFile<T>::operator T *()
{
	return m_pPointer;
}

template <class T>
void SHMMapFile<T>::reset()
{
	*m_piUsed = 1;
}

template <class T>
bool SHMMapFile<T>::exist()
{
    return m_pPointer;
}

template <class T>
unsigned int SHMMapFile<T>::malloc()
{
	#ifdef  USERINFO_REUSE_MODE
    if (*m_piDeledSE == *m_piUsedSE)
        return 0;
    unsigned int uiRet = m_pIArray[(*m_piUsedSE)];
    if(!uiRet)
        uiRet = (*m_piUsedSE);
    
    (*m_piUsedSE)++;
    if( (*m_piUsedSE) >= (*m_piTotalSE) )
        (*m_piUsedSE) = 0 ;
    
    return uiRet;
    #else
	if ((*m_piTotal) == (*m_piUsed))
		return 0;
    
	return ((*m_piUsed)++);
	#endif
}

template <class T>
unsigned int SHMMapFile<T>::getCount()
{
    #ifdef  USERINFO_REUSE_MODE
    unsigned int iCnt = 0;

    if( (*m_piUsedSE) > (*m_piDeledSE) ){
        iCnt = ( (*m_piUsedSE) - (*m_piDeledSE) -1 );
    }else{
        iCnt = (*m_piTotalSE)- 1 -  (  (*m_piDeledSE) - (*m_piUsedSE) );    
    }
    return iCnt;
    #else
	if (*m_piUsed)
		return ((*m_piUsed)-1);

	return 0;
	#endif
}

template <class T>
unsigned int SHMMapFile<T>::getTotal()
{
    #ifdef  USERINFO_REUSE_MODE
    if (*m_piTotalSE)
        return (*m_piTotalSE)-1;
    #else
	if (*m_piTotal)
		return ((*m_piTotal)-1);

	return 0;
	#endif
}

template <class T>
int SHMMapFile<T>::delFile()
{
    int iRet = 0;
	
	iRet = remove(m_pFileName);
    if(iRet == -1 && errno == ENOENT)
		iRet = 0;
    
    return iRet;
}

template <class T>
int SHMMapFile<T>::createFile()
{
    int iFd = 0;
    int iCnt = 0;
    int i = 0;
	long lTotal;
	
	#ifdef  USERINFO_REUSE_MODE
	lTotal = ((*m_piTotalSE)+1)* (sizeof(T)+sizeof(int));
	#else
	lTotal = (*m_piTotal)*sizeof(T);
	#endif
	
    m_ulSize = lTotal;
	
    if(lTotal%ONCE_MAX)
        iCnt = lTotal/ONCE_MAX + 1;
    else
        iCnt = lTotal/ONCE_MAX;
    
    iFd = open(m_pFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    if(iFd == -1)
        return iFd;
    
    for(i = 0; i < (iCnt-1); i ++){
        lseek(iFd, ONCE_MAX-1, SEEK_END);
        write(iFd, "", 1);
    }
    
    if(i == (iCnt-1)){
        lseek(iFd, lTotal%ONCE_MAX-1, SEEK_END);
        write(iFd, "", 1);
    }
    
    return iFd;
}

template <class T>
int SHMMapFile<T>::openFile()
{
    int iFd = 0;
    
    iFd = open(m_pFileName, O_RDWR);
    
    return iFd;
}

template <class T>
int SHMMapFile<T>::mmapFile(int iFd, long lSize)
{
    int iRet = 0;
	
    if(!lSize)
        #ifdef  USERINFO_REUSE_MODE
        lSize = ((*m_piTotalSE)+1)* (sizeof(T)+sizeof(int));
        #else
        lSize = (*m_piTotal)*sizeof(T);
        #endif
    
    m_pPointer = NULL;
    
    if(iFd == -1)
        return iRet;
    
    if(m_ulSize != lSize)
        m_ulSize = lSize;
    
    m_pPointer = (T *)mmap(0, (size_t)lSize, PROT_READ|PROT_WRITE, MAP_SHARED, iFd, 0);
    if(m_pPointer == MAP_FAILED)
        return iRet;
    close(iFd);
    
    return iRet;
}

template <class T>
void SHMMapFile<T>::munmapFile()
{
    if(m_pPointer){
		msync((char *)m_pPointer, m_ulSize, MS_SYNC);
    	munmap((char *)m_pPointer, m_ulSize);
    }
}

template <class T>
void SHMMapFile<T>::setMapFile()
{
    long i = 0;
    int iCnt = 0;
	
    #ifdef  USERINFO_REUSE_MODE
	if((((*m_piTotalSE)+1)*(sizeof(T)+sizeof(int)))%ONCE_MAX)
        iCnt = (((*m_piTotalSE)+1)*(sizeof(T)+sizeof(int)))/ONCE_MAX + 1;
    else
        iCnt = (((*m_piTotalSE)+1)*(sizeof(T)+sizeof(int)))/ONCE_MAX;
    
    for(i = 0; i < (iCnt-1); i ++){
        memcpy((char *)m_pPointer + i*ONCE_MAX, (char *)m_pSHMPointer + i*ONCE_MAX, ONCE_MAX);
		//msync((char *)m_pPointer + i*ONCE_MAX, ONCE_MAX, MS_SYNC);
		printf("%s Finish: %d %%!\n", m_pFileName, ((i+1)*100)/iCnt);
    }
    
    if(i == (iCnt-1)){
        memcpy((char *)m_pPointer + i*ONCE_MAX, (char *)m_pSHMPointer + i*ONCE_MAX, (((*m_piTotalSE)+1)*(sizeof(T)+sizeof(int)))%ONCE_MAX);
		//msync((char *)m_pPointer + i*ONCE_MAX, (((*m_piTotalSE)+1)*(sizeof(T)+sizeof(int)))%ONCE_MAX, MS_SYNC);
        printf("%s Finish: 100 %%!\n", m_pFileName);
	}
    #else
    if((*m_piUsed)%MEM_COPY_MAX)
        iCnt = (*m_piUsed)/MEM_COPY_MAX + 1;
    else
        iCnt = (*m_piUsed)/MEM_COPY_MAX;
    
    for(i = 0; i < (iCnt-1); i ++){
        memcpy(m_pPointer + i*MEM_COPY_MAX, m_pSHMPointer + i*MEM_COPY_MAX, MEM_COPY_MAX*sizeof(T));
		//msync(m_pPointer + i*MEM_COPY_MAX, MEM_COPY_MAX*sizeof(T), MS_SYNC);
		printf("%s Finish: %d %%!\n", m_pFileName, ((i+1)*100)/iCnt);
    }
    
    if(i == (iCnt-1)){
        memcpy(m_pPointer + i*MEM_COPY_MAX, m_pSHMPointer + i*MEM_COPY_MAX, ((*m_piUsed)%MEM_COPY_MAX)*sizeof(T));
		//msync(m_pPointer + i*MEM_COPY_MAX, ((*m_piUsed)%MEM_COPY_MAX)*sizeof(T), MS_SYNC);
		printf("%s Finish: 100 %%!\n", m_pFileName);
    }
    #endif
}

template <class T>
int SHMMapFile<T>::openMap()
{
    int iRet = 0;
    int iFd = -1;
	struct stat statbuf;
    
    iFd = openFile();
    stat(m_pFileName, &statbuf);
	
	m_ulSize = statbuf.st_size;
	
    iRet = mmapFile(iFd, m_ulSize);
    
    if(!iRet){
        m_piTotal = (((unsigned int *)m_pPointer) + 1);
        m_piUsed = m_piTotal + 1;
        
        #ifdef  USERINFO_REUSE_MODE
        m_piVersion = m_piTotal - 1;
        m_piTotalSE = m_piTotal + 2;
        m_piFlag    = m_piTotal + 3;
        m_piUsedSE  = m_piTotal + 4;
        m_piDeledSE = m_piTotal + 5;
        m_piDelOver = m_piTotal + 6;
        
        if(*m_piTotalSE)
            m_pIArray = (unsigned int *) &m_pSHMPointer[(*m_piTotalSE)+1];
        else
            m_pIArray = 0;
        #endif
    }
    
	return iRet;
}

template <class T>
int SHMMapFile<T>::create()
{
    int iRet = 0;
    int iFd = -1;
    
    delFile();
    iFd = createFile();
    
    iRet = mmapFile(iFd);
	
    setMapFile();
	
    printf("madvise ret: %d\n", madvise((char *)m_pPointer, m_ulSize, MADV_DONTNEED));
	
    munmapFile();
    printf("madvise ret: %d\n", madvise((char *)m_pPointer, m_ulSize, MADV_DONTNEED));
	return iRet;
}

#endif
