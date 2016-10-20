/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：ASNDecode.h 
* 摘    要：文件定义了解码asn.1协议方法
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月28日
*/

#include <sys/stat.h>
#include "ASNTree.h"
#include "ASNCheck.h"


#define BIGSIZE 			256 * 1024 * 1024
#define CACHESIZE 			8 * 1024
#define BIGGERCACHETIMES	(BIGSIZE) / (CACHESIZE)


enum err {
	err_param_invalid = 1,
	err_data_invalid,
	err_file_open,
	err_file_read,
	err_file_close,
	err_file_null,
	err_check_byUTree,
	err_data_enough,
	err_add_child,
	err_big_size,
};

enum decode_status{
	decode_tag		= 0,
	decode_len		= 1,
	decode_values	= 2
};

class ASNParser {

#define TOP	stack[top - 1]
#define TOPSIZE sizestack[top - 1]
#define DECODEOVER	(1 << 31 - 1)

protected:
	ASNParser() {
		for (int i = 0; i < 20; i++) {
			stack[i] = NULL;
			sizestack[i] = 0;
		}
		code 		= NULL;
		current 	= stack[0];
		size 		= 0;
		top			= 0;
		status 		= decode_tag;
	}

	virtual ~ASNParser() {
		for (int i = 0; i < 20; i++) {
			stack[i] = NULL;
			sizestack[i] = 0;
		}
		code 		= NULL;
		current		= NULL;
		top			= 0;
		status 		= decode_tag;
	}

	/*
	* 函数功能：解码tag部分
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int DecodeTag();
	/*
	* 函数功能：解码len部分
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int DecodeLen();

	/*
	* 函数功能：解码values部分
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int copyValues();

	/*
	* 函数功能：捕获到异常的出来函数
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int catchException(err & iRet);
	/*
	* 函数功能：将当前类中资源完全释放
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int destorySite();
	/*
	* 函数功能：新增节点的校验入口
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int check() {
		return 0;
	}
public:
	/*
	* 函数功能：解码入口函数
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int decode();
	
	virtual ASNTree * getRecord() {
		return record;
	}

private:
	
	inline int tagCode(unsigned short tag)	{
		if (tag < 0x1F) {
			return 1;
		}

		int num = 2;

		for (int key = tag; key > 0x7F; key = key / 0x7F) {
			num++;
		}

		return num;
	}
	
	inline int lenCode(unsigned int len)	{
		if (len <= 0x7F) {
			return 1;
		}

		int num = 2;

		for (int key = len; key > 0xFF; key = key / 0xFF) {
			num++;
		}

		return num;
	}
	
	/*
	* 函数功能：将新解码出来的节点加入树中
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int encodeTree(ASNTree* node);
	/*
	* 函数功能：在协议栈中弹出第一个元素
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int pop();
protected:
	char*			code;								// 待解析的码流数据
	unsigned int 	size;								// 数据大小
	decode_status 	status;
	
	ASNTree*		stack[20];							// 协议解析栈
	unsigned int 	sizestack[20];						// 数据长度栈
	int				top;								// 栈顶指针

	ASNTree*		record;								// 数据解析后的承载体
	ASNTree*		current;							// 最近加入到树中的节点

	static unsigned int m_uiCheckIdx;					// 校验数据索引
};

class ASNFileParser : public ASNParser {
public:

	static ASNFileParser* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNFileParser();
			if (!m_poInstance) {
				printf("new memory error\n");
				exit(-1);
			}
		}
		m_uiCheckIdx = index;
		return m_poInstance;
	}
	/*
	* 函数功能：解码入口函数
	* 输入参数：文件全路径
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int decode(const char* sFileFullName);

	virtual int freeSomeNode();
	
protected:
	ASNFileParser() {
		m_iFd = 0;

		m_sCacheBuff	= new char[CACHESIZE];					// 开辟10k缓冲用于读取文件
		code = m_sCacheBuff;
		m_uiCacheSize = 0;
	}

	virtual ~ASNFileParser() {
		m_iFd = 0;

		if (m_sCacheBuff) {
			delete m_sCacheBuff;
			m_sCacheBuff = NULL;
		}

		m_uiCacheSize = 0;
	}


	/*
	* 函数功能：将数据加载到缓冲中
	* 输入参数：文件全路径
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	int cacheBuff();
	/*
	* 函数功能：捕获到异常的出来函数
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int catchException(err & iRet);
	
protected:
	int			m_iFd;

	char*		m_sCacheBuff;
	unsigned int m_uiCacheSize;
private:
	static ASNFileParser* m_poInstance;
};

class ASNMessageParser : public ASNParser {
public:
	static ASNMessageParser* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNMessageParser();
			if (!m_poInstance) {
				printf("new memory error\n");
				exit(-1);
			}
		}
		m_uiCheckIdx = index;
		return m_poInstance;
	}

	int decode(char* sCode, unsigned int len);
protected:
	ASNMessageParser() {
		
	}

	virtual ~ASNMessageParser() {

	}

private:
	static ASNMessageParser* m_poInstance;
};

class ASNFileMutiParser : public ASNFileParser {

struct TRecordList {
	ASNTree*	node;
	TList 		list;
	TRecordList () {
		node = NULL;
		list.poNext = &list;
		list.poPrev = &list;
	}
};
public:
	static ASNFileMutiParser* getInstance(unsigned int index = 0) {
		if (!m_poInstance) {
			m_poInstance = new ASNFileMutiParser();
			if (!m_poInstance) {
				printf("new memory error\n");
				exit(-1);
			}
		}
		m_uiCheckIdx = index;
		return m_poInstance;
	}
	

	/*
	* 函数功能：解码入口函数
	* 输入参数：文件全路径
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int decode(const char* sFileFullName);
	
	virtual ASNTree * getRecord() {
		return m_poHead ? m_poHead->node : NULL;
	}

protected:
	ASNFileMutiParser() {
		m_poHead = NULL;
		m_uiNum  = 0;
	}

	virtual ~ASNFileMutiParser() {
		freeList();
	}
	
	/*
	* 函数功能：释放资源
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int freeList();
protected:
	TRecordList* 	m_poHead;
	unsigned int	m_uiNum;
	unsigned int 	m_uiFileSize;
private:
	static ASNFileMutiParser* m_poInstance;
};

