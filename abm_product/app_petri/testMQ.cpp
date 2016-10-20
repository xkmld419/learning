#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdio.h> 
#include <string.h>

struct cmd_msg {
	long mtype;
	char mbody[2048];
};

int main()
{
	key_t key= 0x0010;
	cmd_msg msg;
	msg.mtype = 1;
	strcpy(msg.mbody,"hello");
	int msgid;
	msgid = msgget(key,0666|IPC_CREAT);
	printf("msgid=%d\n",msgid);
	if (msgid == -1 ) {
		printf("errno = %d,errmsg=%s\n",errno,strerror(errno));
	}

	int ret = msgsnd(msgid,&msg,sizeof(cmd_msg),IPC_NOWAIT);
	if ( ret == -1 ) {
		printf("errno = %d,errmsg=%s\n",errno,strerror(errno));
	};



	return 0;
};
