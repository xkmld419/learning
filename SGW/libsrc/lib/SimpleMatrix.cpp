// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SimpleMatrix.h"
#include <string.h>
#include "Exception.h"
#include "MBC.h"

//##ModelId=4248C65A0312
SimpleMatrix::SimpleMatrix(int iRow, int iCol, bool isSame)
:m_iRow(iRow), m_iCol(iCol), m_bSame(isSame)
{
	int i;
	unsigned long l = m_iRow * m_iCol + 7;
        l = (l>>3);
        m_sBuf = new char[l];
        if (!m_sBuf)
                THROW(MBC_SimpleMatrix+1);
        memset (m_sBuf, 0, l);

	for (i=0; i<8; i++) {
		m_cBuf[i] = (0X80 >> i);
	}

	m_piRetBuf = new int[iRow+1];
        if (!m_piRetBuf)
                THROW(MBC_SimpleMatrix+1);
}

//##ModelId=4248C69C01B8
void SimpleMatrix::setBelong(int x, int y)
{
	int i, j;

	if (x>m_iRow || y>m_iCol) return;

	if (m_bSame && get(y,x)) return;

	//##第一步设置x属于y 
	set (x, y);

	if (!m_bSame) return;

	if (!get(x,x)) set (x, x);
	//##第二步，归整其他的
	//##思路：当设置x属于y的时候，
	//##1，x要继承y的所有关系
	//##2，所有归属x的要继承x新产生的归属关系
	for (j=0; j<m_iRow; j++) {
		if (get(j, x))
			set (j, y);
	}

	for (i=0; i<m_iCol; i++) {
		if (get(y, i)) {
			set (x, i);
			for (j=0; j<m_iRow; j++) {
				if (get(j, x))
					set (j, i);
			}
		}
	}
}

//##ModelId=4248C79802AB
bool SimpleMatrix::getBelong(int x, int y)
{
	if (x>m_iRow || y>m_iCol) return false;

	return get (x, y);
}

//##ModelId=4248CA9E0181
SimpleMatrix::~SimpleMatrix()
{
	if (m_sBuf)
		delete [] m_sBuf;
}

//##ModelId=4248CC4E0223
void SimpleMatrix::set(int x, int y)
{
	unsigned long l = x*m_iCol + y;
	int i = l&0x7;
	l = (l>>3);
	m_sBuf[l] |= m_cBuf[i];	
}

//##ModelId=4248CC590341
int SimpleMatrix::get(int x, int y)
{
	unsigned long l = x*m_iCol + y;
	int i = l&0X7;
	l = (l>>3);
	return (m_sBuf[l] & m_cBuf[i]);
}

int * SimpleMatrix::getChilds(int y)
{
	int iTemp = 0, jTemp;

	for (jTemp=0; jTemp<m_iRow; jTemp++) {
		if (get (jTemp, y)) {
			m_piRetBuf[iTemp++] = jTemp;
		}
	}

	m_piRetBuf[iTemp++] = -1;

	return m_piRetBuf;
}

