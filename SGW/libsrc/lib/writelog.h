#ifndef WRITELOG_H_HEADER_INCLUDED
#define WRITELOG_H_HEADER_INCLUDED

#include <stdio.h>
#include <openssl/bio.h>

using namespace std;

class WriteLog{
	public:
			WriteLog(int sec, int num, char *sPath, char *sPreFix);
			~WriteLog();
			bool InsertLog(char * sBuf, int size);
			int  flush();
			char * getErrMsg();
	private:
			int sec;	// 日志时间间隔
			int num;	//	单个文件的最大记录
			char sPath[256];//	日志路径	
			char sPreFix[256]; // 日志前缀
			char sFileName[512];
			char m_szTmpFileName[512];

			char m_szFileNameOld[512];
			char m_szTmpFileNameOld[512];
			FILE *bf;
			
			time_t	timeold,timenew;
			int   iNowNum;
			
			int GetNowTime(time_t t,char * sTimeBuf,char * sFormat ,int iSizeof);
			char sErrMsg[2000];
};

#endif
