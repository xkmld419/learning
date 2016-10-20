#include <stdio.h>
#include <string.h>
#include "atmi.h"




int main(int argc,char** argv) 
{




	if (tpinit((TPINIT *) NULL) == -1) {
		printf("init err\n");
		return -1;
	};

	char*sendbuf,*recvbuf;

	if( (sendbuf = (char*) tpalloc("STRING",NULL,1025)) == NULL ) {
		printf("tpalloc err\n");
		return -1;
	}

	if( (recvbuf = (char*) tpalloc("STRING",NULL,1025)) == NULL ) {
		printf("tpalloc err\n");
		return -1;
	}

	if (argc >1) {
		sprintf(sendbuf,"<lTransactionID=201007001><iOperation=1><lTransitionID=%s><iNeedAck=1>",argv[1]);
	}else {
		//strcpy(sendbuf,"<iOperation=1><lTransitionID=213><iNeedAck=1>");
		strcpy(sendbuf,"<lTransactionID=201007001><lTransitionID=214><sSrcState=D><sDescState=E><iOperation=4>");
	}

	long length;

	int ret = tpcall("PETRI_FIRE",(char*) sendbuf,0,(char**)&recvbuf,&length,(long)0);
	if (ret == -1 ) {
		printf("tpcall err\n");
		return -1;
	} else {
		printf("%s\n",recvbuf);
	}

	return 0;


}
