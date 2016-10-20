// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "EventReceiver.h"
#include "Log.h"
#include "Process.h"
#include "ReadIni.h"
#include "DateTime.h"

EventReceiver::EventReceiver(int iProcID)
{
	char sql[2048];
	//SQLITEQuery qry;
	DEFINE_QUERY(qry);
	/*
	 if(iProcID/1000==4)
	 {
	 sprintf (sql, "select a.MQ_ID, \
       				a.MQ_SIZE, \
       				a.MAX_MSG_NUM, \
       				a.MQ_BUFF_TYPE ,\
       				c.PRIORITY_TYPE \
  					from wf_mq_attr a, WF_PROCESS_MQ b, wf_process c  \
   				 where  b.process_id=c.process_id \
   					and a.mq_id = b.mq_id and b.PROCESS_ID in (4001,4002,4003,4004)");
	 }
	 else
	 {*/

	sprintf(
			sql,
			"select a.MQ_ID, \
       				a.MQ_SIZE, \
       				a.MAX_MSG_NUM, \
       				a.MQ_BUFF_TYPE ,\
       				c.PRIORITY_TYPE \
  					from wf_mq_attr a, WF_PROCESS_MQ b, wf_process c  \
   				 where b.PROCESS_ID = %d  \
   				  and b.process_id=c.process_id \
   					and a.mq_id = b.mq_id",
			iProcID);

	qry.setSQL(sql);
	qry.open();
	while (qry.next() && /*qry.getFieldAsInteger(0)*/qry.field("MQ_ID").asInteger() > 0)
	{
		TReceiveInfo temp;
		temp.pmq = NULL;
		temp.pmq = new MessageQueue(/*qry.getFieldAsInteger(0)*/qry.field("MQ_ID").asInteger());
		temp.m_iBuffType = /*qry.getFieldAsInteger(3)*/qry.field("MQ_BUFF_TYPE").asInteger();
		m_iPriorityType = /*qry.getFieldAsInteger(4)*/qry.field("PRIORITY_TYPE").asInteger();
		if (!temp.pmq)
			throw(MBC_EventReceiver + 1);

		temp.pmq->open(true, true, qry.field("MQ_SIZE").asInteger(), qry.field("MAX_MSG_NUM").asInteger());
		VTReceiveInfo.push_back(temp);
	}
	qry.close();

	if (m_iPriorityType == 2) //1:发送带优先级 2:接收按优先级
	{
		memset(sql, 0, sizeof(sql));
		sprintf(
				sql,
				"select a.QueuePriority, \
									a.ServiceLevel, \
									a.NetLevel, \
									a.ServicePercent, \
									a.NetPercent, \
									a.MessageTotalNum \
								from sgw_sla_que_rel a \
									order by a.QueuePriority ");

		qry.setSQL(sql);
		qry.open();

		int i = 0;
		m_iPriorityLev = 0;
		m_iNetPrior = 0;
		while (qry.next())
		{
			int iServicePercent = qry.field("ServicePercent").asInteger();
			int iMessageTotalNum = qry.field("MessageTotalNum").asInteger();
			int iNetPercent = qry.field("NetPercent").asInteger();
			if (iServicePercent * iMessageTotalNum != 0)
			{
				if (iServicePercent * iMessageTotalNum % 100 > 50)
					m_iLevSum[m_iPriorityLev] = iServicePercent * iMessageTotalNum / 100 + 1;
				else
					m_iLevSum[m_iPriorityLev] = iServicePercent * iMessageTotalNum / 100;

				m_iNetSum[m_iNetPrior] = iNetPercent;
				m_iPriorityLev++;
				m_iNetPrior++;
			}
		}
		qry.close();
	}
	m_poInnerStruct = new InnerStruct();
}

//##ModelId=4313CB250194
EventReceiver::~EventReceiver()
{
	for (int i = 0; i < VTReceiveInfo.size(); i++)
	{
		VTReceiveInfo[i].pmq->close();
		delete (VTReceiveInfo[i].pmq);
		VTReceiveInfo[i].pmq = NULL;
	}
	VTReceiveInfo.clear();
	delete m_poInnerStruct;
}

int EventReceiver::receive(vector<ProtoToBusiData> &VTProtoToBusiData, long lMtype, bool bParse)
{
	int ret = 0, iSendcnt = 0, iNetCnt = 0, iNewNetCnt = 0, flag = 0;
	VTProtoToBusiData.clear();

	flag = Process::m_poCmdCom->readIniInteger("agreement", "congestion_flag", 0);
	if (flag / 2 == 1)
		return 0;

	for (int i = 0; i < m_iPriorityLev; i++)
	{
		iSendcnt = 0;
		for (int k = 1; k < m_iNetPrior + 1; k++)
		{
			iNetCnt = m_iLevSum[i] - iSendcnt;

			if (m_iLevSum[i] * m_iNetSum[k - 1] % 100 >= 50)
				iNewNetCnt = m_iLevSum[i] * m_iNetSum[k - 1] / 100 + 1;
			else
				iNewNetCnt = m_iLevSum[i] * m_iNetSum[k - 1] / 100;

			if (iNetCnt > iNewNetCnt)
				iNetCnt = iNewNetCnt;

			for (int l = 0; l < iNetCnt && iSendcnt <= m_iLevSum[i]; l++)
			{
				Process::isReceive = false;
				ret = VTReceiveInfo[0].pmq->receive(Process::m_sReceBuf, (i + 1) * 100 + k);
				Process::isReceive = true;
				if (ret > 0)
				{
					iSendcnt++;
					m_poInnerStruct->parserBuff(VTReceiveInfo[0].m_iBuffType);
					VTProtoToBusiData.push_back(Process::m_tProtoData);
					//						Process::m_pLogObj->sgwlog(0,Process::m_iProcID,Process::m_iSysPID,Process::m_iAppID,LOG_LEVEL_INFO,LOG_TYPE_BUSI,-1,"","收到消息[%s]:[%s]!",Process::m_tProtoData.m_strSessionID,Process::m_tProtoData.m_strServiceContextId);
				}
				else
					break;
			}
		}
	}
	return 0;
}

int EventReceiver::receive(long lMtype, bool bParse)
{
	int ret = 0;
	int fir = 0;

	//如果消息队列的信息在收到SIGTERM信号后不退出继续处理业务
	Process::isReceive = false;
///	ret = VTReceiveInfo[0].pmq->receive(Process::m_sReceBuf, lMtype);
	static char sTemp[32008];
	sTemp[0]=0;
	ret = VTReceiveInfo[0].pmq->receive (sTemp ,lMtype);
	if(ret>0)
		Process::m_sReceBuf.assign(sTemp, ret);
	//如果消息队列的信息在收到SIGTERM信号后不退出继续处理业务
	Process::isReceive = true;
	// 记录进程耗时
	int iLogProcess = Process::m_poCmdCom->readIniInteger("log", "log_process_time", 0);
	if (iLogProcess == 1)
	{
		Process::m_lRecvTime = get_curtime_ms();
	}
	if (ret > 0)
	{
		m_poInnerStruct->parserBuff(VTReceiveInfo[0].m_iBuffType);
#ifdef DEBUG_ONE
		cout << "++++++++++++++receive msg begin(pid = " << getpid() << ")+++++++++++++" << endl;
		Process::m_tProtoData.Display();
		cout << "++++++++++++++receive msg end (pid = " << getpid() << ")+++++++++++++" << endl;
		Process::m_pLogObj->sgwlog(0, Process::m_iProcID, Process::m_iSysPID, Process::m_iAppID, LOG_LEVEL_INFO,
				LOG_TYPE_BUSI, -1, "", "收到消息[%s]:[%s]!", Process::m_tProtoData.m_strSessionID,
				Process::m_tProtoData.m_strServiceContextId);
#endif
	}

	return ret;
}

int EventReceiver::receive(ProtoToBusiData & tProtoData, long lMtype, bool bParse)
{
	int ret = 0;
	int fir = 0;
	static string strRecvBuf;
	strRecvBuf.clear();
	//如果消息队列的信息在收到SIGTERM信号后不退出继续处理业务
	Process::isReceive = false;
	static char sTemp[32008];
	sTemp[0]=0;
	ret = VTReceiveInfo[0].pmq->receive (sTemp ,lMtype);
	if(ret>0)
		strRecvBuf.assign(sTemp, ret);
		
///	ret = VTReceiveInfo[0].pmq->receive(strRecvBuf, lMtype);
	//如果消息队列的信息在收到SIGTERM信号后不退出继续处理业务
	Process::isReceive = true;
	// 记录进程耗时
	int iLogProcess = Process::m_poCmdCom->readIniInteger("log", "log_process_time", 0);
	if (iLogProcess == 1)
	{
		Process::m_lRecvTime = get_curtime_ms();
	}

	if (ret > 0)
	{
		m_poInnerStruct->parserBuff(strRecvBuf, tProtoData, VTReceiveInfo[0].m_iBuffType);
#ifdef DEBUG_ONE
		cout << "++++++++++++++receive msg begin+++++++++++++" << endl;
		Process::m_tProtoData.Display();
		cout << "++++++++++++++receive msg end+++++++++++++" << endl;
		Process::m_pLogObj->sgwlog(0, Process::m_iProcID, Process::m_iSysPID, Process::m_iAppID, LOG_LEVEL_INFO,
				LOG_TYPE_BUSI, -1, "", "收到消息[%s]:[%s]!", Process::m_tProtoData.m_strSessionID,
				Process::m_tProtoData.m_strServiceContextId);
#endif
	}

	return ret;
}

int EventReceiver::send(long lMtype)
{
	int ret = VTReceiveInfo[0].pmq->send(Process::m_sReceBuf, lMtype);
	return ret;
}

void EventReceiver::setBlockFlag(bool ifblock)
{
	for (int i = 0; i < VTReceiveInfo.size(); i++)
	{
		VTReceiveInfo[i].pmq->setBlockFlag(ifblock);
	}
}

int EventReceiver::getMessageNumber()
{
	return VTReceiveInfo[0].pmq->getMessageNumber();
}
