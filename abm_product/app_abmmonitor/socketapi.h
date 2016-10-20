/*VER: 1*/ 
#ifndef _SOCKET_API_H_
#define _SOCKET_API_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void time_out(int a);
int ListenSocket( int HostPort );
int ConnectSocket( char * RemoteHostIP,int RemoteHostPort);
int ReadSocket( int SockId,char * Buff,int Len);
int WriteSocket( int SockId,char * Buff,int Len);

#endif
