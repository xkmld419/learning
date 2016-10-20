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
#include <iostream>
#include <fstream> 
#include "Exception.h"

//#include "BalanceHold100Sql.h"

using namespace std;


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
	//BalanceHold100SqlMgr::Terminate();
}

void Environment::getDefaultLogin()
{
	char sHome[254];
	char * p;

	if ((p=getenv ("ABM_HOME")) == NULL)
    	sprintf(sHome,"/home/abm/abmapp");
    else
        sprintf (sHome, "%s/deploy", p);

	ReadIni reader;
    char sFile[254];

    if (sHome[strlen(sHome)-1] == '/') 
    {
    	sprintf (sFile, "%sconfig/data_access.ini", sHome);
    }
    else 
    {
        sprintf (sFile, "%s/config/data_access.ini", sHome);
    }

    reader.readIniString (sFile, "ABMTTXLA", "username", m_sDBUser, "");
    
    char sTemp[254];		
	reader.readIniString (sFile, "ABMTTXLA", "password", sTemp,"");
	decode(sTemp,m_sDBPwd);
	
    reader.readIniString (sFile, "ABMTTXLA", "connstr", m_sDBStr, "");
	
}


TOCIDatabase *Environment::getDBConn(bool bReconnect) //默认参数false
{
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

bool Environment::m_bConnect = false;





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
}
