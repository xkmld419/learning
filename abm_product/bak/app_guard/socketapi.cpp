/*VER: 1*/ 
/**********************************
 #        Writen by RUANYJ        #
 #          3.1.2000              #
 **********************************/

//#define  SINGLE_PROCESS

#include "socketapi.h"
#include <arpa/inet.h>
/*
extern "C" {
extern __sighandler_t sigset (int __sig, __sighandler_t __disp) __THROW;
};
*/
void time_out(int a)
{
}
int ListenSocket( int HostPort )
{

	int optval=1;
	int pid;
	struct sockaddr_in ServerAddr,NewAddr;
	int SockId,NewSockId, NewLen;
	memset((char *)&ServerAddr,0,sizeof(ServerAddr));
	ServerAddr.sin_family      = AF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	ServerAddr.sin_port        = htons( HostPort );

	while( (SockId=socket(AF_INET,SOCK_STREAM,0))<0 )
	{
		if ( errno==ENOSR||errno==ENOMEM )
		{	sleep(1);
			continue;
		}
		printf ("[ERROR]: socket error\n ");
		return -1;
	}
	if( setsockopt(SockId,SOL_SOCKET,SO_REUSEADDR,
			(char *)&optval,sizeof(optval))!=0 )
	{	printf ("[ERROR]: setsockopt error\n");
		return -1;
	}
	if( bind(SockId,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr))<0)
	{	printf ("[ERROR]: bind error\n");
		return -1;
	}

	if( listen(SockId,5)<0 )
	{	printf ("[ERROR]: listen error\n");
		return -1;
	}
	printf ("[MESSAGE]: Listen at Port: %d\n", HostPort);

	while (1) {

		for(;;)
		{	
			#ifdef DEF_HP
			if( (NewSockId = accept(SockId,(struct sockaddr*)&NewAddr,/*(socklen_t *)*/ &NewLen)) <0 )
			#else
			if( (NewSockId = accept(SockId,(struct sockaddr*)&NewAddr, (socklen_t *)&NewLen)) <0 )
			#endif
			{	
				sleep(1);
				continue;
			}
			else	break;
		}
	
#ifdef  SINGLE_PROCESS
		close (SockId);
		return NewSockId;
#else
		signal (SIGCHLD, SIG_IGN);
		if ((pid = fork())==0) {
			close (SockId);	
			return NewSockId;
		} else  {
			printf ("[MESSAGE]: Socket Server fork process: pid=%d\n",pid);
			close (NewSockId);
			continue;
		}
#endif
	}

}
int ConnectSocket( char * RemoteHostIP,int RemoteHostPort)
{	
	struct sockaddr_in ServerAddr;
	struct linger mylinger;
	int SocketId,Result,optval=1;
	ServerAddr.sin_family         = AF_INET;
	ServerAddr.sin_addr.s_addr    = inet_addr( RemoteHostIP );
	ServerAddr.sin_port           = htons( RemoteHostPort );

	if((SocketId=socket(AF_INET,SOCK_STREAM,0))<0)
	{	printf ("[ERROR]: Socket error\n");
		return -1;
	}
	while( 1 )
	{
		mylinger.l_onoff  = 1;
		mylinger.l_linger = 0;

		setsockopt(SocketId, SOL_SOCKET, SO_LINGER, 
			(char *) &mylinger,sizeof(struct linger));

		signal(SIGPIPE,SIG_IGN);
		optval = 1;
		setsockopt(SocketId, SOL_SOCKET, SO_KEEPALIVE, 
			(char *) &optval,sizeof(int));


		Result = connect(SocketId,(struct sockaddr*)&ServerAddr,
				sizeof(ServerAddr));
		if( Result != 0 )
		{	shutdown( SocketId,2 );
			close( SocketId );
			if( errno == ECONNREFUSED )
			{	sleep(5);
				continue;
			}
			printf ("[ERROR]: Connect error\n");
			return -1;
		}
		else break;
	}
	return SocketId;
}
int ReadSocket( int SockId,char * Buff,int Len)
{
	int num=0;
	int ret;
/*
	//ret = recv (SockId, Buff, Len, MSG_WAITALL);
	ret = recv (SockId, Buff, Len, 0x40);
	if (ret < 0) {
		printf ("[ERROR]: Recv error\n");
		return -1;
	} else if (ret == 0) {
		printf ("[ERROR]: Socket Close\n");
		return -1;
	}
*/

	while( num < Len )
	{
		ret=read(SockId,(char *)&Buff[num],Len-num);  
		if ( ret < 0 )
		{	
			return -1;
		} else if ( ret == 0) {
			printf ("[ERROR]: Socket Close\n");
			return -1;
		}
		num+=ret;
	}

return ret;
}

int WriteSocket( int SockId,char * Buff,int Len)
{
	int num=0;
	int ret;
/*
	ret = send (SockId, Buff, Len, 0);
	if( ret<0 )
	{	printf ("[ERROR]: write error\n");
		return -1;
	}
	return ret;
*/

	while( num < Len )
	{
		ret=write(SockId,(char *)&Buff[num],Len-num);
		if( ret<0 )
		{	printf ("[ERROR]: write error\n");
			return -1;
		}
		num+=ret;
	}
	return num;

}

