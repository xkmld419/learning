#ifndef ACE_OS_NS_STRING_H
#define ACE_OS_NS_STRING_H
#include <stdio.h>
#include <strings.h>

#ifdef DEF_LINUX 
#include <string.h>
#else

#include <stdlib.h>
#ifndef strtok_r
char *strtok_r(char *s, const char *tokens, char **lasts);
#endif
#endif
#endif
