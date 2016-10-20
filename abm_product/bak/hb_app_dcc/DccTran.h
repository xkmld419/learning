// ver 2.0
#ifndef __DCC_TRAN_H
#define __DCC_TRAN_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <stdio.h>

#include "ParamDefineMgr.h"
#include "Log.h"
#include "Socket.h"
#include "interrupt.h"
#include "StdEvent.h"
#include "mntapi.h"
#include "Environment.h"
#include "MBC.h"
#include "EventSender.h"
#include "EventReceiver.h"
#include "IpcKey.h"
#include "CSemaphore.h"
#include "SHMData.h"
#include "Date.h"

#include "OcpMsgParser.h"
#include "dcc_ParamsMgr.h"
#include "dcc_ConstDef.h"

#define DWRINTERVAL 20
#define CCANOTSEND  30

class DccTran
{
public:
	DccTran();
	~DccTran();

	static int run();  // 创建子进程和孙进程

private:
	static int  receiveDccMsg();
	static int  sendDccMsg();
	static int  sendCER();
	static int  sendDWR();
	static int  sendDPR();

public:
	static int  init();
	static bool prepare(int argc, char **argv);

private:
	static void printManual();
	static void getPack(char *buf, char *sPack, int iPackLen);
	static void sigQuit(int signo);
	static void *getShmAddr(key_t shmKey, int shmSize);
};

#endif


