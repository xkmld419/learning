#ifndef _ENCODE_H
#define _ENCODE_H
#include <string.h>

extern "C"
{
extern void encode(char *inblock,char * outblock);
extern void decode(char *inblock,char * outblock);
}
#endif /* _ENCODE_H */

