#ifndef _HSS_USER_CHECK_H_
#define _HSS_USER_CHECK_H_

#include <string>
#include <stdio.h>
#include <map>

using namespace std;

typedef std::map<string, string>  USERMap;
	
class CheckUserInfo
{
public:
    CheckUserInfo()
    {
    }

    ~CheckUserInfo()
    {
    };

    /*
    *   函数介绍：初始化CheckUserInfo定义文件
    *   输入参数：无
    *   输出参数：无
    *   返回值  ：返回错误码，如果成功则返回0
    */
    static int ParseFile();

    /*
    *   函数介绍：获取错误的描述信息
    *   输入参数：错误码
    *   输出参数：无
    *   返回值  ：返回错误码描述信息
    */
    static bool checkUerInfo(char * sUser,char *sPwd);



private:
    static USERMap m_mParamMap; // 错误码信息
};


#endif
