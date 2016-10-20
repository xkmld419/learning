#include "CmdParserPoolMgr.h"

//##ModelId=4BF3CABD0020
CmdParserPoolMgr::CmdParserPoolMgr()
{
	m_iMixProcessCnt=0;
	m_iMaxProcessCnt=0;

}

//##ModelId=4BF3CAC5034E
CmdParserPoolMgr::~CmdParserPoolMgr()
{
}

//##ModelId=4BF3CF49003E
bool CmdParserPoolMgr::Init(void )
{
	
	this->setMaxProcessCnt(1);
	this->setMixProcessCnt(1);
	Log::log(0,"CmdParser进程池初始化成功");
	return true;
}

//CSemaphore* CmdParserPoolMgr::getPoolMaxSem(void)
//{
//
//	char sKey[MAX_SEM_KEY_LEN]="95279527";
//	CSemaphore* pSem = new CSemaphore();
//	CmdParserPoolMgr::getSemKey(sKey,MAX_SEM_KEY_LEN);
//	if(!pSem->getSem(sKey))
//	{
//		Log::log(0,"进程池没有初始化同步信号量");
//		delete pSem; pSem=0;
//	}
//	return pSem;
//
//	////替换成数据库去IPC_KEY
//	//if((0!= pBuf)&&iSize>0)
//	//{
//	//	strncpy(pBuf,"95279527",strlen("95279527")>iSize?iSize:strlen("95279527"));
//	//	pBuf[strlen("95279527")>iSize?iSize:strlen("95279527")]=0;
//	//}
//	return true;
//}
//##ModelId=4BF3CFA9032C
void CmdParserPoolMgr::addTask(CmdMsg* pMsg)
{
	if(0!= pMsg)
	{
		if(!m_pMsgSender->SendCmd(pMsg,(long)1))
		{
			//发送消息失败检查最大进程数和最小进程数

		}
	}
}


void CmdParserPoolMgr::addDefaultMsg(CmdMsg* pMsg)
{
	m_vctDefaultMsg.push_back(pMsg);
}
//##ModelId=4BF3D045033F
void CmdParserPoolMgr::addProcess()
{
	return;
	pid_t iPid ;
	if((iPid= fork()==0))
	{
		//Child Process
		CmdParserProc* pCmdParserProc = new CmdParserProc();
		Log::log(0,"子进程启动 CmdParserProc %d",getpid());
		sleep(3);	
		if(!pCmdParserProc->Init(m_pMsgRecverCmdParser,m_pMsgSenderCmdParser))
		{
			Log::log(0,"初始化子进程失败");
		}else
		{
			variant vtRet;
			int iRetSize;
			long lMsgID=MSG_OP_RETURN_OK;
			for(vector<CmdMsg*>::iterator it =m_vctDefaultMsg.begin();
				it!= m_vctDefaultMsg.end(); it++)
			{
				pCmdParserProc->execMsgCmd(*it,lMsgID,iRetSize,vtRet);
			}
			pCmdParserProc->start();		
		}
		delete pCmdParserProc; pCmdParserProc=0;
		Log::log(0,"子进程退出 CmdParserProc %d",iPid);
		exit(0);
	}else
	{
		m_vectorPid.push_back(iPid);
	}	
}

//##ModelId=4BF3D0550312
bool CmdParserPoolMgr::subProcess()
{
	if(this->m_pMsgSender !=0)
	{
		string strTmp;
		CmdMsg* pMsg = CommandCom::CreateNewMsg(
			MSG_PROCESS_STOP_SIG,strTmp);
		m_pMsgSender->SendCmd(pMsg,(long)1);
		delete pMsg;
	}
	return true;
}


//##ModelId=4BF3D3D10290
unsigned int const& CmdParserPoolMgr::getMaxProcessCnt() const
{
    return m_iMaxProcessCnt;
}


//##ModelId=4BF3D3D102ED
void CmdParserPoolMgr::setMaxProcessCnt(unsigned int left)
{
	if(m_iMaxProcessCnt<left)
	{
		m_iMaxProcessCnt = left;
		return ;
	}
	for(unsigned int i=left;i<m_iMaxProcessCnt-left;i++)
		this->subProcess();
}


//##ModelId=4BF3D3D103C2
unsigned int const& CmdParserPoolMgr::getMixProcessCnt() const
{
    return m_iMixProcessCnt;
}


//##ModelId=4BF3D3D2001A
void CmdParserPoolMgr::setMixProcessCnt(unsigned int left)
{
	if(m_iMixProcessCnt>left)
	{
		m_iMixProcessCnt = left;
		return ;
	}
	for(unsigned int i=0;i<left-m_iMixProcessCnt;i++)
	{
		sleep(1);
		addProcess();
	}
}


//##ModelId=4BF5F26A0322
void CmdParserPoolMgr::setMsgSender(CmdSender* pMsgSender)
{
	m_pMsgSender = pMsgSender;
}
void CmdParserPoolMgr::setMsgSenderCmdParser(CmdSender *pMsgSender)
{
	m_pMsgSenderCmdParser = pMsgSender;
}
//##ModelId=4BF5F2A801D2
void CmdParserPoolMgr::setMsgRecverCmdParser(CmdReceiver* pMsgRecver)
{
	m_pMsgRecverCmdParser = pMsgRecver;
	CmdMsg* pMsg=0;
	while(1)	
	{
		pMsg= CommandCom::CreateNullMsg();
		if(m_pMsgRecverCmdParser->RecvCmd(pMsg)&&(pMsg!=0))
		{
			delete pMsg;
			pMsg=0;
		}else
			break;
	}
}


