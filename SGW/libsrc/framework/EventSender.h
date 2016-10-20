// Copyright (c) 2011
// All rights reserved.

#ifndef EVENTSENDER_H_HEADER_INCLUDED_BDD948082
#define EVENTSENDER_H_HEADER_INCLUDED_BDD948082

//#include "Process.h"
#include "SGWSHMParamApi.h"


class MessageQueue;

#include <vector>

using namespace std;
class InnerStruct;

typedef struct 
{
	int		iSendConditionID;
	int		iMqId;
	int		iSendMode;
	int		iBuffType;
	MessageQueue * pmq;
}TRouterRule;

//## 事件发送器
class EventSender
{
  public:

    //## 初始化变量，加载分发规则、根据分发规则实例化Distributer
    EventSender(int iAppID);

    //## 释放所申请的资源
    ~EventSender();

	//## 发送变长缓冲
	int send(long lMtype=0);
	int send(string & strSend,ProtoToBusiData & tProtoData,long lMtype=0);

  private:

	vector<TRouterRule> VTRouterRule;

   
   int m_iSendCondition;
   
   CongestLevel m_tCongestLevel;
   
	ServiceContextListBaseData  tmpServiceContextListData;
	UserInfoData  tmpUserInfoData;
	NetInfoData tmpNetInfoData;   
   
   int m_iMaxLev;
   
   int m_iLevSum[10];
   
   int m_iNetSum[10];
   
   int m_iPriorityType;

   InnerStruct * m_poInnerStruct;
};



#endif /* EVENTSENDER_H_HEADER_INCLUDED_BDD94802 */
