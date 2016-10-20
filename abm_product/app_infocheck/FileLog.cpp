/******************************************************************************************
文件名      : FileLogAppender.cpp
作者        :
版本        :
生成日期    : 2008-07-02
文件描述    : CFileLog 类，记录文件日志类
其它        :
函数列表    :
修改记录    :

 ******************************************************************************************/
#include "FileLog.h"
//default constructor
CFileLog::CFileLog()
{
    m_pStream = NULL;
    m_bIsOpen = false;
    m_strFileName = "";
    m_strFilePath = "";
    m_pLock = new CmutexLock();
}

//commonly used constructor
CFileLog::CFileLog ( std::string strFilePath )
{
    m_pStream = NULL;
    m_bIsOpen = false;
    m_strFilePath = strFilePath;
    //如果输入的路径名称后没有加/" 则加上/"
    char szFilePath[MAXSORTPATH + 1] =
    {
        0
    };
    strncpy ( szFilePath, strFilePath.c_str(), MAXSORTPATH );

    m_strFilePath = szFilePath;
    if ( m_strFilePath.length() > 0 )
    {
        if ( m_strFilePath.c_str() [m_strFilePath.length() - 1] != '/' )
        {
            m_strFilePath.append ( "/" );
        }
    }
    m_strFileName = "";
    m_pLock = new CmutexLock();
}

//destructor
CFileLog::~CFileLog()
{
    DelObject();
    if (NULL != m_pLock)
    {
        delete m_pLock;
        m_pLock = NULL;
    }
}

void CFileLog::DelObject()
{
    if ( m_bIsOpen )
    {
        m_bIsOpen = false;
    }
    if ( NULL != m_pStream )
    {
        m_pStream->close();
        delete m_pStream;
        m_pStream = NULL;
    }
}


//open method
bool CFileLog::OpenFile ( std::string strFileName )
{

    // 加上日志文件路径和日志文件名，生成完整的日志文件名（包括路径）
    std::string strfilename; //路径+文件名
    m_strFileName = strFileName;
    strfilename = m_strFilePath + strFileName;

    //如果没有打开文件
    if ( false == m_bIsOpen )
    {
        //如果文件流为空
        if ( NULL == m_pStream )
        {
            //打开文件流
            m_pStream = new std::ofstream ( strfilename.c_str(), std::ios::app );

            //如果打开失败，返回false
            if ( NULL == m_pStream )
            {
                std::cerr << "CFileLog::OpenFile：can't open file  " << strfilename.c_str() << "! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
                DelObject();
                return false;
            }

            // 判断是否是真的打开，
            //如果日志路径不对，可以初始化m_pStream，但是实际上并没有正真打开
            if ( false == m_pStream->is_open() )
            {
                //打开失败
                std::cerr << "CFileLog::OpenFile：m_pStream status is not open ! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
                DelObject();
                return false;
            }

            m_pStream->setf ( std::ios::unitbuf ); //turn off buffering
        }
        else
        {
            m_pStream->open ( strfilename.c_str(), std::ios::app );
            if ( !m_pStream )
            {
                std::cerr << "CFileLog::OpenFile：can't open file  " << strfilename.c_str() << "! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
                DelObject();
                return false;
            }

            // 判断是否是真的打开，
            //如果日志路径不对，可以初始化m_pStream，但是实际上并没有正真打开
            if ( 0 == m_pStream->is_open() )
            {
                //打开失败
                std::cerr << "CFileLog::OpenFile： m_pStream status is not open ! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
                DelObject();
                return false;
            }

            m_pStream->setf ( std::ios::unitbuf ); //turn off buffering
        }
        m_bIsOpen = true;
    }

    return true;
}

//close method
bool CFileLog::CloseFile()
{
    bool bResult = false;
    //如果日志文件已经打开
    if ( m_bIsOpen )
    {
        //如果日志文件流为空，返回失败
        if ( NULL == m_pStream )
        {
            bResult = false;
        }
        else
        {
            //关闭文件流，返回成功
            m_pStream->close();
            bResult = true;
        }
        //设置打开标志为false
        m_bIsOpen = false;
    }
    return bResult;
}



std::string CFileLog::CreateFileName()
{

    int iPosition;
    struct tm* today;
    std::string strmode;
    time_t ltime;
    char szdatestr[MAXSORTPATH + 1] = "\0";

    //获得时间结构today
    time ( &ltime );
    today = localtime ( &ltime );

    //获得模块名
    strmode = m_moduleName;

    if ( std::string::npos != ( iPosition = strmode.find ( "_" ) ) )
    {
        strmode.resize ( iPosition );
    }

    snprintf ( szdatestr, MAXSORTPATH, "process_%04d-%02d-%02d_%s.log", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday, strmode.c_str() );


    //生成日志文件名字
    return std::string ( szdatestr );
}


bool CFileLog::IsReady()
{
    if ( m_bIsOpen && ( NULL != m_pStream ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

int CFileLog::WriteLog ( int _nPriority, const char* _FileName, int _nPosition, const char* _szLogInfo )
{
    m_pLock->lock();
    //格式化日志信息
    std::string strInfo = formatLog ( _nPriority, _FileName, _nPosition, _szLogInfo );
    int nResult =  - 1;

    //为了实现运行时候删除日志文件后还可以生成文件
    if ( m_bIsOpen )
    {
        std::string strfilename = m_strFilePath + m_strFileName;
        //如果访问不到文件
        if ( access ( strfilename.c_str(), R_OK | W_OK ) < 0 )
        {
            DelObject();
        }
    }

    //如果没有打开文件，那么打开文件
    if ( !m_bIsOpen )
    {
        //获得文件名并且打开文件
        m_strFileName = CreateFileName();
        if ( !OpenFile ( m_strFileName ) )
        {
            std::cerr << "CFileLog::WriteLog：can't open file! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
            return  - 1;
        }
    }

    //判断是否已经准备好，可以写文件日志
    if ( !IsReady() )
    {
        std::cerr << "CFileLog::WriteLog：write file is not ready 。 " << __FILE__ << "  " << __LINE__ << std::endl;
        return  - 1;
    }

    struct tm* today;
    time_t ltime;
    time ( &ltime );
    today = localtime ( &ltime );
    char szcntime[ID_LEN_32 + 1] =
    {
        0
    };
    snprintf ( szcntime, ID_LEN_32, "process_%04d-%02d-%02d", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday );
    std::string strDate = szcntime;



    /*日志文件名如:  YYYY-MM-DD_模块名.log，一天一个日志文件
    判断文件名和当前时间是否是一样，如果不是一样，
    关闭打开的文件，并且创建并打开新文件
     */

    bool bSameFile = false; //判断是否是当前时间的日志
    if ( m_strFileName.length() > 18 )
    {
        if ( m_strFileName.substr ( 0, 18 ) == strDate )
        {
            bSameFile = true;
        }
    }
    //如果不是同一个文件
    if ( !bSameFile )
    {
        //如果已经开了一个文件，关闭
        if ( m_bIsOpen )
        {
            CloseFile();
        }

        //获得将要打开的文件名
        m_strFileName = CreateFileName();
        //打开文件
        if ( !OpenFile ( m_strFileName ) )
        {
            std::cerr << "CFileLog::WriteLog：can't open file! : error in : " << __FILE__ << "  " << __LINE__ << std::endl;
            return  - 1;
        }
    }

    if ( NULL == m_pStream )
    {
        nResult =  - 1;
    }
    else
    {
        //写日志信息
        *m_pStream << strInfo << "\n";
	std::cout<<strInfo<<std::endl;
        nResult = 1;
    }
    m_pLock->unlock();
    return nResult;
}

std::string CFileLog::formatLog ( int _nPriority, const char* _FileName, int _nPosition, const char* _szLogInfo )
{
    std::ostringstream message;
    // 获得当前时间，并且转换为YYYY-MM-DD HH:MI:SS 格式的字符串
    struct tm* today;
    time_t ltime;
    time ( &ltime );
    today = localtime ( &ltime );
    char sztime[ID_LEN_32 + 1] =
    {
        0
    };
    snprintf ( sztime, ID_LEN_32, "%04d-%02d-%02d %02d:%02d:%02d", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday, today->tm_hour, today->tm_min, today->tm_sec );

    //将输入的参数和当前时间串，格式化为一个完整的日志信息
    //const std::string& priorityName =nsCLog::getPriorityName(nPriority);

    message << "[TIME: " << sztime << "] [Level: " << _nPriority << "] [FileName: " << _FileName
    << "] [Position: " << _nPosition << "] " << std::endl <<
    " |-------------------------------------------------Message-------------------------------------------------|"
    << std::endl<<_szLogInfo << std::endl;

    return message.str();

}

void CFileLog::setInfo ( const char* _moduleName )
{
    //GetSource(_moduleName);
    m_moduleName = _moduleName;
    m_strFileName = CreateFileName();
}
