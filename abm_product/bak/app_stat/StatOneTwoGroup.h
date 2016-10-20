#ifndef APP_STAT_STATONETWOGROUP_H
#define APP_STAT_STATONETWOGROUP_H

#define TIMELENG 20
#define SQLLENG 4000
#define TEMPLENG 1024

#include <map>
#include <vector>
#include <string>
using namespace std;

class ErrorData
{
public:
	ErrorData();
	~ErrorData();
	
	int m_iOneFileID;
	int m_iTwoFileID;
	int m_iOneCount;
	int m_iTwoCount;
	char m_sOneFileName[TEMPLENG];
	char m_sTwoFileName[TEMPLENG];
	char m_sErrorMsg[TEMPLENG];
};

class StatOneTwoGroup
{
public:
	StatOneTwoGroup();
	~StatOneTwoGroup();
	
	bool ControlMgr();
		
private:
	bool GetOneGroup();
	bool GetTwoGroup();
	bool CompareGroup();
	bool CommitAll(int iOpType=0);
	bool UpdateCfgTable();
	int LoadConfig();
	
private:
	int  m_iCommitCount;
	char m_sBeginTime[TIMELENG];
	char m_sOneEndTime[TIMELENG];
	char m_sTwoEndTime[TIMELENG];
	char m_sThisTime[TIMELENG];
	//char m_sNextTime[20];
	long m_lWaitTime;
	long m_lStatTime;
	long m_lAddTime;
	char m_sTypeList[TEMPLENG];
	map <string,string> m_mapOne;
	map <string,string> m_mapTwo;
	vector <ErrorData>  m_vErrorData;
};

#endif
