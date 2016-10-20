#include "basesock.h"

int C_BaseSock::Socket(int ProtocolFamily,int SocketType,int Protocol)
{
	return socket(ProtocolFamily,SocketType,Protocol);
}

int C_BaseSock::Bind(int SocketHandle,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength)
{
	return bind(SocketHandle,SocketAddress,SocketAddressLength);
}

int C_BaseSock::Listen(int SocketHandle,int BackLog)
{
	return listen(SocketHandle,BackLog);
}

#if defined(aCC) || defined(CC)
int C_BaseSock::Accept(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength)
{
	return accept(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

#if defined(xlC) || defined(gcc) || defined(DEF_LINUX)
int C_BaseSock::Accept(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength)
{
	return accept(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

int C_BaseSock::Connect(int SocketHandle,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength)
{
	return connect(SocketHandle,SocketAddress,SocketAddressLength);
}

int C_BaseSock::Close(int SocketHandle)
{
	return close(SocketHandle);
}

int C_BaseSock::Select(int nfds,fd_set *ReadHandle,fd_set *WriteHandle,fd_set *ExceptHandle,struct timeval *TimeOut)
{
	return select(nfds,ReadHandle,WriteHandle,ExceptHandle,TimeOut);
}

int C_BaseSock::Recv(int SocketHandle,void *Buffer,size_t Bytes,int Flags)
{
	return recv(SocketHandle,Buffer,Bytes,Flags);
}

int C_BaseSock::Send(int SocketHandle,const void *Buffer,size_t Bytes,int Flags)
{
	return send(SocketHandle,Buffer,Bytes,Flags);
}

#if defined(aCC) || defined(CC)
int C_BaseSock::Recvfrom(int SocketHandle,void *Buffer,size_t Bytes,int Flags,struct sockaddr *SocketAddress,int *SocketAddressLength)
{
	return recvfrom(SocketHandle,(char *)Buffer,Bytes,Flags,SocketAddress,SocketAddressLength);
}
#endif

#if defined(xlC) || defined(gcc)
int C_BaseSock::Recvfrom(int SocketHandle,void *Buffer,size_t Bytes,int Flags,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength)
{
	return recvfrom(SocketHandle,(char *)Buffer,Bytes,Flags,SocketAddress,SocketAddressLength);
}
#endif

int C_BaseSock::Sendto(int SocketHandle,const void *Buffer,size_t Bytes,int Flags,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength)
{
	return sendto(SocketHandle,(char *)Buffer,Bytes,Flags,SocketAddress,SocketAddressLength);
}

#if defined(aCC) || defined(CC)
int C_BaseSock::Getpeername(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength)
{
	return getpeername(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

#if defined(xlC) || defined(gcc)
int C_BaseSock::Getpeername(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength)
{
	return getpeername(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

#if defined(aCC) || defined(CC)
int C_BaseSock::Getsockname(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength)
{
	return getsockname(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

#if defined(xlC) || defined(gcc)
int C_BaseSock::Getsockname(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength)
{
	return getsockname(SocketHandle,SocketAddress,SocketAddressLength);
}
#endif

int C_BaseSock::SetSocketOpt(int SocketHandle,int level,int optname,const void *optval,socklen_t optlen)
{
	return setsockopt(SocketHandle,level,optname,optval,optlen);
}

#if defined(aCC) || defined(CC)
int C_BaseSock::GetSocketOpt(int SocketHandle,int level,int optname,void *optval,int *optlenptr)
{
	return getsockopt(SocketHandle,level,optname,optval,optlenptr);
}
#endif

#if defined(xlC) || defined(gcc)
int C_BaseSock::GetSocketOpt(int SocketHandle,int level,int optname,void *optval,socklen_t *optlenptr)
{
	return getsockopt(SocketHandle,level,optname,optval,optlenptr);
}
#endif

int C_BaseSock::Isfdtype(int SocketHandle,int fdtype)
{
	struct stat Buff;

	if (fstat(SocketHandle,&Buff))
	{
		return -1;
	}
	if ((Buff.st_mode & S_IFMT) == fdtype)
	{
		return 1;
	}
	return 0;

}

int C_BaseSock::inet_pton(int ProtocolFamily,const char *PointAddress,void *IPAddress)
{
	if (ProtocolFamily == AF_INET)
	{
		in_addr_t Address;

		if((Address = inet_addr(PointAddress)) == (in_addr_t) -1)
		{
			return 0 ;
		}
		((struct in_addr*)IPAddress)->s_addr = Address;
		return 1;
	}
	return -1;
}

char *C_BaseSock::inet_ntop(int ProtocolFamily,const void *IPAddress,char *PointAddress,size_t Length)
{
	if(ProtocolFamily== AF_INET)
	{
		const unsigned char *Address = (const u_char *)IPAddress;
		sprintf(PointAddress,(char *)"%d.%d.%d.%d",Address[0],Address[1],Address[2],Address[3]);
		return PointAddress;
	}
	return NULL;
}

size_t C_BaseSock::WriteN(int SocketHandle,const void *Buffer,size_t Bytes)
{
	size_t	Left,Finished;
	const char *lv_chBuffer = (const char *)Buffer;
	Left = Bytes;
	Finished = 0;

	while (Left > 0)
	{
		if (IsReadyToWrite(SocketHandle) == false)
		{
			return 0;
		}
		if ( (Finished = write(SocketHandle,lv_chBuffer,Left)) <= 0)
		{
			break;
		}
		Left -= Finished;
		lv_chBuffer += Finished;
	}
	return Bytes - Left;
}

size_t C_BaseSock::ReadN(int SocketHandle,void *Buffer,size_t Bytes)
{
	size_t	Left,Finished;
	char *lv_chBuffer = (char *)Buffer;
	Left = Bytes;
	
	while(Left > 0)
	{
		if (IsReadyToRead(SocketHandle) == false)
		{
			return 0;
		}
		if ( (Finished = read(SocketHandle,lv_chBuffer,Left)) <= 0)
		{
			break;
		}
		Left -= Finished;
		lv_chBuffer += Finished;
	}
	return Bytes - Left;
}

size_t C_BaseSock::ReadLine(int SocketHandle, void *Buffer, size_t MaxLength)
{
	int iResult;
	int i;
	char c, *lv_chBuffer = NULL;

	lv_chBuffer = (char *)Buffer;
	for (i = 1; i < MaxLength; i++, lv_chBuffer++)
	{
		if (IsReadyToRead(SocketHandle) == false)
		{
			return 0;
		}
		iResult = read(SocketHandle, &c, 1);
		if(1 == iResult)
		{
			if (c == '\n')
			{
				break;
			}
			*lv_chBuffer = c;
		}
		else if(iResult <= 0)
		{
			if(iResult == EINTR)
			{
				continue;
			}
			else
			{
				return 0;
			}
		}
	}
	*lv_chBuffer = 0;
	return(i - 1);
}

size_t C_BaseSock::WriteLine(int SocketHandle,void *Buffer,size_t MaxLength)
{
	char c = '\n';
	if (IsReadyToWrite(SocketHandle) == true)
	{
		if(write(SocketHandle,Buffer,strlen((char *)Buffer)) == -1)
		{
			return 0;
		}
		if(write(SocketHandle,&c,1) == -1)
		{
			return 0;
		}
		return strlen((char *)Buffer) + 1;
	}
	else
	{
		return 0;
	}
}

bool C_BaseSock::IsReadyToRead(int SocketHandle)
{
	fd_set ReadHandle;
	struct timeval TimeOut;

	FD_ZERO(&ReadHandle);
	FD_SET(SocketHandle,&ReadHandle);

	TimeOut.tv_sec=180;
	TimeOut.tv_usec=0;
	if (select(SocketHandle + 1,&ReadHandle,NULL,NULL,&TimeOut) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool C_BaseSock::IsReadyToWrite(int SocketHandle)
{
	fd_set WriteHandle;
	struct timeval TimeOut;

	FD_ZERO(&WriteHandle);
	FD_SET(SocketHandle,&WriteHandle);

	TimeOut.tv_sec=180;
	TimeOut.tv_usec=0;
	if (select(SocketHandle + 1,NULL,&WriteHandle,NULL,&TimeOut) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------The End--------------------------------

