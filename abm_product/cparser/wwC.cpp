/*VER: 1*/ 
#include "TScriptParser.h"
#include "exception.h"
#include <unistd.h>
//#define SPEED_TEST



class MyParser;

class MyParser: public ScriptParser 
{
public:
    double Sysinput(const char * name)
	{ 
	  cout<<"sysinput:"<<name<<endl;
	if ( strcmp ( name, "F0")==0)
		return 100;
	
    	if ( strcmp ( name, "F1") ==0)
    		return 50;

	if (strcmp (name, "T00_00_00") == 0)
		return 20;
    	return 0;
	};
    char * SysinputS(const char * name)
    {
    	if ( strcmp ( name, "S0")==0)
    	cout<<"S0 : "<<endl;
    		return "s0";
    	
    	if ( strcmp ( name, "S1") ==0)
    	cout<<"S1 : "<<endl;
    		return "s1";
    	return "";
    };
};


//double Sysinput(const char * name)

//³ÌÐòÈë¿Ú
int main (int argc, char * argv[])
{
	int  result,i;
	MyParser * parser;


	for  (i=0; ; i++) {
	parser = new MyParser;
		if ( argc>1){
			parser->SetSource(argv[1], strlen(argv[1]));
		}
#ifndef SPEED_TEST		
		cout<<"parse..."<<endl;
#endif		
			parser->Parse();
	

		
		try {
			//parser->Intp.print();			//parser->SetInputHandler(Input);
		
			//parser->PrintCommandFlow();
			
#ifndef SPEED_TEST		
			cout<<"run..."<<endl;
#endif		
			parser->Run();
			parser->StartRet();
			while( parser->NextRet()){
				cout<<"name:"<<parser->Ret->getName()<<" value:"<<parser->Ret->asInteger()<<endl;
			}
#ifndef SPEED_TEST		
			cout<<"ok!"<<endl;
			cout<<"R0 value:"<<parser->Result("R0")->asString()<<endl;
			cout<<"R1 value:"<<parser->Result("R1")->asInteger()<<endl;
#endif
		if (i%100000 == 0){
			cout<<"i="<<i<<endl;
			system("date");
			cout<<"R0 value:"<<parser->Result("R0")->asInteger()<<endl;
		}
		
		}
		catch (TException e)
		{
			cout<<"Output exception:"<<e.GetErrMsg()<<endl;
		};
#ifndef SPEED_TEST
		parser->Clear();
		delete parser;
		break;	
		
#endif
    delete parser;
    usleep(100);
	}
	
	
	return 0;
}
