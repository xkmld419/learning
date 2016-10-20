#ifndef _CTP_NET_H_
#define _CTP_NET_H_

#include <ctype.h>

int socket_new(int *s);
int socket_close(int *s);
int socket_can_read(int s, int time_out);
int socket_can_write(int s, int time_out);
int socket_send_n(int s, void *buf, size_t len);
int socket_recv_n(int s, void *buf, size_t len);

#endif
