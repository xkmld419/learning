#include "CmdReceiver.h"
#include "CmdMsgQueue.h"
//##ModelId=4B8CDB250003
bool CmdReceiver::RecvCmd(CmdMsg* pCmdMsg,long lType)
{
	void* pData=0;
	if(!RecvData(&pData,lType))
		return false;

	pCmdMsg->AttachData(pData);
	//CmdMsg删除时负责释放
	//delete[] pData;
	return true;
	//free(pData);
}

//##ModelId=4B8CEA6B0114
bool CmdReceiver::RecvData(void** ppData,long lType)
{
//	cout<<"********************Recv*******************"<<endl;
	if(!this->pQueue)
		return false;
	
	int iSize=0;

	int iRetLen = pQueue->receive(m_pBuf,lType,m_iMqMaxSize);
	if(iRetLen<=0)
		return false;
	if(iRetLen >MAX_CMD_MSG_NUM*MAX_CMD_MSG_LEN)
	{
		//接受的数据超过消息队列的大小
		THROW (MBC_MessageQueue+1000);
	}
	iSize = *(int*)m_pBuf;
	void* pData=0;
	*ppData = malloc(sizeof(char)*iSize);
	pData = *ppData;
	//pData = (char*)m_pBuf+sizeof(int);
	memcpy(pData,((char*)m_pBuf+sizeof(long)),iSize);


#ifdef		_DEBUG_
	printf("***************Debug Info Begin************\n");
	void *pTmp = (void*)m_pBuf;
	for(int i=0;i<iSize;i++)
	{
		printf("%.2X",*(char*)pTmp);
		pTmp =(void*)((char*)pTmp+1);
		if((i!=0)&&!((i-7)%8))
		{
			printf("  :%X",pTmp);
			printf("\n");
		}
	}
	printf("\n");
	printf("***************Debug Info End************\n");
#endif
	//void* pData =&iSize;
	//int iRetLen = pQueue->receive((char*)(pData),sizeof(int));
	//if(iRetLen<=0)
	//	return false;
	//if(iRetLen >MAX_CMD_MSG_NUM*MAX_CMD_MSG_LEN)
	//{
	//	//接受的数据超过消息队列的大小
	//	THROW (MBC_MessageQueue+1000);
	//}
	//*ppData = new char[iSize];
	//pData = *ppData;
	////pData = new char[iSize];
	//iRetLen = pQueue->receive((char*)pData,iSize);

	//if(iRetLen<=0)
	//	return false;
	//if(iRetLen >MAX_CMD_MSG_NUM*MAX_CMD_MSG_LEN)
	//{
	//	//接受的数据超过消息队列的大小
	//	THROW (MBC_MessageQueue+1000);
	//}
	return true;
}

//##ModelId=4B8DC54A0263
CmdReceiver::CmdReceiver()
{
	m_iMqMaxSize = (MAX_CMD_MSG_LEN*MAX_CMD_MSG_NUM)-sizeof(long);
	m_pBuf = new char[m_iMqMaxSize];
	memset(m_pBuf,0,sizeof(m_iMqMaxSize));
}

//##ModelId=4B8DC55B00AB
CmdReceiver::~CmdReceiver()
{
	if(m_pBuf)
		delete[] m_pBuf;
	m_pBuf=0;
}

