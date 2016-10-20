#include "CommandCom.h"
#include <unistd.h>
#define	_JUST_DBLINK_
#include "Environment.h"
#include <signal.h>
#include "ReadIni.h"
#include "GlobalTools.h"
#include "BaseFunc.h"
using namespace StandardDevelopMent;

bool CommandCom::m_bRegister=false;
char	*	CommandCom::g_strAppName=0;
CIniFileMgr* CommandCom::m_pIniFileMgr = 0;
ThreeLogGroup* CommandCom::m_poLogg=0;
CommandCom::CommandCom(void)
{
	m_bRegister =false;
	if(m_poLogg==0)
     m_poLogg = new ThreeLogGroup(true);
}

void CommandCom::SetAppName(char* cstrAppName)
{
	CommandCom::g_strAppName=cstrAppName;
}
void CommandCom::setParamUse(bool flag)
{
	 if(m_pIniFileMgr)
	 {
	 	m_pIniFileMgr->m_bParamUse = flag;
	 }
}
CommandCom::~CommandCom(void)
{
}

bool CommandCom::GetInitParam(const char* pSection,const char* pKeyName)
{
	return true;
}
bool CommandCom::Logout()
{
	m_bRegister=false;
	return true;
}
bool CommandCom::reLoad(void)
{
	if(0!=m_pIniFileMgr){
		m_pIniFileMgr->Clear();
		m_pIniFileMgr->LoadIniFromFile(m_strFileName.c_str());
	}
	return InitClient();				
}
bool CommandCom::AttachShm(void)
{
	string strFileName;

    StandardTools::GetStdConfigFile(strFileName);
    if(strFileName.empty())
        return false;
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
    if(m_pIniFileMgr ==NULL){
        m_pIniFileMgr= new CIniFileMgr();
    }
	m_pIniFileMgr->Init(m_strFileName.c_str());
	bool bRet =false;	
	m_pIniFileMgr->GetDataFromShm(sParamValue);
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
}

bool CommandCom::InitClient(unsigned int iCmdID,const char* cstrAppName)
{
	if(0==cstrAppName)
		cstrAppName=g_strAppName;
	return Register(10,cstrAppName);
}


char* CommandCom::readIniString( char const *Section, char const *Ident, 
            char *sItem, char *defstr )
{
	char sTmp[512] = {0};
  	strcpy(sTmp,Section);
  	bool bRet = Chang2Upper(sTmp);
	SysParamHis oParamHis;
	if(getSectionParam(sTmp,Ident,oParamHis)){
		if(m_pIniFileMgr->IsRestart(Section,Ident)&&(oParamHis.sParamOrValue[0]!='\0')){
			strcpy(sItem,oParamHis.sParamOrValue);
		}else{
			strcpy(sItem,oParamHis.sParamValue);
		}
		return sItem;
	}
	return defstr;
}

long CommandCom::readIniInteger(const char *Section, const char *Ident, int defint)
{
	char sTmp[512] = {0};
	int iTmp = 0;
  strcpy(sTmp,Section);
  bool bRet = Chang2Upper(sTmp);
	SysParamHis oParamHis;
	if(getSectionParam(sTmp,Ident,oParamHis)){
		if(m_pIniFileMgr->IsRestart(Section,Ident)&&(oParamHis.sParamOrValue[0]!='\0')){
			iTmp=atoi(oParamHis.sParamOrValue);
		}else{
			iTmp = atoi(oParamHis.sParamValue);
		}
		if(strlen(oParamHis.sParamName) == 0)
			return defint; 
		else
			return iTmp;
	}
	return defint; 
}

bool CommandCom::FlashMemory(SysParamHis ** ppSysParamHis, int& iCnt)
{
	if(m_pIniFileMgr){
		m_pIniFileMgr->setCfgFileName(m_strFileName);
		return m_pIniFileMgr->FlashMemory(ppSysParamHis,iCnt);
	}
	return false;
}
bool CommandCom::FlashMemoryEx(SysParamHis ** ppSysParamHis, int& iCnt,SysParamHis **ppSysParamHisEx,int& iCntEx)
{
	if(m_pIniFileMgr){
		m_pIniFileMgr->setCfgFileName(m_strFileName);
		return m_pIniFileMgr->FlashMemoryEx(ppSysParamHis,iCnt,ppSysParamHisEx,iCntEx);
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

bool CommandCom::getAllSections(SysParamHis ** ppSysParamHis,int& iCnt)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->getAllSections(ppSysParamHis,iCnt);
	return false;
}
bool CommandCom::getSectionParams(const char* pSection,const char* pParamName,
                    SysParamHis** ppstParamHis,int& iCnt)
{
	if(m_pIniFileMgr)
		return m_pIniFileMgr->getSectionParams(pSection,pParamName,ppstParamHis,iCnt);
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
				return true;
		}
	}
	return false;
}

char CommandCom::toLower(const char ch)
{
	// A - Z
 	if (ch >= 65 && ch <= 90)  return ch | 0x20; // 0010 0000
 	return ch;
}

bool CommandCom::Chang2Lower(char *sSrc)
{
	 int iLen = strlen(sSrc);
	 char sBuf[4096] = {0};
     if(iLen>4096 || sSrc[0] == '\0')
        return false;
 	 char* p = sSrc;
 	 int iPos = 0; 	 
 	 while (*p != '\0')
 	 {
       sBuf[iPos++]=toLower(*p);
	     sBuf[iPos] = '\0';
	     ++p;
	 }    
     sSrc[0] = '\0';
     strcpy(sSrc,sBuf);	 
	 return true;
}
