#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <setjmp.h>
#include <signal.h>
 #include <iostream>
 
#include <atmi.h>
#include <fml32.h>
#include <userlog.h>
#include <vector>
#include "tblflds.h"


#define MAX_XML_SIZE 2048*2048*15
//using namespace std;
int main(int argc, char *argv[])
{
	FBFR32 *sendbuf, *rcvbuf;
	
	long ret, numread = 0;
	char sOrderListID[13]={0}; 
	char sAreaCode[10]={0};
	char sInXML[MAX_XML_SIZE];
	char sOutXML[MAX_XML_SIZE];
	
	char sFileName[30];	
	long lResult;
	char sErrMsg[1000];
	struct  timeb  tp1,tp2;
	memset(sOutXML, 0, MAX_XML_SIZE);
	memset(sErrMsg, 0, 1000);
	memset(sFileName, 0, 30);
	strcpy(sFileName, (char*)argv[1]);
	
	printf("you input  fileName is <%s>\n",  sFileName);

	FILE *fp;
	if((fp = fopen(sFileName, "r")) == NULL)
	{
		printf("can't open file: %s\n", sFileName);
		return 0;
	}
	numread = fread(sInXML, sizeof(char), MAX_XML_SIZE, fp);
	printf("read size is %d\n", numread);
	fclose(fp);

	long rcvlen = 0L;
	
	if ((sendbuf = (FBFR32*)tpalloc("FML32", NULL,MAX_XML_SIZE ))==NULL)
	{
		printf("分配发送buff error\n");
	}
	
	if ((rcvbuf = (FBFR32*)tpalloc("FML32", NULL, MAX_XML_SIZE))==NULL)
	{
		printf("分配接收buff error\n");
	}
	
	rcvlen = Fsizeof32(rcvbuf);
	//初始化	
	//if ((ret=tpinit((TPINIT * )NULL))<0)
	if ((ret=tpinit(NULL))<0)
	{
		printf( "tpinit fail\n");
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		return -1;
	}
	
	Fchg32(sendbuf, IN_XML, 0, (char *)&sInXML, 0L);
	ftime(&tp1);
		
	ret=tpcall("POWER_CTL_RUN",(char*)sendbuf,0L,(char**)&rcvbuf,&rcvlen,0L);
	ftime(&tp2);
	
	if (Fgets32(rcvbuf, OUT_XML, 0, sOutXML) == -1)
	{
		printf( " OUT_XML error \n");
	}
	if (Fget32(rcvbuf, IRESULT, 0, (char*)&lResult, 0) == -1)
	{
		printf( " IRESULT error \n");
	}
	if (Fgets32(rcvbuf, SMSG, 0, sErrMsg) == -1)
	{
		printf( " SMSG error \n");
	}
	
	printf("InXML is %s\n", sInXML);
	printf("OutXML is : %s\n", sOutXML);
	printf("Result is : %ld\n", lResult);
	printf("ErrMsg is : %s\n", sErrMsg); 
	printf("  用时: %d ms\n",  tp2.time*1000+tp2.millitm - tp1.time*1000 -tp1.millitm );

	return 0;
}
