#ifndef	_GLOBAL_TOOLS_H_
#define	_GLOBAL_TOOLS_H_
#include <iostream>
#include "Date.h"
#define	_CONVERT_UPPER 1
#define _CONVERT_LOWER 2


class GlobalTools
{
public:
	static bool CheckDateString(const char* sDate);
	static char* upperLowerConvert( char* srcStr , const int flag );
};

#endif

