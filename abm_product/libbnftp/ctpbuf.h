#ifndef CTP_BUF_H_
#define CTP_BUF_H_

typedef struct 
{
	char* data;
	int max;
	int last;
} ctp_buf_t;

#define ctp_buf_at(b, i) ((b)->data[(i)])
#define ctp_buf_size(b) ((b)->last)
#define ctp_buf_ptr(b) ((b)->data)

int ctp_buf_create(ctp_buf_t* buf, int size);
int ctp_buf_append(ctp_buf_t* buf, char* str, int size);
void ctp_buf_erase(ctp_buf_t* buf);
void ctp_buf_destroy(ctp_buf_t* buf);

#endif
