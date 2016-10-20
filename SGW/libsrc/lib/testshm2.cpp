#include <stdio.h>
#include "SimpleSHM.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
		char *p;
		int * pi;
		int i;


		SimpleSHM *shm = new SimpleSHM (0X500, 10240);
		if (!shm)
		{
			printf ("Mem\n");
			return 0;
		}

		shm->open(true);

		pi = (int *)((char *)(*shm));
		p = (char *)(*shm);

		memset (p, 0, 1000);

		while (1) {
			i = *pi;

			if (i != -1 && i != 0)
				printf ("value:%d\n", i);
			sleep (1);
		}

		shm->close ();
		delete shm;

	
	
	return 0;
}

