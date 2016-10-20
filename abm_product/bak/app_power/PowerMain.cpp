#include "PowerXml.h"

using namespace std;

int main(int argc,char *argv[])
{


	char xml_buffer[40000];
	char xml_return[40000];

	char sAccNbr[20];
	int iArea;
	int nsize;
	std::string sBOName, sStaff, sSite;		
	memset(xml_buffer,0,sizeof(xml_buffer));
	
	FILE *fp=fopen(argv[1],"r");
	if (fp == NULL ) {
		printf("open file error. file:<%s>\n",argv[1]);
		return 0;
	}
	nsize=fread(xml_buffer,sizeof(xml_buffer),1,fp);
	printf("buf length=%d\n",strlen(xml_buffer));
	fclose(fp);

char sIp[124]={0};
char sOnlineIp[124]={0};
	PowerXml a;
//a.getVpdnIPByImsi("460031234567890",sIp,sOnlineIp);
	
	a.PowerXmlProcess(xml_buffer,xml_return);

//	a.PowerXmlProcess(xml_buffer,xml_return);
	return 0;
}


