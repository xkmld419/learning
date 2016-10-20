#include "GlobalTools.h"

bool GlobalTools::CheckDateString(const char* sDate)
{
	if(0== sDate)
		return false;
	static Date d1;
	d1.parse(sDate);
	return !strncmp(d1.toString(),sDate,strlen(sDate));
};
char* GlobalTools::upperLowerConvert( char* srcStr , const int flag )
{
	int iStrLen = strlen(srcStr);
	if ( iStrLen == 0 )
		return 0;

	if ( flag == _CONVERT_UPPER )
	{
		for ( int i = 0; i < iStrLen ; i++ )
		{
			if( srcStr[ i ] >= 'a' && srcStr[ i ] <= 'z' )
				srcStr[ i ] -= 32;
		}
	}
	else if ( flag == _CONVERT_LOWER )
	{
		for ( int i = 0; i < iStrLen ; i++ )
		{
			if( srcStr[ i ] >= 'A' && srcStr[ i ] <= 'Z' )
				srcStr[ i ] += 32;
		}
	}
	return srcStr;
};

