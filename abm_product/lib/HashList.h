/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef HASHLIST_H_HEADER_INCLUDED_BDD83461
#define HASHLIST_H_HEADER_INCLUDED_BDD83461

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "MBC.h"
#include "KeyList.h"

//##ModelId=41DB839500D0
template <class T>
class HashList
{
  public:
	friend class Iteration;

	class Iteration
	{
	  public:
		bool next(T &value)
		{
			while (!m_poCur && m_iHashIndex<m_poHashList->m_iHashValue) {
				m_poCur = m_poHashList->m_poKeyList[m_iHashIndex];
				m_iHashIndex++;
			}

			if (!m_poCur) return false;

			value = m_poCur->m_oData;
			m_poCur = m_poCur->m_poNext;
			return true;		
		}

		Iteration (long i, KeyList<T> *p1, HashList<T> *p2):
		m_iHashIndex(i), m_poCur(p1), m_poHashList(p2)
		{}

		Iteration (const Iteration & right)
		{
			m_iHashIndex = right.m_iHashIndex;
			m_poCur = right.m_poCur;
			m_poHashList = right.m_poHashList;
		}

	  private:
		unsigned long m_iHashIndex;
		KeyList<T> * m_poCur;
		HashList<T> * m_poHashList;
	};


	/*Iteration getIteration ()
	{
		return Iteration (0, m_poKeyList[0], this);
	}	
	*/
	
	Iteration getIteration ()
	{
		return Iteration (0, 0, this);
	}	
	
    //##ModelId=41DCE934027F
    void add(long key, T data) ;

    //##ModelId=41DCE9340293
    bool get(long key, T *value) ;

    //##ModelId=41DCE93402B1
    void destroy();

    void clearAll();

    //##ModelId=4227CC0B039F
    HashList(unsigned long hashvalue);

    //##ModelId=4227D8560390
    ~HashList();

    //##ModelId=4227D88103A6
    void add(char *key, T data) ;

    //##ModelId=4227D88D00AA
    bool get(char *key, T *value) ;
    
    // add by yangks 2007.01.18
    void remove(long key);
    void remove(char *key);
    

  private:
    //##ModelId=4227D80B016C
    long calKey(long key);

    //##ModelId=4227D81903A7
    long calKey(char *key);

  public:
    //##ModelId=4227C4CA009C
    //##Documentation
    //## 数组的维数
    unsigned long m_iHashValue;

    //##ModelId=4227C50A0260
    unsigned long m_iCount;

    //##ModelId=4227CBC301BB
    KeyList<T> **m_poKeyList;

};


//##ModelId=41DCE934027F
template <class T>
void HashList<T>::add(long key, T data) 
{
	long i = calKey (key);
	if (m_poKeyList[i]) {
		m_poKeyList[i]->add (key, data);
	} else {
		m_poKeyList[i] = new KeyList<T> (key, data);
		if (!m_poKeyList[i])
			THROW(MBC_HashList+1);
	}
}

//##ModelId=41DCE9340293
template <class T>
bool HashList<T>::get(long key, T *value) 
{
	long i = calKey (key);
	if (m_poKeyList[i]) {
		return m_poKeyList[i]->get (key, value);
	} else {
		return false;
	}

}

//add by yangks 2007.01.18
template <class T>
void HashList<T>::remove(long key)
{
	long i = calKey (key);
	if (m_poKeyList[i]) {
		m_poKeyList[i]->remove(&(m_poKeyList[i]), key);
	}
}

template <class T>
void HashList<T>::remove(char *key)
{
	long i = calKey (key);
	if (m_poKeyList[i]) {
		m_poKeyList[i]->remove(&(m_poKeyList[i]), key);
	}
}

template <class T>
void HashList<T>::clearAll()
{
	unsigned long i;
	if (!m_poKeyList)
		return;

	for (i=0; i<m_iHashValue; i++) {
		if (m_poKeyList[i]) {
			delete m_poKeyList[i];
			m_poKeyList[i] = 0;
		}		
	}
}
//end add

//##ModelId=41DCE93402B1
template <class T>
void HashList<T>::destroy()
{
	unsigned long i;
	if (!m_poKeyList)
		return;

	for (i=0; i<m_iHashValue; i++) {
		if (m_poKeyList[i])
			delete m_poKeyList[i];
	}

	delete [] m_poKeyList;
	m_poKeyList = 0;
}

//##ModelId=4227CC0B039F
template <class T>
HashList<T>::HashList(unsigned long hashvalue) : m_iHashValue(hashvalue)
{
	if (!hashvalue)
		THROW(MBC_HashList+2);

	m_poKeyList = new KeyList<T> * [hashvalue];
	if (!m_poKeyList)
		THROW(MBC_HashList+1);

	memset (m_poKeyList, 0, sizeof(KeyList<T> *) * hashvalue);
}

//##ModelId=4227D8560390
template <class T>
HashList<T>::~HashList()
{
	destroy ();
}

//##ModelId=4227D88103A6
template <class T>
void HashList<T>::add(char *key, T data) 
{	
	long i = calKey (key);
	if (m_poKeyList[i]) {
		m_poKeyList[i]->add (key, data);
	} else {
		m_poKeyList[i] = new KeyList<T> (key, data);
		if (!m_poKeyList[i])
			THROW(MBC_HashList+1);
	}
}

//##ModelId=4227D88D00AA
template <class T>
bool HashList<T>::get(char *key, T *value) 
{
	long i = calKey (key);
	if (m_poKeyList[i]) {
		return m_poKeyList[i]->get (key, value);
	} else {
		return false;
	}
}

//##ModelId=4227D80B016C
template <class T>
long HashList<T>::calKey(long key)
{
	return key % m_iHashValue;
}

//##ModelId=4227D81903A7
template <class T>
long HashList<T>::calKey(char *key)
{
	long i ,j;
	
	i = 0;
	for (j = 1; *key && (j<13); j++) {
		i += ((*key) * j); /* ascii char times its 1-based index */
		key++;
	}
	
	return i % m_iHashValue;
}

#endif /* HASHLIST_H_HEADER_INCLUDED_BDD83461 */
