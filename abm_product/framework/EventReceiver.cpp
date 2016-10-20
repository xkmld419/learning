/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "EventReceiver.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "WorkFlow.h"
#include "Process.h"
#include "MBC.h"
#include "CommonMacro.h"
//#include "DataIncrementMgr.h"

//##ModelId=41EF5A6C0117
int EventReceiver::receive(StdEvent *pEvent,  bool ifBlock)
{
	/*
    Process::CheckDoReload();
    
	int ret = 0;

	if (!m_poMQ) return 0;
	    
	//add by yks
	if (ifBlock != m_bIfBlock) {
	    m_bIfBlock = ifBlock;
	    m_poMQ->close ();
	    m_poMQ->open (m_bIfBlock, false, sizeof(StdEvent)*m_iMsgEventNum, m_iMaxNum);
	}

	ret = m_poMQ->receive ((char *)pEvent);

////added by zhangapin
//    while(1)
//        {
//        m_iNormalCnt++;
//        m_iIdleCnt++;         
//        if(m_iIdleCnt >300 || m_iNormalCnt>50000)
//            {////60秒或者正常接收5w条消息的时间间隔,扫描一次数据库
//            m_iIdleCnt=0;
//            m_iNormalCnt=0;
//            Environment::g_poDBMQ->refresh();
//            }
//        
//
//        if( Environment::g_poDBMQ->qos()==1 )
//            {/////速率控制表示要从数据库中取
//            ret = Environment::g_poDBMQ->receive( pEvent, Process::m_iProcID);
//            if(ret>0)     break;
//            else
//                {
//                ret = m_poMQ->receive ((char *)pEvent);
//                if(ret>0) break;
//                }
//            }
//        else
//            {/////速率控制表示要从系统消息队列中取
//            ret = m_poMQ->receive ((char *)pEvent);
//            if ( ret>0) break;
//            else
//                {/////速率控制表示要从系统消息队列中取,但是消息队列中没有消息,则从数据库中来取
//                ret=Environment::g_poDBMQ->receive( pEvent, Process::m_iProcID);
//                if(ret>0) break;
//                }
//            }
//        
//        if( ifBlock==false) 
//            break;
//        
//        usleep(200000);
//
//        
//        }

    
	if (ret > 0) {
		SaveLastEvent (pEvent);

		if (pEvent->m_iServID>0 && G_PUSERINFO) {
			G_PUSERINFO->getServ (G_SERV, pEvent->m_iServID, pEvent->m_sStartDate);
		}
		
		Process::m_iFromProcID=pEvent->m_iProcessID;
		TStatLogMgr::addInEventEX(pEvent->m_iFileID, pEvent, 1);
	}
 
	return ret;
	*/
	return 1;
}

int EventReceiver::receive(AcctItemTransData * itemData, bool ifBlock)
{
	
	int ret = 0;
/*
	if (!m_poMQ) return 0;

	if (ifBlock != m_bIfBlock) {
	    m_bIfBlock = ifBlock;
	    m_poMQ->close ();
	    m_poMQ->open (m_bIfBlock, false, sizeof(AcctItemTransData)*MAX_ITEM_RECORD, m_iMaxNum);
	}

	ret = m_poMQ->receive ((char *)itemData);
	if (ret > 0) {
		//这里帐单是否要考虑实时保存一下
		Process::m_iFromProcID=itemData[0].m_iProcessID;
		#ifdef __WF_STAT_DETAIL_LOG__
		// 只对原始事件做量的统计
		if (itemData[0].iEventSourceType==1)
		{
			TStatLogMgr::addInEventEX(itemData[0].iFileID,itemData[0].m_iEventTypeID,itemData[0].m_iProcessID,1);
		}
		#endif
	}
	*/
	return ret;
}


// add by zhangch
int EventReceiver::receive(char *buffer, bool ifBlock)
{
	int ret = 0;

	if (!m_poMQ) return 0;
	    
	if (ifBlock != m_bIfBlock) {
	    m_bIfBlock = ifBlock;
	    m_poMQ->close ();
	    m_poMQ->open (m_bIfBlock, false, sizeof(StdEvent)*m_iMsgEventNum, m_iMaxNum);
	}

	ret = m_poMQ->receive (buffer);

	return ret;

}

//##ModelId=41F6032A020C
EventReceiver::EventReceiver(int iProcID) 
{
	TOCIQuery qry (Environment::getDBConn ());
	char sql[2048];
	
	m_poMQ = 0;
    m_bIfBlock = true; // add by yks
    
	bool bTransAcctItem = false;//帐单传输
    
	/*sprintf (sql, "select a.process_id, b.mq_id mq_id, b.max_msg_num max_msg_num "
			"from wf_process_mq a, wf_mq_attr b "
			"where a.process_id=%d and a.mq_id = b.mq_id", iProcID);
*/
	// modified by zhangch
	sprintf (sql, "select a.process_id, b.mq_id mq_id, b.max_msg_num max_msg_num, nvl(b.msg_event_num, 1) msg_event_num "
			"from wf_process_mq a, wf_mq_attr b "
			"where a.process_id=%d and a.mq_id = b.mq_id", iProcID);
	qry.setSQL (sql);
	qry.open ();

	if (qry.next ()) {

		m_poMQ = new MessageQueue (qry.field(1).asInteger());
		if (!m_poMQ) THROW(MBC_EventReceiver+1);

		// add by zhangch
		m_iMsgEventNum = qry.field(3).asInteger();
		if (ACCT_ITEM_TRANS == m_iMsgEventNum){
			bTransAcctItem = true;
		}
		m_iMsgEventNum = m_iMsgEventNum>20 ? 20 : m_iMsgEventNum;

		//##消息队列的创建由调度引擎去操作
		m_iMaxNum = qry.field(2).asInteger();
		/*
		if (bTransAcctItem){
			m_poMQ->open (true, false, sizeof(AcctItemTransData)*MAX_ITEM_RECORD , m_iMaxNum);
		}else{
		m_poMQ->open (true, false, sizeof(StdEvent)*m_iMsgEventNum , m_iMaxNum);
		}
		*/
		m_poMQ->open (true, false, sizeof(StdEvent)*m_iMsgEventNum , m_iMaxNum);
		//m_poMQ->open (true, false, sizeof(StdEvent), m_iMaxNum);
	} 
    
	qry.close ();


    
}


//##ModelId=41F6032A0252
EventReceiver::~EventReceiver()
{
	if (m_poMQ) delete m_poMQ;

	m_poMQ = 0;
}



