#include "petri.h"

namespace PETRI{

struct cmd {

	virtual void doCmd() = 0;
	void exeCmd();
	//static shm   ; //共享内存操作部分
	//static MQ<cmd_msg>* recvmq; //消息队列操作部分
	//static MQ<cmd_msg>* sendmq; //消息队列操作部分
	cmd(){};

};


struct cmd_fire:public cmd { //点火请求

	virtual void doCmd() ;

};

struct cmd_stop:public cmd {

	virtual void doCmd() ;
};


struct cmd_query:public cmd {

	virtual void doCmd() ;
};

struct cmd_changeState:public cmd {

	virtual void doCmd() ;

};


struct commander {

	commander();
	~commander();
	void applyCMD();
	cmd* getCmd(int req_id);
	void initCmd();

	private:
		cmd* cmdBuf[8];





};



}

