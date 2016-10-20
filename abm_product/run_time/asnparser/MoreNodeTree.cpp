#include "MoreNodeTree.h"

// 这里定义一个深度变量用于测试时打印树结构
int currentDepth = 0;

/*
* 函数功能：在本节点中增加一个孩子节点
* 输入参数：待加入的孩子节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::childAdd(TList* child)
{
	if (!child) {
		printf("can not insert a null of child!\n");
		return -1;
	}

	if (isMySelf(child)) {
		printf("node can not inert heself as a child!\n");
		return -1;
	}

	/* 如果节点不存在孩子的话则直接插入到节点下面*/
	if (isLeaf(m_oNode)) {
		m_oNode.poPrev = child;
		child->poNext = &m_oNode;
		return 0;
	}

	/* 首先遍历到最后一个孩子节点*/
	TList *latestChild = m_oNode.poPrev;
	findLatestChild((&m_oNode), latestChild);

	/* 如果没有找到最后一个孩子节点就退出则返回异常*/
	if (!latestChild->poNext) {
		printf("find the latest child error!\n");
		return -2;
	}

	/* 将新节点加入最后一个孩子节点之后*/
	child->poNext = latestChild->poNext;
	latestChild->poNext = child;
	return 0;
}


/*
* 函数功能：在本节点中增加一个兄弟节点
* 输入参数：待加入的兄弟节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::brotherAdd(TList* brother)
{
	if (!brother) {
		printf("can not insert a null of child!\n");
		return -1;
	}

	if (isMySelf(brother)) {
		printf("node can not inert heself as a brother node!\n");
		return -1;
	}

	/* 这里不保证根节点没有兄弟节点的情况 */
	brother->poNext = m_oNode.poNext;
	m_oNode.poNext = brother;
	
	return 0;
}


/*
* 函数功能：按先序遍历树节点
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::fetchNode()
{
	currentDepth++;
	TList* node = m_oNode.poPrev;
	int iRet = 0;
	
	iRet = this->process(); 							// 遍历到节点则先进行打印
	if (iRet) {
		currentDepth = 0;
		return iRet;
	}
	while (node->poNext && node != &m_oNode) {
		if (!isLeaf((*node))) {							// 如果是子节点是一棵树，则递归遍历
			iRet = listEntry(node)->fetchNode();
			if (iRet) {
				currentDepth = 0;
				return iRet;
			}
		}
		else {
			iRet = listEntry(node)->process(); 			// 遍历到叶子节点则进行打印
			if (iRet) {
				currentDepth = 0;
				return iRet;
			}
		}
		node = node->poNext;							// 进行下一个兄弟节点的遍历
	}
	currentDepth--;
	return 0;
}

/*
* 函数功能：对节点本身数据做处理，不包含其子节点!
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::process()
{
	for (int i = 1; i < currentDepth; i++) {
		printf("\t");
	}

	if (isLeaf(m_oNode)) {
		printf("  ..");
	}
	else {
		printf("+");
	}

	int iRet = this->printKey();
	if (iRet) {
		return iRet;
	}

	printf("\n");
	
	return 0;
}

/*
* 函数功能：打印节点key，此处用于测试
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::printKey()
{
	return 0;
}
/*
* 函数功能：在本节点中剔除指定孩子节点
* 输入参数：待删除的孩子节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int CMoreNodeTree::childDrop(TList* child)
{
	if (!child) {
		printf("can not delete a null of child!\n");
		return -1;
	}

	if (isMySelf(child)) {
		printf("node can not delete heself as a child!\n");
		return -2;
	}

	/* 如果节点不存在孩子的话则返回删除失败*/
	if (isLeaf(m_oNode)) {
		return -3;
	}

	/* 遍历孩子节点，找到则将指定节点从树中剔除*/
	for (TList* node = m_oNode.poPrev; \
		node->poNext && node != &m_oNode; \
		node = node->poNext) {

		if (node->poNext == child) {
			node->poNext = child->poNext;
			return 0;
		}
	}

	return -3;	//如果没有找到则返回错误
}

/*
* 函数功能：释放树中所有几点资源
* 输入参数：无
* 输出参数：无
* 返回值  ：void
*/
void CMoreNodeTree::destory()
{
	if (isLeaf(m_oNode)) {
		delete this;
		return;
	}

	TList* tmp = NULL;
	TList* node = m_oNode.poPrev;
	m_oNode.poPrev = &m_oNode;							// 将该节点下的子树断开,使之成为叶子节点
	while (node->poNext && node != &m_oNode) {			// 逐个将子树进行destory
		tmp = node->poNext;
		listEntry(node)->destory();
		node = tmp;
	}

	return;
}

/*
* 函数功能：根据node位置找到本身对象
* 输入参数：无
* 输出参数：无
* 返回值  ：void
*/
CMoreNodeTree* CMoreNodeTree::listEntry(TList* node)
{
	return listObject(node, CMoreNodeTree, m_oNode);
}

