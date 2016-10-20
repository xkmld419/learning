#include "ParserXml.h"

using namespace std;

int main(int argc,char *argv[])
{
	char xml_buffer[40000];	
	int iRet=0;
	int nsize;
	std::string sBOName, sStaff, sSite;		
	memset(xml_buffer,0,sizeof(xml_buffer));
	
	FILE *fp=fopen(argv[1],"r");
	if (fp == NULL ) return 0;
	nsize=fread(xml_buffer,sizeof(xml_buffer),1,fp);
	printf("buf length=%d\n",strlen(xml_buffer));
	fclose(fp);
	ParserXml dom;
	iRet = dom.ProcessMessage(xml_buffer,1);
	
	return 0;
}


