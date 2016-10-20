/***********************************************************
 *
 *   Copyright (c) 1997-2010  Asiainfo-Linkage Technology Co.,Ltd.
 *
 *   All rights reserved.
 *
 *   文件名称：CPacketComm.h
 *
 *   简要描述：协议打包公共部分
 *
 *   当前版本：1.0
 *
 *   作者/修改者：林高誉  lingy@asiainfo-linkage.com
 *
 *   编写日期：2010/10/30
 *
 ************************************************************/

#ifndef __PACKET_COMM_H__
#define __PACKET_COMM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <exception>
#include <string>
#include <vector>
#include <iconv.h>
#include <iostream>
#include <errno.h>

using namespace std;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;
//typedef char	int8;				/* Signed integer >= 8	bits */
typedef short int16; /* Signed integer >= 16 bits */
typedef unsigned char uint8; /* Short for unsigned integer >= 8  bits */
typedef unsigned short uint16; /* Short for unsigned integer >= 16 bits */
typedef int int32;
typedef unsigned int uint32; /* Short for unsigned integer >= 32 bits */
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef char* PBYTE;
//typedef char BYTE;
//typedef char TCHAR;
typedef unsigned int UINT;
//typedef bool BOOL;
//typedef int  HANDLE;
typedef void * LPVOID;
typedef char byte;

//Starting here, for Big Endian defition
#define storeInt(T, A) {	\
	*((T)+3) =	(uchar) ((A));	\
	*((T)+2) = (uchar) (((A) >> 8));	\
	*((T)+1) = (uchar) (((A) >> 16));	\
	*(T) = (uchar) (((A) >> 24)); }

#define storeInt2(T, A) {	\
	*((T)+1) = (uchar) (((A)));	\
	*(T) = (uchar) (((A) >> 8)); }

#define readInt(A)	((int) (	\
				    (((int) ((uchar) (A)[0])) << 24) +\
				    (((uint32) ((uchar) (A)[1])) << 16) +\
				    (((uint32) ((uchar) (A)[2])) << 8) +\
					((uchar) (A)[3])))

#define readInt2(A)	((int) (	\
				    (((int) ((uchar) (A)[0])) << 8) +\
					((uchar) (A)[1])))

//换转缓冲区大小
const int MAX_CONVBUF_LEN = 4096;
const int MILLISECONDS_PER_SECONDS = 1000; //1秒=1000毫秒
const int MICROSECONDS_PER_MILLSECONDS = 1000; //1毫秒=1000微秒
const int MICROSECONDS_PER_SECONDS = 1000000; //1秒=1000000微秒

/*
 char C_EVENT_RETURN = '\n';
 char C_EVENT_SPLIT = '|';
 string STR_EVENT_SPLIT = "|";
 */
//编码转换方案
typedef enum
{
	EN_NO_CONVERT = 0, //不转,保持不变
	EN_Utf8ToGB2312 = 1,
	EN_GB2312ToUtf8 = 2,
	EN_Utf8ToGBK = 3,
	EN_GBKToUtf8 = 4,
	EN_Ucs2ToGB2312 = 5,
	EN_GB2312ToUcs2 = 6,
	EN_Ucs2ToGBK = 7,
	EN_GBKToUcs2 = 8
} E_CODE_CONVERT_SCHEMA;

typedef enum
{
	CODE_SCHEMA_TUXC_TO_SGWS = EN_Utf8ToGBK, //外部TUXEDO客户端到SGW服务端
	CODE_SCHEMA_SGWS_TO_TUXC = EN_GBKToUtf8,//SGW服务端到外部TUXEDO客户端
	CODE_SCHEMA_SGWC_TO_TUXS = EN_GBKToUtf8,//SGW客户端到外部TUXED服务端
	CODE_SCHEMA_TUXS_TO_SGWC = EN_Utf8ToGBK
//外部TUXEDO服务端到SGW客户端
} E_CODE_CONVERT_DIRECTION;

typedef enum
{
	EN_EVENT_SPLIT = 1, EN_EVENT_RETURN = 2

} E_RECORD_STYLE;

//包处理工具类
class CPacketUtil
{
public:
	CPacketUtil();
	~CPacketUtil();
private:
	//编码转换
	int convert(const char *sFromCharset,const char *sToCharset,const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
public:
	//编码转换
	int convert(int iConvertSchema,const char *sInBuf, char *sOutBuf, int &iOutLen);
	//编码转换
	int convert(int iConvertSchema,const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	//缓冲区计算
	int calcbuf2(char *buf, bool bTrans);
	int calcbuf3(char *buf, bool bTrans);
	int calcbuf4(char *buf, bool bTrans);
	void Int32toArray(char *buf, uint32 uiValue);
	uint32 ArraytoInt32(char *buf);
	void Int16toArray(char *buf, uint16 uiValue);
	uint16 ArraytoInt16(char *buf);

	//计算汉字字符个数
	int ChineseCharCount(const char *sMsg, int iFirst, int iLast);
	//消息拆分
	int splitMessage(const char *sMsgContent, int iMsgLen, int iSingleMsgLen, vector<string> &vOutMsg);
	//记录分解
	int splitRecord(string &in_sOneEvent, const string &in_sSep, int in_iStyle, vector<string> &out_vFieldList);

	//UTF8码转为GB2312码
	int Utf8ToGB2312(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
	//GB2312码转为UTF8码
	int GB2312ToUtf8(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	//UTF8码转为GBK码
	int Utf8ToGBK(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
	//GBK码转为UTF8码
	int GBKToUtf8(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	//UCS2码转为GB2312码
	int Ucs2ToGB2312(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
	//GB2312码转为UCS2码
	int GB2312ToUcs2(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	//UCS2码转为GBK码
	int Ucs2ToGBK(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
	//GBK码转为UCS2码
	int GBKToUcs2(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	//只拷贝,不转换
	int copy(const char *sInBuf, char *sOutBuf, int &iOutLen);
	//只拷贝,不转换
	int copy(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);

	// 系统编码转UTF8
	int Sys2Utf(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
	// UTF8转系统编码
	int Utf2Sys(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen);
};

#endif  /* __PACKET_COMM_H__ */
