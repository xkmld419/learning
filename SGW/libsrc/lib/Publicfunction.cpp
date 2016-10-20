#include <unistd.h>
#include <ctype.h>
#include "ReadIni.h"
#include "encode.h"
#include "Publicfunction.h"

int getConnectDBinfo(char * aimstr, char *username, char * password, char * connstr, char * cachepwd)
{
	ReadIni reader;
	char sAim[32];
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	char sOraPwd[32];
	char sFile[254];
	char * p;
	char sTmp[32];

	char m_sTIBS_HOME[101] = {0};

	if ((p=getenv ("APP_HOME")) == NULL)
	{
		printf("Please set env value APP_HOME!!!");
		return -1;
	}
	else
		sprintf (m_sTIBS_HOME, "%s", p);

	if (m_sTIBS_HOME[strlen(m_sTIBS_HOME)-1] == '/')
		sprintf (sFile, "%setc/data_access.ini", m_sTIBS_HOME);
	else
		sprintf (sFile, "%s/etc/data_access.ini", m_sTIBS_HOME);

	if (aimstr == NULL)
		return -1;
	memset(sAim, 0, sizeof(sAim));
	sprintf(sAim, aimstr);

	reader.readIniString (sFile, sAim, "username", sUser, "");
	reader.readIniString (sFile, sAim, "password", sTmp, "");
	decode(sTmp, sPwd);
	reader.readIniString (sFile, sAim, "connstr", sStr, "");
	if (cachepwd != NULL)
	{
		reader.readIniString (sFile, sAim, "oraclepwd", sTmp, "");
		decode(sTmp, sOraPwd);
	}

	sprintf(username, sUser);
	sprintf(password, sPwd);
	sprintf(connstr, sStr);
	if (cachepwd != NULL)
		sprintf(cachepwd, sOraPwd);

	return 1;
}

/*
*	Function Name:	Trim
*	Description:	去掉字符串左右的空格
*	Input Param:
*		String -------> 需要去掉空格的字符串
*	Returns:	无
*	complete:	2002/03/13
*/
//void Trim(char * String)
//{
//	char *Position = String;
//	/*找到第一个不是空格的位置*/
//	while(isspace(*Position))
//	{
//		Position++;
//	}
//	/*如果为一空串则退出*/
//	if (*Position == '\0')
//	{
//		*String = '\0';
//		return;
//	}
//	/*除去前面的空格*/
//	while(*Position)
//	{
//		*String = *Position;
//		String++;
//		Position++;
//	}
//	/*除去后面的空格*/
//	do
//	{
//		*String = '\0';
//		String--;
//	}while(isspace(*String));
//}

/*
*	Function Name:	MakeUpper
*	Description:	将字符串全部改为大写
*	Input Param:
*		pchString -------> 输入字符串
*	Returns:	无
*	complete:	2002/03/13
*/
//void MakeUpper(char * pchString)
//{
//	int iLen = strlen(pchString);
//
//	for(int i=0; i<iLen; i++)
//	{
//		pchString[i] = toupper(pchString[i]);
//	}
//}

/*
*	Function Name:	MakeLower
*	Description:	将字符串全部改为小写
*	Input Param:
*		pchString -------> 输入字符串
*	Returns:	无
*	complete:	2002/03/13
*/
//void MakeLower(char * pchString)
//{
//	int iLen = strlen(pchString);
//
//	for(int i=0; i<iLen; i++)
//	{
//		pchString[i] = tolower(pchString[i]);
//	}
//}

void disp_buf(char *buf, int size)
{
	int i;
	printf("buf [%d] start:\n", size);
	for( i = 0 ; i < size; i ++ )
	{
		if( i%16 == 0 )
			printf("\n");
		printf("%02X ", buf[i]&0xff);
	}
	printf("\nbuf end\n");
}


