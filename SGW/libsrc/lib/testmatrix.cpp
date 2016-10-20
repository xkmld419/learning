#include "platform.h"
#include "SimpleMatrix.h"

int main()
{
	SimpleMatrix sm(100,100);
	
	sm.setBelong (10, 11);
	sm.setBelong (11, 12);
	sm.setBelong (12, 13);
	sm.setBelong (13, 14);
	sm.setBelong (12, 15);
	sm.setBelong (15, 20);


	cout << sm.getBelong (10, 14) << endl;
	cout << sm.getBelong (10, 13) << endl;
	cout << sm.getBelong (10, 12) << endl;
	cout << sm.getBelong (10, 15) << endl;
	cout << sm.getBelong (14, 20) << endl;
	cout << sm.getBelong (10, 20) << endl;

	return 0;
}

