#include "Environment.h"
#include <string>
#include <map>
#include <vector>

#include <sstream>
#include <fstream>
#include "Date.h"

using namespace std;

class FileInfo
{
public:
	FileInfo(void):m_iCount(0){m_dStateDate.getCurDate();};
	~FileInfo(void);
public:
	string m_sFileName;
	ofstream m_File;
	int m_iCount;
	ostringstream m_osContent;
	//ostringstream m_osFileID;
	vector<long> m_vecFileID;
	Date m_dStateDate;
};

typedef map<string,FileInfo *> FILE_MAP;

class PrepLog
{
public:
	PrepLog(void);
	~PrepLog(void);

public:
	void run();

	//查找文件
	FileInfo *getFileInfo(string sAreaCode,int iFlag = 0);//iFlag:0 不创建新文件，1 创建新文件

	//写文件
	int writeFile(string sTmpPath,string sPath,string sAreaCode );
	//更新表状态
	int updateState(string sAreaCode );

	void clearFileMap();
	//判断时间强制提交
	int forceCommit(string sTmpPath,string sPath,int iSecond);
	string GetUpdateString(const char* sFileId,char* sState);
private:
	FILE_MAP m_FileMap;
};
