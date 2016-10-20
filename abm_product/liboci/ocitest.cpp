/*VER: 1*/ 
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;

#include "TOCIQuery.h"


int main()
{
	cout << "ocitest start..." << endl;
	
	try 
	{
		TOCIDatabase database;
		TOCIQuery * qry;
		
		database.connect("BILL","BILL","");
		
		cout << "connect database success" << endl;

		//system("date");
		time_t stime;
		time(&stime);
	

		qry = new TOCIQuery(&database);
		
                char *scripSelect= "SELECT 1234567890 ID FROM DUAL";
		
                qry->setSQL(scripSelect);
		qry->open();
                qry->next();    

                cout << "ID-Float:   " << qry->field("ID").asFloat()   << endl;
                cout << "ID-Integer: " << qry->field("ID").asInteger() << endl;
                cout << "ID-String:  " << qry->field("ID").asString()  << endl;

                delete qry;

		time_t etime;
		time(&etime);

		int utime;
		utime = etime-stime;
        }

	catch(TOCIException &oe)
	{
		cout<<"TOCIException catched"<<endl;
		cout<<oe.getErrMsg()<<endl;		
		cout<<oe.getErrSrc()<<endl;
	}
	catch(...)
	{
		cout<<"Exception occur"<<endl;
	}
	// system("date");
	
	return 0;
}



