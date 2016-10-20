#include "CmdSender.h"

//##ModelId=4B8CDFC8032B
bool CmdSender::SendCmd(CmdMsg* pCmdMsg,long lType,bool bSendLock)
{
	if(!pCmdMsg)
		pCmdMsg->SetSessionID(getuid());
	int iSize=0;
	void* pData = pCmdMsg->DetachSendData(iSize);
	if(0==pData)
		return false;
    bool bRet=false;
	*(int*)pData=iSize;
        bRet = SendData(pData,iSize,lType);

#ifdef		_DEBUG_
	printf("***************Debug Info Begin************\n");
	for(int i=0;i<iSize;i++)
	{
		printf("%.2X",*(char*)pData);
		pData =(void*)((char*)pData+1);
		if((i!=0)&&!((i-7)%8)){
			printf("  :%X",pData);
			printf("\n");
		}
	}
	printf("\n");
	printf("***************Debug Info End************\n");
#endif
	free(pData);
    return bRet;
}

//##ModelId=4B8CEA780102
bool CmdSender::SendData(void* pData, unsigned int iSize,long lType)
{
	//cout<<"********************Send*******************"<<endl;
	if(pQueue)
	{
		/*int iLen =pQueue->send((char*)(&iSize),sizeof(int));
		if(iLen!= sizeof(int))
			THROW (MBC_MessageQueue+1000);*/
		int iLen = pQueue->send(pData,lType,iSize);
		if(iLen!=iSize)
			THROW (MBC_MessageQueue+1000);
		return iLen==iSize?true:false;
	}
	return false;
}

//##ModelId=4B8DC56A0186
CmdSender::CmdSender()
{
	m_pLock=0;
}

//##ModelId=4B8DC57103E6
CmdSender::~CmdSender()
{
}

