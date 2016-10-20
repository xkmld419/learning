#ifndef	_CDBACCESSIF_H_
#define	_CDBACCESSIF_H_

#include "MsgDefineServer.h"
#include "IniFileMgr.h"
#include "TOCIQuery.h"
#define		MAX_SQL_NAME_LEN				256
#define		MAX_SQL_PASSWORD_LEN		256
#define		MAX_TNS_LEN								256



typedef  struct _DB_INIFO_
{
	char sName[MAX_SQL_NAME_LEN];
	char sPassWord[MAX_SQL_PASSWORD_LEN];
	char sTnsName[MAX_TNS_LEN];
	char sSection[MAX_SECTION_LEN_INIT];
}DBInfo;

class CDBAccessIF
{
public:
	~CDBAccessIF();
	static CDBAccessIF* GetInstance(void);

	//处理需要插入表数据
	//返回数据库处理失败
	long	DealInDbData(stIniCompKey& oIniCompKey);

	//处理数据连接
	long	DealDBConnect(stIniCompKey& oIniCompKey);

	long	InitDBConnect(const char* pSysParamName);


	bool TestConnect(char* pName,char* pPass,char* pTns);
	long	ChangeDBConnectInfo(const char* pSection,char* pName,char* pPass,char* pTns);

	bool SetDefaultConnect(char* pName,char* pPass,char* pTns);
	bool TestMySelf(void);
	void Clear();
protected:
	CDBAccessIF();
	bool Init(void);
	long ChangeIPCKEY(int iBillFlowID,char* pValue,const char* pKeyName);
	long ChangeSwitchDB(long lSwitchID,const char *pValue);
	long ChangeParamDefineDB(const char* pSegment,const char* pCode,
		const char* pValue);
	static CDBAccessIF* m_gpDBAccessIF;

	TOCIDatabase* m_pDatabase;
	TOCIQuery* m_pQuery;
private:
	   int     m_iHostID;
        char    m_sUserName[64];
};

#endif

