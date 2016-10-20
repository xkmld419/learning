#ifndef _EVENTTOOLKITS_H_
#define _EVENTTOOLKITS_H_

#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "StdEvent.h"

using namespace std;
/*class IndexTimeStamp
{
public:
    IndexTimeStamp(time_t ltmStamp,const string strIndex1):lTimeStamp(ltmStamp),strIndex(strIndex1){};
public:
	time_t    lTimeStamp;
    const string  strIndex;
};

bool TimeStampGreate(const IndexTimeStamp& stamp1,const IndexTimeStamp& stamp2)
{
    return stamp1.lTimeStamp>=stamp2.lTimeStamp;
};
*/
class EventToolKits
{
	public :

		// 构造函数
		EventToolKits();

		// 析构函数
		~EventToolKits();

		// 根据事件属性ID获取事件属性值与类型
		string get_param( const StdEvent *pStd, int iEventAttrId, int &iType);

		// 根据事件属性ID获取事件属性值
		string get_param( const StdEvent *pStd, int iEventAttrId );

		string get_param2(const StdEvent * pStd, int iEventAttrId, int & iType,void **pEventAttr,int &iValueSize);
		string get_param2(const StdEvent *pStd,int iEventAttrID,int &iType);

		// 根据事件属性ID设置事件属性值
		void set_param( StdEvent *pStd, int iEventAttrId, string strAttr );

		// long 转换为 string
		string toString( const long lval );

		// int 转换为 string
		string toString( const int ival );

		// string 转换为 int
		int toInt( string strval );

		// string 转换为 bool
		bool toBool( string strval );

		// 转换为小写
		string &lowercase( string &strval );

		// 转换为大写
		string &uppercase( string &strval );

		// 去除首尾空格
		string trim( string &strval );

		// 计算时间差，单位秒
		double time_diff( const char *strTime1, const char *strTime2 );
		double time_diff( string const &strTime1, string const &strTime2 );

		// 字符串转换为日期
		time_t toDate( const char *strDate );

		// 字符串转换为时间
		time_t toTime( const char *strTime );

		// 字符串转换为日期时间
		time_t toDateTime( const char *strDateTime );

		// string转换为float
		double toFloat( string strval );
		
		// string 转换成long
		long toLong( string strval);
	private :

		// 用于格式转换的字符流
		ostringstream m_oSstream;

		// 小写转换functor
		struct lowertrans
		{
			void operator()( string::value_type &r )
			{
				r = r | 0x20;
			}
		};
		// 大写转换functor
		struct uppertrans
		{
			void operator()( string::value_type &r )
			{
				r = r & ( !0x20 );
			}
		};
};




















#endif
