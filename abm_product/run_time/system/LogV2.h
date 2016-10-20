#ifndef __LOG_V2_H_INCLUDED_
#define __LOG_V2_H_INCLUDED_

#include "ABMException.h"


#ifdef THREE_LOG_GROUP
class ThreeLogGroup;
extern ThreeLogGroup *g_po3Log;
#else
class LogV2;
extern LogV2 *g_po3Log;
#endif

extern bool init3Log();

#define __ADD_LOG_(Text) LogV2::log(__FILE__, __LINE__, 0, Text)
#define __PRT_LOG_(Format, Para) LogV2::log(__FILE__, __LINE__, 0, Format, Para)
#define __ERR_LOG_(Func) \
				do { \
					char sBuf[512]; \
					snprintf(sBuf, sizeof(sBuf), "%s:errno=%d ", Func, errno); \
					strerror_r(errno, &sBuf[strlen(sBuf)], sizeof(sBuf)-strlen(sBuf)); \
					sBuf[sizeof(sBuf)-1] = '\0'; \
					__ADD_LOG_(sBuf); \
				} while (0);
					
#define __INIT_LOG_(oExp) LogV2::logExcep(oExp)

#ifdef __STD99
#define __DEBUG_LOG_(Level, Text, ...) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, __VA_ARGS__)
#endif
				
#define __DEBUG_LOG0_(Level, Text) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text)  	
#define __DEBUG_LOG1_(Level, Text, Para1) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1)
#define __DEBUG_LOG2_(Level, Text, Para1, Para2) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2)
#define __DEBUG_LOG3_(Level, Text, Para1, Para2, Para3) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3)
#define __DEBUG_LOG4_(Level, Text, Para1, Para2, Para3, Para4) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4)
#define __DEBUG_LOG5_(Level, Text, Para1, Para2, Para3, Para4, Para5) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5)
#define __DEBUG_LOG6_(Level, Text, Para1, Para2, Para3, Para4, Para5, Para6) \
		if (Level <= LogV2::m_iLogLevel) LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5, Para6)	
			
#define __DEBUG_LOG7_(Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7) \
		if (Level <= LogV2::m_iLogLevel) \
			LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7);
				
#define __DEBUG_LOG8_(Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8) \
		if (Level <= LogV2::m_iLogLevel) \
			LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8);	
																										
#define __DEBUG_LOG9_(Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8, Para9) \
		if (Level <= LogV2::m_iLogLevel) \
			LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8, Para9);
				
#define __DEBUG_LOG10_(Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8, Para9, Para10) \
		if (Level <= LogV2::m_iLogLevel) \
			LogV2::log(__FILE__, __LINE__, Level, Text, Para1, Para2, Para3, Para4, Para5, Para6, Para7, Para8, Para9, Para10);				
				
class LogV2
{

public:

	LogV2();

	~LogV2();

	static void setFileName(const char *sName);

	static void log(const char *sFile, const int iLine, int iLogLevel, char const * sFormt, ...);

	static void logExcep(ABMException &oExp);


	static int m_iLogLevel;

	static char m_sLogName[256];
	
	//不影响测试 添加三个日志组的接口
	void  log(int iCodeId,int iLogLevel,int iLogClassId,int iLogTypeId, \
              int iAction,char *pExpand,char* format,...) {
        return;            
    }

};



#endif/*__LOG_V2_H_INLCUDED_*/
