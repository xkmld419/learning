/***********************************************************
 *
 *   Copyright (c) 1997-2010  Asiainfo-Linkage Technology Co.,Ltd.
 *
 *   All rights reserved.
 *
 *   文件名称：CPacketComm.cpp
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

#include "CPacketComm.h"

#ifdef  DEF_LINUX
#define MSG_PARSE_UTF_ "UTF-8"
#define MSG_PARSE_GBK_ "gb2312"
#endif
#ifdef  DEF_HP
#define MSG_PARSE_UTF_ "UTF8"
#define MSG_PARSE_GBK_ "hp15CN"
#endif
#ifdef  DEF_AIX
#define MSG_PARSE_UTF_ "UTF-8"
#define MSG_PARSE_GBK_ "GB18030"
#endif

/****************************************************************************
 * CPacketUtil 工具类
 ****************************************************************************/

CPacketUtil::CPacketUtil()
{
}
CPacketUtil::~CPacketUtil()
{
}

/*
 * -----------------------------------------------------
 * 名称	           第一字节	                                第二字节
 * -----------------------------------------------------
 * GB2312	0xB0-0xF7(176-247) 	0xA0-0xFE(160-254)
 * GBK	    0x81-0xFE(129-254)	0x40-0xFE(64-254)
 * Big5	    0x81-0xFE(129-255)	0x40-0x7E(64-126)
 *                              0xA1－0xFE(161-254)
 * -----------------------------------------------------
 **/
/******************************************************************************************************************
 *       函数名称:     ChineseCharCount
 *       函数描述:     计算汉字字符个数
 *       输入参数:     sMsg	       消息地址
 *                    iFirst   起始地址
 *                    iLast    终止地址
 *       输出参数:     无
 *       返 回 值:      计算汉字字符个数
 ******************************************************************************************************************/
int CPacketUtil::ChineseCharCount(const char *sMsg, int iFirst, int iLast)
{
	int iCount = 0;
	unsigned char uiValue = 0;

	for (int i = iFirst; i <= iLast; i++)
	{
		uiValue = (unsigned char) sMsg[i];

		if (uiValue >= 64 && uiValue <= 254)
		{
			iCount++;
		}
	}
	return iCount;
}

/******************************************************************************************************************
 *       函数名称:     splitMessage
 *       函数描述:     消息拆分
 *       输入参数:     sMsgContent	  待拆分的消息内容
 *                    iMsgLen       待拆分的消息长度
 *       输出参数:     vOutMsg       拆分后的消息
 *       返 回 值:      0:成功; 非0:失败
 ******************************************************************************************************************/
int CPacketUtil::splitMessage(const char *sMsgContent, int iMsgLen, int iSingleMsgLen, vector<string> &vOutMsg)
{
	int i, iCurPos = 0, iPrePos = 0, iOffset = 5, iOffsetCopy, iCopyLen = 0;
	int iSumLen = 0; //原始总长度
	int iPerLen = iSingleMsgLen - iOffset; //每条短信长度
	int iSmNo = 0, iSmCounts = 0;
	unsigned char uiChar1, uiChar2, uiChar3;
	char sSmText[150];

	memset(sSmText, 0, sizeof(sSmText));

	vOutMsg.clear();

	if (sMsgContent == NULL)
	{
		return (-1);
	}

	if (iMsgLen <= iSingleMsgLen)
	{
		iSumLen = strlen(sMsgContent);
		if (iSumLen <= iSingleMsgLen)
		{
			vOutMsg.push_back(string(sMsgContent));
			return 0;
		}
	}
	else
	{
		iSumLen = iMsgLen;
	}

	iCurPos = -1; //当前位置
	iPrePos = -1; //前一个位置
	iSmNo = 0; //当前短信条数
	iSmCounts = static_cast<int> (iSumLen / iPerLen) + 1; //总条数

	try
	{
		while (iSmNo < iSmCounts)
		{
			iSmNo++;
			sprintf(sSmText, "(%d/%d)", iSmNo, iSmCounts);

			iOffsetCopy = strlen(sSmText);

			iPrePos = iCurPos;
			iCurPos += iPerLen;

			if (iCurPos < iSumLen - 1) //如果剩余要处理的消息长度大于规定的一条消息的长度
			{
				uiChar2 = (unsigned char) sMsgContent[iCurPos];

				if (uiChar2 >= 64 && uiChar2 <= 254) //是汉字字符
				{
					int iCalcCount = ChineseCharCount(sMsgContent, iPrePos + 1, iCurPos);

					if ((iCalcCount % 2) != 0)
					{
						//是第奇数个汉字字符,说明是个汉字的第一个字节,定位回退一位
						iCurPos--;
					}
				}
			}
			else
			{
				//剩余字节全部处理,是拆分后的最后一条短信
				iCurPos = iSumLen - 1;
			}
			iCopyLen = iCurPos - iPrePos;
			memcpy(sSmText + iOffsetCopy, (sMsgContent + iPrePos + 1), iCopyLen);
			sSmText[iOffsetCopy + iCopyLen] = '\0';
			vOutMsg.push_back(string(sSmText));
		}
	}
	catch(exception &ex)
	{
		printf("<< splitMessage >> exception = %s \n", ex.what());
		return 1;
	}
	catch(...)
	{
		printf("<< splitMessage >>  UNKNOWN ERROR. \n");
		return 2;
	}
	return 0;
}

/*
 * 功能:分解记录(分隔符)
 * in_iStyle == 1 的风格：
 *   123|456|789|
 *    即每个字段都有同样的分隔符
 * in_iStyle == 2 的风格：
 *    123|456|789\n
 *    最后一个字段的分隔符不同,是换行符\n
 * */

/******************************************************************************************************************
 *       函数名称:           splitRecord
 *       函数描述: 分解记录(按分隔符)
 * in_iStyle == 1 的风格：
 *   123|456|789|
 *    即每个字段都有同样的分隔符
 * in_iStyle == 2 的风格：
 *    123|456|789\n
 *    最后一个字段的分隔符不同,是换行符\n
 *
 *       输入参数:           in_sOneEvent    记录
 *                           in_sSep    分隔符
 *                           in_iStyle       分隔符类型(1或2)
 *       输出参数:           out_vFieldList    记录输出
 *       返 回 值：                              0:成功, 非0:失败
 ******************************************************************************************************************/
int CPacketUtil::splitRecord(string &in_sOneEvent, const string &in_sSep, int in_iStyle, vector<string> &out_vFieldList)
{
	int iPos = 0, iCount = 0;

	out_vFieldList.clear();

	if (in_sOneEvent.length() == 0)
	{
		return -1;
	}

	try
	{
		iPos = in_sOneEvent.find(in_sSep, 0);
		while (iPos >= 0)
		{
			out_vFieldList.push_back(in_sOneEvent.substr(0, iPos));
			in_sOneEvent.erase(0, iPos + 1);
			iCount++;

			iPos = in_sOneEvent.find(in_sSep, 0);
		}
		if (in_iStyle == EN_EVENT_RETURN)
		{
			//取最后一个字段
			iPos = in_sOneEvent.find("\n", 0);
			if (iPos > 0)
			{
				out_vFieldList.push_back(in_sOneEvent.substr(0, iPos));
			}
			else
			{
				out_vFieldList.push_back(in_sOneEvent);
			}
		}
	}
	catch(exception &ex)
	{
		printf("%s(%d), exception = %s\n", __FILE__, __LINE__, ex.what());
		return -2;
	}
	catch(...)
	{
		printf("%s(%d), other error!\n", __FILE__, __LINE__);
		return -3;
	}
	return 0;
}

/******************************************************************************************************************
 *       函数名称:           calcbuf2
 *       函数描述:           计算两个字节的缓冲区
 *       输入参数:           buf       缓冲区地址
 *                           bTrans    是否进行字节序转换(true: 转换; false: 不转换)
 *       输出参数:           无
 *       返 回 值：                              计算结果
 ******************************************************************************************************************/
int CPacketUtil::calcbuf2(char *buf, bool bTrans)
{
	int iret;
	char s_buf[4];

	if (bTrans)
	{
		s_buf[3] = 0x0;
		s_buf[2] = 0x0;
		s_buf[1] = buf[0];
		s_buf[0] = buf[1];
	}
	else
	{
		s_buf[0] = 0x0;
		s_buf[1] = 0x0;
		s_buf[2] = buf[0];
		s_buf[3] = buf[1];
	}

	memcpy(&iret, s_buf, 4);

	return iret;

}

/******************************************************************************************************************
 *       函数名称:           calcbuf3
 *       函数描述:           计算三个字节的缓冲区
 *       输入参数:           buf       缓冲区地址
 *                           bTrans    是否进行字节序转换(true: 转换; false: 不转换)
 *       输出参数:           无
 *       返 回 值：                              计算结果
 ******************************************************************************************************************/
int CPacketUtil::calcbuf3(char *buf, bool bTrans)
{
	int iret;
	char s_buf[4];

	if (bTrans)
	{
		s_buf[3] = 0x0;
		s_buf[2] = buf[0];
		s_buf[1] = buf[1];
		s_buf[0] = buf[2];
	}
	else
	{
		s_buf[0] = 0x0;
		s_buf[1] = buf[0];
		s_buf[2] = buf[1];
		s_buf[3] = buf[2];
	}

	memcpy(&iret, s_buf, 4);

	return iret;
}

/******************************************************************************************************************
 *       函数名称:           calcbuf4
 *       函数描述:           计算四个字节的缓冲区
 *       输入参数:           buf       缓冲区地址
 *                           bTrans    是否进行字节序转换(true: 转换; false: 不转换)
 *       输出参数:           无
 *       返 回 值：                              计算结果
 ******************************************************************************************************************/
int CPacketUtil::calcbuf4(char *buf, bool bTrans)
{
	int iret;
	char s_buf[4];

	if (bTrans)
	{
		s_buf[3] = buf[0];
		s_buf[2] = buf[1];
		s_buf[1] = buf[2];
		s_buf[0] = buf[3];
		memcpy(&iret, s_buf, 4);
	}
	else
	{
		memcpy(&iret, buf, 4);
	}

	return iret;

}

void CPacketUtil::Int32toArray(char *buf, uint32 uiValue)
{
	*(buf) = (uchar) (uiValue >> 24);
	*(buf + 1) = (uchar) ((uiValue >> 16) & 255);
	*(buf + 2) = (uchar) ((uiValue >> 8) & 255);
	*(buf + 3) = (uchar) (uiValue & 255);
}

uint32 CPacketUtil::ArraytoInt32(char *buf)
{
	return (uint32) (((uchar) buf[0] << 24) + ((uint32) buf[1] << 16) + ((uint32) buf[2] << 8) + (uchar) buf[3]);
}

void CPacketUtil::Int16toArray(char *buf, uint16 uiValue)
{
	*(buf) = (uchar) (uiValue >> 8);
	*(buf + 1) = (uchar) (uiValue & 255);
}

uint16 CPacketUtil::ArraytoInt16(char *buf)
{
	return (uint32) (((uchar) buf[0] << 8) + (uchar) buf[1]);
}

/******************************************************************************************************************
 *       函数名称:           convert
 *       函数描述:           编码转换
 *       输入参数:           sFromCharset  原编码
 *                           sToCharset   目标编码
 *                           sInBuf       输入缓冲区地址
 *                           iInLen       输入缓冲区长度
 *       输出参数                                sOutBuf       输出缓冲区地址
 *                           iOutLen      输出缓冲区长度
 *       返 回 值：                              计算结果                0: 成功, 非0: 失败
 ******************************************************************************************************************/
int CPacketUtil::convert(const char *sFromCharset,const char *sToCharset,const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	iconv_t cd;
	int iRet;
	int iTmpLength = 200;
	char out[MAX_CONVBUF_LEN];
	char *inbuf = (char *) sInBuf;
	char *outbuf = out;
	size_t inlen = iInLen;
	size_t outlen = MAX_CONVBUF_LEN;

	if (inlen <= 0)
		inlen = strlen(sInBuf);

	if (inlen == 0)
	{
		iOutLen = 0;
		return 0;
	}

	//打开句柄
	cd = iconv_open(sToCharset, sFromCharset);
	if (cd == (iconv_t) - 1)
	{
		printf("iconv_open(\"%s\", \"%s\") failed[%s]\n", sToCharset, sFromCharset, strerror(errno));
		return (-1);
	}

	memset(out, 0, sizeof(out));

	iRet = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);

	if (iRet == (size_t) -1)
	{
		if (errno == E2BIG)
		{
			printf("Skipping input line with length greater than column width of %d characters:\n%s[%s]\n", 255, sInBuf,
					strerror(errno));
		}
		else
		{
			printf("iconv() %s failed[%s]\n", sInBuf, strerror(errno));
		}

		//关闭句柄
		iconv_close(cd);

		return (-2);
	}

	//转换后的长度
	iOutLen = MAX_CONVBUF_LEN - outlen;

	memcpy(sOutBuf, out, iOutLen);
	sOutBuf[iOutLen] = '\0';

	//关闭句柄
	iconv_close(cd);

	return 0;

}

//UTF8码转为GB2312码
int CPacketUtil::Utf8ToGB2312(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("UTF-8", "GB2312", sInBuf, iInLen, sOutBuf, iOutLen);
}
//GB2312码转为UTF8码
int CPacketUtil::GB2312ToUtf8(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("GB2312", "UTF-8", sInBuf, iInLen, sOutBuf, iOutLen);
}

//UTF8码转为GBK码
int CPacketUtil::Utf8ToGBK(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("UTF-8", "GBK", sInBuf, iInLen, sOutBuf, iOutLen);
}
//GBK码转为UTF8码
int CPacketUtil::GBKToUtf8(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("GBK", "UTF-8", sInBuf, iInLen, sOutBuf, iOutLen);
}

//UCS2码转为GB2312码
int CPacketUtil::Ucs2ToGB2312(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("UCS-2", "GB2312", sInBuf, iInLen, sOutBuf, iOutLen);
}
//GB2312码转为UCS2码
int CPacketUtil::GB2312ToUcs2(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("GB2312", "UCS-2", sInBuf, iInLen, sOutBuf, iOutLen);
}

//UCS2码转为GBK码
int CPacketUtil::Ucs2ToGBK(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("UCS-2", "GBK", sInBuf, iInLen, sOutBuf, iOutLen);
}
//GBK码转为UCS2码
int CPacketUtil::GBKToUcs2(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert("GBK", "UCS-2", sInBuf, iInLen, sOutBuf, iOutLen);
}

//只拷贝,不转换
int CPacketUtil::copy(const char *sInBuf, char *sOutBuf, int &iOutLen)
{
	int iInLen = strlen(sInBuf);
	return copy(sInBuf, iInLen, sOutBuf, iOutLen);
}

//只拷贝,不转换
int CPacketUtil::copy(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	if (iInLen <= 0)
	{
		iOutLen = 0;
	}
	else
	{
		strncpy(sOutBuf, sInBuf, iInLen);
		iOutLen = iInLen;
	}
	return iOutLen;
}

//编码转换
int CPacketUtil::convert(int iConvertSchema,const char *sInBuf, char *sOutBuf, int &iOutLen)
{
	int iInLen = strlen(sInBuf);
	return convert(iConvertSchema, sInBuf, iInLen, sOutBuf, iOutLen);
}

//编码转换
int CPacketUtil::convert(int iConvertSchema,const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	switch (iConvertSchema)
	{
		case EN_NO_CONVERT://不转换
			return copy(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_Utf8ToGB2312:
			return Utf8ToGB2312(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_GB2312ToUtf8:
			return GB2312ToUtf8(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_Utf8ToGBK:
			return Utf8ToGBK(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_GBKToUtf8:
			return GBKToUtf8(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_Ucs2ToGB2312:
			return Ucs2ToGB2312(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_GB2312ToUcs2:
			return GB2312ToUcs2(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_Ucs2ToGBK:
			return Ucs2ToGBK(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		case EN_GBKToUcs2:
			return GBKToUcs2(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
		default://不转换
			return copy(sInBuf, iInLen, sOutBuf, iOutLen);
			break;
	}
}

// 系统编码转UTF8
int CPacketUtil::Sys2Utf(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert(MSG_PARSE_GBK_, MSG_PARSE_UTF_, sInBuf, iInLen, sOutBuf, iOutLen);
}

// UTF8转系统编码
int CPacketUtil::Utf2Sys(const char *sInBuf, int iInLen, char *sOutBuf, int &iOutLen)
{
	return convert(MSG_PARSE_UTF_, MSG_PARSE_GBK_, sInBuf, iInLen, sOutBuf, iOutLen);
}

/*
 int main(void)
 {
 char *in_utf8 = "姝ｅ湪瀹夎";
 char *in_gb2312 = "正在安装";
 char out[MAX_CONVBUF_LEN];
 int ioutlen = MAX_CONVBUF_LEN;
 int iinlen = strlen(in_gb2312);
 string ss("");
 string ss1("");
 int ret;

 CPacketUtil oPacketUtil;

 memset(out, 0, sizeof(out));

 ret = oPacketUtil.gb2312_to_utf8(in_gb2312, iinlen, out, ioutlen);
 ss = out;
 cout << "out=[" << out << "]" << endl;
 cout << "ss1=[" << ss << "]" << endl;

 memset(out, 0, sizeof(out));
 ioutlen = MAX_CONVBUF_LEN;
 ret = oPacketUtil.utf8_to_gb2312((char *) ss.c_str(), ss.length(), out, ioutlen);
 ss1 = out;

 cout << "out=[" << out << "]" << endl;
 cout << "ss2=[" << ss1 << "]" << endl;

 iinlen = strlen(in_utf8);
 memset(out, 0, sizeof(out));
 ioutlen = MAX_CONVBUF_LEN;
 ret = oPacketUtil.utf8_to_gb2312(in_utf8, iinlen, out, ioutlen);
 ss = out;
 cout << "out=[" << out << "]" << endl;
 cout << "ss3=[" << ss << "]" << endl;

 memset(out, 0, sizeof(out));
 ioutlen = MAX_CONVBUF_LEN;
 ret = oPacketUtil.gb2312_to_utf8((char *) ss.c_str(), ss.length(), out, ioutlen);
 ss1 = out;

 cout << "out=[" << out << "]" << endl;
 cout << "ss4=[" << ss1 << "]" << endl;

 return 0;
 }*/
