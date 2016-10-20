#include "HBECodeParse.h"
#include "File.h"

HBErrorCode::~HBErrorCode()
{
    if (NULL != m_poEFile) {
        delete m_poEFile;
    }

    for (EInfoMap::iterator begin = m_oEInfo.begin(); begin != m_oEInfo.end();) {
        if (NULL != begin->second) {
            delete begin->second;
        }
	m_oEInfo.erase(begin++);
    }
}

/*
*   函数介绍：初始化HBErrorCode定义文件
*   输入参数：无
*   输出参数：无
*   返回值  ：返回错误码，如果成功则返回0
*/
int HBErrorCode::init()
{
	
    if (0 != m_oEInfo.size()) {
        return -1;
    }
    char *penv = NULL;
    char sLibEnv[128] = {0};
    char sfile[256] = {0};
    char Pwd[128] = {0};
    if ((penv=getenv("SGW_HOME")) == NULL) {
        printf( "环境变量SGW)_HOME没有设置, init HBErrorCode Information failed返回错误");             
        return -1;
    }
    strncpy(sLibEnv, penv, sizeof(sLibEnv)-2);
    if (sLibEnv[strlen(sLibEnv)-1] != '/') {
        sLibEnv[strlen(sLibEnv)] = '/';             
    }

    snprintf(sfile, sizeof(sfile), "%s%s/MBC_SGW.h", sLibEnv,"libsrc/lib");
    m_poEFile = new File(sfile);

    return ParseErrorFile();
}

/*
*   函数介绍：获取错误的描述信息
*   输入参数：错误码
*   输出参数：无
*   返回值  ：返回错误码描述信息
*/
char* HBErrorCode::getCauseInfo(int iHBErrorCode)
{
	if (m_oEInfo[iHBErrorCode])
		return m_oEInfo[iHBErrorCode]->m_sCause;
	else
		return NULL;
}

/*
*   函数介绍：获取错误的专家修复建议
*   输入参数：错误码
*   输出参数：无
*   返回值  ：返回错误码的专家修复建议
*/
char* HBErrorCode::getAdviceInfo(int iHBErrorCode)
{
	if (m_oEInfo[iHBErrorCode])
		return m_oEInfo[iHBErrorCode]->m_sCause;
	else
		return NULL;
}

/*
*   函数介绍：获取错误的相关信息包括错误描述及修复建议
*   输入参数：错误码
*   输出参数：无
*   返回值  ：返回错误码的描述信息及修复建议
*/
EInfo* HBErrorCode::getErrorInfo(int iHBErrorCode)
{
    return m_oEInfo[iHBErrorCode];
}

/*
*   函数介绍：解析文件，获取错误码信息
*   输入参数：无
*   输出参数：无
*   返回值  ：成功返回0，否则返回指定错误
*/
int HBErrorCode::ParseErrorFile()
{
    if (NULL == m_poEFile) {
        return -1;          // 文件不存在

    }


    FILE *fd;
    char sBuffer[1024 + 1];
    char sCode[10 + 1];
    char *p = NULL;
    char *q = NULL;
    unsigned int iRow = 0;
    unsigned int iLen = 0;
    memset(sBuffer, 0x00, sizeof(sBuffer));
    memset(sCode, 0x00, sizeof(sCode));
    int     iECode = 0;
    EInfo  *poInfo = NULL;

    if (!(fd = fopen(m_poEFile->getFullFileName(), "r"))) {
        return -1;          // 打开文件失败
    }

    // 错误码描述行定义为：
    //       "//@@\tCODE\tCAUSE\tADVICE"
    //       其中CODE代表错误码 CAUSE是错误码描述 ADVICE是修复建议
    //       他们之间都以\t及0-n个空格来间隔
    while(fgets(sBuffer, 1024, fd)) {
        iRow++;
        if (CompStrHead(sBuffer)) {
            continue;
        }

        p = sBuffer + 4;
        if ('\t' != *p) {
            printf("一行格式错误的错误码描述信息, 行号；%d, %s\n", iRow, sBuffer);
            break;
        }
        p++;

        // 解析 CODE
        q=p;
        p = strchr(p, '\t');
        if (NULL == p) {
            printf("一行格式错误的错误码描述信息, 行号；%d, %s\n", iRow, sBuffer);
            break;
        }
        memset(sCode, 0x00, sizeof(sCode));
        iLen = p - q;
        memcpy(sCode, q, iLen > 10 ? 10 : iLen);
        iECode = strtol(sCode, &q, 10);
        p++;

        poInfo = new EInfo();

        // 解析 错误描述
        q=p;
        p = strchr(p, '\t');
        if (NULL == p) {
            printf("一行格式错误的错误码描述信息, 行号；%d, %s\n", iRow, sBuffer);
            delete poInfo;
            poInfo = NULL;
            break;
        }
        ///去掉前面的空格
        while ((' ' == *q || '\t' == *q) && q++);
        memcpy(poInfo->m_sCause, q, p - q);
        p++;

        // 解析 错误描述
        while ((' ' == *p || '\t' == *p) && p++);
        char *pp=strchr(p, '\n');  
        if(*pp) *pp=0;
        strcpy(poInfo->m_sAdvice, p);

        if (!m_oEInfo.count(iECode)) {
            m_oEInfo.insert(EInfoMap::value_type(iECode, poInfo));
        }
        else {
            printf("读取到一行重复的错误码描述信息, 行号：%d\n", iRow);
        }
        poInfo = NULL;
    }

    return 0;
}

/*
*   函数介绍：判断字符串前4位是否是//@@
*   输入参数：输入字符串
*   输出参数：无
*   返回值  ：成功返回0，否则返回指定错误
*/
inline int HBErrorCode::CompStrHead(const char *sStr)
{
    if (NULL == sStr) {
        return -1;
    }

    if (4 >= strlen(sStr)) {
        return -1;
    }

    if ('/' == sStr[0] && '/' == sStr[1] && '@' == sStr[2] && '@' == sStr[3]) {
        return 0;
    }
    return -1;
}

