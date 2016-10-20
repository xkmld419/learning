#include "CheckPointMgr.h"
//#include <string.h>
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
#include <signal.h>
#include <unistd.h>
#include "Date.h"
//#include "FileMgr.h"
//#include "CommandCom.h"
#include "ThreeLogGroup.h"
#include "ParamDefineMgr.h"
//#include "highFeeAggrMgr.h"
#include "File.h"
#include "IpcKey.h"
#include "MBC_ABM.h"

#define _HIGHFEE		"highfee"


#define HSS_HCODE_CHK			1
#define HSS_MIN_CHK				2
#define HSS_LOCAL_HEAD_CHK		3
#define HSS_IMSI_CHK			4

CheckPointMgr::CheckPointMgr()
{
	//m_poCommandCom = 0;
	string strLogWholeFileName;
	strLogWholeFileName = getenv("BILLDIR" );
	strLogWholeFileName += "/log/checkpoint.log";
	m_poLog = new ThreeLogGroup();
	m_poLogGroup = 0;
	
	memset(sHFKey,'\0',sizeof(sHFKey));
	memset(sHFKeyName,'\0',sizeof(sHFKeyName));
	memset(sHFFileName,'\0',sizeof(sHFFileName));
 	
	/*m_poCommandCom = new CommandCom();
	m_poCommandCom->InitClient();
	m_poCommandCom->GetInitParam("MEMORY");
	m_poCommandCom->readIniString("MEMORY","highfeelru.ipc_key_value",sHFKey,NULL);
	m_poCommandCom->readIniString("MEMORY","highfeelru.ipc_key_name",sHFKeyName,NULL);
	m_poCommandCom->readIniString("MEMORY","highfeelru.file_name",sHFFileName,NULL);
	if(sHFFileName[0] == '\0')
	{
		m_poCommandCom->GetInitParam("MONITOR");
		m_poCommandCom->readIniString("MONITOR","highfeelru.file_name",sHFFileName,NULL);
	}*/
	/*
	m_poSHMHFData = 0;
	if(sHFKey[0] == '\0')
	{
		int iIpcKey = IpcKeyMgr::getIpcKey(-1,"HIGH_FEE_DATA_INFO");
		sprintf(sHFKey,"%d",iIpcKey);
	}
	if(sHFKey[0] != '\0' && sHFFileName[0] != '\0')
	{
		m_poSHMHFData = new SHMLruData<HighFeeData>(atol(sHFKey),sHFFileName,0,0,0); 
	}*/
	m_poCheckPointInfo = new CheckPointInfo();
}

CheckPointMgr::~CheckPointMgr()
{
    /*if(m_poFileMgr){
       delete m_poFileMgr; 
	   m_poFileMgr=0;
    }*/
    /*if(m_poLogGroup){
        delete m_poLogGroup;m_poLogGroup=0;
	}*/
	if(m_poCheckPointInfo){
		delete m_poCheckPointInfo;m_poCheckPointInfo=0;
	}
	/*if(m_poCommandCom){
		delete m_poCommandCom;m_poCommandCom=0;
	}*/
	/*
	if(m_poSHMHFData)
	{
	  delete m_poSHMHFData;
	  m_poSHMHFData = 0;
	}*/
}

long CheckPointMgr::getLongBatchID()
{
	getBatch();
	if(strlen(m_sBatchID)<19)
		return atol(m_sBatchID);
	else
		return 0;
}

char *CheckPointMgr::getCharBatchID()
{
	getBatch();
	return m_sBatchID;
}


int CheckPointMgr::CheckFilePath (char * filepath)
{
    ///---apin 200611 检查目录是否存在
    //如果不存在则建立,
    DIR * dp;
    INFOPOINT(2,"[CheckPointMgr.CheckFilePath]检查配置查重目录是否存在:path:%s",filepath);
    if ((dp=opendir(filepath)) == NULL )
    {
        //不能打开目录,试图创建该目录
        if( mkdir(filepath,0770)!=0 )
        {
            Log::log(0,"checkpoint目录(%s)创建失败!",filepath);
			return -1;
        }
    }
    if(dp) 
        closedir(dp);  
    return 1;
}

// 赋值到sRootPath
bool CheckPointMgr::getCheckPointPath(char *sRootPath)
{
	/*memset(m_sPath ,'\0',sizeof(m_sPath));
	char * p = getenv("BILLDIR");
	sprintf(m_sPath,"%s%s\0",p,"/log/check_point");
	sRootPath[0] = '\0';
	sprintf(sRootPath,"%s\0",m_sPath);
	CheckFilePath(sRootPath);
	Log::log(0,"落地文件根目录(%s)",sRootPath);
	return true;*/
	return m_poCheckPointInfo->getCheckPointPath(sRootPath);
}

void CheckPointMgr::getBatch()
{
	 memset(this->m_sBatchID,'\0',sizeof(this->m_sBatchID));
	 Date *p = new Date();
	 strncpy(this->m_sBatchID,p->toString(),sizeof(this->m_sBatchID));
	 delete p; p = 0;
}

bool CheckPointMgr::touchInfoDown()
{
	 return touchInfoDown(true);
}

bool CheckPointMgr::touchInfoDown(bool Show,bool RealTime)
{
	 Date mpDate;
     char slogg[500];
     bool res = true;
	 getBatch();
	 if(this->m_sBatchID[0] == '\0')
	 	return false;
	 memset(m_sPath,'\0',sizeof(m_sPath));
	 strcpy(m_sPath,m_poCheckPointInfo->getChkPointBatchPath(this->m_sBatchID));
	 if(m_sPath[0] == '\0')
	 	return false;
	 CheckFilePath(m_sPath);
	 if(Show)
	 {
	 	if(m_sPath[0]=='\0')
	 		Log::log(0,"批次号[%s],开始执行落地",m_sBatchID);
		else
			Log::log(0,"批次号[%s],开始执行落地所有共享数据到[%s]",m_sBatchID,m_sPath);
	 }
	 //m_sPath 路径 this->m_sBatchID 批次
	 char order[1024] = {0};
	 char file[512] = {0}; 
	 try
	 {
	 		//if(!m_poFileMgr)
			//	m_poFileMgr = new FileMgr(2);
			//HCODE
			sprintf(file,"%s/%s%s%s",m_sPath,"hhi",this->m_sBatchID,".chkpoint");
			sprintf(order,"%s%s","hssexport -q -m hhi -f ",file);
    		if(system(order)!=0)
				THROW(1);
			
			Date date;
			m_poLog->log(MBC_ACTION_CHECK_POINT,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"执行CheckPoint成功,落地数据[HCODE]信息,批次[%s]",this->m_sBatchID);
			//m_poCheckPointInfo->recordBatchInfo(this->m_sBatchID,date.toString(),"file",file);
	 }
	 catch(...)
	 {
	 	if(Show)
	 		Log::log(0,"批次号[%s],执行落地HCODE数据失败",m_sBatchID);
		Date date;
		m_poLog->log(MBC_CHECK_POINT_ERROR,LOG_LEVEL_ERROR,LOG_TYPE_SYSTEM,"执行CheckPoint失败,无法落地数据[HCODE]信息,批次[%s]",this->m_sBatchID);
		res = false;
	 }
	 
	 file[0] = 0; 
	 order[0] = 0;
	 try
	 {
			//MIN码
			sprintf(file,"%s/%s%s%s",m_sPath,"hmi",this->m_sBatchID,".chkpoint");
    		sprintf(order,"%s%s","hssexport -q -m hmi -f ",file);
    		if(system(order)!=0)
				THROW(1);
			Date date;
			m_poLog->log(MBC_ACTION_CHECK_POINT,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"执行CheckPoint成功,落地数据[MIN码]信息");
			//m_poCheckPointInfo->recordBatchInfo(this->m_sBatchID,date.toString(),"file",file);
	 }
	 catch(...)
	 {
	 	if(Show)
	 		Log::log(0,"批次号[%s],执行落地MIN码数据失败",m_sBatchID);
		Date date;
		m_poLog->log(MBC_CHECK_POINT_ERROR,LOG_LEVEL_ERROR,LOG_TYPE_SYSTEM,"执行CheckPoint失败,无法落地数据[MIN码]信息");
		res = false;
	 }
	 
	 file[0] = 0; 
	 order[0] = 0;
	 try
	 {
			//本地号首
			sprintf(file,"%s/%s%s%s",m_sPath,"hli",this->m_sBatchID,".chkpoint");
    		sprintf(order,"%s%s","hssexport -q -m hli -f ",file);
    		if(system(order)!=0)
				THROW(1);
			Date date;
			m_poLog->log(MBC_ACTION_CHECK_POINT,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"执行CheckPoint成功,落地数据[本地号首]信息");
			//m_poCheckPointInfo->recordBatchInfo(this->m_sBatchID,date.toString(),"file",file);
	 }
	 catch(...)
	 {
	 	if(Show)
	 		Log::log(0,"批次号[%s],执行落地本地号首数据失败",m_sBatchID);
		Date date;
		m_poLog->log(MBC_CHECK_POINT_ERROR,LOG_LEVEL_ERROR,LOG_TYPE_SYSTEM,"执行CheckPoint失败,无法落地数据[本地号首]信息");
		res = false;
	 }
	 
	 file[0] = 0; 
	 order[0] = 0;
	 try
	 {
			//IMSI
			sprintf(file,"%s/%s%s%s",m_sPath,"hii",this->m_sBatchID,".chkpoint");
    		sprintf(order,"%s%s","hssexport -q -m hii -f ",file);
    		if(system(order)!=0)
				THROW(1);
			Date date;
			m_poLog->log(MBC_ACTION_CHECK_POINT,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"执行CheckPoint成功,落地数据[IMSI]信息");
			//m_poCheckPointInfo->recordBatchInfo(this->m_sBatchID,date.toString(),"file",file);
	 }
	 catch(...)
	 {
	 	if(Show)
	 		Log::log(0,"批次号[%s],执行落地IMSI数据失败",m_sBatchID);
		Date date;
		m_poLog->log(MBC_CHECK_POINT_ERROR,LOG_LEVEL_ERROR,LOG_TYPE_SYSTEM,"执行CheckPoint失败,无法落地数据[IMSI]信息");
		res = false;
	 }
	 
	 if(Show)
	 	Log::log(0,"批次号[%s],执行落地所有共享数据结束",m_sBatchID);
     return res;
}

// 参数说明:sBatch是批次标识.
// 返回值说明:TRUE 上载成功 FALSE 上载失败
bool CheckPointMgr::touchInfoLoad(char *sBatch)
{
	 bool res = true;
	 try
	 {
     	//res = (res && m_poFileMgr->synInfoIn(sBatch));
	 }
	 catch(...)
	 {
	 	//Log::log(0,"批次号[%s],执行上载(FileMgr使用数据)时失败",sBatch);
	 }
	 //char slogg[1024] = {0};
	 //Date mpDate;
     //sprintf(slogg,"[checkpoint] 批次号:%s 时间:%s FileMgr执行上载(总账,累计量,优惠)!",sBatch,mpDate.toString("yyyy-mm-dd hh:mi:ss"));
    // m_poLogGroup->LogGroupWriteFile(SYSTEMLOG, slogg);
	 
	 return res;
}


//判断文件是否存在
bool CheckPointMgr::FileExist(char *sFileName)
{
	if(sFileName[0] == '\0')
		return false;
	FILE *fp = 0;
	if ((fp = fopen( sFileName,"r")) == NULL)
    {
    	if (( fp = fopen(sFileName,"wb"))==NULL)
        {
			return false;			
		} 
	} 
	return true; 
}


