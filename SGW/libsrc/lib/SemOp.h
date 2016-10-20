#include "platform.h"
#include <errno.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>

#include "platform.h"

union semun
{
	int val;				/* value for SETVAL */ //信号量值，当执行SETVAL命令时使用
	struct semid_ds *buf;	/* buffer for IPC_STAT & IPC_SET */ //在IPC_STAT/IPC_SET命令中使用。代表了内核中使用的信号量的数据结构。
	ushort *array;			/* array for GETALL & SETALL */ //在使用GETALL/SETALL命令时使用的指针。
};

//信号量操作类
//用于实现进程间数据的同步和互斥操作
class SemOp
{
  public:
	SemOp(key_t keyvalue, int maxval);
	~SemOp();

	//初始化信号量的值为value
	//rCreate 存在的话是否重建信号量
	int initSem (int value = 1, bool rCreate = false);
	int attachSem (); //连接信号量
	int destroySem ();	//释放信号量
	int P() ;	//执行信号量操作P操作
	int V();	//执行信号量操作V操作

	void setWait(bool wait = true);

	int setVal(int val);

  private:
  	int 	iSemID;	// 信号量集的IPC 标识符
	key_t	semkey;
	bool	bIfWait;	//在没有资源时，是否锁住，等待资源
	int 	iMaxSemVal; //最大信号量值
};




