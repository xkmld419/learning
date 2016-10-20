/*VER: 1*/ 
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
//#include "TransactionMgr.h"
//#include "ProdOfferAggr.h"
#include <string>
#include <algorithm>
#include <functional>
using namespace std;
//##ModelId=424BBCEA01A8
void Environment::setDBLogin(char *name, char *pwd, char *str)
{
	strncpy (m_sDBUser, name, sizeof(m_sDBUser));
	strncpy (m_sDBPwd, pwd, sizeof(m_sDBPwd));
	strncpy (m_sDBStr, str, sizeof(m_sDBStr));	

	m_bDefaultConn = false;
}

void Environment::getDefaultLogin()
{
	/*
	char sHome[254];
	char * p;

	if ((p=getenv ("TIBS_HOME")) == NULL)
                sprintf (sHome, "/home/bill/TIBS_HOME");
        else
                sprintf (sHome, "%s", p);

	ReadIni reader;
        char sFile[254];

    
    if (sHome[strlen(sHome)-1] == '/') {
            //sprintf (sFile, "%setc/data_access.ini", sHome);
            sprintf (sFile, "%setc/data_access_new.ini", sHome);
    } else {
            //sprintf (sFile, "%s/etc/data_access.ini", sHome);
            sprintf (sFile, "%s/etc/data_access_new.ini", sHome);
    }

    reader.readIniString (sFile, "BILL", "username", m_sDBUser, "");
    //reader.readIniString (sFile, "BILL", "password", m_sDBPwd, "");
    
    char sTemp[254];		
	reader.readIniString (sFile, "BILL", "password", sTemp,"");
	decode(sTemp,m_sDBPwd);
	
    reader.readIniString (sFile, "BILL", "connstr", m_sDBStr, "");
	*/
  //Modified by jx 20100729
  getConnectInfo( "BILL",m_sDBUser,m_sDBPwd,m_sDBStr);
		
}

//##ModelId=424BBCEA01C6
TOCIDatabase *Environment::getDBConn(bool bReconnect)
{
	if (bReconnect) {
		if (m_bConnect)
		try {
			m_oDBConn.disconnect ();
		} catch (...) {
		}	

		m_bConnect = false;
	}

	if (!m_bConnect) {
		if (m_bDefaultConn) {
			getDefaultLogin();
		}

		m_oDBConn.connect (m_sDBUser, m_sDBPwd, m_sDBStr);
	}

	m_bConnect = true;	
	return &m_oDBConn;
}



char Environment::m_sDBUser[24] = {0};

//##ModelId=424BBCEA0176
char Environment::m_sDBPwd[24] = {0};

//##ModelId=424BBCEA0180
char Environment::m_sDBStr[32] = {0};

//##ModelId=424BBCEA0194
TOCIDatabase Environment::m_oDBConn;

//##ModelId=424BBCEA019E
bool Environment::m_bConnect = false;


//##ModelId=427DC28A006F
bool Environment::m_bDefaultConn = true;

//##ModelId=42957FEE0390
void Environment::commit()
{
	if (m_bConnect) m_oDBConn.commit ();
}

//##ModelId=42957FF802D6
void Environment::rollback()
{
	if (m_bConnect) m_oDBConn.rollback ();
}

void Environment::disconnect()
{
	if (m_bConnect) m_oDBConn.disconnect ();
}

void Environment::connectDB(TOCIDatabase * pDbConn, char const * sIniHeader)
{
    char sUser[32];
	char sPwd[32];
	char sStr[32];
	getConnectInfo(sIniHeader, sUser, sPwd, sStr);
	pDbConn->connect(sUser, sPwd, sStr);
}


void Environment::getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr)
{
	//ReadIni reader;
	//char sUser[32];
	//char sPwd[32];
	//char sStr[32];
	//char sFile[254];		
	//char sTIBS_HOME[80];
	//char * p;
	///* TIBS_HOME */
	//if ((p=getenv ("TIBS_HOME")) == NULL)
	//	sprintf (sTIBS_HOME, "/home/bill/TIBS_HOME");
	//else 
	//	sprintf (sTIBS_HOME, "%s", p);
	//
	//if (sTIBS_HOME[strlen(sTIBS_HOME)-1] == '/') {
  //  	//sprintf (sFile, "%setc/data_access.ini", m_sTIBS_HOME);
	//	sprintf (sFile, "%setc/data_access_new.ini", sTIBS_HOME);
	//} else {
	//	//sprintf (sFile, "%s/etc/data_access.ini", m_sTIBS_HOME);
	//	sprintf (sFile, "%s/etc/data_access_new.ini", sTIBS_HOME);
	//}
  //
	//reader.readIniString (sFile, sIniHeader, "username", sUser, "");
////	reader.readIniString (sFile, sIniHeader, "password", sPwd, "");
	//
	//char sTemp[254];		
	//reader.readIniString (sFile, sIniHeader, "password", sTemp,"");
	//decode(sTemp,sPwd);
	//
	//reader.readIniString (sFile, sIniHeader, "connstr", sStr, "");
	//
	//strcpy(sUserName,sUser);
	//strcpy(sPasswd,sPwd);
	//strcpy(sConnStr,sStr);
	
	
	//return;	
	
	//modified by jx 20100729
	ReadIni reader;
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	char sFile[254];
	char sTmpVal[32];
	char sTIBS_HOME[80];
	char *p = 0;
	
	char sIniTmp[32]={0};
	
	memset(sUser, 0, sizeof(sUser));
	memset(sPwd, 0, sizeof(sPwd));
	memset(sStr, 0, sizeof(sStr));
	memset(sFile, 0, sizeof(sFile));

	if ((p=getenv ("ABM_PRO_DIR")) == NULL)
		sprintf (sTIBS_HOME, "/opt/opthss");
	else 
		sprintf (sTIBS_HOME, "%s", p);
	
	if (sTIBS_HOME[strlen(sTIBS_HOME)-1] == '/') {
		sprintf (sFile, "%setc/hssconfig", sTIBS_HOME);
	} else {
		sprintf (sFile, "%s/etc/hssconfig", sTIBS_HOME);
	}
	strcpy(sIniTmp,sIniHeader);
	string strIniTmp = sIniTmp;
	transform(strIniTmp.begin(),strIniTmp.end(),strIniTmp.begin(),::tolower);		
		
		char sTemp[254];	
	memset(sTemp, 0, sizeof(sTemp));
	
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.username",strIniTmp.c_str());
	reader.readIniString (sFile, "ABMDB", sTmpVal, sUser, "");
	
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.password",strIniTmp.c_str());	
	reader.readIniString (sFile, "ABMDB", sTmpVal, sTemp,"");
	decode(sTemp,sPwd);
	string strPwd=sPwd;
	strPwd.erase(strPwd.find_last_not_of(' ')+1);
	strcpy(sPwd,strPwd.c_str());
	memset(sTmpVal, 0, sizeof(sTmpVal));
	sprintf (sTmpVal,"%s.db_server_name",strIniTmp.c_str());
	reader.readIniString (sFile, "ABMDB", sTmpVal, sStr, "");	
	
	strcpy(sUserName,sUser);
	strcpy(sPasswd,sPwd);
	strcpy(sConnStr,sStr);
	return;
}
