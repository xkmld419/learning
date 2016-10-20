#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <limits.h>
#include "ctpconf.h"
#include "ctpnet.h"
#include "ctplib.h"
#include "ctpbuf.h"

int ctp_errno;
int socket_errno;

static int ctp_timeout;
static int iblock = 0;


int ctp_send(int s, ctp_pkg_t *p)
{
	int size = p->size;

	if (size > CTP_PKG_HEAD_SIZE + CTP_PKG_BODY_SIZE)
	{
		return CTP_ERROR;
	}
	p->size = htonl(p->size);

	if (socket_can_write(s, ctp_timeout) == 0 &&
		socket_send_n(s, p, size) == size )
	{
		return CTP_OK;
	}
	else
	{
		return CTP_ERROR;
	}
}


int ctp_recv(int s, ctp_pkg_t *p)
{
	int size = 0;
	
	memset(p, 0, CTP_PKG_HEAD_SIZE + CTP_PKG_BODY_SIZE);
	
	//begin
	if (socket_can_read(s, ctp_timeout) == 0 )
	{
		; 
	}
	else
	{
		return CTP_ERROR;
	}
	
	if (socket_recv_n(s, (void *)&size, sizeof(size)) == sizeof(size))
	{
		size = ntohl(size); 
	}
	else
	{
		return CTP_ERROR;
	}
	
	//end
	
	/*
	if (socket_can_read(s, ctp_timeout) == 0 &&
		socket_recv_n(s, (void *)&size, sizeof(size)) == sizeof(size))
	{
		size = ntohl(size); 
	}
	else
	{
		return CTP_ERROR;
	}
	*/

	if (size > CTP_PKG_HEAD_SIZE + CTP_PKG_BODY_SIZE)
	{
		return CTP_ERROR;
	}
	p->size = size;
	
	if(socket_recv_n(s, (void *)((char *)p + sizeof(int)), size-sizeof(int)) > 0)
	{
		return CTP_OK;
	}
	else
	{
		return CTP_ERROR;
	}
}


int ctp_init(ctp_conn_t* conn, int timeout,int block)
{
	iblock = block;
	ctp_timeout = timeout;
	ctp_memzero(conn, sizeof(ctp_conn_t));
	
	return CTP_OK;
}


int ctp_connect(ctp_conn_t* conn, const char* host, const char* port) 
{
	int sock;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		ctp_errno = CTP_ERR_SOCKET;
		return CTP_ERROR;
	}
	conn->sock = sock;
	
	conn->addr.sin_family = AF_INET;
	conn->addr.sin_port = htons(atoi(port));
	conn->addr.sin_addr.s_addr = inet_addr(host);

	if (connect(conn->sock, (sockaddr*)&conn->addr, sizeof(conn->addr)) < 0) 
	{
		close(conn->sock);
		conn->sock = -1;
		ctp_errno = CTP_ERR_CONNECT;
		socket_errno=errno;
		return CTP_ERROR;
	}
	
	// recv welcome infomation
	if (ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		close(conn->sock);
		conn->sock = -1;
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	
	if (conn->pkg.type != CTP_RES_OK)
	{
		close(conn->sock);
		conn->sock = -1;
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	return CTP_OK;
}


int ctp_auth(ctp_conn_t* conn, const char* usr, const char* pwd) 
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_USER;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(usr) + 1;
	strcpy(conn->pkg.body, usr);

	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_PASS;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(pwd) + 1;
	strcpy(conn->pkg.body, pwd);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	return CTP_OK;
}


int ctp_pasv(ctp_conn_t* conn, char* ip, char* port)
{
	char* token = ":-";
	
	// 请求数据端口
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_PASV;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + 1;
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	// 解析数据ip和端口号
	if (strcmp(strtok(conn->pkg.body, token), "227"))
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	strncpy(ip, strtok(NULL, token), CTP_MAX_IP_SIZE);
	strncpy(port, strtok(NULL, token), CTP_MAX_PORT_SIZE);
	
	return CTP_OK;
}

int myround( double x )
{
  if( x > 0.0 )
          return (int)( x + 0.5 );
  else
          return (int)( x - 0.5 );
}

int ctp_put(ctp_conn_t* conn, const char* local_path, const char* remote_path, const char* timestamp, 
	long offset)
{
	FILE* fd;
	int read_bytes;
	int data_sock;
	char data_ip[CTP_MAX_IP_SIZE];
	char data_port[CTP_MAX_PORT_SIZE];
	struct sockaddr_in data_addr;
	
	if (ctp_pasv(conn, data_ip, data_port) != CTP_OK)
	{
		return CTP_ERROR;
	}
	
	// 连接数据端口
	if (socket_new(&data_sock) < 0)
	{
		ctp_errno = CTP_ERR_SOCKET;
		return CTP_ERROR;
	}
	
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(atoi(data_port));
	data_addr.sin_addr.s_addr = inet_addr(data_ip);
	
	if (connect(data_sock, (sockaddr*)&data_addr, sizeof(data_addr)) < 0) 
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_CONNECT;
		return CTP_ERROR;
	}
	
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_PUT;
	if (offset)
	{
		sprintf(conn->pkg.body, "%s %s %d", remote_path, timestamp, offset);
	}
	else
	{
		sprintf(conn->pkg.body, "%s %s", remote_path, timestamp);
	}
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(conn->pkg.body) + 1;
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		socket_close(&data_sock);
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	if (!(fd = fopen(local_path, "r")) || fseek(fd, offset, SEEK_SET))
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_FOPEN;
		return CTP_ERROR;
	}
	
	//设置是否是非阻塞模式，iblock ，khw
	double start_time =0, end_time = 0;
			
	if(iblock != 0)
	{
		//struct timeval timeout = {0,0};
		//timeout.tv_sec = ctp_timeout;
		
		//setsockopt(data_sock,SOL_SOCKET,SO_SNDTIMEO,\
		          //(char *)&timeout,sizeof(struct timeval)); 
		          
		start_time = get_time();
	}
			
	do 
	{
		pack_fill_default(conn->pkg);
		read_bytes = fread(conn->pkg.body, 1, CTP_PKG_BODY_SIZE, fd);
		if (read_bytes <= 0)
		{
			if (!feof(fd))
			{
				fclose(fd);
				socket_close(&data_sock);
				
				ctp_errno = CTP_ERR_FREAD;
				return CTP_ERROR;
			}
			else
			{
				break;
			}
		}
		else
		{
			conn->pkg.type = CTP_DATA_FILE;
			conn->pkg.size = CTP_PKG_HEAD_SIZE + read_bytes;
			
			if (ctp_send(data_sock, &conn->pkg) != CTP_OK)
			{
				fclose(fd);
				socket_close(&data_sock);
				
				ctp_errno = CTP_ERR_IO;
				return CTP_ERROR;
			}
			
			//khw
			if(iblock != 0)
			{
				end_time = get_time();
				int Rate;
				
				if(end_time ==0 || start_time == 0) 
					Rate =0;
				else
					Rate = end_time - start_time;
					
				if(Rate > ctp_timeout)
				{
					fclose(fd);
				  socket_close(&data_sock);
				
				  ctp_errno = CTP_PUT_TIME_OUT;
				  return CTP_ERROR;
				}
			}
			
		}
	} while(1);
	
	fclose(fd);
	socket_close(&data_sock);
	
	// 等待结束确认包
	if (ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}

	return CTP_OK;
}	

double  get_time()
{
	struct timeb tm = {0};
	double Req_Begin=0,Req_Begin_ms=0,Res_Begin_Time=0;
	ftime(&tm);
	Req_Begin = tm.time;
	//Req_Begin_ms = tm.millitm;
	//Res_Begin_Time = ((long)Req_Begin*1000 +(long)Req_Begin_ms);
	return Req_Begin;
}

int ctp_get(ctp_conn_t* conn, const char* remote_path, const char* local_path)
{
	FILE* fd;
	long offset;
	int write_bytes;
	int data_sock;
	char data_ip[CTP_MAX_IP_SIZE];
	char data_port[CTP_MAX_PORT_SIZE];
	char* token = "-:";
	struct sockaddr_in data_addr;
	
	if (ctp_pasv(conn, data_ip, data_port) != CTP_OK)
	{
		return CTP_ERROR;
	}
	
	// 连接数据端口
	if (socket_new(&data_sock) < 0)
	{
		ctp_errno = CTP_ERR_SOCKET;
		return CTP_ERROR;
	}
	
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(atoi(data_port));
	data_addr.sin_addr.s_addr = inet_addr(data_ip);
	
	if (connect(data_sock, (sockaddr*)&data_addr, sizeof(data_addr)) < 0) 
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_CONNECT;
		return CTP_ERROR;
	}
	
	if (!(fd = fopen(local_path, "ab+")) ||
		fseek(fd, 0, SEEK_END) ||
		(offset = ftell(fd)) < 0)
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_FOPEN;
		return CTP_ERROR;
	}
	
	// 请求文件
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_GET;
	if (!offset)
	{
		conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(remote_path) + 1;
		strcpy(conn->pkg.body, remote_path);
	}
	else 
	{
		sprintf(conn->pkg.body, "%s %d", remote_path, offset);
		conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(conn->pkg.body) + 1;
	}
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		socket_close(&data_sock);
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	do 
	{
		if(ctp_recv(data_sock, &conn->pkg) != CTP_OK)
		{
			break;
		}
		
		write_bytes = conn->pkg.size - CTP_PKG_HEAD_SIZE;
		if (fwrite(conn->pkg.body, 1, write_bytes, fd) != write_bytes)
		{
			socket_close(&data_sock);
			fclose(fd);
			
			ctp_errno = CTP_ERR_FWRITE;
			return CTP_ERROR;
		}
			
	} while(1);
	
	fclose(fd);
	socket_close(&data_sock);
	
	// 等待结束确认包
	if (ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		unlink(local_path);
		
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	return CTP_OK;
}


int ctp_quit(ctp_conn_t* conn)
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_QUIT;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + 1;
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}

	return CTP_OK;
}


int ctp_cwd(ctp_conn_t* conn, const char* remote_path)
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_CWD;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(remote_path) + 1;
	strcpy(conn->pkg.body, remote_path);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return ctp_errno;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return ctp_errno;
	}
	
	return CTP_OK;
}


int ctp_pwd(ctp_conn_t* conn, char* current_path)
{
	char* token = "-";
	
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_PWD;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + 1;
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	// 解析回复包体
	if (strcmp(strtok(conn->pkg.body, token), "257"))
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	strcpy(current_path, strtok(NULL, token));
	
	return CTP_OK;
}


int ctp_mkdir(ctp_conn_t* conn, const char* new_path)
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_MKDIR;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(new_path) + 1;
	strcpy(conn->pkg.body, new_path);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return ctp_errno;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return ctp_errno;
	}
	
	return CTP_OK;
}


static int ctp_parse_list(char* str, int max_num, ctp_file_attr_t* list)
{
	char* p1 = str;
	char* p2;
	char* token = " ";
	int file_num = 0;
	
	while (p1 && *p1 && max_num--)
	{
		if((p2 = strchr(p1, '\r')) && *(p2+1) == '\n')
		{
			*p2 = 0;
			p2 = p2 + 2;
		}
		
		list[file_num].type = *strtok(p1, token);
		list[file_num].size = atoi(strtok(NULL, token));
		strcpy(list[file_num].time, strtok(NULL, token));
		strcpy(list[file_num].name, strtok(NULL, token));
		
		++file_num;
		p1 = p2;
	}
	
	return file_num;
}


int ctp_nlist(ctp_conn_t* conn, const char* remote_path, int max_num, 
	ctp_file_attr_t* list, int* num)
{
	int data_sock;
	int body_size;
	char data_ip[CTP_MAX_IP_SIZE];
	char data_port[CTP_MAX_PORT_SIZE];
	struct sockaddr_in data_addr;
	ctp_buf_t body_buf;

	if (ctp_pasv(conn, data_ip, data_port) != CTP_OK)
	{
		return CTP_ERROR;
	}
	
	// 连接数据端口
	if (socket_new(&data_sock) < 0)
	{
		ctp_errno = CTP_ERR_SOCKET;
		return CTP_ERROR;
	}
	
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(atoi(data_port));
	data_addr.sin_addr.s_addr = inet_addr(data_ip);
	
	if (connect(data_sock, (sockaddr*)&data_addr, sizeof(data_addr)) < 0) 
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_CONNECT;
		return CTP_ERROR;
	}
	
	// 请求文件列表信息
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_LIST;
	if (remote_path)
	{
		conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(remote_path) + 1;
		strcpy(conn->pkg.body, remote_path);	
	}
	else
	{
		conn->pkg.size = CTP_PKG_HEAD_SIZE;
	}
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		socket_close(&data_sock);
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		socket_close(&data_sock);
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	if (ctp_buf_create(&body_buf, 4096) == -1)
	{
		return CTP_ERROR;
	}
	
	do 
	{
		if (ctp_recv(data_sock, &conn->pkg) != CTP_OK)
		{
			break;
		}
		
		if (!(body_size = conn->pkg.size - CTP_PKG_HEAD_SIZE))
		{
			break;
		}
		
		ctp_buf_append(&body_buf, conn->pkg.body, body_size);
	} while(1);
	
	socket_close(&data_sock);
	
	// 等待结束确认包
	if (ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_buf_destroy(&body_buf);
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_buf_destroy(&body_buf);
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	// 解析列表
	ctp_buf_append(&body_buf, "\0", 1);
	*num = ctp_parse_list(ctp_buf_ptr(&body_buf), max_num, list);
	ctp_buf_destroy(&body_buf);
	
	return CTP_OK;
}


int ctp_list(ctp_conn_t* conn, const char* remote_path, ctp_file_attr_t* list, int* num)
{
	return ctp_nlist(conn, remote_path, INT_MAX, list, num);
}



static int ctp_renm_or_link(ctp_conn_t* conn, char type, const char* old_path, const char* new_path)
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = type;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(old_path) + strlen(new_path) + 2;
	sprintf(conn->pkg.body, "%s %s", old_path, new_path);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	return CTP_OK;
}


int ctp_renm(ctp_conn_t* conn, const char* old_path, const char* new_path)
{
	return ctp_renm_or_link(conn, CTP_CMD_RENM, old_path, new_path);
}


int ctp_link(ctp_conn_t* conn, const char* old_path, const char* new_path)
{
	return ctp_renm_or_link(conn, CTP_CMD_LINK, old_path, new_path);
}


int ctp_delf(ctp_conn_t* conn, const char* delf_path)
{
	pack_fill_default(conn->pkg);
	conn->pkg.type = CTP_CMD_DELF;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(delf_path) + 1;
	strcpy(conn->pkg.body, delf_path);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	return CTP_OK;
}


static int ctp_time_or_size(ctp_conn_t* conn, char type, const char* remote_path, char* output)
{
	char* token = "-";
	
	pack_fill_default(conn->pkg);
	conn->pkg.type = type;
	conn->pkg.size = CTP_PKG_HEAD_SIZE + strlen(remote_path) + 1;
	strcpy(conn->pkg.body, remote_path);
	
	if (ctp_send(conn->sock, &conn->pkg) != CTP_OK ||
		ctp_recv(conn->sock, &conn->pkg) != CTP_OK)
	{
		ctp_errno = CTP_ERR_IO;
		return CTP_ERROR;
	}
	if (conn->pkg.type != CTP_RES_OK)
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	// 解析回复包体
	if (strcmp(strtok(conn->pkg.body, token), "250"))
	{
		ctp_errno = get_err_code(conn->pkg);
		return CTP_ERROR;
	}
	
	strcpy(output, strtok(NULL, token));
	
	return CTP_OK;
}


int ctp_time(ctp_conn_t* conn,const char* remote_path, char* time)
{
	return ctp_time_or_size(conn, CTP_CMD_TIME, remote_path, time);
}
	

int ctp_size(ctp_conn_t* conn, const char* remote_path, long* size)
{
	char ascii_size[32];
	if (ctp_time_or_size(conn, CTP_CMD_SIZE, remote_path, ascii_size) != CTP_OK)
	{
		return CTP_ERROR;
	}
	*size = atol(ascii_size);
	
	return CTP_OK;
}


int ctp_close(ctp_conn_t* conn)
{
	if (conn->sock != -1)
	{
		socket_close(&conn->sock);
	}
	
	return CTP_OK;
}

/*
int main(int argc, char* argv[])
{
	ctp_conn_t conn[500];
	int fail_num = 0;
	
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		if (ctp_connect("136.160.25.145", "11103", &conn[i]) != CTP_OK ||
			ctp_auth(&conn[i], "ctest2", "test2") != CTP_OK)
		{
			++fail_num;
			printf("%04d [FAIL]\n", i+1);
		}
		else
		{
			printf("%04d [SUCC]\n", i+1);
		}
	}
	
	printf("%d conn, %d fail\n", atoi(argv[1]), fail_num);
	
	return 1;
}
*/

/*
int main()
{
	ctp_conn_t conn;
	
	if (ctp_connect("136.160.25.145", "11103", &conn) != CTP_OK)
		return -1;
	printf("[OK] ctp_connect\n");
		
	if (ctp_auth(&conn, "ctest2", "test2") != CTP_OK)
		return -1;
	printf("[OK] ctp_auth\n");
		
	// if (ctp_put(&conn, "/idep/ctp-chenf/test/a", "/idep/ctp-chenf/test/b") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_put\n");
		
	// if (ctp_get(&conn, "/idep/ctp-chenf/test/a", "/idep/ctp-chenf/a") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_get\n");
	
	// if (ctp_delf(&conn, "/idep/ctp-chenf/test/b") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_delf\n");	
	
	// if (ctp_cwd(&conn, "/idep/ctp-chenf/test/") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_cwd\n");
	
	// char current_path[1024];
	// if (ctp_pwd(&conn, current_path) != CTP_OK)
		// return -1;
	// printf("[OK] ctp_pwd\n");
	// printf("%s\n", current_path);

	// if (ctp_mkdir(&conn, "/idep/ctp-chenf/test/abc") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_mkdir\n");
		
	// if (ctp_renm(&conn, "/idep/ctp-chenf/test/c", "/idep/ctp-chenf/test/d") != CTP_OK)
		// return -1;
	// printf("[OK] ctp_renm\n");

	// char time[100];
	// long size;
	
	// if (ctp_time(&conn, "/idep/ctp-chenf/test/c", time) != CTP_OK)
		// return -1;
	// printf("[OK] ctp_time\n");
	// printf("%s\n", time);
	
	// if (ctp_size(&conn, "/idep/ctp-chenf/test/c", &size) != CTP_OK)
		// return -1;
	// printf("[OK] ctp_size\n");
	// printf("%d\n", size);


	int file_num;
	ctp_file_attr_t file_list[1000];
	if (ctp_nlist(&conn, "/IDEPtest2/idep/data/ctptest/sorpath/NIC*", 1000, file_list, &file_num) != CTP_OK)
		return -1;
	printf("[OK] ctp_list\n");
	for (int i = 0; i < file_num; ++i)
		printf("%c %d %s %s\n", file_list[i].type, file_list[i].size, file_list[i].time, file_list[i].name);
	printf("file number: %d\n", file_num);
	
	
	if (ctp_quit(&conn) != CTP_OK)
		return -1;
	printf("[OK] ctp_quit\n");
	
	return 0;
}
*/
