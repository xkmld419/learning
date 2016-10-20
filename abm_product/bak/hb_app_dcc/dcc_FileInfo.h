/*ver:1*/
#ifndef _DCC_FILE_TYPE_INFO_H_
#define _DCC_FILE_TYPE_INFO_H_

#include <regex.h>  // 正则表达式的头文件 
#include "Environment.h"
#include "Directory.h"
#include "File.h"
#include "dcc_ConstDef.h"
#include "dcc_ParamsMgr.h"

using namespace std;


class DccFileFieldInfo
{
	public:
		DccFileFieldInfo();
		~DccFileFieldInfo();
		void genDcrHeadValue();
		bool genSendCCRString(string &sCCRString,char *sServiceContext);

		string m_sFieldInfo;
		string m_sSessionID;
		unsigned int m_iHopbyHop;
		unsigned int m_iEndtoEnd;
		unsigned int m_iAuthApplicationID;
		unsigned int m_iRequestType;
		unsigned int m_iApplicationID;
		string m_sDestinationHost;
		string m_sDestinationRealm;
		int m_iFieldSeq;
		int m_iFieldType;
	private:
		dccParamsMgr m_oParamMgr;
	
};

class DccFileInfo
{	
	friend class FileToDccMgr;
	public:
		DccFileInfo();		
		~DccFileInfo();
		void freeFieldInfoVec();
		bool getFile(File *pFile);
		DccFileFieldInfo *getNextRecord();
		char *getServiceContext();
	private:
		bool checkFileTypeState();
		void readRecordToVec();
		bool checkPatern(const char *sFileName) throw(string);
		bool recordIsEmpty();
		int m_iFileTypeID;
		char m_sServiceContext[65];
		char m_sBreakFileDate[15];
		char m_sBreakFile[FILE_NAME_LENGTH];
		int m_iBreakLine;
		char m_sFileTypeState[4];
		char m_sFilePatern[FILE_PATHER_LENGTH];

		Directory *m_pStoreDir;
		Directory *m_pBakDir;
		char m_sCurFileName[FILE_NAME_LENGTH];
		vector<DccFileFieldInfo *> m_vecFieldList;
		int m_iCurFieldSeq;
		File *m_pCurFile;
		bool m_bHaveDoneFile;
};


#endif

