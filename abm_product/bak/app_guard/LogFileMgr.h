#ifndef LOG_FILE_MGR_H
#define LOG_FILE_MGR_H

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using   namespace   std;
class LogFileMgr
{
	public:
			LogFileMgr();
			~LogFileMgr();
			bool getLogData(int ProcessID,vector<string> &vec_log,int iBegineLine = 0);
			char m_sErrInfo[256];
			int is_zh_ch(char p);
			bool checkLine(char *str);
	private:
			char m_sFileName[256];
};

#endif