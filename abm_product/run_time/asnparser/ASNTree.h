/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：ASNTree.h 
* 摘    要：文件定义了一颗ASN.1格式的树，用于承载asn协议解码后的数据
*	当节点为叶子节点时:
*		list.next指向的是同一父节点下一个兄弟节点
*		最后一个兄弟节点指向父节点;list.prev指向父节点
*	当节点为非叶子节点时:
*		list.prev它指向子节点的最左边节点
*		list.next指向同一父节点下的下一个兄弟节点
*		最后一个兄弟节点next指向父节点
* 		root节点next指针在树中没有具体价值
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月20日
*/
#ifndef __ASN_TREE_H_
#define __ASN_TREE_H_

#include <stdio.h>
#include "TLV.h"
#include "MoreNodeTree.h"

/* ************** 类型定义************ */


/* ************** 常量定义************ */
#define		ASN_FLAG_STRUCT				1				// 结构类型
#define		ASN_FLAG_UNION				0				// 原子式
#define 	ASN_FLAG_INT				2				// INT 型
#define 	ASN_FLAG_STRING				3				// string 型
#define 	ASN_FLAG_LONG				4				// LONG 型
#define 	ASN_FLAG_DATE				5				// 时间 型<yyyymmdd>
#define 	ASN_FLAG_CHOICE				6				// 枚举类型
#define 	ASN_FLAG_DATETIME			7				// 时间 型<yyyymmddHH24MISS>

#define		ASN_TYPE_UNIVERSAL			0				// 通用类型
#define		ASN_TYPE_APPLICATION		1				// 应用程序独有类型节点
#define		ASN_TYPE_CONTEXTSPECIFIC 	2				// 上下文相关类型节点
#define		ASN_TYPE_PRIVATE			3				// 该asn结构私有类型节点

class ASNTree;

typedef int (*PROCESS)(ASNTree* node, void * data);	// 用于遍历树时调用指定的处理函数


/*
* 函数功能：判定主机是否高字节序
* 输入参数：无
* 输出参数：无
* 返回值  ：1-高端字节序, 0-低端字节序
*/
int isBigEndian();

/*
* 函数功能：按文本格式打印树的叶子节点
* 输入参数：无
* 输出参数：无
* 返回值  ：0成功, 1失败
*/
int printRecord(ASNTree* node, void * data);

/*
* 函数功能：按文本格式打印树的叶子节点到文件中
* 输入参数：无
* 输出参数：无
* 返回值  ：0成功, 1失败
*/
int printRecordToFile(ASNTree* node, void * data);


/* ************** ASN数据基类************ */
class ASNTree : public CMoreNodeTree, public TLV {
public:
	ASNTree() {
		m_usFlag = ASN_FLAG_UNION;
		m_usType = ASN_TYPE_UNIVERSAL;
		m_v.iData = 0;
	}

	virtual ~ASNTree() {
		m_usFather = 0;
	}
	
	/*
	* 函数功能：在本节点中增加一个孩子节点
	* 输入参数：待加入的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int childAdd(ASNTree* child) {
		child->m_usFather = this->m_usTag;
		return CMoreNodeTree::childAdd(child);
	}
	
	virtual int asInt() {
		if (m_usValid) {
			return m_v.iData;
		}
		else {
			return deInt();
		}
	}

	virtual long asLong() {
		if (m_usValid) {
			return m_v.lData;
		}
		else {
			return deLong();
		}
	}

	virtual const char* asString() {
		if (m_usValid) {
			return m_v.sData;
		}
		else {
			return deString();
		}
	}

	/*
	* 函数功能：按先序遍历树节点
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int fetchProcess(PROCESS fn, void * data);

	/*
	* 函数功能：按先序遍历树节点
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual ASNTree* listNext() {
		return listObject(m_oNode.poNext,ASNTree,m_oNode);
	}
	
	/*
	* 函数功能：按16进制将values打印出来
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：void
	*/
	virtual void traceValues() {
		if (1 == m_usFlag) {
			return;
		}
		char *p = m_sValues;
		for (int i = 0; i < m_uiLen; i++, p++) {
			printf(" %02x", (*p) & 0x000000FF);
		}
		p = NULL;
		return ;
	}
protected:
	
	/*
	* 函数功能：对节点本身数据做处理，不包含其子节点
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int process();

	/*
	* 函数功能：打印节点key，此处用于测试
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int printKey();

	
	/*
	* 函数功能：根据node位置找到本身对象
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：void
	*/
	virtual CMoreNodeTree* listEntry(TList* node) {
		return listObject(node,ASNTree,m_oNode);
	}

	virtual int deInt() {
		if (NULL == m_sValues 
			|| m_usFlag != ASN_FLAG_INT) {
			return DEFAULTINT;
		}
		
		char *p = const_cast<char*>(m_sValues);
		m_v.iData = 0;
		while(m_uiLen--) {									// 此处按int型由高到低字节序进行解码
			m_v.iData *= 1 << 8;
			m_v.iData += 0x000000ff&((unsigned int )(*p));
			p++;
		}
		m_usValid = 1;
		
		return m_v.iData;
	}
	
	virtual long deLong() {
		if (NULL == m_sValues 
			|| m_usFlag != ASN_FLAG_LONG) {
			return DEFAULTINT;
		}
		
		char *p = const_cast<char*>(m_sValues);
		m_v.lData = 0;
		while(m_uiLen--) {									// 此处按int型由高到低字节序进行解码
			m_v.lData *= 1 << 8;
			m_v.lData += 0x000000ff&((unsigned int )(*p));
			p++;
		}
		m_usValid = 1;
		
		return m_v.lData;
	}
	
	virtual char* deString() {
		if (NULL == m_sValues || (
			m_usFlag != ASN_FLAG_STRING
			&& m_usFlag != ASN_FLAG_DATE
			&& m_usFlag != ASN_FLAG_CHOICE)) {
			return NULL;
		}
		m_usValid = 1;
		
		m_v.sData = m_sValues;

		return m_v.sData;
	}
public:
	unsigned short 		m_usFlag;
	unsigned short		m_usType;
	TAG_UINT16			m_usFather;

	

	union tlv_v {
		int 	iData;
		long	lData;
		char*	sData;
	}					m_v;
};




/* ************** ASN基本数据类型INT************ */
class ASNInt : public ASNTree {
public:
	ASNInt() {
		m_iData 	= DEFAULTINT;
		m_bParse 	= false;
	}

	virtual ~ASNInt() {

	}

	virtual int asInt() {
		return m_bParse ? m_iData : decode();
	}
private:
	virtual int decode();
private:
	int			m_iData;
	bool		m_bParse;
};




/* ************** ASN基本数据类型LONG************ */
class ASNLong : public ASNTree {
public:
	ASNLong() {
		m_lData 	= DEFAULTLONG;
		m_bParse 	= false;
	}

	virtual ~ASNLong() {

	}

	virtual long asLong() {
		return m_bParse ? m_lData : decode();
	}
private:
	virtual long decode();
private:
	long		m_lData;
	bool		m_bParse;
};




/* ********* ASN基本数据类型定长string******** */
class ASNString : public ASNTree {
public:
	ASNString() {
		m_sData 	= NULL;
		m_bParse 	= false;
	}

	virtual ~ASNString() {
		if (NULL != m_sData) {
			delete m_sData;
			m_sData = NULL;
		}
	}

	virtual const char* asString() {
		return m_bParse ? m_sData : decode();
	}
private:
	virtual const char* decode();
private:
	char*		m_sData;
	bool		m_bParse;
};

#endif

