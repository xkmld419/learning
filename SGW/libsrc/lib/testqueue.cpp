#include "Queue.h"
#include "platform.h"

#include "platform.h"

int main ()
{
	Queue<int> queue;

	int i, j;

	for (i=0; i<100; i++) {
		queue.put (i);
	}

	while (queue.get (j)) {
		cout << j << endl;
	}

	cout << endl;

	return 0;
}

