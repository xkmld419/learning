/*
公共函数部分
电信计费研发部
朱逢胜
2010.5.25
*/
#ifndef _PUBFUN_H_201005_
#define _PUBFUN_H_201005_
#include <vector>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <regex.h>
#include <dirent.h>
#include <sys/mman.h>//mmap
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
namespace nsFuntion
{
//分割字符串
    std::vector<std::string> StringSplit(const std::string& _soureString, const std::string& _dilimiter);

//效验Unix路径的访问权限
    bool AccessOfPath(const std::string& _pathName);

//从字符串中替换某个字符
//返回值为替换的个数
    int replaceChar(std::string& _sourceString, const char _oldChar, const char _replaceChar);

//正则表达式，匹配返回真，错误或者不匹配返回假
    bool regexExpression(const std::string& _sourceString, const std::string& _expression);

//字符串是否为十进制正整数
    bool isNumber(const std::string& _sourceString);

    std::string itoa(const int& _number);

//文件映射存储
    int MapFile(const std::string &,std::string&);
}
#endif
