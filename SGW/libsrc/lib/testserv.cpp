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
	int 	i;
		if (a.listen(5001,true)<0){
			printf("error!\n");
			exit(-1);
		}
		
		i=a.read((unsigned char *)&sBuf1,10);
		if (i<0){
			exit(-1);
		}
		sBuf1[i]='\0';
		cout<<sBuf1<<endl;
	
}

