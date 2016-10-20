#include "platform.h"
#include <time.h>
#include <stdio.h>

#include "platform.h"

int main()
{
	cout << "Time test:" << endl;
	time_t tbegin;
	time(&tbegin);
	sleep(2);
	time_t tend;
	time(&tend);
	cout << "Spand time:" << tend-tbegin << endl;
}

