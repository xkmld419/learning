/*VER: 1*/ 
#include <iostream>
#include <time.h>
#include <stdio.h>

using namespace std;

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
