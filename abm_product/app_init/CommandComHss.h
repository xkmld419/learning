#ifndef	_COMMAND_COM_H_
#define	_COMMAND_COM_H_
#include <unistd.h>
#include "SysParamDefine.h"
#include "Log.h"
#include <string>
#include <stack>
#include <vector>
#include <assert.h>
#include "IniFileMgrHss.h"
using namespace std;
#define			DEFAULT_CONFIG_FILE_NAME	"/etc/hssconfig"		//配置文件
#define			SHM_PARAM_SECTION				"MEMORY"
#define			SHM_PARAM_KEY					"param.sem_key_value"
#define			SHM_PARAM_SIZE					"param.init_data_size"

#define abc 
using namespace std;
// 提供服务注册,消息预约
//##ModelId=4BE010820312
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
	//启动Init服务
	//##ModelId=4BE01082039D
    	bool InitService();
	bool InitClient(unsigned int iCmdID=0,const char* cstrAppName=0);

	//读取系统启动参数
	//会累加结果
	bool GetInitParam(const char* pSection,const char* pKeyName=0);


	char *readIniString( char const *Section, char const *Ident, 
            char *sItem, char *defstr );
	long readIniInteger( char const *Section, char const *Ident, int defint );
	bool FlushParamEx(vector<SysParamHis>* pvtSysParamHis=0);	
	bool FlushParam(vector<SysParamHis>* pvtSysParamHis=0);
        void GetVecParam(vector<SysParamHis>& vtSysParam){vtSysParam=m_vtInitParam;}
	//读取修改的参数
	bool GetSubChangeParamInfo(vector<SysParamHis>* pvtSysParamHis=0);


protected :
		//##ModelId=4BE010820393
    bool Register(unsigned int iTimeOut=10,const char* cstrAppName=0);
	static	bool m_bRegister;

public:
	static bool __MsgInterruptFlag;

	static void SetAppName(char* cstrAppName);
	static char * g_strAppName;

  private:
    //##ModelId=4BF277ED020F
    static void SetPid(unsigned int iPid);
    //##ModelId=4BF2787C022F
    static unsigned int m_iPid;
	bool AttachShm(bool bCreate=false);
	static CIniFileMgr *m_pIniFileMgr;
	string m_strFileName;
  protected:
	vector<SysParamHis> m_vtInitParam;
public:
	bool FlashMemory(SysParamHis ** ppSysParamHis, int& iCnt);  
	bool GetIniKeyByString(string& strLine,CIniSection& oIniCompKey);
	bool getAllSection(SysParamHis ** ppSysParamHis,int& iCnt);
	bool getSectionParam(const char* pSection,const char* pParamName,
								SysParamHis& stParamHis);
	bool getSection(const char* pSection,SysParamHis** ppSysParamHis,
								int& iCnt);
	bool changeSectionKey(string strContent);
};

#endif


