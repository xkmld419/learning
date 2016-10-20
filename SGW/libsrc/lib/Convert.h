#ifndef _INCLUDE_CONVERT_H_
#define _INCLUDE_CONVERT_H_

#include "platform.h"
#include "platform.h"
#include <string>
#include "platform.h"

template<class T>
void to_string(string & result,const T & t)
{
	 //static ostringstream oss;//创建一个流
	 ostringstream oss;//创建一个流
	 oss.clear();
	 oss<<t;//把值传递如流中
  	 result=oss.str();//获取转换后的字符转并将其写入result
}

/*
template<class out_type,class in_value> out_type convert(const in_value & t)
{
//static stringstream stream;
stringstream stream;
stream.clear();

stream<<t;//向流中传值

out_type result;//这里存储转换结果

stream>>result;//向result中写入值

return result;
}
*/

template<class out_type,class in_value> out_type convert(const in_value & t)
{

	 ostringstream oss;//创建一个流
	 oss.clear();
	 oss<<t;//把值传递如流中
	 string result=oss.str();//获取转换后的字符转并将其写入result
  	 return result;
}


#endif



