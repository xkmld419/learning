/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMDATA_H_HEADER_INCLUDED_BDBB63C4
#define SHMDATA_H_HEADER_INCLUDED_BDBB63C4

#include "HashList.h"
#include "SHMAccess.h"

#define ATTACH_DATA(X, Y, Z) \
        X = new SHMData<Y> (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }


//##ModelId=424418EA006B
//##Documentation
//## 共享内存数据区模板类，要求存放的数据的
//## 每个元素的大小不小于8个字节，如果元素的
//## 大小小于8个字节，会出现问题
template <class T>
class SHMData : public SHMAccess
{
  public:
    typedef T value_type;
    
    
    //##ModelId=42441AD80198
    SHMData(char *shmname, bool bShm = true);

    //##ModelId=42441D7D0001
    SHMData(long shmkey);

    //##ModelId=42441AD801E8
    ~SHMData();

    //##ModelId=4244200C0125
    operator T *();

    //##ModelId=424420D20076
    //##Documentation
    //## 创建数据区
    void create(
        //##Documentation
        //## 最多元素个数
        unsigned int itemnumber) ;

    void reset();
    void clear();

    //##ModelId=42442167032D
    //##Documentation
    //## 非0为申请成功，
    //## 0为申请失败
    unsigned int malloc();
    //##ModelId=4247DADF01DA
    unsigned int getCount();
	

	void	setCount(unsigned int iCount);
//modify by jinx for app_guard 070402
	unsigned int getTotal();
		
	time_t getLoadTime();

	unsigned  int  getMemSize();
	unsigned  long getRealSize();

	unsigned  int  getUsedSize();
	
	unsigned long   getMemAdress();
  private:
    //##ModelId=4244CD2302FA
    void open();

    //##ModelId=42441A3201CC
    unsigned int *m_piTotal;

    //##ModelId=42441A47006D
    unsigned int *m_piUsed;

 
    //##ModelId=42442087029F
    T *m_pPointer;

};



//##ModelId=42441AD80198
template <class T>
SHMData<T>::SHMData(char *shmname, bool bShm) :
SHMAccess (shmname, bShm)
{
	if (m_poSHM)
		open ();
}

//核心参数需要用到更改数据大小
template <class T>
void SHMData<T>::setCount(unsigned int iCount)
{
	if(*m_piTotal<iCount)
		return ;
	(*m_piUsed)=iCount;
}
//##ModelId=42441D7D0001
template <class T>
SHMData<T>::SHMData(long shmkey) :
SHMAccess (shmkey)
{
	if (m_poSHM)
		open ();
}


//##ModelId=42441AD801E8
template <class T>
SHMData<T>::~SHMData()
{
}

//##ModelId=4244200C0125
template <class T>
SHMData<T>::operator T *()
{
	return m_pPointer;
}

//##ModelId=424420D20076
template <class T>
void SHMData<T>::create(unsigned int itemnumber) 
{
	SHMAccess::create(((itemnumber+1)*sizeof(T)));
	open ();
	*m_piTotal = itemnumber + 1;
	*m_piUsed = 1;
}

template <class T>
void SHMData<T>::reset()
{
	*m_piUsed = 1;
}

//##ModelId=42442167032D
template <class T>
unsigned int SHMData<T>::malloc()
{
	if (*m_piTotal == *m_piUsed)
		return 0;

	return ((*m_piUsed)++);
}

//##ModelId=4244CD2302FA
template <class T>
void SHMData<T>::open()
{
	m_piTotal = (unsigned *)((char *)(*m_poSHM)) + 1;
	m_piUsed = m_piTotal + 1;
	m_pPointer = (T *)((char *)(*m_poSHM));
}

//##ModelId=4247DADF01DA
template <class T>
unsigned int SHMData<T>::getCount()
{
	if (*m_piUsed)
		return (*m_piUsed)-1;

	return 0;
}

//modify by jinx for app_guard 070402
template <class T>
unsigned int SHMData<T>::getTotal()
{
	if (*m_piTotal)
		return (*m_piTotal)-1;

	return 0;	
}
template <class T>
unsigned int SHMData<T>::getMemSize()
{
	 if (*m_piTotal)
		 return (*m_piTotal)* sizeof(T);

		    return 0;
}
template <class T>
unsigned int SHMData<T>::getUsedSize()
{
	if (*m_piUsed)
		return (*m_piUsed)* sizeof(T);

		 return 0;
}

template <class T>
unsigned long SHMData<T>::getMemAdress()
{		
	if (m_poSHM)
	{
		return   (unsigned long)m_poSHM;
	}
	
	return 0;
}
template <class T>
time_t SHMData<T>::getLoadTime()
{
	time_t shm_ctime;
	
	if (m_poSHM)
	{
		shm_ctime = m_poSHM->getShmTime();	
		
		return shm_ctime ;
	}
	return  0;
}

template <class T>
unsigned long SHMData<T>::getRealSize()
{
	  unsigned long size = m_poSHM->size();
		 return size;

}


#endif /* SHMDATA_H_HEADER_INCLUDED_BDBB63C4 */
