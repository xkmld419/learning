/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef EVENTSENDER_H_HEADER_INCLUDED_BDD94802
#define EVENTSENDER_H_HEADER_INCLUDED_BDD94802
//#include "DistributeRule.h"



class MessageQueue;
class BillConditionMgr;

#include "StdEvent.h"

class StdEvent;
class AcctItemTransData;

//##ModelId=41EF55000310
//##Documentation
//## 事件发送器
class EventSender
{
  public:
    //##ModelId=41EF5A7203BF
    //##Documentation
    //## 发送一个事件，依次调用每个Distributer的send方法
    //## 返回这个事件共发送给哪些进程的列表
   // int * send (StdEvent *evnet);

	// add by zhangch
    // 发送一个buffer，大小为StdEvent的整数size倍，依次调用每个Distributer的send方法
    // 返回这个事件共发送给哪些进程的列表
   // int * send (char *buffer);

	//发送一个帐单数组
	//int * send (AcctItemTransData * itemData,StdEvent * evnet,bool bNew = true);



    //##ModelId=41F603420134
    //##Documentation
    //## 初始化变量，加载分发规则、根据分发规则实例化Distributer
    EventSender(int iProcID);

    //##ModelId=41F603420166
    //##Documentation
    //## 释放所申请的资源
    ~EventSender();
    //##ModelId=428D5B2203B6
    //##Documentation
    //## 返回某File的话单被发送到了哪些进程
    //int *getOutProcessList(int iFileID);


  private:



    //##ModelId=4226D671038B
   // DistributeRule * m_poRuleList;
    //##ModelId=428D4A1000E2
   // BillConditionMgr *m_poBillConditionMgr;
    //##ModelId=428D5B92014A
    //##Documentation
    //## 存放ProcessID的缓冲区，用于getOutProcessList
    int m_aiProcessIDBuf[MAX_OUT_PROCESS_NUM];


	// int  m_iMsgEventNum;  // add by zhangch，单条消息包含的StdEvent个数

};



#endif /* EVENTSENDER_H_HEADER_INCLUDED_BDD94802 */
