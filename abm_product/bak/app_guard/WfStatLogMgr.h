#ifndef WF_STAT_LOG_MGR__H
#define WF_STAT_LOG_MGR__H

#include <map>

#include "Process.h"
#include "Directory.h"
#include "mntapi.h"

using namespace std;

class WfStatLogMgr:public Process
{
	public:
		WfStatLogMgr();
		~WfStatLogMgr();
		
		int run();
		
	private:
		
		bool getLogFile();
		list<File> m_lLogFileList;
		list<File>::iterator m_pLogFile;
			
		FILE *m_pFile;	
		char m_sLogCloseFilePath[300];
		
	private:		
		
		int indbStatLog();
		int writeWfStatLog();
		int m_iStatLogCnt;
		
		TStatLog *m_pStatLog;
		
	private:		
		
		int delLogFile();
		int bakLogFile();    
		
	private:
		
    	int getFileRecursive(char * sPath , list<File> &logFileList);//	Directory	没有权限
    	
    private:
    	
    	bool mergeStatLog(TStatLog *pStatLog);
    	map<string, TStatLog*> m_mapStatLog;
    	map<string, TStatLog*>::iterator m_pMergeIter;
};

#endif
