/*
* Copyright 2002 LinkAge Co.,Ltd. Inc. All Rights Reversed
*/

/*********************************************************************************/
/*** Name             : BASEFUN.H                                              ***/
/***                                                                           ***/
/*** Description      : 基本功能函数                                           ***/
/***                                                                           ***/
/*** Author           : 郭亮                       	                       ***/
/***                             	                                       ***/
/*** Begin Time       : 2002/05/26                                             ***/
/***                                                                           ***/
/*** Last Change Time : 2002/05/30       	                               ***/
/*** Revision History : 2003/08/28	修改readdir_r的声明
										**/
/***                                                                           ***/
/*********************************************************************************/

#ifndef BASEFUN_H_
#define BASEFUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include <vector>
#include <list>
#include <stdexcept>
using namespace std;
/*********************************************************************************/
/***                           Standard Include                                ***/
/*********************************************************************************/

extern int errno;

/*********************************************************************************/
/***                           General Constants                               ***/
/*********************************************************************************/

#ifndef LENGTH
#define LENGTH 255
#endif

/*文件名校验错误号定义*/
#define	ERROR_FILE_FILTE_RULE 					"F0001"	//文件名正规表达式描述错误，参数配置错误，请检查配置
#define ERROR_FILE_SEQUENCE_DEF	  			"F0002" //文件名正规表达式中开始序列号描述错误，含非法字符
#define	ERROR_FILE_NAME_FORMAT					"F8000"	//文件名格式错误
#define	ERROR_DATE_FORMAT								"F8001"	// 文件名中的日期不合法
#define	ERROR_FILE_DATE_BEFORE					"F8002"	// 文件名日期早于规定的时间范围
#define	ERROR_FILE_EXPIRE								"F8003"	// 文件名时间超期(文件名时间超过系统当前时间1天)
#define	ERROR_SERIAL_NUMBER							"F8004"	//文件名序号不正确
#define	ERROR_PROVINCE_NO								"F8005"	//文件不属于上传省
#define	ERROR_FILE_NOT_SEQUENCE					"F9000"	// 文件跳号等


const int BUFFSIZE = 8192;
const int TOTAL = 20;

//SearchOneFile的返回值
const int FIND_NOFILE = 0;
const int FIND_NOREAD = -2;
const int FIND_PATHERROR = -1;
const int FIND_OK = 1;
const int FIND_FILTER_NOMATCH=-3;
const int FIND_IGNOREERROR = -4;


/*********************************************************************************/
/***                           Function Declare                                ***/
/*********************************************************************************/

	/*字符串匹配函数*/
	bool Compare(const char *lv_chCompareString,const char *lv_chCompareMod);
	/*字符串匹配函数*/
	bool Compare(const char *lv_chCompareString,const char *lv_chCompareMod,char *lv_ErrorMsg);
	/*将路径配置完整*/
	int FullPath(char *chPath);
	/*得到系统当前日期(两位年)*/
	void GetShortSystemDate(char *pchCurrentDate);
	/*得到系统当前日期*/	
	void GetSystemDate(char *CurrentDate);
	/*得到系统当前时间*/
	void GetSystemTime(char *CurrentTime);
	/*得到系统当前日期时间(14位YYYYMMDDHHMMSS)*/
	void GetSystemDateTime(char *CurrentDateTime);
	/*得到系统当前日期时间(12位YYMMDDHHMMSS)*/
	void GetShortSystemDateTime(char *CurrentDateTime);
	/*得到本机IP*/
	bool GetLocalIp(char *LocalIp);
	/*获取文件大小*/
	long GetFileSize(const char *chFileName);
	/*去掉字符串左右的空格*/
	void Trim(char * String);
	/*将字符串全部改为大写*/
	void MakeUpper(char * pchString);
	/*将字符串全部改为小写*/
	void MakeLower(char * pchString);
	/*校验日期正确性*/
	bool CheckDate(const char *pchString);
	/*校验时间正确性*/
	bool CheckTime(const char *pchString);
	/*校验日期时间正确性*/
	bool CheckDateTime(const char *pchString);
	/*时分秒转换为秒*/
	long hhmmssToss(char * pchString);
	/*从文件中读取一行内容*/
	int GetLine(char *Line,FILE *File);
	/*判断所给路径是否存在*/
	bool IsDirectory(const char *dirname);
	/*按照文件搜索规则查找一个文件*/
	int SearchOneFile(const char *Pathname, const char *Format, char *Filename);
	int SearchOneFile(const char *Pathname, const char *Format, const int IgnoreFlag, char *Filename);
	int SearchOneFile(const char *Pathname, const char *Format, char *Filename, char *ErrorMsg);
	/*按照文件搜索规则从多个目录中查找一个文件*/
	int SearchOneFile(const char Pathname[TOTAL][LENGTH], const char Format[TOTAL][LENGTH], const int TotalIn, char *Filename);
	bool SearchAllFiles(const char *Pathname, const char *Format, vector<string> &vecFileSet);
	bool SearchAllFiles(const char *Pathname, const char *Format, list<string> &lstFileSet);
	/*将文件从一个目录移动到另外的目录并改名*/
	int MoveFile(const char * DesFullFileName, const char *SrcFullFileName);
	/*将文件从一个目录拷贝到另外的一个目录并改名*/
	int CopyFile(const char * DesFullFileName, const char *SrcFullFileName);
	/*获取文件的行数*/
	long GetFileRows(const char *pchFullFileName);
	/*获取短程序名*/
	void GetShortProcName(char *pchProcName);
	/*获取短程序名和所带的路径名*/
	void GetShortProcNameAndPath(char * pchProcNameWithPath, char *pchProcName, char * pchpath);
	//加密
	void Encrypt(const char *pszPlain, char *pszCryptograph);
	//解密
	void Decrypt(const char *pszCryptograph, char *pszPlain);
	//解析转义字符，遵循c style
	char* DecodeBackSlash(char* Str);
	/*校验日期->日正确性*/
	bool CheckDay(const char *pchString);
	/*校验日期->月份正确性*/
	bool CheckMonth(const char *pchString);
	/*校验日期->月日正确性*/
	bool CheckMonthDay(const char *pchString);
	/*校验日期->年份正确性*/
	bool CheckYear(const char *pchString);
	/*校验日期->年月正确性*/
	bool CheckYearMonth(const char *pchString);
	/*删除指定通道下指定批次文件*/
	bool DeleteFile(const char *pFullPathName, const char *pBatchCode);
	void JudgeBusiClassAndType(char * chFileName, int iBusiClass, int iBusiType);
	void 	JudgeYcFlag(const int iBusiClass, const char * chRecordType, const int iDuring, int iYcFlag);
	int time_diff(char *pchStrBeginTime,char *pchStrEndTime);
	
	
#endif		//BASEFUN_H_



