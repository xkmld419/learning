/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>
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
		
		qry->SetSQL("select to_char(a1) a1,to_char(a2) a2,to_char(a3) a3,to_char(a4) a4,to_char(a5) a5,to_char(a6) a6,to_char(a7) a7,to_char(a8) a8 from oci_test");
		qry->Open(10);
		
		i=0;
		while (qry->Next())
		{
			char  a[100];
			
			i++;
			
			
			strcpy (a , qry->Field("a1").asString());
			strcpy (a , qry->Field("a2").asString());
			strcpy (a , qry->Field("a3").asString());
			strcpy (a , qry->Field("a4").asString());
			strcpy (a , qry->Field("a5").asString());
			strcpy (a , qry->Field("a6").asString());
			strcpy (a , qry->Field("a7").asString());
			strcpy (a , qry->Field("a8").asString());
			
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

