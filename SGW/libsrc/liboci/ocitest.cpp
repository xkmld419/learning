#include "platform.h"
#include <cstring>
#include <string>
#include <ctime>
#include "platform.h"

#include "TOCIQuery.h"
#include "Oci_Env.h"


int main(int argc, char* argv[])
{
	int method = 0;
	string strA = "9";
	string strB = "9test";
	
	try 
	{
		
		if(argc != 2)
		{
			fprintf(stderr, "usage:%s 1(insert)/2(update)/3(deletee)/4(select)\n", argv[0]);
			exit(0);
		}
		
		method = atoi(argv[1]);
		if(method <1 || method >4)
		{
			fprintf(stderr, "usage:%s 1(insert)/2(update)/3(deletee)/4(select)\n", argv[0]);
			exit(0);
		}
		
		if(!OciEnv::getInstance()->connectDb("srd", "srd", "srd"))
		{
			fprintf(stderr,"连接数据库出错\n");
			exit(0);
		}
		
		/*
		TOCIDatabase database;
		
		int i = 3;
		int size = sizeof(i);
		
		database.connect("srd","srd","jsbill");
		
		cout << "connect database success" << endl;

		//system("date");
		
		*/
		
		time_t stime;
		time(&stime);
		
		DEFINE_OCICMD(qry);
		//TOCIQuery qry(OciEnv::getInstance()->getOCIDatabase());
		char *scripSelect;
	
	
		//qry = new TOCIQuery(&database);
		//qry = new TOCIQuery(OciEnv::getInstance()->getOCIDatabase());
		
		switch(method)
		{
			case 1:
				//insert demo
				cout << "ocitest insert 9" << endl;
				scripSelect = "INSERT INTO CHENL_TEST(A,B) VALUES(:A,:B)";
				qry.setSQL(scripSelect);
				qry.setParameter("A", strA.c_str());
				qry.setParameter("B", strB.c_str());
				qry.execute();
				//qry.commit();
				
				break;
				
			case 2:
				//update demo
				cout << "ocitest update 9" << endl;
				scripSelect = "UPDATE CHENL_TEST set b = :b WHERE A = :a";
				qry.setSQL(scripSelect);
				qry.setParameter("a", "9");
				qry.setParameter("b", "9 update test");
				qry.execute();
				//qry.commit();
				break;
				
			case 3:
				//delete demo
				cout << "ocitest delete 9" << endl;
				scripSelect = "delete CHENL_TEST WHERE A = :a";
				qry.setSQL(scripSelect);
				qry.setParameter("a", "9");
				qry.execute();
				//qry.commit();
				break;
				
			case 4:
				//select demo
				cout << "ocitest select 9" << endl;
				scripSelect = "SELECT  A,B FROM CHENL_TEST WHERE A = :a";
				qry.setSQL(scripSelect);
				qry.setParameter("a", "9");
				qry.open();
		    while (qry.next())
		    {
		    	cout << "A:" << qry.field("A").asInteger()   << endl;
		    	cout << "B:" << qry.field("B").asString() << endl;
		  	}
				break;
			default:
				fprintf(stderr, "error method:%d\n", method);
				break;
		}
		
    //delete qry;
		//OciEnv::getInstance()->commit();
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




