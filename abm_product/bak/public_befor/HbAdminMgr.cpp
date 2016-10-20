#include <stdio.h> 
#include <iostream>
#include "ReadIni.h"
#include <stdlib.h>
#include <string>
#include "encode.h"
#include "Log.h"
#include "HbAdminMgr.h"
#include "TOCIQuery.h"
#include "Environment.h"

static char * sSql = "SELECT 1 FROM  B_PARAM_DEFINE WHERE  PARAM_SEGMENT = 'ADMIN' AND \
  				  PARAM_CODE = 'INITTAG' AND PARAM_VALUE = 1";
  				 
static char * sSql2 = "SELECT 1 FROM  B_PARAM_DEFINE WHERE  PARAM_SEGMENT = 'ADMIN' AND \
  				  PARAM_CODE = 'LOGTAG' AND PARAM_VALUE = 2"	;
  				    				  
char HbAdminMgr::m_sLogUserName[20];
char HbAdminMgr::m_sLogPassWord[20];
char HbAdminMgr::m_sUserName[20];
char HbAdminMgr::m_sPassWord[20];
char HbAdminMgr::m_sTIBS_HOME[20];

HbAdminMgr::HbAdminMgr()
{		
	memset(m_sLogUserName,0,sizeof(m_sLogUserName));
	memset(m_sLogPassWord,0,sizeof(m_sLogPassWord));	
	memset(m_sUserName,0,sizeof(m_sUserName));	
	memset(m_sPassWord,0,sizeof(m_sPassWord));
	memset(m_sTIBS_HOME,0,sizeof(m_sTIBS_HOME));

}

HbAdminMgr::~HbAdminMgr()
{
}
HbAdminMgr::getLogInfo(int argc, char **argv)
{		

	loginInit(1);
	if (argc != 2 || argv[0] == NULL || argv[1] == NULL) 
	{		
		Log::log (0, "usg: %s username/password", argv[0]);	
		return -1;
	}	
	CutString(argv[1]);	
	
	return 1;		

}
int HbAdminMgr::getConfInfo(const char * sIniHeader)
{
	char sFile[254];
	char sUser[32];
	char sPwd[32];
	ReadIni reader;	

	authenticateInit(1); 
	
	sprintf (m_sTIBS_HOME, "/cbilling/app/TIBS_HOME_2.8/");
	if (m_sTIBS_HOME[strlen(m_sTIBS_HOME)-1] == '/')
	{
		sprintf (sFile, "%s/etc/admin.ini",m_sTIBS_HOME);
	}	
	else
	{	
		sprintf (sFile, "%s/etc/admin.ini", m_sTIBS_HOME);
	}
	
	char sTemp[254];
	memset(sTemp, 0, sizeof(sTemp));	
	reader.readIniString (sFile, sIniHeader, "username", sUser, "");	
	reader.readIniString (sFile, sIniHeader, "password", sTemp,"");
	decode(sTemp,sPwd);
	
	char* pwd = new char[20];	
	tickOutTab(pwd, sPwd);
	
	strcpy(m_sUserName, sUser);
	strcpy(m_sPassWord, pwd);	
	
	return  1 ;
}     
/*
 * 函 数 名 :getAuthenticate
 * 函数功能 : 判断用户是否登录成功
 * 时    间 : 2011年3月11日
 * 返 回 值 : 成功返回1,失败返回0,未验证返回-1;
 * 参数说明 : 无
 */
 
 int HbAdminMgr::doJude()
 {
 	if (!strcmp(m_sLogUserName,m_sUserName))
	{										 							
		if (!strcmp(m_sLogPassWord,m_sPassWord))	
		{
			authenticateInit(2); 
			return  1 ;
		}	
	}
	authenticateInit(4);
	
	return 0;
 		
 }
int  HbAdminMgr::getAuthenticate()
{		
	DEFINE_QUERY (qry) ;
	char *penv = NULL;
	
	if ((penv = getenv ("HbAdmin")) == NULL) return -1;
		 
  	qry.close(); 
	qry.setSQL (sSql);	 
  	qry.open();	 

  	if (!qry.next())
  	{
  		qry.close();
  		return -1;
	}	
  	qry.close();
  	qry.setSQL (sSql2);	
  	qry.open();
		  
  	if (qry.next()) {
  		qry.close();
  		return 1 ;  
  	}
  	qry.close();	
 	
	return 0;
}   

int HbAdminMgr::CutString(const char * sInputInfo)
{
	char tempUserName[20] ={0};
	char tempPassSWord[20]= {0};
	int  i = 0 ,j = 0 ;	
		
	if (sInputInfo == NULL) return 0;

	while (*sInputInfo != '\0')
	{
		if (*sInputInfo == '/')	break;		
		tempUserName[i++] = *sInputInfo++;
	}	 	
	while (*sInputInfo++ != '\0')	
	{	
		tempPassSWord[j++] = *sInputInfo;
	}	
	
	strcpy(m_sLogUserName,tempUserName);
	strcpy(m_sLogPassWord, tempPassSWord);	
			
	return 1;
}

int  HbAdminMgr::authenticateInit(const int sInputInfo)
{
	DEFINE_QUERY(qry);
	char sSql[400] = {0};
	
	sprintf(sSql, "UPDATE B_PARAM_DEFINE SET PARAM_VALUE = %d WHERE PARAM_SEGMENT = 'ADMIN' AND\
  				  PARAM_CODE = 'LOGTAG' ", sInputInfo);
  	
  	qry.close(); qry.setSQL (sSql);qry.execute(); qry.commit();	qry.close();
  	
  	return 1; 
}

int HbAdminMgr::loginInit(const int ihasLog)
{	
	DEFINE_QUERY(qry);
	char sSql[400] = {0};
	
	sprintf(sSql, "UPDATE B_PARAM_DEFINE SET PARAM_VALUE = %d WHERE PARAM_SEGMENT = 'ADMIN' AND\
  				  PARAM_CODE = 'INITTAG' ", ihasLog);
  	
  	qry.close(); qry.setSQL (sSql);qry.execute(); qry.commit();	qry.close();
  		
  	return 1; 
}

int  HbAdminMgr::tickOutTab (char* ptempWd, const char* sPwd)
{	
	if (ptempWd == NULL || ptempWd == NULL)
	{
		 return 0 ;
	}
	while (*sPwd != '\0')
	{		
		if (*sPwd != ' ')
		{
			*ptempWd = *sPwd;		
		}
		ptempWd++;
		sPwd++;
	}
	
	return 1;	
}

