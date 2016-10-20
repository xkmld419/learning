/*VER: 1*/ 
#include <iostream>
#include <list>
#include <string>

int main(int argc, char * argv[])
{
	list<string *> str_list;
	string s1;
	
	int i;
	
	for ( i=0;i<10000000; i++){
		
		str_list.push_back(new string("abc"));
		
		if ( i%1000000 == 0){
			cout<<"i="<<i<<endl;
			system("date");
		}
	}
}
