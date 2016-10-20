/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef PRECISION_H_HEADER_INCLUDED_BDAA833B
#define PRECISION_H_HEADER_INCLUDED_BDAA833B

#include <stdlib.h>
#include <stdio.h>

//##ModelId=4255060C001A
class Precision
{
  public:
    //##ModelId=425506480319
    //0: 不变; >0 有入; <0 有舍
    //iMode: 0四舍五入; 1:向下取整; 2:向上取整
    static int set(int *iValue, int iRatePrecision, int iCalPrecision, 
        int iMode=0);

    static int set(long *lValue, int iRatePrecision, int iCalPrecision, 
        int iMode=0);
};



#endif /* PRECISION_H_HEADER_INCLUDED_BDAA833B */
