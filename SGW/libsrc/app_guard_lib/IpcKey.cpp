/*VER: 1*/ 

#define _JUST_DBLINK_

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "platform.h"
#include <unistd.h>

//#include "WfPublic.h"
#include "IpcKey.h"
#include "Environment.h"
#include "Log.h"
#include "MBC.h"
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
        throw (MBC_IpcKey + 1);
        
    }
    
    sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG"
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
    
//    int iret = gethostname(_sHostName,100);
    
  //  if (iret) {
   //     return -1;
   // }
       int iret = 0;
    char *pp = NULL;
    if ((pp=getenv ("SGW_HOST_NAME")) == NULL)
    {
            cout<<"Please set env value SGW_HOST_NAME. For example  export SGW_HOST_NAME=sgw1"<<endl;
            THROW(-1);
    }
    sprintf (_sHostName, "%s", pp);

    
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


