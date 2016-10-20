#include "platform.h"
#include "Socket.h"
#include <string.h>
#include "Exception.h"

#include "platform.h"
int main(int argc ,char* argv[])
{
	Socket	a;
	char 	sBuf[100];
	char	sBuf1[100];
	
	
	strcpy(sBuf1,"abcdefjhijklmnEEEEEEEEEEEE");
	cout<<sBuf1<<endl;
	
	a.connect("192.168.1.200",5001);
	a.write((unsigned char *)&sBuf1,strlen(sBuf1));
	
}

