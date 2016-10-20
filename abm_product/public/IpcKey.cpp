/*VER: 1*/ 

#define _JUST_DBLINK_

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>

#include "WfPublic.h"
#include "IpcKey.h"
#include "Environment.h"
#include "Log.h"

//////////////////////////////////////////////////////////
/*
    获取系统中定义的 当前主机、当前用户，指定流程、IPC名称 对应的IPC_KEY 值
    成功, 返回值为获取的 IPC_KEY
    失败, 返回值 小于 0

    xueqt: 作为public下面的, 异常情况, 可能有致命影响的, 暂都改用THROW
*/
long IpcKeyMgr::getIpcKey(int _iBillFlowID, char const * _sIpcName)
{
    char *pUserName;
    char sHostName[200];
    char sSql[2048];
    long lKeyRet;
    DEFINE_QUERY(qry);
    
    pUserName = getenv("LOGNAME");
    int iHostID = getHostInfo(sHostName);
        
    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_IpcKey + 1);
    }
    
    sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG "
        " WHERE BILLFLOW_ID=%d AND IPC_NAME='%s' "
        " AND host_id=%d"
        " AND sys_username='%s'",
        _iBillFlowID, _sIpcName, iHostID, pUserName);
    
    qry.close ();
    qry.setSQL (sSql); qry.open ();

    if (!qry.next ()) {
        Log::log (0, "B_IPC_CFG中没有配置：%s, USER_NAME: %s, HOST_ID:%d", 
                _sIpcName, pUserName, iHostID);
        THROW (MBC_IpcKey + 2);
    }
    
    lKeyRet = qry.field("IPC_KEY").asLong();
    qry.close ();
    
    return lKeyRet;
}

long IpcKeyMgr::getIpcKey(bool bForceBillFlow,int _iBillFlowID, char const * _sIpcName)
{
	if(bForceBillFlow){
		return getIpcKey(_iBillFlowID,_sIpcName);
	}
    char *pUserName;
    char sHostName[200];
    char sSql[2048];
    long lKeyRet;
    DEFINE_QUERY(qry);
    
    pUserName = getenv("LOGNAME");
    int iHostID = getHostInfo(sHostName);
        
    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_IpcKey + 1);
    }
    
    sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG "
        " WHERE BILLFLOW_ID=%d AND IPC_NAME='%s' "
        " AND host_id=%d"
        " AND sys_username='%s'",
        _iBillFlowID, _sIpcName, iHostID, pUserName);
    
    qry.close ();
    qry.setSQL (sSql); qry.open ();

    if (!qry.next ()) {
		qry.close();
		memset(sSql,0,sizeof(sSql));
		sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG "
        " WHERE BILLFLOW_ID=-1 AND IPC_NAME='%s' "
        " AND host_id=%d"
        " AND sys_username='%s'",_sIpcName, iHostID, pUserName);

		qry.setSQL (sSql); qry.open ();

		if (!qry.next ()) {
			Log::log (0, "B_IPC_CFG中没有配置：%s, USER_NAME: %s, HOST_ID:%d,BILLFLOW_ID:(%d OR -1)", 
					_sIpcName, pUserName, iHostID,_iBillFlowID);
			THROW (MBC_IpcKey + 2);
		}
    }
    
    lKeyRet = qry.field("IPC_KEY").asLong();
    qry.close ();
    
    return lKeyRet;
}

long IpcKeyMgr::getIpcKeyEx(int _iBillFlowID, char const * _sIpcName)
{
    char *pUserName;
    char sHostName[200];
    char sSql[2048];
    int iNum;
    long lKeyRet;
    DEFINE_QUERY(qry);

    pUserName = getenv("LOGNAME");
    int iHostID = getHostInfo(sHostName);
    memset(sSql,0,sizeof(sSql));
    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_IpcKey + 1);
    }
    
    if(strcmp(_sIpcName,"SHM_AcctItemData")==0 || strcmp(_sIpcName,"SHM_AcctItemIndex_S")==0 || 
       strcmp(_sIpcName,"SEM_AcctItemData")==0 || strcmp(_sIpcName,"SEM_AcctItemIndex")==0)
	{
	    sprintf(sSql,"SELECT COUNT(*) FROM B_IPC_CFG "
	        " WHERE BILLFLOW_ID != -1 AND IPC_NAME in ('%s','%s','%s','%s') "
	        " AND host_id=%d"
	        " AND sys_username='%s' and BILLFLOW_ID=%d",
	        "SHM_AcctItemData","SHM_AcctItemIndex_S",
		"SEM_AcctItemData","SEM_AcctItemIndex", 
		iHostID, pUserName, _iBillFlowID);		

	    qry.close ();
	    qry.setSQL(sSql);
	    qry.open();
	    if(!qry.next())
	    {
	        Log::log (0, "B_IPC_CFG中没有配置：%s, USER_NAME: %s, HOST_ID:%d", 
	                _sIpcName, pUserName, iHostID);
	        THROW (MBC_IpcKey + 2);    	
	    }
	    iNum = qry.field(0).asInteger();
	    qry.close ();
	    if(iNum != 4 )
	    {
	    	if(iNum !=0 )
	    	{
		        Log::log (0, "B_IPC_CFG中总帐加载共享内存的相关流程ID配置不完整：%s,"
				" USER_NAME: %s, HOST_ID:%d", 
		                _sIpcName, pUserName, iHostID);
		        THROW (MBC_IpcKey + 2);
	    	}
	    	_iBillFlowID = -1;
	    }
    	memset(sSql,0,sizeof(sSql));
	}
    sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG "
        " WHERE BILLFLOW_ID=%d AND IPC_NAME='%s' "
        " AND host_id=%d"
        " AND sys_username='%s'",
        _iBillFlowID, _sIpcName, iHostID, pUserName);
    
    qry.close ();
    qry.setSQL (sSql); qry.open ();

    if (!qry.next ()) {
        Log::log (0, "B_IPC_CFG中没有配置：%s, USER_NAME: %s, HOST_ID:%d", 
                _sIpcName, pUserName, iHostID);
        THROW (MBC_IpcKey + 2);
    }
    
    lKeyRet = qry.field("IPC_KEY").asLong();
    qry.close ();
    
    return lKeyRet;
}

long IpcKeyMgr::getIpcKey (char const * _sIpcName)
{
    char *env;
    int  iFlowID = 0;
    long lRet = 0;
    
    if ((env = (char *)getenv ("BILLFLOW_ID")) != NULL)
        iFlowID = atoi(env);
    
    if (iFlowID) {
        try {
            lRet = getIpcKey (iFlowID, _sIpcName);
        } catch (...) {
            // do nothing;
        }
    }
    
    if (lRet <= 0)
        lRet = getIpcKey (-1, _sIpcName);
    
    return lRet;
}

// 成功: 返回当前主机的ID, 主机名称_sHostName
// 失败: 返回 -1
int IpcKeyMgr::getHostInfo (char *_sHostName)
{
    char sSql[2048];
    DEFINE_QUERY(qry);
    
    int iret = gethostname(_sHostName,100);
    
    if (iret) {
        return -1;
    }
    
    sprintf(sSql,"select nvl(max(host_id),-1) host_id "
        " from wf_hostinfo where host_name='%s'",_sHostName);
    qry.close ();
    qry.setSQL (sSql);
    qry.open ();
    qry.next ();
    
    iret = qry.field("host_id").asInteger();
    qry.close ();
    
    return iret;
}

int IpcKeyMgr::getHostInfo (char *_sHostName,TOCIQuery &qry)
{
    char sSql[2048];
    
    int iret = gethostname(_sHostName,100);
    
    if (iret) {
        return -1;
    }
    
    sprintf(sSql,"select nvl(max(host_id),-1) host_id "
        " from wf_hostinfo where host_name='%s'",_sHostName);
    qry.close ();
    qry.setSQL (sSql);
    qry.open ();
    qry.next ();
    
    iret = qry.field("host_id").asInteger();
    qry.close ();
    
    return iret;
}

/* 
*   添加系统中需要的当前主机,当前操作系统用户,指定流程,IPC名称对应的值
*   先根据条件判断该IPC是否已经存在,存在则直接返回
*   不存在则插入,IPC_KEY如果和表中冲突,会抛异常
*/
bool IpcKeyMgr::setIpcKey(int _iBillFlowID, char const * _sIpcName,char const * sRemark, int _iIpcKey)
{
    char *pUserName = 0;
    char sHostName[256] = {0};
    char sSql[2048] = {0};
    char sSql2[2048] = {0};
    long lIpcKey = 0;

    DEFINE_QUERY(qry);

    if(!_sIpcName)   //_iBillFlowID 与 _iIpcKey为整型,不可能为空,故不判断,参数如有问题会返回失败
    {
        Log::log (0, "设置的IPCNAME不可以为空");
        THROW (MBC_IpcKey + 3);
    }

    pUserName = getenv("LOGNAME");  //获取环境变量的LOGNAME
    int iHostID = getHostInfo(sHostName);  //通过HOSTNAME去数据库获取HOSTID
        
    if (!strlen(pUserName) || iHostID==-1) 
    {
        Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_IpcKey + 1);
    }

    sprintf(sSql, " SELECT IPC_KEY FROM B_IPC_CFG "
                " WHERE BILLFLOW_ID=%d AND IPC_NAME = '%s' "
                " AND HOST_ID = %d "
                " AND SYS_USERNAME = '%s' ",
                _iBillFlowID, _sIpcName, iHostID, pUserName);
    
    qry.close();
    qry.setSQL(sSql);
    qry.open();

    if (qry.next ())  //查询该条件的IPC_KEY是否已经存在,存在则直接返回
    {
        qry.close();
        return true;
    }

    lIpcKey = _iIpcKey;

    qry.close();
    
    //这里如果IPC_KEY和表中数据冲突,会插入失败,抛出异常
    sprintf(sSql2, " INSERT INTO B_IPC_CFG "
        " (BILLFLOW_ID, IPC_NAME, IPC_KEY, REMARK, HOST_ID, SYS_USERNAME) "
        " VALUES (%d, '%s', %d, '%s', %d, '%s') ",
        _iBillFlowID, _sIpcName, lIpcKey,sRemark, iHostID, pUserName);
    qry.setSQL(sSql2);
    qry.execute();
    qry.commit();
    qry.close();
    return true;

}

bool IpcKeyMgr::setIpcKey(char const * _sIpcName,char const * sRemark)
{
    char *pUserName = 0;
    char sHostName[256] = {0};
    char sSql[2048] = {0};
    char sSql1[2048] = {0};
    char sSql2[2048] = {0};
    long lIpcKey = 0;
    long lKeyBegin = 0,lKeyEnd = 0;

    DEFINE_QUERY(qry);

    if(!_sIpcName)   //_iBillFlowID 与 _iIpcKey为整型,不可能为空,故不判断,参数如有问题会返回失败
    {
        Log::log (0, "设置的IPCNAME不可以为空");
        THROW (MBC_IpcKey + 3);
    }

    pUserName = getenv("LOGNAME");  //获取环境变量的LOGNAME
    int iHostID = getHostInfo(sHostName);  //通过HOSTNAME去数据库获取HOSTID
        
    if (!strlen(pUserName) || iHostID==-1) 
    {
        Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_IpcKey + 1);
    }
    
    sprintf(sSql, "SELECT A.KEY_BEGIN,A.KEY_END " 
            " FROM B_IPCKEY_LIMIT A "
            " WHERE A.HOST_ID= %d "
            " AND A.SYS_USERNAME = '%s' "
            " AND A.ipckey_type = 1 ",
            iHostID,pUserName);
    qry.close();
    qry.setSQL(sSql);
    qry.open();
    if (!qry.next ()) {
        Log::log(0, "IpckeyType=1未在表b_ipckey_limit表中配置");
     THROW(MBC_IpcKeyMgr+1);
    } else {
      lKeyBegin = qry.field(0).asInteger();
      lKeyEnd = qry.field(1).asInteger();    	
    }
    qry.close();
    sprintf(sSql1, " SELECT IPC_KEY FROM B_IPC_CFG "
                " WHERE BILLFLOW_ID=-1 AND IPC_NAME = '%s' "
                " AND HOST_ID = %d "
                " AND SYS_USERNAME = '%s' ",
                _sIpcName, iHostID, pUserName);
    

    qry.setSQL(sSql1);
    qry.open();

    if (qry.next ())  //查询该条件的IPC_KEY是否已经存在,存在则直接返回
    {

        qry.close();
        return true;

    }

    lIpcKey = getNextIpcKey(iHostID,pUserName,_sIpcName,lKeyBegin,lKeyEnd);
    qry.close();
    
    //这里如果IPC_KEY和表中数据冲突,会插入失败,抛出异常
    sprintf(sSql2, " INSERT INTO B_IPC_CFG "
        " (BILLFLOW_ID, IPC_NAME, IPC_KEY, REMARK, HOST_ID, SYS_USERNAME) "
        " VALUES (-1, '%s', %d, '%s', %d, '%s') ",
        _sIpcName, lIpcKey,sRemark, iHostID, pUserName);
    qry.setSQL(sSql2);
    qry.execute();
    qry.commit();
    qry.close();
    return true;

}


long IpcKeyMgr::getNextIpcKey(int iHostID,char const * pUserName,char const * _sIpcName,long lKeyBegin,long lKeyEnd)
{
  char sSql[2048] = {0};
  long lIpcKey = 0;
  DEFINE_QUERY(qry);
    sprintf(sSql, " SELECT MAX(a.ipc_key) FROM b_ipc_cfg a "
            " WHERE a.sys_username = '%s' AND a.host_id = %d " );
   qry.close();
   qry.setSQL(sSql);
   qry.open();
   
   if (qry.next ())
   {
   	lIpcKey=qry.field(0).asInteger();
   }
   qry.close();
   while(seekIpcKey(iHostID,lIpcKey))
   {
   	lIpcKey++;
   }
   if(lIpcKey>lKeyEnd||lIpcKey<lKeyBegin)
   {
     Log::log(0, "IPC_NAME:%s值:%d已超出其在表B_IPCKEY_LIMIT表中的分配的范围", _sIpcName,lIpcKey);
     THROW(MBC_IpcKeyMgr+2);
   }
   return lIpcKey;
    
}

 bool IpcKeyMgr::seekIpcKey(int iHostID,long lIpcKey)
{
  char sSql[2048] = {0};
  DEFINE_QUERY(qry);
  sprintf(sSql," SELECT IPC_NAME "
    " FROM B_IPC_CFG A "
    " WHERE A.IPC_KEY=%d"
    " AND A.HOST_ID= %d",lIpcKey,iHostID);
   qry.close();
   qry.setSQL(sSql);
   qry.open();
   
   if (qry.next ())
  {
   	qry.close();
   	return true;

  }
   else
  {
   	qry.close();   
   	return false;
  }
}
