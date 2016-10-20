// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef KEYTREE_H_HEADER_INCLUDED_BDCDA7DF
#define KEYTREE_H_HEADER_INCLUDED_BDCDA7DF

#include <string.h>
#include "MBC.h"
#include "KeyList.h"
#include "Stack.h"

//##ModelId=41DB838802D0
template <class T>
class KeyTree
{
  public:
    //##ModelId=41DCE8ED0395
    void add(char *key, T data);

    //##ModelId=423252A101CC
    void add(int key, T data);

    //##ModelId=41DCE8FB03C7
    void destroy();

    //##ModelId=42325311024F
    bool get(int key, T *value) const;

    //##ModelId=423253290165
    bool get(char *key, T *value) const;

    //##ModelId=423255B80093
    KeyTree();

    //##ModelId=423255B800C5
    virtual ~KeyTree();

    unsigned int getCount() {return m_iCount;}

  private:

    KeyTree<T> * m_poNext[10];
    KeyList<T> * m_poValue;
    unsigned int m_iMaxDeep;
    unsigned int m_iCount;
};

//##ModelId=41DCE8ED0395
template <class T>
void KeyTree<T>::add(char *key, T data)
{
	//##first step: find the target node
	char *p = key;
	KeyTree<T> * po = (KeyTree<T> *) this;
	int i;
	while (*p)
	{
		i = (*p) % 10;
		if (!(po->m_poNext[i]))
		{
			po->m_poNext[i] = new KeyTree<T> ();
			m_iCount++;
			if (!(po->m_poNext[i]))
				THROW(MBC_KeyTree+1);
		}

		po = po->m_poNext[i];
		p++;
	}

	//##second: add
	if (!(po->m_poValue)) {
		po->m_poValue = new KeyList<T> (key, data);
		if (!(po->m_poValue)) THROW(MBC_KeyTree+1);
		m_iMaxDeep = (m_iMaxDeep>strlen(p) ? m_iMaxDeep : strlen(p));	
	} else {
		po->m_poValue->add (key, data);
	}
}

//##ModelId=423252A101CC
template <class T>
void KeyTree<T>::add(int key, T data)
{
	//##first step: find the target node
	int p = key;
	KeyTree<T> * po = (KeyTree<T> *) this;
	int i(0);
	unsigned int j(0);

	while (p)
	{
		i = p % 10;
		
		if (!(po->m_poNext[i]))
		{
			po->m_poNext[i] = new KeyTree<T> ();
			m_iCount++;
			if (!(po->m_poNext[i]))
				THROW(MBC_KeyTree+1);
		}

		po = po->m_poNext[i];
		p = p /10;
		j++;
	}

	//##second: add
	if (!(po->m_poValue)) {
		po->m_poValue = new KeyList<T> (key, data);
		m_iMaxDeep = m_iMaxDeep>j ? m_iMaxDeep : j;
	} else {
		po->m_poValue->add (key, data);
	}
}


//##ModelId=41DCE8FB03C7
template <class T>
void KeyTree<T>::destroy()
{
	//##free memory
	int i;

	Stack<KeyTree<T> *> stack;
	Stack<int> stack1;

	KeyTree<T> * p = (KeyTree<T> *) this;

	//##不使用递归，从底层开始删

	while (p) {
		for (i=0; i<10; i++) {
			if (p->m_poNext[i]) {
				stack.push (p);
				stack1.push (i);
				p = p->m_poNext[i];
				goto _CONTINUEWHILE;
			}
		}

		if (!stack.pop (p)) {
			if (m_poValue) {
				delete m_poValue;
				m_poValue = 0;
			}
			return;
		}

		stack1.pop (i);
		delete p->m_poNext[i];
		p->m_poNext[i] = 0;

		_CONTINUEWHILE:
			;
	} //end while (p)

}

//##ModelId=42325311024F
template <class T>
bool KeyTree<T>::get(int key, T *value) const
{
	//##first step: find the target node
	int p = key;  
	KeyTree<T> * po = (KeyTree<T> *) this;
	int i;
	while (p)
	{
		i = p % 10;
		if (!(po->m_poNext[i]))
			return false;

		po = po->m_poNext[i];
		p = p / 10;
	}

	//##second: get
	if (!(po->m_poValue)) return false;

	return po->m_poValue->get(key, value);
}

//##ModelId=423253290165
template <class T>
bool KeyTree<T>::get(char *key, T *value) const
{
	
	//##first step: find the target node
	char *p = key;
	KeyTree<T> * po = (KeyTree<T> *) this;
	int i;
	while (*p)
	{
		i = (*p) % 10;
		if (!(po->m_poNext[i]))
			return false;

		po = po->m_poNext[i];
		p++;
	}

	//##second: get
	if (!(po->m_poValue)) return false;

	return po->m_poValue->get(key, value);
}

//##ModelId=423255B80093
template <class T>
KeyTree<T>::KeyTree() : m_poValue(0) , m_iMaxDeep(0), m_iCount(0)
{
	memset (m_poNext, 0, sizeof(m_poNext));
}


//##ModelId=423255B800C5
template <class T>
KeyTree<T>::~KeyTree()
{
	destroy ();
}


#endif /* KEYTREE_H_HEADER_INCLUDED_BDCDA7DF */



