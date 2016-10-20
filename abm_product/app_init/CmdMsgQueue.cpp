#include "CmdMsgQueue.h"
#include "ReadIni.h"
//##ModelId=4BD1589C03E7
bool CmdMsgQueue::Init(unsigned int iSysMQID,bool bBlock)
{
	m_bBlock =bBlock;
	pQueue = new MessageQueueEx(iSysMQID);

        char sTemp[254];
        memset(sTemp, 0, sizeof(sTemp));
	
	pQueue->open(bBlock,true,MAX_CMD_MSG_LEN,MAX_CMD_MSG_NUM);
	m_iMQKey = iSysMQID;
	return true;
}



//##ModelId=4BD1589D0007
CmdMsgQueue::CmdMsgQueue()
{
	pQueue = 0;
}

//##ModelId=4BD1589D0005
CmdMsgQueue::~CmdMsgQueue()
{
	if(pQueue)
		delete pQueue;
	pQueue=0;
}



//##ModelId=4BF63906001E
bool CmdMsgQueue::ChangeQueueID(int iQueueID)
{
	if(0!=pQueue)
	{
		delete pQueue;
		pQueue=0;
	}
	return Init(iQueueID,m_bBlock);
}

