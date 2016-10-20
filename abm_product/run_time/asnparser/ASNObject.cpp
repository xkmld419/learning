#include <stdlib.h>
#include <string.h>
#include "ASNObject.h"

int checkDateValid(const char * sDate) {
	if (!sDate) {
		return -1;
	}

	char tempDate[9] = {0};
	strcpy (tempDate, sDate);
	int iDay = atoi (tempDate+6);
	tempDate[6] = 0;
	int iMon = atoi (tempDate+4);
	tempDate[4] = 0;
	int iYear = atoi (tempDate);

	if (iDay > 12 || iDay < 1) {
		return -2;
	}
	if (iMon > 12 || iDay < 1) {
		return -3;
	}
	if (iYear > 9999 || iYear < 1) {
		return -4;
	}

	switch (iMon) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	{
		if ( 1 > iDay || iDay > 31 ) {
			return -5;
		}
		break;
	}
	case 4:
	case 6:
	case 9:
	case 11:
	{
		if ( 1 > iDay || iDay > 30 ) {
			return -5;
		}
		break;
	}
	case 2:
	{
		iYear = iYear % 400;
		if (!(iYear%4) && (iYear%100)) {
			if ( 1 > iDay || iDay > 29 ) {
				return -5;
			}
		}
		else {
			if ( 1 > iDay || iDay > 28 ) {
				return -5;
			}
		}
		break;
	}
	default:
		return -6;
	}
	
	return 0;
}

int checkDateTimeValid(const char * sDate) {
	if (!sDate) {
		return -1;
	}

	char tempDate[16] = {0};
	strcpy (tempDate, sDate);
	int iSec = atoi (tempDate+12);
	tempDate[12] = 0;
	int iMin = atoi (tempDate+10);
	tempDate[10] = 0;
	int iHour = atoi (tempDate+8);
	tempDate[8] = 0;
	int iDay = atoi (tempDate+6);
	tempDate[6] = 0;
	int iMon = atoi (tempDate+4);
	tempDate[4] = 0;
	int iYear = atoi (tempDate);

	if (iSec > 59 || iSec < 1) {
		return -7;
	}
	if (iMin > 59 || iMin < 1) {
		return -8;
	}
	if (iHour > 24 || iHour < 1) {
		return -9;
	}
	if (iDay > 12 || iDay < 1) {
		return -2;
	}
	if (iMon > 12 || iDay < 1) {
		return -3;
	}
	if (iYear > 9999 || iYear < 1) {
		return -4;
	}

	switch (iMon) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	{
		if ( 1 > iDay || iDay > 31 ) {
			return -5;
		}
		break;
	}
	case 4:
	case 6:
	case 9:
	case 11:
	{
		if ( 1 > iDay || iDay > 30 ) {
			return -5;
		}
		break;
	}
	case 2:
	{
		iYear = iYear % 400;
		if (!(iYear%4) && (iYear%100)) {
			if ( 1 > iDay || iDay > 29 ) {
				return -5;
			}
		}
		else {
			if ( 1 > iDay || iDay > 28 ) {
				return -5;
			}
		}
		break;
	}
	default:
		return -6;
	}
	
	return 0;
}


/*
* 函数功能：在asn元树中加入一个节点
* 输入参数：待加入节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNUTree::UTreeAdd(ASNUTree * node)
{
	if (!node) {										// 参数异常
		printf("ASNUTree::UTreeAdd param err, node is 0x0\n");
		return -1;
	}

	ASNUTree* father = UTreeFind(node->m_usFather);		// 首先在树中查找其父节点

	if (father) {
		if (isLeaf(father->m_oNode)) {
			return father->childAdd(node);				// 如果父节点目前还是叶子节点的话，则直接加入
		}
		else {											// 否则先找到其在父节点下的位置再进行插入
			TList* brother = father->m_oNode.poPrev;
			ASNUTree* finderUTree = listObject(brother, ASNUTree, m_oNode);
			ASNUTree* brotherUTree = listObject(brother, ASNUTree, m_oNode);
			while (brother && brother != &father->m_oNode) {
				finderUTree = listObject(brother, ASNUTree, m_oNode);

				if (!finderUTree) {
					printf("ASNUTree::UTreeAdd momery invalid, finderUTree is 0x0\n");
					return -3;
				}
				else if (node->m_usTag == finderUTree->m_usTag) {
					printf("ASNUTree::UTreeAdd node is repeat, tag is %d\n", node->m_usTag);
					return -4;
				}
				else if (node->m_iSequence < finderUTree->m_iSequence) {
					return brotherUTree->brotherAdd(node);
				}

				brotherUTree = finderUTree;
				brother = brother->poNext;
			}

			if (brother == &father->m_oNode) {
				return brotherUTree->brotherAdd(node);
			}
		}
	}

	printf("ASNUTree::UTreeAdd find the new node father failed!, tag is %d\n", node->m_usTag);
	return -2;											// 如果没有找到其父亲则返回失败
}

/*
* 函数功能：在asn元树找到指定节点
* 输入参数：待加入节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
ASNUTree* ASNUTree::UTreeFind(TAG_UINT16 tag)
{
	if (tag == m_usTag) {
		return this;
	}

	TList* finder = m_oNode.poPrev;
	ASNUTree* finderUTree = NULL;
	while (finder->poNext && finder != &m_oNode) {
		if (!isLeaf((*finder))) {							// 如果是子节点是一棵树，则递归查找
			finderUTree = listObject(finder, ASNUTree, m_oNode)->UTreeFind(tag);
			if (finderUTree) {
				return finderUTree;
			}
		}
		else {
			finderUTree = listObject(finder, ASNUTree, m_oNode);
			if (finderUTree && tag == finderUTree->m_usTag) {
				return finderUTree;
			}
		}

		finder = finder->poNext;
	}

	return NULL;
}

/*
* 函数功能：判断下一个加入的节点是否符合树的规则
* 输入参数：待加入节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNUTree::UTreeCheckField(ASNTree * node, ASNUTree * &next)
{
	if (!node) {										// 参数异常
		printf("ASNUTree::UTreeCheckField param err, node is 0x0\n");
		return -1;
	}

	if (isRoot(node->m_oNode)) {
		if (this->m_usTag != node->m_usTag) {
			printf("ASNUTree::UTreeCheckField check the root node failed\n");
			return -6;
		}
		next = listObject(this->m_oNode.poPrev, ASNUTree, m_oNode);
		return this->doCheck(node);
	}

	TList * tmp = &this->m_oNode;
	ASNUTree * tmpTree = this;
	while (!isRoot((*tmp))) {	
		tmpTree = listObject(tmp, ASNUTree, m_oNode);
		
		if (tmpTree->m_usTag == node->m_usFather) {
			tmp = tmp->poPrev;
			continue;
		}
		else if (tmpTree->m_usFather != node->m_usFather) {
			
			tmp = tmp->poNext;
			
			/* .如果下一个节点不是其父节点则判断下一个兄弟节点是否可选*/
			if (tmpTree->m_usFather != listObject(tmp, ASNUTree, m_oNode)->m_usTag) {	
				tmpTree = listObject(tmp, ASNUTree, m_oNode);
				if (tmpTree->m_iNum != -1 && tmpTree->m_iNum != 0) {
					printf("ASNUTree::UTreeCheckField a must node has not been found, tag is %d\n", tmpTree->m_usTag);
					return -5;							// 必选兄弟字段没有填写
				}
			}
			continue;
		}

		if (tmpTree->m_usTag == node->m_usTag) {
			next = tmpTree;
			return tmpTree->doCheck(node);			// 对字段本身数据进行校验
		}
		else {
			tmp = tmp->poNext;
			TAG_UINT16 father = tmpTree->m_usFather;
			
			/* .如果下一个节点是其父节点则认为新增字段不合法*/
			while (father != listObject(tmp, ASNUTree, m_oNode)->m_usTag) {	
				tmpTree = listObject(tmp, ASNUTree, m_oNode);
				if (tmpTree->m_usTag != node->m_usTag) {
					if (tmpTree->m_iNum != -1 || tmpTree->m_iNum != 0) {
						printf("ASNUTree::UTreeCheckField a must node has not been found, tag is %d\n", tmpTree->m_usTag);
						return -5;						// 必选兄弟字段没有填写
					}
				}
				else {
					next = tmpTree;
					return tmpTree->doCheck(node);		// 对字段本身数据进行校验
				}
			}
			printf("ASNUTree::UTreeCheckField this tree has not the node, tag is %d\n", node->m_usTag);
			return -6;									// 字段没有在该位置被定义
		}
	}

	printf("ASNUTree::UTreeCheckField find the new node father failed!, tag is %d\n", node->m_usTag);
	return -2;											// 没有找到该节点的父节点
}

/*
* 函数功能：判断新节点是否符合本身属性的定义
* 输入参数：待加入节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNUTree::doCheck(ASNTree * node)
{
	if (!node) {
		return -1;
	}

	
	return typeCheck(node->m_usType) || flagCheck(node) || lenCheck(node);
}

