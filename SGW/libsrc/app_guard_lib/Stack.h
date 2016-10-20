// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef STACK_H_HEADER_INCLUDED_BDB70A11
#define STACK_H_HEADER_INCLUDED_BDB70A11

#include "CommonMacro.h"
#include "MBC.h"
#include "common.h"

//##ModelId=4247E46F02D5
//##Documentation
//## 栈的模板类，等lib checkin了之后放在lib
template <class T>
class Stack
{
  public:
    //##ModelId=4248C1FF0250
    class Item
    {
        friend class Stack;

      public:
        //##ModelId=4248C2080086
        Item();

      private:
        //##ModelId=4248C212017B
        T m_oBuf[16];
	Item * m_poNext;
	Item * m_poPre;
    };

    //##ModelId=4247E497020A
    void push(T data);

    //##ModelId=4247E4B4016B
    bool pop(T &data);

    bool empty();

    //##ModelId=4248C1D00036
    Stack();

    //##ModelId=4248C1D40028
    ~Stack();

  private:
    //##ModelId=4247E53F0298
    int m_iCur;

    //##ModelId=4248C24101BF
    Item *m_poCur;

};



//##ModelId=4248C2080086
template <class T>
Stack<T>::Item::Item():m_poNext(0),m_poPre(0)
{
}

//##ModelId=4247E497020A
template <class T>
void Stack<T>::push(T data)
{
	m_poCur->m_oBuf[m_iCur++] = data;
	if (m_iCur == 16) {
		if (!(m_poCur->m_poNext)) {
			m_poCur->m_poNext = new Item;
			if (!(m_poCur->m_poNext))
				THROW(MBC_Stack+1);
		}

		m_poCur = m_poCur->m_poNext;
		m_iCur = 0;
	}
}

template <class T>
bool Stack<T>::empty()
{
	if (m_iCur || m_poCur->m_poPre) return false;
	return true;
}

//##ModelId=4247E4B4016B
template <class T>
bool Stack<T>::pop(T &data)
{
	if (!m_iCur) {
		if (m_poCur->m_poPre) {
			m_poCur = m_poCur->m_poPre;
			data = m_poCur->m_oBuf[15];
			m_iCur = 15;
			return true;
		}
		return false;
	} else {
		m_iCur--;
		data = m_poCur->m_oBuf[m_iCur];
		return true;
	}
}

//##ModelId=4248C1D00036
template <class T>
Stack<T>::Stack()
{
	m_poCur = new Item;
	if (!m_poCur)
		THROW(MBC_Stack+1);
	m_iCur = 0;
}

//##ModelId=4248C1D40028
template <class T>
Stack<T>::~Stack()
{
	Item *p ;
	while ((p= m_poCur->m_poNext)!=NULL) {
		m_poCur->m_poNext = p->m_poNext;
		delete p;
	}

	while ((p=m_poCur->m_poPre)!=NULL) {
		m_poCur->m_poPre = p->m_poPre;
		delete p;
	}

	delete m_poCur;
}

#endif /* STACK_H_HEADER_INCLUDED_BDB70A11 */



