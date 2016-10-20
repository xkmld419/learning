#include "petri.h"

namespace PETRI {

void sendMsg(char* sbody,int req) 
{
	strcpy(P_CMDMSG->mbody,sbody);
	pid_t pid = getpid();
	P_CMDMSG->mtype=(long) pid;
	if (P_MQ.cmdMQ.send()) {
		printf("send success\n");
	} else {
		printf("fail\n");
	};

	if (req) {

		int i =0;
		while (i++ <5) {
			sleep(1);
			if (P_MQ.ackMQ.receive() ) {
				printf("time = %d,%s\n",i,P_ACKMSG->mbody);
				break;
			};
		}
	};

	printf("the end\n");

};


/*

void testSend()
{
		MQ<cmd_msg> mq;
		mq.setKey(0x0010);
		cmd_msg* msg = mq.getMsg();
		msg->mtype = 0;
		strcpy(msg->mbody,"<iOperationID=1><lTransitionID=213><iNeedAck=1>");
		mq.send();

}
*/










};






int main(int argc,char** argv)
{

	int i;
	long l_operation = 4;
	long l_transition = 213;
	long l_draw= 0;


	while( (i = getopt(argc,argv,"ro:t:d:")) != -1) {
		switch(i) {
			case 'o':
				if (optarg) {
					l_operation = atol(optarg);
				}
				break;
			case 't':
				if (optarg) {
					l_transition = atol(optarg);
				}
				printf("T=%ld\n",l_transition);
				break;
			case 'd':
				if (optarg) {
					l_draw= atol(optarg);
				}
				};
	}
	char sbody[2048];
	memset (sbody,0,sizeof(sbody));
	sprintf(sbody, 
		"<iStaffID=1><iOperation=%ld><lTransitionID=%ld><lDrawID=%ld><iNeedAck=1><sTransactionID=20100728>",
		l_operation,
		l_transition,
		l_draw);

	PETRI::sendMsg(sbody,i);

	return 0;


};

