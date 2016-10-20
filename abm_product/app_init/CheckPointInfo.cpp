#include "CheckPointInfo.h"
#include  <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include "Date.h"
#include "ParamDefineMgr.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "MsgDefineClient.h"
#include "MsgDefineServer.h"
#include "IpcKey.h"
CheckPointInfo::CheckPointInfo()
{
	m_poCommandCom = 0;
	lock = 0;
	m_iLockKey = 0;
	
}

CheckPointInfo::~CheckPointInfo()
{
	if(m_poCommandCom)
		delete m_poCommandCom;
	if(lock)
		delete lock;
}

char *CheckPointInfo::getCharBatchID()
{
	getBatch();
	return m_sBatchID;
}

// 赋值到sRootPath
bool CheckPointInfo::getCheckPointPath(char *sRootPath)
{
	/*if(!ParamDefineMgr::getParam(_CHECKPOINT, "CHECKPOINT_ROOT_PATH", m_sPath))
    {
		return false;
    }
	CheckFilePath(m_sPath);
	sRootPath[0] = '\0';
	sprintf(sRootPath,"%s\0",m_sPath);
	CheckFilePath(sRootPath);
	memset(m_sPath,'\0',sizeof(m_sPath));
	strncpy(m_sPath,sRootPath,sizeof(m_sPath));
	*/
	
	memset(m_sPath ,'\0',sizeof(m_sPath));
	char * p = getenv("BILLDIR");
	if(p[0] == '\0')
		return false;
	sprintf(m_sPath,"%s%s\0",p,"/log/check_point");
	sRootPath[0] = '\0';
	sprintf(sRootPath,"%s\0",m_sPath);
	CheckFilePath(sRootPath);
	Log::logEx(0,"落地文件根目录(%s)",sRootPath);
	return true;
}

// 赋值到sRootPath
char *CheckPointInfo::getChkPointBatchPath(char *batch)
{
	memset(m_sPath ,'\0',sizeof(m_sPath));
	char * p = getenv("BILLDIR");
	if(p==NULL || p[0] == '\0')
		return false;
	if(p[strlen(p)-1] != '/')
	 	sprintf(m_sPath,"%s%s%s",p,"/log/check_point/",batch);
	 else
	 	sprintf(m_sPath,"%s%s%s",p,"log/check_point/",batch);
	return m_sPath;
}

char *CheckPointInfo::getNowBatch()
{
	if(this->m_sBatchID[0] != '\0')
		return this->m_sBatchID;
	return NULL;
}

bool CheckPointInfo::createBatchID()
{
	 memset(this->m_sBatchID,'\0',strlen(this->m_sBatchID));
	 Date *p = new Date();
	 strncpy(this->m_sBatchID,p->toString(),strlen(this->m_sBatchID));
	 return true;
}

int CheckPointInfo::CheckFilePath (char * filepath)
{
    ///---apin 200611 检查目录是否存在
    //如果不存在则建立,
    DIR * dp;
    INFOPOINT(3,"[CheckPointInfo.CheckFilePath]检查配置查重目录是否存在:path:%s",filepath);
    if ((dp=opendir(filepath)) == NULL )
    {
        //不能打开目录,试图创建该目录
        if( mkdir(filepath,0770)!=0 )
        {
            Log::logEx(0,"checkpoint目录(%s)创建失败!",filepath);
			return -1;
        }
    }
    if(dp) 
        closedir(dp);  
    return 1;
}

void CheckPointInfo::getBatch()
{
	 memset(this->m_sBatchID,'\0',sizeof(this->m_sBatchID));
	 Date *p = new Date();
	 strncpy(this->m_sBatchID,p->toString(),sizeof(this->m_sBatchID));
	 delete p;
	 p = 0;
}

/*#ifdef DEF_SCHEDULE
bool CheckPointInfo::P(bool Real)
{
	if(!m_poCommandCom)
	{
		m_poCommandCom = new CommandCom();
		m_poCommandCom->InitClient(); 
	}
	string str("SYSTEM.checkpoint_state=1");
	bool bRet = m_poCommandCom->changeSectionKey(str);
	if(!bRet){
		INFOPOINT(3,"[CheckPointInfo.p()]%s","失败");	   
		return false;
	}else {
		int i = 1;
		INFOPOINT(3,"[CheckPointInfo.p()]%s","修改参数成功");
		if(Real)
			return true;
		while(1)
		{
			if(getNeedState())
				return true;
			else
				sleep(5);
		}
	}
}
#else*/
bool CheckPointInfo::P(bool Real)
{
	/*if(Real)//日志组切换数据不保证
		return true;*/
	//
	if(!m_poCommandCom)
	{
		m_poCommandCom = new CommandCom();
		m_poCommandCom->InitClient();
		char lockKey[20] = {0};
		m_poCommandCom->readIniString("CHECKPOINT","lock_key_value",lockKey,NULL);
		if(lockKey[0] != '\0')
			m_iLockKey	= atoi(lockKey);
	}
	if(!lock)
	{
		lock = new CSemaphore();
		if(m_iLockKey<=0)
		{
			m_iLockKey = IpcKeyMgr::getIpcKey (-1,"CHK_SEM");
		}
		char key[32] = {0};
		sprintf(key,"%d",m_iLockKey);
		if(!lock->getSem(key))
		{
			delete lock;
			lock = 0;
			return false;
		}		
	}
	return lock->P();
}
//#endif

bool CheckPointInfo::getNeedState()
{
	string str("SYSTEM.checkpoint_state");
	bool res = true;
	int iValue=0;
	if(!m_poCommandCom)
	{
		m_poCommandCom = new CommandCom();
		m_poCommandCom->InitClient();
		char lockKey[20] = {0};
		m_poCommandCom->readIniString("CHECKPOINT","lock_key_value",lockKey,NULL);
		if(lockKey[0] != '\0')
			m_iLockKey	= atoi(lockKey);
	}
	iValue = m_poCommandCom->readIniInteger("SYSTEM","checkpoint_state",iValue);
	if(iValue==2)
		return true;
	else
		return false;
}

int CheckPointInfo::waitTimer(int &iTimer,int iMaxTimeNum)
{
	if(iTimer == 1)
	{
	   iTimer++;
	   sleep(2);
	   return 1;
	}
	if(iTimer<=(iMaxTimeNum-1))
	{
		usleep(100);
		iTimer++;
		return 1;
	} else if(iTimer == iMaxTimeNum)
	{
		sleep(1);
		iTimer++;
		return 1;
	} else {
		return 0;	
	}
}

/*#ifdef DEF_SCHEDULE
bool CheckPointInfo::V()
{
	if(!m_poCommandCom)
	{
		m_poCommandCom = new CommandCom();
		m_poCommandCom->InitClient(); 
	}
	string str("SYSTEM.checkpoint_state=0");
	return m_poCommandCom->changeSectionKey(str);
}
#else*/
bool CheckPointInfo::V()
{
	if(lock)
	{
		return lock->V();
	}
	return true;
}
//#endif

bool CheckPointInfo::recordBatchInfo(char * sBatch, char* sTime, char *sDataType,char*desc,char *tablename)
{
	 if(sBatch[0] == '\0' || sTime[0] == '\0' || sDataType[0] == '\0')
	 {
	    return false;
	 }
	 
	 DEFINE_QUERY(qry);
     qry.close();
	 char sql[1024] = {0};
	 
	 sprintf(sql,"insert into b_check_point_record (batch_id,time_flag,data_type,chk_table_name,chk_desc) "
            	 " values ('%s','%s','%s','%s','%s')",sBatch,sTime,sDataType,tablename,desc);
	 qry.setSQL(sql);
     /*qry.setParameter("Batch_id",sBatch);
     qry.setParameter("Time_flag",sTime);
	 qry.setParameter("Data_type",sDataType);
	 qry.setParameter("Table",tablename);
	 qry.setParameter("Desc",desc);*/
	 try
	 {
		qry.execute();
		qry.commit();
	 }catch (TOCIException& e) 
	 {
		//Log::log(0,"%s","执行退出存储过程时出现错误");
		qry.close();
		return false;
	} catch (...)
	{
		return false;
	}
    qry.close(); 
    return true;
}


