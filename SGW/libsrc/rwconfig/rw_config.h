/*
* Copyright 2001 LinkAge Co.,Ltd. Inc. All Rights Reversed
*/

/*********************************************************************************/
/*** Name             : CONFIG.H                                               ***/
/***                                                                           ***/
/*** Description      : 读写配置文件模块                                       ***/
/***                                                                           ***/
/*** Author           : 郭亮                                                   ***/
/***                                                                           ***/
/*** Begin Time       : 2001/11/03                                             ***/
/***                                                                           ***/
/*** Last Change Time : 2001/11/09                                             ***/
/***                                                                           ***/
/*********************************************************************************/

/*********************************************************************************/
/***               Class C_Config user include Compile declare                 ***/
/***               Config encapsulate all global methods(functions)            ***/
/*********************************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

/*********************************************************************************/
/***                           Standard Include                                ***/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <vector>

#include <stdexcept>
using namespace std;
#ifdef DEF_AIX
#include "c++dcl.h"
#endif

/*********************************************************************************/
/***                          General Constants                                ***/
/*********************************************************************************/

#ifndef	LINELENGTH
#define LINELENGTH 4096
#endif

class C_Config
{
public:
	C_Config();

	/*读配置文件函数*/
	bool GetParam(const char *FileName,const char *Section,const char *Key,char *Value);
	/*写配置文件函数*/
	bool SetParam(const char *FileName,const char *Section,const char *Key,const char *Value);

private:
	/*去掉字符串两边的空格函数*/
	void Trim(char * String);
	/*从文件中得到一行数据函数*/
	int GetLine(int Fd,char *Line);
	/*向文件中写入一行数据函数*/
	int PutLine(int Fd,char *Line);
	/*设置锁状态*/
	int SetLockStatus(int Fd, int Type,
			int Cmd = F_SETLKW, off_t Offset = 0,
			int Whence = SEEK_SET, off_t Len = 0);
private:
	/*读配置文件所有信息的容器*/
	vector<string> VRecord;
	/*上一次读取的配置文件名*/
	char chFileTemp[LINELENGTH];
	/*写文件标志位*/
	int iWriteFlag;

};

//typedef C_Config CConfig;

#endif/*CONFIG.H*/



