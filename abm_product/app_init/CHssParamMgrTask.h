#ifndef CHBPARAMMGRTASK_H_HEADER_INCLUDED_B401BE33
#define CHBPARAMMGRTASK_H_HEADER_INCLUDED_B401BE33

#include "HssTask.h"
#include "ParamAccessor.h"
#include <string>
#include "MsgDefineClient.h"
#include "InDependProcess.h"
#include "SysParamDefine.h"
#include "IniFileMgr.h"
#include "BaseRuleMgr.h"
//#include "CDBAccessIF.h"
using namespace std;
#define	MAX_PARAM_FILE_LEN				1024
#define	MAX_PARAM_SIZE					1024


//##ModelId=4BFE31040141
class CHbParamMgrTask
	:public CHbTaskBase
{
  public:
    //##ModelId=4BFE313C006E
    CHbParamMgrTask();
    //##ModelId=4BFE313C00C8
    virtual ~CHbParamMgrTask();
    //##ModelId=4BFE332E0220
    bool getTaskResult(CmdMsg *vt,long& lMsgID,variant& vtRet,int* piRetSize=0);    //##ModelId=4BFE33360082    bool Init();	void OnExit(void);
	bool Init(void);
	void OnExit(void);
  private:
	string m_strIniFileName;
	string m_strMemKey;
	string m_strMemLockKey;


private:
	////逗号分隔不同的参数设置内容
	//bool splitParamInfo(string& strContent,string& strSection,
	//	string& strParam,string& strValue);
	SysParamHis* m_pParamInfo ;
	SysParamHis* m_pParamInfoVector;
	//参数的访问
	CIniFileMgr* m_pIniFileMgr;
	BaseRuleMgr* m_pRuleMgr;
	//添加
	long AddKey(stIniCompKey& oIniCompKey);	
	//CDBAccessIF*	m_pDBAccessIF;
private:
	void Clear(void);
	string m_strLogPath;
	CmdMsgFactory* m_pMsgFactory;
};



#endif /* CHBPARAMMGRTASK_H_HEADER_INCLUDED_B401BE33 */
