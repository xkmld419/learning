/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef MESSAGEQUEUE_EX_H_HEADER_INCLUDED_BDFE1869
#define MESSAGEQUEUE_EX_H_HEADER_INCLUDED_BDFE1869

#include "Exception.h"
//##ModelId=41DB83BD00A6
//##Documentation
//## 消息队列封装类，
//## 支持POSIX和SYSTEM V两种标准
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


class MessageQueueEx
{
  public:
    //##ModelId=41DCE72A01B6
    //##Documentation
    //## 打开消息队列，制定方式：
    //## 1，是否阻塞
    //## 2，如果不存在是否创建。
    void open(bool ifblock = true, bool ifcreate = true, int maxlen = 2048, int maxnum = 10000 );

    //##ModelId=41DCE731009D
    //##Documentation
    //## 删除一个已经存在的队列
    void remove();
	
    //##ModelId=41DCE73802A6
    //##Documentation
    //## 断开到某队列的连接
    void close();

    //##ModelId=41DCE73F033C
    //##Documentation
    //## 返回该队列是否存在
    bool exist();

    //##ModelId=41DCE76F01B5
    //##Documentation
    //## 发送一段数据
    int send(void *data, int len);
	int send(void* data,long lType,int len);
    //##ModelId=41DCE77202B4
    //##Documentation
    //## 从消息队列接收一段数据
    int receive(char *buf);

	int receive(char* buf,unsigned int iLen);
	int receive(char* buf,long lType,unsigned int iLen);	//=SJ\V86(@`PM5DO{O"
    //##ModelId=41DCE77902FA
    //##Documentation
    //## 队列内的消息条数
    int getMessageNumber();

    //##ModelId=4200B83B0109
    //##Documentation
    //## 构造函数
    /*MessageQueue(
        //##Documentation
        //## 消息队列的名字，要求：
        //## 是一个文件或者目录的完整绝对路径，如：
        //## /home/bill/mqfile/process1000.input
        //## （注意，路径中不能包括"."，".."，如：/home/bill/../bill/mqfile/process1000.input）
        char *mqname);*/
    MessageQueueEx(int key);

    //##ModelId=42017C500050
    //##Documentation
    //## 返回消息队列的IPCID
    int getSysID();

    //##ModelId=42017CA40228
    //##Documentation
    //## 返回使用百分比，精确到整数
    int getOccurPercent();

    //##ModelId=42018258024F
    ~MessageQueueEx();
    //##ModelId=4219719B0006
    operator bool();

	enum ERRTYPE
	{
		E_PERM = 1,
		E_NOENT,
		E_INVAL,
		E_EXIST,
		E_NOMEM,
		E_SYS,
		E_NOMSG,
		E_INTR,
		E_CLOSE,
		E_FULL,
		E_UNKNW
	};
  private:
    //##ModelId=4201CC4C01B2
    bool _open();

    //##ModelId=4201CC5203B9
    bool _create();
    //##ModelId=421590F302CE
    int _getStandardError(int err);

    int m_iKey;

    //##ModelId=41F4B0100358
    int m_iSysMQID;

    //##ModelId=420087E102BF
    char *m_sMQName;

    //##ModelId=4200CDE6012D
    //##Documentation
    //## 容纳的最大消息数
    int m_iMaxNum;

    //##ModelId=4200CE0003D3
    //##Documentation
    //## 每条消息的最大长度
    int m_iMaxLen;

    //##ModelId=4200CE3A0304
    int m_iBlock;

    //##ModelId=4200CE6001D2
    bool m_bAttached;
    //##ModelId=4215913D0342
    int m_iErrorNo;


    //##ModelId=421591750375
    char *m_pBuff;
	
	char* m_pBuffEx;
	unsigned int m_iBuffExSize;

};

#ifdef SYSTEM_V
#define MQ_PERMISSION	0666
#else
#define MQ_PERMISSION	0660
#endif

#endif /* MESSAGEQUEUE_H_HEADER_INCLUDED_BDFE1869 */
