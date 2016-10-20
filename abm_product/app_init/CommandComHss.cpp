#include "CommandComHss.h"
#include <unistd.h>
#define	_JUST_DBLINK_
#include <signal.h>
#include "ReadIni.h"
unsigned int CommandCom::m_iPid=0;
bool CommandCom::m_bRegister=false;
char	*	CommandCom::g_strAppName=0;
CIniFileMgr *CommandCom::m_pIniFileMgr=0;
CommandCom::CommandCom(void)
{
	m_pIniFileMgr = 0;
	SetPid(getpid());
	m_bRegister =false;
}

void CommandCom::SetAppName(char* cstrAppName)
{
		CommandCom::g_strAppName=cstrAppName;
}

CommandCom::~CommandCom(void)
{
	//Logout();
}


bool CommandCom::Logout()
{
	m_bRegister=false;
	return true;
}
bool CommandCom::AttachShm(bool bCreate)
{
	string strFileName=DEFAULT_CONFIG_FILE_NAME;
	char *p = getenv ("TIBS_HOME");
	if(p)
		strFileName.insert(0,p);
	if (-1 ==access( strFileName.c_str(), R_OK| W_OK| F_OK ) ){
		Log::log(0,"文件不存在或者由于权限问题无法访问:%s",
			strFileName.c_str());
		return false;
	}
	char sParamValue[256]={0};	//共享内存键值
	ReadIni * pReadIni = new ReadIni();
	pReadIni->readIniString(strFileName.c_str(),SHM_PARAM_SECTION,
		SHM_PARAM_KEY,sParamValue,sParamValue);
	if(sParamValue[0]==0){
		Log::log(0,"从文件中:%s 读取Section :%s,ParamKey: %s 失败",
			strFileName.c_str(),SHM_PARAM_SECTION,SHM_PARAM_KEY);
		return false;
	}
	m_strFileName=strFileName;
	bool bRet =false;	
	if(bCreate){
		char sShmSize[256]={0};//共享内存大小
		int iShmSize=0;
		pReadIni->readIniString(strFileName.c_str(),SHM_PARAM_SECTION,SHM_PARAM_SIZE,
			sShmSize,sShmSize);
		iShmSize = atoi(sShmSize);
		if(0==iShmSize)
			iShmSize=MAX_INI_KEY_SIZE;
		bRet= m_pIniFileMgr->LoadIniFromFile(strFileName.c_str());
		bRet= m_pIniFileMgr->CopyToShm(sParamValue,iShmSize);
		if(!bRet){
			Log::log(0,"装载文件失败");
			m_strFileName.clear();
			return false;
		}
	}else{
		m_pIniFileMgr->GetDataFromShm(sParamValue);
	}
	return true;	
}
bool CommandCom::Register(unsigned int iTimeOut,const char* cstrAppName)
{
	if(m_bRegister)
		return true;

		//不在投递消息了，直接连接共享内存算了
	if(!AttachShm())
			return false;
	return true;
//	if(m_bRegister)
}

bool CommandCom::InitClient(unsigned int iCmdID,const char* cstrAppName)
{
	if(0==cstrAppName)
		cstrAppName=g_strAppName;
	if(0== m_pIniFileMgr){
		m_pIniFileMgr = new CIniFileMgr();
	}
	return Register(10,cstrAppName);
}

bool CommandCom::InitService()
{
	if(0==m_pIniFileMgr){
	m_pIniFileMgr = new CIniFileMgr();
	}
	return AttachShm(true);
}

void CommandCom::SetPid(unsigned int iPid)
{
	m_iPid = iPid;
}


char* CommandCom::readIniString( char const *Section, char const *Ident, 
            char *sItem, char *defstr )
{
	SysParamHis oParamHis;
	if(getSectionParam(Section,Ident,oParamHis)){
		strcpy(sItem,oParamHis.sParamValue);
		return sItem;
	}
	return defstr;
}

long CommandCom::readIniInteger(const char *Section, const char *Ident, int defint)
{
	SysParamHis oParamHis;
	if(getSectionParam(Section,Ident,oParamHis)){
		defint = atoi(oParamHis.sParamValue);
		return defint;
	}
	return 0; 
}

bool CommandCom::FlashMemory(SysParamHis ** ppSysParamHis, int& iCnt)
{
	if(m_pIniFileMgr){
		m_pIniFileMgr->setCfgFileName(m_strFileName);
		return m_pIniFileMgr->FlashMemory(ppSysParamHis,iCnt);
	}
	return false;
}
bool CommandCom::GetIniKeyByString(string& strLine,CIniSection& oIniCompKey)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->GetIniKeyByString(strLine,oIniCompKey);
	return false;
}
bool CommandCom::getAllSection(SysParamHis ** ppSysParamHis,int& iCnt)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->getAllSection(ppSysParamHis,iCnt);
	return false;
}
bool CommandCom::getSectionParam(const char* pSection,const char* pParamName,
                    SysParamHis& stParamHis)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->getSectionParam(pSection,pParamName,stParamHis);
	return false;
}
bool CommandCom::getSection(const char* pSection,SysParamHis** ppSysParamHis,
                      int& iCnt)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->getSection(pSection,ppSysParamHis,iCnt);
	return false;
}
bool CommandCom::changeSectionKey(string strContent)
{
	if(m_pIniFileMgr){
		if(m_pIniFileMgr->changeSectionKey(strContent)){
			if(m_pIniFileMgr->SaveToFile(m_strFileName.c_str()))
				return true;
		}
	}
	return false;
}
