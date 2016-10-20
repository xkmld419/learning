#ifndef ABMCLEARFILE_H
#define ABMCLEARFILE_H

#include "Log.h"
#include "Environment.h"
#include  "ParamDefineMgr.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>



#define PATHNAME_LEN 500

#define DEFINE_QUERY(x) TOCIQuery x (Environment::getDBConn())

using namespace std;

class ClearFile
{
	public:
		
		ClearFile() ;
		~ClearFile() ;
		
		char * CFtrim(char *sp) ;
		void setEndTime(char *t) ;
		int checkPermission(const char *filePath) ; //权限校验
		bool getTaskPath() ; //
		bool getErroeFilePath() ;
		bool getMergeFilePath() ;//
		bool getDupHisPath() ;//
		bool getWdbHisPath() ;//
		//bool getTdtransPath() ;
		bool getAsnBackupPath() ;
		bool getRSexbakPath() ;
		
		int clearGaherTaskFile() ;
		int clearCheckErrorFile() ;
		int clearDataMergeFile() ;
		int clearDupHisFile() ;
		int clearwdHisFile() ;
		int clearTdtransTmpFile() ;
		int clearAsnBkFile() ;
		int clearRSexbakFile() ;	 
		
	private:

		vector< string> m_gTaskPath;
		char m_cErrorFilePath[PATHNAME_LEN] ;
		string m_mMergeFilePath ;
		char m_dupHisPath[PATHNAME_LEN] ;
		char m_wdbHisPath[PATHNAME_LEN] ;
		//char m_tdTransPath[PATHNAME_LEN] ;
		vector<string> m_asnBackupPath ;
		char m_eRSexbakPath[PATHNAME_LEN] ;
		char endtime[20];
};

#endif