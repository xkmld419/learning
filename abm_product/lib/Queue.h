/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef QUEUE_H_HEADER
#define QUEUE_H_HEADER

#include "CommonMacro.h"
#include "Exception.h"
#include "MBC.h"

template <class T>
class Queue
{
  public:
    class Item
    {
        friend class Queue;

      public:
        Item();

      private:
        T m_oBuf[16];
	Item * m_poNext;
    };

    void put(T data);

    bool get(T &data);

    void clear();

    Queue();

    ~Queue();

  private:
	int m_iHead;
	int m_iTail;

	Item * m_poHead;
	Item * m_poTail;
};

template <class T>
Queue<T>::Item::Item():m_poNext(0)
{
}

template <class T>
void Queue<T>::put(T data)
{
	m_poTail->m_oBuf[m_iTail++] = data;

	if (m_iTail == 16) {
		if (m_poTail->m_poNext == m_poHead) {
			m_poTail->m_poNext = new Item ();
			if (!(m_poTail->m_poNext)) THROW(MBC_Queue+1);

			m_poTail->m_poNext->m_poNext = m_poHead;
		}

		m_poTail = m_poTail->m_poNext;
		m_iTail = 0;
	}
}

template <class T>
void Queue<T>::clear()
{
	m_iTail = m_iHead;
	m_poTail = m_poHead;
}

template <class T>
bool Queue<T>::get(T &data)
{
	if (m_iHead==m_iTail && m_poHead==m_poTail) 
		return false;

	data = m_poHead->m_oBuf[m_iHead];

	m_iHead++;

	if (m_iHead == 16) {
		m_iHead = 0;
		m_poHead = m_poHead->m_poNext;
	}

	return true;
}

template <class T>
Queue<T>::Queue()
{
	m_poHead = new Item ();
	if (!m_poHead) THROW(MBC_Queue+1);

	m_poHead->m_poNext = m_poHead;
	m_poTail = m_poHead;
	
	m_iHead = m_iTail = 0;
}

template <class T>
Queue<T>::~Queue()
{
	Item *p ;
	while ((p= m_poHead->m_poNext) != m_poHead) {
		m_poHead->m_poNext = p->m_poNext;
		delete p;
	}

	delete m_poHead;

	m_poHead = 0;
}

#endif
