// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef EVENTRECEIVER_H_HEADER_INCLUDED_BDC329FA
#define EVENTRECEIVER_H_HEADER_INCLUDED_BDC329FA

#include "Exception.h"
#include "MessageQueue.h"
#include "SgwDefine.h"
#include <vector>

using namespace std;

typedef struct 
{
	int		m_iBuffType;
	MessageQueue * pmq;
}TReceiveInfo;
class InnerStruct;

//## 事件接收器，从消息队列接收事件
class EventReceiver
{
  public:
    //## 初始化变量，实例化消息队列访问对象，连接消息队列
    EventReceiver(int iProcID);

    //## 释放所申请的资源
    ~EventReceiver();

    //## 从消息队列变长缓冲,接收数据
    int receive(long lMtype=0 , bool bParse=true);

	int receive(ProtoToBusiData & tProtoData, long lMtype=0 , bool bParse=true);
    
    int	receive(vector<ProtoToBusiData> &VTProtoToBusiData,long lMtype=0 , bool bParse=true);

    //## 从消息队列变长缓冲,发送数据
    int send(long lMtype=0 );

	void setBlockFlag(bool ifblock=true);

	int getMessageNumber();

  private:
  	
  	vector<TReceiveInfo> VTReceiveInfo;
  	
  	vector<ProtoToBusiData> VTProtoToBusiData;

//	int m_iBuffType;
	
	int m_iPriorityType;
	
	int	m_iPriorityLev;
	
	int	m_iReceiveSum;
	
	int m_iLevSum[10];
	
	int	m_iNetPrior;
	
	int m_iNetSum[10];
//    MessageQueue * m_poMQ;
	InnerStruct * m_poInnerStruct;
};



#endif /* EVENTRECEIVER_H_HEADER_INCLUDED_BDC328FA */
