/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：ASNObject.h 
* 摘    要：文件定义了用于校验asn解码格式的元数据树
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月21日
*/
#ifndef __ASN_OBJECT_
#define __ASN_OBJECT_

#include "ASNTree.h"


int checkDateValid(const char * sDate);
int	checkDateTimeValid(const char * sDate);



/* ************** ASN元记录型承载类************ */
/* ******* 用于存放预配置的数据字典******* */
class ASNUTree : public ASNTree {
public:
	ASNUTree() {
		m_uiMaxLen 	= 0;
		m_iNum 		= 0;
		m_iSequence	= 0;
		m_uiMinLen	= 0;
		m_iCurNum	= 0;
	}

	virtual ~ASNUTree() {
		
	}

	/*
	* 函数功能：在asn元树中加入一个节点
	* 输入参数：待加入节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int UTreeAdd(ASNUTree * node);

	/*
	* 函数功能：在asn元树找到指定节点
	* 输入参数：待加入节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual ASNUTree* UTreeFind(TAG_UINT16 tag);

	/*
	* 函数功能：判断下一个加入的节点是否符合树的规则
	* 输入参数：待加入节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int UTreeCheckField(ASNTree * node, ASNUTree * &next);

	
	/*
	* 函数功能：判断新节点是否符合本身属性的定义
	* 输入参数：待加入节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int doCheck(ASNTree * node);

private:
	int typeCheck(const unsigned short type) {
		return 0;//m_usType == type ? 0 : -1;
	}
	int flagCheck(ASNTree * node) {
		if (1 == m_usFlag) {
			return 1 == node->m_usFlag? 0 : -1;
		}
		else {
			if (1 == node->m_usFlag) {
				printf("ASNUTree::flagCheck the m_usFlag of utree is 0 , but this is 1, node tag is %d\n", this->m_usTag);
				return -1;
			}

			node->m_usFlag = m_usFlag;
		}

		int iRet = 0;
		switch (m_usFlag) {
		case ASN_FLAG_INT : {
			if (node->m_uiLen > 4 || 0 >= node->m_uiLen) {
				printf("ASNUTree::flagCheck the node(int) len invalid, node len is %d, node tag is %d\n", node->m_uiLen, this->m_usTag);
				iRet = -1;
			}
			break;
		}
		case ASN_FLAG_LONG : {
			if (node->m_uiLen > 8 || 0 >= node->m_uiLen) {
				printf("ASNUTree::flagCheck the node(long) len invalid, node len is %d, node tag is %d\n", node->m_uiLen, this->m_usTag);
				iRet = -1;
			}
			break;
		}
		case ASN_FLAG_STRING : {
			
			break;
		}
		case ASN_FLAG_DATE : {
			if (node->m_uiLen != 8) {
				printf("ASNUTree::flagCheck the node(date) len invalid, node len is %d, node tag is %d\n", node->m_uiLen, this->m_usTag);
				iRet = checkDateValid(node->asString());
			}
			break;
		}
		case ASN_FLAG_CHOICE : {
			
			break;
		}
		case ASN_FLAG_DATETIME: {
			if (node->m_uiLen != 14) {
				printf("ASNUTree::flagCheck the node(datetime) len invalid, node len is %d, node tag is %d\n", node->m_uiLen, this->m_usTag);
				iRet = checkDateTimeValid(node->asString());
			}
			break;
		}
		default: {
		
			break;
		}
		}

		return iRet;
	}
	int lenCheck(ASNTree * node) {
		if (!m_uiMinLen && !m_uiMaxLen) {
			return 0;
		}

		if (node->m_uiLen < m_uiMinLen || node->m_uiLen > m_uiMaxLen) {
			printf("ASNUTree::lenCheck the node len invalid, node len is %d, node tag is %d\n", node->m_uiLen, this->m_usTag);
			return -1;
		}
		return  0;
	}
	
public:
	unsigned int 		m_uiMaxLen;
	unsigned int 		m_uiMinLen;
	int					m_iNum;
	int					m_iSequence;
	int 				m_iCurNum;
};




/* ************** ASN文件记录型承载类************ */
class ASNRecord : public ASNTree {
public:
	ASNRecord() {
		// current = m_poNode;
	}

	virtual ~ASNRecord() {
		current = NULL;
	}

	/*
	* 函数功能：在本节点中增加一个孩子节点
	* 输入参数：待加入的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	/*
	virtual int childAdd(CMoreNodeTree* child) {
		int iRet = CMoreNodeTree::childAdd(child);

		if (!iRet) {
			current = child->m_poNode;				// 更新树中节点后，重新设置current
		}

		return iRet;
	}
	*/
	/*
	* 函数功能：在本节点中增加一个兄弟节点
	* 输入参数：待加入的兄弟节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	/*
	virtual int brotherAdd(CMoreNodeTree* brother) {
		int iRet = CMoreNodeTree::brotherAdd(brother);

		if (!iRet) {
			current = brother->m_poNode;			// 更新树中节点后，重新设置current
		}

		return iRet;
	}
	*/
public:
	TList*	current;		// 保存一个当前处理的节点指针，它指向m_poNode下的任何一个节点
};




/* ************** ASN消息记录型承载类************ */
class ASNMessage : public ASNTree {
public:
	ASNMessage() {

	}

	virtual ~ASNMessage() {

	}
};


#endif

