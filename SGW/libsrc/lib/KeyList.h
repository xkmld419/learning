// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef KEYLIST_H_HEADER_INCLUDED_BDD81450
#define KEYLIST_H_HEADER_INCLUDED_BDD81450

#include "Exception.h"
#include "MBC.h"

//##ModelId=4227C5830337
//##Documentation
//## 如果Key相同，后者覆盖前者
template <class T>
class KeyList
{

  public:
    //##ModelId=4227CC7100D5
    KeyList(int key, T data);
    KeyList(char *key, T data);

    //##ModelId=4227CC78000D
    void add(char *key, T data);

    //##ModelId=4227CC9C00AF
    void add(int key, T data);

    //##ModelId=4227CC7B018D
    bool get(int key, 
        //##Documentation
        //## 用于输出
        T *vlaue);

    //##ModelId=4227CC7E00FC
    bool get(char *key, T *value) const;

    void remove(KeyList<T> **pList, int key);
    void remove(KeyList<T> **pList, char *key);

    //##ModelId=4227D75D0193
    ~KeyList();

//  private:
	KeyList<T> *m_poNext;	
	T m_oData;
	char *m_sOriKey;
	int m_iOriKey;
};

//##ModelId=4227CC7100D5
template <class T>
KeyList<T>::KeyList(int key, T data) 
: m_poNext(0),m_oData(data),m_sOriKey(0),m_iOriKey(key)
{
}

template <class T>
KeyList<T>::KeyList(char * key, T data) 
: m_poNext(0),m_oData(data),m_iOriKey(0)
{
	m_sOriKey = new char[strlen(key)+1];
	if (!m_sOriKey)
		THROW(MBC_KeyList+1);
	strcpy (m_sOriKey, key);
}

//##ModelId=4227CC78000D
template <class T>
void KeyList<T>::add(char *key, T data)
{
	//##不用递归
	KeyList<T> * p = (KeyList<T> *) this;

	while (p) {
		if (!strcmp(key, p->m_sOriKey)) {
			p->m_oData = data;
			return;
		}
		p = p->m_poNext;
	}

	//##是新的key
	p = new KeyList<T> (key, data);
	if (!p)
		THROW(MBC_KeyList+1);

	p->m_poNext = this->m_poNext;
	this->m_poNext = p;
}

//##ModelId=4227CC9C00AF
template <class T>
void KeyList<T>::add(int key, T data)
{
	//##不用递归
	KeyList<T> * p = (KeyList<T> *) this;

	while (p) {
		if (key == m_iOriKey) {
			p->m_oData = data;
			return;
		}
		p = p->m_poNext;
	}

	//##是新的key
	p = new KeyList<T> (key, data);
	if (!p)
		THROW(MBC_KeyList+1);

	p->m_poNext = this->m_poNext;
	this->m_poNext = p;
}

//##ModelId=4227CC7B018D
template <class T>
bool KeyList<T>::get(int key, T *value)
{
	//##不用递归
	KeyList<T> * p = (KeyList<T> *) this;

	while (p) {
		if (key == p->m_iOriKey) {
			*value = p->m_oData;
			return true;
		}
		p = p->m_poNext;
	}

	return false;
}

//##ModelId=4227CC7E00FC
template <class T>
bool KeyList<T>::get(char *key, T *value) const
{
	//##不用递归
	KeyList<T> * p = (KeyList<T> *) this;

	//value = NULL;
	int iFlag=0;
	
	while (p) {
		if (!strcmp(key, p->m_sOriKey)) {
			*value = p->m_oData;
			iFlag =1;
			break;
			//return true;
		}
		p = p->m_poNext;
	}
	
	if (iFlag) 
		return true;
	else
		return false;
}

template <class T>
void KeyList<T>::remove(KeyList<T> ** ppList, int key)
{
	KeyList<T> * p;

	while ((p = *ppList) != 0) {
		if (p->m_iOriKey == key) {
			*ppList = p->m_poNext;
			p->m_poNext = 0;
			delete p;
			return;
		}

		ppList = &(p->m_poNext);
	}
}

template <class T>
void KeyList<T>::remove(KeyList<T> ** ppList, char *key)
{
	KeyList<T> * p;
	while ((p = *ppList) != 0) {
		if (!strcmp(key, p->m_sOriKey)) {
			*ppList = p->m_poNext;
			p->m_poNext = 0;
			delete p;
			return;
		}

		ppList = &(p->m_poNext);
	}
}

//##ModelId=4227D75D0193
template <class T>
KeyList<T>::~KeyList()
{
	//##不用递归
	KeyList<T> * p;

	if (m_sOriKey) 
		delete [] m_sOriKey;
	
	while (m_poNext) {
		p = m_poNext;
		m_poNext = p->m_poNext;
		p->m_poNext = 0;
		delete p;
	}
}

#endif /* KEYLIST_H_HEADER_INCLUDED_BDD81450 */



