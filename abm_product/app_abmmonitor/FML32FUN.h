#ifndef _FML32FUN_H
#define _FML32FUN_H

#include <fml32.h>

void FMLFadd32(FBFR32 * buf, FLDID32 fieldid, char * value, FLDLEN32 len, char * fldName);

void FMLFget32(FBFR32 * buf, FLDID32 fieldid, int occ, char * value, FLDLEN32 & maxlen, char * fldName);


#endif

