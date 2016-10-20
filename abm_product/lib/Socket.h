/*VER: 1*/ 
/* Copyright (c) 2004-<2006> Linkage, All rights Reserved */
/* #ident       "@(#)Socket.h     1.0     2004/11/07" */
#ifndef SOCKET_H_HEADER_INCLUDED_BE6E2CF5
#define SOCKET_H_HEADER_INCLUDED_BE6E2CF5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

//##ModelId=41DB83F60012
//##Documentation
//## TCP/IP Socket封装
class Socket
{
  public:
    //##ModelId=41DCB45A02BF
    //##Documentation
    //## 服务器侦听(服务端使用)
    int listen(
        //##Documentation
        //## 端口号
        int iPortID, 
        //##Documentation
        //## 服务进程连接后，是否fork自身.
        bool bForkFlag = false);

    //##ModelId=41DCB45A0338
    //##Documentation
    //## 连接socket(客户端使用)
    int connect(
        //##Documentation
        //## 远程服务器IP
        char *sRemoteHostIP, 
        //##Documentation
        //## 远程服务器端口
        int iRemotePortID,
		char *sLocalHostIP = NULL,
		int iLocalPort = 0);

    //##ModelId=41DCB45A037E
    Socket();


    //##ModelId=41DCB45A0388
    virtual ~Socket();

    //##ModelId=41DCB45A039C
    //##Documentation
    //## 读socket
    int read(
        //##Documentation
        //## 读取socket存放的缓冲
        unsigned char *sBuffer, 
        //##Documentation
        //## 指定读取长度
        int iLen);

    //##ModelId=41DCB45B0004
    //##Documentation
    //## 写socket
    int write(
        //##Documentation
        //## 写入socket的内容地址
        unsigned char *sBuffer, 
        //##Documentation
        //## 写入socket长度
        int iLen);

	// add by zhangch
	int closeSocket();
	
	// add by zhangch
	int selectRead(unsigned char *sBuffer, int iLen, int iSec);

  private:
    //##ModelId=41DCB45A028D
    //##Documentation
    //## SocketID
    int m_iSocketID;

};



#endif /* SOCKET_H_HEADER_INCLUDED_BE6E2CF5 */
