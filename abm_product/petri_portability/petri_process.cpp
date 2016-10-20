#include "petri_process.h"


int petri_process::run()
{
	setErrMsg();
	setResult(0);

	Pprintf(0,0,0,"Hello World!\n");
	sleep(10);
	Pprintf(0,0,0,"Hello World!\n");
	sleep(10);

	setResult(1);
	setErrMsg("fatel error!");

	return 1;

}

DEFINE_MAIN(petri_process);

