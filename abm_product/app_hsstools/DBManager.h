#pragma once

#include "Environment.h"

#include <string>
#include <vector>
#include <map>

class DBInfo
{
public:
	char sDBStr[64];
	char sUser[32];
	char sPasswd[32];
	char sOldPasswd[32];
public:
	DBInfo(){memset(this,0,sizeof(DBInfo));};
	~DBInfo(){};

};

class FileContent
{
public:
	int iFileID;
	char sFileName[512];
	char sSection[64];
	char sDBStr[64];
	char sUser[32];
	char sPasswd[32];
public:
	FileContent(){memset(this,0,sizeof(FileContent));};
	~FileContent(){};
};



typedef std::vector<DBInfo*> VEC_DBINFO;
typedef std::vector<FileContent*> VEC_FILECONTENT;
typedef std::map<std::string,VEC_FILECONTENT*> MAP_FILECONTENT;

class DBManager
{
public:
	DBManager(void);
	~DBManager(void);
public:
	//密码编码
	char* encode(char* sSrcStr,char * sDecStr);
	//密码解码
	char* decode(char* sSrcStr,char * sDecStr);

	//读取数据库中密码信息
	int GetPassInfo();
	//读取文件内容
	int GetFileContent();

	//修改数据库密码
	int ChangeDBPass(char* sDBStr,char* sUser,char* sOldPass,char* sNewPass);
	//修改文件
	int UpdateCfgFile();
	
	int doProcess();

	int CreateCfgFile();


private:
	std::string m_temp_path;
	void ClearVecDBInfo();
	void ClearFileContent();

	DBInfo *m_pDBInfo;
	VEC_DBINFO m_vecDBInfo;
	
	FileContent *m_pFileContent;
	MAP_FILECONTENT m_mapFileContent;
};
