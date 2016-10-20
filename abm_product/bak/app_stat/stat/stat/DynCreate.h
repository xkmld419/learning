/*VER: 1*/ 
#ifndef __DYN_CREATE_H_
#define __DYN_CREATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TOCIQuery.h"

extern void  DupTable(const char *sTemplate,const char *sTableName);
extern void  DupTableIndex(const char * sTemplateTable,const char * sTableName,const char * sIndExt);

extern void  DupTable(const char *sTemplate,const char *sTableName,TOCIDatabase *pDBLink);
extern void  DupTableIndex(const char * sTemplateTable,const char * sTableName,const char * sIndExt,TOCIDatabase *pDBLink);

#endif


