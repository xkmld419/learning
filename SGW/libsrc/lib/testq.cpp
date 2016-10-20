#include "platform.h"
#include "MessageQueue.h"
#include <string.h>
#include "Exception.h"
#include <string>
#include "platform.h"

int main (int argc, char * argv[])
{
	MessageQueue	*test1,*test2;
	char	pChar[4096];
	char	pChar2[4096];
	string  str1,str2;
	int 	i;
	i=111111;
	test1 = new MessageQueue (i);
	test2 = new MessageQueue (i);
	i=0;
	test1->open(false,true,1024,10,10240);
	test2->open(false,false,1024,10,10240);
	
	memset(pChar,'E',4096);
	memset(pChar2,'O',4096);
	strcpy(pChar,"linjinxin");
	str1.assign(pChar,4096);
		
	test1->send(pChar,4096);
	test1->send(str1);
	
	 i=test2->receive(str2);
        cout<<"i="<<i<<endl;
        cout<<str2<<endl;

	i=test2->receive(pChar2);
	cout<<"i="<<i<<endl;
	for (int j=0;j<4096;j++)
		cout<<pChar2[j];
	cout<<endl;
	test2->setBlockFlag(false);
	i=test2->receive(str2);
	cout<<"i="<<i<<endl;
	cout<<str2<<endl;
	cout<<"sizeof()="<<sizeof(pChar)<<endl;
}

