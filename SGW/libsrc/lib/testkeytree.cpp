#include "platform.h"

#include "platform.h"

#include "KeyTree.h"

int main()
{
	int i;
	KeyTree<int> tree;

	int min=100000000;
	int max=100000010;
	int j = 0;

	tree.add ("200050_05238387736", 9);

	cout << tree.get ("200050_05238387736", &i) << endl;
	cout << i << endl;

	tree.destroy ();
	return 0;


	for (i=min; i<max;  i+=97) {
		tree.add (i, i);
		j++;
	}

	cout << "NUMBERS:" << tree.getCount () << endl;
	cout << "ITEMS:" << j << endl;

	cout << tree.get(1, &i) << endl;
	cout << i << endl;
	cout << tree.get(2, &i) << endl;
	cout << i << endl;

	
	tree.destroy ();


	cin >> i;


	cout << "haha" << endl;

	cin >> i;
	
	return 0;
}

