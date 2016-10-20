#include "ASNCheck.h"
#include "HSSLogV3.h"
#include "LogV2.h"
#include "ABMException.h"


const char* sQuerySql = "SELECT INDEX, TAG, TYPE, FLAG, MAXLEN, NUM, FATHER, SEQUENCE, MINLEN "
						"FROM ASNDECODECONFIGURATION "
						"ORDER BY INDEX, FATHER, SEQUENCE ";
unsigned int ASNCheck::m_uiCurIdx = 0;
ASNDBConf* ASNDBConf::m_poInstance = NULL;

/*
* 函数功能：对asn子树进行格式校验
* 输入参数：待校验的节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNCheck::checkField(ASNTree * field)
{
	if (!field) {
		return -1;
	}

	if (!m_oFormatMap[m_uiCurIdx]) {
		return  0;										// 没有装载到校验数据则直接返回成功
	}

	ASNUTree * tmp = NULL;

	if (isRoot(field->m_oNode)) {
		m_oFormatMap[m_uiCurIdx]->current = m_oFormatMap[m_uiCurIdx]->UTree;
		m_oFormatMap[m_uiCurIdx]->current->m_iCurNum = 0;
	}
	int iRet = m_oFormatMap[m_uiCurIdx]->current->UTreeCheckField(field, tmp);

	if (m_oFormatMap[m_uiCurIdx]->current == tmp) {
		m_oFormatMap[m_uiCurIdx]->current->m_iCurNum++;

		if (m_oFormatMap[m_uiCurIdx]->current->m_iCurNum > 1 &&
			m_oFormatMap[m_uiCurIdx]->current->m_iCurNum > m_oFormatMap[m_uiCurIdx]->current->m_iNum
			&& m_oFormatMap[m_uiCurIdx]->current->m_iNum > 0) {
			printf("ASNCheck::checkField num of the node is too big!");
			return -9;									// 节点出现次数超出定义的节点数
		}
	}
	else {
		if (m_oFormatMap[m_uiCurIdx]->current->m_usFather != tmp->m_usTag) {
			m_oFormatMap[m_uiCurIdx]->current = tmp;
			m_oFormatMap[m_uiCurIdx]->current->m_iCurNum = 1;
		}
		else {
			m_oFormatMap[m_uiCurIdx]->current = tmp;
			m_oFormatMap[m_uiCurIdx]->current->m_iCurNum++;
			if (m_oFormatMap[m_uiCurIdx]->current->m_iCurNum > 1 &&
				m_oFormatMap[m_uiCurIdx]->current->m_iCurNum > m_oFormatMap[m_uiCurIdx]->current->m_iNum
				&& m_oFormatMap[m_uiCurIdx]->current->m_iNum > 0) {
				printf("ASNCheck::checkField num of the node is too big!");
				return -9;									// 节点出现次数超出定义的节点数
			}
		}
	}
/*
	TList * tmp = m_oFormatMap[2]->current->m_oNode.poNext;

	if (!m_oFormatMap[2]->current->m_iNum) {
		if (isLeaf(m_oFormatMap[2]->current->m_oNode)) {
			
			while(m_oFormatMap[2]->current->m_usFather == listObject(tmp, ASNUTree, m_oNode)->m_usTag) {
				m_oFormatMap[2]->current = listObject(tmp, ASNUTree, m_oNode);
				tmp = m_oFormatMap[2]->current->m_oNode.poNext;
			}
		}
		else {
			tmp = m_oFormatMap[2]->current->m_oNode.poPrev;
		}
		m_oFormatMap[2]->current = listObject(tmp, ASNUTree, m_oNode);
	}
*/

	return iRet;
}


/*
* 函数功能：装载数据字典,这里通过数据库进行配置
* 输入参数：待校验的节点
* 输出参数：无
* 返回值  ：如果成功则返回0，否则返回非零
*/
int ASNDBConf::init()
{
	const char *conInfo = "HSSTT";
	int iRet = GetTTCursor(m_poTTCursor, m_poTTConn, conInfo, oExp);
	if (0 != iRet) {
		directLog(V3_FATAL, V3_DB, MBC_PARAM_LOST_ERROR, "get timesTen Cursor failed(result = %d), conifo is %s.", iRet, conInfo);
		return iRet;
	}

	m_poTTCursor->Close();
	m_poTTCursor->Prepare(sQuerySql);
	m_poTTCursor->Commit();

	try {
		m_poTTCursor->Execute();

		int lastIndex 			= -1;
		long index				= -1;
		long tag				= -1;
		char stag[100];
		long type				= -1;
		long flag 				= -1;
		long len				= -1;
		long num				= -1;
		long father				= -1;
		long sequence			= -1;
		long minlen				= -1;
		ASNFormatData *pData 	= NULL;
		ASNUTree *pUTree		= NULL;
		while(!m_poTTCursor->FetchNext()) {
			pUTree = new ASNUTree();
			m_poTTCursor->getColumn(1, &index);
			m_poTTCursor->getColumn(2, &tag);
			m_poTTCursor->getColumn(2, stag);
			int xxff = m_poTTCursor->getColumnLength(2);
			m_poTTCursor->getColumn(3, &type);
			m_poTTCursor->getColumn(4, &flag);
			m_poTTCursor->getColumn(5, &len);
			m_poTTCursor->getColumn(6, &num);
			m_poTTCursor->getColumn(7, &father);
			m_poTTCursor->getColumn(8, &sequence);
			m_poTTCursor->getColumn(9, &minlen);
			
			pUTree->m_usTag		= tag;
			pUTree->m_usType	= type;
			pUTree->m_usFlag	= flag;
			pUTree->m_uiMaxLen	= len;
			pUTree->m_iNum		= num;
			pUTree->m_usFather	= father;
			pUTree->m_iSequence = sequence;
			pUTree->m_uiMinLen = minlen;
			pUTree->m_usValid	= 1;
			
			if (lastIndex != index ) {
				pData = new ASNFormatData();
				pData->UTree = pUTree;
				pData->current = pUTree;
				lastIndex = index;

				if (m_oFormatMap[index]) {
					printf("一个重复的配置数据出现,请检查数据库配置 index is %d\n", index);
					return -1;
				}
				else if (index >= 20) {
					printf("最多配置20组数据,请检查数据库配置 index is %d\n", index);
					return -1;
				}
				m_oFormatMap[index] = pData;
			}
			else {
				if (pData->current->m_usFather == pUTree->m_usFather) {	// 如果新节点也current是同一个父节点,则直接根据current进行插入
					iRet = pData->current->brotherAdd(pUTree);			// 此处需要重新实现ASNUTree的brotherAdd函数

					if (iRet) {
						break;
					}
				}
				else {
					iRet = pData->UTree->UTreeAdd(pUTree);
					
					if (iRet) {
						break;
					}
				}
			}

		}
	}
	catch(TTStatus st)
	{
		m_poTTCursor->Close();
		__DEBUG_LOG1_(0, "ErrorCode::ParseErrorTable failed! err_msg=%s", st.err_msg);
		iRet = -1;
	}

	if (iRet) {
		destory();
	}
	
	return iRet;
	
}


