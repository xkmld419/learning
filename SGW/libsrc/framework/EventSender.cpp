// Copyright (c) 2010
// All rights reserved.

#include "EventSender.h"
#include "MessageQueue.h"
#include "Process.h"
#include "Log.h"
#include "DateTime.h"

EventSender::EventSender(int iProcID)
{
	char sql[1024] = { 0 };
	//SQLITEQuery qry;
	DEFINE_QUERY(qry);
	sprintf(
			sql,
			"select DISTINCT a.mq_id mq_id, \
				  nvl(a.MAX_MSG_NUM, -1) max_msg_num, \
				  nvl(a.MQ_Size, -1) MQ_Size, \
				  nvl(a.MQ_BUFF_TYPE, -1) MQ_BUFF_TYPE, \
				  c.CONDITION_ID, \
				  d.PRIORITY_TYPE, \
				  c.SEND_MODE \
				  from wf_mq_attr a, WF_PROCESS_MQ b, wf_distribute_rule c , wf_process d  \
				  where d.process_id=%d \
				  and c.process_id=d.process_id \
				  and c.out_process_id=b.process_id  \
				  and b.mq_id=a.mq_id ",
			iProcID);

	qry.setSQL(sql);
	qry.open();
	while (qry.next())
	{
		TRouterRule temp;
		temp.iMqId = /*qry.getFieldAsInteger(0);*/qry.field("mq_id").asInteger();
		temp.iBuffType = /*qry.getFieldAsInteger(3);*/qry.field("MQ_BUFF_TYPE").asInteger();
		temp.iSendConditionID = /*qry.getFieldAsInteger(4);*/qry.field("CONDITION_ID").asInteger();
		m_iPriorityType = /*qry.getFieldAsInteger(5);*/qry.field("PRIORITY_TYPE").asInteger();
		temp.iSendMode = /*qry.getFieldAsInteger(6);*/qry.field("SEND_MODE").asInteger();
		temp.pmq = NULL;

		for (int k = 0; k < VTRouterRule.size(); k++)
		{
			if (VTRouterRule[k].iSendConditionID == temp.iSendConditionID && VTRouterRule[k].iMqId == temp.iMqId)
			{
				throw(MBC_EventSender + 3);
			}
		}

		temp.pmq = new MessageQueue(qry.field("mq_id").asInteger());
		if (!temp.pmq)
			THROW(MBC_EventSender+1);
		temp.pmq->open(temp.iSendMode, true, qry.field("MQ_Size").asInteger(), qry.field("max_msg_num").asInteger());
		VTRouterRule.push_back(temp);
	}
	qry.close();

	if (m_iPriorityType == 1) //1:发送带优先级 2:接收按优先级
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

		m_iMaxLev = 0;
		while (qry.next())
		{
			m_iLevSum[m_iMaxLev] = /*qry.getFieldAsInteger(1)*/qry.field("ServiceLevel").asInteger();
			m_iNetSum[m_iMaxLev] = /*qry.getFieldAsInteger(2)*/qry.field("NetLevel").asInteger();
			m_iMaxLev++;
		}
		qry.close();
	}
	m_poInnerStruct = new InnerStruct();
}

EventSender::~EventSender()
{
	for (int i = 0; i < VTRouterRule.size(); i++)
	{
		VTRouterRule[i].pmq->close();
		delete (VTRouterRule[i].pmq);
		VTRouterRule[i].pmq = NULL;
	}
	VTRouterRule.clear();
	delete m_poInnerStruct;
}

int EventSender::send(long lMtype)
{
	memset(&m_tCongestLevel, 0x00, sizeof(m_tCongestLevel));

	if (m_iPriorityType == 1)
	{
		int flag = Process::m_poCmdCom->readIniInteger("agreement", "congestion_flag", 0);
		if ((!SHMCongestLevel::GetShmCongestLevel(m_tCongestLevel) && m_tCongestLevel.m_iLevel != 0) || flag % 2 == 1)
		{
			Process::m_iRouteRule = 0;
			memset(&tmpServiceContextListData, 0x0, sizeof(ServiceContextListBaseData));
			strcpy(tmpServiceContextListData.m_strServiceContextId, Process::m_tProtoData.m_strServiceContextId);
			SHMServiceTxtListBase::getServiceTxtListBase(tmpServiceContextListData);
			if (tmpServiceContextListData.m_iPriorityLevel == 0)
				tmpServiceContextListData.m_iPriorityLevel = 1;
			Process::m_tProtoData.m_iCapPriority = tmpServiceContextListData.m_iPriorityLevel;

			StatAllInfoMgr::addBlockInfo(Process::m_tProtoData.m_strServiceContextId, Process::m_tProtoData.m_iCapPriority,
					1, 0, 0, NULL);

			for (int i = 1; i < m_iMaxLev; i++)
			{
				if (Process::m_tProtoData.m_iCapPriority > m_iLevSum[i - 1] && Process::m_tProtoData.m_iCapPriority
						<= m_iLevSum[i])
					//Process::m_iRouteRule = i;
					lMtype = i * 100;
			}

			if (strcmp(Process::m_tProtoData.m_strUserName, "") == 0)
			{
				lMtype += 1;
				Process::m_tProtoData.m_iNePriority = 1;
			}
			else
			{
				memset(&tmpUserInfoData, 0x0, sizeof(UserInfoData));
				memset(&tmpNetInfoData, 0x0, sizeof(NetInfoData));
				strcpy(tmpUserInfoData.m_strUserName, Process::m_tProtoData.m_strUserName);
				SHMUserInfoData::GetUserInfoData(tmpUserInfoData.m_strUserName, tmpUserInfoData);

				tmpNetInfoData.m_iNetinfoSeq = tmpUserInfoData.m_iNetinfoSeq;
				SHMNetInfoData::GetNetInfoData(tmpNetInfoData);

				if (tmpNetInfoData.m_iPriorityLevel == 0)
					tmpNetInfoData.m_iPriorityLevel = 1;

				Process::m_tProtoData.m_iNePriority = tmpNetInfoData.m_iPriorityLevel;
				for (int i = 1; i < m_iMaxLev; i++)
				{
					if (Process::m_tProtoData.m_iNePriority > m_iNetSum[i - 1] && Process::m_tProtoData.m_iNePriority
							<= m_iNetSum[i])
						lMtype += i;
				}
			}
		}
	}
	// 记录进程耗时
	int iLogProcess = Process::m_poCmdCom->readIniInteger("log", "log_process_time", 0);
	if (iLogProcess == 1)
	{
		static char szTmp[256] = {0};
		static char szSend[18] = {0};
		static char szRecv[18] = {0};
		if (Process::m_tProtoData.m_szTraceInfo[0] == '\0')
		{
			strcpy(Process::m_tProtoData.m_szTraceInfo,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
			strcat(Process::m_tProtoData.m_szTraceInfo,"<DATA>");
		}
		Process::m_lSendTime = get_curtime_ms();
		sprintf(szTmp,"<process intput=\"%s\" output=\"%s\" diff=%ld>%d</process>",get_longtime(Process::m_lRecvTime,szRecv),get_longtime(Process::m_lSendTime,szSend),Process::m_lRecvTime - Process::m_lSendTime,Process::m_iProcID);
		strcat(Process::m_tProtoData.m_szTraceInfo,szTmp);
	}
	for (int i = 0; i < VTRouterRule.size(); i++)
	{
		if (VTRouterRule[i].iSendConditionID == Process::m_iRouteRule)
		{
			m_poInnerStruct->packBuff(VTRouterRule[i].iBuffType);
			VTRouterRule[i].pmq->send(Process::m_sSendBuf, lMtype);
#ifdef DEBUG_ONE
			cout << "++++++++++++++send msg begin (pid = " << getpid() << ")+++++++++++++" << endl;
			Process::m_tProtoData.Display();
			cout << "lMtype:[" << lMtype << "]" << endl;
			cout << "++++++++++++++send msg end (pid = " << getpid() << ")+++++++++++++" << endl;
			Process::m_pLogObj->sgwlog(0, Process::m_iProcID, Process::m_iSysPID, Process::m_iAppID, LOG_LEVEL_INFO,
					LOG_TYPE_BUSI, -1, "", "发送消息[%s]:[%s]!", Process::m_tProtoData.m_strSessionID,
					Process::m_tProtoData.m_strServiceContextId);
#endif
		}
	}
	Process::m_lRecvTime = 0;
	Process::m_lSendTime = 0;
	return 0;
}

int EventSender::send(string &strSend, ProtoToBusiData & tProtoData, long lMtype)
{
	memset(&m_tCongestLevel, 0x00, sizeof(m_tCongestLevel));
	if (m_iPriorityType == 1)
	{
		int flag = Process::m_poCmdCom->readIniInteger("agreement", "congestion_flag", 0);
		if ((!SHMCongestLevel::GetShmCongestLevel(m_tCongestLevel) && m_tCongestLevel.m_iLevel != 0) || flag % 2 == 1)
		{
			Process::m_iRouteRule = 0;
			memset(&tmpServiceContextListData, 0x0, sizeof(ServiceContextListBaseData));
			strcpy(tmpServiceContextListData.m_strServiceContextId, tProtoData.m_strServiceContextId);
			SHMServiceTxtListBase::getServiceTxtListBase(tmpServiceContextListData);
			if (tmpServiceContextListData.m_iPriorityLevel == 0)
				tmpServiceContextListData.m_iPriorityLevel = 1;
			tProtoData.m_iCapPriority = tmpServiceContextListData.m_iPriorityLevel;

			StatAllInfoMgr::addBlockInfo(tProtoData.m_strServiceContextId, tProtoData.m_iCapPriority, 1, 0, 0, NULL);

			for (int i = 1; i < m_iMaxLev; i++)
			{
				if (tProtoData.m_iCapPriority > m_iLevSum[i - 1] && tProtoData.m_iCapPriority <= m_iLevSum[i])
					lMtype = i * 100;
			}

			if (strcmp(tProtoData.m_strUserName, "") == 0)
			{
				lMtype += 1;
				tProtoData.m_iNePriority = 1;
			}
			else
			{
				memset(&tmpUserInfoData, 0x0, sizeof(UserInfoData));
				memset(&tmpNetInfoData, 0x0, sizeof(NetInfoData));
				strcpy(tmpUserInfoData.m_strUserName, tProtoData.m_strUserName);
				SHMUserInfoData::GetUserInfoData(tmpUserInfoData.m_strUserName, tmpUserInfoData);

				tmpNetInfoData.m_iNetinfoSeq = tmpUserInfoData.m_iNetinfoSeq;
				SHMNetInfoData::GetNetInfoData(tmpNetInfoData);

				if (tmpNetInfoData.m_iPriorityLevel == 0)
					tmpNetInfoData.m_iPriorityLevel = 1;

				tProtoData.m_iNePriority = tmpNetInfoData.m_iPriorityLevel;
				for (int i = 1; i < m_iMaxLev; i++)
				{
					if (tProtoData.m_iNePriority > m_iNetSum[i - 1] && tProtoData.m_iNePriority <= m_iNetSum[i])
						lMtype += i;
				}
			}
		}
	}
	// 记录进程耗时
	int iLogProcess = Process::m_poCmdCom->readIniInteger("log", "log_process_time", 0);
	if (iLogProcess == 1)
	{
		static char szTmp[256] = {0};
		static char szSend[18] = {0};
		static char szRecv[18] = {0};
		if (tProtoData.m_szTraceInfo[0] == '\0')
		{
			strcpy(tProtoData.m_szTraceInfo,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
			strcat(tProtoData.m_szTraceInfo,"<DATA>");
		}
		Process::m_lSendTime = get_curtime_ms();
		sprintf(szTmp,"<process intput=\"%s\" output=\"%s\" diff=%ld>%d</process>",get_longtime(Process::m_lRecvTime,szRecv),get_longtime(Process::m_lSendTime,szSend),Process::m_lRecvTime - Process::m_lSendTime,Process::m_iProcID);
		strcat(tProtoData.m_szTraceInfo,szTmp);
	}

	for (int i = 0; i < VTRouterRule.size(); i++)
	{
		if (VTRouterRule[i].iSendConditionID == Process::m_iRouteRule)
		{
			m_poInnerStruct->packBuff(strSend, tProtoData, VTRouterRule[i].iBuffType);
			VTRouterRule[i].pmq->send(strSend, lMtype);
		}
	}
	Process::m_lRecvTime = 0;
	Process::m_lSendTime = 0;
	return 0;
}
