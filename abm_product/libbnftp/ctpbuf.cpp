#include <string.h>
#include <stdlib.h>
#include "ctpbuf.h"

int ctp_buf_create(ctp_buf_t* buf, int size)
{
	buf->data = (char*)malloc(size);
	if (!(buf->data)) 
	{
		return -1;
	}
	buf->last = 0;
	buf->max = size;
	return 0;
}


int ctp_buf_append(ctp_buf_t* buf, char* str, int size)
{
	int inc_size = 0;
	
	if (buf->last + size > buf->max)
	{
		inc_size = (size/4096 + 1) * 4096;
		buf->data = (char*)realloc(buf->data, buf->max + inc_size);
		if (!buf->data)
		{
			buf->max = 0;
			buf->last = 0;
			return -1;
		}
		buf->max += inc_size;
	}
	memcpy(buf->data+buf->last, str, size);
	buf->last += size;
	return 0;
}


void ctp_buf_erase(ctp_buf_t* buf)
{
	buf->last = 0;
}


void ctp_buf_destroy(ctp_buf_t* buf)
{
	if (!buf || !(buf->data)) return;
	
	free(buf->data);
	buf->data = NULL;
}
