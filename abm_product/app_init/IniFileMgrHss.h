#ifndef	_INIT_FILE_MGR_H_
#define	_INIT_FILE_MGR_H_
#include <sys/types.h>
#include <sys/ipc.h>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include "Log.h"
#include <vector>
#include "SHMData.h"
#include "WriteIni.h"
#include <string>
#include <iostream>
#include "CRWLock.h"
#include "SysParamDefine.h"

using namespace std;
class SysParamHis;
class CIniSection{
	public :
		CIniSection()
		{
			memset(sSectionName,0,sizeof(sSectionName));
			memset(sKeyName,0,sizeof(sKeyName));
			memset(sKeyValue,0,sizeof(sKeyValue));
			memset(sEffDate,0,sizeof(sEffDate));
		};
	char sSectionName[MAX_SECTION_LEN_INIT];
	char sKeyName[MAX_KEY_NAME_LEN];
	char sKeyValue[MAX_STACK_LEN][MAX_KEY_VALUE_LEN];
	char sEffDate[MAX_DATE_LEN];
};


class BaseRuleMgr;
class CIniFileMgr
{
public:
	CIniFileMgr(void);
	~CIniFileMgr(void);
	
	bool ClearShm(void);

	//从文件中装载Ini配置
	bool LoadIniFromFile(const char* cpsFileName);

	bool ReLoadIniFromFile(void);
	//保存到文件
	bool SaveToFile(const char* cpsFileName);

	//清理数据
	void Clear(void);

	bool Init(void);

	////从流中装载Ini配置
	//bool LoadIniFromString(string& strIniContent);
	bool GetIniKey(const char* sSectionName,const char* sParamName,
		CIniSection& oIniCompKey);

	bool GetIniKey(const char* sSectionName,const char* sParamName,
		stIniCompKey& oIniCompKey);

	////信息复制到共享内存
	bool CopyToShm(const char * cpsKey,long lShmSize=0);
	bool CopyToShm(const char * cpsKey,const char* cpsLockKey,long lShmSize=0){return true;};

	////从共享内存读取配置信息 
	bool GetDataFromShm(const char* cpsKey);
	
	bool Compare(CIniFileMgr* pIniFileMgr,vector<stIniCompKey>& vtCompKey,bool bLock=true);

	bool SaveKey(const char* cpsFileName,stIniCompKey & oIniKey);

	bool AddKey(CIniSection& oIniKey,OP_FLAG flag);
	bool AddKey(stIniCompKey& Key);
	bool ReplaceKey(stIniCompKey &oIniKey);
	void FlashIt(void);
	bool FlashMemory(SysParamHis ** ppSysParamHis, int& iCnt);  	
	void SetLockKey(const char* sLockKey) { m_strMemLockKey=sLockKey;};

	bool GetIniKeyByString(string& strLine,CIniSection& oIniCompKey);
	bool GetIniKeyByString(string& strLine,stIniCompKey& oIniCompKey);
	bool InsertSection(CIniSection* pIniSection);
	bool getAllSection(SysParamHis ** ppSysParamHis,int& iCnt);
	bool getSectionParam(const char* pSection,const char* pParamName,
					SysParamHis& stParamHis);
	bool getSection(const char* pSection,SysParamHis** ppSysParamHis,
					int& iCnt);
	bool changeSectionKey(string strContent);
	void setCfgFileName(string& strFileName){ m_strFileName=strFileName;};
	string m_strFileName;

	unsigned int LoadFromStream(istream& inputStream);

	bool AttachIniShm(const char* cpsKey);

	bool AnalysisLine(string &strLine ,string& strSection,string& strKey,string& strValue);

	string m_strSection;

	unsigned int m_iIniKeyNum;
	
	void ShiftRight(char sParamValue[MAX_STACK_LEN][MAX_KEY_VALUE_LEN] ,int iPos=1);

	//合并相同的节点
	bool MergeSection(CIniSection* pSectionDest,CIniSection* pSectionSource1);
	bool MergeSection(CIniSection* pSectionDest,CIniSection* pSectionSource1,
			CIniSection* pSectionSource2);

	//内存扩充
	int ExtendSectionMem(CIniSection** ppSection,int iSectionCnt);

	unsigned int GetUseNum(void) {return m_iUseNum;};
	unsigned int GetTotalNum(void) {return m_iTotalNum;};
private:
	bool AddKey(const char* pSection,const char* pParamName,const char* pParamValue,
			OP_FLAG flag);
	void ChangeSHMDataCount(unsigned int iCnt);

public :
	CIniSection* m_pIniKey;
	SHMData<CIniSection>* m_poIniSection;
	unsigned int m_iTotalNum;//总共多少
	unsigned int m_iUseNum;//用了多少
	string m_strMemKey;
	string m_strMemLockKey;
	int m_iMemLockKey;
	char m_sTmp[MAX_TMP_STRING_LEN];
	CRWLock* m_prwLock;
	BaseRuleMgr* m_pRuleMgr;
};

#endif

