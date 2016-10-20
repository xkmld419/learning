/*
* Copyright 2002 LinkAge Co.,Ltd. Inc. All Rights Reversed
*/

/*********************************************************************************/
/*** Name             : BASEFUN.CC                                             ***/
/***                                                                           ***/
/*** Description      : 基本功能函数                                           ***/
/***                                                                           ***/
/*** Author           : 郭亮                       	                       ***/
/***                             	                                       ***/
/*** Begin Time       : 2002/05/26                                             ***/
/***                                                                           ***/
/*** Last Change Time : 2002/05/30       	                               ***/
/***                                                                           ***/
/*********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <ctype.h>

#include "basefun.h"

using namespace std;

extern "C"
{
	int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
}


bool _GetSubRange(const char **Src, char *Range1, char *Range2)
{
	const char *pSrc = *Src;
	const char *p1 = Range1;
	const char *p2 = Range2;

	//解析--subrange1:subrange2
	//前一个range1
	while (*pSrc == ' ' || *pSrc == '\t')
		++pSrc;

	while ( isalnum(*pSrc) )
		*Range1++ = *pSrc++;
	*Range1 = 0;

	while (*pSrc == ' ' || *pSrc == '\t')
		++pSrc;

	if (*pSrc != ':')
		return false;
	else
		++pSrc;

	//后一个range2
	while (*pSrc == ' ' || *pSrc == '\t')
		++pSrc;

	while ( isalnum(*pSrc) )
		*Range2++ = *pSrc++;
	*Range2 = 0;

	while (*pSrc == ' ' || *pSrc == '\t')
		++pSrc;

	if (*p1 == 0 || *p2 == 0)
		return false;

	*Src += pSrc - *Src;

	return true;
}

/*
*	Function Name:	Compare
*	Description:	比较两个字符串是否匹配（相等）
*	Input Param:
*		lv_chCompareString -------> 被比较的字符串
*		lv_chCompareMod	   -------> 匹配的字符串，支持*,?,[]等通配符
*	Returns:
*		如果两个字符串匹配，返回true
*		如果两个字符串不匹配，返回false
*	complete:	2001/12/13
*/
bool Compare(const char *lv_chCompareString,const char *lv_chCompareMod)
{
	while(1)
	{
		switch(*lv_chCompareMod)
	  	{
	  		case '\0':
					if (*lv_chCompareString == '\0')
					{
						return true;
					}
					else
					{
						return false;
					}
			case '%': //掩码:mask_type[mask_value] %R表示范围(range),%R[sub1:sub2, sub3:sub4]
				++lv_chCompareMod;
				if (strncmp(lv_chCompareMod, "R[", 2) == 0)
				{
					int iLen = 0;
					char *p;
					char SubRange1[30];
					char SubRange2[30];

					lv_chCompareMod += 2; //跳过R[
					while (1)
					{
						if (!_GetSubRange(&lv_chCompareMod, SubRange1, SubRange2))
							return false;

						iLen = strlen(SubRange1);
						if (strlen(lv_chCompareString) < (size_t)iLen)
							return false;

						if ( (strncmp(lv_chCompareString, SubRange1, iLen) >= 0) &&
							(strncmp(lv_chCompareString, SubRange2, iLen) <= 0 ) )
						{ //匹配
							p = strchr(const_cast<char *>(lv_chCompareMod), ']');
							if (p)
							{
								//跳过整个模式以及相应长度的字符串
								lv_chCompareMod = p + 1;
								lv_chCompareString += iLen;
								break;
							}
							else
							{
								return false;
							}
						} //测试下一个
						else if (*lv_chCompareMod == ',')
						{
							++ lv_chCompareMod;
							continue;
						}
						else
						{
							return false;
						}
					}
					continue; //外层while
				}
				return false;
			case '!':
					if (Compare(lv_chCompareString,lv_chCompareMod + 1) == true)
					{
						return false;
					}
					else
					{
						return true;
					}
			case '?' :
					if(*lv_chCompareString == '\0')
					{
						return false;
					}
					return Compare(lv_chCompareString + 1,lv_chCompareMod + 1);
			case '*' :
					if(*(lv_chCompareMod+1) == '\0')
					{
						return true;
					}
					do
					{
						if(Compare(lv_chCompareString,lv_chCompareMod+1)==true)
						{
							return true;
						}
					}while(*(lv_chCompareString++));
					return false;
			case '[' :
					lv_chCompareMod++;
					do
					{

						if(*lv_chCompareMod == *lv_chCompareString)
						{
							while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']')
							{
								lv_chCompareMod++;
							}
							if(*lv_chCompareMod == ']')
							{
								lv_chCompareMod++;
							}
							return Compare(lv_chCompareString+1,lv_chCompareMod);
						}
						lv_chCompareMod++;
						if((*lv_chCompareMod == ':') && (*(lv_chCompareMod+1) != ']'))
						{
							if((*lv_chCompareString >= *(lv_chCompareMod - 1)) && (*lv_chCompareString <= *(lv_chCompareMod + 1)))
							{
								while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']')
								{
									lv_chCompareMod++;
								}
								if(*lv_chCompareMod == ']')
								{
									lv_chCompareMod++;
								}
								return Compare(lv_chCompareString+1,lv_chCompareMod);
							}
							lv_chCompareMod++;
							lv_chCompareMod++;

						}
					}while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']');

					return false;
			default  :
					if(*lv_chCompareString == *lv_chCompareMod)
					{
						++lv_chCompareString;
						++lv_chCompareMod;
						continue;
						//return Compare(lv_chCompareString+1,lv_chCompareMod+1);
					}
					else
					{
						return false;
					}
		}//switch
	}//while(1)
}

/************************************************************************
*	Function Name:	Compare
*	Description:	比较两个字符串是否匹配（相等）
*	Input Param:
*		lv_chCompareString -------> 被比较的字符串
*		lv_chCompareMod	   -------> 匹配的字符串，
*																支持*,?,[YYYY,MM,DD,HH,MI,SS](含小写),[NNNN,0,9999]等通配符
*	Returns:
*		如果两个字符串匹配，返回true
*		如果两个字符串不匹配，返回false
*	complete:	2001/12/13
*************************************************************************/
bool Compare(const char *lv_chCompareString,const char *lv_chCompareMod,char *lv_ErrorMsg)
{
	while(1)
	{
		switch(*lv_chCompareMod)
	  	{
	  		case '\0':
					if (*lv_chCompareString == '\0')
					{
						return true;
					}
					else
					{
						return false;
					}
			case '%': //掩码:mask_type[mask_value] %R表示范围(range),%R[sub1:sub2, sub3:sub4]
				++lv_chCompareMod;
				if (strncmp(lv_chCompareMod, "R[", 2) == 0)
				{
					int iLen = 0;
					char *p;
					char SubRange1[30];
					char SubRange2[30];

					lv_chCompareMod += 2; //跳过R[
					while (1)
					{
						if (!_GetSubRange(&lv_chCompareMod, SubRange1, SubRange2))
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
						}
						
						iLen = strlen(SubRange1);
						if (strlen(lv_chCompareString) < (size_t)iLen)
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
						}

						if ( (strncmp(lv_chCompareString, SubRange1, iLen) >= 0) &&
							(strncmp(lv_chCompareString, SubRange2, iLen) <= 0 ) )
						{ //匹配
							p = strchr(const_cast<char *>(lv_chCompareMod), ']');
							if (p)
							{
								//跳过整个模式以及相应长度的字符串
								lv_chCompareMod = p + 1;
								lv_chCompareString += iLen;
								break;
							}
							else
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
						} //测试下一个
						else if (*lv_chCompareMod == ',')
						{
							++ lv_chCompareMod;
							continue;
						}
						else
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
						}
					}
					continue; //外层while
				}
				strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
				return false;
			case '!':
					if (Compare(lv_chCompareString,lv_chCompareMod + 1,lv_ErrorMsg) == true)
					{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
					}
					else
					{
						return true;
					}
			case '?' :
					if(*lv_chCompareString == '\0')
					{
						strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
						return false;
					}
					return Compare(lv_chCompareString + 1,lv_chCompareMod + 1,lv_ErrorMsg);
			case '*' :
					if(*(lv_chCompareMod+1) == '\0')
					{
						return true;
					}
					do
					{
						if(Compare(lv_chCompareString,lv_chCompareMod+1,lv_ErrorMsg)==true)
						{
							return true;
						}
					}while(*(lv_chCompareString++));
					strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
					return false;
			case '[' :
					int i_TempLength,i_Length, i_StartSeq, i_EndSeq, i_SeqLength;
					int i, j; //控制循环次数
					char chTempFormat[LENGTH],chTempFileName[LENGTH];
					char chPreFlag;
					char *pchPos;
					
					memset(chTempFormat,0,LENGTH - 1);
					memset(chTempFileName,0,LENGTH - 1);
					lv_chCompareMod++;
					i_Length = strlen(lv_chCompareMod);
					for (i=0;i<i_Length;i++)
					{
						if(lv_chCompareMod[i] == '[')
						{
							return false;
						}
						else if(lv_chCompareMod[i] == ']')
						{
							break;
						}
					}
					
					strncpy(chTempFormat,lv_chCompareMod,i);
					MakeUpper(chTempFormat);
					chPreFlag = *(lv_chCompareMod - 2);
					
					/*计算文件名长度，从当前位置开始*/
					i_Length = strlen(lv_chCompareString);
//					pchPos = lv_chCompareString;
										
					if ( strcmp(chTempFormat , "YY") == 0 )
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -2;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,2);
									if (CheckYear(chTempFileName))
									{
										return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,2);
								if (CheckYear(chTempFileName))
								{
									return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "YYYY") == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -4;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,4);
									if (CheckYear(chTempFileName))
									{
										return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,4);
								if (CheckYear(chTempFileName))
								{
									return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "MM") == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -2;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,2);
									if (CheckMonth(chTempFileName))
									{
										return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,2);
								if (CheckMonth(chTempFileName))
								{
									return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "DD") == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -2;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,2);
									if (CheckDay(chTempFileName))
									{
										return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (2 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,2);
								if (CheckDay(chTempFileName))
								{
									return Compare(lv_chCompareString + 2,lv_chCompareMod + 2 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "YYMM" ) == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -4;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,4);
									if (CheckYearMonth(chTempFileName))
									{
										return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,4);
								if (CheckYearMonth(chTempFileName))
								{
									return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "YYYYMM" ) == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (6 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -6;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,6);
									if (CheckYearMonth(chTempFileName))
									{
										return Compare(lv_chCompareString + 6,lv_chCompareMod + 6 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (6 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,6);
								if (CheckYearMonth(chTempFileName))
								{
									return Compare(lv_chCompareString + 6,lv_chCompareMod + 6 + 1,lv_ErrorMsg);
								}
							}
						}
						
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "MMDD" ) == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -4;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,4);
									if (CheckMonthDay(chTempFileName))
									{
										return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (4 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,4);
								if (CheckMonthDay(chTempFileName))
								{
									return Compare(lv_chCompareString + 4,lv_chCompareMod + 4 + 1,lv_ErrorMsg);
								}
							}
						}
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "YYMMDD" ) == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (6 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -6;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,6);
									if (CheckDate(chTempFileName))
									{
										return Compare(lv_chCompareString + 6,lv_chCompareMod + 6 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (6 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,6);
								if (CheckDate(chTempFileName))
								{
									return Compare(lv_chCompareString + 6,lv_chCompareMod + 6 + 1,lv_ErrorMsg);
								}
							}
						}
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					else if( strcmp(chTempFormat, "YYYYMMDD" ) == 0)
					{
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (8 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (i=0;i<= i_Length -8;i++)
								{
									strncpy(chTempFileName,lv_chCompareString,8);
									if (CheckDate(chTempFileName))
									{
										return Compare(lv_chCompareString + 8,lv_chCompareMod + 8 + 1,lv_ErrorMsg);
									}
									lv_chCompareString++;
								}
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (8 > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								strncpy(chTempFileName,lv_chCompareString,8);
								if (CheckDate(chTempFileName))
								{
									return Compare(lv_chCompareString + 8,lv_chCompareMod + 8 + 1,lv_ErrorMsg);
								}
							}
						}
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
					}
					/*处理序列号*/
					/*配置规则：[NNNN,0,9999]，各位置说明如下：*/
					/*其中N为序列标识，N字符的个数表示序列号长度,0表示开始序列号，9999表示结束序列号*/
					else if( (strncmp(chTempFormat,"N",1) == 0) && strchr(chTempFormat,',') )
					{
						char chTemp[LENGTH];
						
						memset(chTemp,0,LENGTH-1);
						/*分解序列标识*/
						pchPos = strchr(chTempFormat,',');
						if (pchPos)
						{
							strncpy(chTemp,chTempFormat,pchPos - chTempFormat) ;
						}
						else
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
						}
						/*验证序列号标识是否合法*/
						Trim(chTemp);
						i_SeqLength = strlen(chTemp);
						for (i = 0; i < i_SeqLength; i++)
						{
							if ( 'N' != chTemp[i] )
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_FILTE_RULE);
								return false;
							}
						}
						
						/*分解开始序列号*/
						pchPos = strchr(chTempFormat + i_SeqLength + 1,',');
						if (pchPos)
						{
							memset(chTemp,0,sizeof(chTemp)-1);
							strncpy(chTemp,chTempFormat+i_SeqLength + 1,pchPos - chTempFormat - i_SeqLength - 1) ;
						}
						else
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
							return false;
						}
						
						/*验证开始序列号是否合法*/
						Trim(chTemp);
						i_TempLength = strlen(chTemp);
						for ( i = 0 ; i < i_TempLength; i++)
						{
							if (chTemp[i] < '0' || chTemp[i] > '9')
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_SEQUENCE_DEF);
								return false;
							}
						}
						/*验证开始序列号长度是否合法*/
						if (i_TempLength > i_SeqLength )
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_SEQUENCE_DEF);
							return false;
						}
						i_StartSeq = atoi(chTemp);
						
						/*验证结束序列号是否合法*/
						pchPos++;
						Trim(pchPos);
						i_TempLength = strlen(pchPos);
						
						
						for ( i = 0 ; i < i_TempLength; i++)
						{
							if (pchPos[i] < '0' || pchPos[i] > '9')
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_SEQUENCE_DEF);
								return false;
							}
						}
						
						/*验证结束序列号长度是否合法*/
						if (i_TempLength > i_SeqLength )
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_SEQUENCE_DEF);
							return false;
						}
						
						i_EndSeq = atoi(pchPos);
						if (i_EndSeq < i_StartSeq)
						{
							strcpy(lv_ErrorMsg,ERROR_FILE_SEQUENCE_DEF);
							return false;
						}
						
						/*考虑[前面是否星号，如果为星号，则从当前位置开始向后进行匹配，每次后移一位*/
						if (chPreFlag == '*')
						{
							if (i_SeqLength > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								/*验证文件名中序列号是否为数字*/
								for (i=0;i < i_Length -i_SeqLength;i++)
								{
									for (j = 0; j < i_SeqLength; j++)
									{
										if (lv_chCompareString[j] < '0' || lv_chCompareString[j] > '9')
										{
											break;
										}
									}
									
									/*验证文件名中指定位置是否为合法序列号*/
									if ( j == i_SeqLength )
									{
										memset(chTemp,0,sizeof(chTemp));
										strncpy(chTemp,lv_chCompareString,i_SeqLength);
										if(atoi(chTemp) < i_StartSeq || atoi(chTemp) > i_EndSeq)
										{
											lv_chCompareString++;
											continue;
										}
										else
										{
											return Compare(lv_chCompareString + i_SeqLength,lv_chCompareMod + strlen(chTempFormat) + 1,lv_ErrorMsg);
										}
									}
									lv_chCompareString++;
								}
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
						}
						else
						{
							/*不含星号的，则从当前位置开始匹配，且不后移*/
							if (i_SeqLength > i_Length)
							{
								strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
								return false;
							}
							else
							{
								for (j = 0; j < i_SeqLength; j++)
								{
									if (lv_chCompareString[j] < '0' || lv_chCompareString[j] > '9')
									{
										strcpy(lv_ErrorMsg,ERROR_SERIAL_NUMBER);
										return false;
									}
								}
								
								memset(chTemp,0,sizeof(chTemp));
								strncpy(chTemp,lv_chCompareString,i_SeqLength);
								if(atoi(chTemp) < i_StartSeq || atoi(chTemp) > i_EndSeq)
								{
									strcpy(lv_ErrorMsg,ERROR_SERIAL_NUMBER);
									return false;
								}
								return Compare(lv_chCompareString + i_SeqLength,lv_chCompareMod + strlen(chTempFormat) + 1,lv_ErrorMsg);
							}
						}
						/******caihua 2007/8/16 12:46:25 "statement is unreachable" 
						strcpy(lv_ErrorMsg,ERROR_DATE_FORMAT);
						return false;
						*/
					}
					else
					{		
						do
						{
	
							if(*lv_chCompareMod == *lv_chCompareString)
							{
								while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']')
								{
									lv_chCompareMod++;
								}
								if(*lv_chCompareMod == ']')
								{
									lv_chCompareMod++;
								}
								return Compare(lv_chCompareString+1,lv_chCompareMod,lv_ErrorMsg);
							}
							lv_chCompareMod++;
							if((*lv_chCompareMod == ':') && (*(lv_chCompareMod+1) != ']'))
							{
								if((*lv_chCompareString >= *(lv_chCompareMod - 1)) && (*lv_chCompareString <= *(lv_chCompareMod + 1)))
								{
									while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']')
									{
										lv_chCompareMod++;
									}
									if(*lv_chCompareMod == ']')
									{
										lv_chCompareMod++;
									}
									return Compare(lv_chCompareString+1,lv_chCompareMod,lv_ErrorMsg);
								}
								lv_chCompareMod++;
								lv_chCompareMod++;
	
							}
						}while(*lv_chCompareMod != '\0' && *lv_chCompareMod != ']');
					}
					strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
					return false;
			default  :
					if(*lv_chCompareString == *lv_chCompareMod)
					{
						++lv_chCompareString;
						++lv_chCompareMod;
						continue;
						//return Compare(lv_chCompareString+1,lv_chCompareMod+1,lv_ErrorMsg);
					}
					else
					{
						strcpy(lv_ErrorMsg,ERROR_FILE_NAME_FORMAT);
						return false;
					}
		}//switch
	}//while(1)
}

/*
*	Function Name:	GetSystemDateTime
*	Description:	获得当前系统日期时间
*	Input Param:
*		CurrentDateTime -------> 当前系统日期(格式14位YYYYMMDDHHMMSS)
*	Returns:	无
*	complete:	2003/06/30
*/
void GetSystemDateTime(char *CurrentDateTime)
{
	time_t timer;
	struct tm *today;

	timer = time(NULL);
	today = localtime(&timer);
	sprintf(CurrentDateTime,"%04d%02d%02d%02d%02d%02d",today->tm_year + 1900,
				today->tm_mon+1,today->tm_mday,
				today->tm_hour,today->tm_min,today->tm_sec);
}

/*
*	Function Name:	GetShortSystemDateTime
*	Description:	获得当前系统日期时间
*	Input Param:
*		CurrentDateTime -------> 当前系统日期(格式12位YYMMDDHHMMSS)
*	Returns:	无
*	complete:	2003/06/30
*/
void GetShortSystemDateTime(char *CurrentDateTime)
{
	time_t timer;
	struct tm *today;

	timer = time(NULL);
	today = localtime(&timer);
	sprintf(CurrentDateTime,"%02d%02d%02d%02d%02d%02d",today->tm_year,
				today->tm_mon+1,today->tm_mday,
				today->tm_hour,today->tm_min,today->tm_sec);
}

/*
*	Function Name:	GetSystemDate
*	Description:	获得当前系统日期
*	Input Param:
*		CurrentDate -------> 当前系统日期(格式yyyymmdd)
*	Returns:	无
*	complete:	2001/12/07
*/
void GetSystemDate(char *CurrentDate)
{
	time_t timer;
	struct tm *today;

	timer = time(NULL);
	today = localtime(&timer);
	sprintf(CurrentDate,"%04d%02d%02d",today->tm_year + 1900,today->tm_mon+1,today->tm_mday);
}

/*
*	Function Name:	GetSystemTime
*	Description:	获得当前系统时间
*	Input Param:
*		CurrentTime -------> 当前系统时间(格式yymmdd)
*	Returns:	无
*	complete:	2001/12/07
*/
void GetSystemTime(char *CurrentTime)
{
	time_t timer;
	struct tm *timenow;

	timer = time(NULL);
	timenow = localtime(&timer);
	sprintf(CurrentTime,"%02d%02d%02d",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
}

/*
*	Function Name:	GetLocalIp
*	Description:	得到本机的IP地址
*	Input Param:
*		LocalIp -------> 得到的本机IP地址
*	Returns:	无
*	complete:	2002/03/13
*/
bool GetLocalIp(char *LocalIp)
{
	char chHostName[LENGTH];
	int i;
	struct in_addr addr;
	/*取得机器名称*/
	if(gethostname(chHostName,sizeof(chHostName)) !=-1)
	{
		/*取得给定机器的信息*/
		struct hostent* phost=gethostbyname(chHostName);
		/*到每一个地址结尾*/
		for(i=0;phost!= NULL&&phost->h_addr_list[i]!=NULL;i++)
		{
        		memcpy(&addr, phost->h_addr_list[i], sizeof(struct in_addr));
    		}
    		strcpy(LocalIp,inet_ntoa(addr));
    		return true;
	}
	else
	{
		return false;
	}
}

/*
*	Function Name:	FullPath
*	Description:	给出完整的路径名
*	Input Param:
*		chPath -------> 路径名
*	Returns:	如果路径存在，则补齐,并返回0;不存在则返回-1
*	complete:	2002/03/13
*/
int FullPath(char *chPath)
{
	int len;

	if(chPath[0] == '\0')
	{
		return -1;
	}

	len = strlen(chPath);
	if(chPath[len - 1] != '/')
	{
		chPath[len] = '/';
		chPath[len + 1] = '\0';
	}

	return 0;
}

/*
*    Function Name    : IsDirectory
*    Description      : 判断是否路径存在
*    Input Param :
*		dirname -------> 需要进行判断的路径名
*    Returns          : 如果是路径名，则返回true
*			如果不是路径名，则返回false
*/
bool IsDirectory(const char *dirname)
{
	struct stat sDir;

	if (stat(dirname,&sDir) < 0)
	{
		return false;
	}

	if (S_IFDIR == (sDir.st_mode & S_IFMT))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
*	Function Name:	GetFileSize
*	Description:	获取文件的大小
*	Input Param:
*		chFileName -------> 文件的名字
*	Returns:	无
*	complete:	2002/03/13
*/
long GetFileSize(const char *chFileName)
{
	struct stat buf;

	if(stat(chFileName, &buf) < 0)
	{
		return -1;
	}
	return buf.st_size;
}

/*
*	Function Name:	Trim
*	Description:	去掉字符串左右的空格
*	Input Param:
*		String -------> 需要去掉空格的字符串
*	Returns:	无
*	complete:	2002/03/13
*/
void Trim(char * String)
{
	char *Position = String;
	/*找到第一个不是空格的位置*/
	while(isspace(*Position))
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
	}while(isspace(*String));
}


/*
*    Function Name    : GetLine
*    Description      :	在文件中获得一行的内容
*    Input lv_chParameters :
*	Line        -----------> 这一行的内容（字符串）
*	File        -----------> 打开的文件指针
*	Returns          :得到的字节数.
*	complete	:2001/11/09
*/
int GetLine(char *Line,FILE *File)
{
	int iByteRead = 0;
	for(;;)
	{
		if (feof(File))
		{
			*Line='\0';
			break;
		}
		*Line=fgetc(File);
		iByteRead ++;
		if (*Line == '\n')
		{
			*Line='\0';
			break;
		}
		Line++;
	}
	return iByteRead;
}

/*
*	Function Name:	MakeUpper
*	Description:	将字符串全部改为大写
*	Input Param:
*		pchString -------> 输入字符串
*	Returns:	无
*	complete:	2002/03/13
*/
void MakeUpper(char * pchString)
{
	int iLen = strlen(pchString);

	for(int i=0; i<iLen; i++)
	{
		pchString[i] = toupper(pchString[i]);
	}
}

/*
*	Function Name:	MakeLower
*	Description:	将字符串全部改为小写
*	Input Param:
*		pchString -------> 输入字符串
*	Returns:	无
*	complete:	2002/03/13
*/
void MakeLower(char * pchString)
{
	int iLen = strlen(pchString);

	for(int i=0; i<iLen; i++)
	{
		pchString[i] = tolower(pchString[i]);
	}
}
/*
*	Function Name:	CheckYear
*	Description:	校验年份正确性
*	Input Param:
*		pchString -------> 输入字符串(格式yyyy或者yy)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2007/04/10
*/
bool CheckYear(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int iYear = 0,i;

	iLen = strlen(pchString);
	if ((iLen != 4) && (iLen != 2))
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	if (iLen == 2)
	{
		//校验年份
		strncpy(tmp, pchString, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		iYear = num + 2000;
		if ((num < 0) || (num > 99))
		{
			return false;
		}
	}
	else if (iLen == 4)
	{
		strncpy(tmp, pchString, 4);
		tmp[4] = '\0';
		num = atoi(tmp);
		iYear = num;
		num = num - 2000;
		if ((num <0) || (num > 99))
		{
			return false;
		}
	}
	return true;
}

/*
*	Function Name:	CheckYearMonth
*	Description:	校验日期正确性
*	Input Param:
*		pchString -------> 输入字符串(格式yyyymm或者yymm)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2002/03/13
*/
bool CheckYearMonth(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int iYear = 0,iMonth = 0,i;

	iLen = strlen(pchString);
	if ((iLen != 4) && (iLen != 6))
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	if (iLen == 4)
	{
		//校验年份
		strncpy(tmp, pchString, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		iYear = num + 2000;
		if ((num < 0) || (num > 99))
		{
			return false;
		}
		//校验月份
		strncpy(tmp, pchString + 2, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		iMonth = num;
		if ((num < 1) || (num > 12))
		{
			return false;
		}
	}
	else if (iLen == 6)
	{
		strncpy(tmp, pchString, 4);
		tmp[4] = '\0';
		num = atoi(tmp);
		iYear = num;
		num = num - 2000;
		if ((num <0) || (num > 99))
		{
			return false;
		}
		//校验月份
		strncpy(tmp, pchString + 4, 4);
		tmp[2] = 0;
		num = atoi(tmp);
		iMonth = num;
		if ((num < 1) || (num > 12))
		{
			return false;
		}
	}
	
	return true;
}



/*
*	Function Name:	CheckMonth
*	Description:	校验月份正确性
*	Input Param:
*		pchString -------> 输入字符串(格式mm或者m)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2007/04/10
*/
bool CheckMonth(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int iMonth = 0,i;

	iLen = strlen(pchString);
	if ((iLen != 1) && (iLen != 2))
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	strcpy(tmp, pchString);
	
//	if (iLen == 1)
//	{
//		tmp[1] = 0;
//	}
//	else if (iLen == 2)
//	{
//		tmp[2] = 0;
//	}

	num = atoi(tmp);
	iMonth = num;
	if ((num < 1) || (num > 12))
	{
		return false;
	}

	return true;
}


/*
*	Function Name:	CheckDay
*	Description:	校验日期正确性
*	Input Param:
*		pchString -------> 输入字符串(格式d或者dd)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2007/04/10
*/
bool CheckDay(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int i;

	iLen = strlen(pchString);
	if ((iLen != 1) && (iLen != 2))
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

		strcpy(tmp, pchString);
	
//	if (iLen == 1)
//	{
//		tmp[1] = 0;
//	}
//	else if (iLen == 2)
//	{
//		tmp[2] = 0;
//	}

	num = atoi(tmp);
	if ((num < 1) || (num > 31))
	{
		return false;
	}

	return true;
}

/*
*	Function Name:	CheckMonthDay
*	Description:	校验日期正确性
*	Input Param:
*		pchString -------> 输入字符串(格式mmdd)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2007/04/10
*/
bool CheckMonthDay(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int iMonth = 0,i;

	iLen = strlen(pchString);
	if (iLen != 4)
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	//校验月份
	strncpy(tmp, pchString, 2);
	tmp[2] = 0;
	num = atoi(tmp);
	iMonth = num;
	if ((num < 1) || (num > 12))
	{
		return false;
	}
	//校验天
	strncpy(tmp, pchString + 2, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		if ((num < 1) || (num > 31))
		{
			return false;
		}

	switch(iMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if ((num < 1) || (num > 31))
			{
				return false;
			}
			break;
		case 2:
			if ((num < 1) || (num > 29))
			{
				return false;
			}
			break;
		default :
			if((num < 1) || (num > 30))
			{
				return false;
			}
	}

	return true;
}

/*
*	Function Name:	CheckDate
*	Description:	校验日期正确性
*	Input Param:
*		pchString -------> 输入字符串(格式yyyymmdd或者yymmdd)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2002/03/13
*/
bool CheckDate(const char *pchString)
{
	int num = 0,iLen;
	char tmp[LENGTH];
	int iYear = 0,iMonth = 0,i;

	iLen = strlen(pchString);
	if ((iLen != 6) && (iLen != 8))
	{
		return false;
	}

	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	if (iLen == 6)
	{
		//校验年份
		strncpy(tmp, pchString, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		iYear = num + 2000;
		if ((num < 0) || (num > 99))
		{
			return false;
		}
		//校验月份
		strncpy(tmp, pchString + 2, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		iMonth = num;
		if ((num < 1) || (num > 12))
		{
			return false;
		}

		//校验天
		strncpy(tmp, pchString + 4, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		if ((num < 1) || (num > 31))
		{
			return false;
		}
	}
	else if (iLen == 8)
	{
		strncpy(tmp, pchString, 4);
		tmp[4] = '\0';
		num = atoi(tmp);
		iYear = num;
		num = num - 2000;
		if ((num <0) || (num > 99))
		{
			return false;
		}
		//校验月份
		strncpy(tmp, pchString + 4, 4);
		tmp[2] = 0;
		num = atoi(tmp);
		iMonth = num;
		if ((num < 1) || (num > 12))
		{
			return false;
		}
		//校验天
		strncpy(tmp, pchString + 6, 2);
		tmp[2] = 0;
		num = atoi(tmp);
		if ((num < 1) || (num > 31))
		{
			return false;
		}
	}

	switch(iMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if ((num < 1) || (num > 31))
			{
				return false;
			}
			break;
		case 2:
			//判断闰年
			if (((iYear%4 == 0) && (iYear%100 != 0)) || (iYear%400 == 0))
			{
				if ((num < 1) || (num > 29))
				{
					return false;
				}
			}
			else
			{
				if ((num < 1) || (num > 28))
				{
					return false;
				}
			}
			break;
		default :
			if((num < 1) || (num > 30))
			{
				return false;
			}
	}

	return true;
}

/*
*	Function Name:	CheckTime
*	Description:	校验时间正确性
*	Input Param:
*		pchString -------> 输入字符串(格式hhmmss或者为hhmmsss最后一位是十分之一秒)
*	Returns:
*		成功，返回true
*		失败，返回false
*	Complete:	2002/03/13
*	Update:		2003/01/25
*/
bool CheckTime(const char *pchString)
{
	int num,i;
	char tmp[LENGTH];

	if ((strlen(pchString) != 6) && (strlen(pchString) != 7))
	{
		return false;
	}

	int iLen = strlen(pchString);
	for (i=0; i<iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
		{
			return false;
		}
	}

	//校验小时
	strncpy(tmp, pchString, 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if ((num < 0) || (num > 23))
	{
		return false;
	}

	//校验分钟
	strncpy(tmp, pchString + 2, 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if ((num < 0) || (num > 59))
	{
		return false;
	}

	//校验秒
	strncpy(tmp, pchString + 4, 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if ((num < 0) || (num > 59))
	{
		return false;
	}
	//校验十分之一秒
	if(strlen(pchString) == 7)
	{
		strncpy(tmp, pchString + 6, 1);
		tmp[1] = 0;
		num = atoi(tmp);
		if((num < 0) || (num > 9))
		{
			return false;
		}
	}
	return true;
}
/*
*	Function Name:	CheckDateTime
*	Description:	校验日期正确性
*	Input Param:
*		pchString -------> 输入字符串(格式yyyymmddhhmmss)
*	Returns:
*		成功，返回true
*		失败，返回false
*	complete:	2004/07/23
*/
bool CheckDateTime(const char *pchString)
{
	char szTemp[8 + 1];
	memset(szTemp, 0, sizeof(szTemp));

	memcpy(szTemp, pchString, 8);

	return (CheckDate(szTemp) && CheckTime(pchString + 8));
}

/*
*	Function Name:	hhmmssToss
*	Description:	时分秒转换为秒
*	Input Param:
*		pchString -------> 输入字符串(格式为hhmmss)
*	Returns:
*		Duration -------> 秒数
*	complete:	2002/03/13
*/
long hhmmssToss(char * pchString)
{
	long Duration;
	char tmp[LENGTH];

	//小时转换为秒
	memcpy(tmp,pchString,2);
	tmp[2] = 0;
	Duration = 3600*atoi(tmp);

	//分钟转换为秒
	memcpy(tmp,pchString+2,2);
	tmp[2] = 0;
	Duration += 60*atoi(tmp);

	//累加秒
	memcpy(tmp,pchString+4,2);
	tmp[2] = 0;
	Duration += atoi(tmp);

	return Duration;
}




/*
*    Function Name    : SearchOneFile
*    Description      :	判断所给的匹配模式在所给的目录下有相应的文件存在
*    Input lv_chParameters :
*	pathname    -----------> 路径名
*	Format      -----------> 匹配模式
*	Filename    -----------> 匹配成功的文件名
*	Returns          : 如果该路径下该文件存在，返回FIND_OK
*			   如果该路径下该文件不存在，返回FIND_NOFILE
*			   如果出现路径不能打开，返回FIND_PATHERROR
*			   如果出现找到的文件不能访问，返回FIND_NOREAD
*	complete	:2001/12/06
*/
int SearchOneFile(const char *Pathname, const char *Format, char *Filename)
{
	DIR *dirp = NULL;
	struct dirent entry;
	struct dirent temp;
	struct dirent *dp = &temp;
	char chFileName[LENGTH];
	char chFullName[LENGTH];
	char chTmpPath[LENGTH];

	memset(chFileName, 0, sizeof(chFileName));
	memset(chTmpPath, 0, sizeof(chTmpPath));

	strcpy(chTmpPath, Pathname);

	FullPath(chTmpPath);
	dirp = opendir(chTmpPath);
	if (dirp == NULL)
	{
		sprintf(Filename, "open dir %s FAILED", Pathname);
		return FIND_PATHERROR;
	}

	while (readdir_r(dirp, &entry, &dp) == 0)
	{
		if (dp == NULL)
		{
			closedir(dirp);
			return FIND_NOFILE;
		}
		strcpy(chFileName, dp->d_name);
		sprintf(chFullName, "%s%s", chTmpPath, chFileName);

		if ((strcmp(chFileName, ".") == 0) || (strcmp(chFileName, "..") == 0))
		{
			continue;
		}
		if (IsDirectory(chFullName) == true)
		{
			continue;
		}
		if (chFileName[0] == '!')
		{
			continue;
		}
		if (Compare(chFileName, Format) == true)
		{
			strcpy(Filename, chFileName);
			closedir(dirp);
			if (access(chFullName, F_OK|R_OK) == -1)
			{
				return FIND_NOREAD;
			}
			return FIND_OK;
		}
	}
	closedir(dirp);
	return FIND_NOFILE;
}

/*
*    Function Name    : SearchOneFile
*    Description      :	判断所给的匹配模式在所给的目录下有相应的文件存在
*    Input lv_chParameters :
*	pathname    -----------> 路径名
*	Format      -----------> 匹配模式
* IgnoreFlag  -----------> 过滤标识，以该字符开头的文件将不会提取(可以取0或ASCII值在33和126之间的可视字符,如果取0则不过滤文件)
*	Filename    -----------> 匹配成功的文件名
*	Returns          : 如果该路径下该文件存在，返回FIND_OK
*			   						 如果该路径下该文件不存在，返回FIND_NOFILE
*										 如果出现路径不能打开，返回FIND_PATHERROR
*			   						 如果出现找到的文件不能访问，返回FIND_NOREAD
*										 如果过滤标识为非可视字符，返回FIND_IGNOREERROR
* 其它说明：为了防止IgnoreFlag使用(char)型与(char *)型冲突，故该参数类型选择int型
*	complete	:2007/11/08
*/
int SearchOneFile(const char *Pathname, const char *Format, const int IgnoreFlag, char *Filename)
{
	DIR *dirp = NULL;
	struct dirent entry;
	struct dirent temp;
	struct dirent *dp = &temp;
	char chFileName[LENGTH];
	char chFullName[LENGTH];
	char chTmpPath[LENGTH];

	memset(chFileName, 0, sizeof(chFileName));
	memset(chTmpPath, 0, sizeof(chTmpPath));

	/*判断IgnoreFlag是否为可视字符*/
	if ( IgnoreFlag != 0  && ( IgnoreFlag < 33 || IgnoreFlag > 126 ))
	{
		return FIND_IGNOREERROR;
	}

	strcpy(chTmpPath, Pathname);
	
	FullPath(chTmpPath);
	dirp = opendir(chTmpPath);
	if (dirp == NULL)
	{
		sprintf(Filename, "open dir %s FAILED", Pathname);
		return FIND_PATHERROR;
	}
	
	while (readdir_r(dirp, &entry, &dp) == 0)
	{
		if (dp == NULL)
		{
			closedir(dirp);
			return FIND_NOFILE;
		}
		strcpy(chFileName, dp->d_name);
		sprintf(chFullName, "%s%s", chTmpPath, chFileName);

		if ((strcmp(chFileName, ".") == 0) || (strcmp(chFileName, "..") == 0))
		{
			continue;
		}
		if (IsDirectory(chFullName) == true)
		{
			continue;
		}
		if (chFileName[0] == IgnoreFlag)
		{
			continue;
		}
		if (Compare(chFileName, Format) == true)
		{
			strcpy(Filename, chFileName);
			closedir(dirp);
			if (access(chFullName, F_OK|R_OK) == -1)
			{
				return FIND_NOREAD;
			}
			return FIND_OK;
		}
	}
	closedir(dirp);
	return FIND_NOFILE;
}


/*
*    Function Name    : SearchOneFile
*    Description      :	判断所给的匹配模式在所给的目录下有相应的文件存在
*    Input lv_chParameters :
*	pathname    -----------> 路径名
*	Format      -----------> 匹配模式
*	Filename    -----------> 匹配成功的文件名
* ErrorMsg    -----------> 如果匹配不成功，则返回指定的错误信息
*	Returns          : 如果该路径下该文件存在，返回FIND_OK
*			   如果该路径下该文件不存在，返回FIND_NOFILE
*			   如果出现路径不能打开，返回FIND_PATHERROR
*			   如果出现找到的文件不能访问，返回FIND_NOREAD
*	complete	:2001/12/06
*/
int SearchOneFile(const char *Pathname, const char *Format, char *Filename, char *ErrorMsg)
{
	DIR *dirp = NULL;
	struct dirent entry;
	struct dirent temp;
	struct dirent *dp = &temp;
	char chFileName[LENGTH];
	char chFullName[LENGTH];
	char chTmpPath[LENGTH];

	memset(chFileName, 0, sizeof(chFileName));
	memset(chTmpPath, 0, sizeof(chTmpPath));

	strcpy(chTmpPath, Pathname);

	FullPath(chTmpPath);
	dirp = opendir(chTmpPath);
	if (dirp == NULL)
	{
		sprintf(Filename, "open dir %s FAILED", Pathname);
		return FIND_PATHERROR;
	}

	while (readdir_r(dirp, &entry, &dp) == 0)
	{
		if (dp == NULL)
		{
			closedir(dirp);
			return FIND_NOFILE;
		}
		strcpy(chFileName, dp->d_name);
		sprintf(chFullName, "%s%s", chTmpPath, chFileName);

		if ((strcmp(chFileName, ".") == 0) || (strcmp(chFileName, "..") == 0))
		{
			continue;
		}
		if (IsDirectory(chFullName) == true)
		{
			continue;
		}
		if (chFileName[0] == '!')
		{
			continue;
		}
		if (Compare(chFileName, Format, ErrorMsg) == true)
		{
			strcpy(Filename, chFileName);
			closedir(dirp);
			//if (access(chFullName, F_OK|R_OK) == -1)
			//{
			//	return FIND_NOREAD;
			//}
			return FIND_OK;
		}
		else
		{
			/*由原来SearchOneFile修改而来，为了保证只读取一个文件，此处增加break*/
			strcpy(Filename, chFileName);
			break;
		}
	}
	closedir(dirp);
	return FIND_NOFILE;
}


/*
*    Function Name    : SearchOneFile
*    Description      :	判断所给的匹配模式在所给的多个目录下有相应的文件存在
*    Input lv_chParameters :
*	pathname    -----------> 路径名
*	Format      -----------> 匹配模式
*	Filename    -----------> 匹配成功的文件名
*	Returns          : 如果该路径下该文件存在，返回正整数
*			   如果该路径下该文件不存在，返回0
*			   如果出现异常，返回负整数
*	complete	:2003/01/07
*/
int SearchOneFile(const char Pathname[TOTAL][LENGTH], const char Format[TOTAL][LENGTH], const int TotalIn, char *Filename)
{
	int iCount = 0;
	int iReturn = 0;

	for (iCount=0; iCount<TotalIn; iCount++)
	{
		if ((iReturn = SearchOneFile(Pathname[iCount], Format[iCount], Filename)) > 0)
		{
			return 1;
		}
		else if (0 == iReturn)
		{
			continue;
		}
		else
		{
			return iReturn;
		}
	}
	return 0;
}


bool SearchAllFiles(const char *Pathname, const char *Format, vector<string> &vecFileSet)
{
	DIR *dirp = 0;
	struct dirent entry;
	struct dirent temp;
	struct dirent *dp = &temp;
	char chFileName[LENGTH];
	char chFullName[LENGTH];
	char chTmpPath[LENGTH];
	string sFileName;

	memset(chFileName, 0, sizeof(chFileName));
	memset(chTmpPath, 0, sizeof(chTmpPath));

	strcpy(chTmpPath, Pathname);

	FullPath(chTmpPath);
	dirp = opendir(chTmpPath);
	if (dirp == NULL)
	{
		return false;
	}

	while (readdir_r(dirp, &entry, &dp) == 0)
	{
		if (dp == NULL)
		{
			closedir(dirp);
			return true;
		}
		strcpy(chFileName, dp->d_name);
		sprintf(chFullName, "%s%s", chTmpPath, chFileName);

		if ((strcmp(chFileName, ".") == 0) || (strcmp(chFileName, "..") == 0))
		{
			continue;
		}
		if (IsDirectory(chFullName) == true)
		{
			continue;
		}
		if (chFileName[0] == '~')
		{
			continue;
		}
		if (Compare(chFileName, Format) == true)
		{
			sFileName = chFileName;
			vecFileSet.push_back(sFileName);
		}
	}//end while (readdir_r(dirp, &entry, &dp) == 0)
	closedir(dirp);
	return true;
}

/*
*  		 		Function Name : SearchAllFiles
*    				Description :	从指定上当下描述指定条件的文件列表，并按ASCII表顺序排序
* Input lv_chParameters :
*	　　　　　　　Pathname    -----------> 目录
*									Format    -----------> 文件名掩码，按正则表达式格式使用
*					Return values :
*							lstFileSet    ----------->返回文件列表（排序后）
*	complete	:2008/05/11
*/
bool SearchAllFiles(const char *Pathname, const char *Format, list<string> &lstFileSet)
{
	DIR *dirp = 0;
	struct dirent entry;
	struct dirent temp;
	struct dirent *dp = &temp;
	char chFileName[LENGTH];
	char chFullName[LENGTH];
	char chTmpPath[LENGTH];
	string sFileName;

	memset(chFileName, 0, sizeof(chFileName));
	memset(chTmpPath, 0, sizeof(chTmpPath));

	strcpy(chTmpPath, Pathname);

	FullPath(chTmpPath);
	dirp = opendir(chTmpPath);
	if (dirp == NULL)
	{
		return false;
	}

	while (readdir_r(dirp, &entry, &dp) == 0)
	{
		if (dp == NULL)
		{
			closedir(dirp);
			break;
		}
		strcpy(chFileName, dp->d_name);
		sprintf(chFullName, "%s%s", chTmpPath, chFileName);

		if ((strcmp(chFileName, ".") == 0) || (strcmp(chFileName, "..") == 0))
		{
			continue;
		}
		if (IsDirectory(chFullName) == true)
		{
			continue;
		}
		if (chFileName[0] == '~')
		{
			continue;
		}
		if (Compare(chFileName, Format) == true)
		{
			sFileName = chFileName;
			lstFileSet.push_back(sFileName);
		}
	}//end while (readdir_r(dirp, &entry, &dp) == 0)
	closedir(dirp);
	lstFileSet.sort();
	return true;
}


/*
*    Function Name    : CopyFile
*    Description      :	将一个文件从源目录拷贝到目标目录并改名
*    Input lv_chParameters :
*	DesFullFileName    -----------> 源目录＋源文件名
*	SrcFullFileName    -----------> 目标目录＋目标文件名
*	Returns          : 如果源文件不存在，返回-1
*			   如果不能创建目标文件，返回-2
*			   如果文件流出现异常，返回-3
*			   未知的错误，返回-4
*			   正确则返回0
*	complete	:2003/01/06
*/
int CopyFile(const char *SrcFullFileName, const char *DesFullFileName)
{
	int iReturn = 0;

	if (-1 == access(SrcFullFileName, F_OK|R_OK))
	{
		return -1;
	}
	if ((0 == access(DesFullFileName, F_OK|R_OK)) && strcmp(SrcFullFileName, DesFullFileName))
	{
		if (-1 == unlink(DesFullFileName))
		{
			return -4;
		}
	}

	iReturn = link(SrcFullFileName, DesFullFileName);

	if (iReturn == 0)
	{
		return 0;
	}

	else if ((iReturn == -1) && (errno == EXDEV))
	{
		FILE *fSrc = NULL;
		FILE *fDes = NULL;
		char chTemp[BUFFSIZE];

		if (NULL == (fSrc = fopen(SrcFullFileName, "r")))
		{
			return -1;
		}

		if (NULL == (fDes = fopen(DesFullFileName, "w+")))
		{
			fclose(fSrc);
			return -2;
		}

		for(;;)
		{
			memset(chTemp, 0, sizeof(chTemp));
			iReturn  = 0;

			if (BUFFSIZE == (iReturn = fread(chTemp, sizeof(char), BUFFSIZE, fSrc)))
			{
				fwrite(chTemp, sizeof(char), BUFFSIZE, fDes);
			}
			else
			{
				if(feof(fSrc))
				{
					fwrite(chTemp, sizeof(char), iReturn, fDes);
					break;
				}
				if(ferror(fSrc))
				{
					fclose(fSrc);
					fclose(fDes);
					return -3;
				}
			}
		}

		fclose(fSrc);
		fclose(fDes);

		return 0;
	}
	else
	{
		return -4;
	}
}

/*
*    Function Name    : MoveFile
*    Description      :	将一个文件从源目录移动到目标目录并改名
*    Input lv_chParameters :
*	DesFullFileName    -----------> 源目录＋源文件名
*	SrcFullFileName    -----------> 目标目录＋目标文件名
*	Returns          : 如果源文件不存在，返回-1
*			   如果不能创建目标文件，返回-2
*			   如果文件流出现异常，返回-3
*			   未知的错误，返回-4
*			   如果不能删除源文件，返回-5
*			   正确则返回0
*	complete	:2003/01/06
*/
int MoveFile(const char *SrcFullFileName, const char * DesFullFileName)
{
	int iReturn = 0;
	iReturn = CopyFile(SrcFullFileName, DesFullFileName);
	if (0 == iReturn)
	{
		if (0 != unlink(SrcFullFileName))
		{
			return -5;
		}
	}
	return iReturn;
}

/*
*    Function Name    : GetFileRows
*    Description      :	得到文件的行数
*    Input lv_chParameters :
*	pchFullFileName    -----------> 带路径的文件名
*	Returns          : 如果出错，返回-1
*			   正确则返回文件的行数
*	complete	:2003/01/06
*/
long GetFileRows(const char *pchFullFileName)
{
	FILE *fp = NULL;
	char chCommand[BUFFSIZE];
	char chTemp[BUFFSIZE];
	long lRows = 0;

	if (pchFullFileName[0] == '\0')
	{
		return -1;
	}
	if (access(pchFullFileName, F_OK|R_OK) == -1)
	{
		return -1;
	}

	sprintf(chCommand, "wc -l %s | awk '{print $1}'", pchFullFileName);

	if (NULL == (fp = popen(chCommand, "r")))
	{
		return -1;
	}

	if (NULL == fgets(chTemp, sizeof(chTemp), fp))
	{
		pclose(fp);
		return -1;
	}

	pclose(fp);

	lRows = atol(chTemp);

	if (lRows < 0)
	{
		return -1;
	}

	return lRows;
}

/*
*	Function Name:	GetShortSystemDate
*	Description:	获得当前系统日期(两位年)
*	Input Param:
*		CurrentDate -------> 当前系统日期(格式yymmdd)
*	Returns:	无
*	complete:	2003/01/24
*/
void GetShortSystemDate(char *pchCurrentDate)
{
	char chTempDate[LENGTH];
	memset(chTempDate, 0, sizeof(chTempDate));

	GetSystemDate(chTempDate);

	strcpy(pchCurrentDate, chTempDate + 2);

	return;
}

/*
*	Function Name:	GetShortProcessName
*	Description:	如果进程命令行是绝对路径则获得进程的短名称,如果不是则保持输入参数的原状
*	Input Param:
*		pchFullProcName -------> 进程的命令行
*	Returns:	无
*	complete:	2003/03/18
*/
void GetShortProcName(char *pchProcName)
{
	//此变量用来截取不带路径的执行文件名
	char *pchTemp = 0;
	pchTemp = strrchr(pchProcName, '/');

	char chShortName[LENGTH];
	memset(chShortName, 0, sizeof(chShortName));

	if(0 == pchTemp)
	{
		Trim(pchProcName);
	}
	else
	{
		pchTemp++;
		strcpy(chShortName, pchTemp);
		strcpy(pchProcName, chShortName);
		Trim(pchProcName);
	}
	return;
}

/*
*	Function Name:	GetShortProcessNameAndPath
*	Description:	如果进程命令行是绝对路径则获得进程的短名称和前面路径,如果不是则保持输入参数的原状
*	Input Param:
*	pchFullProcName -------> 进程的命令行
*	Returns:	无
*	complete:	2003/03/18
*/
void GetShortProcNameAndPath(char * pchProcNameWithPath, char *pchProcName, char *pchPath)
{
	char *pchTemp = 0;
	char chShortName[LENGTH];

	memset(chShortName, 0, sizeof(chShortName));

	pchTemp = strrchr(pchProcNameWithPath, '/');
	if(0 == pchTemp)
	{
		Trim(pchProcName);
	}
	else
	{
		pchTemp++;
		strcpy(pchProcName, pchTemp);
		strncpy(pchPath, pchProcNameWithPath, pchTemp-pchProcNameWithPath);
		pchPath[pchTemp-pchProcNameWithPath] = '\0';
		Trim(pchProcName);
		Trim(pchPath);
	}
	
	return;
}

//pszPlain:明文
//pszCryptograph:密文
void Encrypt(const char *pszPlain, char *pszCryptograph)
{
	for(size_t i = 0; i < strlen(pszPlain); ++i)
	{
		pszCryptograph[i] = pszPlain[i] + 12;
	}
	return;
}

//pszCryptograph:密文
//pszPlain:明文
void Decrypt(const char *pszCryptograph, char *pszPlain)
{
	for(size_t i = 0; i < strlen(pszCryptograph); ++i)
	{
		pszPlain[i] = pszCryptograph[i] - 12;
	}
	return;
}


//解析转义字符
//输入:Str,返回已经修改过的Str;
//eg. "aabb\tdd\x20gg"-->"aabb	dd gg"

char* DecodeBackSlash(char* Str)
{
	unsigned char ch;
	char* RetPtr = Str;
	char* Dsp; // dest string pointer

	for (Dsp = Str; *Str; )
	{
		if (*Str != '\\')
		{
			*Dsp++ = *Str++;
			continue;
		}

		++Str; //skip '\'
		switch (*Str++)
		{
			case 'a':	*Dsp++ = 7; break;	//响铃
			case 'b':	*Dsp++ =  '\b'; break;	//退格
			case 'f':	*Dsp++ =  '\f'; break;	//换页
			case 'n':	*Dsp++ =  '\n'; break;	//换行LF
			case 'r':	*Dsp++ =  '\r'; break;	//回车CR
			case 't':	*Dsp++ =  '\t'; break;	//水平制表HT
			case 'v':	*Dsp++ =  '\v'; break;	//垂直制表VT
			case '\\':	*Dsp++ =  '\\'; break;	//反斜杠
			case '\?':	*Dsp++ =  '?'; break;	//问号
			case '\'':	*Dsp++ =  '\''; break;	//单引号
			case '"':	*Dsp++ =  '"'; break;	//双引号
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':	//8进制 \ddd
				ch = *(Str - 1) - '0';
				if (*Str >= '0' && *Str <= '7')
				{
					ch = (ch << 3) + *Str++ - '0';
					if (*Str >= '0' && *Str <= '7')
						ch = (ch << 3) + *Str++ - '0';
				}
				*Dsp++ = ch;
				break;
			case 'x':
				*Dsp = tolower(*Str++);
				if (*Dsp >= '0' && *Dsp <= '9')
					ch = *Dsp - '0';
				else if (*Dsp >= 'a' && *Dsp <= 'f')
					ch = *Dsp - 'a' + 10;
				else // 格式错: \x%c,保留原信息
				{
					*Dsp++ = '\\'; *Dsp++ = 'x';
					--Str;
					break;
				}
				*Dsp = tolower(*Str++);
				if (*Dsp >= '0' && *Dsp <= '9')
					ch = (ch << 4) + *Dsp - '0';
				else if (*Dsp >= 'a' && *Dsp <= 'f')
					ch = (ch << 4) + *Dsp - 'a' + 10;
				else
					--Str;
				*Dsp++ = ch;
				break;
			default:
				*Dsp++ = '\\'; *Dsp++ = *(Str - 1);
		}
	}
	*Dsp = 0;

	return RetPtr;
}


bool DeleteFile(const char *pFullPathName, const char *pBatchCode)
{
	int iResult;
	char chFileName[LENGTH];
	char chFileFilter[LENGTH];
	char chFullFileName[LENGTH];
	memset(chFullFileName, 0, sizeof(chFullFileName));
	memset(chFileFilter, 0, sizeof(chFileFilter));
	memset(chFileName, 0, sizeof(chFileName));
	
	strcpy(chFileFilter, "*");
	strcat(chFileFilter, pBatchCode);
	strcat(chFileFilter, "*");
	
	iResult = SearchOneFile(pFullPathName, chFileFilter, chFileName);
	
	while(iResult > 0)
	{
		strcpy(chFullFileName, pFullPathName);
		strcat(chFullFileName, chFileName);
		if(remove(chFullFileName)!=0)
		{
			return false;
		}
		
		iResult = SearchOneFile(pFullPathName, chFileFilter, chFileName);
	}

	if(iResult < 0)
	{
		return false;
	}

	return true;	
}

void JudgeBusiClassAndType(char * chFileName, int iBusiClass, int iBusiType)
{
	char * pchtemp = NULL;
	char * pchflag = NULL;
	char pchTempFileName[32] = "";
			
	//根据文件名判断业务大类和业务类型
	pchtemp = chFileName;
	pchflag = strchr(chFileName, '_');
	strncpy(pchTempFileName, chFileName, pchflag - pchtemp);
	pchTempFileName[pchflag-pchtemp] = '\0';
	
	//省际漫游语音话单
	if((strcmp(pchTempFileName, "NOC") == 0) || (strcmp(pchTempFileName, "NIC") == 0))
	{
		iBusiClass = 10;
		iBusiType = 10;
	}
	//国际漫游语音话单
	else if((strcmp(pchTempFileName, "IOC") == 0) || (strcmp(pchTempFileName, "IIC") == 0))
	{
		iBusiClass = 20;
		iBusiType = 20;
	}
	//边界漫游调帐话单
	else if((strcmp(pchTempFileName, "OBRC") == 0) || (strcmp(pchTempFileName, "IBRC") == 0))
	{
		iBusiClass = 30;
		iBusiType = 30;
	}
	//不同制式国际漫游话单
	else if((strcmp(pchTempFileName, "KOC") == 0) || (strcmp(pchTempFileName, "KIC") == 0))
	{
		iBusiClass = 40;
		iBusiType = 40;
	}
	//国际漫游GPRS话单
	else if(strcmp(pchTempFileName, "IOG") == 0)
	{
		iBusiClass = 50;
		iBusiType = 50;
	}
	//ISMP结算话单
	else if(strcmp(pchTempFileName, "ISMP") == 0)
	{
		iBusiClass = 50;
		iBusiType = 50;
	}
	//短信国际漫游话单
	else if((strcmp(pchTempFileName, "IOSMS") == 0) || (strcmp(pchTempFileName, "IISMS") == 0))
	{
		iBusiClass = 60;
		iBusiType = 60;
	}
	//异地充值结算文件
	else if((strcmp(pchTempFileName, "OVC") == 0) || (strcmp(pchTempFileName, "IVC") == 0))
	{		
		iBusiClass = 70;
		iBusiType = 70;
	}
	else 
	{
		iBusiClass = 0;
		iBusiType = 0;
	}  
}

void 	JudgeYcFlag(const int iBusiClass, const char * chRecordType, const int iDuring, int iYcFlag)
{
	switch(iBusiClass)
	{
		case 10:
		case 30:
			if( strcmp(chRecordType, "1") || strcmp(chRecordType, "2"))
			{
				if(iDuring > 21600)
				{
					iYcFlag = 1;
				}
			}
			else if(strcmp(chRecordType, "3")) 
			{
				if(iDuring > 129600)
				{
					iYcFlag = 1;
				}
			}
			break;
		case 20:
		case 40:		
			if(iDuring > 21600)
			{
				iYcFlag = 1;
			}
			break;
		case 50:
		case 70:
			if(iDuring > 43200)
			{
				iYcFlag = 1;
			}
			break;

		case 60:
			if(iDuring > 86400)
			{
				iYcFlag = 1;
			}
			break;
		default:
			break;
	}
}

int time_diff(char *pchStrBeginTime,char *pchStrEndTime)
{
	struct tm tBeginTime, tEndTime;
	int i_time;
	char temp[10];        //临时时间变量
	
	temp[0] = '\0';
  //得到服务开始时间
  memcpy(temp,pchStrBeginTime,4);
  temp[4]=0;
  tBeginTime.tm_year=atoi(temp)-1900;
  memcpy(temp,pchStrBeginTime+4,2);
  temp[2]=0;
  tBeginTime.tm_mon=atoi(temp)-1;
  memcpy(temp,pchStrBeginTime+6,2);
  temp[2]=0;
  tBeginTime.tm_mday=atoi(temp);
  memcpy(temp,pchStrBeginTime+8,2);
  temp[2]=0;
  tBeginTime.tm_hour=atoi(temp);
  memcpy(temp,pchStrBeginTime+10,2);
  temp[2]=0;
  tBeginTime.tm_min=atoi(temp);
  memcpy(temp,pchStrBeginTime+12,2);
  temp[2]=0;
  tBeginTime.tm_sec=atoi(temp);
  //得到服务结束时间
  memcpy(temp,pchStrEndTime,4);
  temp[4]=0;
  tEndTime.tm_year=atoi(temp)-1900;
  memcpy(temp,pchStrEndTime+4,2);
  temp[2]=0;
  tEndTime.tm_mon=atoi(temp)-1;
  memcpy(temp,pchStrEndTime+6,2);
  temp[2]=0;
  tEndTime.tm_mday=atoi(temp);
  memcpy(temp,pchStrEndTime+8,2);
  temp[2]=0;
  tEndTime.tm_hour=atoi(temp);
  memcpy(temp,pchStrEndTime+10,2);
  temp[2]=0;
  tEndTime.tm_min=atoi(temp);
  memcpy(temp,pchStrEndTime+12,2);
  temp[2]=0;
  tEndTime.tm_sec=atoi(temp);
	i_time = mktime(&tEndTime) - mktime(&tBeginTime);
	return i_time;
}
