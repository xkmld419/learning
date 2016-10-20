/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：MoreNodeTree.h 
* 摘    要：文件定义了一个可拥有多子节点的树状结构
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年10月20日
*/
#ifndef __MORE_NODE_TREE_H_
#define __MORE_NODE_TREE_H_

#include <stdio.h>


#ifndef NULL
#define NULL 0
#endif

#define listself(node) { \
		node->poNext = node; \
		node->poPrev = node; \
	}
// 判断节点是否就是本事节点
#define isMySelf(node) (node == &this->m_oNode)
// 判断节点是否叶子节点
#define isLeaf(node) (node.poPrev == &node)
// 判断节点是否root节点
#define isRoot(node) (node.poNext == &node)
// 查找最后一个孩子节点
#define findLatestChild(node, latestChild) { \
	for(latestChild = node->poPrev; latestChild->poNext && latestChild->poNext != node; latestChild = latestChild->poNext) { } \
	}
#define listObject(ptr, type, member) \
	((type*)((char*)ptr - (unsigned long)(&((type*)0)->member)))

/*用于连接各节点的指针链*/
struct TList {
	TList *poNext;	// 
	TList *poPrev;	// 
	
	TList() {
		poNext = NULL;
		poPrev = NULL;
	}
};

class CMoreNodeTree {								//该类承载一个多节点的树状结构
public:
	CMoreNodeTree() {
		listself((&m_oNode));
	}

	virtual ~CMoreNodeTree() {
		if (!isLeaf(m_oNode)) {
			destory();
		}
	}

	/*
	* 函数功能：在本节点中增加一个孩子节点
	* 输入参数：待加入的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int childAdd(CMoreNodeTree* child) {
		return child == NULL ? -1 : childAdd(&child->m_oNode);
	}

	/*
	* 函数功能：在本节点中增加一个兄弟节点
	* 输入参数：待加入的兄弟节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int brotherAdd(CMoreNodeTree* brother) {
		return brother == NULL ? -1 : brotherAdd(&brother->m_oNode);
	}

	
	/*
	* 函数功能：在本节点中删除指定孩子节点
	* 输入参数：待删除的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int childDrop(CMoreNodeTree* child) {
		return child == NULL ? -1 : childDrop(&child->m_oNode);
	}

	/*
	* 函数功能：按先序遍历树节点
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int fetchNode();

protected:
	/*
	* 函数功能：在本节点中增加一个孩子节点
	* 输入参数：待加入的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int childAdd(TList* child);

	
	/*
	* 函数功能：在本节点中增加一个兄弟节点
	* 输入参数：待加入的兄弟节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int brotherAdd(TList* brother);

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
	* 函数功能：在本节点中删除指定孩子节点
	* 输入参数：待删除的孩子节点
	* 输出参数：无
	* 返回值  ：如果成功则返回0，否则返回非零
	*/
	virtual int childDrop(TList* child);

	/*
	* 函数功能：释放树中所有几点资源
	* 输入参数：无
	* 输出参数：无
	* 返回值  ：void
	*/
	virtual void destory();
	
	/*
	* 函数功能：根据node位置找到本身对象
	* 输入参数：node节点位置
	* 输出参数：无
	* 返回值  ：void
	*/
	virtual CMoreNodeTree* listEntry(TList* node);
public:
	TList	m_oNode;		/*最简单的树只有一个指针域来完成节点之间的链接即可*/
};

#endif

