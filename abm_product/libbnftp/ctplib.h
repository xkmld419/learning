#ifndef CTP_LIB_H_
#define CTP_LIB_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define CTP_VERSION 0x02

#define CTP_RES_FAIL   (char)1
#define CTP_RES_OK     (char)2
#define CTP_CMD_USER   (char)3
#define CTP_CMD_PASS   (char)4
#define CTP_CMD_PASV   (char)5
#define CTP_CMD_QUIT   (char)6
#define CTP_CMD_CWD    (char)11
#define CTP_CMD_PWD    (char)13
#define CTP_CMD_MKDIR  (char)14
#define CTP_CMD_LIST   (char)15
#define CTP_CMD_GET    (char)23
#define CTP_CMD_PUT    (char)24
#define CTP_CMD_RENM   (char)25
#define CTP_CMD_LINK   (char)26
#define CTP_CMD_DELF   (char)27
#define CTP_CMD_TIME   (char)28
#define CTP_CMD_SIZE   (char)29
#define CTP_DATA_FILE  (char)100

#define CTP_PUT_TIME_OUT  1099
#define CTP_ERR_SOCKET 100
#define CTP_ERR_IO 101
#define CTP_ERR_CONNECT 102
#define CTP_ERR_FOPEN 103
#define CTP_ERR_UNLINK 104
#define CTP_ERR_PARSE 105
#define CTP_ERR_FWRITE 106
#define CTP_ERR_FREAD 107

#define CTP_PKG_HEAD_SIZE (sizeof(char)*4 + sizeof(int))
#define CTP_PKG_BODY_SIZE 4096

#define MAX_FILE_TIME_SIZE 16
#define MAX_FILE_NAME_SIZE 256

typedef struct {
	int size;
	char version;
	char type;
	char encrypt;
	char reserved;
	char body[CTP_PKG_BODY_SIZE];
} ctp_pkg_t;

typedef struct {
	int sock;
	struct sockaddr_in addr;
	ctp_pkg_t pkg;
} ctp_conn_t;

typedef struct {
	char type;
	int size;
	char time[MAX_FILE_TIME_SIZE];
	char name[MAX_FILE_NAME_SIZE];
} ctp_file_attr_t;

#define ctp_memzero(m, s) memset(m, 0, s)
#define ctp_memcpy(x, y, z) memcpy(x, y, z)

#define pack_fill_default(p) \
	(p).version = (char) CTP_VERSION; \
	(p).encrypt = (char) 0x01; \
	(p).reserved = (char) 0x00; \
	memset((p).body, 0, sizeof((p).body));
	
#define get_err_code(p) (atoi(strtok(p.body, "-")))

extern int ctp_errno;
extern int socket_errno;

int ctp_init(ctp_conn_t* conn, int timeout,int block = 0);
int ctp_connect(ctp_conn_t* conn, const char* host, const char* port);
int ctp_auth(ctp_conn_t* conn, const char* usr, const char* pwd);
int ctp_pasv(ctp_conn_t* conn, char* ip, char* port);
int ctp_put(ctp_conn_t* conn, const char* local_path, const char* remote_path, const char* timestamp, 
	long offset=0);
int ctp_get(ctp_conn_t* conn, const char* remote_path, const char* local_path);
int ctp_quit(ctp_conn_t* conn);
int ctp_cwd(ctp_conn_t* conn, const char* remote_path);
int ctp_pwd(ctp_conn_t* conn, char* current_path);
int ctp_mkdir(ctp_conn_t* conn, const char* new_path);
int ctp_list(ctp_conn_t* conn, const char* remote_path, ctp_file_attr_t* list, int* num);
int ctp_nlist(ctp_conn_t* conn, const char* remote_path, int max_num, 
	ctp_file_attr_t* list, int* num);
int ctp_renm(ctp_conn_t* conn, const char* old_path, const char* new_path);
int ctp_link(ctp_conn_t* conn, const char* old_path, const char* new_path);
int ctp_delf(ctp_conn_t* conn, const char* delf_path);
int ctp_time(ctp_conn_t* conn, const char* remote_path, char* time);
int ctp_size(ctp_conn_t* conn, const char* remote_path, long* size);
int ctp_close(ctp_conn_t* conn);
double  get_time();

int myround( double x );

#endif
