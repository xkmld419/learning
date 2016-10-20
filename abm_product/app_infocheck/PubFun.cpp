#include "PubFun.h"


std::vector<std::string> nsFuntion::StringSplit(const std::string& _sourceString, const std::string& _dilimiter)
{
    int iLengthOfDilimiter = _dilimiter.size();
    int iDilimiterPos = 0;
    int iCurrentPos = 0;
    std::vector<std::string> vecSplit;
    do
    {
        iDilimiterPos = _sourceString.find(_dilimiter, iCurrentPos);
        vecSplit.push_back(_sourceString.substr(iCurrentPos, iDilimiterPos - iCurrentPos));
        iCurrentPos = iDilimiterPos + iLengthOfDilimiter;
    }
    while (iDilimiterPos != std::string::npos);
    return vecSplit;
}
bool nsFuntion::AccessOfPath(const std::string& _pathName)
{
    DIR* dirp = opendir( _pathName.c_str() );
    if ( NULL == dirp )
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << "无法打开目录请检查是否存在该目录" << std::endl;
        return  false;
    }
    closedir( dirp );
    if ( access( _pathName.c_str(), R_OK | W_OK ) < 0 )
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << "目录无访问权限" << std::endl;
        return false;
    }
    return true;
}

int nsFuntion::replaceChar(std::string& _sourceString, const char _oldChar, const char _replaceChar)
{
    int iTimes = 0;
    if (_sourceString.empty() == true)
        return iTimes;

    for (std::string::iterator iter = _sourceString.begin(); iter != _sourceString.end(); iter++)
    {
        if (_oldChar == (*iter))
            (*iter) = _replaceChar;
        iTimes++;
    }
    return iTimes;
}

bool nsFuntion::regexExpression(const std::string& _sourceString, const std::string& _expression)
{
    const int SUBSLEN = 10;
    const int EBUFLEN = 128; /* 错误消息缓存长度 */
    size_t len; /* 错误消息的长度 */
    regex_t re; /* 编译后的正则表达式 */
    regmatch_t subs[SUBSLEN]; /* 匹配的字符串位置 */
    char errbuf[EBUFLEN]; /* 错误消息 */
    int err, i;
    err = regcomp( &re, _expression.c_str(), REG_EXTENDED );
    if ( err )
    {
        len = regerror( err, &re, errbuf, sizeof( errbuf ));
        //throw string( errbuf );
    }
    err = regexec( &re, _sourceString.c_str(), ( size_t )SUBSLEN, subs, 0 );
    if ( err == REG_NOMATCH )
    {
        regfree( &re );
        return false;
    }
    else if ( err )
    {
        len = regerror( err, &re, errbuf, sizeof( errbuf ));
        //throw string( errbuf );
    }
    regfree( &re );
    return true;
}

bool nsFuntion::isNumber(const std::string& _sourceString)
{
    for (std::string::const_iterator iter = _sourceString.begin(); iter != _sourceString.end(); iter++)
    {
        if (isdigit(*iter) == false)
            return false;
    }
    return true;
}

std::string nsFuntion::itoa(const int& _number)
{
    std::ostringstream os_str;
    os_str << _number;
    return os_str.str();
}

int nsFuntion::MapFile(const  string& _strFileName,string& _strFileContent)
{
    if (_strFileName.size() == 0)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << "输入的文件名为空" << std::endl;
        return -1;
    }
    if (access(_strFileName.c_str(),R_OK )<0)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << _strFileName<<"文件未找到或者无法访问,请检查" << std::endl;
        return -1;
    }
    struct stat fileStat;
    if ( stat( _strFileName.c_str(), &fileStat ) < 0 )
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << "stat file error" << std::endl;
        return -1;
    }
    int iFileDesc = 0;
    if ( iFileDesc = open(_strFileName.c_str(),O_RDONLY)<0)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl
        << "open file error" << std::endl;
        return -1;
    }
    char* cMemoryBegin = NULL;
    if (( cMemoryBegin = ( char* )mmap( cMemoryBegin, fileStat.st_size, PROT_READ, MAP_PRIVATE, iFileDesc, 0 )) == NULL )
    {
	std::cerr << __FILE__ << "  " << __LINE__ << std::endl
         << "mmap file error"  << std::endl;
        return -1;
    }
    _strFileContent = cMemoryBegin;
    while ( isprint( *( _strFileContent.end() - 1 )) == true )
    {
        _strFileContent.erase( _strFileContent.end() - 1 );
    }
    close(iFileDesc);
    munmap( cMemoryBegin, fileStat.st_size );
    return 1;
}
