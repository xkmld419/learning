/*VER: 1*/ 
#include <iostream.h>
#include <stdlib.h>
#include <time.h>
#include "TOCIDirPath.h"

int main(int argc,char **argv)
{
	cout<<"dptest start..."<<endl;
	
	try {
		TOCIDatabase database;
		
		database.Connect("sett","fzsett","sett");
		
		cout<<"Connect database success"<<endl;
		system("date");
		time_t stime;
		time(&stime);
		int i,j;

	for (  j=0; j<10; j++){
		

		TOCIDirPath	dirpath(&database,"xzc_dp_test");
		
		dirpath.setSchameName("sett");
		
		cout<<"dirpath create success"<<endl;
		
		
		dirpath.prepare();
		
		cout<<"dirpath prepare ok"<<endl;
		
		char strTmp[256];
		

		for( i=0;i<100000;i++)
		{
		    /*if ( i && i%1000==0)
		        dirpath.save();
			*/
			sprintf(strTmp,"test%d",i);
			dirpath.append();
#if 1
			dirpath.setField("C1",strTmp);
			dirpath.setField("C2",(long)i);
			dirpath.setField("C3","19980601080000");
			dirpath.setField("C4",strTmp);
			dirpath.setField("C5",strTmp);
			dirpath.setField("C6",strTmp);
			dirpath.setField("C7",strTmp);
			dirpath.setField("C8",strTmp);
#else
			dirpath.setField((unsigned )0,strTmp);
			dirpath.setField((unsigned )1,i);
			dirpath.setField((unsigned )2,"19980601080000");
			dirpath.setField((unsigned )3,strTmp);
			dirpath.setField((unsigned )4,strTmp);
			dirpath.setField((unsigned )5,strTmp);
			dirpath.setField((unsigned )6,strTmp);
			dirpath.setField((unsigned )7,strTmp);

#endif
			/* */

			//dirpath.save();
	
			/*dirpath.finish();	
			dirpath.prepare();
			*/
		}
		
		cout<<"begin save"<<endl;

		dirpath.save();
		
		cout<<"begin finish"<<endl;

		dirpath.finish();	
		cout<<"finish!"<<endl;

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
