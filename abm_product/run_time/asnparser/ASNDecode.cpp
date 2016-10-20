#include <string.h>
#include "ASNDecode.h"
#include "HSSLogV3.h"


unsigned int ASNParser::m_uiCheckIdx = 0;

int ASNParser::DecodeTag()
{
	if (!code || !size) {
		return err_data_invalid;						// 待解析数据不能为空
	}


	/*
		8	7	6	5	4	3	2	1
	  +-------+---+---+---+---+---+---|
	1 | CLASS |P/C| 1 | 1 | 1 | 1 | 1 |
	  +-------------------------------+

		7 \ 8 两位表示类型,6位表示是否原子式

		如果5-1位全为1,则表示tag值大于31,用后续字节表示
	*/

	TOP->m_usType  = (((*code) & 0xC0) >> 6) && 0x00FF;	// 解析字段的type,存在于首字节的最高2位(由右至左)
	TOP->m_usFlag	= (((*code) & 0x20) >> 5) && 0x00FF;// 解析字段的flag,存在于首字节的第6位(由右至左)



	if (((*code) & 0x1F) != 0x1F) {						// tag小于31时直接进行tag读取
		TOP->m_usTag = (TAG_UINT16)(*code & 0x1F) * 1;
		code++;
		size--;
		return 0;
	}
	
	while (size-- && code++ && ((*code) & 0x80)) {		// tag大于31时按字节转换读取
		if (!size || !code ) {
			return err_data_enough;
		}
		
		TOP->m_usTag	*= 1 << 7;						// tag按intergate方式由高位到低位读取
		TOP->m_usTag 	+= (TAG_UINT16)((*code) & 0x7F) & 0x00FF;
		
	};

	{													// 再将最后一位加上
		if (!size || !code ) {
			return err_data_enough;
		}
		
		TOP->m_usTag	*= 1 << 7;						// tag按intergate方式由高位到低位读取
		TOP->m_usTag	+= (TAG_UINT16)((*code) & 0x7F) & 0x00FF;

		size--;
		code++;
	}
	return 0;	
}

int ASNParser::DecodeLen()							// 长度表示内容字段的净长度，不包括内容以外的其它字段的长度
{
	if (!code || !size) {
		return err_data_invalid;						// 待解析数据不能为空
	}

	/*

		8	7	6	5	4	3	2	1
	  +-------------------------------|
	1 | 0	L	L	L	L	L	L	L |
	  +-------------------------------+

		短编码方式: 第8位为0, 7-1位表示真实长度
		长编码方式: 第8位为1, 7-1位表示承载长度的字节数
		变长编码: 该字节位0x80, values通过0x00结束
		
	*/	

	TAG_UINT16 len = (TAG_UINT16)((*code) & 0x7F) & 0x00FF;

	if (0x80 == (*code)) {								// 变长编码则直接赋值位-1
		TOP->m_uiLen 		= 1 << 32;
		size--;
		code++;
	}
	else if (((*code) & 0x80)) {						// 长编码则计算具体长度
		if (len > size) {								// 如果长度不够则返回失败
			return err_data_enough;
		}
		while (len-- && ++code && --size) {
			TOP->m_uiLen 	*= 1 << 8;
			TOP->m_uiLen	+= (TAG_UINT16)(*code) & 0x00FF;
		}

		size--;
		code++;
	}
	else {												// 短编码则赋值之前计算好的长度
		TOP->m_uiLen	= len;
		size--;
		code++;
	}

	return 0;
}

int ASNParser::copyValues()
{
	if (!code || !size) {
		return err_data_invalid;						// 待解析数据不能为空
	}

	if (size < TOP->m_uiLen) {
		return err_data_enough;							// 待解析数据不能小于该字段的len
	}

	if (TOP->m_sValues) {								
		delete TOP->m_sValues;
	}
	
	TOP->m_sValues = new char[TOP->m_uiLen + 1];		// 可以考虑申请一块内存重复使用
	TOP->m_sValues[TOP->m_uiLen] = 0x00;
	memcpy(TOP->m_sValues, code, TOP->m_uiLen);
	size -= TOP->m_uiLen;
	code += TOP->m_uiLen;

	return 0;
}


int ASNParser::decode()
{
	if (!code || !size) {
		return err_data_invalid;						// 待解析数据不能为空
	}

	int	iRet			= 0;
	ASNTree* node		= NULL;

	while (TOP) {
		switch (status) {
		case decode_tag: {			
			if ((iRet = pop())) {							// 首先将解析完成的进行出栈
				return iRet == DECODEOVER ? 0 : iRet;
			}
			
			node 			= new ASNTree();			// 创建一个新节点,用于保存接下来需要解析的内容
			/* 需要先将节点加入树中,再将其加入栈*/
			if ((iRet = encodeTree(node)))	{		// 将新节点加入tree中
				return iRet;
			}
			stack[top++] 	= node;						// push操作将新节点加入stack
			
			if ((iRet = DecodeTag())) {
				break;
			}

			status = decode_len;
			break;
		}
		case decode_len: {
			if ((iRet = DecodeLen())) {
				break;
			}
			sizestack[top - 1] 	= TOP->m_uiLen;			// 新加入节点的数据长度加入栈顶
			
			status = TOP->m_usFlag ? decode_tag : decode_values;
			break;
		}
		case decode_values:	{
			if ((iRet = copyValues())) {
				break;
			}
				
			status = decode_tag;
			break;
		}
		default: {
			destorySite();								// 将现场资源进行释放
			return -3;
		}
		}

		if (iRet && catchException((err)iRet)) {
			printf("ASNParser::decode error, error no is %d!\n", iRet);
			destorySite();								// 将现场资源进行释放
			return iRet;
		}
	}

	return 0;
}


int ASNParser::encodeTree(ASNTree* node)
{
	if (!node) {
		return err_param_invalid;
	}

	if (!record) {
		record	= node;
		return check();
	}

	int iRet = TOP->childAdd(node);
	return iRet ? iRet : check();
}

int ASNParser::pop()
{
	unsigned int len	= 0;
	int 		 iRet	= 0;
	if (top) {											// 首次解析root节点时不进行出栈判断

		if (!TOP->m_usFlag) {							// 如果是结构体则不进行pop操作
			TOPSIZE 	-= TOP->m_uiLen;
		}
		
		iRet	= ASNDBConf::getInstance(m_uiCheckIdx)->checkField(TOP);
		if (iRet) {
			return iRet;
		}
		while (!TOPSIZE ){								// 如果栈顶元素已经完全解析完则pop出来

			/* 这里不仅仅需要减去values长度,也需要计算tag\len所占的字节*/
			len 	= TOP->m_uiLen + tagCode(TOP->m_usTag) + lenCode(TOP->m_uiLen);
			if (!--top) { 								// 如果最后一个根节点弹出则认为解码结束
				return DECODEOVER;
			}
			TOPSIZE -= len;
		}

	}

	return 0;
}

int ASNParser::catchException(err &iRet)
{
	return  iRet;
}

int ASNParser::destorySite()
{
	for (int i = 0; i < 20; i++) {
		stack[i] = NULL;
		sizestack[i] = 0;
	}

	code = NULL;
	current = NULL;
	top = 0;
	size = 0;
	status = decode_tag;

	if (record) {
		delete record;
		record = NULL;
	}
	return  0;
}

ASNFileParser* ASNFileParser::m_poInstance = NULL;

int ASNFileParser::decode(const char* sFileFullName)
{
	if (NULL == sFileFullName) {						// 参数校验
		return err_param_invalid;
	}

	if (m_iFd) {										// 如果之前已经打开过文件则需要清理之前环境
		destorySite();									// 清理之前解析记录

		if  (close(m_iFd) != 0) {						// 关闭之前文件
			if (errno != EINTR) {
				perror("fclose");
				return err_file_close;
			}
		}

	}
	
	m_iFd = 0;
	m_uiCacheSize = 0;
	status = decode_tag;
	
	if (0 == (m_iFd = open(sFileFullName, O_RDONLY))) {	// 打开文件
        if (errno != EINTR) {
			DIRECT_LOG(V3_ERROR, V3_SERVICE, MBC_FILE_OPEN_FAIL, "OPEN FILE FAILED");
            perror("fopen");
            return err_file_open;
        }
	}

	int iRet = 0;
	if ((iRet = cacheBuff())) {									// 将文件中数据装载入缓冲进行解析
		return iRet;
	}

	return ASNParser::decode();
}
int ASNFileParser::cacheBuff()
{
	if (m_uiCacheSize == BIGSIZE) {						// 如果装载的文件数据已经超过256m了，则咨询用户是否需先数据缓冲数据
		return err_big_size;
	}
	
	if (size && code != m_sCacheBuff) {
		char * tmp = m_sCacheBuff;
		for (int i = 0; i < size; i++) {
			*tmp = *code;
			tmp++;
			code++;
		}

	}

	int bytes = read(m_iFd, m_sCacheBuff + size, CACHESIZE - size);

	if (!bytes) {
		return err_file_null;							// 表示文件已经读完
	}
	else if (-1 == bytes) {
		return err_file_read;							// 读取文件失败
	}
 
	size += bytes;
	m_uiCacheSize += bytes;
	code = m_sCacheBuff;

	return 0;
}

int ASNFileParser::catchException(err &iRet)
{
	switch(iRet) {
	case err_data_invalid: {
		if (!(size && !code)) {
			iRet = (err)cacheBuff();
		}
		break;
	}
	case err_check_byUTree: {
		break;
	}
	case err_data_enough: {
		iRet = (err)cacheBuff();
		break;
	}
	case err_add_child: {
		break;
	}
	default: {
		break;
	}
	}

	return iRet;
}

int ASNFileParser::freeSomeNode()
{
	
	return 0;
}

ASNMessageParser* ASNMessageParser::m_poInstance = NULL;

int ASNMessageParser::decode(char* sCode, unsigned int len)
{
	if (!sCode || !len) {								// 参数校验
		return err_param_invalid;
	}

	destorySite();										// 清理之前解析记录

	status = decode_tag;
	size = len;
	code = sCode;

	return ASNParser::decode();
}

ASNFileMutiParser* ASNFileMutiParser::m_poInstance = NULL;

int ASNFileMutiParser::decode(const char* sFileFullName)
{
	if (NULL == sFileFullName) {						// 参数校验
		return err_param_invalid;
	}

	if (m_iFd) {										// 如果之前已经打开过文件则需要清理之前环境
		destorySite();									// 清理之前解析记录
		freeList();
		if  (close(m_iFd) != 0) {						// 关闭之前文件
			if (errno != EINTR) {
				perror("fclose");
				return err_file_close;
			}
		}

	}
	
	m_iFd = 0;
	m_uiCacheSize = 0;
	status = decode_tag;

	
	
	if (0 == (m_iFd = open(sFileFullName, O_RDONLY))) {	// 打开文件
        if (errno != EINTR) {
			DIRECT_LOG(V3_ERROR, V3_SERVICE, MBC_FILE_OPEN_FAIL, "OPEN FILE FAILED");
            perror("fopen");
            return err_file_open;
        }
	}

	struct stat statbuf;
	if (stat (sFileFullName, &statbuf)) {
		perror("stat");
		return err_file_open;
	}
	m_uiFileSize = statbuf.st_size;
	
	int iRet = 0;
	if ((iRet = cacheBuff())) {									// 将文件中数据装载入缓冲进行解析
		return iRet;
	}

	m_poHead = NULL;
	m_uiNum = 0;
	TRecordList * tmpnew = NULL;
	while(!(iRet = ASNParser::decode())) {
		
		tmpnew = new TRecordList();
		m_uiNum++;
		if (!m_poHead) {
			m_poHead = tmpnew;
		}
		else {													// 组织一个双向链表
			tmpnew->list.poNext = &m_poHead->list;			
			tmpnew->list.poPrev = m_poHead->list.poPrev;
			m_poHead->list.poPrev->poNext = &tmpnew->list;
			m_poHead->list.poPrev = &tmpnew->list;
		}

		record = NULL;
		current = NULL;
		status = decode_tag;
		for (int i = 0; i < 20; i++) {
			stack[i] = NULL;
			sizestack[i] = 0;
		}
		top = 0;
		
		if (m_uiFileSize == m_uiCacheSize && !size) {
			break;
		}

	}

	if (iRet) {
		freeList();
	}
	
	return iRet;
}

int ASNFileMutiParser::freeList()
{
	TRecordList * tmp = m_poHead;
	TList * tmpList = NULL;
	while (tmp && m_uiNum--) {
		delete tmp;

		tmpList = tmp->list.poNext;
		tmp = listObject(tmpList, TRecordList, list);

		if (tmp == m_poHead) break;
	}

	return 0;
}
