/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "ParamDefineMgr.h"
#include "Environment.h"
#include "IpcKey.h"
#include "Log.h"

bool ParamDefineMgr::getParam(char const *_sSegment, char const *_sCode, 
char *_sValue, int _iMaxLen )
{
	char sSql[1000];
	memset(sSql,0,1000);
	
	if (!_sValue)
		return false;

#ifdef PRIVATE_PARAM_DEFINE

    char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "找不到host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_ParamDefineMgr + 1);
    }

	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' "
        " and host_id=%d "
        " and sys_username='%s' "  ,
		_sSegment, _sCode, iHostID, pUserName);

#else
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		_sSegment, _sCode);
	
#endif

	DEFINE_QUERY(qry);
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	if (!qry.next())
		return false;
	
	if(_iMaxLen<0)
    {   
         strcpy(_sValue, qry.field("param_value").asString());
    }
    else
    {
         strncpy(_sValue, qry.field("param_value").asString(), _iMaxLen);            
    }
	
	return true;
}

bool ParamDefineMgr::getParam(char const *_sSegment, char const *_sCode, 
char *_sValue, TOCIQuery &qry,int _iMaxLen )
{
	char sSql[1000];
	memset(sSql,0,1000);
	
	if (!_sValue)
		return false;

#ifdef PRIVATE_PARAM_DEFINE

    char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName,qry);

    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "找不到host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_ParamDefineMgr + 1);
    }

	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' "
        " and host_id=%d "
        " and sys_username='%s' "  ,
		_sSegment, _sCode, iHostID, pUserName);

#else
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		_sSegment, _sCode);
	
#endif

	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	if (!qry.next()){
		qry.close();
		return false;
	}
	
	if(_iMaxLen<0)
    {   
         strcpy(_sValue, qry.field("param_value").asString());
    }
    else
    {
         strncpy(_sValue, qry.field("param_value").asString(), _iMaxLen);            
    }

	qry.close();
	
	return true;
}

long ParamDefineMgr::getLongParam(int iBillFlowID,char const *sSegment, char const *sCode)
{
	//把流程ID与sCode合并后（格式：sCode_iBillFlowID)调用getLongParam(char const *sSegment, char const *sCode)
	char sParamCode[128];
	memset(sParamCode,0,sizeof(sParamCode));

	sprintf(sParamCode,"%s_%d",sCode,iBillFlowID);
	
	char sSql[1000];
	memset(sSql,0,1000);
    char sTemp[32];

#ifdef PRIVATE_PARAM_DEFINE

    char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        THROW (MBC_ParamDefineMgr + 1);
    }

    sprintf(sSql,
        " select trim(param_value) param_value  from b_param_define "
        " where param_segment = '%s' and param_code = '%s' "
        " and host_id=%d "
        " and sys_username='%s' "  ,
        sSegment, sParamCode, iHostID, pUserName);

#else
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		sSegment, sParamCode
	);

#endif
	
	DEFINE_QUERY(qry); qry.close();
	qry.setSQL(sSql); qry.open();	
	if (!qry.next()) {
		return getLongParam(sSegment,sCode);
    }
	strcpy(sTemp, qry.field("param_value").asString());    
	return strtol (sTemp, 0, 10);
}

long ParamDefineMgr::getLongParam(char const *sSegment, char const *sCode)
{
	char sSql[1000];
	memset(sSql,0,1000);

    char sTemp[32];

#ifdef PRIVATE_PARAM_DEFINE

    char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        THROW (MBC_ParamDefineMgr + 1);
    }

    sprintf(sSql,
        " select trim(param_value) param_value  from b_param_define "
        " where param_segment = '%s' and param_code = '%s' "
        " and host_id=%d "
        " and sys_username='%s' "  ,
        sSegment, sCode, iHostID, pUserName);

#else
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		sSegment, sCode
	);

#endif
	
	DEFINE_QUERY(qry); qry.close();
	qry.setSQL(sSql); qry.open();
	
	if (!qry.next()) {
        Log::log (0, "找不到参数配置: SEGMENT: %s CODE: %s",
                        sSegment, sCode);
        THROW (MBC_ParamDefineMgr + 2);
    }
	
    
	strcpy(sTemp, qry.field("param_value").asString());

    
	return strtol (sTemp, 0, 10);
}
//通过PARAM_SEGMENT和PARAM_CODE删除对应的配置
bool ParamDefineMgr::delParam(char const *_sSegment, char const *_sCode)
{
    char sSql[2048];
    memset(sSql, 0, 2048);
    if(!_sSegment || !_sCode)
    {
        Log::log(0, "删除参数配置失败,需要删除参数配置的PARAM_SEGMENT或PARAM_CODE不可以为空");
        THROW (MBC_ParamDefineMgr + 3);
    }

    char *pUserName;
    char sHostName[200] = {0};
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "找不到host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_ParamDefineMgr + 1);
    }

    sprintf(sSql, " DELETE FROM B_PARAM_DEFINE "
        " WHERE PARAM_SEGMENT = '%s' AND PARAM_CODE = '%s' "
        " AND HOST_ID = %d AND SYS_USERNAME = '%s' ",
        _sSegment, _sCode, iHostID, pUserName);

    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL(sSql);
    qry.execute();
    qry.commit();
    qry.close();

    return true;
    
}

//配置参数,使用前需要确定该配置参数在表中不存在
bool ParamDefineMgr::setParam(char const *_sSegment, char const *_sCode, 
char const *_sValue)
{
    char sSql[2048];
    memset(sSql, 0, 2048);

    if(!_sSegment || !_sCode || !_sValue)
    {
        Log::log(0, "配置参数字段信息不全, 设置参数失败");
        THROW (MBC_ParamDefineMgr + 4);
    }

    char *pUserName;
    char sHostName[200] = {0};
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "找不到host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_ParamDefineMgr + 1);
    }

    sprintf(sSql, " INSERT INTO B_PARAM_DEFINE "
        " (PARAM_SEGMENT, PARAM_CODE, PARAM_VALUE, PARAM_NAME, REMARK, HOST_ID, SYS_USERNAME) "
        " VALUES ('%s', '%s', '%s', '', '', %d, '%s') ",
        _sSegment, _sCode, _sValue, iHostID, pUserName);

    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL(sSql);
    qry.execute();
    qry.commit();
    qry.close();

    return true;
}

//配置参数,使用前需要确定该配置参数在表中不存在
bool ParamDefineMgr::setParam(char const *_sSegment, char const *_sCode,
char const *_sValue, char const *_sName)
{
    char sSql[2048];
    memset(sSql, 0, 2048);

    if(!_sSegment || !_sCode || !_sName || !_sValue)
    {
        Log::log(0, "配置参数字段信息不全, 设置参数失败");
        THROW (MBC_ParamDefineMgr + 4);
    }

    char *pUserName;
    char sHostName[200] = {0};
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

    if (!strlen(pUserName) || iHostID==-1) {
        Log::log (0, "找不到host_id, HOST_NAME:%s", sHostName);
        THROW (MBC_ParamDefineMgr + 1);
    }

    sprintf(sSql, " INSERT INTO B_PARAM_DEFINE "
        " (PARAM_SEGMENT, PARAM_CODE, PARAM_VALUE, PARAM_NAME, REMARK, HOST_ID, SYS_USERNAME) "
        " VALUES ('%s', '%s', '%s', '%s', '', %d, '%s') ",
        _sSegment, _sCode, _sValue, _sName, iHostID, pUserName);

    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL(sSql);
    qry.execute();
    qry.commit();
    qry.close();

    return true;
}
