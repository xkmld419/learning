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
#include <string>
#include "platform.h"

//##ModelId=4314101B03E3
//##Documentation
//## TCP/IP Socket封装
class Socket
{
  public:
    //##ModelId=4314101E02A7
    //##Documentation
    //## 服务器侦听(服务端使用)
    int listen(
        //##Documentation
        //## 端口号
        int iPortID,
        //##Documentation
        //## 服务进程连接后，是否fork自身.
        bool bForkFlag = false);

    //##ModelId=4314101E02AA
    //##Documentation
    //## 连接socket(客户端使用)
    int connect(
        //##Documentation
        //## 远程服务器IP
        char *sRemoteHostIP,
        //##Documentation
        //## 远程服务器端口
        int iRemotePortID);

    int setBlock();

    //##ModelId=4314101E02AD
    Socket();

    //##ModelId=4314101E02AE
    virtual ~Socket();

    //##ModelId=4314101E02B0
    //##Documentation
    //## 读socket
    int read(
        //##Documentation
        //## 读的缓存区
        string &str
     	);

    //##ModelId=4314101E02B3
    //##Documentation
    //## 写socket
    int write(
        //##Documentation
        //## 写的缓存区
        string &str
        );
    //##ModelId=432E7B0E019F
    int read(
        //##Documentation
        //## 读取socket存放的缓冲
        unsigned char *sBuffer,
        //##Documentation
        //## 指定读取长度
        int iLen);


    //##ModelId=432E7B5303C5
    int write(
        //##Documentation
        //## 写入socket的内容地址
        unsigned char *sBuffer,
        //##Documentation
        //## 写入socket长度
        int iLen);

  private:
    //##ModelId=4314101E029F
    //##Documentation
    //## SocketID
    int m_iSocketID;
};


#endif /* SOCKET_H_HEADER_INCLUDED_BE6E2CF5 */


