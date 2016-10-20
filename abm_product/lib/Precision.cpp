/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Precision.h"
#include "Calculator.h"

int Precision::set(long * value, int iRateValue, int iCalValue, int iMode) 
{

	int k = iRateValue - iCalValue;

	int z = abs (k);

	int c = 1;
	for (int i=0; i<z; i++) {
		c *= 10;
	}

	if (k>0) {
		*value = *value * c;
		return 0;
	} else {
		long i = *value;

        switch (iMode) {
        case 0:
		    *value = Calculator::roundDiv (i, c);
		    break;
		    
		case 1:
		    *value = i / c;
		    break;
		    
		case 2:
		    *value = (i+c-1) / c;
		    break;
		}
		
		return (*value * c - i);
	}

}

int Precision::set(int * value, int iRateValue, int iCalValue, int iMode) 
{

	int k = iRateValue - iCalValue;

	int z = abs (k);

	int c = 1;
	for (int i=0; i<z; i++) {
		c *= 10;
	}

	if (k>0) {
		*value = *value * c;
		return 0;
	} else {
		int i = *value;

        switch (iMode) {
        case 0:
		    *value = Calculator::roundDiv (i, c);
		    break;
		    
		case 1:
		    *value = i / c;
		    break;
		    
		case 2:
		    *value = (i+c-1) / c;
		    break;
		}
		
		return (*value * c - i);
	}

}

//##ModelId=425506480319

/*
void Precision::set(int *iValue, int iRatePrecision, int iCalPrecision)
{
	int i, iTemp, iTemp2, iNewValue;

	iNewValue = *iValue;
	iTemp2 = abs (iRatePrecision);
	for (i=0, iTemp=1; i<iTemp2; i++)
		iTemp *= 10;

	if (iRatePrecision != 0) {
		if (iRatePrecision > 0) {
			iNewValue *= iTemp;
		} else {
			iNewValue /= iTemp;

			// 四舍五入 
			iTemp2 = iTemp/2;
			if ((*iValue)%iTemp >= iTemp2)
				iNewValue++;
		}
	}

	if (iCalPrecision > 0) {
		for (i=0,iTemp=1; i<iCalPrecision; i++)
			iTemp *= 10;

		iNewValue = iNewValue - iNewValue%iTemp;
	}

	*iValue = iNewValue;
}

*/

