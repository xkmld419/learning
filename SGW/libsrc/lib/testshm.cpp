#include <stdio.h>
#include "SimpleSHM.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
		char *p;
		int * pi;
		pid_t pid;
		int i;
		char s[10240];
		char s1[10240];

		SimpleSHM *shm = new SimpleSHM (0X500, 10240);
		if (!shm)
		{
			printf ("Mem\n");
			return 0;
		}

		shm->open(true);

		pi = (int *)((char *)(*shm));
		p = (char *)(*shm);

		while (1) {
			*pi = -1;
		}


		shm->close();
		
		delete shm;
	
	
	return 0;
}

