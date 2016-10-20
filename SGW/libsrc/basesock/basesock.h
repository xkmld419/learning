#ifndef _BASESOCK_H_
#define	_BASESOCK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <string.h>

class C_BaseSock
{
	public:
		int Socket(int ProtocolFamily,int SocketType,int Protocol);
		int Bind(int SocketHandle,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength);
		int Listen(int SocketHandle,int BackLog);

		#if defined(aCC) || defined(CC)
		int Accept(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength);
		#endif

		#if defined(xlC) || defined(gcc) ||  defined(DEF_LINUX)
		int Accept(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength);
		#endif

		int Connect(int SocketHandle,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength);
		int Close(int SocketHandle);
		int Select(int SocketHandle,fd_set *ReadHandle,fd_set *WriteHandle,fd_set *ExceptHandle,struct timeval *TimeOut);

		/*适用TCP协议*/
		int Recv(int SocketHandle,void *Buffer,size_t Bytes,int Flags);
		int Send(int SocketHandle,const void *Buffer,size_t Bytes,int Flags);

		/*适用UDP协议*/
		#if defined(aCC) || defined(CC)
		int Recvfrom(int SocketHandle,void *Buffer,size_t Bytes,int Flags,struct sockaddr *SocketAddress,int *SocketAddressLength);
		#endif

		#if defined(xlC) || defined(gcc)
		int Recvfrom(int SocketHandle,void *Buffer,size_t Bytes,int Flags,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength);
		#endif

		int Sendto(int SocketHandle,const void *Buffer,size_t Bytes,int Flags,const struct sockaddr *SocketAddress,socklen_t SocketAddressLength);

		/*周边函数*/

		#if defined(aCC) || defined(CC)
		int Getpeername(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength);
		#endif

		#if defined(xlC) || defined(gcc)
		int Getpeername(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength);
		#endif

		#if defined(aCC) || defined(CC)
		int Getsockname(int SocketHandle,struct sockaddr *SocketAddress,int *SocketAddressLength);
		#endif

		#if defined(xlC) || defined(gcc)
		int Getsockname(int SocketHandle,struct sockaddr *SocketAddress,socklen_t *SocketAddressLength);
		#endif

		int SetSocketOpt(int SocketHandle,int level,int optname,const void *optval,socklen_t optlen);

		#if defined(aCC) || defined(CC)
		int GetSocketOpt(int SocketHandle,int level,int optname,void *optval,int *optlenptr);
		#endif

		#if defined(xlC) || defined(gcc)
		int GetSocketOpt(int SocketHandle,int level,int optname,void *optval,socklen_t *optlenptr);
		#endif

		int Isfdtype(int SocketHandle,int fdtype);

		int inet_pton(int ProtocolFamily,const char *PointAddress,void *IPAddress);
		char *inet_ntop(int ProtocolFamily,const void *IPAddress,char *PointAddress,size_t len);

		/*TCP扩展读写函数*/
		size_t WriteN(int SocketHandle,const void *Buffer,size_t Bytes);
		size_t ReadN(int SocketHandle,void *Buffer,size_t Bytes);

		size_t WriteLine(int SocketHandle,void *Buffer,size_t MaxLength);
		size_t ReadLine(int SocketHandle,void *Buffer,size_t MaxLength);

		/*读写检测*/
		bool IsReadyToRead(int SocketHandle);
		bool IsReadyToWrite(int SocketHandle);
};

typedef C_BaseSock CBaseSock;

#endif//_BASESOCK_H_

//-----------------------------------The End---------------------------------------




