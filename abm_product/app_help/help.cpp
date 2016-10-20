#include "stdio.h"
#include "Xml.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
	char filename[200];
	const char* file= NULL;
	file = getenv("ABM_PRO_DIR");
	if(file != NULL)
	{
	  sprintf (filename, "%s/etc/help.xml", file);
	}
	else
	{
		sprintf(filename,"/hss/opt/opthss/tec/help.xml");
	}
	Xml xml;
	if(!xml.load(filename))
		return 0;
	
	char argvNew[128][1024];
	for(int i=0;i<argc;i++)
	{
		 strncpy(argvNew[i],argv[i],sizeof(argvNew[i]));
	}
	xml.parse(argc,argvNew);
	
	return 1;
}
