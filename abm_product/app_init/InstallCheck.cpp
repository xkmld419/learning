#include "InstallCheck.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>


using namespace std;
InstallCheck::InstallCheck()
{
	err = new char[256];
	m_poCIniFileMgr = new CIniFileMgr();
	m_poHssProcess = new HssProcessInit();
	//m_pCmdCom = new CommandCom();
	m_poBaseRuleMgr = new BaseRuleMgr();
	pProcessCtl = new ShmProcessControl();
	if(!err || !m_poCIniFileMgr || !m_poBaseRuleMgr || !pProcessCtl || !m_poHssProcess)
	{
		Log::log(0,"没内存空间了,请检查!");
		THROW(1);
	}
	/*
	if(!pProcessCtl->Init())
	{
		Log::log(0,"内存初始化失败,请检查!");
		THROW(1);
	}	
	*/
	m_poSysParamHis = 0;
	//m_pCmdCom->InitClient();
}

InstallCheck::~InstallCheck()
{
	if(m_poCIniFileMgr)
		delete m_poCIniFileMgr;
	if(err)
		delete []err;
	if(m_poBaseRuleMgr)
		delete m_poBaseRuleMgr;
	if(!m_poSysParamHis)
		delete []m_poSysParamHis;
}
int InstallCheck::checkAllParam()
{
	if(getAllParam() <=0 )
		return 0;
	if(!m_poSysParamHis)
		return 0;
	for(int i=0 ;i<m_iCnt ;i++)
	{
		int res  = checkParam(m_poSysParamHis[i].sSectionName,m_poSysParamHis[i].sParamName,m_poSysParamHis[i].sParamValue,m_poSysParamHis[i].sDate);
		if(res != 0)
			return res;
	}
	return 0;
}



int  InstallCheck::getAllParam()
{
	 //
	 /*if(!m_pCmdCom->getSection(NULL,&m_poSysParamHis,m_iCnt))
	 {
	 	return 0;
	 }*/
	 char *p = getenv("BILLDIR");
	 if(!p)
	 	return 0;
	 char filename[1024] = {0};
	 if(p[strlen(p)-1] == '/')
	 {
	 		sprintf(filename,"%s%s",p,"etc/hssconfig");
	 } else {
	 	  sprintf(filename,"%s/%s",p,"etc/hssconfig");
	 }
	 
	 m_poCIniFileMgr->LoadIniFromFile(filename);
	 if(!m_poCIniFileMgr->getSection(NULL,&m_poSysParamHis,m_iCnt))
	 {
	 		return 0;
	 }
	 return m_iCnt;
}

bool  InstallCheck::initprocess(bool usemem)
{
	 //
	 char *pMnt = "mnt -b";
	 if(system(pMnt) != 0){
	 	Log::log(0,"初始化失败,请检查参数!");
		return false;
	 }
	 pMnt = "mnt -r";
	 if(system(pMnt) != 0){
	 	Log::log(0,"配置初始化启动失败,请检查配置!");
		return false;
	 }
	 
	 	if(!pProcessCtl->Init())
	 {
		Log::log(0,"连接GuardMain共享内存失败,请检查!");
		THROW(1);
	 }	
	
	
	  Log::log(0,"开始启动HSS的各个模块……");
	  
	  if(m_poHssProcess->getProcess("hssserver")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
		}
	  }
	  
	  if(m_poHssProcess->getProcess("hssclient")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  
	  if(m_poHssProcess->getProcess("hssftp")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  
	  if(m_poHssProcess->getProcess("hssdccparse")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  
	  if(m_poHssProcess->getProcess("hssdccpkg")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	 
	  Log::log(0,"数据接收模块启动完毕…");
	  
	  if(m_poHssProcess->getProcess("hssquery")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  
	  if(m_poHssProcess->getProcess("hssorder")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
		
	  if(m_poHssProcess->getProcess("hss")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  
	  if(m_poHssProcess->getProcess("hssregist")>0)	
	  {
	  	for(vector<HssProInfo>::iterator itr = m_poHssProcess->vec_process.begin();itr!=m_poHssProcess->vec_process.end();itr++)
		{
	  		pProcessCtl->startProcess((int)(*itr).proid);
	  	}
	  }
	  Log::log(0,"数据更新模块启动完毕…");
	  
	  /*if(usemem)
	  {
	  	pProcessCtl->StartProc("hssstore");
	  	pProcessCtl->StartProc("hsspublish");
	  } else {
	  	m_poHssProcess->startApp("hssstore");
	  	m_poHssProcess->startApp("hsspublish");
	  }*/
	  Log::log(0,"数据发布模块启动完毕…");
	  Log::log(0,"按照验证成功");
	 return true;
}

int InstallCheck::checkParam(char *section,char*param,char*value,char*effdate)
{
	 if(section && section[0] != '\0')
	 {
	 	if(param && param[0] != '\0')
		{
			//if(m_poBaseRuleMgr->IsGroupParam(section,param))
			//	return true; // 组参数
			int res = m_poBaseRuleMgr->checkNowValue(section,param,value,effdate);
			if(res != 0)
			{
				printf("%s%s\n","参数检测失败,异常信息:",m_poBaseRuleMgr->sErrInfo);
			}
			return res;
		} else {
			printf("%s%s%s\n","段[",section,"]有参数名为空,无效参数,忽略");
		}
	 } else {
	 	//printf("%s\n","有无效参数,段名为空,忽略");
	 }
	 return 0;
}


