#ifndef _ABM_MESSAGEQUEUE_H_
#define _ABM_MESSAGEQUEUE_H_

#include <string>
#include "ABMException.h"

using namespace std;

//##ModelId=4314101B0389
//##Documentation
//## 消息队列封装类，
//## 当SYSTEM V标准的时候，要求#define SYSTEM_V

/******************************************************************************
	在SYSTEM_V情况下和POSIX情况下，MessageQueue使用方法的区别：

	操作	SYSTEM_V					POSIX
	----------------------------------------------------------------
	创建	要求有创建者有超级用户		不需要
		权限，因为默认只有16k
		大小的控件，通常需要扩
		展空间

	打开	要求所以使用该消息队列		打开的时候无需制定，自动获取
		的程序在打开或创建的时		创建时用的参数值。
		候使用的maxnum，maxlen
		相同。

	关闭	其实没有关闭				关闭了

	其他一样

	参考资料：SYSTEM_V if cmd is IPC_SET,an attempt is being made
			to increase to the value of msg_qbytes,and the calling process does not
		    have the SUPER-USER PRIVILEGE.

	当系统POSIX的时候，建议使用POSIX的消息队列。
******************************************************************************/

struct msg_rbuf
{
   long mtype;     /* message type, must be > 0 */
   char mtext[1025];  /* message data */
};

class MessageQueue
{
  public:
    //##ModelId=4314101D004F
    //##Documentation
    //## 打开消息队列，制定方式：
    //## 1，是否阻塞
    //## 2，如果不存在是否创建。
    int open(ABMException &oExp, bool ifblock = true, bool ifcreate = true);

    //##ModelId=4314101D0059
    //##Documentation
    //## 删除一个已经存在的队列
    int remove(ABMException &oExp);

    //##ModelId=4314101D005A
    //##Documentation
    //## 断开到某队列的连接
    void close();

    //##ModelId=4314101D005B
    //##Documentation
    //## 返回该队列是否存在
    bool exist();

    //##ModelId=4314101D0066
    //##Documentation
    //## 队列内的消息条数
    int getMessageNumber();

    MessageQueue(int key);

    //##ModelId=4314101D0069
    //##Documentation
    //## 返回消息队列的IPCID
    int getSysID();

    //##ModelId=4314101D006A
    //##Documentation
    //## 返回使用百分比，精确到整数
    int getOccurPercent();

    //##ModelId=4314101D006B
    ~MessageQueue();

    //##ModelId=4331083B0017
    void setBlockFlag(bool ifblock = true) { m_iBlock = ifblock;}

    bool getBlockFlag(){return m_iBlock; };

	
  public:
    //add by huff 支持带消息优先级发送
    /*
    *   函数功能: 从消息队列接收消息优先级为mqtype,最大为maxLen的内容
    *   参数:
    *       void *buf: 接收缓冲区的首地址struct my_msgbuf { mtyp_t mtype;
    *                                                     char  mtext[MSGSIZ]; } buf
    *       int maxLen: 接收缓冲区的长度
    *       long mqtype: 接收的消息优先级
    *       bool inter: 阻塞接收时 是否被信号中断
    *   返回: 成功返回接收的消息长度, 失败返回-1or -2(被信号中断)
    */
    int Receive( void *buf, int maxLen, long mqtype, bool inter=false, bool forcenoblock=false);
    
    /*
    *   函数功能: 将首地址为void *buf, 长度为len的内容发往消息队列
    *   参数:
    *       void *buf : 实际传入类型为struct my_msgbuf { mtyp_t mtype;
    *                                                     char  mtext[MSGSIZ]; }
    *       int len: 待发送数据的总长度, 包括sizeof(mtyp_t)包头
    *       bool &inter: 阻塞发送时, 被信号中断是否返回, inter==true 返回-2
    *   返回: 成功返回发送长度, 失败返回-1 or -2(被信号中断)
    */
    int Send( void *buf, int len, bool inter=false, bool forcenoblock=false);   
    
  private:
    //##ModelId=4314101D006D
    int _open(ABMException &oExp);

    //##ModelId=4314101D006E
    int _create(ABMException &oExp);

    //##ModelId=4314101D0032
    int m_iKey;

    //##ModelId=4314101D0039
    int m_iSysMQID;

    //##ModelId=4314101D0045
    int m_iBlock;

    //##ModelId=4314101D0046
    bool m_bAttached;
};

#define MQ_PERMISSION	0666

#endif
