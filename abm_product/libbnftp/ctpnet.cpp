#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "ctpnet.h"

char errmsg[1024]={0};


int socket_new(int *s)
{
	*s = -1;
	*s =  socket(AF_INET,SOCK_STREAM, 0);
	return *s;
}


int socket_close(int* s)
{
/*	linger lingerStruct;
	lingerStruct.l_onoff  =1;
	lingerStruct.l_linger = 0;
	setsockopt(*s,SOL_SOCKET,SO_LINGER,(const char*)&lingerStruct,sizeof(linger));
	shutdown(*s,2);
*/	close(*s);
	*s = -1;
	return 0;
}


int socket_can_read(int s, int time_out)
{
	fd_set fd_read;
	struct timeval time_value;

	if (s == -1) return -1;

	if (time_out <= 0) time_out = 10;

	time_value.tv_sec = time_out;
	time_value.tv_usec = 0;

	FD_ZERO(&fd_read);
	FD_SET(s, &fd_read);
	if (select(s + 1, &fd_read, NULL, NULL, &time_value) <= 0)
	{
		/* 忽略对errno == EINTR的处理,但应用需要阻塞相关的信号*/
		return -1;
	}

	if (!FD_ISSET(s, &fd_read)) return -1;
	return 0;
}


int socket_can_write(int s, int time_out)
{
	int ret = -1;
	fd_set fd_write;
	struct timeval time_value;

	if (s == -1) return -1;

	if (time_out <= 0) time_out = 3;

	time_value.tv_sec = time_out;
	time_value.tv_usec = 0;

	FD_ZERO(&fd_write);
	FD_SET(s, &fd_write);
	
	ret = select(s + 1, NULL, &fd_write, NULL, &time_value);
	if (ret <= 0)
	{
		/* 忽略对errno == EINTR的处理,但应用需要阻塞相关的信号*/
		ret = errno;
		sprintf(errmsg,"%s",strerror(errno));
		return ret;
	}

	if (!FD_ISSET(s, &fd_write)) return -1;
	return 0;
}


/* 发送指定长度的数据(阻塞)*/
int socket_send_n(int s, void *buf, size_t len)
{
	int has_send_cnt = 0;
	const char *p = (const char *)buf;
	has_send_cnt= send(s, (const void *)(p),len, 0);
	return has_send_cnt;
}


/* 接收指定长度的数据(阻塞)*/
int socket_recv_n(int s, void *buf, size_t len)
{
	int has_read_cnt = 0;
	char *p = (char *)buf;
	
	while (has_read_cnt != len)
	{
		int bytes = recv(s, (void *)(p + has_read_cnt), len - has_read_cnt, 0);
		/* RETURN VALUE*/
		/*		n   Successful completion.  n is the number of bytes received.*/
		/*		0   The socket is blocking and the transport connection to the*/
		/*                remote node failed.*/
		/*		-1   Failure.  errno is set to indicate the error.*/
		int ierrno = errno;
		if (0 == bytes)
		{
			break;
		}
		else if(bytes < 0)
		{
			perror("errno msg: ");
			printf("errno is : %d",ierrno);
			break;
		}
		else
		{;}
		
		has_read_cnt += bytes;
	}
	
	/*返回接收的字符串长度*/
	return has_read_cnt;
}
