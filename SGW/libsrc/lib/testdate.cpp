#include "Date.h"
#include "Precision.h"
#include "platform.h"
int main()
{
	int i;
	Date d;
	DateDiff df;

	Date x;
	Date y;

	cout << x.toString () << " " << x.getMilSec () 
	     << y.toString () << " " << y.getMilSec () << endl;

	/* test Precision */
	int iValue=12345;
	Precision::set (&iValue, 0, 0);
	cout << "iValue=" << iValue << endl;
	Precision::set (&iValue, 1, 0);
	cout << "iValue=" << iValue << endl;
	Precision::set (&iValue, -2, 0);
	cout << "iValue=" << iValue << endl;
	Precision::set (&iValue, 0, 2);
	cout << "iValue=" << iValue << endl;

	Date d1("20050103000001");
	Date d2("20050102220000") ;
	cout <<d2 - (const Date)d1<< endl;
	cout << "Diff Seconds:" << d1.diffSec(d2) << endl;

	cout << d.toString (0) << endl;
	cout << d.getMilSec () << endl;
	d.getCurDate ();
	cout << d.toString (0) << endl;
	cout << d.getMilSec () << endl;

	for (i=0; i<10; i++) {
		d.addDay (1);
		cout << "WEEK:" << d.getWeek () << endl;

		cout << "DATE:" << d.toString ("yyyyÄêmmÔÂddÈÕ hh:mi:ss") << endl;
	}

	d.parse ("2009-01-31 10:18:18", "YYYY-MM-DD HH:MI:SS");
	cout << d.toString () << endl;

	d.parse ("20060301000000");
	cout << d.toString ("yyyy-mm-dd hh:mi:ss") << endl;
	

/*
	Date d("20000101000000");
	cout << d.toString(0) << endl;
	cout << (char *)d << endl;
	return 1;
//	d.addDay (335);
//	cout << i << endl;
	try {
		for (i=1; i<50000; i++) {
			d.addSec (-1);
			cout << d.toString(0)  << " " << i << endl;
		}
	}
	catch (...) {
		cout << i << endl;
	}
	return 0;
*/
}

