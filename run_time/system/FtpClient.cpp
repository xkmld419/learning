// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "FtpClient.h"

//##ModelId=41DCEA510148
FtpClient::FtpClient ()
{
	m_sIP[0] = 0;
	m_sUser[0] = 0;
	m_sPassword[0] = 0;
	m_sCommand[0] = 0;
}

//##ModelId=41DCEA51015C
FtpClient::FtpClient (char *ip)
{
	m_sUser[0] = 0;
	m_sPassword[0] = 0;
	m_sCommand[0] = 0;

	strcpy (m_sIP, ip);
}

//##ModelId=41DCEA5101A2
FtpClient::FtpClient (char *ip, char *user, char *password)
{
	m_sCommand[0] = 0;

	strcpy (m_sIP, ip);
	strcpy (m_sUser, user);
	strcpy (m_sPassword, password);
}

//##ModelId=41DCEA510242
FtpClient::~FtpClient ()
{

}

//##ModelId=41DCEA510256
void FtpClient::setRemoteIP (char *ip)
{
	strcpy (m_sIP, ip);
}

//##ModelId=41DCEA51029C
void FtpClient::setLogin (char *user, char *password)
{
	strcpy (m_sUser, user);
	strcpy (m_sPassword, password);
}

//##ModelId=41DCE9A800D7
void FtpClient::clearCommand ()
{
	memset (m_sCommand, 0, sizeof(m_sCommand));
}

//##ModelId=41DCE9A101BD
void FtpClient::addCommand (char *command)
{
	strcat (m_sCommand, command);
	strcat (m_sCommand, "\n");
}

//##ModelId=41DCE9B10288
void FtpClient::execCommand ()
{
	char sTemp[2048];

	sprintf (sTemp, "ftp -n %s <<!EOF\nuser %s %s\n%squit\n", m_sIP, m_sUser, m_sPassword, m_sCommand);
	
	system (sTemp);
}

