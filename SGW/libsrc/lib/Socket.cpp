/* Copyright (c) 2004-<2006> Linkage, All rights Reserved */
/* #ident       "@(#)Socket.cpp     1.0     2004/11/07" */
#include "Socket.h"

using namespace std;

//##ModelId=4314101E02A7
int Socket::listen(int iPortID, bool bForkFlag)
{
	int optval=1;
	int pid;
	struct sockaddr_in ServerAddr,NewAddr;
	int iSocketId, iNewSocketId, iNewLen=sizeof(struct sockaddr);
	memset((char *)&ServerAddr,0,sizeof(ServerAddr));
	ServerAddr.sin_family      = AF_INET;
	ServerAddr.sin_addr.s_addr = htonl( INADDR_ANY);
	ServerAddr.sin_port        = htons( iPortID );

	while((iSocketId=socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		//ENOSR---资源不足
		//ENOMEM---内存不足
		if ( errno==ENOSR||errno==ENOMEM )
		{	sleep(1);
			continue;
		}
		printf("socket create error\n ");
		return -1;
	}
/*
	1、当有一个有相同本地地址和端口的socket1处于TIME_WAIT状态时，而你启
动的程序的socket2要占用该地址和端口，你的程序就要用到该选项。
    2、SO_REUSEADDR允许同一port上启动同一服务器的多个实例(多个进程)。但
每个实例绑定的IP地址是不能相同的。在有多块网卡或用IP Alias技术的机器可
以测试这种情况。
    3、SO_REUSEADDR允许单个进程绑定相同的端口到多个socket上，但每个soc
ket绑定的ip地址不同。这和2很相似，区别请看UNPv1。
    4、SO_REUSEADDR允许完全相同的地址和端口的重复绑定。但这只用于UDP的
多播，不用于TCP。
*/
	if(setsockopt(iSocketId,SOL_SOCKET,SO_REUSEADDR,
			(char *)&optval,sizeof(optval)) != 0)
	{
		printf("setsockopt SO_REUSEADDR error\n");
		return -1;
	}
	if( ::bind(iSocketId,(struct sockaddr*)&ServerAddr,iNewLen)<0)
	{	printf("bind error\n");
		return -1;
	}
	if( ::listen(iSocketId,5)<0 )
	{	printf("listen error\n");
		return -1;
	}
	printf ("[Message]: Listen at Port: %d\n", iPortID);

	while (1)
	{
		for(;;)
		{
			if((iNewSocketId=
#ifdef DEF_HP
				accept(iSocketId,(struct sockaddr*)&NewAddr, &iNewLen)) <0 )
#else

				accept(iSocketId,(struct sockaddr*)&NewAddr,(socklen_t *)&iNewLen)) <0 )
#endif
			{
				printf("accept eror :%s\n",strerror(errno));
				sleep(1);
				continue;
			}
			else
			{
				m_iSocketID = iNewSocketId;
				break;
			}
		}
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

//##ModelId=4314101E02AA
int Socket::connect(char *sRemoteHostIP, int iRemotePortID)
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
		{
			printf("connect error:%s",strerror(errno));
			shutdown( iSocketId,2 );
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

//设置非阻塞方式
int Socket::setBlock()
{
	int optval = 1;
	fcntl(m_iSocketID , F_GETFL , &optval);
 	fcntl(m_iSocketID , F_SETFL , optval|O_NONBLOCK);

 	return 0;
}

//##ModelId=4314101E02AD
Socket::Socket()
{
	m_iSocketID = -1;
}

//##ModelId=4314101E02AE
Socket::~Socket()
{
	close(m_iSocketID);
	m_iSocketID = -1;
}

//##ModelId=4314101E02B0
int Socket::read(string &str)
{
	int num=0;
	int ret;
	char	sLen[14];
	int		iBufLen;
	char	* data;

	if (m_iSocketID <= 0)
	{
		printf ("Socket not connected\n");
		return -1;
	}

	while( num < 14 )
	{
		ret = /*std*/::read (m_iSocketID, &sLen[num], 14-num);
		if ( ret < 0 )
		{
			printf("read error:%s\n",strerror(errno));
			return -1;
		}
		else if ( ret == 0)
		{
			printf ("Socket Close\n");
			return -2;
		}
		num+=ret;
	}
	sLen[13]='\0';

	if ( strncmp(sLen,"LJX",3) )
	{
		printf("socket read validated error !\n");
		return -1;
	}

	iBufLen=atoi(sLen+3);

	str.resize(iBufLen,0);

	data=(char *)str.data();

	num=0;

	while( num < iBufLen )
	{
		ret = /*std*/::read (m_iSocketID, &data[num], iBufLen-num);
		if ( ret < 0 )
		{
			printf("read error:%s\n",strerror(errno));
			return -1;
		}
		else if ( ret == 0)
		{
			printf ("Socket Close\n");
			return -2;
		}
		num+=ret;
	}

	return num;
}

//##ModelId=4314101E02B3
int Socket::write(string &str)
{
	int num=0;
	int ret;
	char	sLen[14];
	const	char	*data;
	int	iLen;

	if (m_iSocketID <= 0) {
		printf ("Socket not connected\n");
		return -1;
	}

	iLen=str.size();
	data=str.data();

	if (iLen==0)
		return iLen;

	sprintf(sLen,"LJX%-10d",iLen);
	sLen[13]='\0';


	while(num<14)
	{
		ret = ::write (m_iSocketID,&sLen[num],14-num);
		if (ret <0 )
		{
			printf("write error:%s\n", strerror(errno));
//			printf("write error:%d\n", errno);
			return -1;
		}
		num+=ret;
	}

	num=0;

	while( num < iLen )
	{
		ret = /*std*/::write (m_iSocketID, &data[num], iLen-num);
		if( ret<0 )
		{
			printf("write error:%s\n",strerror(errno));
			return -1;
		}
		num+=ret;
	}
	return num;
}

//##ModelId=432E7B0E019F
int Socket::read(unsigned char *sBuffer, int iLen)
{
	int st;
	string str;
	st = read(str);
	if (st < 0)
		return st;
	else if (str.size()>iLen)
	{
		printf("read buffer len > buffer len\n");
		return -1;
	}
	else
	{
		memcpy(sBuffer,str.data(),str.size());
		return str.size();
	}
}

//##ModelId=432E7B5303C5
int Socket::write(unsigned char *sBuffer, int iLen)
{
	string str;
	str.assign((char *)sBuffer,iLen);
	return write(str);
}

