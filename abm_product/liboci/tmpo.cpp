/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>
#include <string.h>

#include "TOCIQuery.h"

#define  LOADCOUNT  200000
#define  BUFSIZE   1000

struct TestRec {
	char c1[40];
	int  c2;
	char c3[16];
	char c4[20];
	char c5[20];
	char c6[20];
	char c7[20];
	char c8[20];
};

struct TestRec  TestArray[1000];

int main()
{
	cout<<"ocitest start..."<<endl;
	
	try {
		TOCIDatabase database;
		TOCIQuery * qry;
		int i,j;
		
		database.Connect("bill","wyhfz","bill");
		
		cout<<"Connect database success"<<endl;
		system("date");
		time_t stime;
		time(&stime);
	
	for ( i=0; i<LOADCOUNT/BUFSIZE; i++){
		qry = new TOCIQuery(&database);
		
		for ( j=0; j<BUFSIZE; j++){
            char temp[40];
            
            sprintf(temp,"testpc:%d",j);
            
            strcpy ( TestArray[j].c1,temp);
            strcpy ( TestArray[j].c4,temp);
            strcpy ( TestArray[j].c5,temp);
            strcpy ( TestArray[j].c6,temp);
            strcpy ( TestArray[j].c7,temp);
            strcpy ( TestArray[j].c8,temp);
            strcpy ( TestArray[j].c3,"19980101080000");
            TestArray[j].c2 = j;
        }
#if 1		
		qry->SetSQL("insert into xzc_dp_test(c1,c2,c3,c4,c5,c6,c7,c8) values (:c1,:c2,to_date(:c3,'yyyymmddhh24miss'),:c4,:c5,:c6,:c7,:c8)");
#else
		qry->SetSQL("insert into xzc_dp_test(c1,c2,c4,c5,c6,c7,c8) values (:c1,:c2,:c4,:c5,:c6,:c7,:c8)");

#endif		
		qry->SetParamArray("c1",(char **)&(TestArray[0].c1), sizeof(TestArray[0]),40);
		qry->SetParamArray("c2",&(TestArray[0].c2), sizeof(TestArray[0]));
		qry->SetParamArray("c3",(char **)&(TestArray[0].c3), sizeof(TestArray[0]),15);
		qry->SetParamArray("c4",(char **)&(TestArray[0].c4), sizeof(TestArray[0]),20);
		qry->SetParamArray("c5",(char **)&(TestArray[0].c5), sizeof(TestArray[0]),20);
		qry->SetParamArray("c6",(char **)&(TestArray[0].c6), sizeof(TestArray[0]),20);
		qry->SetParamArray("c7",(char **)&(TestArray[0].c7), sizeof(TestArray[0]),20);
		qry->SetParamArray("c8",(char **)&(TestArray[0].c8), sizeof(TestArray[0]),20);
		
		qry->Execute(j);
		qry->Commit();
		delete qry;
	}
		time_t etime;
		time(&etime);

		int utime;
		utime = etime-stime;
		cout<<i*j<<" rows  loaded use time: "<<utime<<endl;
		
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


