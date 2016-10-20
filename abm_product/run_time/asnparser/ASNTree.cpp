#include "ASNTree.h"
#include <string.h>
#include <unistd.h>

/*
* 函数功能：判定主机是否高字节序
* 输入参数：无
* 输出参数：无
* 返回值  ：1-高端字节序, 0-低端字节序
*/
int isBigEndian()
{
	union {
		short 		n;
		char		s[sizeof(short)];
	} un;

	un.n 	= 0x0102;
	if (0x01 == un.s[0] && 0x02 == un.s[1]) {
		return 1;
	}

	return 0;
}

/*
* 函数功能：按文本格式打印树的叶子节点
* 输入参数：无
* 输出参数：无
* 返回值  ：0成功, 1失败
*/
int printRecord(ASNTree* node, void * data)
{
	if (!node) {
		return -1;
	}
	
	if (1 == node->m_usFlag) {								// 结构类型不打印
		return 0;
	}

	switch(node->m_usFlag) {
	case ASN_FLAG_LONG : {
		printf("%l", node->asLong());
		break;
	}
	case ASN_FLAG_INT : {
		printf("%d", node->asInt());
		break;
	}
	case ASN_FLAG_STRING :
	case ASN_FLAG_DATE :
	case ASN_FLAG_CHOICE : {
		printf("%s", node->asString());
		break;
	}
	default: {
		node->traceValues();		
		return -1;
	}
	}

	if (node->m_usFather == node->listNext()->m_usTag) {
		printf("\n");
	}
	else {
		printf("|");
	}

	return 0;
}

/*
* 函数功能：按文本格式打印树的叶子节点到文件中
* 输入参数：无
* 输出参数：无
* 返回值  ：0成功, 1失败
*/
int printRecordToFile(ASNTree* node, void * data)
{
	if (!node || !data) {
		return -1;
	}
	
	if (1 == node->m_usFlag) {								// 结构类型不打印
		return 0;
	}

	int * fd = static_cast<int*>(data);
	if (!fd || !(*fd)) {
		return -2;											// 文件句柄指针位空，或者句柄id位0则返回异常
	}
	static char sData[128] = { 0 };
	static char* pData = NULL;
	
	switch(node->m_usFlag) {
	case ASN_FLAG_LONG : {
		snprintf(sData, 128, "%l", node->asLong());
		write(*fd, sData, strlen(sData));
		break;
	}
	case ASN_FLAG_INT : {
		snprintf(sData, 128, "%d", node->asInt());
		write(*fd, sData, strlen(sData));
		break;
	}
	case ASN_FLAG_STRING :
	case ASN_FLAG_DATE :
	case ASN_FLAG_CHOICE : {
		write(*fd, node->asString(), node->m_uiLen);
		break;
	}
	default: {
		node->traceValues();
		return -1;
	}
	}

	if (node->m_usFather == node->listNext()->m_usTag) {
		write(*fd, "\n", 1);
	}
	else {
		write(*fd, "|", 1);
	}

	return 0;
}


/*
* 函数功能：对节点本身数据做处理，不包含其子节点
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNTree::process()
{
	return CMoreNodeTree::process();
}

/*
* 函数功能：按先序遍历树节点
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNTree::fetchProcess(PROCESS fn, void * data)
{
	TList* node = m_oNode.poPrev;
	int iRet = 0;
	
	iRet = fn(this, data); 									// 遍历到节点则先进行打印
	if (iRet) {
		printf("ASNTree::fetchNode call fn failed, this tag is %d\n", this->m_usTag);
		return iRet;
	}
	while (node->poNext && node != &m_oNode) {
		if (!isLeaf((*node))) {							// 如果是子节点是一棵树，则递归遍历
			iRet = listObject(node,ASNTree,m_oNode)->fetchProcess(fn, data);
			if (iRet) {
				return iRet;
			}
		}
		else {
			iRet = fn(listObject(node,ASNTree,m_oNode), data); 				// 遍历到叶子节点则进行打印
			if (iRet) {
				printf("ASNTree::fetchNode call fn failed, the tag is %d\n", listObject(node,ASNTree,m_oNode)->m_usTag);
				return iRet;
			}
		}
		node = node->poNext;							// 进行下一个兄弟节点的遍历
	}
	return 0;
}

/*
* 函数功能：打印节点key，此处用于测试
* 输入参数：无
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNTree::printKey()
{	
	printf("%d( %04d ) =", m_usTag, m_uiLen);				// 按tag(len):values格式打印出来,用于测试
	traceValues();
	return 0;
}

/* asn协议基本类型的解码函数 */
int ASNInt::decode()
{
    if (NULL == m_sValues || 0 == m_uiLen || m_uiLen > sizeof(int)) {
        return DEFAULTINT;
    }

    char *p = const_cast<char*>(m_sValues);

    while(m_uiLen--) {									// 此处按int型由高到低字节序进行解码
        m_iData *= 1 << 8;
        m_iData += 0x000000ff&((unsigned int )(*p));
        p++;
    }
    m_usValid = 1;

    return m_iData;
	
}

/* asn协议基本类型的解码函数 */
long ASNLong::decode()
{
    if (NULL == m_sValues || 0 == m_uiLen || m_uiLen > sizeof(long)) {
        return DEFAULTLONG;
    }

    int bytes = 1;
    char *p = const_cast<char*>(m_sValues);
    while(m_uiLen--) {									// 此处按int型由高到低字节序进行解码
        m_lData *= bytes;
        m_lData += 0x000000ff&((unsigned int )(*p));
        bytes = 1 << 8;
        p++;
    }
    m_usValid = 1;

    return m_lData;
}

/* asn协议基本类型的解码函数 */
const char* ASNString::decode()
{
    m_usValid = 1;
	m_sData = m_sValues;
	
    return m_sData;
}


