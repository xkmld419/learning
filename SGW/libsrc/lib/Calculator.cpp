# include "Calculator.h"

long Calculator::roundDiv(long lValue1, long lValue2)
{
	long lValue = lValue1 * 10 / lValue2;
	if (lValue > 0) {
		return (lValue+5)/10;
	} else {
		return (lValue-5)/10;
	}

	return -1;
}


