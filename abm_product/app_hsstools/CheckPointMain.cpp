#include "CheckPoint.h"

int main()
{
	CheckPoint *p = new CheckPoint();
	if(p)
	{
		return p->run();
	}
	return 0;
}
