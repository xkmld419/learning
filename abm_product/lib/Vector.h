/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef VECTOR_H_HEADER
#define VECTOR_H_HEADER

#include "CommonMacro.h"
#include "Exception.h"
#include "MBC.h"
#include "Stack.h"
#include "Queue.h"

template <class T>
class Vector
{
  public:
    class Item
    {
      public:
        Item();

        T m_oBuf[256];
    };

    class Node
    {
      public:
        Node();
        Node * child(int i) {
            return (Node *)(m_poChild[i]);
        }

        void * m_poChild[256];
    };

    inline int put(T & data);

    inline int push_back(T & data) {
        put (data);
    }

    inline T &  operator [] (int iSub);

    inline int  size();
    inline void clear();

    Vector();
    ~Vector();

  private:
    inline void calSubSerial(int iSub);

  private:
	int m_iTail;
	int m_iCount;
    int m_iSubSerial[4];

    Node * m_poNode;
    Item * m_poCurItem;
};

template <class T>
Vector<T>::Item::Item()
{
    memset (m_oBuf, 0, sizeof (m_oBuf));
}

template <class T>
Vector<T>::Node::Node()
{
    memset (m_poChild, 0, sizeof (m_poChild));
}

template <class T>
inline void Vector<T>::clear()
{
    m_iCount = 0;
    m_iTail = 0;
    m_poCurItem = (Item *)(m_poNode->child(0)->child(0)->child(0));
}

template <class T>
inline T & Vector<T>::operator [] (int iSub)
{
    calSubSerial (iSub);

#   define SS m_iSubSerial

    return ((Item *)(m_poNode->child(SS[0])->child(SS[1])->child(SS[2])))->m_oBuf[SS[3]];

#   undef SS
}

template <class T>
inline void Vector<T>::calSubSerial(int iSub)
{
    m_iSubSerial[3]  = (iSub & 255);
    iSub             = iSub >> 8;

    m_iSubSerial[2]  = (iSub & 255);
    iSub             = iSub >> 8;

    m_iSubSerial[1]  = (iSub & 255);
    iSub             = iSub >> 8;

    m_iSubSerial[0]  = iSub;
}

template <class T>
inline int Vector<T>::put(T & data)
{
    m_poCurItem->m_oBuf[m_iTail++] = data;
    m_iCount++;

    if (m_iTail != 256) return m_iCount-1;

    m_iTail = 0;

    calSubSerial (m_iCount);
    
    Node * pNode = m_poNode;
    
#   define SS m_iSubSerial

    for (int j=0; j<2; j++) {
        if (!pNode->m_poChild[SS[j]]) {
            pNode->m_poChild[SS[j]] = new Node ();
            if (!pNode->m_poChild[SS[j]]) THROW (3);
        }

        pNode = pNode->child(SS[j]);
    }

    //if (pNode->m_poChild[SS[2]]) THROW (4);

    if (!pNode->m_poChild[SS[2]]) {
        pNode->m_poChild[SS[2]] = new Item ();
        if (!pNode->m_poChild[SS[2]]) THROW (5);
    }

    m_poCurItem = (Item *)(pNode->m_poChild[SS[2]]);

#   undef SS

    return m_iCount-1;
}

template <class T>
Vector<T>::Vector() 
{
    m_iTail = m_iCount = 0;

    Node * pNode[3];

    for (int j=0; j<3; j++) {
        pNode[j] = new Node ();
        if (!pNode[j]) THROW (1);
    }

    m_poNode = pNode[0];

    pNode[0]->m_poChild[0] = pNode[1];
    pNode[1]->m_poChild[0] = pNode[2];

    m_poCurItem =  new Item ();
    if (!m_poCurItem) THROW (2);

    pNode[2]->m_poChild[0] = m_poCurItem;
}

template <class T>
inline int Vector<T>::size()
{
    return m_iCount;
}

template <class T>
Vector<T>::~Vector()
{
    Node * pNode = m_poNode;
    Stack<Node *> stack;
    Queue<Node *> queue;

    stack.push (pNode);
    queue.put  (pNode);
    int i = 3;
    
    while (queue.get (pNode)) {
        if (i==1) {
            for (int j=0; j<256; j++) {
                if (pNode->m_poChild[j]) {
                    delete (Item *)(pNode->m_poChild[j]);
                    pNode->m_poChild[j] = 0;
                }
            }
        } else {
            for (int j=0; j<256; j++) {
                if (pNode->m_poChild[j]) {
                    queue.put  ((Node *)(pNode->m_poChild[j]));
                    stack.push ((Node *)(pNode->m_poChild[j]));
                }
            }
            i--;
        }
    }

    while (stack.pop (pNode)) {
        delete pNode;
    }
}

#endif
