/*	common.h	*/
#ifndef  COMMON_H
#define  COMMON_H   

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <string>
#include "StdEvent.h"
#include "platform.h"
#include "strings.h"
//using namespace std;

#define   NEG_ONE_STRING     "-1"
#define   NEG_ONE_INTEGER   (-1)
#define   NUMBER_ZERO_INTEGER    0
#define   NULL_CONST_P    NULL
#define   STRING_END_FLAG     '\0'
#define   STRING_NULL     ""		  /*  空串 */


#define   FALSE_CONST_BOOL    false
#define   TRUE_CONST_BOOL      true

#define MY_ABS(x)  ( (x)<0 ? -(x) : (x) )
#define MY_MAX( x , y )  ( (x)>(y) ?  (x) : (y) )
#define MY_MIN( x , y )  ( (x)<(y) ?  (x) : (y) )



#define SIZE_LOG_BUFFER 1024		/*日志每行长度*/
#define SIZE_LOG_NAME	256		/*日志文件名长度*/


#ifndef MAXBUFFSIZE
#define MAXBUFFSIZE		32000
#endif

#ifndef EXTEND_LEN
#define EXTEND_LEN		32000
#endif


#ifndef  BUFF_LEN
#define  BUFF_LEN       32000
#endif

#ifndef  VARCHAR2_TYPE
#define VARCHAR2_TYPE            1
#endif

#ifndef  NUMBER_TYPE
#define NUMBER_TYPE              2
#endif

#ifndef  INT_TYPE
#define INT_TYPE		 3
#endif

#ifndef  FLOAT_TYPE
#define FLOAT_TYPE               4
#endif

#ifndef STRING_TYPE
#define STRING_TYPE              5
#endif

#ifndef  ROWID_TYPE
#define ROWID_TYPE              11
#endif

#ifndef  DATE_TYPE
#define DATE_TYPE               12
#endif

#ifndef  NOT_EQ
#define NOT_EQ              !=
#endif

#ifndef  EQUAL_TO
#define  EQUAL_TO          ==
#endif


#ifndef TRUE
#define TRUE                    1
#endif /* TRUE */

#ifndef FALSE
#define FALSE                   0
#endif /* TRUE */


#include "platform.h"

extern char * ltoa(const long long lValue);
extern char * ltoa(const long lValue);
extern char * ltoa(const short lValue);
extern char * stoa(const short lValue);
extern short atos(const char *lValue);
extern char *ltoa(long InNum,long sumlen);
//#include "platform.h"



extern "C" {
//标准字符串
class StdString
{
	public:
      long m_lTotalLen; //总计长度
	    long m_lCurPos;		//当前指标
	    string m_strResult; //记录总的结果
	    char *m_pStr;
	    
	    StdString()
        {
            m_lCurPos = 0;
            m_lTotalLen = 0;
            m_pStr = NULL;
            //memset(&StdString,0,sizeof(struct StdString));
        }
        ~StdString()
       {
       		if(m_pStr)
       		{
       			free(m_pStr);
       			m_pStr = NULL;
       		}
       		m_lCurPos = 0;
       		m_lTotalLen = 0;
      	}
        
        void resetStdString()
        {
            memset(m_pStr,0,m_lCurPos);
            m_lCurPos = 0;
            m_strResult.clear();
        }
};

//消息交互性能监控  是10分钟监控一次(并且放在server服务最后调用)
struct MessageMonitor   //消息交互性能监控
{
	public:
		long m_lServiceId;
		char m_sServiceType[4];
		long m_lRequestTime;  //平均相应时间
		long m_lRequestWaitCnt;
		long m_lAnswerWaitCnt;
		
		MessageMonitor()
		{
			m_lServiceId = 0;
			m_sServiceType[0]=0;
			m_lRequestTime = 0;
			m_lRequestWaitCnt = 0;
			m_lAnswerWaitCnt = 0;
		}
		
		void reSet()
		{
			memset(m_sServiceType,0,4);
			m_lServiceId = 0;
			m_lRequestTime = 0;
			m_lRequestWaitCnt = 0;
			m_lAnswerWaitCnt = 0;
		}
		
};

struct MessageRequest
{
	public:
		long m_lServiceId;
		long m_lServiceType;
		char m_sAbmRequestId[30];
		char m_sOutRequestId[30];
		long m_lResultCode;
		
		MessageRequest()
		{
			m_lServiceId = 0;
			m_lServiceType = 0;
			m_sAbmRequestId[0]=0;
			m_sOutRequestId[0]=0;
			m_lResultCode=0;
		}
		
		void reSet()
		{
			m_lServiceId = 0;
			m_lServiceType = 0;
			memset(m_sAbmRequestId,0,30);
			memset(m_sOutRequestId,0,30);
		}
		
};

struct ACLExceptionLog   //鉴权异常日志
{
	public:
		long m_lServiceId;
		char m_sRequestId[20];
		long m_lServiceType;
		char m_sResultDesc[20];
		
		ACLExceptionLog()
		{
			m_lServiceId = 0;
			m_lServiceType = 0;
			m_sRequestId[0]=0;
			m_sResultDesc[0]=0;
		}
		
		void reSet()
		{
			m_lServiceId = 0;
			m_lServiceType = 0;
			memset(m_sResultDesc,0,20);
			memset(m_sRequestId,0,20);
		}
	
};

struct ProcessLog  //进程日志
{
	public:
		long m_lProcessCode;
		char m_sProcessName[50];
		long m_lLogCodeId;
		char m_sLogGrade[4];
		char m_sLogCon[200];
		char m_sHostName[50];
		char m_sHostIp[24];  
		
		ProcessLog()
		{
			m_lProcessCode = 0;
			m_sProcessName[0] = 0;
			m_lLogCodeId = 0;
			m_sLogGrade[0]=0;
			m_sLogCon[0]= 0;
			m_sHostName[0]=0;
			m_sHostIp[0]=0;
		}
		void reSet()
		{
			m_lProcessCode = 0;
			m_lLogCodeId = 0;
			memset(m_sProcessName,0,50);
			memset(m_sLogGrade,0,4);
			memset(m_sLogCon,0,200);
			memset(m_sHostName,0,50);
			memset(m_sHostIp,0,24);
		}
};

extern char toUpper(const char& ch); 
extern char toLower(const char& ch); 
extern std::string Upper(const std::string s); 
extern std::string Lower(const std::string s); 

extern long Memcpy(StdString &sStdStr,const char *sValue,const long Len);
extern long Memset(char *ptr,const char *Value,long lLen);
std::string trim(std::string const& source, char const* delims = " \t\r\n");
std::string delAll(std::string const & source, char const * delims = "\n");

string toString(StdString &sStdStr);
//void Pprintf(int DataNodeId,void *LogInfo);

void AllTrim(char *sBuf);
extern void GetNowDateTime(char *sDateTime,long lDateTimeFlag);
extern void StrTrim(char sBuf[]);
extern int HexCharToDecInt(char cHex);
extern char DecIntToHexChar(int iDec);
extern void StringFill(char *sDest,char *sSrc,long lLen,int iFlag,char cFilled);
extern void DecimalIntoHex(char *szHexNum,int iByte,int iNum);
extern void  MemoryFill(char * sDest, const char * sSrc, long lLen, int iFlag,  char cFilled, long lSrcLen);
long WriteFixLenInfo(char * sDest,const char * sSrc, long lBegin, long lLen);
long ReadFixLenInfo(char * sDest, const char * sSrc, long lBegin, long lLen);

extern  long  specialXML_Sql_recover( const char* ,   char**   );
extern long  rmInvalidChar(char*);

extern long WriteFixLenInfo_EX(char * * ppDest, const char * sSrc, long lBegin, long lLen, long * plDestLen);
extern long replace(const char * m_sSqlIn, const char * sOriginStr, const char * sNewStr, char** ppSqlOut);
extern long specialXML2Sql_replace(const char * sSqlIn, char** ppSqlOut);

long Str2IntString(const char * sSqlIn, const char cConnChar, char * * ppSqlOut);
long IntString2Str(char * sSqlIn, const char cConnChar);

extern   int  GenerateDigitVoc(const char * upDigit, int uiType, char * Buffer);  /* grtvox.cpp */

extern int stoi(const std::string str);

extern long stol(string str);
extern string ltos(long InNum,long sumlen);

//int DisconnectTerminalMonitor ();
//bool Pprintf(int iAlertCode,bool bInsertDB,int iFileID,char const *fmt,...);

}
#endif




