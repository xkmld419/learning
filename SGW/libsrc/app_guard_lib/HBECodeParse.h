#ifndef ERROR_CODE_PARSE_H_
#define ERROR_CODE_PARSE_H_

#include <string>
#include <stdio.h>
#include <map>

using namespace std;


const unsigned MAX_ERROR_CAUSE_LEN = 100;       // 错误描述信息长度
const unsigned MAX_ERROR_ADVICE_LEN = 100;      // 修复建议长度

struct EInfo
{
public:
    EInfo()
    {
        memset(m_sCause, 0x00, sizeof(m_sCause));
        memset(m_sAdvice, 0x00, sizeof(m_sAdvice));
    }

public:
    char m_sCause[MAX_ERROR_CAUSE_LEN + 1];     // 错误描述信息
    char m_sAdvice[MAX_ERROR_ADVICE_LEN + 1];   // 修复建议

};

typedef std::map<int, EInfo*>  EInfoMap;
class File;
class HBErrorCode
{
public:
    HBErrorCode()
    {
        m_poEFile = NULL;
    }

    virtual ~HBErrorCode();

    /*
    *   函数介绍：初始化HBErrorCode定义文件
    *   输入参数：无
    *   输出参数：无
    *   返回值  ：返回错误码，如果成功则返回0
    */
    int init();

    /*
    *   函数介绍：获取错误的描述信息
    *   输入参数：错误码
    *   输出参数：无
    *   返回值  ：返回错误码描述信息
    */
    char* getCauseInfo(int iHBErrorCode);

    /*
    *   函数介绍：获取错误的专家修复建议
    *   输入参数：错误码
    *   输出参数：无
    *   返回值  ：返回错误码的专家修复建议
    */
    char* getAdviceInfo(int iHBErrorCode);

    /*
    *   函数介绍：获取错误的相关信息包括错误描述及修复建议
    *   输入参数：错误码
    *   输出参数：无
    *   返回值  ：返回错误码的描述信息及修复建议
    */
    EInfo* getErrorInfo(int iHBErrorCode);

private:
    /*
    *   函数介绍：解析文件，获取错误码信息
    *   输入参数：无
    *   输出参数：无
    *   返回值  ：成功返回0，否则返回指定错误
    */
    int ParseErrorFile();

    /*
    *   函数介绍：判断字符串前4位是否是//@@
    *   输入参数：输入字符串
    *   输出参数：无
    *   返回值  ：成功返回0，否则返回指定错误
    */
    int CompStrHead(const char *sStr);
private:
    File *m_poEFile;              // 错误码定义文件
    EInfoMap m_oEInfo; // 错误码信息

};


#endif
