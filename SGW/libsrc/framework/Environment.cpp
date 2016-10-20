// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

// Modified by chenk 20051012 
// getDBConn中增加登陆选项配置，选项配置在当前目录的文件"DBConn.dat"中
// 文件格式：
// 用户名（默认bill）
// 密码（默认bill）
// 数据连接（默认tibs_bill）
#include "Environment.h"
#include <string.h>
#include "platform.h"
#include "platform.h" 
#include "Exception.h"
#include "BaseFunc.h"

//#include "BalanceHold100Sql.h"

#include "platform.h"

//changed by jiangmj 2008-6-5
//EventSection * g_poEventSection = 0;
//StdEventExt  * g_poStdEventExt = 0;

bool Environment::m_bConnect = false;

void Environment::setDBLogin(char *name, char *pwd, char *str)
{
	strncpy (m_sDBUser, name, sizeof(m_sDBUser));
	strncpy (m_sDBPwd, pwd, sizeof(m_sDBPwd));
	strncpy (m_sDBStr, str, sizeof(m_sDBStr));	

	m_bDefaultConn = false; 
}

void Environment::initSQLStatement()
{
	//ProfileSqlMgr::Initialize();
	//ACLMonitorSqlMgr::Initialize();
	//FindErrorSqlMgr::Initialize();
	//GetSysInfoSqlMgr::Initialize();
	//BalanceHold100SqlMgr::Initialize();
}

void Environment::Terminate()
{
	//ProfileSqlMgr::Terminate();
	//ACLMonitorSqlMgr::Terminate();
	//FindErrorSqlMgr::Terminate();
	//GetSysInfoSqlMgr::Terminate();
//	BalanceHold100SqlMgr::Terminate();
}

void Environment::getDefaultLogin()
{
    getConnectInfo( "sgwdb", m_sDBUser, m_sDBPwd, m_sDBStr);
}

void Environment::getConnectInfo(
    char * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr )
{
	ReadIni reader;
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	char sFile[254];
	char sTmp[32];

	char sTmpVal[32];
	memset(sUser, 0, sizeof(sUser));
	memset(sPwd, 0, sizeof(sPwd));
	memset(sStr, 0, sizeof(sStr));
	memset(sFile, 0, sizeof(sFile));
	
	char sIniTmp[32]={0};
	//modified by jx 20100930	
	char *p = 0;
	char sIniPath[128] = {0};
	if ((p=getenv ("APP_HOME")) == NULL)
		sprintf (sIniPath, "/opt/optsr/etc");
	else 
		sprintf (sIniPath, "%s/etc", p);
	if((p = getenv("SGW_CONFIG")) == NULL)
		sprintf(sFile,"%s/sgwconfig",sIniPath);
	else
		sprintf(sFile,"%s",p);
		
	strcpy(sIniTmp,sIniHeader);
   // Chang2Lower(sIniTmp);

	char sTemp[254];	
	memset(sTemp, 0, sizeof(sTemp));
	
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.username",sIniTmp);
	reader.readIniString (sFile, "SGWDB", sTmpVal, sUser, "");
	
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.password",sIniTmp);	
	reader.readIniString (sFile, "SGWDB", sTmpVal, sTemp,"");
	decode(sTemp,sPwd);
	
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.server_name",sIniTmp);
	reader.readIniString (sFile, "SGWDB", sTmpVal, sStr, "");	
	

	strcpy(sUserName,sUser);
	strcpy(sPasswd,sPwd);
	strcpy(sConnStr,sStr);

	return;
}
	



TOCIDatabase *Environment::getDBConn(bool bReconnect) //默认参数false
{
#ifdef __SQLITE__
	return &m_oDBConn;
#else
 
#endif
	if (bReconnect) 
	{
		if (m_bConnect)
		try 
		{
			m_oDBConn.disconnect ();
		}
		catch (...) {
		}	

		m_bConnect = false;
	}

	if (!m_bConnect) 
	{
		if (m_bDefaultConn) 
		{
			getDefaultLogin();
		}

		m_oDBConn.connect (m_sDBUser, m_sDBPwd, m_sDBStr);
		
	}

	m_bConnect = true;	
	return &m_oDBConn;
}


char Environment::m_sDBUser[24] = {0};

char Environment::m_sDBPwd[24] = {0};

char Environment::m_sDBStr[32] = {0};
	
TOCIDatabase Environment::m_oDBConn;







//EventSectionUnioner *Environment::m_poEventSectionUnioner = 0;


/*void Environment::useUserInfo()
{
	 if (!m_poUserInfoInterface) {
                m_poUserInfoInterface = new UserInfoInterface ();
                if (!m_poUserInfoInterface) THROW(MBC_Environment+1);
        }

        if (!m_poServ) {
                m_poServ = new Serv ();
                if (!m_poServ) THROW(MBC_Environment+1);
        }

	if (!m_poAcctItemMgr) {
		m_poAcctItemMgr = new AcctItemMgr ();
                if (!m_poAcctItemMgr) THROW(MBC_Environment+1);
	}

	if (!m_poOrgMgr) {
		m_poOrgMgr = new OrgMgr ();
		if (!m_poOrgMgr) THROW (MBC_Environment+1);
	}

	if (!m_poEventTypeMgr) {
		m_poEventTypeMgr = new EventTypeMgr ();
                if (!m_poEventTypeMgr) THROW(MBC_Environment+1);
	}

}



void Environment::useEventSectionUnion()
{
	if (!m_poEventSectionUnioner) {
                m_poEventSectionUnioner = new EventSectionUnioner ();
                if (!m_poEventSectionUnioner) THROW(MBC_Environment+1);
        }
}
*/

bool Environment::m_bDefaultConn = true;


void Environment::commit()
{
	if (m_bConnect) m_oDBConn.commit ();
}


void Environment::rollback()
{
	if (m_bConnect) m_oDBConn.rollback ();
}

void Environment::disconnect()
{
	if (m_bConnect) m_oDBConn.disconnect ();
	m_bConnect = false;
}

