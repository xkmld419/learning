/*
* Copyright 2001 LinkAge Co.,Ltd. Inc. All Rights Reversed
*/

/*********************************************************************************/
/*** Name             : CONFIG.CPP                                             ***/
/***                                                                           ***/
/*** Description      : 读写配置文件模块功能函数                               ***/
/***                                                                           ***/
/*** Author           : 郭亮                       	                       ***/
/*** Upate			  : 冯亮                                               ***/
/***                                                                           ***/
/*** Begin Time       : 2001/11/03                                             ***/
/***                                                                           ***/
/*** Last Change Time : 2001/11/09       	                               ***/
/***                                                                           ***/
/*********************************************************************************/
//modify:主要修改了此功能模块和出入库的互斥锁冲突的问题, bad smell.
/*********************************************************************************/
/***              Class C_Config user include Compile body                     ***/
/***              Config encapsulate all methods(functions)            	       ***/
/*********************************************************************************/

#include "rw_config.h"

C_Config::C_Config()
{
	iWriteFlag = 0;
	memset(chFileTemp, 0, sizeof(chFileTemp));
}

/*
*	Function Name	:Trim
*	Description	:去掉字符串左右的空格
*	Input Param	:需要去掉空格的字符串
*	Returns		:无
*	complete	:2001/11/20
*/
void C_Config::Trim(char * String)
{
	char *Position = String;
	/*找到第一个不是空格的位置*/
	while(isspace((unsigned char)*Position))
	{
		Position++;
	}
	/*如果为一空串则退出*/
	if (*Position == '\0')
	{
		*String = '\0';
		return;
	}

	/*除去前面的空格*/
	while(*Position)
	{
		*String = *Position;
		String++;
		Position++;
	}
	/*除去后面的空格*/
	do
	{
		*String = '\0';
		String--;
	}while(isspace((unsigned char)*String));
}

/*
*    Function Name    : GetLine
*    Description      :	在配置文件中获得一行的内容
*    Input Param      :
*	Line        -----------> 这一行的内容（字符串）
*	Fd          -----------> 打开的文件描述符
*	Returns          :得到的字节数.
*	complete	:2001/11/09
*/
int C_Config::GetLine(int Fd,char *Line)
{
	int iByteRead = 0;
	for(;;)
	{
		if(read(Fd,Line,1) <= 0)
		{
			break;
		}
		iByteRead ++;
		if (*Line == '\n')
		{
			break;
		}
		Line++;
	}
	*Line='\0';
	return iByteRead;
}

/*
*    Function Name    : PutLine
*    Description      :	在配置文件中写入一行的内容
*    Input Param      :
*	Line        -----------> 这一行的内容（字符串）
*	Fd          -----------> 打开的文件描述符
*	Returns          :写入的字节数.
*	complete	:2001/11/09
*/
int C_Config::PutLine(int Fd,char *Line)
{
	char chLine[LINELENGTH];

	sprintf(chLine, "%s\n", Line);
	return write(Fd,chLine,strlen(chLine));
}
/*
*    Function Name    : SetLockStatus
*    Description      :	设置锁的状态
*    Input Param      :
*	Fd         -----------> 打开的文件说明符
*	Cmd        -----------> 锁命令
*	Type       -----------> 文件锁类型
*	Offset     -----------> 相对位移量
*	Whence     -----------> 相对位移量起点
*	Len        -----------> 区域的长度
*	Returns          :锁的状态
*	complete	:2002/11/19
*/
int C_Config::SetLockStatus(int Fd, int Type, int Cmd, off_t Offset, int Whence, off_t Len)
{
	struct flock Lock;

	Lock.l_type = Type;
	Lock.l_whence = Whence;
	Lock.l_start = Offset;
	Lock.l_len = Len;

	return fcntl(Fd, Cmd, &Lock);
}

/*
*    Function Name    : GetParam
*    Description      :	读取配置文件的键值
*    Input Param      :
*	FileName     -----------> 配置文件名
*	Section      -----------> 读取的组名
*	Key          -----------> 读取的关键字名
*	Value        -----------> 读取的键值
*	Returns          :如果组名和关键字名都存在，则返回true，得到正确的键值
*			  否则返回false
*	complete	:2002/11/19
*/
bool C_Config::GetParam(const char *FileName,const char *Section,const char *Key,char *Value)
{
	if(-1 == access(FileName, F_OK|R_OK))
	{
		return false;
	}

	int fd;
	int iCount,iLength;
	char chBuff[LINELENGTH],chTemp[LINELENGTH];
	char chValueTemp[LINELENGTH];
	char chSection[LINELENGTH],chKey[LINELENGTH];
	char *pchPoint = NULL,*pchTemp = NULL,*pchEnv = NULL;
	string sLine,sSection;

	memset(chTemp, 0, sizeof(chTemp));

	iCount = 0;

	if(strcmp(FileName,chFileTemp) != 0)
	{
		VRecord.clear();
		strcpy(chFileTemp,FileName);

		if(-1 == (fd = open(FileName,O_RDONLY)))
		{
			return false;
		}

		//将所有文件内容放入向量容器中
		for(;;)
		{
			memset(chBuff,0,sizeof(chBuff));
			if ((iCount = GetLine(fd,chBuff)) == 0)
			{
				break;
			}
			Trim(chBuff);
			if ((';' == chBuff[0]) || ('#' == chBuff[0]) || ('\0' == chBuff[0]))
			{
				continue;
			}
			sLine = chBuff;
			VRecord.push_back(sLine);
		}
		close(fd);
	}

	iCount = 0;
	vector<string>::iterator it;
	it = VRecord.begin();

	memset(chSection, 0, sizeof(chSection));
	memset(chKey,0,sizeof(chKey));

//得到Section
	memset(chTemp, 0, sizeof(chTemp));
	sprintf(chTemp,"[%s]",Section);
	sSection = chTemp;

	while(it<VRecord.end())
	{
		if (*it == sSection)
		{
			it++;
			iCount++;
			break;
		}
		it++;
		iCount++;
		if (iCount == VRecord.size())
		{
			return false;
		}
	}

//得到Key
	while(it<VRecord.end())
	{
		memset(chBuff, 0, sizeof(chBuff));
		strcpy(chBuff, (*it).c_str());

		//在本组里找不到关键字
		if ('[' == chBuff[0])
		{
			return false;
		}

		pchPoint = chBuff;
		if ((pchTemp = strchr(pchPoint,'=')) != NULL)
		{
			strncpy(chKey, chBuff, pchTemp - pchPoint);
			chKey[pchTemp - pchPoint] = '\0';
			Trim(chKey);
		}

		if (!strcmp(chKey,Key))
		{
			it++;
			iCount++;
			break;
		}
		it++;
		iCount++;
		if (iCount == VRecord.size())
		{
			return false;
		}
	}

//得到Value
	pchTemp++;
	strcpy(chValueTemp,pchTemp);
	Trim(chValueTemp);//20030424郭亮修改：不能操作传进来的内存部分，而应该操作自己的内存部分
	chValueTemp[LINELENGTH-1] = 0;
	strcpy(Value, chValueTemp);

//遇到配置文件比较长的时候,需要换行,则以'\'符号标志做为分隔符
	iLength = strlen(Value);
	
	if ('\\' == Value[iLength - 1])
	{
		Value[iLength - 1] ='\0';
		while(it<VRecord.end())
		{
			memset(chBuff,0,sizeof(chBuff));
			strcpy(chBuff,(*it).c_str());
			iLength = strlen(chBuff);
			
			if (chBuff[iLength - 1] == '\\')
			{
				chBuff[iLength - 1] = '\0';
				strcat(Value,chBuff);
			}
			else if (chBuff[iLength - 1] != '\\')
			{
				strcat(Value,chBuff);
				break;
			}
			it++;
			iCount++;
			if (iCount == VRecord.size())
			{
				break;
			}
		}
	}

	if('$' == Value[0])
	{
		pchPoint = Value;
		pchPoint++;
		if(((pchTemp = strchr(pchPoint,'.')) != NULL) && (strchr(pchPoint, '/') == NULL))
		{
			strncpy(chSection,pchPoint,pchTemp - pchPoint);
			chSection[pchTemp - pchPoint] = '\0';
			pchTemp++;
			strcpy(chKey,pchTemp);

			return GetParam(FileName,chSection,chKey,Value);
		}
		if(pchTemp = strchr(pchPoint,'/'))
		{
			strncpy(chTemp,pchPoint,pchTemp - pchPoint);
			chTemp[pchTemp - pchPoint] = '\0';

			memset(Value,0,sizeof(Value));
			if((pchEnv = getenv(chTemp)) != NULL)
			{
				if(('/' == *(pchEnv + strlen(pchEnv) - 1))&&('/' == *pchTemp))
				{
					pchTemp++;
				}
				strcpy(chTemp,pchTemp);
				sprintf(Value,"%s%s",pchEnv,chTemp);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

/*
*    Function Name    : SetParam
*    Description      :	设置配置文件
*    Input Param      :
*	Filename     -----------> 配置文件名
*	Section      -----------> 建立的组名
*	Key          -----------> 建立的关键字名
*	Value        -----------> 建立的键值
*	Returns          :如果文件不存在，返回false
*                         如果文件存在，设置完毕则返回true
*	complete	:2001/11/09
*/
bool C_Config::SetParam(const char *FileName,const char *Section,const char *Key,const char *Value)
{
	int fdt,fd;
	int iCount = 0;
	char chTemp[LINELENGTH], chTempName[LINELENGTH];;
	char chSection[LINELENGTH],chKey[LINELENGTH];
	char *pchPoint = NULL,*pchTemp = NULL;
	string sLine;

	sprintf(chTempName,"%s.tmp",FileName);
	if(-1 == (fdt = open(chTempName,O_RDWR|O_CREAT|O_EXCL)))
	{
		if(-1 == (fdt = open(chTempName,O_RDWR|O_TRUNC)))
		{
			unlink(chTempName);
			return false;
		}
	}
	//设置文件写锁
	SetLockStatus(fdt,F_WRLCK);

	fchmod(fdt,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	//设置文件写标志
	iWriteFlag = 1;
	//清空容器内容
	VRecord.clear();

	if(-1 == (fd = open(FileName,O_RDONLY)))
	{
		SetLockStatus(fdt,F_UNLCK);
		close(fdt);
		iWriteFlag = 0;
		unlink(chTempName);
		return false;
	}

	//将文件内容全部放入容器中
	for(;;)
	{
		memset(chTemp, 0, sizeof(chTemp));
		if(0 == GetLine(fd, chTemp))
		{
			break;
		}
		sLine = chTemp;
		VRecord.push_back(sLine);
	}
	close(fd);

	//防止文件空洞而将原文件长度截为0
	if (-1 == (fd = open(FileName,O_RDWR|O_CREAT|O_TRUNC)))
	{
		SetLockStatus(fdt,F_UNLCK);
		close(fdt);
		iWriteFlag = 0;
		unlink(chTempName);
		return false;
	}

	//设置文件属性为只读,防止被错误修改
	if (-1 == fchmod(fd,S_IRUSR|S_IRGRP))
	{
		SetLockStatus(fdt,F_UNLCK);
		close(fdt);
		iWriteFlag = 0;
		unlink(chTempName);
		return false;
	}

	iCount = 0;
	vector<string>::iterator it;
	it = VRecord.begin();

	memset(chSection, 0, sizeof(chSection));
	memset(chKey,0,sizeof(chKey));

//得到Section
	sprintf(chSection,"[%s]",Section);

	//郭亮2003-01-20修改
	//如果文件为空,则新建所有选项,写入文件
	if (VRecord.size() == 0)
	{
		sprintf(chKey, "\t%s = %s", Key, Value);
		PutLine(fd, chSection);
		PutLine(fd, chKey);
		//释放锁并关闭文件
		SetLockStatus(fdt,F_UNLCK);
		fchmod(fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
		close(fdt);
		close(fd);
		unlink(chTempName);
		return true;
	}

	while(it<VRecord.end())
	{
		memset(chTemp, 0, sizeof(chTemp));
		strcpy(chTemp, (*it).c_str());
		PutLine(fdt, chTemp);
		it++;
		iCount++;
		Trim(chTemp);
		if ((';' == chTemp[0]) || ('#' == chTemp[0]))
		{
			continue;
		}

		if (!strcmp(chTemp, chSection))
		{
			break;
		}

		//已经到文件末尾,没有该组名,则在文件末尾新建该组名并写入参数值
		if (iCount == VRecord.size())
		{
			for(it = VRecord.begin();it<VRecord.end();it++)
			{
				memset(chTemp, 0, sizeof(chTemp));
				strcpy(chTemp, (*it).c_str());
				PutLine(fd, chTemp);
			}

			sprintf(chKey, "\t%s = %s", Key, Value);
			PutLine(fd, chSection);
			PutLine(fd, chKey);
			//释放锁并关闭文件
			SetLockStatus(fdt,F_UNLCK);
			fchmod(fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
			close(fdt);
			close(fd);
			unlink(chTempName);
			return true;
		}
	}

	//郭亮2003-01-21修改
	//找到了该组名,但是已经是文件最后一行,则在文件末尾写入参数值
	if (iCount == VRecord.size())
	{
		for(it = VRecord.begin();it<VRecord.end();it++)
		{
			memset(chTemp, 0, sizeof(chTemp));
			strcpy(chTemp, (*it).c_str());
			PutLine(fd, chTemp);
		}
		sprintf(chKey, "\t%s = %s", Key, Value);
		PutLine(fd, chKey);
		//释放锁并关闭文件
		SetLockStatus(fdt,F_UNLCK);
		fchmod(fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
		close(fdt);
		close(fd);
		unlink(chTempName);
		return true;
	}

//得到Key
	while(it<VRecord.end())
	{
		memset(chTemp, 0, sizeof(chTemp));
		strcpy(chTemp, (*it).c_str());
		Trim(chTemp);

		//本组里没有该关键字,新建参数值并写入文件
		if ('[' == chTemp[0])
		{
			sprintf(chKey, "\t%s = %s", Key, Value);
			PutLine(fdt, chKey);
			PutLine(fdt, chTemp);
			it++;
			iCount++;
			break;
		}
		if ((';' == chTemp[0]) || ('#' == chTemp[0]))
		{
			it++;
			iCount++;
			PutLine(fdt, chTemp);
			continue;
		}

		pchPoint = chTemp;
		if ((pchTemp = strchr(pchPoint,'=')) != NULL)
		{
			strncpy(chKey, chTemp, pchTemp - pchPoint);
			chKey[pchTemp - pchPoint] = '\0';
			Trim(chKey);
		}
		if (!strcmp(chKey,Key))
		{
			it++;
			iCount++;
			sprintf(chKey, "\t%s = %s", Key, Value);
			PutLine(fdt, chKey);
			break;
		}

		sprintf(chTemp, "%s",(*it).c_str());
		PutLine(fdt, chTemp);
		it++;
		iCount++;
		//已经到文件末尾,则添加该参数值
		if (iCount == VRecord.size())
		{
			for(it = VRecord.begin();it<VRecord.end();it++)
			{
				memset(chTemp, 0, sizeof(chTemp));
				strcpy(chTemp, (*it).c_str());
				PutLine(fd, chTemp);
			}

			sprintf(chKey, "\t%s = %s", Key, Value);
			PutLine(fd, chKey);
			//释放锁并关闭文件
			SetLockStatus(fdt,F_UNLCK);
			fchmod(fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
			close(fdt);
			close(fd);
			unlink(chTempName);
			return true;
		}
	}

	while(it<VRecord.end())
	{
		memset(chTemp, 0, sizeof(chTemp));
		strcpy(chTemp, (*it).c_str());
		PutLine(fdt, chTemp);
		it++;
	}

	//将临时文件描述符移动到临时文件头,将内容写回原文件
	lseek(fdt,0,SEEK_SET);

	for(;;)
	{
		memset(chTemp, 0, sizeof(chTemp));
		if(0 == GetLine(fdt, chTemp))
		{
			break;
		}
		PutLine(fd, chTemp);
	}

	SetLockStatus(fdt,F_UNLCK);
	fchmod(fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	close(fdt);
	close(fd);
	unlink(chTempName);
	return true;
}

