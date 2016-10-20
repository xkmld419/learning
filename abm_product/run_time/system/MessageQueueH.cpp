// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "MessageQueueH.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#ifndef NULL
#define NULL 0
#endif

/*
##	设计思路：
##	2，使用方式，
##				构造：MessageQueue *p = new MessageQueue("/home/bill");
##				判断：if (!p || !(*p)) throw 1;
##				使用...
##
*/

//##ModelId=4314101D004F
//void open(bool ifblock = true, bool ifcreate = true, int maxlen = 2048, int maxnum = 1000, int buflen = -1 );
int MessageQueue::open(ABMException &oExp, bool ifblock, bool ifcreate)
{
	m_iBlock = ifblock;

	//##如果已经open了，不再调用
	if (m_bAttached) return 0;

__OPEN:
	//##没有open，先看看能不能直接open
	int iRet = _open (oExp);

	if (!iRet) return 0;

	//##检查错误原因
	if (iRet != ENOENT) {
        ADD_EXCEPT0(oExp, "MessageQueue::_open() failed");
        return iRet;
    }

	//##检查消息创建参数
	if (!ifcreate) {
        ADD_EXCEPT1(oExp, "MessageQueue::_open() failed: MessageQueue 0x%x not exist", m_iKey);
        return iRet;
    }

    oExp.clear ();

	//##试着创建
	iRet = _create (oExp);
	if (!iRet) return 0;

	//##检查错误原因
	if (iRet != EEXIST) {
        ADD_EXCEPT0(oExp, "MessageQueue::_create() failed");
        return iRet;
    }

	goto __OPEN;
}

//##ModelId=4314101D0059
int MessageQueue::remove(ABMException &oExp)
{
	//##如果不存在
	if (!exist()) return 0;

	//##SYSTEM_V下面的操作
	int iRet = _open (oExp);

	if (!iRet) {
		//##异常处理
		if (iRet == ENOENT) goto _TRUE;
		else {
            ADD_EXCEPT0(oExp, "MessageQueue::_open() failed");
            return iRet;
        }
	}

	if (msgctl (m_iSysMQID, IPC_RMID, NULL)) {
		//##异常处理
		if (errno == EIDRM)
			goto _TRUE;
		else {
            ADD_EXCEPT1(oExp, "msgctl (IPC_RMID) failed, ERRNO: %d", errno);
			return errno;
        }
	}

_TRUE:
	m_bAttached = false;
	return 0;
}

//##ModelId=4314101D005A
void MessageQueue::close()
{
	return;
}

//##ModelId=4314101D005B
bool MessageQueue::exist()
{
	//##如果已经open了，那一定存在了
	if (m_bAttached)
		return true;

    ABMException oExp;
	//##如果能直接打开，说明存在
	int iRet = _open (oExp);
	if (!iRet) {
		//##不管关闭是否成功
		close();
		return true;
	} else {

		if (iRet == EPERM) return true;

		return false;
	}
}

//##ModelId=4314101D0066
int MessageQueue::getMessageNumber()
{
	if (!m_bAttached) {
		return 0;
	}

	struct msqid_ds attr;

	//##获取消息队列相关信息
	if (msgctl (m_iSysMQID, IPC_STAT, &attr)) {
		//##异常处理
		if (errno == ENOENT) m_bAttached = false;

		return 0;
	}

	return attr.msg_qnum;
}

//MessageQueue::MessageQueue(char *mqname)
//##ModelId=4314101D0067
MessageQueue::MessageQueue(int key)
{

	m_iSysMQID = -1;
	m_iBlock = 0;
	m_bAttached = false;

	//##十进制的数转成十六进制的数，1000->0X1000
	//##方便去找队列

	{
		int iTemp = 0;

		m_iKey = 0;
		while (key) {
			m_iKey |= ((key %10) << iTemp);
			key = key / 10;
			iTemp += 4;
		}
	}

}

//##ModelId=4314101D0069
int MessageQueue::getSysID()
{
	return m_iSysMQID;
}

//##ModelId=4314101D006A
int MessageQueue::getOccurPercent()
{

	if (!m_bAttached) {
		return 0;
	}

	struct msqid_ds attr;

	//##获取消息队列相关信息
	if (msgctl (m_iSysMQID, IPC_STAT, &attr)) {
		//##异常处理
		if (errno == ENOENT)
			m_bAttached = false;

		return -1;
	}

	if (!attr.msg_qbytes) return 0;

	return attr.msg_cbytes * 100 / attr.msg_qbytes;
}

//##ModelId=4314101D006B
MessageQueue::~MessageQueue()
{
	close ();
}

//##ModelId=4314101D006D
int MessageQueue::_open(ABMException &oExp)
{
	if (m_bAttached) return 0;

	//##SYSTEM_V下的操作
	m_iSysMQID = msgget (m_iKey, MQ_PERMISSION);  //若成功则为消息队列描述字若出错则为-1。
	if (m_iSysMQID == -1) { //##异常处理
        //ADD_EXCEPT3(oExp, "msgget (0x%x, %o) failed, ERRNO:%d", m_iKey, MQ_PERMISSION, errno);
        //oExp.printErr();
		return errno;
	}

	m_bAttached = true;
	return 0;
}

//##ModelId=4314101D006E
int MessageQueue::_create(ABMException &oExp)
{
	//##SYSTEM_V下面的操作
	m_iSysMQID = msgget (m_iKey, MQ_PERMISSION | IPC_CREAT | IPC_EXCL);  //若成功则为消息队列描述字若出错则为-1。

	if (m_iSysMQID == -1) {
		//##异常处理
        ADD_EXCEPT3(oExp, "msgget (0x%x, %o) failed, ERRNO:%d",
                m_iKey, MQ_PERMISSION|IPC_CREAT|IPC_EXCL, errno);
		return errno;
	}


	m_bAttached = true;
	return 0;
}

//##ModelId=4314101D0064
int MessageQueue::Receive(void *buf, int maxLen, long mqtype, bool inter, bool forcenoblock)
{
	//##如果没有消息，返回空
	/*
		2005-05-24
		接收消息的时候，忽略信号引起的中断
	*/
	int ret;
    
	__RECEIVE:

	if (forcenoblock)    //这里的MSG_NOERROR是否需要, 如果接收缓存小于消息长度 会截断 20110401 DELETED BY huff 
		ret = msgrcv (m_iSysMQID, buf, maxLen, mqtype, IPC_NOWAIT);
	else 
		ret = msgrcv (m_iSysMQID, buf, maxLen, mqtype, (m_iBlock ? 0 : IPC_NOWAIT));

    if (ret < 0) {
		if (errno == ENOMSG) 
		    return 0;
		else if (errno == EINTR) {
		    if (inter) 
				return -2;
            else
		    	goto __RECEIVE;
		}		
		else 
		    return -1;
	}

	return ret+sizeof(long);
}


/*
*   函数功能: 将首地址为void *buf, 长度为len的内容发往消息队列
*   参数:
*       void *data : 实际传入类型为struct my_msgbuf { mtyp_t mtype;
*                                                     char  mtext[MSGSIZ]; }
*       int len: 待发送数据的总长度, 包括sizeof(mtyp_t)包头
*       bool &inter: 阻塞发送时, 被信号中断是否返回, inter==true 返回-2
*		bool forcenoblock: 发送时强制非阻塞
*   返回: 成功返回发送长度, 失败返回-1 or -2
*/
int MessageQueue::Send(void *buf, int len, bool inter, bool forcenoblock)
{
	/*
	*  传入的类型 struct my_msgbuf
	*  struct my_msgbuf { mtyp_t mtype;
    *                     char   mtext[MSGSIZ]; } buf;
	*/
	int ret;
	int _len = len - sizeof(long);
	
	if (*(long *)buf == 0) 
	    *(long *)buf = 9999;    
	
__SEND1:
	
	if (forcenoblock)
		ret = msgsnd (m_iSysMQID, buf, _len, IPC_NOWAIT);
	else	 
		ret = msgsnd (m_iSysMQID, buf, _len, (m_iBlock ? 0 : IPC_NOWAIT));

	if (ret == -1) {
		
	    switch (errno) {
	    case ENOMEM:
			return 0;
	    case EAGAIN:
	        return 0;
	    case EINTR:
	        if (inter)  
	        	return -2;
	        else	
	        	goto __SEND1;
	    default:
	        return -1;
	    }
	}

	return len;
}
