#include "platform.h"
//#include "KeyListItem.h"
//#include "KeyList.h"
#include <string.h>
#include "Exception.h"
#define MBC 1000
#include "HashList.h"
int main ()
{
	int i;
	int j;
	HashList<int> ilist (12);
	ilist.add (1, 1);
	ilist.add (13, 2);
	cout << ilist.get (1, &i) << endl;
	cout << i << endl;
	cout << ilist.get (2, &i) << endl;
	cout << i << endl;
	ilist.add (1, 3);
	cout << ilist.get (1, &i) << endl;
	cout << i << endl;
	cout << ilist.get (13, &i) << endl;
	cout << i << endl;

	ilist.destroy();

	HashList<int> * list2 = new HashList<int>(10000);
	for (i=0; i< 1000000; i++)
		list2->add (i, i);

	j = -1;
	for (i=0; i< 1000000; i++) {
		if (!(list2->get(i, &j)) || (i!=j)) {
			cout << i << "---" << j << endl;
		}
		j = -1;
	}
			
//	list2->destroy ();
	delete list2;

	return 0;
}

