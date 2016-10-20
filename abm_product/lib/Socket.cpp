/*VER: 1*/ 
/* Copyright (c) 2004-<2006> Linkage, All rights Reserved */
/* #ident       "@(#)Socket.cpp     1.0     2004/11/07" */
#include "Socket.h"

using namespace std;

//##ModelId=41DCB45A02BF
int Socket::listen(int iPortID, bool bForkFlag)
{

	int optval=1;
	int pid;
	struct sockaddr_in ServerAddr,NewAddr;
	int iSocketId, iNewSocketId, iNewLen;
	memset((char *)&ServerAddr,0,sizeof(ServerAddr));
	ServerAddr.sin_family      = AF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	ServerAddr.sin_port        = htons( iPortID );

	while( (iSocketId=socket(AF_INET,SOCK_STREAM,0))<0 )
	{
		if ( errno==ENOSR||errno==ENOMEM )
		{	sleep(1);
			continue;
		}
		printf("socket error\n ");
		return -1;
	}
	if( setsockopt(iSocketId,SOL_SOCKET,SO_REUSEADDR,
			(char *)&optval,sizeof(optval))!=0 )
	{	printf("setsockopt error\n");
		return -1;
	}
	if( ::bind(iSocketId,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr))<0)
	{	printf("bind error\n");
		return -1;
	}

	if( ::listen(iSocketId,5)<0 )
	{	printf("listen error\n");
		return -1;
	}
	printf ("[Message]: Listen at Port: %d\n", iPortID);

	while (1) {

#ifdef DEF_HP
		for(;;)
		{	if( (iNewSocketId=
				accept(iSocketId,&NewAddr,&iNewLen)) <0 )
			{	
				sleep(1);
				continue;
			}
			else 
			{
				m_iSocketID = iNewSocketId;
				break;
			}
		}
#else
		for(;;)
		{	if( (iNewSocketId=
				accept(iSocketId,(struct sockaddr*)&NewAddr,(socklen_t *)&iNewLen)) <0 )
			{	
				sleep(1);
				continue;
			}
			else 
			{
				m_iSocketID = iNewSocketId;
				break;
			}
		}
#endif
	
		if (bForkFlag == false) 
		{
			close (iSocketId);
			return iNewSocketId;
		} 
		else 
		{
#ifndef DEF_LINUX
			sigset (SIGCHLD, SIG_IGN);
#else
			signal (SIGCHLD, SIG_IGN);
#endif
			if ((pid = fork())==0) 
			{
				close (iSocketId);	
				return iNewSocketId;
			} 
			else  
			{
				printf ("fork process: pid=%d\n",pid);
				close (iNewSocketId);
				continue;
			}
		}
	}

}

//##ModelId=41DCB45A0338
int Socket::connect(char *sRemoteHostIP, int iRemotePortID, char *sLocalHostIP, int iLocalPort)
{
	struct sockaddr_in ServerAddr;
	struct linger mylinger;
	int iSocketId,iResult,optval=1;
	ServerAddr.sin_family         = AF_INET;
	ServerAddr.sin_addr.s_addr    = inet_addr( sRemoteHostIP );
	ServerAddr.sin_port           = htons( iRemotePortID );

	if((iSocketId=socket(AF_INET,SOCK_STREAM,0))<0)
	{	printf("Socket error\n");
		return -1;
	}

	// add by zhangch
	if (sLocalHostIP != NULL)
	{
		struct sockaddr_in saLoc;
		memset(&saLoc, 0, sizeof(struct sockaddr_in));
		saLoc.sin_family = AF_INET;
		saLoc.sin_port = htons(iLocalPort);
		saLoc.sin_addr.s_addr = inet_addr(sLocalHostIP);

		if( setsockopt(iSocketId,SOL_SOCKET,SO_REUSEADDR,
						(char *)&optval,sizeof(optval))!=0 )
		{	printf("setsockopt error\n");
			return -1;
		}

		if (bind(iSocketId, (struct sockaddr *)&saLoc, sizeof(struct sockaddr)) < 0)
		{
			printf("Bind error\n");
			return -1;
		}
	}

	while( 1 )
	{
		mylinger.l_onoff  = 1;
		mylinger.l_linger = 0;

		setsockopt(iSocketId, SOL_SOCKET, SO_LINGER, 
			(char *) &mylinger,sizeof(struct linger));

#ifndef DEF_LINUX
		sigset(SIGPIPE,SIG_IGN);
#else
		signal(SIGPIPE,SIG_IGN);
#endif
		optval = 1;
		setsockopt(iSocketId, SOL_SOCKET, SO_KEEPALIVE, 
			(char *) &optval,sizeof(int));


		iResult = /*std*/::connect(iSocketId,(struct sockaddr*)&ServerAddr,
				sizeof(ServerAddr));
		if( iResult != 0 )
		{	shutdown( iSocketId,2 );
			close( iSocketId );
			if( errno == ECONNREFUSED )
			{	sleep(5);
				continue;
			}
			printf("Connect error\n");
			return -1;
		}
		else break;
	}
	m_iSocketID = iSocketId;
	return iSocketId;
	
}

//##ModelId=41DCB45A037E
Socket::Socket()
{
	m_iSocketID = -1;
}

//##ModelId=41DCB45A0388
Socket::~Socket()
{
	m_iSocketID = -1;
}

//##ModelId=41DCB45A039C
int Socket::read(unsigned char *sBuffer, int iLen)
{
	int num=0;
	int ret;

	if (m_iSocketID <= 0) {
		printf ("Socket not connected\n");
		return -1;
	}

	while( num < iLen )
	{
		ret = /*std*/::read (m_iSocketID, &sBuffer[num], iLen-num);  
		if ( ret < 0 )
		{	printf("read error\n");
			return -1;
		} else if ( ret == 0) {
			printf ("Socket Close\n");
			return -1;
		}
		num+=ret;
	}
	return ret;
}

//##ModelId=41DCB45B0004
int Socket::write(unsigned char *sBuffer, int iLen)
{
	int num=0;
	int ret;

	if (m_iSocketID <= 0) {
		printf ("Socket not connected\n");
		return -1;
	}

	while( num < iLen )
	{
		ret = /*std*/::write (m_iSocketID, &sBuffer[num], iLen-num);
		if( ret<0 )
		{	printf("write error\n");
			return -1;
		}
		num+=ret;
	}
	return num;
}

int Socket::closeSocket()
{
	if (shutdown(m_iSocketID, 2) < 0)
	{
		return -1;
	}
	return close (m_iSocketID);
}

int Socket::selectRead(unsigned char *sBuffer, int iLen, int iSec)
{
	int num=0;
	int ret;
#ifdef	DEF_LINUX
	fd_set fds;
#else
	struct fd_set fds; 
#endif
	struct timeval timeout={iSec, 0};

	FD_ZERO(&fds); 
	FD_SET(m_iSocketID, &fds); 
	int maxfdp = m_iSocketID + 1;    // fd_set中描述符最大值加1

	switch(select(maxfdp, &fds, NULL, NULL, &timeout)) 
	{
	case -1:
		printf("select error\n");
		return -1;
		break;
	case 0:
		printf("select timeout\n");
		return 0;
		break;
	default:
		if (FD_ISSET(m_iSocketID, &fds))            
		{
			// ret = this->read(sBuffer, iLen);
			while( num < iLen )
			{
				ret = ::read (m_iSocketID, &sBuffer[num], iLen-num);  
				if ( ret < 0 )
				{	printf("read error\n");
					return -1;
				} else if ( ret == 0) {
					printf ("read data empty\n");
					return 0;
				}
				num+=ret;
			}
		}
		break;
	}

	return ret;
	
}


