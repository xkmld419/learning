/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef DISTRIBUTERULE_H_HEADER_INCLUDED_BDB2D7D5
#define DISTRIBUTERULE_H_HEADER_INCLUDED_BDB2D7D5

#include "CommonMacro.h"
#include "MessageQueue.h"
#include "KeyList.h"
class SendRecord;

//##ModelId=41F4ED18001F
//##Documentation
//## 分发规则，对应数据库里面的配置
class DistributeRule
{
  public:
    //##ModelId=424CFCE6018D
    DistributeRule(MessageQueue *poMQ, int conID, DistributeRule *next, int iOutProcessID);

    //##ModelId=41F4F02702A6
    //##Documentation
    //## 条件标识
    int m_iConditionID;

    //##ModelId=424BED8E0348
    MessageQueue *m_poMQ;

    //##ModelId=424CFC3302E4
    DistributeRule *m_poNext;
    //##ModelId=428D5A6D0120
    int m_iOutProcessID;

	int m_iMsgEventNum;  // add by zhangch，单条消息包含的StdEvent个数

    //##ModelId=429141D00057
    KeyList <SendRecord *> *m_poSendRecord;

};

#endif /* DISTRIBUTERULE_H_HEADER_INCLUDED_BDB2D7D5 */
