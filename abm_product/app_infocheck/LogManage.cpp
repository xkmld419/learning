#include "LogManage.h"
CFileLog* CLogManage::m_pFileLog = NULL;
CLogManage* CLogManage::m_pLogManage = NULL;
CLogManage::CLogManage()
{
      m_pFileLog = NULL;
}

CLogManage::~CLogManage()
{
      if ( NULL != m_pFileLog )
      {
            delete m_pFileLog;
            m_pFileLog = NULL;
      }
}

int CLogManage::WriteLog ( int _nPriority, const char* _FileName, int _nPosition, const char* _szLogInfo )
{    
      if ( m_pFileLog->WriteLog ( _nPriority,  _FileName,  _nPosition,  _szLogInfo ) < 0 )
      {
            std::cerr << __FILE__ << " " << __LINE__ <<" "<< "write log error" << std::endl;
            return  - 1;
      }
      return 1;
}

int CLogManage::setLog ( const char* _ModuleName )
{
      const char* pEnvPath = NULL;
      if ( ( pEnvPath = getenv ( "TIBS_HOME" ) ) == NULL )
      {
            std::cerr << __FILE__ << " " << __LINE__ << "get TIBS_HOME error" << std::endl;
            return  - 1;
      }
      std::string strLogPath = pEnvPath;
      strLogPath[strLogPath.length() - 1] == '/' ? strLogPath.append ( "log/" ) : strLogPath.append ( "/log/" );

      m_pFileLog = new CFileLog ( strLogPath.c_str() );
      if ( NULL == m_pFileLog )
      {
            std::cerr << __FILE__ << " " << __LINE__ << "new m_pFileLog error" << std::endl;
            return  - 1;
      }
      m_pFileLog->setInfo ( _ModuleName );
      return 1;
}

int CLogManage::setLog(const int _ProcessId)
{
      const char* pEnvPath = NULL;
      if ( ( pEnvPath = getenv ( "TIBS_HOME" ) ) == NULL )
      {
            std::cerr << __FILE__ << " " << __LINE__ << "get TIBS_HOME error" << std::endl;
            return  - 1;
      }
      std::string strLogPath = pEnvPath;
      strLogPath[strLogPath.length() - 1] == '/' ? strLogPath.append ( "log/" ) : strLogPath.append ( "/log/" );

      m_pFileLog = new CFileLog ( strLogPath.c_str() );
      if ( NULL == m_pFileLog )
      {
            std::cerr << __FILE__ << " " << __LINE__ << "new m_pFileLog error" << std::endl;
            return  - 1;
      }
      char szProcessId[16]= {0};
      sprintf(szProcessId,"%d",_ProcessId);
      m_pFileLog->setInfo ( szProcessId );
return 1;
}


CLogManage* CLogManage::getInstance()
{
      if ( NULL != m_pLogManage )
            return m_pLogManage;
      m_pLogManage = new CLogManage();
      return m_pLogManage;
}
