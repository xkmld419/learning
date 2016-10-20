/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef EVENTRECEIVER_H_HEADER_INCLUDED_BDC328FA
#define EVENTRECEIVER_H_HEADER_INCLUDED_BDC328FA

#include "Exception.h"
#include "MessageQueue.h"


#include "StdEvent.h"

class StdEvent;
class AcctItemTransData;

//##ModelId=41EF57D20051
//##Documentation
//## 事件接收器，从消息队列接收事件
class EventReceiver
{
  public:
    //##ModelId=41EF5A6C0117
    //##Documentation
    //## 从消息队列接收事件
    int receive(
        //##Documentation
        //## 接收存放到的缓冲区
        StdEvent *pEvent, bool ifBlock = true);

	// add by zhangcch
	// 从消息队列接收buffer，buffer大小为StdEvent的整数size倍
	int receive(char *buffer, bool ifBlock = true);

	//从消息队列接收一个帐单数组
	int receive(AcctItemTransData * itemData, bool ifBlock = true);


    //##ModelId=41F6032A020C
    //##Documentation
    //## 初始化变量，实例化消息队列访问对象，连接消息队列
    EventReceiver(int iProcID);

    //##ModelId=41F6032A0252
    //##Documentation
    //## 释放所申请的资源
    ~EventReceiver();

  private:

    //##ModelId=4226C8C20370
    MessageQueue * m_poMQ;


    
    // 是否阻塞, add by yangks 2007.02.02
    bool m_bIfBlock;
    int  m_iMaxNum;
	int  m_iMsgEventNum;  // add by zhangch，单条消息包含的StdEvent个数
};



#endif /* EVENTRECEIVER_H_HEADER_INCLUDED_BDC328FA */
