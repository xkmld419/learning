/* 服务非XA方式*/
#define TUXEDO  1

#include "./mcci.h"

#include <userlog.h>
#include <sqlca.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <exception>
#include <ctype.h>
#include "encode.h"
#include "mcci.h"
#include "cslpublic.h"
//#include "Environment.h"
//#include "Timesten_Env.h"

//class TOCIDatabase *database = NULL;
//class TTConnection *ttConn = NULL;

using namespace std;

/*
 保留字符串中的字符和下划线，其余按空格和TAB裁减
 */
void StrTrim(char sBuf[])
{
	long i, lFirstChar, lEndPos, lFirstPos;
	lEndPos = lFirstChar = lFirstPos = 0;

	for (i = 0; sBuf[i] != '\0'; i++)
	{
		if ((sBuf[i] == ' ') || (sBuf[i] == '\t'))
		{
			if (lFirstChar == 0)
				lFirstPos++;
		}
		else if ((sBuf[i] >= 'a') && (sBuf[i] <= 'z') || ((sBuf[i] >= 'A') && (sBuf[i] <= 'Z')) || (isdigit(sBuf[i])) | (sBuf[i] == '_'))
		{
			lEndPos = i;
			lFirstChar = 1;
		}

	}

	for (i = lFirstPos; i <= lEndPos; i++)
		sBuf[i - lFirstPos] = sBuf[i];

	sBuf[i - lFirstPos] = '\0';
}

/*
 根据服务名称从"NONXA_SERVERS.ini"文件中获取连接数据库的连接串，二次登陆数据库用户名
 要求文件格式如：服务名 =连接串/数据库用户名
 sServerName	为服务名称，有大小写区分
 sConnectStr	为数据库连接串，无大小写区分
 sSecondUser	为数据库二次登陆用户名，无大小写区分
 失败返回-1，成功返回0
 */
long GetConnectInfo(char *sServerName, char *sConnectStr, char *sSecondUser, char *sSecondPasswd)
{
	FILE *fp;
	char sLine[256];
	char *sBegin = NULL;
	char *sEnd = NULL;
	char *sConEnd = NULL;
	long lRet = 0;

	char sEncodePassword[256]; //Add By Linzhong . 2006.07.19

	strcpy(sConnectStr, "");
	strcpy(sSecondUser, "");

	strcpy(sEncodePassword, ""); //Add By Linzhong . 2006.07.19

	fp = fopen("NONXA_SERVERS.ini", "rt");
	if (fp == NULL)
	{
		userlog("NONXA_SERVERS.ini open error\n");
		lRet = -1;
		return lRet;
	}
	else
		while (!feof(fp))
		{
			memset(sLine, 0, 256);
			fgets(sLine, 256, fp);
			StrTrim(sLine);

			sBegin = strstr(sLine, sServerName);

			if (sBegin != NULL)
			{
				sEnd = sBegin + strlen(sServerName);
				sBegin = strchr(sEnd, '=');
				if (sBegin != NULL)
				{
					if (sBegin < sLine + 255)
						sBegin = sBegin + 1;
					else
						continue;

					if (sBegin != NULL)
					{
						sEnd = strchr(sBegin, '/');

						if (sEnd != NULL)
						{
							*sEnd = '\0';
							if (sEnd < sLine + 255)
							{
								sEnd = sEnd + 1;
								sConEnd = strchr(sEnd, '/');
								if (sConEnd != NULL)
								{
									*sConEnd = '\0';
									if (sConEnd < sLine + 255)
									{
										sConEnd = sConEnd + 1;
										if (sConEnd != NULL)
										{//连接串
											strncpy(sConnectStr, sConEnd, 20);
											sConnectStr[19] = '\0';
											StrTrim(sConnectStr);
										}
									}
								}
								if (sEnd != NULL)
								{//连接密码
									/* Modify By Linzhong . 2006.07.19
									 strncpy(sSecondPasswd, sEnd, 50);
									 sSecondPasswd[19] = '\0';
									 StrTrim(sSecondPasswd);
									 */

									/*Modity By Lijb . 2007.05.12
									 strncpy(sEncodePassword, sEnd, 50);
									 sEncodePassword[49] = '\0';
									 StrTrim(sEncodePassword);
									 */

									decode(sEnd, sSecondPasswd);
									StrTrim(sSecondPasswd);
								}
							}
						}
						//连接用户
						strncpy(sSecondUser, sBegin, 20);
						sSecondUser[19] = '\0';
						StrTrim(sSecondUser);
					}
				}
				else //Modify By Linzhong . 2006.05.17 for service_name = 3, 1=acct/acct@acct_136
				{
					continue;
				}

				break;
			}
		}

	if (strcmp(sConnectStr, "") == 0)
	{
		userlog("Error in get ConnectString!\n");
		lRet = -1;
	}

	if (strcmp(sSecondUser, "") == 0)
	{
		userlog("Error in get SecondUser\n");
		lRet = -1;
	}
	if (fp)
	{
		fclose(fp);
	}
	return lRet;
}

long ConnectDatabase(char *sServerName)
{

	char sUserName[20], sPassWord[50], sConnStr[20];

	//Ocs_Environment::getConnectInfo(sServerName, sUserName, sPassWord, sConnStr);
	userlog("\n ServerName=%s UserName=%s PassWord=%s ConnStr=%s \n", sServerName, sUserName, sPassWord, sConnStr);

	try
	{
		//Ocs_Environment::setDBLogin(sUserName, sPassWord, sConnStr);
		//database = Ocs_Environment::getDBConn();
    ;
	}
	/*
	catch(TOCIException &oe)
	{
		throw oe;
	}
	catch(TException &ex)
	{
		throw ex;
	}*/
	catch(...)
	{
		throw ;
	}

	return 0;
}

long ConnectTimesTen(char *sServerName)
{

	char sUserName[20], sPassWord[50], sConnStr[200];
	string sConnectStr;

	//Ocs_Environment::getConnectInfo(sServerName, sUserName, sPassWord, sConnStr);

	try
	{
		sConnectStr = "dsn=";
		sConnectStr += sConnStr;
		sConnectStr += ";uid=";
		sConnectStr += sUserName;
		sConnectStr += ";pwd=";
		sConnectStr += sPassWord;
		userlog("\n ConnectStr=%s", sConnectStr.c_str());

		//TimesTenEnv::setDBLogin(sConnectStr);
		//ttConn = TimesTenEnv::getDBConn();
	}/*
	catch(TTStatus st)
	{
		throw st;
	}*/
	catch(...)
	{
		throw ;
	}

	return 0;
}
void writeErrMsg(int line, char *filename, int iOraCode)
{
	userlog("\nORACLE SQL_ERROR in '%s' line %d, ora %d \n ", filename, line, iOraCode);
	return;
}

/*服务非XA方式*/
/*服务的控制返回*/
//edited by lingy on 2009-04-30, 去掉 #ifdef	NONXA
long cslreturn(int rval, long rcode, char *data, long len, int iDBControl)
{
	long lRet = 0;

	//#ifdef	NONXA
	/*如果是非XA方式服务*/
	if (iDBControl & COMMIT)
	{
		if (iDBControl & ORA_DB && iDBControl & MM_DB)
		{
			//if (ttConn)
				//TimesTenEnv::commit();
			//if (database)
				//Ocs_Environment::commit();
		}
		else if (iDBControl & MM_DB)
		{
			//if (ttConn)
				//TimesTenEnv::commit();
		}
		else
		{
			//if (database)
				//Ocs_Environment::commit();
		}

	}
	else if (iDBControl & ROLLBACK)
	{
		if (iDBControl & ORA_DB && iDBControl & MM_DB)
		{
			//if (ttConn)
				//TimesTenEnv::rollback();
			//if (database)
				//Ocs_Environment::rollback();
		}
		else if (iDBControl & MM_DB)
		{
			//if (ttConn)
				//TimesTenEnv::rollback();
		}
		else
		{
			//if (database)
				//Ocs_Environment::rollback();
		}
	}
	else
	{
		userlog("cslreturn() Error rval in wwreturn() function!\n");
	}
	//#endif/*NONXA*/

	cslreturn_common(rval, rcode, data, len);

	return 0;
}

/*服务资源连接*/
//edited by lingy on 2009-04-30 去掉#ifndef	NONXA
long cslopen(char *sServerName, int iDBControl)
{
	long lRet = 0;

	/*
	 #ifndef	NONXA
	 //XA方式服务
	if (cslopen_common() == -1)
		return -1;
	*/
	//#else
	try
	{
		if (iDBControl & ORA_DB)
		;//ConnectDatabase(sServerName);
		if (iDBControl & MM_DB)
		;//ConnectTimesTen(sServerName);
	}
	catch(...)
	{
		return -1;
	}
	/*
	catch(TOCIException &bex)
	{
		userlog("Connect Database Failed! Error Code = %d, ErrMsg = %s", bex.getErrCode(), bex.getErrMsg());
		return -1;
	}
	catch(TException &ex)
	{
		userlog("Connect Database Failed!");
		return -1;
	}
	catch (TTStatus st)
	{
		userlog("Connect Memory Database Failed!");
		return -1;
	}*/
	//#endif/*NONXA*/

	return 0;
}

/*服务资源断开*/
//edited by lingy on 2009-04-30 去掉#ifndef	NONXA
long cslclose(char *sServerName)
{
	long lRet = 0;

	/*
	 #ifndef NONXA
	 //XA方式服务
	if (cslclose_common() == -1)
		return -1;
	*/
	//#else
	/*非XA方式服务*/
	//if (database || ttConn)
	{
		try
		{
			//if (database)
			//Ocs_Environment::disconnect();
			//if (ttConn)
			//TimesTenEnv::disconnect();
		}
		catch(...)
		{
			throw;
		}/*
		catch(TOCIException &oraex)
		{
			throw oraex;
		}
		catch(TException &ex)
		{
			throw ex;
		}
		catch (TTStatus st)
		{
			throw st;
		}
		catch(...)
		{
			throw;
		}*/
	}
	//#endif/*NONXA*/

	return 0;
}

void sql_error(int line, char *filename)
{
	userlog("\nORACLE SQL_ERROR in '%s' line %d \n ", filename, line);
	return;
}

