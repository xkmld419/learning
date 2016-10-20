/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：ASNCheck.h 
* 摘    要：文件定义了用于校验asn解码格式的元数据树
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月21日
*/
#ifndef __ASN_CHECK_
#define __ASN_CHECK_

#include <map>
#include <unistd.h>
#include "ASNObject.h"
#include "TimesTenAccess.h"

using namespace std;

extern ABMException oExp;

struct ASNFormatData {
	ASNUTree*	UTree;
	ASNUTree*	current;

	ASNFormatData() {
		UTree 	= NULL;
		current = NULL;
	}

	~ASNFormatData() {
		if (UTree) {
			delete UTree;
			UTree = NULL;
		}
		current = NULL;
	}
};

typedef std::map<int, ASNFormatData*>			TASNFORMATMAP;
typedef std::map<int, ASNFormatData*>::iterator	TASNFORMATITER;


/* ************** ASN字段格式校验类************ */
class ASNCheck {
public:
	
	/*
	* 函数功能：对asn子树进行格式校验
	* 输入参数：待校验的节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int checkField(ASNTree * field);
	
protected:
	ASNCheck() {
		for (int i = 0; i < 20 ; i++) {
			m_oFormatMap[i] = NULL;
		}
	}

	virtual ~ASNCheck() {
		destory();
	}

	/*
	* 函数功能：校验格式的元数据装载
	* 输入参数：
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int init() = 0;

	virtual void destory()
	{
		for (int i = 0; i < 20 ; i++) {
			if (m_oFormatMap[i]) {
				delete m_oFormatMap[i];
			}

			m_oFormatMap[i] = NULL;
		}
	}

public:
	ASNFormatData 		*m_oFormatMap[20];

	static unsigned int m_uiCurIdx;
};




/* ************** ASN字段格式校验子类************ */
/* ** 无数据字典,测试其它未定义数据时用*** */
class ASNNULLConf : public ASNCheck {
public:
	static ASNNULLConf* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNNULLConf();
			if (!m_poInstance) {
				printf("ASNCheck::ASNCheck()函数调用失败,申请内存失败!");
				exit(-1);
				return NULL;
			}
			ABMException oExp;
			if (m_poInstance->init()) {
				printf("ASNCheck::init函数调用失败!");
				exit(-1);
				return NULL;
			}
		}
		m_uiCurIdx = index;
		return m_poInstance;
	}
	/*
	* 函数功能：对asn子树进行格式校验
	* 输入参数：待校验的节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int checkField(ASNTree * field)
	{
		return 0;
	}
protected:
	ASNNULLConf() {
		m_poInstance = NULL;
	}

	virtual ~ASNNULLConf() {

	}

	/*
	* 函数功能：装载数据字典,这里通过数据库进行配置
	* 输入参数：
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int init()
	{
		return 0;
	}
private:
	static ASNNULLConf	*m_poInstance;
};



/* ************** ASN字段格式校验子类************ */
/* *********** 数据字典由数据库中装载********* */
class ASNDBConf : public ASNCheck {
public:
	static ASNDBConf* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNDBConf();
			if (!m_poInstance) {
				printf("ASNCheck::ASNCheck()函数调用失败,申请内存失败!");
				exit(-1);
				return NULL;
			}

			if (m_poInstance->init()) {
				printf("ASNCheck::init函数调用失败!");
				exit(-1);
				return NULL;
			}
		}
		m_uiCurIdx = index;
		return m_poInstance;
	}
	
protected:
	ASNDBConf() {
		m_poTTCursor = NULL;
		m_poTTConn   = NULL;
	}

	virtual ~ASNDBConf() {
		RelTTCursor(m_poTTCursor);

		m_poTTCursor = NULL;
		m_poTTConn   = NULL;
	}

	/*
	* 函数功能：装载数据字典,这里通过数据库进行配置
	* 输入参数：
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int init();

private:
	TimesTenCMD 		*m_poTTCursor;				// tt游标
	TimesTenConn		*m_poTTConn;
	
	static ASNDBConf	*m_poInstance;
};



/* ************** ASN字段格式校验子类************ */
/* ********* 数据字典由配置文件中装载******** */
class ASNFileConf : public ASNCheck {
public:
	static ASNFileConf* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNFileConf();
			if (!m_poInstance) {
				printf("ASNCheck::ASNCheck()函数调用失败,申请内存失败!");
				exit(-1);
				return NULL;
			}

			ABMException oExp;
			if (m_poInstance->init()) {
				printf("ASNCheck::init函数调用失败!");
				exit(-1);
				return NULL;
			}
		}
		m_uiCurIdx = index;
		return m_poInstance;
	}
	
	/*
	* 函数功能：对asn子树进行格式校验
	* 输入参数：待校验的节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int checkField(ASNTree * field)
	{
		//需要再实现
		return 0;
	}
protected:
	ASNFileConf() {
		m_poFile 	= NULL;
		m_poInstance = NULL;
	}

	virtual ~ASNFileConf() {
		if (m_poFile) {
			fclose(m_poFile);
			m_poFile = NULL;
		}
	}

	/*
	* 函数功能：装载数据字典,这里通过配置文件进行配置
	* 输入参数：
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int init()
	{
		//需要再实现
		return 0;
	}

private:
	FILE				*m_poFile;
	
	static ASNFileConf	*m_poInstance;
};


#endif

