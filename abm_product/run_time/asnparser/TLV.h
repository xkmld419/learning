/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：TLV.h 
* 摘    要：文件定义了TLV格式数据结构
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月20日
*/
#ifndef __TLV_H_
#define __TLV_H_

/* ************** 类型定义************ */
typedef unsigned short	valid_UINT16; 
typedef unsigned short 	TAG_UINT16;
typedef unsigned int 	LEN_UINT32;
typedef char *		 	VALUES_CHARPTR;




/* ************** 常量定义************ */
#define INTBITS			(sizeof(int) * 8)
#define LONGBITS		(sizeof(long) * 8)
#define DEFAULTINT  	((1 << (INTBITS - 1)) - 1)
#define DEFAULTLONG  	((1 << (LONGBITS - 1)) - 1)	
#ifndef NULL
#define NULL 0
#endif




/* ************** TLV类型定义************ */
/* TLV类定义了tlv类型的最基本数据* */
/* 结构，并增加一个valid变量表示** */
/* 字段是否真实有效                             *** */
class TLV {
public:
	TLV()	{
		m_usTag 		= 0;
		m_uiLen 		= 0;
		m_sValues 		= NULL;
	}

	virtual ~TLV() {
		if (NULL != m_sValues) {
			delete m_sValues;
			m_sValues = NULL;
		}
	}

	virtual valid_UINT16 isValid() {					// 判断字段是否有效,1:有效,0:无效
		return m_usValid;
	}

	virtual int asInt() {								// 这里用32位整形最大值来作为默认值
		return DEFAULTINT;								//派生类中可以根据values解析后的值进行返回
	}

	virtual long asLong() {							// 这里用长整形最大值来作为默认值
		return DEFAULTLONG;								//派生类中可以根据values解析后的值进行返回
	}

	virtual const char* asString() {
		return m_sValues;
	}
public:
	valid_UINT16		m_usValid;
	TAG_UINT16			m_usTag;
	LEN_UINT32  		m_uiLen;
	VALUES_CHARPTR		m_sValues;
};

#endif

