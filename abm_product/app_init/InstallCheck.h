#ifndef INSTALL_CHECK_H
#define INSTALL_CHECK_H
#include "CommandCom.h"
#include "MsgDefineClient.h"
#include "MsgDefineServer.h"
#include "SysParamDefine.h"
#include "BaseRuleMgr.h"
//#include "ParamAccessor.h"
#include "ProcessControler.h"
#include "HssProcessInit.h"


class InstallCheck
{
	public:
			InstallCheck();
			~InstallCheck();
			//
			int checkAllParam();
			bool  initprocess(bool usemem = true);
			char *err;
	private:
			BaseRuleMgr *m_poBaseRuleMgr;	
			ShmProcessControl *pProcessCtl;
			CommandCom *m_pCmdCom;
			CIniFileMgr *m_poCIniFileMgr;
			int m_iCnt;
			SysParamHis *m_poSysParamHis;
			HssProcessInit *m_poHssProcess;
			int  getAllParam();
			int checkParam(char *section,char*param,char*value,char*effdate=0);
};

#endif