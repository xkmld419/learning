#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "Log.h"

//#include "petri.h"

namespace PETRI {
template <typename MSGBUF >
struct MQ {
	//MQ(int key)iKey(key),ifOpen(false) {};
	MQ(key_t key);
	MQ();
	void setKey(key_t key);
	bool open();
	void checkopen();
	bool send();
	bool receive(long mtype = 0);
	MSGBUF* getMsg();
	void cpyMsg(MSGBUF* mb);
	bool ifOpen;

	private:
		void checkBuf();
		MSGBUF msgbuf;
		int bufsize;
		int msgID;
		int iMaxNum;
		key_t msgKey;
};

};


template <typename MSGBUF >
::PETRI::MQ<MSGBUF>::MQ(key_t key)
{
	checkBuf();
	msgKey = key;
	iMaxNum = 512;
	ifOpen = false;
	checkopen();
};

template <typename MSGBUF >
::PETRI::MQ<MSGBUF>::MQ()
{
	checkBuf();
	iMaxNum = 512;
	ifOpen = false;
};

template <typename MSGBUF >
void ::PETRI::MQ<MSGBUF>::setKey(key_t key)
{
	msgKey = key;
	ifOpen = open();
	checkopen();
};



template <typename MSGBUF >
void 
::PETRI::MQ<MSGBUF>::cpyMsg(MSGBUF* mb)
{
	memcpy (&msgbuf,mb,sizeof (MSGBUF));
	return;
}


template <typename MSGBUF >
MSGBUF* 
::PETRI::MQ<MSGBUF>::getMsg() 
{
	return &msgbuf;
};

template <typename MSGBUF >
void 
::PETRI::MQ<MSGBUF>::checkBuf() 
{
	assert( (void *)&(msgbuf.mtype)== (void*)&(msgbuf));
	return;
};

template <typename MSGBUF >
void
::PETRI::MQ<MSGBUF>::checkopen() 
{

	//ifOpen = ifOpen?ifOpen:open() ;
	if (ifOpen) return;
	else ifOpen = open();
	return;

};

template <typename MSGBUF >
bool
::PETRI::MQ<MSGBUF>::open() 
{

	int ret;
	struct msqid_ds attr;
	msgID = msgget(msgKey,0666|IPC_CREAT);

	if ( msgID != -1 ) {
		return true;
	}

	if (errno != ENOENT) {
		return false;
	}

	msgID = msgget(msgKey,0666|IPC_CREAT|IPC_EXCL);
	if( msgID == -1) {
		return false;
	}
	attr.msg_qbytes = iMaxNum*sizeof(MSGBUF);
	ret = msgctl (msgID,IPC_SET,&attr);

	return true;
};

template <typename MSGBUF >
bool
::PETRI::MQ<MSGBUF>::send() 
{
	int ret;
	//pid_t pid= getpid();
	//*((long*)(&msgbuf)) = pid;

	ret = msgsnd(msgID,&msgbuf,sizeof(MSGBUF),IPC_NOWAIT);

	if (ret == -1 ){
		printf("%s\n",strerror(errno));
		Log::log (0, "%d\n",msgID);
		Log::log (0, "%s\n",strerror(errno));
		Log::log (0, "%d\n",IPC_NOWAIT);
		Log::log (0, "%s\n",&msgbuf);		
		
		return false;
	}

	return true;

};

template <typename MSGBUF >
bool
::PETRI::MQ<MSGBUF>::receive(long mtype) 
{
	int ret;

	memset(&msgbuf,sizeof(MSGBUF),0);
	ret = msgrcv(msgID,&msgbuf,sizeof(MSGBUF),mtype,IPC_NOWAIT|MSG_NOERROR);

	if (ret == -1 ){
		return false;
	}

	return true;
};

