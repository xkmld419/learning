#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "platform.h"
#include <string.h>

#include "TOCIQuery.h"


#define  LOADCOUNT  1000000
#define  BUFSIZE   1000
char  c1[BUFSIZE][40];
int   c2[BUFSIZE];
char  c3[BUFSIZE][15];
char  c4[BUFSIZE][20];
char  c5[BUFSIZE][20];
char  c6[BUFSIZE][20];
char  c7[BUFSIZE][20];
char  c8[BUFSIZE][20];

int main()
{
	cout<<" fetch_test start..."<<endl;
	
	try {
		TOCIDatabase database;
		TOCIQuery * qry;
		int i,j;
		
		database.Connect("sett","sett","BILL");
		
		cout<<"Connect database success"<<endl;
		system("date");
		time_t stime;
		time(&stime);

		qry = new TOCIQuery(&database);
		
		qry->SetSQL("select a1, a2,a3,a4,a5,a6,a7,a8, a8 a9, a8 a10, a8 a11, a8 a12, a8 a13, a8 a14, a8 a15, a8 a16, a8 a17, a8 a18 from oci_test");
		qry->Open(10);
		
		i=0;
		while (qry->Next())
		{
			int a;
			
			i++;
			
#if 0			
			a = qry->Field("a1").asInteger();
			a = qry->Field("a2").asInteger();
			a = qry->Field("a3").asInteger();
			a= qry->Field("a4").asInteger();
			 a=qry->Field("a5").asInteger();
			 a=qry->Field("a6").asInteger();
			 a=qry->Field("a7").asInteger();
			 a=qry->Field("a8").asInteger();
			a = qry->Field("a9").asInteger();
			a = qry->Field("a10").asInteger();
			
			a = qry->Field("a11").asInteger();
			a = qry->Field("a12").asInteger();
			
			a = qry->Field("a13").asInteger();
			a= qry->Field("a14").asInteger();
			 a=qry->Field("a15").asInteger();
			 a=qry->Field("a16").asInteger();
			 a=qry->Field("a17").asInteger();
			 a=qry->Field("a18").asInteger();
#else
			a = qry->Field(0).asInteger();
			a = qry->Field(1).asInteger();
			a = qry->Field(2).asInteger();
			a = qry->Field(3).asInteger();
			a = qry->Field(4).asInteger();
			a = qry->Field(5).asInteger();
			a = qry->Field(6).asInteger();
			a = qry->Field(7).asInteger();
			a = qry->Field(8).asInteger();
			a = qry->Field(9).asInteger();
			a = qry->Field(10).asInteger();
			a = qry->Field(11).asInteger();
			a = qry->Field(12).asInteger();
			a = qry->Field(13).asInteger();
			a = qry->Field(14).asInteger();
			a = qry->Field(15).asInteger();
			a = qry->Field(16).asInteger();
			a = qry->Field(17).asInteger();
#endif
		}
		
		delete qry;
	
		time_t etime;
		time(&etime);

		int utime;
		utime = etime-stime;
		cout<<i<<" rows  fetched use time: "<<utime<<endl;
		
	}
	catch(TOCIException &oe)
	{
		cout<<"TOCIException catched"<<endl;
		cout<<oe.GetErrMsg()<<endl;		
		cout<<oe.GetErrSrc()<<endl;
	}
	catch(...)
	{
		cout<<"Exception occur"<<endl;
	}
	system("date");
	
	return 0;
}


