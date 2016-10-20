#ifndef	_COMMAND_COM_H_
#define	_COMMAND_COM_H_
#include <unistd.h>
#include "SysParamDefine.h"
#include "Log.h"
#include <string>
#include <stack>
#include <vector>
#include <assert.h>
#include "IniFileMgr.h"
#include "ThreeLogGroup.h"

using namespace std;
#define			DEFAULT_CONFIG_FILE_NAME	"/etc/sgwconfig"		//配置文件
#define			SHM_PARAM_SECTION				"MEMORY"
//#define			SHM_PARAM_KEY					"param.sem_key_value"
#define			SHM_PARAM_KEY					"param.ipc_key_value"
#define			SHM_PARAM_SIZE					"param.init_data_size"

using namespace std;
// 提供服务注册,消息预约
//##ModelId=4BE010820312
class ThreeLogGroup ;
class CommandCom
{
public:
    //  提供
    //##ModelId=4BE01082038D
	CommandCom(void);
    // 提供消息队列访问功能
    //##ModelId=4BE010820390
	virtual ~CommandCom(void);
	
	bool Logout();

	bool InitClient(unsigned int iCmdID=0,const char* cstrAppName=0);
	bool GetInitParam(const char* pSection,const char* pKeyName=0);


	char *readIniString( char const *Section, char const *Ident, 
            char *sItem, char *defstr );
	long readIniInteger( char const *Section, char const *Ident,
			int defint );
	bool reLoad(void);
protected :
		//##ModelId=4BE010820393
	bool Register(unsigned int iTimeOut=10,const char* cstrAppName=0);
	static	bool m_bRegister;

public:

	static void SetAppName(char* cstrAppName);
	static char * g_strAppName;
	static ThreeLogGroup *m_poLogg;
	//static ThreeLogGroup *m_poLogg;
	

private:

	bool AttachShm(void);
	char toLower(const char ch);
    bool Chang2Lower(char *sSrc);
	static CIniFileMgr *m_pIniFileMgr;
	string m_strFileName;

public:
	bool FlashMemory(SysParamHis ** ppSysParamHis, int& iCnt);  
	bool FlashMemoryEx(SysParamHis ** ppSysParamHis, int& iCnt,SysParamHis **ppSysParamHisEx,int& iCntEx);  
	bool GetIniKeyByString(string& strLine,CIniSection& oIniCompKey);
	bool getAllSection(SysParamHis ** ppSysParamHis,int& iCnt);
	bool getAllSections(SysParamHis ** ppSysParamHis,int& iCnt);
	bool getSectionParams(const char* pSection,const char* pParamName,
		SysParamHis **ppstParamHis,int& iCnt);
	bool getSectionParam(const char* pSection,const char* pParamName,
								SysParamHis& stParamHis);
	bool getSection(const char* pSection,SysParamHis** ppSysParamHis,
								int& iCnt);
	bool changeSectionKey(string strContent);
	void setParamUse(bool flag);
};

#endif


