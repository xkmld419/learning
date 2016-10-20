/*VER: 1*/ 
# ifndef CALCULATOR_H
# define CALCULATOR_H

class Calculator
{
  public:
	static long roundDiv(long lValue1, long lValue2) {
        long lValue = lValue1 * 10 / lValue2;
        if (lValue > 0) {
            return (lValue+5)/10;
        } else {
            return (lValue-5)/10;
        }

        return -1;
    }

	static long roundDivEx(long lValue1, long lValue2, int & iUD) {
        long lValue = lValue1 * 10 / lValue2;

        if (lValue > 0) {
            int iTemp = lValue % 10;
            long lRet = lValue / 10;

            if (!iTemp) {
                iUD = 0;
                return lRet;
            } else if ( iTemp >= 5) {
                iUD = 1;
                return lRet + 1;
            } else {
                iUD = -1;
                return lRet;
            }
        } else {
            int iTemp = lValue % 10;
            long lRet = lValue / 10;

            if (!iTemp) {
                iUD = 0;
                return lRet;
            } else if (iTemp <= -5) {
                iUD = -1;
                return lRet - 1;
            } else {
                iUD = 1;
                return lRet;
            }
        }

        return -1;
    }
};

# endif
